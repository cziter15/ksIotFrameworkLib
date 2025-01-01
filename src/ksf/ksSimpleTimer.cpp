/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "Arduino.h"

#include "ksSimpleTimer.h"

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
	
	void ksSimpleTimer::makeExpired()
	{
		if (!hasTimePassed())
			lastTriggerTimeMs -= intervalMs;
	}

	bool ksSimpleTimer::hasTimePassed() const
	{
		return intervalMs && millis() - lastTriggerTimeMs > intervalMs;
	}

	bool ksSimpleTimer::triggered()
	{
		if (hasTimePassed()) 
		{
			restart();
			return true;
		}

		return false;
	}

	void ksSimpleTimer::restart()
	{
		lastTriggerTimeMs = millis();
	}
}