/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#if ESP32
	#include <WiFi.h>
#elif ESP8266
	#include <ESP8266WiFi.h>
#else 			
	#error Platform not implemented.
#endif

#include "ksApplication.h"
#include "ksComponent.h"

namespace ksf
{
	bool ksApplication::loop()
	{	
		auto it{components.begin()};

		while (it != components.end()) 
		{
			auto& component{*it};

			switch (component->componentState)
			{
				case ksComponentState::Looping:
				{
					if (!component->loop(this))
					{
						Serial.println("Component loop failed.");
						return false;
					}
				}
				break;

				case ksComponentState::ToBeRemoved:
				{
					 Serial.println("Removing component...");
					 it = components.erase(it);
					 continue;
				}
				break;

				case ksComponentState::NotInitialized:
				{
					Serial.println("Initializing component...");
					if (!component->init(this))
						return false;
					Serial.println("Component initialized.");
					component->componentState = ksComponentState::Initialized;
				}
				break;
				
				case ksComponentState::Initialized:
				{
					Serial.println("Post-Initializing component...");
					if (!component->postInit(this))
						return false;
					Serial.println("Component post initialized.");
					component->componentState = ksComponentState::Looping;
				}
				break;

				default: break;
			}
			++it;
		}

		/* This call will keep millis64 on track (handles rollover). */
		updateDeviceUptime();

		return true;
	}
}