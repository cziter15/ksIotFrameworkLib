# ksIotFrameworkLib
<p align="center">
  <img src="docfiles/header.jpg">
</p>

The aim of this project is to provide simple kick-start application pattern for ESP 8266/32 IoT app development. Last year I was creating a lot of esp apps for multiple devices and realized that I everytime need to copy whole app and modify few classes. The purpose of this project is to tidy this process up.

## Architecture
<p align="center">
  <img src="docfiles/app_diagram.png">
</p>

## Components
| Component  | Function |
| ------------- | ------------- |
| ksConfigProvider  | Used to manage parameters, configurator component calls each config provider to handle parameter inject/capture on WiFi configuration stage. |
| ksLed  | Used to handle diodes, easy blinking, turn off, turn on etc. |
| ksMqttConfigProvider  | Used to manage MQTT parameters (broker, password, prefix etc..). |
| ksMqttConnector  | Used to maintain connection with Mqtt, user can bind to onMessage, onConnected events. |
| ksMqttDebugResponder  | Provides debug commands for app with ksMqttConnector component. |
| ksTimer  | Easy timer component with multiple events support. |
| ksWiFiConfigurator | Base WiFi configurator component, brings WiFi management portal, allow config providers to inject and capture parameters. |
| ksWiFiConnector | Handles WiFi connection. |

## Building application
To build an application simply create new class inherited from ksApplication. Inside init method add components and setup them, then call base ksApplication's init method. You can also optionally override loop method, but remember that baseclass method (ksApplication's loop) iterates over component list executing loop call on each registered component.

## A word of warning
The idea was to prevent launching application when any component initialization fail. This will lead to false returned from ksApplication::init (base class) method, but due to inheritance, user can override it's behaviour. Application will then try to launch and after initialization, it will tick every component, even if one of them failed to initialize. This can lead to crashes, especially inside loop method.

So the flow is:
- Add components (addComponent simply construct class and add it's pointer to app component list).
- Run ksApplicaiton:init (it will iterate through component list and initialize them, returning false if any init failed).
- If ksApplication::init() does not return true, simply return false in your app init method

```c++
bool EnergyMonitor::init()
{
	addComponent<ksf::ksWifiConnector>(EnergyMonitorConfig::emonDeviceName);
	addComponent<ksf::ksMqttDebugResponder>();
	mqtt = addComponent<ksf::ksMqttConnector>();
	statusLed = addComponent<ksf::ksLed>(STATUS_LED_PIN);
	eventLed = addComponent<ksf::ksLed>(EVENT_LED_PIN);

	auto sensor_timer = addComponent<ksf::ksTimer>(EMON_TIMER_INTERVAL, true);
	auto sec_timer = addComponent<ksf::ksTimer>(EMON_SEC_TIMER, true);
	
	if (!ksApplication::init())
		return false;

	/* [ Rest of application initialization code ] */
}
```

## RTTI - compiler flags
Component subsystem requires RTTI C++ feature. To enable, simply pass `-frtti` in `compiler.cpp.extra_flags=` option in your board.txt file.
Whole example setup:
```ini
# ksIotFramework build property overrides
compiler.cpp.extra_flags=-fno-split-wide-types -finline-limit=3 -ffast-math -frtti
```

## Dependencies
This project depends on following libraries:
* Arduino for ESP8266/32
* WiFiManager https://github.com/tzapu/WiFiManager
* PubSubClient https://github.com/knolleary/pubsubclient
