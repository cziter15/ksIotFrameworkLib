/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <stdint.h>

namespace ksf::misc
{
	/*!
		@brief A simple timer class that does not rely on component architecture.

		Used to trigger events periodically by internally checking if the timer interval has passed. 
		If the interval is set to 0, the timer will never trigger.

		There are two ways to use this timer:
		- By using the ksSimpleTimer::triggered() method, which returns true if the timer interval has just passed and updates the last trigger time.
		- By using the ksSimpleTimer::hasTimePassed() method, which returns true if the timer interval has just passed without updating the last trigger time.
	*/
	class ksSimpleTimer
	{
		protected:
			uint32_t intervalMs{0};				//!< Timer interval (milliseconds).
			uint32_t lastTriggerTimeMs{0};		//!< Last trigger time (milliseconds).

		public:
			/*!
				@brief Constructs timer with interval (milliseconds).
				@param intervalMs Initial timer interval in milliseconds.
			*/
			ksSimpleTimer(uint32_t intervalMs);

			/*!
				@brief Destructs timer.
			*/
			virtual ~ksSimpleTimer();

			/*!
				@brief Sets timer interval (milliseconds) and restarts the timer.
				@param intervalMs Timer interval in milliseconds. If 0 then timer is disabled.
			*/
			void setInterval(uint32_t intervalMs);

			/*!
				@brief Restarts timer. Will set last trigger time to current time w/o triggering the timer).
			*/
			void restart();

			/*!
				@brief Checks if timer interval just passed and resets the timer in this case.
				If timer interval is 0 then always returns false.
				@return True if timer just triggered, otherwise false.
			*/
			bool triggered();

			/*!
				@brief Checks if timer interval just passed. Restarting timer relies on user.
				If timer interval is 0 then always returns false.
				@return	True if timer interval just passed, otherwise false.
			*/
			bool hasTimePassed() const;
	};
}
