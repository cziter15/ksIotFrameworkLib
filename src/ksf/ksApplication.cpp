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

	bool ksApplication::init()
	{
		auto initFunc = [this](const std::shared_ptr<ksComponent>& comp) { return comp->init(this); };
		auto postInitFunc = [this](const std::shared_ptr<ksComponent>& comp) { return comp->postInit(this); };

		components.applyPendingOperations();

		if (!forEachComponent(initFunc))
			return false;

		components.applyPendingOperations();

		if (!forEachComponent(postInitFunc))
			return false;

		return true;
	}

	bool ksApplication::loop()
	{
		/* Loop through all components and synchronize list at end of scope. */
		{
			ksf::ksSafeListScopedSync{components};
			if (!forEachComponent([](const std::shared_ptr<ksComponent>& comp) { return comp->loop(); } ))
				return false;
		}

		/* This call will keep millis64 on track (handles rollover). */
		updateDeviceUptime();

		return true;
	}
	
	ksApplication::~ksApplication()
	{
		WiFi.softAPdisconnect();
		WiFi.disconnect(true, false);
		WiFi.mode(WIFI_OFF);
	}
}