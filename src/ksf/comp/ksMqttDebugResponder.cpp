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
		mqtt_wp = owner->findComponent<ksMqttConnector>();

		if (auto mqtt_sp = mqtt_wp.lock())
		{
			mqtt_sp->onConnected->registerEvent(connEventHandle, std::bind(&ksMqttDebugResponder::onConnected, this));
			mqtt_sp->onMesssage->registerEvent(msgEventHandle, std::bind(&ksMqttDebugResponder::onMessage, this, _1, _2));
		}
	
		return true;
	}

	bool ksMqttDebugResponder::loop()
	{
		return breakloop == false;
	}

	void ksMqttDebugResponder::onConnected()
	{
		if (auto mqtt_sp = mqtt_wp.lock())
			mqtt_sp->subscribe(cmdChannelName);
	}

	void ksMqttDebugResponder::respond(String message) const
	{
		if (auto mqtt_sp = mqtt_wp.lock())
			mqtt_sp->publish(logChannelName, message, false);
	}

	void ksMqttDebugResponder::onMessage(const String& topic, const String& message)
	{
		if (topic.equals(cmdChannelName))
		{
			if (message.equals("netinfo"))
			{
				if (auto mqtt_sp = mqtt_wp.lock())
				{
					respond(
						"IP: " + WiFi.localIP().toString() + ", " +
						"CT: " + String(mqtt_sp->connectionTimeSeconds) + " s, " +
						"RC: " + String(mqtt_sp->reconnectCounter) + ", " +
						"RSSI " + String(WiFi.RSSI()) + " dBm"
					);
				}
			}
			else if (message.equals("sysinfo"))
			{
				respond(
					"Free sketch: " + String(ESP.getFreeSketchSpace()) + " b, " +
					"Free heap: " + String(ESP.getFreeHeap()) + " b, " +
#ifdef ESP32
					"Free PSRAM: " + String(ESP.getFreePsram()) + " b, " +
					"Chip temperature: " + String(temperatureRead(), 1) + " [C], " +	
#endif
					"CPU clock: " + String(ESP.getCpuFreqMHz()) + " MHz"
				);
			}
			else if (message.equals("remove_dbg"))
			{
				respond("removed ksMqttDebugResponder");
				queueDestroy();
			}
			else if (message.equals("restart"))
			{
				ESP.restart();
			}
			else if (message.equals("break_app"))
			{
				breakloop = true;
			}
			else
			{
				bool dbgMsgHandled = false;
				customDebugHandler->broadcast(this, message, dbgMsgHandled);
					
				if (!dbgMsgHandled)
					respond("command not supported: " + message);
			}
		}
	}
}
