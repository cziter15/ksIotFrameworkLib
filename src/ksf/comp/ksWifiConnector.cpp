/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#if ESP32
	#include <WiFi.h>
	#include <esp_wifi.h>
#elif ESP8266
	#include <user_interface.h>
	#include <ESP8266WiFi.h>
#else 			
	#error Platform not implemented.
#endif

#include "../ksApplication.h"
#include "../ksConstants.h"
#include "ksWifiConnector.h"

namespace ksf::comps
{
	ksWifiConnector::ksWifiConnector(const char* hostname)
	{
#if ESP32
		/* Disable STA mode. */
		WiFi.enableSTA(false);
		/* On ESP32 hostname must be set when not in STA mode. */
		WiFi.setHostname(hostname);
#endif
		WiFi.enableSTA(true);

		setupMacAddress();


#if ESP8266
		/* On ESP8266 hostname must be set when in STA mode. */
		WiFi.setHostname(hostname);
#endif
	}

	void ksWifiConnector::setupMacAddress()
	{
#if defined(ESP32)
		uint32_t chipId = static_cast<uint32_t>(ESP.getEfuseMac());
#elif defined(ESP8266)
		uint32_t chipId = ESP.getChipId();
#else
#error Platform not implemented.
#endif
		uint8_t mac_sta[6] = {
			0xfa, 0xf1, 
			static_cast<uint8_t>(chipId >> 8),
			static_cast<uint8_t>(chipId), 
			static_cast<uint8_t>(chipId >> 16), 
			static_cast<uint8_t>(chipId >> 24)
		};
#if defined(ESP32)
		esp_wifi_set_mac(WIFI_IF_STA, mac_sta);
#elif defined(ESP8266)
		wifi_set_macaddr(STATION_IF, mac_sta);
#else
#error Platform not implemented.
#endif
	}

	bool ksWifiConnector::init(ksApplication* owner)
	{
		std::string ssid, pass;
		ksf::loadCredentials(ssid, pass);

		if (ssid.empty())
			return false;

		WiFi.begin(ssid.c_str(), pass.c_str());

#if ESP32
		WiFi.setSleep(true);
#endif

		configTime(0, 0, "pool.ntp.org", "time.nist.gov");
		
		return true;
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
				WiFi.disconnect(false, false);

				wasConnected = false;
				wifiDisconnectedInternal();
			}

			if (wifiReconnectTimer.hasTimePassed())
			{
				WiFi.begin();
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

	ksWifiConnector::~ksWifiConnector()
	{
		WiFi.disconnect(true, false);
	}
}