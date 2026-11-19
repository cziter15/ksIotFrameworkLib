/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <memory>
#include <string>

#ifdef ESP32
	#include <esp_arduino_version.h>
#endif

#if defined(ESP32) && ESP_ARDUINO_VERSION_MAJOR >= 3
	class NetworkClient;
	namespace ksf::misc { using ksNetworkClient_t = NetworkClient; }
#elif defined(ESP32) || defined(ESP8266)
	class WiFiClient;
	namespace ksf::misc { using ksNetworkClient_t = WiFiClient; }
#else
	#error Platform not implemented.
#endif

namespace ksf::misc
{
	/*!
		@brief Creates a TLS client pinned to a certificate fingerprint.

		The client validates the pin before allowing application data to be sent.
		Certificate-chain, hostname and validity checks are not required in this mode.
		@param fingerprint Certificate fingerprint in hex (SHA-256 on ESP32, SHA-1 on ESP8266).
		@return Configured client, or nullptr if the fingerprint is invalid.
	*/
	std::unique_ptr<ksNetworkClient_t> createPinnedTlsClient(const std::string& fingerprint);
}
