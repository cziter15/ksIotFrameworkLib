#pragma once

#include <string>
#include <stdint.h>

#include <WiFiClientSecure.h>

namespace ksf
{
	class ksCertFingerprintBase
	{
		protected:
			bool stringToBytes(const std::string& string, uint8_t * bytes, uint8_t len);

		public:
			virtual bool setup(WiFiClientSecure* client, const std::string& fingerprint);
			virtual bool verify(WiFiClientSecure* client) const;
	};

#ifdef ESP32
	class ksCertFingerprintESP32 : public ksCertFingerprintBase
	{
		private:
			uint8_t fingerprintBytes[32];

		public:
			bool setup(WiFiClientSecure* client, const std::string& fingerprint) override;
			bool verify(WiFiClientSecure* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP32;
#endif

#ifdef ESP8266
	class ksCertFingerprintESP8266 : public ksCertFingerprintBase
	{
		public:
			bool setup(WiFiClientSecure* client, const std::string& fingerprint) override;
			bool verify(WiFiClientSecure* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP8266;
#endif
}