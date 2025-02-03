/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksDomainQuery.h"
#include "ksConstants.h"
#include <WiFiUdp.h>

namespace ksf 
{
	ksDomainQuery::ksDomainQuery(IPAddress dnsServer)
		: serverIP(dnsServer)
	{
		udp = std::make_unique<ksUdpClient_t>();
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
	
	bool ksDomainQuery::getResolvedIP(IPAddress& ip)
	{
		if (resolvedIP.operator uint32_t() != 0)
		{
			ip = resolvedIP;
			return true;
		}
		return false;
	}
	
	void ksDomainQuery::sendQuery()
	{
		transactionID++;

		if (!udp->beginPacket(serverIP, 53)) 
			return; // Prevent sending if packet fails to begin

		udp->write(transactionID >> 8);
		udp->write(transactionID & 0xFF);
		udp->write(0x01); udp->write(0x00); // Standard query
		udp->write(0x00); udp->write(0x01); // One question
		udp->write(0x00); udp->write(0x00); // No answer
		udp->write(0x00); udp->write(0x00); // No authority
		udp->write(0x00); udp->write(0x00); // No additional

		// Encode domain
		std::vector<uint8_t> domainBuffer;
		size_t labelStart = 0;
		domainBuffer.push_back(0);
		for (char c : domain) 
		{
			if (c == '.') 
			{
				if (labelStart >= domainBuffer.size()) 
					return; // Prevent buffer overrun
				domainBuffer[labelStart] = domainBuffer.size() - labelStart - 1;
				labelStart = domainBuffer.size();
				domainBuffer.push_back(0); // Placeholder for next label length
			} 
			else domainBuffer.push_back(c);
		}

		if (labelStart >= domainBuffer.size()) 
			return;

		domainBuffer[labelStart] = domainBuffer.size() - labelStart - 1; // Final label length
		domainBuffer.push_back(0x00); // Null terminator

		// Send precomputed domain buffer
		udp->write(domainBuffer.data(), domainBuffer.size());

		// Type A (IPv4) and Class IN
		udp->write(0x00); udp->write(0x01);
		udp->write(0x00); udp->write(0x01);

		udp->endPacket();
	}

	void ksDomainQuery::receiveResponse()
	{
		/* Return if no packet is available. */
		if (udp->parsePacket() == 0) 
			return;

		/* Read UDP packet. */
		uint8_t buffer[512];
		size_t len = udp->read(buffer, sizeof(buffer));
		if (len < 12) 
			return;

		/* Check transaction ID. */
		if (auto responseID{(buffer[0] << 8) | buffer[1]}; responseID != transactionID) 
			return;

		/* Check response code. */
		size_t anCount{(static_cast<size_t>(buffer[6] << 8) | buffer[7])};
		if (anCount == 0) 
			return;

		/* Skip query section */
		size_t pos{12};
		while (pos < len && buffer[pos] != 0x00) 
			pos++;
		pos += 5; // Skip null terminator, QTYPE (2 bytes), and QCLASS (2 bytes)

		/* Parse answers */
		for (size_t i {0}; i < anCount; i++) 
		{
			while (pos < len && buffer[pos] >= 192) 
				pos += 2;

			if (pos + 10 > len) 
				return;

			auto type{(buffer[pos] << 8) | buffer[pos + 1]};
			auto dataLength{(buffer[pos + 8] << 8) | buffer[pos + 9]};
			pos += 10;

			if (type == 1 && dataLength == 4 && pos + 4 <= len) 
			{
				resolvedIP = IPAddress(buffer[pos], buffer[pos + 1], buffer[pos + 2], buffer[pos + 3]);
				return;
			}

			pos += dataLength;
		}
	}

	void ksDomainQuery::process()
	{
		/* Return if the domain is empty or if we have a valid IP. */
		if (domain.empty())
			return;

		/* If we have a valid IP, return immediately, we're done. */
		if (resolvedIP.operator uint32_t() != 0)
			return;

		/* Check if it's time to send a query. */
		if (lastQuerySendTimeMs == 0 || millis() - lastQuerySendTimeMs > KSF_DOMAIN_QUERY_INTERVAL_MS)
		{
			/* If there's a valid IP, return immediately, we're done. */
			if (resolvedIP.fromString(domain.c_str()))
				return;

			/* Send the DNS query. */
			sendQuery();

			/* Update the last query send time. */
			lastQuerySendTimeMs = millis();
		}

		/* Check if we have a response. */
		receiveResponse();
	}
}
