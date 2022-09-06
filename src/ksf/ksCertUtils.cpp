#include "ksCertUtils.h"

namespace ksf
{
	inline uint8_t htoi (unsigned char c)
	{
		if (c>='0' && c <='9') return c - '0';
		else if (c>='A' && c<='F') return 10 + c - 'A';
		else if (c>='a' && c<='f') return 10 + c - 'a';
		else return 255;
	}
	
	bool ksCertFingerprintBase::stringToBytes(const std::string& string, uint8_t * bytes, uint8_t len)
	{
		if (string.size() != len * 2)
			return false;

		for (uint8_t idx = 0; idx < len;)
		{
			uint8_t c = htoi(string[idx*2]);
			uint8_t d = htoi(string[idx*2+1]);

			if ((c>15) || (d>15))
				return false;

			bytes[idx++] = (c<<4)|d;
		}

		return true;
	}

#ifdef ESP32
	bool ksCertFingerprintESP32::setup(WiFiClientSecure* clientSecure, const std::string& fingerprint)
	{
		if (stringToBytes(fingerprint, fingerprintBytes, sizeof(fingerprintBytes)))
		{
			clientSecure->setInsecure();
			return true;
		}

		return false;
	}

	bool ksCertFingerprintESP32::verify(WiFiClientSecure* client) const
	{
		static const char characters[] = "0123456789ABCDEF";

		std::string fingerprintStr(sizeof(fingerprintBytes) * 2, 0);
		
		auto buf = const_cast<char *>(fingerprintStr.data());
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
		if (stringToBytes(fingerprint, fingerprintBytes, sizeof(fingerprintBytes)))
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
