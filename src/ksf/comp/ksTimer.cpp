/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "ksTimer.h"
#include "Arduino.h"

namespace ksf::comps
{
	ksTimer::ksTimer() {}

	ksTimer::ksTimer(uint32_t interval, bool looping)
	{
		set(interval, looping);
	}

	void ksTimer::unset()
	{
		set(0, false);
	}

	void ksTimer::set(uint32_t interval, bool looping)
	{
		lastTick = millis();
		timerInterval = interval;
		timerLooping = looping;
	}

	bool ksTimer::init(ksf::ksComposable* owner)
	{
		lastTick = millis();
		return true;
	}

	bool ksTimer::loop()
	{
		if (timerInterval > 0)
		{
			if (millis() - lastTick > timerInterval)
			{
				onTimerExpired->broadcast();
				lastTick = millis();

				if (!timerLooping)
					unset();
			}
		}

		return true;
	}
}
