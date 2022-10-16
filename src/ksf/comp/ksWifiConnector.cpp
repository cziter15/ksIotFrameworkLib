/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "../ksConstants.h"
#include "ksWifiConnector.h"

#if ESP32
	#include <WiFi.h>
	#include <esp_wifi.h>
#elif ESP8266
	#include <user_interface.h>
	#include <ESP8266WiFi.h>
#else 			
	#error Platform not implemented.
#endif

namespace ksf::comps
{
	ksWifiConnector::ksWifiConnector(const char* hostname)
	{
		#if ESP32
			/* On ESP32 setting hostname works only outside STA mode. */
			WiFi.mode(WIFI_OFF);
			WiFi.setHostname(hostname);
			WiFi.mode(WIFI_STA);
		#elif ESP8266
			/* On ESP8266 setting hostname works only in STA mode. */
			WiFi.mode(WIFI_STA);
			WiFi.setHostname(hostname);
		#else
			#error Platform not implemented.
		#endif

		setupMacAddress();
		WiFi.setAutoConnect(false);
		WiFi.setAutoReconnect(false);
	}

	void ksWifiConnector::setupMacAddress()
	{
		#if ESP32
			uint32_t chipId = static_cast<uint32_t>(ESP.getEfuseMac());
		#elif ESP8266
			uint32_t chipId = ESP.getChipId();
		#else
			#error Platform not implemented.
		#endif

		auto chipIdBytes = reinterpret_cast<uint8_t*>(&chipId);
		uint8_t mac_sta[6] = { 0xfa, 0xf1, chipIdBytes[2], chipIdBytes[1], chipIdBytes[3], chipIdBytes[0] };

		#if ESP32
			esp_wifi_set_mac(WIFI_IF_STA, mac_sta);
		#elif ESP8266
			wifi_set_macaddr(STATION_IF, mac_sta);
		#else
			#error Platform not implemented.
		#endif
	}

	bool ksWifiConnector::init(ksf::ksComposable* owner)
	{
		WiFi.begin();

		#if ESP32
			WiFi.setSleep(true);
		#endif

		configTime(0, 0, "pool.ntp.org", "time.nist.gov");
		
		return true;
	}

	bool ksWifiConnector::connectStation()
	{
		#if ESP32
			return esp_wifi_connect() == ESP_OK;
		#elif ESP8266
			return wifi_station_connect();
		#else
			#error Platform not implemented.
		#endif
	}

	bool ksWifiConnector::disconnectStation()
	{
		#if ESP32
			return esp_wifi_disconnect() == ESP_OK;
		#elif ESP8266
			return wifi_station_disconnect();
		#else
			#error Platform not implemented.
		#endif

		return false;
	}

	void ksWifiConnector::wifiConnectedInternal()
	{
		onConnected->broadcast();
	}

	void ksWifiConnector::wifiDisconnectedInternal()
	{
		onDisconnected->broadcast();
	}

	bool ksWifiConnector::loop()
	{
		if (wifiIpCheckTimer.triggered())
			gotIpAddress = WiFi.localIP().operator uint32_t() != 0;

		if (!isConnected())
		{
			if (wifiTimeoutTimer.triggered())
				return false;

			if (wasConnected)
			{
				/*
					Enforce WiFi disconnect when IP lost. 
					Unable to call WiFi.disconnect() here due to issue on arduino-esp8266
					See https://github.com/esp8266/Arduino/issues/8689 for reference.
				*/
				disconnectStation();

				wasConnected = false;
				wifiDisconnectedInternal();
			}

			if (wifiReconnectTimer.hasTimePassed())
			{
				/* Enforce WiFi full reconnect flow here. */
				disconnectStation();
				connectStation();

				wifiReconnectTimer.restart();
			}
		}
		else
		{
			wifiReconnectTimer.restart();
			wifiTimeoutTimer.restart();

			if (!wasConnected)
			{
				wasConnected = true;
				wifiConnectedInternal();
			}
		}

		return true;
	}

	bool ksWifiConnector::isConnected() const
	{
		return WiFi.isConnected() && gotIpAddress;
	}
}