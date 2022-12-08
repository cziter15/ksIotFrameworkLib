/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "ksMqttDebugResponder.h"
#include "ksMqttConnector.h"
#include "../ksConstants.h"

#if ESP32
	#include <WiFi.h>
	#include <esp_phy_init.h>
#elif ESP8266
	#include <ESP8266WiFi.h>
#else			
	#error Platform not implemented.
#endif

#include <LittleFS.h>

using namespace std::placeholders;

namespace ksf::comps
{
	bool ksMqttDebugResponder::init(ksf::ksComposable* owner)
	{
		this->owner = owner;
		return true;
	}

	void ksMqttDebugResponder::postInit()
	{
		mqttConnWp = owner->findComponent<ksMqttConnector>();

		if (auto mqttConnSp{mqttConnWp.lock()})
		{
			mqttConnSp->onConnected->registerEvent(connEventHandle, std::bind(&ksMqttDebugResponder::onConnected, this));
			mqttConnSp->onMesssage->registerEvent(msgEventHandle, std::bind(&ksMqttDebugResponder::onMessage, this, _1, _2));
		}
	}

	bool ksMqttDebugResponder::loop()
	{
		return breakloop == false;
	}

	void ksMqttDebugResponder::onConnected()
	{
		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->subscribe("cmd");
	}

	void ksMqttDebugResponder::respond(const std::string& message) const
	{
		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->publish("log", message, false);
	}

	std::string ksMqttDebugResponder::getResetReason()
	{
		#if ESP32
			switch (esp_reset_reason())
			{
				case ESP_RST_POWERON:
					return {"Power On"};
				case ESP_RST_SW:
					return {"Software/System restart"};
				case ESP_RST_PANIC:
					return {"Exception"};
				case ESP_RST_INT_WDT:
					return {"Watchdog (interrupt)"};
				case ESP_RST_TASK_WDT:
					return {"Watchdog (task)"};
				case ESP_RST_WDT:
					return {"Watchdog (other)"};
				case ESP_RST_DEEPSLEEP:
					return {"Deep-Sleep Wake"};
				case ESP_RST_BROWNOUT:
					return {"Brownout"};
				case ESP_RST_SDIO:
					return {"SDIO"};
				default:
					return {"Unknown"};
			}
		#elif ESP8266
			return {ESP.getResetReason().c_str()};
		#else			
			#error Platform not implemented.
		#endif
	}

	void ksMqttDebugResponder::onMessage(const std::string_view& topic, const std::string_view& message)
	{
		/* If topic is not equal to cmd, break here. */
		if (topic.compare("cmd") != 0)
			return;

		if (message.compare("netinfo") == 0)
		{
			if (auto mqttConnSp{mqttConnWp.lock()})
			{
				respond(
					"IP: " + std::string{WiFi.localIP().toString().c_str()} + ", " +
					"CT: " + std::to_string(mqttConnSp->getConnectionTimeSeconds()) + " s, "
					"RC: " + std::to_string(mqttConnSp->getReconnectCounter()) + ", "
					"RSSI " + std::to_string(WiFi.RSSI()) + " dBm"
				);
			}
		}
		else if (message.compare("sysinfo") == 0)
		{
			uint32_t uptimeSec{(uint32_t)(millis64() / 1000)};
			respond(
				"Build hash: " + std::string(ESP.getSketchMD5().c_str()) + ", "
				"Device uptime: " + std::to_string(uptimeSec) + " sec, "
				"Free fw space: " + std::to_string(ESP.getFreeSketchSpace()) + " b, "
				"Free heap: " + std::to_string(ESP.getFreeHeap()) + " b, " +
				#if ESP32
					"Free PSRAM: " + std::to_string(ESP.getFreePsram()) + " b, "
					"Chip temperature: " + ksf::to_string(temperatureRead(), 1) + " [C], "
				#endif
				"CPU clock: " + std::to_string(ESP.getCpuFreqMHz()) + " MHz, "
				"Reset reason: " + getResetReason().c_str()
			);
		}
		else if (message.compare("remove_dbg") == 0)
		{
			respond("Removing ksMqttDebugResponder. Commands will not be available.");
			owner->queueComponentForRemoval(shared_from_this());
		}
		else if (message.compare("restart") == 0)
		{
			respond("Restarting system. It may take few seconds.");
			delay(500);
			ESP.restart();
		}
		else if (message.compare("rfcalib") == 0)
		{
			respond("Erasing RF CAL data, please wait...");
			delay(500);
			
			#if ESP32
				esp_phy_erase_cal_data_in_nvs();
			#elif ESP8266
				flash_size_map size_map{system_get_flash_size_map()};
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
			#else			
				#error Platform not implemented.
			#endif

			respond("Erasing RF CAL done, restarting. It may take few seconds.");
			delay(500);

			ESP.restart();
		}
		else if (message.compare("format") == 0)
		{
			LittleFS.format();
			respond("Erasing flash done, restarting. It may take few seconds.");
			delay(500);
			ESP.restart();
		}
		else if (message.compare("break_app") == 0)
		{
			breakloop = true;
		}
		else
		{
			bool dbgMsgHandled{false};
			customDebugHandler->broadcast(this, message, dbgMsgHandled);
				
			if (!dbgMsgHandled)
				respond("command not supported: " + std::string{message});
		}
	}
}
