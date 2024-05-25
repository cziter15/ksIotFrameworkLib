#pragma once

#include <ksIotFrameworkLib.h>

namespace apps
{
	class DeviceFunctionsApp : public ksf::ksApplication
	{
		protected:

			std::weak_ptr<ksf::comps::ksLed> wifiLedWp;										// Weeak pointer to WiFi status LED component.
			std::weak_ptr<ksf::comps::ksMqttConnector> mqttConnWp;							// Weeak pointer to MQTT component.

			std::unique_ptr<ksf::evt::ksEventHandle> connEventHandleSp, disEventHandleSp;	// Handles to MQTT events.

			void onMqttConnected();
			void onMqttDisconnected();

		public:
			bool init() override;
			bool loop() override;
	};
}