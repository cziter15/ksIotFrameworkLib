/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include "../evt/ksEvent.h"

namespace ksf
{
	namespace comps
	{
		class ksTimer : public ksf::ksComponent
		{
			private:
				bool timerLooping = false;			//< Current looping value -should loop or not?.
				uint32_t timerInterval = 0;			//< Current timer interval. If equal to zero, callback won't be executed.
				uint32_t lastTick = 0;				//< Last tick timestamp (ms since boot).

			public:
				DECLARE_KS_EVENT(onTimerExpired)	//< onTimerExpired event that the user can bind to.

				/*
					Constructor without initial parameters.
				*/
				ksTimer();

				/*
					Constructor with initial parameters. Calls set method under the hood.
					@param interval - timer initial interval.
					@param looping - timer initial looping.
				*/
				ksTimer(uint32_t interval, bool looping = true);

				/*
					Disables the timer.
				*/
				void unset();

				/*
					Configures timer interval and looping.
					@param interval - timer interval.
					@param looping - timer looping.
				*/
				void set(uint32_t interval, bool looping = true);

				/*
					Initializes timer component.
					@param owner - pointer to owning composable interface (application).
					@return - true on success, false on fail.
				*/
				bool init(ksf::ksComposable* owner) override;

				/*
					Handles timer component loop logic.
					@return - true on success, false on fail.
				*/
				bool loop() override;
		};
	}
}