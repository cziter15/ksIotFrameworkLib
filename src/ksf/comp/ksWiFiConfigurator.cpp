/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "ksWiFiConfigurator.h"
#include "ksConfigProvider.h"
#include "ksLed.h"
#include "../ksConstants.h"

namespace ksf::comps
{
	ksWiFiConfigurator::ksWiFiConfigurator(const std::string& devicePrefixName) 
		: deviceName(devicePrefixName)
	{
		deviceName += '-';

		#ifdef ESP32
			deviceName += std::to_string(ESP.getEfuseMac());
		#else
			deviceName += std::to_string(ESP.getChipId());
		#endif
	}

	bool ksWiFiConfigurator::init(ksf::ksComposable* owner)
	{
		this->owner = owner;
		return true;
	}

	void ksWiFiConfigurator::postInit()
	{
		std::vector<std::weak_ptr<ksLed>> ledCompsWp;
		owner->findComponents<ksLed>(ledCompsWp);

		for (auto& ledCompWp : ledCompsWp)
			if (auto ledCompSp = ledCompWp.lock())
				ledCompSp->setEnabled(true);
	}

	bool ksWiFiConfigurator::loop()
	{
		manager.setConfigPortalTimeout(KSF_CAP_PORTAL_TIMEOUT_SEC);
		manager.setConnectTimeout(KSF_CAP_WIFI_CONNECT_TIMEOUT_SEC);

		std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
		owner->findComponents<ksConfigProvider>(configCompsWp);

		for (auto& configCompWp : configCompsWp)
			if (auto configCompSp = configCompWp.lock())
				configCompSp->injectManagerParameters(manager);

		manager.startConfigPortal(deviceName.c_str());

		for (auto& configCompWp : configCompsWp)
			if (auto configCompSp = configCompWp.lock())
				configCompSp->captureManagerParameters(manager);

		return false;
	}
}