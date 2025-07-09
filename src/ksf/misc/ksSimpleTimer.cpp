/*
 *	Copyright (c) 2020-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include <Arduino.h>

#include "ksSimpleTimer.h"

namespace ksf::misc
{
	ksSimpleTimer::ksSimpleTimer(uint32_t intervalMs) 
	{
		setInterval(intervalMs);
	}

	ksSimpleTimer::~ksSimpleTimer() = default;

	void ksSimpleTimer::setInterval(uint32_t intervalMs)
	{
		this->intervalMs = intervalMs;
		restart();
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