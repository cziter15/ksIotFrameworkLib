/*
 *	Copyright (c) 2020-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#if defined(ESP32)
	#include <WiFi.h>
	#include <esp_wifi.h>
#elif defined(ESP8266)
	#include <user_interface.h>
	#include <ESP8266WiFi.h>
	#define esp_wifi_disconnect() wifi_station_disconnect()
	#define esp_wifi_connect() wifi_station_connect()
#else 			
	#error Platform not implemented.
#endif

#include "../ksApplication.h"
#include "../ksConstants.h"

#include "ksWifiConnector.h"

namespace ksf::comps
{
	ksWifiConnector::ksWifiConnector(const char* hostname, bool savePower)
		: wifiTimeoutTimer{KSF_WIFI_TIMEOUT_MS}, wifiReconnectTimer{KSF_WIFI_RECONNECT_TIME_MS}, wifiIpCheckTimer{KSF_ONE_SEC_MS}
	{
		bitflags.savePower = savePower;
#if defined(ESP32)
		/* Disable STA mode. */
		WiFi.enableSTA(false);
		/* On ESP32 hostname must be set when not in STA mode. */
		WiFi.setHostname(hostname);
#endif
		WiFi.enableSTA(true);
		setupMacAddress();
#if defined(ESP8266)
		/* On ESP8266 hostname must be set when in STA mode. */
		WiFi.setHostname(hostname);
#endif
	}

	ksWifiConnector::~ksWifiConnector()
	{
		WiFi.disconnect(true);
	}

	void ksWifiConnector::setupMacAddress()
	{
#if defined(ESP32)
		auto chipId{static_cast<uint32_t>(ESP.getEfuseMac())};
#elif defined(ESP8266)
		auto chipId{ESP.getChipId()};
#else
		#error Platform not implemented.
#endif
		const uint8_t mac_sta[6] = {
			0xfa, 0xf1, 
			static_cast<uint8_t>(chipId >> 8),
			static_cast<uint8_t>(chipId), 
			static_cast<uint8_t>(chipId >> 16), 
			static_cast<uint8_t>(chipId >> 24)
		};
#if defined(ESP32)
		esp_wifi_set_mac(WIFI_IF_STA, const_cast<uint8_t*>(mac_sta));
#elif defined(ESP8266)
		wifi_set_macaddr(STATION_IF, const_cast<uint8_t*>(mac_sta));
#else
		#error Platform not implemented.
#endif
	}

	bool ksWifiConnector::init(ksApplication* app)
	{
		std::string ssid, pass;
		ksf::loadCredentials(ssid, pass);

		if (ssid.empty())
			return false;

		WiFi.begin(ssid.c_str(), pass.c_str());
		WiFi.setSleep(bitflags.savePower);

		configTime(0, 0, "pool.ntp.org", "time.nist.gov");
		
		return true;
	}

	bool ksWifiConnector::loop(ksApplication* app)
	{
		/*
			It's good to have manual timer for checking if IP address is set. 
			Sometimes the connection status is not updated as fast as it should.
			It occurred for me on ESP32S3 when using "FunBox" as a router. Then after disabling WiFi and
			re-enabling it, the device was not able to reconnect to the router.
		*/
		if (wifiIpCheckTimer.triggered())
			bitflags.gotIpAddress = WiFi.localIP().operator uint32_t() != 0;

		if (!isConnected())
		{
			if (wifiTimeoutTimer.triggered())
				return false;

			if (bitflags.wasConnected)
			{
				esp_wifi_disconnect();
				bitflags.wasConnected = false;
				onDisconnected->broadcast();
			}

			if (wifiReconnectTimer.hasTimePassed())
			{
				esp_wifi_connect();
				wifiReconnectTimer.restart();
			}
		}
		else
		{
			wifiReconnectTimer.restart();
			wifiTimeoutTimer.restart();

			if (!bitflags.wasConnected)
			{
				bitflags.wasConnected = true;
				onConnected->broadcast();
			}
		}

		return true;
	}

	bool ksWifiConnector::isConnected() const
	{
		return WiFi.isConnected() && bitflags.gotIpAddress;
	}
}
