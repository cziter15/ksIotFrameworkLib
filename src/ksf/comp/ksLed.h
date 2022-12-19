/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <Arduino.h>
#include "../ksComponent.h"

namespace ksf::comps
{
	class ksLed : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksLed, ksComponent)

		protected:
			uint8_t pin{0};						// Pin number assigned to LED.
			bool activeLow{false};				// True if pin should be driven LOW to enable LED. Otherwise false.
			uint32_t lastBlinkTimeMs{0};		// Last time LED state was toggled (milliseconds).
			uint32_t blinkIntervalMs{0};		// Time in ms between LED state toggle (0 to disable blinking).
			uint32_t blinkLoops{0};				// Number of loops (0 for infinite loop).

		public:
			/*
				Constructs LED object, assigning passed pin number.

				@param pin Pin number assigned to LED.
				@param activeLow True if pin should be driven LOW to enable LED. Otherwise false.
			*/
			ksLed(uint8_t pin, bool activeLow = false);

			/*
				Initializes LED component.
				
				@param owner Pointer to ksApplication object that owns this component.
				@return True if init succedeed, otherwise false.
			*/
			bool init(ksApplication* owner) override;

			/*
				Executes LED component logic.

				@return True if everything is okay, otherwise false to break application logic.
			*/
			bool loop() override;

			/*
				Sets LED blinking state.

				@param blinkIntervalMs Time in ms between LED state toggle (0 to disable blinking).
				@param blinkLoops Number of loops (0 for infinite loop).
			*/
			void setBlinking(uint32_t blinkIntervalMs, uint32_t blinkLoops = 0);

			/*
				Returns whether LED is currently blinking.

				@return True if LED is currently blinking, otherwise false.
			*/
			bool isBlinking() const;

			/*
				Returns whether LED is enabled.

				@return True if LED is enabled, otherwise false.
			*/
			bool isEnabled() const;

			/*
				Enables or disables LED.
				
				@param enabled True to enable LED, false to disable.
			*/
			void setEnabled(bool enabled);

			/*
				Returns pin number assigned to LED.

				@return Pin number assigned to LED.
			*/
			uint8_t getPin() const { return pin; }

			/*
				Destructor, disables LED and restores INPUT pin state.
			*/
			virtual ~ksLed();
	};
}