/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <WiFiClientSecure.h>

#include <string>
#include <stdint.h>

namespace ksf
{
	class ksCertFingerprint
	{
		protected:
			/*
				@brief Converts fingerprint string into array of bytes.
				@param fingerprint Fingerprint string
				@param bytes Output buffer pointer
				@param bytesLen Output buffer size
				@return True if fingerprint seems to be OK, otherwise false.
			*/
			bool fingerprintToBytes(const std::string& fingerprint, uint8_t* bytes, uint8_t bytesLen) const;

		public:
			/*
				@brief Performs certificate fingerprint setup (platform dependent).
				@param client Pointer of WiFiClientSecure
				@param fingerprint Fingerprint string
				@return True if setup has been successfull, otherwise false.
			*/
			virtual bool setup(WiFiClientSecure* client, const std::string& fingerprint);

			/*
				@brief Performs certificate fingerprint validation (platform dependent).
				@param client Pointer of WiFiClientSecure
				@return True on verification pass, otherwise false.
			*/
			virtual bool verify(WiFiClientSecure* client) const;
	};

#ifdef ESP32
	class ksCertFingerprintESP32 : public ksCertFingerprint
	{
		private:
			uint8_t fingerprintBytes[32];	// Fingerprint string transformed into array of bytes.

		public:
			/*
				@brief Performs fingerprint setup (platform dependent).
				
				On ESP32 it will copy fingerprint into fingerprintBytes.
				
				@param client Pointer to WiFiClientSecure
				@param fingerprint Fingerprint string
				@return True if setup passed, otherwise false.
			*/
			bool setup(WiFiClientSecure* client, const std::string& fingerprint) override;

			/*
				@brief Converts bytes into a string and performs fingerprint validation.
				@param client Pointer to WiFiClientSecure
				@return True if verification passed, otherwise false.
			*/
			bool verify(WiFiClientSecure* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP32;
#endif

#ifdef ESP8266
	class ksCertFingerprintESP8266 : public ksCertFingerprint
	{
		public:
			/*
				@brief Performs fingerprint setup (platform dependent).
				@param client Pointer of WiFiClientSecure.
				@param fingerprint Fingerprint string.
				@return True if setup passed, otherwise false.
			*/
			bool setup(WiFiClientSecure* client, const std::string& fingerprint) override;

			/*
				@brief Converts bytes into a string and performs fingerprint validation.

				Always returns true on ESP8266 as this functionality is handled under the hood.

				@param client Pointer to WiFiClientSecure
				@return Always true in case of ESP8266.
			*/
			bool verify(WiFiClientSecure* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP8266;
#endif
}