/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksSimpleTimer.h"
#include "Arduino.h"

namespace ksf 
{
	ksSimpleTimer::ksSimpleTimer(uint32_t intervalMs) 
	{
		setInterval(intervalMs);
	}

	void ksSimpleTimer::setInterval(uint32_t intervalMs)
	{
		this->intervalMs = intervalMs;
		restart();
	}

	bool ksSimpleTimer::hasTimePassed() const
	{
		return millis() - lastTriggerTimeMs > intervalMs;
	}

	bool ksSimpleTimer::triggered()
	{
		if (!hasTimePassed())
			return false;

		restart();
		return true;
	}

	void ksSimpleTimer::restart()
	{
		lastTriggerTimeMs = millis();
	}
}