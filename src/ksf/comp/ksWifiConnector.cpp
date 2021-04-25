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
		WiFi.begin();
		WiFi.setAutoReconnect(true);

		#ifdef ESP32
			WiFi.setSleep(WIFI_PS_MAX_MODEM);
		#endif

		configTime(0, 0, "pool.ntp.org", "time.nist.gov");

		lastWifiCheckTime = millis();

		return true;
	}

	bool ksWifiConnector::loop()
	{
		unsigned long currentTime = millis();

		if (!WiFi.isConnected())
		{
			if (currentTime - lastWifiCheckTime > KSF_WIFI_TIMEOUT_MS)
			{
				lastWifiCheckTime = currentTime;
				return false;
			}
		}
		else 
		{
			lastWifiCheckTime = currentTime;
		}

		return true;
	}
}