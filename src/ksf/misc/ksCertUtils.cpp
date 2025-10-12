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
		if (fingerprintToBytes(fingerprint, fingerprintBytes, sizeof(fingerprintBytes)))
		{
			constexpr auto handshakeTimeoutSec{KSF_MQTT_TIMEOUT_MS / KSF_ONE_SEC_MS};
			clientSecure->setHandshakeTimeout(handshakeTimeoutSec);
			clientSecure->setInsecure();
			return true;
		}
		return false;
	}

	bool ksCertFingerprintESP32::verify(ksCertUtilsNetCLientSecure_t* client) const
	{
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
	}
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
