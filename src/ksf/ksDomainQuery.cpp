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

		udp->beginPacket(serverIP, 53);
		udp->write(transactionID >> 8);
		udp->write(transactionID & 0xFF);
		udp->write(0x01); udp->write(0x00); // Standard query
		udp->write(0x00); udp->write(0x01); // One question
		udp->write(0x00); udp->write(0x00); // No answer
		udp->write(0x00); udp->write(0x00); // No authority
		udp->write(0x00); udp->write(0x00); // No additional

		// Encode domain beforehand
		size_t labelStart{0};
		std::vector<uint8_t> domainBuffer;
		domainBuffer.push_back(0);
		for (char c : domain) 
		{
			if (c == '.') 
			{
				domainBuffer[labelStart] = domainBuffer.size() - labelStart - 1;  // Set label length
				labelStart = domainBuffer.size();
				domainBuffer.push_back(0); // Placeholder for next label length
			} 
			else domainBuffer.push_back(c);
		}
		domainBuffer[labelStart] = domainBuffer.size() - labelStart - 1; // Final label length
		domainBuffer.push_back(0x00); // Null terminator

		// Send precomputed domain buffer
		udp->write(domainBuffer.data(), domainBuffer.size());

		// Type A (IPv4) and Class IN
		udp->write(0x00); 
		udp->write(0x01);
		udp->write(0x00); 
		udp->write(0x01);

		// Send query.
		udp->endPacket();
	}

	void ksDomainQuery::receiveResponse()
	{
		/* Check if we have a response. */
		if (udp->parsePacket() == 0) 
			return;

		/* Read the response. */
		uint8_t buffer[512];
		size_t len{static_cast<size_t>(udp->read(buffer, sizeof(buffer)))};
		if (len < 12) 
			return;

		/* Check the response ID. */
		auto responseID{(buffer[0] << 8) | buffer[1]};
		if (responseID != transactionID) 
			return;

		/* Check if the response contains at least one answer. */
		auto anCount{(buffer[6] << 8) | buffer[7]};
		if (anCount == 0) 
			return;

		/* Parse the header. */
		size_t pos{12};
		while (pos < len && buffer[pos] != 0x00) 
			pos++;  // Skip domain name
		pos += 5;  // Skip null terminator, QTYPE (2 bytes), and QCLASS (2 bytes)

		/* Parse the answers. */
		for (ssize_t i{0}; i < anCount; i++) 
		{
			while (pos < len && buffer[pos] >= 192) 
				pos += 2; // Skip name (compressed)

			/* Ensure we have enough data for the answer header. */
			if (pos + 10 > len) 
				return;

			/* Parse the answer header. */
			auto type{(buffer[pos + 0] << 8) | buffer[pos + 1]};
			auto dataLength{(buffer[pos + 8] << 8) | buffer[pos + 9]};
			pos += 10;

			/* Check if this is an A record. */
			if (type == 1 && dataLength == 4 && pos + 4 <= len) 
			{
				/* Resolve the IP address. */
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
		if (lastQuerySendTimeMs == 0 || millis() - lastQuerySendTimeMs > queryIntervalMs)
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