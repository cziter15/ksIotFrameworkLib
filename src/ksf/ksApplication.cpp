/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksApplication.h"
#include "ksComponent.h"

#include "comp/ksWiFiConfigurator.h"
#include "comp/ksWifiConnector.h"

#if ESP32
	#include <WiFi.h>
#elif ESP8266
	#include <ESP8266WiFi.h>
#else 			
	#error Platform not implemented.
#endif
namespace ksf
{
	void ksApplication::markComponentToRemove(const std::shared_ptr<ksComponent> component)
	{
		components.remove(std::move(component));
	}

	void ksApplication::cleanupComponentsInProperOrder()
	{
		std::vector<std::shared_ptr<ksComponent>> componentsToDeleteAtTheEnd;

		for (const auto& comp : components.getRef())
		{
			bool shouldRemoveAtTheEnd {
				comp->isA(comps::ksWiFiConfigurator::getClassType() ||
				comp->isA(comps::ksWifiConnector::getClassType()))
			};

			if (shouldRemoveAtTheEnd)
				componentsToDeleteAtTheEnd.emplace_back(std::move(comp));
		}

		components.clearAll();
	}

	bool ksApplication::init()
	{
		auto initFunc = [this](const std::shared_ptr<ksComponent>& comp) { return comp->init(this); };
		auto postInitFunc = [this](const std::shared_ptr<ksComponent>& comp) { return comp->postInit(this); };
		auto cleanupComponentsAndReturnFalse = [this]() { cleanupComponentsInProperOrder(); return false; };

		components.applyPendingOperations();

		if (!forEachComponent(initFunc))
			return cleanupComponentsAndReturnFalse();

		components.applyPendingOperations();

		if (!forEachComponent(postInitFunc))
			return cleanupComponentsAndReturnFalse();

		return true;
	}

	bool ksApplication::loop()
	{
		/* A bool indicating if we should continue loop. */
		bool continueLoop{true};

		/* Loop through all components and synchronize list at end of scope. */
		{
			ksf::ksSafeListScopedSync{components};
			if (!forEachComponent([](const std::shared_ptr<ksComponent>& comp) { return comp->loop(); } ))
				continueLoop = false;
		}

		/* This call will keep millis64 on track (handles rollover). */
		updateDeviceUptime();

		/* If app has been marked to stop, exit loop. */
		if (!continueLoop)
			cleanupComponentsInProperOrder();

		return continueLoop;
	}
}