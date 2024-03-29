/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <stdint.h>
#include <limits>
#include <string>
#include <charconv>
#include "stdlib_noniso.h"

#include "Arduino.h"

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
#define KSF_MQTT_RECONNECT_DELAY_MS 5000UL
#endif

#ifndef KSF_MQTT_TIMEOUT_SEC
/*! MQTT socket timeout in seconds. On ESP32 is also used as connect (blocking) timeout. */
#define KSF_MQTT_TIMEOUT_SEC 4UL
#endif

#ifndef KSF_CAP_WIFI_CONNECT_TIMEOUT_SEC
/*! Configuration portal maximum time limit for test connection to AP. */
#define KSF_CAP_WIFI_CONNECT_TIMEOUT_SEC 10UL
#endif

#ifndef KSF_WIFI_TIMEOUT_MS
/*! Time in milliseconds that must pass without WiFi connection to trigger device restart. */
#define KSF_WIFI_TIMEOUT_MS 120000UL
#endif

#ifndef KSF_WIFI_RECONNECT_TIME_MS
/*! Time in milliseconds that must pass without WiFi connection to retry connection. */
#define KSF_WIFI_RECONNECT_TIME_MS 5000UL
#endif

/*! Helper macro for init ks Framework. */
#define KSF_FRAMEWORK_INIT() ksf::initializeFramework();

namespace ksf
{
	namespace EOTAType
	{
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
		@brief Initializes ksIotFramework.
	*/
	extern void initializeFramework();

	/*!
		@brief Removes directory with all its content.
		@param path Path to be removed.
		@return True if removal was successful.
	*/
	extern bool removeDirectory(const char* path);

	/*!
		@brief Erases configuration data (config file and NVS data).
		@return True if erase was successful. False if one or more of the steps failed.
	*/
	extern bool eraseConfigData();

	/*!
		@brief Updates device uptime, handling millis() function rollover.
	*/
	extern void updateDeviceUptime();

	/*!
		@brief Retrieves device uptime in milliseconds (64 bit wide).
		@return Milliseconds that have passed since device boot/restart.
	*/
	extern uint64_t millis64();

	/*!
		@brief Helper functionn double values into stirng.

		GCC is missing double support in std::to_string.

		@param value Double value to be converted
		@param base Requested decimal points
	*/
	extern std::string to_string(double value, const int base);

	/*!
		@brief Helper functionn float values into stirng.

		GCC is missing float support in std::to_string.

		@param value Float value to be converted
		@param base Requested decimal points
		@return Converted value in a form of string.
	*/
	extern std::string to_string(float value, const int base);

	/*!
		@brief Helper function template to convert a string into another type.
		@param input Value to be converted
		@return Converted value in a form of string.
	*/
	template <typename _Type>
	inline std::string to_string(const _Type& input)
	{
		return std::to_string(input);
	}

	/*!
		@brief Helper function to convert from string to another type.
		@param input Input - string, string_view etc
		@param out Output reference - can be int, double (anything that  std::from_chars support)
		@return True if conversion succeded, otherwise false.
	*/
	template <typename _In, typename _Out>
	bool from_chars(const _In& input, _Out& out)
	{
		const auto& result{std::from_chars(input.data(), input.data() + input.size(), out)};
		return result.ec == std::errc();
	}

	/*!
		@brief Retrieves OTA boot type.
		@return OTA boot type.
	*/
	extern EOTAType::Type getOtaBootType();

	/*!
		@brief Saves OTA boot type.

		The value will be used in next boot to determine what type of OTA has been started.

		@param type OTA boot type to be saved
	*/
	extern void saveOtaBootIndicator(EOTAType::Type type = EOTAType::OTA_GENERIC);

	/*!
		@brief Helper function to check if a string or string view starts with.
		@param input Input string
		@param match Matching string
	*/
	template<class _Type1, class _Type2>
	inline bool starts_with(const _Type1& input, const _Type2& match)
	{
		return input.size() >= match.size()	&& std::equal(match.begin(), match.end(), input.begin());
	}

	/*!
		@brief Helper function to get latest reset reason.
	*/
	extern const std::string getResetReason();

	/*!
		@brief Helper function to get uptime from seconds.
		@param seconds Seconds to be converted
		@return Converted value in a form of string.
	*/
	extern const std::string getUptimeFromSeconds(uint32_t seconds);

	/*!
		@brief Helper function to get uptime in a form of string.
		@return Uptime in a form of string.
	*/
	extern const std::string getUptimeString();

	/*!
		@brief This function loads WiFi credentials from flash.
		@param ssid SSID ref to be loaded
		@param password Password ref to be loaded
	*/
	extern void loadCredentials(std::string& ssid, std::string& password);

	/*!
		@brief This function saves WiFi credentials to flash.
		@param ssid SSID to be saved
		@param password Password to be saved
	*/
	extern void saveCredentials(std::string ssid, std::string password);
}