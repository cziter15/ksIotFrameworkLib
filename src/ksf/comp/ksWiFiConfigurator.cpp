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
#include "ksLed.h"

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
		// TODO: Try PGM macro in all instances.
		deviceName += '-';

		#if ESP32
			deviceName += std::to_string(ESP.getEfuseMac());
		#elif ESP8266
			deviceName += std::to_string(ESP.getChipId());
		#else			
			#error Platform not implemented.
		#endif
	}

	bool ksWiFiConfigurator::postInit(ksApplication* owner)
	{
		this->owner = owner;

		std::vector<std::weak_ptr<ksLed>> ledCompsWp;
		owner->findComponents<ksLed>(ledCompsWp);

		for (auto& ledCompWp : ledCompsWp)
			if (auto ledCompSp{ledCompWp.lock()})
				ledCompSp->setEnabled(true);

		return true;
	}

	bool ksWiFiConfigurator::loop()
	{
		manager.setConfigPortalTimeout(KSF_CAP_PORTAL_TIMEOUT_SEC);
		manager.setConnectTimeout(KSF_CAP_WIFI_CONNECT_TIMEOUT_SEC);

		std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
		owner->findComponents<ksConfigProvider>(configCompsWp);

		for (auto& configCompWp : configCompsWp)
			if (auto configCompSp{configCompWp.lock()})
				configCompSp->injectManagerParameters(manager);

		manager.startConfigPortal(deviceName.c_str());

		for (auto& configCompWp : configCompsWp)
			if (auto configCompSp{configCompWp.lock()})
				configCompSp->captureManagerParameters(manager);

		return false;
	}
}