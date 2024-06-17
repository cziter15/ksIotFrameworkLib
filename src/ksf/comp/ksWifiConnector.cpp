/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#if ESP32
	#include <WiFi.h>
	#include <esp_wifi.h>
	#include "lwip/dns.h"
#elif ESP8266
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
	inline void overrideDnsWithGoogleDns()
	{
		/* Define override DNS addresses. */
		static const ip_addr primaryDns{LWIP_MAKEU32(8, 8, 8, 8)};
		static const ip_addr secondaryDns{LWIP_MAKEU32(8, 8, 4, 4)};
		
		/* Set DNS servers. */
		dns_setserver(0, &primaryDns);
		dns_setserver(1, &secondaryDns);
	}

	ksWifiConnector::ksWifiConnector(const char* hostname, bool savePower) 
		: savePower(savePower)
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
		auto chipId{static_cast<uint32_t>(ESP.getEfuseMac())};
#elif defined(ESP8266)
		auto chipId{ESP.getChipId()};
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

	bool ksWifiConnector::init(ksApplication* app)
	{
		std::string ssid, pass;
		ksf::loadCredentials(ssid, pass);

		if (ssid.empty())
			return false;

		WiFi.begin(ssid.c_str(), pass.c_str());
		WiFi.setSleep(savePower);

		configTime(0, 0, "pool.ntp.org", "time.nist.gov");
		
		return true;
	}

	void ksWifiConnector::wifiConnectedInternal()
	{
		/*
			There is a bug in Arduino-esp8266 core which causes domain-based connections to fail in a specific case.
			Easiest way to trigger the issue is to use LTE router and disconnect SIM card for a few seconds.
			This will cause MQTT to disconnect and fail on each reconnect forever untill device reboot.
			Manual DNS override resolves the issue. It might be also good idea to set default gateway as DNS.
		*/
		overrideDnsWithGoogleDns();
		onConnected->broadcast();
	}

	void ksWifiConnector::wifiDisconnectedInternal()
	{
		onDisconnected->broadcast();
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
			gotIpAddress = WiFi.localIP().operator uint32_t() != 0;

		if (!isConnected())
		{
			if (wifiTimeoutTimer.triggered())
				return false;

			if (wasConnected)
			{
				esp_wifi_disconnect();
				wasConnected = false;
				wifiDisconnectedInternal();
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
		WiFi.disconnect(true);
	}
}
