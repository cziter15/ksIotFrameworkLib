/*
 *	Copyright (c) 2025 Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */
#include <cstddef>
#include <WiFiUdp.h>

#include "ksDomainQuery.h"
#include "../ksConstants.h"


namespace ksf::misc
{
	constexpr std::size_t MAX_DNS_UDP_PACKET_SIZE = 512;

	uint16_t readUint16(const uint8_t* buffer, std::size_t pos) 
	{
		return static_cast<uint16_t>((buffer[pos] << 8) | buffer[pos + 1]);
	}

	void writeUint16(const std::unique_ptr<ksUdpClient_t>& udp, uint16_t value) 
	{
		udp->write(static_cast<uint8_t>(value >> 8));
		udp->write(static_cast<uint8_t>(value & 0xFF));
	}

	ksDomainQuery::ksDomainQuery() 
		: ksDomainQuery(KSF_DOMAIN_QUERY_DNS_SERVER) 
	{}

	ksDomainQuery::ksDomainQuery(IPAddress dnsServer)
		: serverIP(dnsServer), udp(std::make_unique<ksUdpClient_t>())
	{
		udp->begin(0);
	}

	ksDomainQuery::~ksDomainQuery() = default;

	void ksDomainQuery::invalidate()
	{
		resolvedIP = IPAddress(0, 0, 0, 0);
		lastQuerySendTimeMs = 0;
	}

	void ksDomainQuery::setDomain(std::string domain)
	{
		this->domain = std::move(domain);
		invalidate();
	}
	
	bool ksDomainQuery::getResolvedIP(IPAddress& ip) const
	{
		/* If we have a valid IP, return it. */
		if (resolvedIP.operator uint32_t() != 0)
		{
			/* Copy the IP address to the output parameter. */
			ip = resolvedIP;
			return true;
		}

		/* In case of failure, return false. */
		return false;
	}
	
	void ksDomainQuery::sendQuery()
	{
		/* Begin a packet. */
		if (!udp->beginPacket(serverIP, 53)) 
			return;

		/* Increment transaction ID and write it. */
		++transactionID;
		writeUint16(udp, transactionID);
		
		/* Write flags and number of questions. */
		writeUint16(udp, 0x0100); // Standard query
		writeUint16(udp, 0x0001); // One question
		writeUint16(udp, 0x0000); // No answer
		writeUint16(udp, 0x0000); // No authority
		writeUint16(udp, 0x0000); // No additional

		/* Process domain name. */
		for (std::size_t start{0}, pos = 0; ; start = pos + 1) 
		{
			pos = domain.find('.', start);
			if (pos == std::string::npos)
				pos = domain.size();

			/* Check label length. */
			auto labelLength{static_cast<uint8_t>(pos - start)};
			if (labelLength == 0 || labelLength > 63)
				return;

			/* Write label length and label */
			udp->write(labelLength);
			udp->write(reinterpret_cast<const uint8_t*>(domain.data() + start), labelLength);

			if (pos == domain.size())
				break; 
		}

		/* Null terminator. */
		udp->write(uint8_t{0x00});

		/* Write query type and class. */
		writeUint16(udp, 0x0001); // Type A
		writeUint16(udp, 0x0001); // Class IN

		/* Send the query to the DNS server. */
		udp->endPacket();
	}

	void ksDomainQuery::receiveResponse()
	{
		/* Return if no UDP packet is available. */
		if (udp->parsePacket() == 0)
			return;

		/* Read the UDP packet. */
		uint8_t buffer[MAX_DNS_UDP_PACKET_SIZE];
		auto len{static_cast<std::size_t>(udp->read(buffer, MAX_DNS_UDP_PACKET_SIZE))};
		if (len < 12 || len > MAX_DNS_UDP_PACKET_SIZE)
			return;

		/* Check that the transaction ID matches. */
		if (readUint16(buffer, 0) != transactionID)
			return;

		/* Check number of answers. */
		auto answerCount{readUint16(buffer, 6)};
		if (answerCount == 0)
			return;

		/* 
			Skip over the header and query section.
			Start after the header (12 bytes), then skip the QNAME (until a null byte),
			and finally skip the null terminator, QTYPE (2 bytes), and QCLASS (2 bytes).
		*/
		std::size_t pos{12};
		while (pos < len && buffer[pos] != 0)
			pos++;
		pos += 5;

		/* Process the answers. */
		for (std::size_t i{0}; i < answerCount; i++)
		{
			/* 
				Skip name pointers if present. 
				In DNS, a pointer is indicated when the top two bits are set (0xC0).
			*/
			while (pos < len && (buffer[pos] & 0xC0) == 0xC0)
				pos += 2;

			/*
				Ensure that there's enough room for the variable part of the answer.
				Fixed fields: TYPE (2), CLASS (2), TTL (4), RDLENGTH (2), RDATA (variable)
			*/
			if (pos + 10 > len)
				return;

			/* Read TYPE and RDLENGTH from the answer. */
			auto type{readUint16(buffer, pos)};
			auto dataLength{readUint16(buffer, pos + 8)};
			pos += 10;

			/* If it's an A record (TYPE 1) and data length is 4 bytes (IPv4), read the IP address. */
			if (type == 1 && dataLength == 4 && pos + 4 <= len)
			{
				/* Read the IP octects and construct the IP address. */
				resolvedIP = IPAddress(buffer[pos], buffer[pos + 1], buffer[pos + 2], buffer[pos + 3]);
				return;
			}

			/* Skip over the resource data if this answer is not the one we want. */
			pos += dataLength;
		}
	}

	void ksDomainQuery::process()
	{
		/* If we have a valid IP, return immediately, we're done. */
		if (resolvedIP.operator uint32_t() != 0)
			return;

		/* Return if the domain is empty or if we have a valid IP. */
		if (domain.empty())
			return;

		/* Check if it's time to send a query. */
		if (lastQuerySendTimeMs == 0 || millis() - lastQuerySendTimeMs > KSF_DOMAIN_QUERY_INTERVAL_MS)
		{
			/* If there's a valid IP, return immediately, we're done. */
			if (resolvedIP.fromString(domain.c_str()))
				return;

			/* Send the DNS query and update the last query send time. */
			sendQuery();
			lastQuerySendTimeMs = millis();
		}

		/* Check if we have a response. */
		receiveResponse();
	}
}
