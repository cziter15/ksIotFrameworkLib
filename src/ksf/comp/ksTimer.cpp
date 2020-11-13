#include "../ksComposable.h"
#include "ksTimer.h"
#include "Arduino.h"

namespace ksf
{
	ksTimer::ksTimer() {}

	ksTimer::ksTimer(unsigned int interval, bool looping)
	{
		set(interval, looping);
	}

	void ksTimer::unset()
	{
		set(0, false);
	}

	void ksTimer::set(unsigned int interval, bool looping)
	{
		last_tick = millis();
		timer_inteval = interval;
		timer_looping = looping;
	}

	bool ksTimer::init(ksComposable* owner)
	{
		last_tick = millis();
		return true;
	}

	bool ksTimer::loop()
	{
		if (timer_inteval > 0)
		{
			if (millis() - last_tick > timer_inteval)
			{
				onTimerExpired.broadcast();
				last_tick = millis();

				if (!timer_looping)
					unset();
			}
		}

		return true;
	}
}
