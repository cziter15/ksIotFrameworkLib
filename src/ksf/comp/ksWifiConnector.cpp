#include "../ksComposable.h"
#include "../ksConstants.h"
#include "ksWifiConnector.h"

#ifdef ESP32
	#include <WiFi.h>
#else
	#include <ESP8266WiFi.h>
#endif

namespace ksf
{
	ksWifiConnector::ksWifiConnector(const char* hostname)
	{
		#ifdef ESP32
			WiFi.setHostname(hostname);
		#else
			WiFi.hostname(hostname);
		#endif
	}

	bool ksWifiConnector::init(ksComposable* owner)
	{
		WiFi.mode(WIFI_STA);
		WiFi.setAutoConnect(false);
		WiFi.setAutoReconnect(false);
		WiFi.begin();

		#ifdef ESP32
			WiFi.setSleep(true);
		#endif

		configTime(0, 0, "pool.ntp.org", "time.nist.gov");

		lastReconnectTryTime = lastWifiCheckTime = millis();
		
		return true;
	}

	bool ksWifiConnector::loop()
	{
		uint32_t currentTime = millis();

		if (!WiFi.isConnected())
		{
			if (currentTime - lastReconnectTryTime > KSF_WIFI_RECONNECT_TIME_MS)
			{
				WiFi.reconnect();
				lastReconnectTryTime = currentTime;
			}
			else if (currentTime - lastWifiCheckTime > KSF_WIFI_TIMEOUT_MS)
			{
				lastWifiCheckTime = currentTime;
				return false;
			}
		}
		else 
		{
			lastReconnectTryTime = lastWifiCheckTime = currentTime;
		}

		return true;
	}
}