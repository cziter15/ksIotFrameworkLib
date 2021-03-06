#include "../ksComposable.h"
#include "ksResetButton.h"
#include "Arduino.h"

#ifdef ESP32
#include "SPIFFS.h"
#include "WiFi.h"
#else
#include "FS.h"
#include "Esp8266WiFi.h"
#endif

#include "Esp.h"

namespace ksf
{
	#define SHORT_TRIGGER 50
	#define LONG_TRIGGER 5000

	ksResetButton::ksResetButton(uint8_t pin, uint8_t triggerBy)
		: buttonPin(pin), triggerState(triggerBy)
	{

	}

	bool ksResetButton::init(ksComposable* owner)
	{
		pinMode(buttonPin, INPUT);
		lastState = digitalRead(buttonPin);
		return true;
	}

	bool ksResetButton::loop()
	{
		uint8_t currentState = digitalRead(buttonPin);

		if (currentState != lastState)
		{
			if (currentState == triggerState)
			{
				pressedTime = millis();
			}
			else if (currentState != triggerState)
			{
				releasedTime = millis();

				unsigned long pressDuration = releasedTime - pressedTime;

				if (pressDuration > LONG_TRIGGER)
				{
					WiFi.disconnect(true);
					SPIFFS.format();
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
		pinMode(buttonPin, INPUT);
	}
}
