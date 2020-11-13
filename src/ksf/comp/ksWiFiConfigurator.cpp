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
		deviceName += ESP.getChipId();
	}

	bool ksWiFiConfigurator::init(ksComposable* owner)
	{
		parent = owner;
		return true;
	}

	void ksWiFiConfigurator::postInit()
	{
		std::vector<ksLed*> ledComps;
		parent->findComponents<ksLed>(ledComps);

		for (auto& led : ledComps) {
			led->setEnabled(true);
		}
	}

	bool ksWiFiConfigurator::loop()
	{
		manager.setConfigPortalTimeout(KSF_CAP_PORTAL_TIMEOUT_SECONDS);
		manager.setConnectTimeout(KSF_CAP_WIFI_CONNECT_TIMEOUT);

		std::vector<ksConfigProvider*> configComps;
		parent->findComponents<ksConfigProvider>(configComps);

		for (auto& configComp : configComps) {
			configComp->injectManagerParameters(manager);
		}

		manager.startConfigPortal(deviceName.c_str());

		for (auto& configComp : configComps) {
			configComp->captureManagerParameters(manager);
		}

		return false;
	}
}