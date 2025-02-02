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
#include <stdint.h>
#include <memory>

#include "ksApplication.h"

#if (defined(ESP32) && ESP_ARDUINO_VERSION_MAJOR >= 3)
	#define ksUdpClient_t NetworkUDP
#else
	#define ksUdpClient_t WiFiUDP
#endif

class ksUdpClient_t;

namespace ksf 
{
	class ksDomainQuery
	{
		protected:
			/* UDP client. */
			std::unique_ptr<ksUdpClient_t> udp;

			/* DNS server IP address. */
			IPAddress serverIP;

			/* Domain to resolve IP address for. */
			std::string domain;
			/* Resolved IP address. */
			IPAddress resolvedIP;

			/* Transaction ID (last). */
			uint16_t transactionID{0};
			/* Last query send time. */
			uint32_t lastQuerySendTimeMs{0};
			/* Query interval in milliseconds. */
			uint32_t queryIntervalMs{3000};

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
				@brief Constructs a new ksDomainQuery object.
				@param dnsServer IP address of the DNS server to use for resolving domain names. By default, 8.8.8.8 is used.
			*/
			ksDomainQuery(IPAddress dnsServer = IPAddress(8, 8, 8, 8));

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
			bool getResolvedIP(IPAddress& ip);

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