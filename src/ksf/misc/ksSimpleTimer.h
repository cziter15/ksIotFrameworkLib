/*
 *	Copyright (c) 2021-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <stdint.h>

namespace ksf::misc
{
	/*!
		@brief Simple timer class, without using component architecture.

		Used to trigger events periodically. Internally checks if timer interval just passed.
		When interval is 0, never triggers the timer.

		There are two ways to use it:
		- using ksSimpleTimer::triggered() method, that returns true if timer interval just passed and updates last trigger time.
		- using ksSimpleTimer::hasTimePassed() method, that returns true if timer interval just passed, without updating last trigger time.
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
