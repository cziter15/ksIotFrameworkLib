#pragma once

#include <ksIotFrameworkLib.h>

namespace apps
{
	/**
	 * @brief Main application that controls LED via MQTT messages.
	 * 
	 * This application demonstrates how to:
	 * - Connect to WiFi and MQTT broker
	 * - Subscribe to MQTT topics
	 * - Handle incoming MQTT messages
	 * - Control hardware (LED) based on MQTT commands
	 * 
	 * The LED can be controlled by publishing messages to the "led" topic
	 * under the configured device prefix. Accepted values are "on" and "off".
	 */
	class MqttLedApp : public ksf::ksApplication
	{
		protected:
			std::weak_ptr<ksf::comps::ksLed> ledWp;                     // Weak pointer to LED component
			std::weak_ptr<ksf::comps::ksMqttConnector> mqttConnWp;      // Weak pointer to MQTT component
			
			std::unique_ptr<ksf::evt::ksEventHandle> connEventHandle;   // Handle to MQTT connected event
			std::unique_ptr<ksf::evt::ksEventHandle> msgEventHandle;    // Handle to MQTT message event

			/**
			 * @brief Called when MQTT connection is established.
			 * 
			 * This method subscribes to the "led" topic when MQTT connects.
			 */
			void onMqttConnected();

			/**
			 * @brief Called when MQTT message is received.
			 * 
			 * @param topic The topic that received the message
			 * @param payload The message payload
			 * 
			 * This method handles "on" and "off" commands to control the LED.
			 */
			void onMqttMessage(const std::string_view& topic, const std::string_view& payload);

		public:
			/**
			 * @brief Initializes the MQTT LED application.
			 * 
			 * This method:
			 * - Creates WiFi connector component
			 * - Creates MQTT connector component
			 * - Creates LED component
			 * - Registers MQTT event handlers
			 * 
			 * @return true if initialization succeeded, false otherwise.
			 */
			bool init() override;

			/**
			 * @brief Main loop of the MQTT LED application.
			 * 
			 * The MQTT connection and message handling is done automatically
			 * by the framework components, so this loop just delegates to the base class.
			 * 
			 * @return true to continue running, false to exit.
			 */
			bool loop() override;
	};
}
