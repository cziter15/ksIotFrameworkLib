/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
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
			uint32_t lastTriggerTimeMs{0};		// Last trigger time (milliseconds).

		public:
			/*
				Constructs timer with interval (milliseconds).
				
				@param intervalMs Initial timer interval in milliseconds.
			*/
			ksSimpleTimer(uint32_t intervalMs);

			/*
				Sets timer interval (milliseconds).
				
				@param intervalMs Initial timer interval in milliseconds.
			*/
			void setInterval(uint32_t intervalMs);

			/*
				Restarts timer. Will set last trigger time to current time w/o triggering the timer).
			*/
			void restart();

			/*
				Checks if timer interval just passed. Restarting timer is automatic.

				@return True if timer just triggered, otherwise false.
			*/
			bool triggered();
			
			/*
				Checks if timer interval just passed. Restarting timer relies on user.

				@return	True if timer interval just passed, otherwise false.
			*/
			bool hasTimePassed() const;
	};
}
