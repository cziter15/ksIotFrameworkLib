/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

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

		ArduinoOTA.onStart([this]() {
			onUpdateStart->broadcast();
		});

		ArduinoOTA.onEnd([this]() {
			ksf::saveOtaBootIndicator();
			onUpdateEnd->broadcast();
		});
	}

	bool ksOtaUpdater::init(ksApplication* owner)
	{
		ArduinoOTA.setHostname(WiFi.getHostname());
		ArduinoOTA.begin();
		return true;
	}

	bool ksOtaUpdater::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

		return true;
	}
}