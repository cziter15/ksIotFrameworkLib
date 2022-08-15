/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include <Arduino.h>

namespace ksf
{
	namespace comps
	{
		class ksResetButton : public ksf::ksComponent
		{
			protected:
				uint8_t pin = 0;					// Cached pin number.
				uint8_t triggerState = 0;			// State that triggers reset logic.
				uint8_t lastState = 0;				// Previous state (used to debouce).
				uint8_t mode = INPUT;				// Cached pin mode.
				uint32_t pressedTime = 0;			// Press timestamp (milliseconds since boot).
				uint32_t releasedTime = 0;			// Release timestamp (milliseconds since boot).

			public:
				/*
					Constructor, used to pass reset button configuration parameters.

					@param pin Pin assigned to reset button.
					@param triggerState State that triggers reset logic (LOW or HIGH).
					@param mode Pin mode to set (INPUT / INPUT_PULLUP etc).
				*/
				ksResetButton(uint8_t pin, uint8_t triggerState, uint8_t mode = INPUT);

				/*
					Initializes reset button component.

					@param owner Pointer to ownning ksComposable object (application).
					@return True on success, false on fail.
				*/
				bool init(class ksf::ksComposable* owner) override;

				/*
					Handles reset button component loop logic.
					@return True on success, false on fail.
				*/
				bool loop() override;

				/*
					PIN number getter.
					@return PIN assigned to the reset button.
				*/
				uint8_t getPin() const { return pin; }

				/*
					Destructs ksResetButton, restoring INPUT pin state.
				*/
				virtual ~ksResetButton();
		};
	}
}