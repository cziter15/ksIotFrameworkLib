/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

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
				Converts passed fingerprint string to the static array of bytes.
				
				@param fingerprint Fingerprint string.
				@param bytes Output buffer pointer.
				@param bytesLen Output buffer size.
				@return True if fingerprint seems to be OK, otherwise false.
			*/
			bool fingerprintToBytes(const std::string& fingerprint, uint8_t* bytes, uint8_t bytesLen) const;

		public:
			/*
				Performs certificate fingerprint setup (platform dependent).
				
				@param client Pointer of WiFiClientSecure.
				@param fingerprint Fingerprint string.
				@return True if setup has been successfull, otherwise false.
			*/
			virtual bool setup(WiFiClientSecure* client, const std::string& fingerprint);

			/*
				Performs certificate fingerprint validation (platform dependent).
				
				@param client Pointer to WiFiClientSecure.
				@return True if fingerprint validation passed, otherwise false.
			*/
			virtual bool verify(WiFiClientSecure* client) const;
	};

#ifdef ESP32
	class ksCertFingerprintESP32 : public ksCertFingerprintBase
	{
		private:
			uint8_t fingerprintBytes[32];	// Fingerprint string transformed into array of bytes.

		public:
			/*
				Performs certificate fingerprint setup (platform dependent).
				On ESP32 it will copy fingerprint into fingerprintBytes.
				
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
				On ESP8266 it will pass fingerprint to WiFiClientSecure's setFingerprint method.
				
				@param client Pointer to WiFiClientSecure.
				@param fingerprint Fingerprint string.
				@return True if setup passed, otherwise false.
			*/
			bool setup(WiFiClientSecure* client, const std::string& fingerprint) override;

			/*
				Always returns true on ESP8266 as this functionality is handled under the hood.

				@param client Pointer to WiFiClientSecure.
				@return Always true on ESP8266.
			*/
			bool verify(WiFiClientSecure* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP8266;
#endif
}