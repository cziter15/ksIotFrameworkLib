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

/* Simply ONE second in milliseconds. */
#define KSF_ONE_SECOND_MS 1000UL

/* MQTT reconnect delay in milliseconds. How much time in ms need to pass to retry connection. */
#define KSF_MQTT_RECONNECT_DELAY_MS 5000UL

/* MQTT socket timeout in seconds. On ESP32 is also used as connect (blocking) timeout. */
#define KSF_MQTT_TIMEOUT_SEC 5UL

/* Time in seconds for how much configuration portal should be available. Device will reset after that time. */
#define KSF_CAP_PORTAL_TIMEOUT_SEC 120UL

/* Configuration portal maximum time limit for test connection to AP. */
#define KSF_CAP_WIFI_CONNECT_TIMEOUT_SEC 10UL

/* Time in milliseconds that must pass without WiFi connection to trigger device restart. */
#define KSF_WIFI_TIMEOUT_MS 120000UL

/* Time in milliseconds that must pass without WiFi connection to retry connection. */
#define KSF_WIFI_RECONNECT_TIME_MS 5000UL

/* Helper macro that handles app (appClass) initialization and calls loop method every delayTime ms (wait) */
#define KSF_RUN_APP_BLOCKING_LOOPED_TIME(appClass, delayTime)	\
{																\
	appClass application;										\
	if (application.init())										\
	{															\
		while(application.loop())								\
		{														\
			delay(delayTime);									\
		}														\
	}															\
}

/* Helper macro that handles app (appClass) initialization and calls loop method as frequent as possible (no-wait). */
#define KSF_RUN_APP_BLOCKING_LOOPED_NODELAY(appClass)			\
{																\
	appClass application;										\
	if (application.init())										\
	{															\
		while(application.loop())								\
		{														\
			yield();											\
		}														\
	}															\
}

/* Helper macro that handles app (appClass) initialization and calls loop method every one ms (wait) */
#define KSF_RUN_APP_BLOCKING_LOOPED(appClass) KSF_RUN_APP_BLOCKING_LOOPED_TIME(appClass, 1)

/* Helper macro for init ks Framework. */
#define KSF_FRAMEWORK_INIT() ksf::initializeFramework();

/* Helper macro for program memory stored strings. */
#define PGM_(x) ksf::string_from_progmem(PSTR(x))

namespace ksf
{
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
		Helper functionn to convert passed value into stirng.
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
		Retrieves if the device has been boot after OTA.
		
		@return True if started after OTA.
	*/
	extern bool isFirstOtaBoot();

	/*
		Saves OTA boot indicator.
	*/
	extern void saveOtaBootIndicator();

	/*
		Converts string from progmem to std::string.

		@param pgm_ptr Pointer to string in progmem.
		@return String from progmem.
	*/
	extern std::string string_from_progmem(const char* pgm_ptr);
}
