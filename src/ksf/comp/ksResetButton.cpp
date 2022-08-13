/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "ksResetButton.h"
#include "Arduino.h"

#ifdef ESP32
	#include <WiFi.h>
#else
	#include <ESP8266WiFi.h>
#endif

#include <LittleFS.h>
#include <Esp.h>

namespace ksf::comps
{
	#define SHORT_TRIGGER 50
	#define LONG_TRIGGER 5000

	ksResetButton::ksResetButton(uint8_t pin, uint8_t triggerState, uint8_t mode)
		: pin(pin), triggerState(triggerState), mode(mode)
	{}

	bool ksResetButton::init(ksf::ksComposable* owner)
	{
		pinMode(pin, mode);
		lastState = digitalRead(pin);
		return true;
	}

	bool ksResetButton::loop()
	{
		uint8_t currentState = digitalRead(pin);

		if (currentState != lastState)
		{
			if (currentState == triggerState)
			{
				pressedTime = millis();
			}
			else if (currentState != triggerState)
			{
				releasedTime = millis();

				uint32_t pressDuration = releasedTime - pressedTime;

				if (pressDuration > LONG_TRIGGER)
				{
					WiFi.disconnect(true);
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
