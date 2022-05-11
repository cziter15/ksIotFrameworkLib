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
#include <Arduino.h>

namespace ksf
{
	class ksResetButton : public ksComponent
	{
		protected:
			uint8_t buttonPin = 0;				//< Cached pin number.
			uint8_t triggerState = 0;			//< State that triggers reset logic.
			uint8_t lastState = 0;				//< Previous state (used to debouce).
			uint8_t pmode = INPUT;				//< Cached pin mode.
			uint32_t pressedTime = 0;			//< Press timestamp (ms since boot).
			uint32_t releasedTime = 0;			//< Release timestamp (ms since boot).

		public:
			/*
				Constructor, used to pass reset button configuration parameters.
				@param pin - pin assigned to reset button.
				@param triggerBy - state that triggers reset logic (LOW or HIGH).
				@param mode - pin mode to set (INPUT / INPUT_PULLUP etc).
			*/
			ksResetButton(uint8_t pin, uint8_t triggerBy, uint8_t mode = INPUT);

			/*
				Initializes reset button component.
				@param owner - pointer to owning composable interface (application).
				@return - true on success, false on fail.
			*/
			bool init(class ksComposable* owner) override;

			/*
				Handles reset button component loop logic.
				@return - true on success, false on fail.
			*/
			bool loop() override;

			/*
				Destructor, restores INPUT pin state.
			*/
			virtual ~ksResetButton();
	};
}

