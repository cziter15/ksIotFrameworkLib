/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include <Arduino.h>

namespace ksf::comps
{
	class ksLed : public ksComponent
		{
			KSF_RTTI_DECLARATIONS(ksLed, ksComponent)

			protected:
				uint8_t pin{0};						// Saved LED pin number.
				bool activeLow{false};				// If false means that the led is turned on.
				uint32_t lastBlinkTimeMs{0};		// Last blink time (milliseconds since boot).
				uint32_t blinkIntervalMs{0};		// Current blink interval (milliseconds).
				uint32_t blinkLoops{0};				// Number of remaining loops.

			public:
				/*
					Constructs ksLed object, assigning passed pin number.

					@param pin Pin number assigned to LED.
					@param activeLow True if pin should be driven LOW to enable LED. Otherwise false.
				*/
				ksLed(uint8_t pin, bool activeLow = false);

				/*
					Initializes ksLed component.
					
					@param owner Pointer to the ksComposable owning the object (application).
					@return True if init succedeed, otherwise false.
				*/
				bool init(class ksf::ksComposable* owner) override;

				/*
					Handles logic of the ksLed component.

					@return True if everything is okay, otherwise false to break application logic.
				*/
				bool loop() override;

				/*
					Used to set LED blinking parameters.

					@param blinkIntervalMs Time in ms between LED state toggle (0 to disable blinking).
					@param blinkLoops Number of loops (0 for infinite loop).
				*/
				void setBlinking(uint32_t blinkIntervalMs, uint32_t blinkLoops = 0);

				/*
					@return True if LED is set up to blink, otherwise false.
				*/
				bool isBlinking() const;

				/*
					Checks if LED is currently enabled.
					@return True if assigned LED pin is HIGH, otherwise false.
				*/
				bool isEnabled() const;

				/*
					Sets LED enabled state.
					@param enabled True if LED should be enabled, otherwise false.
				*/
				void setEnabled(bool enabled);

				/*
					Returns assigned pin number.

					@return Assigned pin number.
				*/
				uint8_t getPin() const { return pin; }

				/*
					Destructor, disables LED and restores INPUT pin state.
				*/
				virtual ~ksLed();
		};
}