/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "ksMqttDebugResponder.h"
#include "ksMqttConnector.h"
#include "../ksConstants.h"

#ifdef ESP32
	#include <WiFi.h>
	#include <esp_phy_init.h>
#else
	#include <ESP8266WiFi.h>
#endif

using namespace std::placeholders;

namespace ksf::comps
{
	bool ksMqttDebugResponder::init(ksf::ksComposable* owner)
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
		return breakloop == false;
	}

	void ksMqttDebugResponder::onConnected()
	{
		if (auto mqtt_sp = mqtt_wp.lock())
			mqtt_sp->subscribe("cmd");
	}

	void ksMqttDebugResponder::respond(const String& message) const
	{
		if (auto mqtt_sp = mqtt_wp.lock())
			mqtt_sp->publish("log", message, false);
	}

	String ksMqttDebugResponder::getResetReason()
	{
		#ifdef ESP32
			switch (esp_reset_reason())
			{
				case ESP_RST_POWERON:
					return String("Power On");
				case ESP_RST_SW:
					return String("Software/System restart");
				case ESP_RST_PANIC:
					return String("Exception");
				case ESP_RST_INT_WDT:
					return String("Watchdog (interrupt)");
				case ESP_RST_TASK_WDT:
					return String("Watchdog (task)");
				case ESP_RST_WDT:
					return String("Watchdog (other)");
				case ESP_RST_DEEPSLEEP:
					return String("Deep-Sleep Wake");
				case ESP_RST_BROWNOUT:
					return String("Brownout");
				case ESP_RST_SDIO:
					return String("SDIO");
				default:
					return String("Unknown");
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
						"CT: " + String(mqtt_sp->getConnectionTimeSeconds()) + " s, " +
						"RC: " + String(mqtt_sp->getReconnectCounter()) + ", " +
						"RSSI " + String(WiFi.RSSI()) + " dBm"
					);
				}
			}
			else if (message.equals("sysinfo"))
			{
				uint32_t uptimeSec = (uint32_t)(millis64() / 1000);
				respond(
					"Build hash: " + ESP.getSketchMD5() + ", " +
					"Device uptime: " + String(uptimeSec) + " sec, " +
					"Free fw space: " + String(ESP.getFreeSketchSpace()) + " b, " +
					"Free heap: " + String(ESP.getFreeHeap()) + " b, " +
					#ifdef ESP32
						"Free PSRAM: " + String(ESP.getFreePsram()) + " b, " +
						"Chip temperature: " + String(temperatureRead(), 1) + " [C], " +	
					#endif
					"CPU clock: " + String(ESP.getCpuFreqMHz()) + " MHz, "
					"Reset reason: " + getResetReason()
				);
			}
			else if (message.equals("remove_dbg"))
			{
				respond("Removing ksMqttDebugResponder. Commands will not be available.");
				app->queueRemoveComponent(shared_from_this());
			}
			else if (message.equals("restart"))
			{
				respond("Restarting system. It may take few seconds.");
				delay(500);
				ESP.restart();
			}
			else if (message.equals("rfcalib"))
			{
				respond("Erasing RF CAL data, please wait...");
				delay(500);
				
				#ifdef ESP32
					esp_phy_erase_cal_data_in_nvs();
				#else
					flash_size_map size_map = system_get_flash_size_map();
					uint32 rf_cal_sec{0};

					switch (size_map) 
					{
						case FLASH_SIZE_4M_MAP_256_256: 	rf_cal_sec = 128 - 5;	break;
						case FLASH_SIZE_8M_MAP_512_512: 	rf_cal_sec = 256 - 5; 	break;
						case FLASH_SIZE_16M_MAP_512_512: 	rf_cal_sec = 512 - 5;	break;
						case FLASH_SIZE_16M_MAP_1024_1024:	rf_cal_sec = 512 - 5;	break;
						case FLASH_SIZE_32M_MAP_512_512:	rf_cal_sec = 1024 - 5;	break;
						case FLASH_SIZE_32M_MAP_1024_1024:	rf_cal_sec = 1024 - 5;	break;
						case FLASH_SIZE_64M_MAP_1024_1024:	rf_cal_sec = 2048 - 5;	break;
						case FLASH_SIZE_128M_MAP_1024_1024:	rf_cal_sec = 4096 - 5;	break;
						default: break;
					}
				
					ESP.flashEraseSector(rf_cal_sec);
				#endif

				respond("Erasing RF CAL done, restarting. It may take few seconds.");
				delay(500);

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
