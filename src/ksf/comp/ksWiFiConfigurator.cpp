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
#include "ksWiFiConfigurator.h"
#include "ksConfigProvider.h"
#include "ksDevicePortal.h"
#include "ksLed.h"
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
	ksWiFiConfigurator::ksWiFiConfigurator()
	{
		ksWiFiConfigurator(PGM_("KSFDevice"));
	}
	
	ksWiFiConfigurator::ksWiFiConfigurator(std::string devicePrefixName)
		 // As the device name is our field, we are able to move, it's better to do so than using const ref
		: deviceName(std::move(devicePrefixName))
	{
		deviceName += '-';

#if ESP32
		deviceName += std::to_string(ESP.getEfuseMac());
#elif ESP8266
		deviceName += std::to_string(ESP.getChipId());
#else			
		#error Platform not implemented.
#endif
	}

	bool ksWiFiConfigurator::init(ksApplication* owner)
	{
		this->owner = owner;

		WiFi.softAP(deviceName.c_str());
		owner->addComponent<ksDevicePortal>().lock()->init(owner);

		return true;
	}

	bool ksWiFiConfigurator::postInit(ksApplication* owner)
	{
		std::vector<std::weak_ptr<ksLed>> ledCompsWp;
		owner->findComponents<ksLed>(ledCompsWp);

		for (auto& ledCompWp : ledCompsWp)
			if (auto ledCompSp{ledCompWp.lock()})
				ledCompSp->setEnabled(true);

		return true;
	}

	bool ksWiFiConfigurator::loop()
	{
		return !configTimeout.triggered();
	}

	ksWiFiConfigurator::~ksWiFiConfigurator()
	{
		WiFi.softAPdisconnect(true);
		WiFi.mode(WIFI_OFF);
	}
}