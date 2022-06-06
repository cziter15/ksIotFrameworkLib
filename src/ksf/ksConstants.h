/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

/* Simply ONE second in milliseconds. */
#define KSF_ONE_SECOND_MS 1000UL

/* MQTT reconnect delay in milliseconds. How much time in ms need to pass to retry connection. */
#define KSF_MQTT_RECONNECT_DELAY_MS 5000UL

/* MQTT socket timeout in seconds. On ESP32 is also used as connect (blocking) timeout. */
#define KSF_MQTT_TIMEOUT_SEC 3UL

/* Time in seconds for how much configuration portal should be available. Device will reset after that time. */
#define KSF_CAP_PORTAL_TIMEOUT_SEC 120UL

/* Configuration portal maximum time limit for test connection to AP. */
#define KSF_CAP_WIFI_CONNECT_TIMEOUT_SEC 10UL

/* Time in milliseconds that must pass without WiFi connection to trigger device restart. */
#define KSF_WIFI_TIMEOUT_MS 120000UL

/* Time in milliseconds that must pass without WiFi connection to retry connection. */
#define KSF_WIFI_RECONNECT_TIME_MS 5000UL

/* Helper macro that handles app (appClass) initialization and calls loop method every delayTime ms (wait) */
#define RUN_APP_BLOCKING_LOOPED_TIME(appClass, delayTime)	\
{															\
	appClass application;									\
	if (application.init())									\
	{														\
		while(application.loop())							\
		{													\
			delay(delayTime);								\
		}													\
	}														\
}

/* Helper macro that handles app (appClass) initialization and calls loop method as frequent as possible (no-wait). */
#define RUN_APP_BLOCKING_LOOPED_NODELAY(appClass)			\
{															\
	appClass application;									\
	if (application.init())									\
	{														\
		while(application.loop())							\
		{													\
			yield();										\
		}													\
	}														\
}

/* Helper macro that handles app (appClass) initialization and calls loop method every one ms (wait) */
#define RUN_APP_BLOCKING_LOOPED(appClass) RUN_APP_BLOCKING_LOOPED_TIME(appClass, 1)

namespace ksf
{
	/* Method that performs ksf framework initialization. Must be called as early as possible. */
	extern void initKsfFramework();
	extern unsigned long long millis64();
}