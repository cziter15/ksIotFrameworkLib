#pragma once

#define KSF_MQTT_RECONNECT_DELAY 5000UL
#define KSF_MQTT_TIMEOUT 15000UL
#define KSF_ONE_SECOND_MS 1000UL
#define KSF_CAP_PORTAL_TIMEOUT_SECONDS 120
#define KSF_CAP_WIFI_CONNECT_TIMEOUT 10
#define KSF_WIFI_TIMEOUT_MS 120000UL
#define KSF_WIFI_RECONNECT_TIME_MS 5000UL

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

#define RUN_APP_BLOCKING_LOOPED(appClass) RUN_APP_BLOCKING_LOOPED_TIME(appClass, 1)

namespace ksf
{
	extern void initKsfFramework();
}