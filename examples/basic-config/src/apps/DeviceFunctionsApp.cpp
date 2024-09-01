#include "ConfigApp.h"
#include "DeviceFunctionsApp.h"

#include "../board.h"

namespace apps
{
	bool DeviceFunctionsApp::init()
	{
		/* Create required components (Wifi and Mqtt debug). */
		addComponent<ksf::comps::ksWifiConnector>(ConfigApp::myDeviceName);
		addComponent<ksf::comps::ksDevStatMqttReporter>();

		/* Create mqttConnector component. */
		mqttConnWp = addComponent<ksf::comps::ksMqttConnector>();

		/* Create LED components. */
		connectionStatusLedWp = addComponent<ksf::comps::ksLed>(CFG_STATUS_LED);

		/* Bind to MQTT callbacks. */
		if (auto connectionStatusLedSp{connectionStatusLedWp.lock()})
		{
			if (auto mqttConnSp{mqttConnWp.lock()})
			{
				mqttConnSp->onConnected->registerEvent(connEventHandle, std::bind(&DeviceFunctionsApp::onMqttConnected, this));
				mqttConnSp->onDisconnected->registerEvent(disEventHandle, std::bind(&DeviceFunctionsApp::onMqttDisconnected, this));
			}
			
			/* Start LED blinking on finished init. */
			connectionStatusLedSp->setBlinking(500);
		}

		/* Application finished initialization, return true as it succedeed. */
		return true;
	}

	void DeviceFunctionsApp::onMqttDisconnected()
	{
		if (auto connectionStatusLedSp{connectionStatusLedWp.lock()})
			connectionStatusLedSp->setBlinking(500);
	}

	void DeviceFunctionsApp::onMqttConnected()
	{
		if (auto connectionStatusLedSp{connectionStatusLedWp.lock()})
			connectionStatusLedSp->setBlinking(0);
	}

	bool DeviceFunctionsApp::loop()
	{
		return ksApplication::loop();
	}
}
