/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>
#include <cstdint>

#if (defined(ESP32))
	#include <esp_arduino_version.h>
	#if ESP_ARDUINO_VERSION_MAJOR >= 4
		#include <WiFiClientSecure.h>
		namespace ksf::misc { class ksPinnedTlsClientESP32; }
		#define ksCertUtilsNetCLientSecure_t ksf::misc::ksPinnedTlsClientESP32
	#elif ESP_ARDUINO_VERSION_MAJOR >= 3
		#define ksCertUtilsNetCLientSecure_t NetworkClientSecure
		class ksCertUtilsNetCLientSecure_t;
	#else
		#define ksCertUtilsNetCLientSecure_t WiFiClientSecure
		class ksCertUtilsNetCLientSecure_t;
	#endif
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

		Implements platform-specific certificate fingerprint verification. 
		This allows validation of certificate fingerprints without requiring certificate-chain verification.
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
#if ESP_ARDUINO_VERSION_MAJOR >= 4
	class ksPinnedTlsClientESP32;
#endif
	class ksCertFingerprintESP32 : public ksCertFingerprint
	{
		private:
			uint8_t fingerprintBytes[32]{};	//!< Fingerprint string transformed into array of bytes.
#if ESP_ARDUINO_VERSION_MAJOR >= 4
			friend class ksPinnedTlsClientESP32;
			ksCertUtilsNetCLientSecure_t* configuredClient{nullptr};
			bool verified{false};
#endif

		public:
			/*!
				@brief Performs fingerprint setup (platform dependent).
				
				On ESP32 it will copy fingerprint into fingerprintBytes.
				On Arduino-ESP32 4.x the client type is ksPinnedTlsClientESP32 and
				setup() also binds this verifier to it.
				
				@param client Pointer to ksCertUtilsNetCLientSecure_t
				@param fingerprint Fingerprint string
				@return True if setup passed, otherwise false.
			*/
			bool setup(ksCertUtilsNetCLientSecure_t* client, const std::string& fingerprint) override;

			/*!
				@brief Checks the certificate fingerprint for the current connection.
				@param client Pointer to ksCertUtilsNetCLientSecure_t
				@return True if verification passed, otherwise false.
			*/
			bool verify(ksCertUtilsNetCLientSecure_t* client) const override;
	};

	using ksCertFingerprintHolder = ksCertFingerprintESP32;

#if ESP_ARDUINO_VERSION_MAJOR >= 4
	class ksPinnedTlsClientESP32 : public NetworkClientSecure
	{
		private:
			ksCertFingerprintESP32* fingerprint{nullptr};

			static int verifyCertificate(void* context, mbedtls_x509_crt* cert, int depth, uint32_t* flags);

		public:
			/*!
				@brief Sets the verifier, which must outlive this client.
				@param verifier Fingerprint verifier to use for subsequent connections.
			*/
			void setFingerprintVerifier(ksCertFingerprintESP32& verifier);

			int connect(IPAddress ip, uint16_t port, int32_t timeout) override;
			int connect(IPAddress ip, uint16_t port) override;
			int connect(const char* host, uint16_t port, int32_t timeout) override;
			int connect(const char* host, uint16_t port) override;
			void stop() override;
			size_t write(const uint8_t* data, size_t size) override;
			size_t write(uint8_t data) override;
	};
#endif
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