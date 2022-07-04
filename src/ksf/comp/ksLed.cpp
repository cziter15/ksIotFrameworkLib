/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "ksLed.h"
#include "Arduino.h"

namespace ksf::comps
{
	ksLed::ksLed(uint8_t pin) : ledPin(pin)
	{}

	bool ksLed::init(ksf::ksComposable* owner)
	{
		pinMode(ledPin, OUTPUT);
		setEnabled(false);
		return true;
	}

	bool ksLed::loop()
	{
		if (blinkInterval > 0)
		{
			if (millis() - lastBlinkTime > blinkInterval)
			{
				setEnabled(!isEnabled());
				lastBlinkTime = millis();

				if (blinkLoops > 0 && --blinkLoops == 0)
					setBlinking(0);
			}
		}

		return true;
	}

	bool ksLed::isEnabled() const
	{
		return digitalRead(ledPin) == HIGH;
	}

	void ksLed::setBlinking(uint32_t intervalMs, uint32_t numLoops)
	{
		setEnabled(intervalMs > 0);
		blinkInterval = intervalMs;
		lastBlinkTime = millis();
		blinkLoops = numLoops;
	}

	bool ksLed::isBlinking() const
	{
		return blinkInterval > 0;
	}

	void ksLed::setEnabled(bool isEnabled)
	{
		digitalWrite(ledPin, isEnabled ? HIGH : LOW);
	}

	ksLed::~ksLed()
	{
		setEnabled(false);
		pinMode(ledPin, INPUT);
	}
}