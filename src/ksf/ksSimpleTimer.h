/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <stdint.h>

namespace ksf 
{
	class ksSimpleTimer
	{
		protected:
			uint32_t intervalMs{0};				// Timer interval (milliseconds).
			uint32_t lastTriggerTimeMs{0};		// Last trigger time (milliseconds since boot).

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
				Checks if timer interval just passed and restarts the timer if so.

				@return True if timer just triggered, otherwise false.
			*/
			bool triggered();
			
			/*
				Only checks if timer interval just passed. Restarting timer relies on user.

				@return True if timer just triggered, otherwise false.
			*/
			bool hasTimePassed() const;
	};
}
