#include "LedBlinkingApp.h"
#include "BlinkConfigProvider.h"
#include "ConfigApp.h"
#include "../board.h"

namespace apps
{
	bool LedBlinkingApp::init()
	{
		// Step 1: Read the blink interval from configuration
		// We create a temporary ConfigProvider to read the saved configuration
		USING_CONFIG_FILE(BlinkConfigProvider::CONFIG_FILENAME)
		{
			// Read the blink interval parameter, defaulting to "1000" if not set
			const std::string& intervalStr = config_file.getParam(BlinkConfigProvider::BLINK_INTERVAL_PARAM, "1000");
			
			// Convert the string to an integer using the framework's from_chars utility
			// If conversion fails or value is 0, use the default of 1000ms
			if (!ksf::from_chars(intervalStr, blinkInterval) || blinkInterval == 0)
				blinkInterval = 1000;  // Use 1000ms default if conversion failed or value was 0
		}

		// Step 2: Set up WiFi connection
		// This is required by the framework to maintain device connectivity
		// The device name is used for mDNS and OTA updates
		addComponent<ksf::comps::ksWifiConnector>(ConfigApp::myDeviceName);

		// Step 3: Create the LED component
		// This component handles all LED control, including the blinking logic
		ledWp = addComponent<ksf::comps::ksLed>(LED_PIN);

		// Step 4: Start LED blinking with the configured interval
		if (auto ledSp = ledWp.lock())
		{
			// setBlinking() takes two parameters:
			// - blinkInterval: time in milliseconds between LED state changes
			// - 0: number of blink cycles (0 = infinite blinking)
			ledSp->setBlinking(blinkInterval, 0);
		}

		// Initialization successful
		return true;
	}

	bool LedBlinkingApp::loop()
	{
		// The LED component automatically handles blinking in its own loop
		// We just need to call the base class loop to let all components run
		return ksApplication::loop();
	}
}
