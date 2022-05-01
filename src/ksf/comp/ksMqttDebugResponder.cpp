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
	bool ksMqttDebugResponder::init(ksComposable* owner)
	{
		app = owner;
		return true;
	}

	void ksMqttDebugResponder::postInit()
	{
		mqtt_wp = app->findComponent<ksMqttConnector>();

		if (auto mqtt_sp = mqtt_wp.lock())
		{
			mqtt_sp->onConnected->registerEvent(connEventHandle, std::bind(&ksMqttDebugResponder::onConnected, this));
			mqtt_sp->onMesssage->registerEvent(msgEventHandle, std::bind(&ksMqttDebugResponder::onMessage, this, _1, _2));
		}
	}

	bool ksMqttDebugResponder::loop()
	{
		if (millis() - secondTimer > KSF_ONE_SECOND_MS)
		{
			secondTimer = millis();
			deviceUptimeSeconds++;
		}

		return breakloop == false;
	}

	void ksMqttDebugResponder::onConnected()
	{
		if (auto mqtt_sp = mqtt_wp.lock())
			mqtt_sp->subscribe("cmd");
	}

	void ksMqttDebugResponder::respond(String message) const
	{
		if (auto mqtt_sp = mqtt_wp.lock())
			mqtt_sp->publish("log", message, false);
	}

	String ksMqttDebugResponder::getResetReason()
	{
#ifdef ESP32
		switch ((int)esp_reset_reason())
		{
			case 1:		return String("POWERON_RESET");
			case 3:		return String("SW_RESET");
			case 4:		return String("OWDT_RESET");
			case 5:		return String("DEEPSLEEP_RESET");
			case 6:		return String("SDIO_RESET");
			case 7:		return String("TG0WDT_SYS_RESET");
			case 8:		return String("TG1WDT_SYS_RESET");
			case 9:		return String("RTCWDT_SYS_RESET");
			case 10:	return String("INTRUSION_RESET");
			case 11:	return String("TGWDT_CPU_RESET");
			case 12:	return String("SW_CPU_RESET");
			case 13:	return String("RTCWDT_CPU_RESET");
			case 14:	return String("EXT_CPU_RESET");
			case 15:	return String("RTCWDT_BROWN_OUT_RESET");
			case 16:	return String("RTCWDT_RTC_RESET");
			default:	return String("NO_MEAN");
		}
#else
		return ESP.getResetReason();
#endif
	}

	void ksMqttDebugResponder::onMessage(const String& topic, const String& message)
	{
		if (topic.equals("cmd"))
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
					"Build hash: " + ESP.getSketchMD5() + ", " +
					"Device uptime: " + String(deviceUptimeSeconds) + " sec, " +
					"Free sketch: " + String(ESP.getFreeSketchSpace()) + " b, " +
					"Free heap: " + String(ESP.getFreeHeap()) + " b, " +
#ifdef ESP32
					"Free PSRAM: " + String(ESP.getFreePsram()) + " b, " +
					"Chip temperature: " + String(temperatureRead(), 1) + " [C], " +	
#endif
					"CPU clock: " + String(ESP.getCpuFreqMHz()) + " MHz, "
					"Reset reason: " + String(getResetReason())
				);
			}
			else if (message.equals("remove_dbg"))
			{
				respond("removed ksMqttDebugResponder");
				app->queueRemoveComponent(shared_from_this());
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
