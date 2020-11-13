#include "../ksComposable.h"
#include "ksMqttDebugResponder.h"
#include "ksMqttConnector.h"
#include "../ksConstants.h"

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

using namespace std::placeholders;

namespace ksf
{
	const char ksMqttDebugResponder::cmdChannelName[] = "cmd";
	const char ksMqttDebugResponder::logChannelName[] = "log";

	bool ksMqttDebugResponder::init(ksComposable* owner)
	{
		app = owner;
		mqtt = owner->findComponent<ksMqttConnector>();

		mqtt->onConnected.registerEvent(connEventHandle, std::bind(&ksMqttDebugResponder::onConnected, this));
		mqtt->onMesssage.registerEvent(msgEventHandle, std::bind(&ksMqttDebugResponder::onMessage, this, _1, _2));

		return true;
	}

	bool ksMqttDebugResponder::loop()
	{
		return breakloop == false;
	}

	void ksMqttDebugResponder::onConnected()
	{
		mqtt->subscribe(cmdChannelName);
	}

	void ksMqttDebugResponder::onMessage(const String& topic, const String& message)
	{
		if (topic.equals(cmdChannelName))
		{
			if (message.equals("netinfo"))
			{
				mqtt->publish(logChannelName,
					"IP: " + WiFi.localIP().toString() + ", " +
					"CT: " + String(mqtt->connectionTimeSeconds) + " s, " +
					"RC: " + String(mqtt->reconnectCounter) + ", " +
					"RSSI " + String(WiFi.RSSI()) + " dBm"
				);
			}
			else if (message.equals("meminfo"))
			{
				mqtt->publish(logChannelName,
					"Free heap: " + String(ESP.getFreeHeap()) + " b, " +
					"Free psram: " + String(
#ifdef ESP32
						ESP.getFreePsram()
#else
						0
#endif
					) + " b, " +
					"Free sketch: " + String(ESP.getFreeSketchSpace()) + " b, " +
					"CPU: " + String(ESP.getCpuFreqMHz()) + " MHz"
				);
			}
			else if (message.equals("remove_dbg"))
			{
				mqtt->publish(logChannelName, "removed ksMqttDebugResponder");
				app->removeComponent(shared_from_this());
			}
			else if (message.equals("break_app"))
			{
				breakloop = true;
			}
			else 
			{
				mqtt->publish(logChannelName, "command not supported: " + message);
			}
		}
	}
}
