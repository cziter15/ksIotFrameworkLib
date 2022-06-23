/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "../ksConstants.h"
#include "ksWifiConnector.h"

#ifdef ESP32
	#include <WiFi.h>
	#include <esp_wifi.h>
#else
	#include <user_interface.h>
	#include <ESP8266WiFi.h>
#endif

namespace ksf::comps
{
	ksWifiConnector::ksWifiConnector(const char* hostname)
	{
		#ifdef ESP32
			WiFi.setHostname(hostname);
		#else
			WiFi.hostname(hostname);
		#endif
	}

	void ksWifiConnector::setupMacAddress()
	{
		#ifdef ESP32
			uint32_t chipId = (uint32_t)ESP.getEfuseMac();
		#else
			uint32_t chipId = ESP.getChipId();
		#endif

		auto chipIdBytes = (uint8_t*)&chipId;
		uint8_t mac_sta[6] = { 0xfa, 0xf1, chipIdBytes[2], chipIdBytes[1], chipIdBytes[3], chipIdBytes[0] };

		#ifdef ESP32
			esp_wifi_set_mac(WIFI_IF_STA, mac_sta);
		#else
			wifi_set_macaddr(STATION_IF, mac_sta);
		#endif
	}

	bool ksWifiConnector::init(ksf::ksComposable* owner)
	{
		WiFi.mode(WIFI_STA);
		setupMacAddress();
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

		if (!WiFi.localIP().isSet()) // todo: this can be probably optimized
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
