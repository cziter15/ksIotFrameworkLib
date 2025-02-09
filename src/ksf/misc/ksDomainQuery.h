/*
 *	Copyright (c) 2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <Arduino.h>
#include <IPAddress.h>
#include <stdint.h>
#include <memory>

#if (defined(ESP32) && ESP_ARDUINO_VERSION_MAJOR >= 3)
	#define ksUdpClient_t NetworkUDP
#else
	#define ksUdpClient_t WiFiUDP
#endif

class ksUdpClient_t;

namespace ksf::misc 
{
	/*! 
		@brief Class for resolving domain names to IP addresses using DNS queries.

		To avoid DNS caching, a new query is sent every KSF_DOMAIN_QUERY_INTERVAL_MS milliseconds.
		The motivation to write this component was to avoid issues that I had with standard DNS libraries.
	*/
	class ksDomainQuery
	{
		protected:
			/* DNS server IP address. */
			IPAddress serverIP;
			/* UDP client. */
			std::unique_ptr<ksUdpClient_t> udp;

			/* Domain to resolve IP address for. */
			std::string domain;
			/* Resolved IP address. */
			IPAddress resolvedIP;
			/* Transaction ID (last). */
			uint16_t transactionID{0};
			/* Last query send time. */
			uint32_t lastQuerySendTimeMs{0};

			/*!
				@brief Sends DNS query to the DNS server.
			*/
			void sendQuery();

			/*!
				@brief Receives DNS response from the DNS server.
			*/
			void receiveResponse();

		public:
			/*!
				@brief Default constructor.
				Uses KSF_DOMAIN_QUERY_DNS_SERVER as the DNS server.
			*/
			ksDomainQuery();

			/*!
				@brief Constructs a new ksDomainQuery object.
				@param dnsServer IP address of the DNS server to use for resolving domain names. By default, KSF_DOMAIN_QUERY_DNS_SERVER is used.
			*/
			ksDomainQuery(IPAddress dnsServer);
			/*!
				@brief Invalidates the resolved IP address and the last query send time. It will cause a new query to be sent.
			*/
			void invalidate();

			/*!
				@brief Sets the domain to resolve IP address for.
				@param domain Domain to resolve IP address for.
			*/
			void setDomain(std::string domain);

			/*!
				@brief Retrieves the resolved IP address for the domain.
				@param ip IP address to store the resolved IP address in.
				@return True if the resolved IP address was successfully retrieved, false otherwise.
			*/
			bool getResolvedIP(IPAddress& ip) const;

			/*!
				@brief Handles resolver tasks, such as sending queries and receiving responses.
			*/
			void process();
			
			/*! 
				@brief Destructor.
			*/
			virtual ~ksDomainQuery();
	};
}