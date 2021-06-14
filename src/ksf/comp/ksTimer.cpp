#include "../ksComposable.h"
#include "ksTimer.h"
#include "Arduino.h"

namespace ksf
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

	bool ksTimer::init(ksComposable* owner)
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
