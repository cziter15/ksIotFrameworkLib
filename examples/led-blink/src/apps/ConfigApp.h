#pragma once

#include <ksIotFrameworkLib.h>

namespace apps
{
	/**
	 * @brief Configuration application that provides a web interface for device setup.
	 * 
	 * This application creates an Access Point (AP) mode configuration interface
	 * where users can set up WiFi credentials and adjust the LED blink interval.
	 * It's activated when WiFi credentials are not configured on the device.
	 */
	class ConfigApp : public ksf::ksApplication
	{
		public:
			static const char myDeviceName[];  // Device name shown in AP mode

			/**
			 * @brief Initializes the configuration application.
			 * @return true if initialization succeeded, false otherwise.
			 */
			bool init() override;

			/**
			 * @brief Main loop of the configuration application.
			 * @return true to continue running, false to exit.
			 */
			bool loop() override;
	};
}
