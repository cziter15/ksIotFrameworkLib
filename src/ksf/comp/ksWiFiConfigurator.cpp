/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "ksWiFiConfigurator.h"
#include "ksConfigProvider.h"
#include "ksLed.h"
#include "../ksConstants.h"

namespace ksf
{
	ksWiFiConfigurator::ksWiFiConfigurator(const String& devicePrefixName) : deviceName(devicePrefixName)
	{
		deviceName += '-';

#ifdef ESP32
		deviceName += (uint32_t)ESP.getEfuseMac();
#else
		deviceName += ESP.getChipId();
#endif
	}

	bool ksWiFiConfigurator::init(ksComposable* owner)
	{
		parent = owner;
		return true;
	}

	void ksWiFiConfigurator::postInit()
	{
		std::vector<std::weak_ptr<ksLed>> ledComps_wp;
		parent->findComponents<ksLed>(ledComps_wp);

		for (auto& ledComp_wp : ledComps_wp)
		{
			if (auto ledComp_sp = ledComp_wp.lock())
				ledComp_sp->setEnabled(true);
		}
	}

	bool ksWiFiConfigurator::loop()
	{
		manager.setConfigPortalTimeout(KSF_CAP_PORTAL_TIMEOUT_SECONDS);
		manager.setConnectTimeout(KSF_CAP_WIFI_CONNECT_TIMEOUT);

		std::vector<std::weak_ptr<ksConfigProvider>> configComps_wp;
		parent->findComponents<ksConfigProvider>(configComps_wp);

		for (auto& configComp_wp : configComps_wp) {
			if (auto configComp_sp = configComp_wp.lock())
				configComp_sp->injectManagerParameters(manager);
		}

		manager.startConfigPortal(deviceName.c_str());

		for (auto& configComp_wp : configComps_wp) {
			if (auto configComp_sp = configComp_wp.lock())
				configComp_sp->captureManagerParameters(manager);
		}

		return false;
	}
}