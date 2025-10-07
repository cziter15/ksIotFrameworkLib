/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <limits>
#include <string>
#include <charconv>
#include <stdlib_noniso.h>

/*! One second in milliseconds. */
#define KSF_ONE_SEC_MS 1000UL

/*!
	Macro used to convert seconds to milliseconds.
	@param seconds Seconds
	@return Milliseconds
 */
#define KSF_SEC_TO_MS(seconds) (seconds*1000UL)

#ifndef KSF_MQTT_RECONNECT_DELAY_MS
/*! MQTT reconnect delay in milliseconds. How much time in ms need to pass to retry connection. */
#define KSF_MQTT_RECONNECT_DELAY_MS 10000UL
#endif

#ifndef KSF_DOMAIN_QUERY_INTERVAL_MS
/*! Interval in milliseconds between DNS query retries. */
#define KSF_DOMAIN_QUERY_INTERVAL_MS 3000UL
#endif

#ifndef KSF_DOMAIN_QUERY_DNS_SERVER
/*! IP address of the DNS server to use for resolving domain names. */
#define KSF_DOMAIN_QUERY_DNS_SERVER IPAddress(8, 8, 8, 8)
#endif

#ifndef KSF_MQTT_TIMEOUT_MS
/*! MQTT socket timeout in milliseconds. On ESP32 is also used as connect (blocking) timeout. */
#define KSF_MQTT_TIMEOUT_MS 4000UL
#endif

#ifndef KSF_WIFI_TIMEOUT_MS
/*! Time in milliseconds that must pass without WiFi connection to trigger device restart. */
#define KSF_WIFI_TIMEOUT_MS 120000UL
#endif

#ifndef KSF_WIFI_RECONNECT_TIME_MS
/*! Time in milliseconds that must pass without WiFi connection to retry connection. */
#define KSF_WIFI_RECONNECT_TIME_MS 5000UL
#endif

#ifndef KSF_WATCHDOG_TIMEOUT_SECS
/*! Watchdog timeout in seconds. */
#define KSF_WATCHDOG_TIMEOUT_SECS 10UL
#endif

/*! Helper macro for init ks Framework. */
#define KSF_FRAMEWORK_INIT() ksf::initializeFramework();

/*! Library version string. Auto-bumped by workflow. */
#define KSF_LIBRARY_VERSION "1.0.39"

namespace ksf
{
	namespace EOTAType
	{
		/*!
			@brief OTA boot type enum.
		*/
		enum Type
		{
			/*! No OTA happened. */
			NO_OTA,
			/*! Generic OTA happened. */
			OTA_GENERIC,
			/*! OTA happened through captive portal. */
			OTA_PORTAL,
		};
	}

	/*!
		@brief Retrieves NVS directory name.
		@return NVS directory name.
	*/
	extern const char* getNvsDirectory();

	/*!
		@brief Initializes the framework.
	*/
	extern void initializeFramework();

	/*!
		@brief Removes a directory with all its content.
		@param path Path of the directory to be removed.
		@return True if removal was successful. False if one or more of the steps failed.
	*/
	extern bool removeDirectory(const char* path);

	/*!
		@brief Erases the configuration data from the device. 
		@return True if erase was successful. False if one or more of the steps failed.
	*/
	extern bool eraseConfigData();

	/*!
		@brief Updates the device uptime, handling millis() function rollover.
	*/
	extern void updateDeviceUptime();

	/*!
		@brief Retrieves the device uptime in milliseconds (64 bit wide).
		@return Milliseconds that have passed since device boot/restart.
	*/
	extern uint64_t millis64();

	/*!
		@brief Helper function that converts double value into a string.

		GCC is missing double support in std::to_string.

		@param value Double value to be converted
		@param base Requested decimal points
	*/
	extern std::string to_string(double value, const int base);

	/*!
		@brief Helper function that converts float value into a string.

		GCC is missing float support in std::to_string.

		@param value Float value to be converted
		@param base Requested decimal points
		@return Converted value in a form of string.
	*/
	extern std::string to_string(float value, const int base);

	/*!
		@brief Helper function template to convert a string into another type.

		@tparam _Type Type to be converted.

		@param input Value to be converted.
		@return Converted value in a form of string.
	*/
	template <typename _Type>
	inline std::string to_string(const _Type& input)
	{
		return std::to_string(input);
	}

	/*!
		@brief Converts an integer to its hexadecimal string representation.

		@param value The integer value to be converted.

		@return A string representing the hexadecimal value of the input integer.
	*/
	inline std::string to_hex(int value)
	{
		if (value == 0)
			return {"0"};

		char buffer[8];
		unsigned int uvalue{static_cast<unsigned int>(value)}, pos{8};
		
		while (uvalue != 0) 
		{
			unsigned int nibble{uvalue & 0xf};
			buffer[--pos] = (nibble < 10) ? ('0' + nibble) : ('a' + (nibble - 10));
			uvalue >>= 4;
		}

		return std::string(buffer + pos, 8 - pos);
	}

	/*!
		@brief Helper function to convert string-like value into another type (int, double etc).

		@tparam _In Input type.
		@tparam _Out Output type.

		@param input Input - string, string_view etc.
		@param out Output reference - can be int, double (anything that std::from_chars support).
		@return True if conversion succeeded, otherwise false.
	*/
	template <typename _In, typename _Out>
	bool from_chars(const _In& input, _Out& out)
	{
		const auto& result{std::from_chars(input.data(), input.data() + input.size(), out)};
		return result.ec == std::errc();
	}

	/*!
		@brief Retrieves device UUID in a form of hexadecimal string.

		This function returns device UUID in a form of hexadecimal string, which is used to identify the device. The UUID is
		generated by the framework and it's unique for each device.

		@return Device UUID in a form of hexadecimal string.
	*/
	std::string getDeviceUuidHex();

	/*!
		@brief Retrieves OTA boot type from device filesystem. If not found, returns EOTAType::NO_OTA.

		There are few OTA boot types, which are defined in EOTAType::Type enum. This value is used to determine reset reason.
		In case of system restart, if there's OTA boot set, it will be used to determine correct reset reason for System Restart.

		@return OTA boot type.
	*/
	extern EOTAType::Type getOtaBootType();

	/*!
		@brief Saves OTA boot type to device filesystem.

		This value will be used on next boot to determine OTA boot type. Upon read, the file containing this value will be removed.

		@param type OTA boot type.
	*/
	extern void saveOtaBootIndicator(EOTAType::Type type = EOTAType::OTA_GENERIC);

	/*!
		@brief Helper function to check if a string or string view starts with.

		@tparam _Type1 First type.
		@tparam _Type2 Second type.

		@param input Input 'string'.
		@param match The string to check with input 'string'.

		@return True if input starts with match. False otherwise.
	*/
	template<class _Type1, class _Type2>
	inline bool starts_with(const _Type1& input, const _Type2& match)
	{
		return input.size() >= match.size() && std::equal(match.begin(), match.end(), input.begin());
	}

	/*!
		@brief Escapes a string for safe use in JSON.

		This function escapes special characters according to JSON specification (RFC 8259).
		It handles: double quotes, backslashes, and control characters.

		@param input The string to be escaped.
		@return JSON-safe escaped string.
	*/
	extern std::string json_escape(const std::string& input);

	/*!
		@brief Helper function to get latest reset reason.
		@return A string representing the reason for the last reset.
	*/
	extern const std::string getResetReason();

	/*!
		@brief Returns uptime string from seconds.
		@param seconds Number of seconds.
		@return Uptime in a form of string (day, hour, minute, second).
	*/
	extern const std::string getUptimeFromSeconds(uint32_t seconds);

	/*!
		@brief Helper function that returns uptime in a form of string.
		@return Device uptime in a form of string (day, hour, minute, second).
	*/
	extern const std::string getUptimeString();

	/*!
		@brief Loads WiFi credentials from flash.
		@param ssid SSID reference to be loaded.
		@param password Password reference to be loaded.
	*/
	extern void loadCredentials(std::string& ssid, std::string& password);

	/*!
		@brief Saves WiFi credentials to flash.
		@param ssid SSID to be saved.
		@param password Password to be saved.
	*/
	extern void saveCredentials(std::string ssid, std::string password);
}
