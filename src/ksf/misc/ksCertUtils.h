/*
 *	Copyright (c) 2021-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>
#include <stdint.h>

#if (defined(ESP32))
	#if (defined(ESP32) && ESP_ARDUINO_VERSION_MAJOR >= 3)
		#define ksCertUtilsNetCLientSecure_t NetworkClientSecure
	#else
		#define ksCertUtilsNetCLientSecure_t WiFiClientSecure
	#endif
	class ksCertUtilsNetCLientSecure_t;
#elif (defined(ESP8266))
	namespace BearSSL{class WiFiClientSecure;}
	#define ksCertUtilsNetCLientSecure_t BearSSL::WiFiClientSecure
#else 
	#error Platform not implemented.
#endif

namespace ksf::misc
{
	/*!
		@brief Interface for certificate fingerprint verification.

		Implements platform-dependent certificate fingerprint verification. 
		Used to validate certificate fingerprints without the requirement of certificate-chain verification.
	*/
	class ksCertFingerprint
	{
		protected:
			/*!
				@brief Converts fingerprint string into array of bytes.
				@param fingerprint Fingerprint string
				@param bytes Output buffer pointer
				@param bytesLen Output buffer size
				@return True if fingerprint seems to be OK, otherwise false.
			*/
			bool fingerprintToBytes(const std::string& fingerprint, uint8_t* bytes, uint8_t bytesLen) const;

		public:
			/*!
				@brief Constructor.
			*/
			ksCertFingerprint();

			/*!
				@brief Destructor.
			*/
			virtual ~ksCertFingerprint();

			/*!
				@brief Performs certificate fingerprint setup (platform dependent).
				@param client Pointer of ksCertUtilsNetCLientSecure_t
				@param fingerprint Fingerprint string
				@return True if setup has been successfull, otherwise false.
			*/
			virtual bool setup(ksCertUtilsNetCLientSecure_t* client, const std::string& fingerprint) = 0;

			/*!
				@brief Performs certificate fingerprint validation (platform dependent).
				@param client Pointer of ksCertUtilsNetCLientSecure_t
				@return True on verification pass, otherwise false.
			*/
			virtual bool verify(ksCertUtilsNetCLientSecure_t* client) const = 0;
	};

#ifdef ESP32
	class ksCertFingerprintESP32 : public ksCertFingerprint
	{
		private:
			uint8_t fingerprintBytes[32];	//!< Fingerprint string transformed into array of bytes.

		public:
			/*!
				@brief Performs fingerprint setup (platform dependent).
				
				On ESP32 it will copy fingerprint into fingerprintBytes.
				
				@param client Pointer to ksCertUtilsNetCLientSecure_t
				@param fingerprint Fingerprint string
				@return True if setup passed, otherwise false.
			*/
			bool setup(ksCertUtilsNetCLientSecure_t* client, const std::string& fingerprint) override;

			/*!
				@brief Converts bytes into a string and performs fingerprint validation.
				@param client Pointer to ksCertUtilsNetCLientSecure_t
				@return True if verification passed, otherwise false.
			*/
			bool verify(ksCertUtilsNetCLientSecure_t* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP32;
#endif

#ifdef ESP8266
	class ksCertFingerprintESP8266 : public ksCertFingerprint
	{
		public:
			/*!
				@brief Performs fingerprint setup (platform dependent).
				@param client Pointer of ksCertUtilsNetCLientSecure_t.
				@param fingerprint Fingerprint string.
				@return True if setup passed, otherwise false.
			*/
			bool setup(ksCertUtilsNetCLientSecure_t* client, const std::string& fingerprint) override;

			/*!
				@brief Converts bytes into a string and performs fingerprint validation.

				Always returns true on ESP8266 as this functionality is handled under the hood.

				@param client Pointer to ksCertUtilsNetCLientSecure_t
				@return Always true in case of ESP8266.
			*/
			bool verify(ksCertUtilsNetCLientSecure_t* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP8266;
#endif
}