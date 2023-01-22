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
	#include <ESPmDNS.h>
#elif ESP8266
	#include <ESP8266WiFi.h>
	#include <ESP8266mDNS.h>
#else 			
	#error Platform not implemented.
#endif

#include "../ksApplication.h"
#include "../ksConstants.h"
#include "ksWifiConnector.h"
#include "ksOtaUpdater.h"

namespace ksf::comps
{
	ksOtaUpdater::ksOtaUpdater()
		: ksOtaUpdater(PGM_("ota_ksiotframework"))
	{}

	ksOtaUpdater::ksOtaUpdater(const std::string& password)
	{
		ArduinoOTA.setPassword(password.c_str());

		ArduinoOTA.onStart([&]() {
			onUpdateStart->broadcast();
		});

		ArduinoOTA.onEnd([&]() {
			ksf::saveOtaBootIndicator();
			onUpdateEnd->broadcast();
		});
	}

	bool ksOtaUpdater::init(ksApplication* owner)
	{
		/* Bind to WiFi events. */
		if (auto wifiConnSp{owner->findComponent<ksWifiConnector>().lock()})
		{
			wifiConnSp->onConnected->registerEvent(wifiConnEventHandleSp, std::bind(&ksOtaUpdater::onWifiConnected, this));
			wifiConnSp->onDisconnected->registerEvent(wifiDisconnEventHandleSp, std::bind(&ksOtaUpdater::onWifiDisconnected, this));		
		}

		/* Initialize Arduino OTA. */
		#if ESP8266
			ArduinoOTA.begin(false);
		#elif ESP32
			ArduinoOTA.setMdnsEnabled(false);
			ArduinoOTA.begin();
		#else 
			#error Platform not implemented.
		#endif

		return true;
	}

	void ksOtaUpdater::onWifiConnected()
	{
		MDNS.begin(WiFi.getHostname());
	}

	void ksOtaUpdater::onWifiDisconnected()
	{
		MDNS.end();
	}

	bool ksOtaUpdater::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

		/* Handle MDNS stuff. */
		#if ESP8266
			if (MDNS.isRunning())
				MDNS.update();
		#endif

		return true;
	}
}