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

		configTime(0, 0, "pool.ntp.org", "time.nist.gov");

		return true;
	}

	bool ksWifiConnector::loop()
	{
		if (!WiFi.isConnected())
		{
			if (millis() - lastWifiCheckTime > KSF_WIFI_TIMEOUT_MS)
			{
				lastWifiCheckTime = millis();
				return false;
			}
		}

		lastWifiCheckTime = millis();
		return true;
	}
}