/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include <WiFiClientSecure.h>

#include "../ksConstants.h"

#include "ksPinnedTlsClient.h"

#if defined(ESP32) && ESP_ARDUINO_VERSION_MAJOR >= 4
	#include <mbedtls/version.h>
	#include <mbedtls/x509.h>
	#if MBEDTLS_VERSION_MAJOR >= 4
		#include <psa/crypto.h>
	#else
		#include <mbedtls/sha256.h>
	#endif
#endif

namespace
{
	class ksPinnedTlsClient final : public WiFiClientSecure
	{
		private:
#ifdef ESP32
			uint8_t fingerprintBytes[32]{};				//!< SHA-256 fingerprint transformed into array of bytes.
			struct
			{
				bool configured : 1;					//!< True if fingerprint has been set.
				bool verified : 1;						//!< True if current connection passed verification.
			} 
			bitflags = {false, false};

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

	static bool fingerprintToBytes(const std::string& fingerprint, uint8_t* bytes, uint8_t bytesLen)
	{
		if (fingerprint.size() != bytesLen * 2)
			return false;

		auto hexToValue = [](char c) -> uint8_t {
			if (c>='0' && c <='9') 
				return c - '0';
			if (c>='A' && c<='F') 
				return 10 + c - 'A';
			if (c>='a' && c<='f') 
				return 10 + c - 'a';
			return 255;
		};

		for (uint8_t bi{0}; bi < bytesLen; ++bi) 
		{
			uint8_t high{hexToValue(fingerprint[bi * 2])};
			uint8_t low{hexToValue(fingerprint[bi * 2 + 1])};
			if ((high | low) > 15)
				return false;
			bytes[bi] = (high << 4) | low;
		}

		return true;
	}

	bool ksPinnedTlsClient::setPinnedFingerprint(const std::string& fingerprint)
	{
#ifdef ESP32
		stop();
		bitflags.configured = false;
#else
		uint8_t fingerprintBytes[20]{};
#endif
		if (!fingerprintToBytes(fingerprint, fingerprintBytes, sizeof(fingerprintBytes)))
			return false;

#ifdef ESP32
		constexpr auto handshakeTimeoutSec{KSF_MQTT_TIMEOUT_MS / KSF_ONE_SEC_MS};
		setHandshakeTimeout(handshakeTimeoutSec);
		setInsecure();
		bitflags.configured = true;
#else
		/* BearSSL validates the fingerprint under the hood during connect. */
		setFingerprint(fingerprintBytes);
#endif
		return true;
	}

#ifdef ESP32
	int ksPinnedTlsClient::connect(IPAddress ip, uint16_t port, int32_t timeout)
	{
		stop();
		if (!bitflags.configured)
			return 0;

		_timeout = timeout;
#if ESP_ARDUINO_VERSION_MAJOR >= 4
		setPlainStart();
		if (!WiFiClientSecure::connect(ip, port, nullptr, nullptr, nullptr, nullptr))
			return 0;

		// No CA is configured: the callback and post-handshake check enforce the pin.
		mbedtls_ssl_conf_authmode(&sslclient->ssl_conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
		mbedtls_ssl_set_verify(&sslclient->ssl_ctx, verifyCertificate, this);
		if (!startTLS() || !bitflags.verified || mbedtls_ssl_get_verify_result(&sslclient->ssl_ctx) != 0)
		{
			stop();
			return 0;
		}
#else
		if (!WiFiClientSecure::connect(ip, port, timeout))
			return 0;

		static constexpr char hexChars[]{"0123456789ABCDEF"};
		char hexBuffer[sizeof(fingerprintBytes) * 2 + 1];
		char* ptr{hexBuffer};
		for (const auto byte : fingerprintBytes)
		{
			*ptr++ = hexChars[byte >> 4];
			*ptr++ = hexChars[byte & 0x0F];
		}
		*ptr = '\0';
		if (!WiFiClientSecure::verify(hexBuffer, nullptr))
		{
			stop();
			return 0;
		}
		bitflags.verified = true;
#endif
		return 1;
	}

	int ksPinnedTlsClient::connect(IPAddress ip, uint16_t port)
	{
		return connect(ip, port, _timeout);
	}

	int ksPinnedTlsClient::connect(const char*, uint16_t, int32_t)
	{
		stop();
		return 0;
	}

	int ksPinnedTlsClient::connect(const char* host, uint16_t port)
	{
		return connect(host, port, _timeout);
	}

	void ksPinnedTlsClient::stop()
	{
		bitflags.verified = false;
		WiFiClientSecure::stop();
	}

	size_t ksPinnedTlsClient::write(const uint8_t* data, size_t size)
	{
		if (!bitflags.verified)
			return 0;
		return WiFiClientSecure::write(data, size);
	}

	size_t ksPinnedTlsClient::write(uint8_t data)
	{
		return write(&data, 1);
	}

#if ESP_ARDUINO_VERSION_MAJOR >= 4
	int ksPinnedTlsClient::verifyCertificate(void* context, mbedtls_x509_crt* cert, int depth, uint32_t* flags)
	{
		auto& pin{*static_cast<ksPinnedTlsClient*>(context)};
		*flags = 0;
		if (depth != 0)
			return 0;

		uint8_t hash[32];
		if (!cert || !cert->raw.p || !cert->raw.len)
			return MBEDTLS_ERR_X509_FATAL_ERROR;
#if MBEDTLS_VERSION_MAJOR >= 4
		size_t hashLen{0};
		if (psa_hash_compute(PSA_ALG_SHA_256, cert->raw.p, cert->raw.len, hash, sizeof(hash), &hashLen) != PSA_SUCCESS || hashLen != sizeof(hash))
			return MBEDTLS_ERR_X509_FATAL_ERROR;
#else
		if (mbedtls_sha256(cert->raw.p, cert->raw.len, hash, 0) != 0)
			return MBEDTLS_ERR_X509_FATAL_ERROR;
#endif
		uint8_t difference{0};
		for (size_t i{0}; i < sizeof(hash); ++i)
			difference |= hash[i] ^ pin.fingerprintBytes[i];
		if (difference)
			return MBEDTLS_ERR_X509_FATAL_ERROR;

		pin.bitflags.verified = true;
		return 0;
	}
#endif
#endif
} // namespace

namespace ksf::misc
{
	std::unique_ptr<ksNetworkClient_t> createPinnedTlsClient(const std::string& fingerprint)
	{
		auto client{std::make_unique<ksPinnedTlsClient>()};
		if (!client->setPinnedFingerprint(fingerprint))
			return nullptr;
		return client;
	}
}
