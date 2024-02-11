/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "Arduino.h"
#include "../ksApplication.h"
#include "ksLed.h"

namespace ksf::comps
{
	ksLed::ksLed(uint8_t pin, bool driveAsActiveLow, bool driveAsPushPull) 
		: pin(pin), driveAsActiveLow(driveAsActiveLow), driveAsPushPull(driveAsPushPull)
	{}

	bool ksLed::init(ksApplication* owner)
	{
		if (!driveAsPushPull)
			pinMode(pin, OUTPUT);

		setEnabled(false);
		return true;
	}

	bool ksLed::loop(ksApplication* app)
	{
		/* If interval is set, check if time interval passed and we should toggle LED state. */
		if (blinkIntervalMs > 0 && (millis() - lastBlinkTimeMs > blinkIntervalMs))
		{
			setEnabled(!isEnabled());

			if (blinkLoops > 0 && --blinkLoops == 0)
				setBlinking(0);
			else
				lastBlinkTimeMs = millis();
		}

		return true;
	}

	bool ksLed::isEnabled() const
	{
		return digitalRead(pin) == (driveAsActiveLow ? LOW : HIGH);
	}

	void ksLed::setBlinking(uint32_t blinkIntervalMs, uint32_t blinkLoops)
	{
		setEnabled(blinkIntervalMs > 0);
		this->blinkIntervalMs = blinkIntervalMs;
		this->blinkLoops = blinkLoops;
		lastBlinkTimeMs = millis();
	}

	bool ksLed::isBlinking() const
	{
		return blinkIntervalMs > 0;
	}

	void ksLed::setDriveAsPushPull(bool driveAsPushPull)
	{
		bool wasEnabled{isEnabled()};
		this->driveAsPushPull = driveAsPushPull;
		if (!driveAsPushPull)
			pinMode(pin, OUTPUT);
		setEnabled(wasEnabled);
	}

	void ksLed::setEnabled(bool enabled)
	{
		if (driveAsPushPull)
			pinMode(pin, (driveAsActiveLow ? !enabled : enabled) ? INPUT_PULLUP : INPUT_PULLDOWN);
		else
			digitalWrite(pin, (driveAsActiveLow ? !enabled : enabled) ? HIGH : LOW);
	}

	ksLed::~ksLed()
	{
		setEnabled(false);
		pinMode(pin, INPUT);
	}
}