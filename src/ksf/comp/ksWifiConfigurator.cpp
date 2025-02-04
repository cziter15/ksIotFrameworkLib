/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
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
#else 			
	#error Platform not implemented.
#endif

#include "../ksApplication.h"
#include "../ksConstants.h"
#include "ksWifiConfigurator.h"
#include "ksConfigProvider.h"
#include "ksDevicePortal.h"
#include "ksLed.h"

namespace ksf::comps
{
	ksWifiConfigurator::ksWifiConfigurator()
	{
		ksWifiConfigurator(PSTR("KSFDevice"));
	}

	ksWifiConfigurator::ksWifiConfigurator(std::string devicePrefixName)
		 // As the device name is our field, we are able to move, it's better to do so than using const ref
		: deviceName(std::move(devicePrefixName))
	{
		deviceName += '-';
		deviceName += ksf::getDeviceUuidHex();
	}


	bool ksWifiConfigurator::init(ksApplication* app)
	{
		WiFi.softAP(deviceName.c_str());
		app->addComponent<ksDevicePortal>();
		return true;
	}

	bool ksWifiConfigurator::postInit(ksApplication* app)
	{
		std::vector<std::weak_ptr<ksLed>> ledCompsWp;
		app->findComponents<ksLed>(ledCompsWp);

		for (auto& ledCompWp : ledCompsWp)
			if (auto ledCompSp{ledCompWp.lock()})
				ledCompSp->setEnabled(true);

		return true;
	}

	void ksWifiConfigurator::handlePeriodicTasks()
	{
		if (WiFi.softAPgetStationNum())
			configTimeout.restart();
	}

	bool ksWifiConfigurator::loop(ksApplication* app)
	{
		if (periodicTasksTimeout.triggered())
			handlePeriodicTasks();

		return !configTimeout.triggered();
	}

	ksWifiConfigurator::~ksWifiConfigurator()
	{
		WiFi.softAPdisconnect(true);
	}
}
