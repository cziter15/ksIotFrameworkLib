#pragma once

#include <ksIotFrameworkLib.h>

namespace apps
{
	/**
	 * @brief Configuration provider for LED blink parameters.
	 * 
	 * This class extends ksConfigProvider to manage the LED blink interval configuration.
	 * It demonstrates how to create a simple configuration parameter that can be adjusted
	 * through the device's configuration interface.
	 */
	class BlinkConfigProvider : public ksf::comps::ksConfigProvider
	{
		protected:
			/**
			 * @brief Reads configuration parameters from storage.
			 * 
			 * This method populates the parameter list with the blink interval setting.
			 * It's called when the configuration interface needs to display current settings.
			 */
			void readParams() override;

			/**
			 * @brief Saves configuration parameters to storage.
			 * 
			 * This method persists the parameter values to the device's non-volatile storage
			 * so they survive device reboots.
			 */
			void saveParams() override;

		public:
			static constexpr const char* CONFIG_FILENAME = "led_blink.cfg";  // Configuration file name
			static constexpr const char* BLINK_INTERVAL_PARAM = "blink_interval";  // Parameter ID for blink interval
	};
}
