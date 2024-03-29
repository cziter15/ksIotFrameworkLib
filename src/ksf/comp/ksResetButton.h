/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
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
				@brief Constructs  reset button component.
				@param pin PIN number to use
				@param triggerState State that triggers reset logic (LOW or HIGH)
				@param mode Pin mode to set (INPUT / INPUT_PULLUP etc)
			*/
			ksResetButton(uint8_t pin, uint8_t triggerState, uint8_t mode = INPUT);

			/*!
				@brief Initializes reset button component.
				@param app Pointer to ksApplication object that owns this component
				@return True on success, false on fail.
			*/
			bool init(ksApplication* app) override;

			/*!
				@brief Handles reset button component loop logic.
				@return True on success, false on fail.
			*/
			bool loop(ksApplication* app) override;

			/*!
				@brief Retrieves assigned pin number.
				@return PIN number assigned to the reset button.
			*/
			uint8_t getPin() const { return pin; }

			/*!
				@brief Destructs ksResetButton, restoring INPUT pin state.
			*/
			virtual ~ksResetButton();
	};
}