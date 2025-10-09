#include "ConfigApp.h"
#include "../board.h"

namespace apps
{
	// Define the device name that will be shown in AP mode
	const char ConfigApp::myDeviceName[] = "MqttLed";

	bool ConfigApp::init()
	{
		// Add WiFi configurator component - creates AP and web interface
		// Users can connect to "MqttLed-XXXX" AP to configure WiFi settings
		addComponent<ksf::comps::ksWifiConfigurator>(myDeviceName);

		// Add MQTT configuration provider for MQTT broker settings
		// This makes MQTT parameters available in the web interface
		addComponent<ksf::comps::ksMqttConfigProvider>();

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
