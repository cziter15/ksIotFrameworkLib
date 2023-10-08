/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include <LittleFS.h>

#if ESP32
	#include <WiFi.h>
	#include <esp_phy_init.h>
#elif ESP8266
	#include <ESP8266WiFi.h>
#else			
	#error Platform not implemented.
#endif

#include "../ksApplication.h"
#include "../ksConstants.h"
#include "ksMqttDebugResponder.h"
#include "ksMqttConnector.h"

using namespace std::placeholders;

namespace ksf::comps
{
	bool ksMqttDebugResponder::postInit(ksApplication* app)
	{
		mqttConnWp = app->findComponent<ksMqttConnector>();

		if (auto mqttConnSp{mqttConnWp.lock()})
		{
			mqttConnSp->onConnected->registerEvent(connEventHandle, std::bind(&ksMqttDebugResponder::onConnected, this));
			mqttConnSp->onDeviceMessage->registerEvent(msgEventHandle, std::bind(&ksMqttDebugResponder::onMessage, this, _1, _2));
		}

		return true;
	}

	bool ksMqttDebugResponder::loop(ksApplication* app)
	{
		return breakloop == false;
	}

	void ksMqttDebugResponder::onConnected()
	{
		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->subscribe(PSTR("cmd"));
	}

	void ksMqttDebugResponder::respond(const std::string& message) const
	{
		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->publish(PSTR("log"), message, false);
	}

	void ksMqttDebugResponder::onMessage(const std::string_view& topic, const std::string_view& message)
	{
		/* If topic is not equal to cmd, break here. */
		if (topic != "cmd")
			return;

		if (message == PSTR("netinfo"))
		{
			if (auto mqttConnSp{mqttConnWp.lock()})
			{
				std::string response;
				response += PSTR("HN: ");
				response += WiFi.getHostname();
				response += PSTR(", IP: ");
				response += WiFi.localIP().toString().c_str();
				response += PSTR(", CT: ");
				response += std::to_string(mqttConnSp->getConnectionTimeSeconds());
				response += PSTR(" s, RC: ");
				response += std::to_string(mqttConnSp->getReconnectCounter());
				response += PSTR(", RSSI ") + std::to_string(WiFi.RSSI()) + PSTR(" dBm");
				respond(response);
			}
		}
		else if (message == PSTR("sysinfo"))
		{
			uint32_t uptimeSec{(uint32_t)(millis64() / 1000)};

			std::string response;
			response += PSTR("Build hash:");
			response += ESP.getSketchMD5().c_str();
			response += PSTR(", Device uptime: ");
			response += std::to_string(uptimeSec);
			response += PSTR(" sec, Free fw space: ");
			response += std::to_string(ESP.getFreeSketchSpace());
			response += PSTR(" b, Free heap: ");
			response += std::to_string(ESP.getFreeHeap());
			response += PSTR(" b");

#if ESP32
			response += PSTR(", Free PSRAM: ");
			response += std::to_string(ESP.getFreePsram());
			response += PSTR(" b, Chip temperature: ");
			response += ksf::to_string(temperatureRead(), 1);
			response += PSTR(" [C] ");
#endif
			response += PSTR(", CPU clock: ");
			response += std::to_string(ESP.getCpuFreqMHz());
			response += PSTR(" MHz, Reset reason: ");
			response += ksf::getResetReason();
			respond(response);
			
		}
		else if (message == PSTR("remove_dbg"))
		{
			respond(PSTR("Removing ksMqttDebugResponder. Commands will not be available."));
			componentState = ksComponentState::ToBeRemoved;
			return;
		}
		else if (message == PSTR("restart"))
		{
			respond(PSTR("Restarting system. It may take few seconds."));
			delay(500);
			ESP.restart();
		}
		else if (message == PSTR("rfcalib"))
		{
			respond(PSTR("Erasing RF CAL data, please wait..."));
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

			respond(PSTR("Erasing RF CAL done, restarting. It may take few seconds."));
			delay(500);

			ESP.restart();
		}
		else if (message == PSTR("format"))
		{
			LittleFS.format();
			respond(PSTR("Erasing flash done, restarting. It may take few seconds."));
			delay(500);
			ESP.restart();
		}
		else if (message == PSTR("break_app"))
		{
			breakloop = true;
		}
		else
		{
			bool dbgMsgHandled{false};
			customDebugHandler->broadcast(this, message, dbgMsgHandled);
				
			if (!dbgMsgHandled)
				respond(PSTR("command not supported: ") + std::string{message});
		}
	}
}
