/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksOtaUpdater.h"
#include "../ksConstants.h"

namespace ksf::comps
{
	ksOtaUpdater::ksOtaUpdater(const char* hostname, const char* password)
	{
		ArduinoOTA.setHostname(hostname);
		ArduinoOTA.setPassword(password);

		ArduinoOTA.onStart([&]() {
			onUpdateStart->broadcast();
		});

		/* Save OTA boot indicator. This will save small file to FS to indicate ota update for next boot. */
		ArduinoOTA.onEnd([&]() {
			ksf::saveOtaBootIndicator();
			onUpdateEnd->broadcast();
		});
	}

	bool ksOtaUpdater::init(class ksf::ksComposable* owner)
	{
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