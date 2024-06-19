/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksCertUtils.h"
#include "../ksConstants.h"

namespace ksf
{
	/*
		Convert ascii char into a byte value.

		@param c ascii char
		@return byte value
	*/
	inline uint8_t htoi (unsigned char c)
	{
		if (c>='0' && c <='9') 
			return c - '0';
		if (c>='A' && c<='F') 
			return 10 + c - 'A';
		if (c>='a' && c<='f') 
			return 10 + c - 'a';

		return 255;
	}
	
	bool ksCertFingerprint::fingerprintToBytes(const std::string& fingerprint, uint8_t * bytes, uint8_t bytesLen) const
	{
		if (fingerprint.size() != bytesLen * 2)
			return false;

		for (uint8_t ci{0}, bi{0}; ci < fingerprint.size(); ci += 2)
		{
			uint8_t c{htoi(fingerprint[ci])};
			uint8_t d{htoi(fingerprint[ci+1])};

			if ((c>15) || (d>15))
				return false;

			bytes[bi++] = (c<<4)|d;
		}

		return true;
	}

#ifdef ESP32
	bool ksCertFingerprintESP32::setup(WiFiClientSecure* clientSecure, const std::string& fingerprint)
	{
		if (fingerprintToBytes(fingerprint, fingerprintBytes, sizeof(fingerprintBytes)))
		{
			clientSecure->setInsecure();
			return true;
		}

		return false;
	}

	bool ksCertFingerprintESP32::verify(WiFiClientSecure* client) const
	{
		static const char characters[] {"0123456789ABCDEF"};

		std::string fingerprintStr(sizeof(fingerprintBytes) * 2, 0);
		
		auto buf{const_cast<char *>(fingerprintStr.data())};
		for (const auto &oneInputByte : fingerprintBytes)
		{
			*buf++ = characters[oneInputByte >> 4];
			*buf++ = characters[oneInputByte & 0x0F];
		}

		return client->verify(fingerprintStr.c_str(), nullptr);
	}
#endif

#ifdef ESP8266
	bool ksCertFingerprintESP8266::setup(WiFiClientSecure* clientSecure, const std::string& fingerprint)
	{
		uint8_t fingerprintBytes[20];
		if (fingerprintToBytes(fingerprint, fingerprintBytes, sizeof(fingerprintBytes)))
		{
			clientSecure->setFingerprint(fingerprintBytes);
			return true;
		}

		return false;
	}

	bool ksCertFingerprintESP8266::verify(WiFiClientSecure* client) const
	{
		return true;
	}
#endif
}
