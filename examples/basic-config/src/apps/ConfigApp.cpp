#include "ConfigApp.h"
#include "board.h"

namespace apps
{
	const char ConfigApp::myDeviceName[] = "ExampleDevice";

	bool ConfigApp::init()
	{
		addComponent<ksf::comps::ksWifiConfigurator>(myDeviceName);
		addComponent<ksf::comps::ksMqttConfigProvider>();

		addComponent<ksf::comps::ksLed>(CFG_STATUS_LED);

		return true;
	}

	bool ConfigApp::loop()
	{
		return ksApplication::loop();
	}
}