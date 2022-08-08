/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <inttypes.h>

namespace ksf 
{
	class ksSimpleTimer
	{
		protected:
			uint32_t interval = 0;
			uint32_t lastTriggerTime = 0;

		public:
			/*
				Constructs ksSimpleTimer.
				@param intervalMs - Initial timer interval in milliseconds.
			*/
			ksSimpleTimer(uint32_t intervalMs);

			/*
				Sets new timer interval.
				@param intervalMs - Initial timer interval in milliseconds.
			*/
			void setInterval(uint32_t intervalMs);

			/*
				Restarts the timer (sets last trigger time to current time w/o triggering the timer).
			*/
			void restart();

			/*
				Checks if timer interval just passed.
				@return True if timer just triggered, otherwise false.
			*/	
			bool triggered();
	};
}