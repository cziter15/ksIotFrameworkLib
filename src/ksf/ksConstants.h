/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
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

/* Simply ONE second in milliseconds. */
#define KSF_ONE_SECOND_MS 1000UL

/* MQTT reconnect delay in milliseconds. How much time in ms need to pass to retry connection. */
#define KSF_MQTT_RECONNECT_DELAY_MS 5000UL

/* MQTT socket timeout in seconds. On ESP32 is also used as connect (blocking) timeout. */
#define KSF_MQTT_TIMEOUT_SEC 4UL

/* Time in seconds for how much configuration portal should be available. Device will reset after that time. */
#define KSF_CAP_PORTAL_TIMEOUT_SEC 120UL

/* Configuration portal maximum time limit for test connection to AP. */
#define KSF_CAP_WIFI_CONNECT_TIMEOUT_SEC 10UL

/* Time in milliseconds that must pass without WiFi connection to trigger device restart. */
#define KSF_WIFI_TIMEOUT_MS 120000UL

/* Time in milliseconds that must pass without WiFi connection to retry connection. */
#define KSF_WIFI_RECONNECT_TIME_MS 5000UL

/* Helper macro for init ks Framework. */
#define KSF_FRAMEWORK_INIT() ksf::initializeFramework();

/* Helper macro for program memory stored strings. */
#define PGM_(x) ksf::string_from_progmem(PSTR(x), sizeof(x) - 1)

namespace ksf
{
	namespace EOTAType
	{
		enum Type
		{
			NO_OTA,
			OTA_GENERIC,
			OTA_PORTAL,
		};
	}

	/*
		Initializes ksIotFramework.
	*/
	extern void initializeFramework();

	/*
		Updates device uptime, handling millis() function rollover.
	*/
	extern void updateDeviceUptime();

	/*
		Retrieves device uptime in milliseconds (64 bit wide).
		
		@return Milliseconds that have passed since device boot/restart.
	*/
	extern uint64_t millis64();

	/*
		Helper functionn double values into stirng.
		GCC is missing double support in std::to_string.

		@param value Double value to be converted.
		@param base Requested decimal points.
	*/
	extern std::string to_string(double value, const int base);

	/*
		Helper functionn float values into stirng.
		GCC is missing float support in std::to_string.

		@param value Float value to be converted.
		@param base Requested decimal points.
		@return Converted value in a form of string.
	*/
	extern std::string to_string(float value, const int base);

	/*
		Helper function template to convert a string into another type.

		@param input Value to be converted.
		@return Converted value in a form of string.
	*/
	template <typename _Type>
	inline std::string to_string(const _Type& input)
	{
		return std::to_string(input);
	}

	/*
		Helper function to convert from string to another type.

		@param input Input - string, string_view etc.
		@param out Output reference - can be int, double (anything that  std::from_chars support)
		@return True if conversion succeded, otherwise false.
	*/
	template <typename _In, typename _Out>
	bool from_chars(const _In& input, _Out& out)
	{
		const auto& result{std::from_chars(input.data(), input.data() + input.size(), out)};
		return result.ec == std::errc();
	}

	
	/*
		Checks whether recent device restart was caused by OTA update.
		
		@return True if OTA caused reboot, otherwise false.
	*/
	extern EOTAType::Type getOtaBootType();

	/*
		Saves OTA boot indicator to EEPROM.
	*/
	extern void saveOtaBootIndicator(EOTAType::Type type = EOTAType::OTA_GENERIC);

	/*
		Helper function to load char array from program memory into std::string.

		@param pgm_str_ptr Pointer to string in program memory.
		@param pgm_str_len Length of string in program memory.
		@return String in a form of std::string.
	*/
	extern std::string string_from_progmem(const char* pgm_str_ptr, size_t pgm_str_len);

	/*
		Helper function to check if a string or string view starts with.

		@param input Input string.
		@param match Matching string.
	*/
	template<class _Type1, class _Type2>
	inline bool starts_with(const _Type1& input, const _Type2& match)
	{
		return input.size() >= match.size()	&& std::equal(match.begin(), match.end(), input.begin());
	}

	/*
		Helper function to get last reset reason.
	*/
	extern const std::string getResetReason();

	/*
		Helper function to get uptime in a form of string.
	*/
	extern const std::string getUptimeString();

	/*
		This function loads credentials from EEPROM.

		@param ssid SSID ref to be loaded.
		@param password Password ref to be loaded.
	*/
	extern void loadCredentials(std::string& ssid, std::string& password);

	/*
		This function saves credentials to EEPROM.

		@param ssid SSID to be saved.
		@param password Password to be saved.
	*/
	extern void saveCredentials(const std::string& ssid, const std::string& password);
}