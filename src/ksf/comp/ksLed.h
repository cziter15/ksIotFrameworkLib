/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"

namespace ksf
{
	namespace comps
	{
		class ksLed : public ksf::ksComponent
		{
			protected:
				uint8_t ledPin = 0;					// Saved LED pin number.

				uint32_t lastBlinkTime = 0;			// Last blink time (ms).
				uint32_t blinkInterval = 0;			// Current blink interval (ms)
				uint32_t blinkLoops = 0;			// Number of remaining loops.

			public:
				/*
					Constructs ksLed object, assigning passed pin number.
					@param pin Pin assigned to LED.
				*/
				ksLed(uint8_t pin);

				/*
					Initializes ksLed component.
					
					@param owner Pointer to ownning ksComposable object (application).
					@return True if init succedeed, otherwise false.
				*/
				bool init(class ksf::ksComposable* owner) override;

				/*
					ksLed component loop.
					Handles LED blinking logic.
				*/
				bool loop() override;

				/*
					Used to set LED blink parameters.

					@param interval Time in ms between LED state toggle (0 to disable blinking).
					@param loops Number of loops (0 for infinite loop).
				*/
				virtual void setBlinking(uint32_t interval, uint32_t loops = 0);

				/*
					@return True if LED is set up to blink, otherwise false.
				*/
				virtual bool isBlinking() const;

				/*
					Checks if LED is currently enabled.
					@return True if assigned LED pin is HIGH, otherwise false.
				*/
				virtual bool isEnabled() const;

				/*
					Sets LED enabled state.
					@param enabled True if LED should be enabled, otherwise false.
				*/
				virtual void setEnabled(bool enabled);

				/*
					PIN number getter.
					@return PIN assigned to the LED.
				*/
				uint8_t getPin() const { return ledPin; }

				/*
					Destructor, disables LED and restores pin state.
				*/
				virtual ~ksLed();
		};
	}
}