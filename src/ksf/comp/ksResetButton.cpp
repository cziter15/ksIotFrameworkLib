/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
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

#include <LittleFS.h>
#include <Esp.h>

#include "../ksApplication.h"
#include "ksResetButton.h"
#include "Arduino.h"

namespace ksf::comps
{
	#define SHORT_TRIGGER 50
	#define LONG_TRIGGER 5000

	ksResetButton::ksResetButton(uint8_t pin, uint8_t triggerState, uint8_t mode)
		: pin(pin), triggerState(triggerState), mode(mode)
	{}

	bool ksResetButton::init(ksApplication* app)
	{
		pinMode(pin, mode);
		lastState = static_cast<uint8_t>(digitalRead(pin));
		return true;
	}

	bool ksResetButton::loop(ksApplication* app)
	{
		if (uint8_t currentState{static_cast<uint8_t>(digitalRead(pin))}; currentState != lastState)
		{
			if (currentState == triggerState)
			{
				pressedTime = millis();
			}
			else if (currentState != triggerState)
			{
				releasedTime = millis();

				if (auto pressDuration{releasedTime - pressedTime}; pressDuration > LONG_TRIGGER)
				{
					WiFi.mode(WIFI_OFF);
					LittleFS.format();
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

	ksResetButton::~ksResetButton()
	{
		pinMode(pin, INPUT);
	}
}
