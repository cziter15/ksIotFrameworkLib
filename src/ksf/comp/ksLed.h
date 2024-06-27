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
	/*!
		@brief A component that simplifies driving the LED.

		It was designed to help the device to signal it's state using blink pattern.
		This component has also some extra features like state inversion (driveAsActiveLow) and low-power pin driving mode
 		knowm as driveAsPushPull, which uses pull resistors and significantly reduces the amout of emitted light.
	*/
	class ksLed : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksLed, ksComponent)

		protected:
			uint8_t pin{0};								//!< Pin number assigned to LED.
			uint32_t lastBlinkTimeMs{0};				//!< Timestamp of previous state change (milliseconds).
			uint32_t blinkIntervalMs{0};				//!< Intervals between state change (0 to disable blinking).
			uint32_t blinkLoops{0};						//!< Number of state change cycles (0 for infinite loop).

			struct {
				bool activeLow : 1;						//!< True if the LED should be active low, otherwise false.
				bool driveAsPushPull : 1;				//!< True if the LED should be driven as push-pull, otherwise false.
			} bitflags = {false, false};

		public:
			/*!
				@brief Constructs the LED object.
				@param pin Pin number assigned to LED.
				@param activeLow True if the LED should be active low, otherwise false
				@param driveAsPushPull True if the LED should be driven as push-pull, otherwise false
			*/
			ksLed(uint8_t pin, bool activeLow = false, bool driveAsPushPull = false);

			/*!
				@brief Initializes the LED component.
				@param owner Pointer to parent ksApplication object.
				@return True if init succedeed, otherwise false.
			*/
			bool init(ksApplication* owner) override;

			/*!
				@brief Executes core of the LED component logic.
				@return True if everything is okay, otherwise false to break application logic.
			*/
			bool loop(ksApplication* app) override;

			/*!
				@brief Sets LED blinking pattern.
				@param blinkIntervalMs Interval in milliseconds between toggle action (0 to disable blinking)
				@param blinkLoops Number of state toggle loops/cycles (0 for infinite loop).
			*/
			void setBlinking(uint32_t blinkIntervalMs, uint32_t blinkLoops = 0);

			/*!
				@brief Returns whether LED is currently blinking.
				@return True if the LED is currently blinking, otherwise false.
			*/
			bool isBlinking() const;

			/*!
				@brief Returns whether the LED is actually enabled.
				@return True if the LED is enabled, otherwise false.
			*/
			bool isEnabled() const;

			/*!
				@brief Enables or disables the LED.
				@param enabled True to enable LED, false to disable.
			*/
			void setEnabled(bool enabled);

			/*!
				@brief Enables or disables the push/pull driving mode.
				@param driveAsPushPull True if LED should is driven as push-pull, false otherwise.
			*/
			void setDriveAsPushPull(bool driveAsPushPull);

			/*!
				@brief Returns a pin number assigned to the LED.
				@return Pin number assigned to LED.
			*/
			uint8_t getPin() const { return pin; }

			/*!
				@brief Destructs the component and restores INPUT mode on the assigned pin.
			*/
			virtual ~ksLed();
	};
}
