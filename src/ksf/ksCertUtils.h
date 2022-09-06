#pragma once

#include <string>
#include <stdint.h>

#include <WiFiClientSecure.h>

namespace ksf
{
	class ksCertFingerprintBase
	{
		protected:
			/*
				Coverts string to static array of bytes. Used to convert SHA1/SHA256 strings into array of bytes.
				
				@param fingerprint Fingerprint string.
				@param bytes Output buffer pointer.
				@param bytesLen Output buffer size.
				@return True if fingerprint seems to be OK, otherwise false.
			*/
			bool fingerprintToBytes(const std::string& fingerprint, uint8_t * bytes, uint8_t bytesLen);

		public:
			/*
				Performs fingerprint setup (platform dependent).
				
				@param client Pointer of WiFiClientSecure.
				@param fingerprint Fingerprint string.
				@return True if setup has been successfull, otherwise false.
			*/
			virtual bool setup(WiFiClientSecure* client, const std::string& fingerprint);

			/*
				Performs fingerprint validation (platform dependent).
				
				@param client Pointer of WiFiClientSecure.
				@return True if verification passed, otherwise false.
			*/
			virtual bool verify(WiFiClientSecure* client) const;
	};

#ifdef ESP32
	class ksCertFingerprintESP32 : public ksCertFingerprintBase
	{
		private:
			uint8_t fingerprintBytes[32];	// Fingerprint transformed into bytes.

		public:
			/*
				Performs fingerprint setup (platform dependent).
				On ESP32 it copies fingerprint into fingerprintBytes.
				
				@param client Pointer of WiFiClientSecure.
				@param fingerprint Fingerprint string.
				@return True if setup has been successfull, otherwise false.
			*/
			bool setup(WiFiClientSecure* client, const std::string& fingerprint) override;

			/*
				Converts bytes into a string and performs fingerprint validation.

				@param client Pointer of WiFiClientSecure.
				@return True on verification pass, otherwise false.
			*/
			bool verify(WiFiClientSecure* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP32;
#endif

#ifdef ESP8266
	class ksCertFingerprintESP8266 : public ksCertFingerprintBase
	{
		public:
			/*
				Performs fingerprint setup (platform dependent).
				On ESP8266 it simply passes fingerprint to setFingerprint method.
				
				@param client Pointer of WiFiClientSecure.
				@param fingerprint Fingerprint string.
				@return True if setup has been successfull, otherwise false.
			*/
			bool setup(WiFiClientSecure* client, const std::string& fingerprint) override;

			/*
				Always returns true on ESP8266 as this functionality is handled under the hood.

				@param client Pointer of WiFiClientSecure.
				@return Always true on ESP8266.
			*/
			bool verify(WiFiClientSecure* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP8266;
#endif
}