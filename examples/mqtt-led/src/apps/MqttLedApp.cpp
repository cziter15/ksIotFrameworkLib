#include "MqttLedApp.h"
#include "ConfigApp.h"
#include "../board.h"

namespace apps
{
	bool MqttLedApp::init()
	{
		// Step 1: Set up WiFi connection
		// This is required by the framework to maintain device connectivity
		// The device name is used for mDNS and OTA updates
		addComponent<ksf::comps::ksWifiConnector>(ConfigApp::myDeviceName);

		// Step 2: Create the MQTT connector component
		// This component handles connection to MQTT broker and message routing
		mqttConnWp = addComponent<ksf::comps::ksMqttConnector>();

		// Step 3: Create the LED component
		// This component handles all LED control
		ledWp = addComponent<ksf::comps::ksLed>(LED_PIN);

		// Step 4: Register MQTT event handlers
		if (auto mqttConnSp = mqttConnWp.lock())
		{
			// Register callback for when MQTT connection is established
			mqttConnSp->onConnected->registerEvent(connEventHandle, 
				std::bind(&MqttLedApp::onMqttConnected, this));
			
			// Register callback for when MQTT messages arrive on subscribed topics
			mqttConnSp->onDeviceMessage->registerEvent(msgEventHandle, 
				std::bind(&MqttLedApp::onMqttMessage, this, std::placeholders::_1, std::placeholders::_2));
		}

		// Initialization successful
		return true;
	}

	void MqttLedApp::onMqttConnected()
	{
		// Subscribe to the "led" topic when MQTT connects
		// This topic will receive commands to control the LED
		// skipDevicePrefix = false means the subscription will use the configured device prefix
		if (auto mqttConnSp = mqttConnWp.lock())
		{
			mqttConnSp->subscribe("led", false);
		}
	}

	void MqttLedApp::onMqttMessage(const std::string_view& topic, const std::string_view& payload)
	{
		// Handle messages received on the "led" topic
		// Check if this message is for the "led" topic
		if (topic.find("led") != std::string_view::npos)
		{
			if (auto ledSp = ledWp.lock())
			{
				// Turn LED on if payload is "on"
				if (payload == "on")
				{
					ledSp->setEnabled(true);
				}
				// Turn LED off if payload is "off"
				else if (payload == "off")
				{
					ledSp->setEnabled(false);
				}
			}
		}
	}

	bool MqttLedApp::loop()
	{
		// The MQTT connector and LED components automatically handle their logic
		// We just need to call the base class loop to let all components run
		return ksApplication::loop();
	}
}
