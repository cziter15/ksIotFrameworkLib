# iotFramework
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

## RTTI - compiler flags
Component subsystem requires RTTI C++ feature. To enable, simply pass `-frtti` in `compiler.cpp.extra_flags=` option in your board.txt file.
Whole example setup:
```
# ksIotFramework build property overrides
#

compiler.cpp.extra_flags=-fno-split-wide-types -finline-limit=3 -ffast-math -frtti

network.password=ota_ksiotframework
network.auth_upload=yes
network.port=8266
```

## Dependencies
This project depends on following libraries:
* Arduino for ESP8266/32
* WiFiManager https://github.com/tzapu/WiFiManager
* PubSubClient https://github.com/knolleary/pubsubclient
