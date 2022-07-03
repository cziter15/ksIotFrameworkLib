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
		interval = intervalMs;
		restart();
	}

	bool ksSimpleTimer::triggered()
	{
		if (millis() - lastTriggerTime > interval)
		{
			restart();
			return true;
		}

		return false;
	}

	void ksSimpleTimer::restart()
	{
		lastTriggerTime = millis();
	}
}