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

#include "ksCertUtils.h"

#if defined(ESP32) && ESP_ARDUINO_VERSION_MAJOR >= 4
	#include <mbedtls/version.h>
	#include <mbedtls/x509.h>
	#if MBEDTLS_VERSION_MAJOR >= 4
		#include <psa/crypto.h>
	#else
		#include <mbedtls/sha256.h>
	#endif
#endif

namespace ksf::misc
{
	ksCertFingerprint::ksCertFingerprint() = default;
	ksCertFingerprint::~ksCertFingerprint() = default;

	bool ksCertFingerprint::fingerprintToBytes(const std::string& fingerprint, uint8_t * bytes, uint8_t bytesLen) const
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

#ifdef ESP32
	bool ksCertFingerprintESP32::setup(ksCertUtilsNetCLientSecure_t* clientSecure, const std::string& fingerprint)
	{
#if ESP_ARDUINO_VERSION_MAJOR >= 4
		configuredClient = nullptr;
		verified = false;
#endif
		if (!clientSecure)
			return false;

		if (fingerprintToBytes(fingerprint, fingerprintBytes, sizeof(fingerprintBytes)))
		{
			constexpr auto handshakeTimeoutSec{KSF_MQTT_TIMEOUT_MS / KSF_ONE_SEC_MS};
			clientSecure->setHandshakeTimeout(handshakeTimeoutSec);
			clientSecure->setInsecure();
#if ESP_ARDUINO_VERSION_MAJOR >= 4
			configuredClient = clientSecure;
			clientSecure->setFingerprintVerifier(*this);
#endif
			return true;
		}
		return false;
	}

	bool ksCertFingerprintESP32::verify(ksCertUtilsNetCLientSecure_t* client) const
	{
#if ESP_ARDUINO_VERSION_MAJOR >= 4
		return client && client == configuredClient && verified && client->connected();
#else
		static constexpr char hexChars[]{"0123456789ABCDEF"};
		constexpr size_t hexLen{sizeof(fingerprintBytes) * 2};
		char hexBuffer[hexLen + 1];
		char* ptr{hexBuffer};
		for (const auto byte : fingerprintBytes) 
		{
			*ptr++ = hexChars[byte >> 4];
			*ptr++ = hexChars[byte & 0x0F];
		}
		*ptr = '\0';
		return client->verify(hexBuffer, nullptr);
#endif
	}

#if ESP_ARDUINO_VERSION_MAJOR >= 4
	void ksPinnedTlsClientESP32::setFingerprintVerifier(ksCertFingerprintESP32& verifier)
	{
		stop();
		fingerprint = &verifier;
		fingerprint->verified = false;
	}

	int ksPinnedTlsClientESP32::verifyCertificate(void* context, mbedtls_x509_crt* cert, int depth, uint32_t* flags)
	{
		auto& pin{*static_cast<ksCertFingerprintESP32*>(context)};
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

		pin.verified = true;
		return 0;
	}

	int ksPinnedTlsClientESP32::connect(IPAddress ip, uint16_t port, int32_t timeout)
	{
		stop();
		if (!fingerprint)
			return 0;
		if (fingerprint->configuredClient != this)
			return 0;

		_timeout = timeout;
		setPlainStart();
		if (!NetworkClientSecure::connect(ip, port, nullptr, nullptr, nullptr, nullptr))
			return 0;

		// No CA is configured: the callback and post-handshake check enforce the pin.
		mbedtls_ssl_conf_authmode(&sslclient->ssl_conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
		mbedtls_ssl_set_verify(&sslclient->ssl_ctx, verifyCertificate, fingerprint);
		if (!startTLS() || !fingerprint->verified || mbedtls_ssl_get_verify_result(&sslclient->ssl_ctx) != 0)
		{
			fingerprint->verified = false;
			stop();
			return 0;
		}
		return 1;
	}

	int ksPinnedTlsClientESP32::connect(IPAddress ip, uint16_t port)
	{
		return connect(ip, port, _timeout);
	}

	int ksPinnedTlsClientESP32::connect(const char*, uint16_t, int32_t)
	{
		stop();
		return 0;
	}

	int ksPinnedTlsClientESP32::connect(const char* host, uint16_t port)
	{
		return connect(host, port, _timeout);
	}

	void ksPinnedTlsClientESP32::stop()
	{
		if (fingerprint)
			fingerprint->verified = false;
		NetworkClientSecure::stop();
	}

	size_t ksPinnedTlsClientESP32::write(const uint8_t* data, size_t size)
	{
		if (!fingerprint || fingerprint->configuredClient != this || !fingerprint->verified || stillInPlainStart())
			return 0;
		return NetworkClientSecure::write(data, size);
	}

	size_t ksPinnedTlsClientESP32::write(uint8_t data)
	{
		return write(&data, 1);
	}
#endif
#endif

#ifdef ESP8266
	bool ksCertFingerprintESP8266::setup(ksCertUtilsNetCLientSecure_t* clientSecure, const std::string& fingerprint)
	{
		uint8_t fingerprintBytes[20];
		if (fingerprintToBytes(fingerprint, fingerprintBytes, sizeof(fingerprintBytes)))
		{
			clientSecure->setFingerprint(fingerprintBytes);
			return true;
		}

		return false;
	}

	bool ksCertFingerprintESP8266::verify([[maybe_unused]] ksCertUtilsNetCLientSecure_t* client) const
	{
		return true;
	}
#endif
}
