/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#else			
#error Platform not implemented.
#endif
#include <Arduino.h>
#include <Esp.h>

#include "../ksApplication.h"
#include "../ksConstants.h"

#include "ksResetButton.h"

namespace ksf::comps
{
	#define SHORT_TRIGGER 50
	#define LONG_TRIGGER 5000

	ksResetButton::ksResetButton(uint8_t pin, uint8_t triggerState, uint8_t mode)
		: pin(pin), triggerState(triggerState), mode(mode)
	{}

	ksResetButton::~ksResetButton()
	{
		pinMode(pin, INPUT);
	}

	bool ksResetButton::init([[maybe_unused]] ksApplication* app)
	{
		pinMode(pin, mode);
		lastState = static_cast<uint8_t>(digitalRead(pin));
		return true;
	}

	bool ksResetButton::loop([[maybe_unused]] ksApplication* app)
	{
		if (auto currentState{static_cast<uint8_t>(digitalRead(pin))}; currentState != lastState)
		{
			if (currentState == triggerState)
			{
				pressedTime = millis();
			}
			else
			{
				releasedTime = millis();

				if (auto pressDuration{releasedTime - pressedTime}; pressDuration > LONG_TRIGGER)
				{
					WiFi.mode(WIFI_OFF);
					eraseConfigData();
					ESP.restart();
					return false;
				}
				else if (pressDuration > SHORT_TRIGGER)
				{
					return false;
				}
			}

			lastState = currentState;
		}

		return true;
	}
}
