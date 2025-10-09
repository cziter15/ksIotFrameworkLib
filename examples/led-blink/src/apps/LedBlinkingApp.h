#pragma once

#include <ksIotFrameworkLib.h>

namespace apps
{
	/**
	 * @brief Main application that controls LED blinking behavior.
	 * 
	 * This application demonstrates how to:
	 * - Read configuration parameters from storage
	 * - Create and control an LED component
	 * - Implement basic device logic with configurable behavior
	 * 
	 * The LED blink interval can be configured through the ConfigApp interface
	 * and is persisted in non-volatile storage, surviving device reboots.
	 */
	class LedBlinkingApp : public ksf::ksApplication
	{
		protected:
			std::weak_ptr<ksf::comps::ksLed> ledWp;  // Weak pointer to LED component
			uint32_t blinkInterval{1000};             // Blink interval in milliseconds (default: 1000ms)

		public:
			/**
			 * @brief Initializes the blink application.
			 * 
			 * This method:
			 * - Reads the blink interval from configuration storage
			 * - Creates the LED component
			 * - Sets up WiFi connection (required by framework)
			 * - Starts LED blinking with the configured interval
			 * 
			 * @return true if initialization succeeded, false otherwise.
			 */
			bool init() override;

			/**
			 * @brief Main loop of the blink application.
			 * 
			 * The LED blinking is handled automatically by the ksLed component,
			 * so this loop just delegates to the base class.
			 * 
			 * @return true to continue running, false to exit.
			 */
			bool loop() override;
	};
}
