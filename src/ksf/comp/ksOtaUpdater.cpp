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
#elif ESP8266
	#include <ESP8266WiFi.h>
#else 			
	#error Platform not implemented.
#endif

#include "../ksApplication.h"
#include "../ksConstants.h"
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

	void ksOtaUpdater::postInit()
	{
		ArduinoOTA.setHostname(WiFi.getHostname()); // TODO: To consider, maybe it should go through WiFiConnector component.
		ArduinoOTA.begin();
	}

	bool ksOtaUpdater::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();
		
		return true;
	}
}