#include "ConfigApp.h"
#include "BlinkConfigProvider.h"
#include "../board.h"

namespace apps
{
	// Define the device name that will be shown in AP mode
	const char ConfigApp::myDeviceName[] = "LedBlink";

	bool ConfigApp::init()
	{
		// Add WiFi configurator component - creates AP and web interface
		// Users can connect to "LedBlink-XXXX" AP to configure WiFi settings
		addComponent<ksf::comps::ksWifiConfigurator>(myDeviceName);

		// Add our custom configuration provider for the blink interval
		// This makes the blink interval parameter available in the web interface
		addComponent<BlinkConfigProvider>();

		// Add a status LED to indicate configuration mode
		// The LED will be controlled by the framework during configuration
		addComponent<ksf::comps::ksLed>(LED_PIN);

		return true;
	}

	bool ConfigApp::loop()
	{
		// Delegate to base class loop implementation
		return ksApplication::loop();
	}
}
