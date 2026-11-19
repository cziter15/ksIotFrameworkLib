/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>
#include <cstdint>

#if defined(ESP32)
	#include <esp_arduino_version.h>
	#include <WiFiClientSecure.h>
	#if ESP_ARDUINO_VERSION_MAJOR >= 3
		#define ksCertUtilsBaseClientSecure_t NetworkClientSecure
	#else
		#define ksCertUtilsBaseClientSecure_t WiFiClientSecure
	#endif
#elif defined(ESP8266)
	#include <WiFiClientSecure.h>
	#define ksCertUtilsBaseClientSecure_t BearSSL::WiFiClientSecure
#else
	#error Platform not implemented.
#endif

namespace ksf::misc
{
	/*!
		@brief Secure client pinned to a single certificate fingerprint.

		Allows validation of certificate fingerprint without certificate-chain verification.
		The platform-specific details are hidden inside, so callers never need platform ifdefs.
		A connection is considered established only if the fingerprint matches.
	*/
	class ksPinnedTlsClient : public ksCertUtilsBaseClientSecure_t
	{
		private:
			static constexpr size_t fingerprintSize{
#ifdef ESP32
				32	// SHA-256
#else
				20	// SHA-1
#endif
			};

			uint8_t fingerprintBytes[fingerprintSize]{};	//!< Fingerprint transformed into array of bytes.
#ifdef ESP32
			bool configured{false};							//!< True if fingerprint has been set.
			bool verified{false};							//!< True if current connection passed verification.
#if ESP_ARDUINO_VERSION_MAJOR >= 4
			static int verifyCertificate(void* context, mbedtls_x509_crt* cert, int depth, uint32_t* flags);
#endif
#endif

		public:
			/*!
				@brief Sets the pinned fingerprint and configures the client for fingerprint-only validation.
				@param fingerprint Fingerprint string (hex).
				@return True if fingerprint is valid and setup passed, otherwise false.
			*/
			bool setPinnedFingerprint(const std::string& fingerprint);

#ifdef ESP32
			int connect(IPAddress ip, uint16_t port, int32_t timeout) override;
			int connect(IPAddress ip, uint16_t port) override;
			int connect(const char* host, uint16_t port, int32_t timeout) override;
			int connect(const char* host, uint16_t port) override;
			void stop() override;
			size_t write(const uint8_t* data, size_t size) override;
			size_t write(uint8_t data) override;
#endif
	};
}
