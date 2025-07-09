/*
 *	Copyright (c) 2020-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <Arduino.h>

#include "../ksComponent.h"

namespace ksf::comps
{
	/*!
		@brief A component that implements reset button functionality.

		On a short press, the component loop returns false, prompting ksAppRotator to switch to the next application. 
		On a long press, it performs a factory reset by erasing user data.

		The component also includes built-in debounce handling.
	*/
	class ksResetButton : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksResetButton, ksComponent)

		protected:
			uint8_t pin{0};						//!< Cached pin number.
			uint8_t triggerState{0};			//!< State that triggers reset logic.
			uint8_t lastState{0};				//!< Previous state (for debouncing).
			uint8_t mode{INPUT};				//!< Cached pin mode.
			uint32_t pressedTime{0};			//!< Press timestamp (milliseconds since boot).
			uint32_t releasedTime{0};			//!< Release timestamp (milliseconds since boot).

		public:
			/*!
				@brief Constructs the reset button component.
				@param pin Pin number assigned to the button.
				@param triggerState Pin state that triggers the reset logic (LOW or HIGH).
				@param mode Pin mode (INPUT / INPUT_PULLUP etc).
			*/
			ksResetButton(uint8_t pin, uint8_t triggerState, uint8_t mode);

			/*!
				@brief Destructs ksResetButton, restoring INPUT pin state.
			*/
			virtual ~ksResetButton();

			/*!
				@brief Initializes the reset button component.
				@param app Pointer to the parent ksApplication.
				@return True on success, false otherwise.
			*/
			bool init(ksApplication* app) override;

			/*!
				@brief Handles the reset button component logic.
			 	@param app Pointer to the parent ksApplication.
				@return True on success, false otherwise.
			*/
			bool loop(ksApplication* app) override;

			/*!
				@brief Retrieves pin number assigned to the button.
				@return Pin number assigned to the reset button.
			*/
			uint8_t getPin() const { return pin; }
	};
}
