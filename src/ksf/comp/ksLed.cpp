/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "ksLed.h"
#include "Arduino.h"

namespace ksf::comps
{
	ksLed::ksLed(uint8_t pin) 
		: pin(pin)
	{}

	bool ksLed::init(ksf::ksComposable* owner)
	{
		pinMode(pin, OUTPUT);
		setEnabled(false);
		return true;
	}

	bool ksLed::loop()
	{
		if (blinkIntervalMs > 0)
		{
			if (millis() - lastBlinkTime > blinkIntervalMs)
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
		return digitalRead(pin) == HIGH;
	}

	void ksLed::setBlinking(uint32_t intervalMs, uint32_t numLoops)
	{
		setEnabled(intervalMs > 0);
		blinkIntervalMs = intervalMs;
		lastBlinkTime = millis();
		blinkLoops = numLoops;
	}

	bool ksLed::isBlinking() const
	{
		return blinkIntervalMs > 0;
	}

	void ksLed::setEnabled(bool isEnabled)
	{
		digitalWrite(pin, isEnabled ? HIGH : LOW);
	}

	ksLed::~ksLed()
	{
		setEnabled(false);
		pinMode(pin, INPUT);
	}
}