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
	bool ksMqttDebugResponder::postInit(ksApplication* owner)
	{
		this->owner = owner;

		mqttConnWp = owner->findComponent<ksMqttConnector>();

		if (auto mqttConnSp{mqttConnWp.lock()})
		{
			mqttConnSp->onConnected->registerEvent(connEventHandle, std::bind(&ksMqttDebugResponder::onConnected, this));
			mqttConnSp->onDeviceMessage->registerEvent(msgEventHandle, std::bind(&ksMqttDebugResponder::onMessage, this, _1, _2));
		}

		return true;
	}

	bool ksMqttDebugResponder::loop()
	{
		return breakloop == false;
	}

	void ksMqttDebugResponder::onConnected()
	{
		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->subscribe(PGM_("cmd"));
	}

	void ksMqttDebugResponder::respond(const std::string& message) const
	{
		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->publish(PGM_("log"), message, false);
	}

	void ksMqttDebugResponder::onMessage(const std::string_view& topic, const std::string_view& message)
	{
		/* If topic is not equal to cmd, break here. */
		if (topic != "cmd")
			return;

		if (message == PGM_("netinfo"))
		{
			if (auto mqttConnSp{mqttConnWp.lock()})
			{
				respond(
					PGM_("HN: ") + WiFi.getHostname() + 
					PGM_(", IP: ") + WiFi.localIP().toString().c_str() + 
					PGM_(", CT: ") + std::to_string(mqttConnSp->getConnectionTimeSeconds()) + 
					PGM_(" s, RC: ") + std::to_string(mqttConnSp->getReconnectCounter()) + 
					PGM_(", RSSI ") + std::to_string(WiFi.RSSI()) + PGM_(" dBm")
				);
			}
		}
		else if (message == PGM_("sysinfo"))
		{
			uint32_t uptimeSec{(uint32_t)(millis64() / 1000)};
			respond(
				PGM_("Build hash: ") + ESP.getSketchMD5().c_str() + PGM_(", "
				"Device uptime: ") + std::to_string(uptimeSec) + PGM_(" sec, "
				"Free fw space: ") + std::to_string(ESP.getFreeSketchSpace()) + PGM_(" b, "
				"Free heap: ") + std::to_string(ESP.getFreeHeap()) + PGM_(" b, "
#if ESP32
				"Free PSRAM: ") + std::to_string(ESP.getFreePsram()) + PGM_(" b, "
				"Chip temperature: ") + ksf::to_string(temperatureRead(), 1) + PGM_(" [C], "
#endif
				"CPU clock: ") + std::to_string(ESP.getCpuFreqMHz()) + PGM_(" MHz, "
				"Reset reason: ") + ksf::getResetReason()
			);
		}
		else if (message == PGM_("remove_dbg"))
		{
			respond(PGM_("Removing ksMqttDebugResponder. Commands will not be available."));
			owner->markComponentToRemove(shared_from_this());
		}
		else if (message == PGM_("restart"))
		{
			respond(PGM_("Restarting system. It may take few seconds."));
			delay(500);
			ESP.restart();
		}
		else if (message == PGM_("rfcalib"))
		{
			respond(PGM_("Erasing RF CAL data, please wait..."));
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

			respond(PGM_("Erasing RF CAL done, restarting. It may take few seconds."));
			delay(500);

			ESP.restart();
		}
		else if (message == PGM_("format"))
		{
			LittleFS.format();
			respond(PGM_("Erasing flash done, restarting. It may take few seconds."));
			delay(500);
			ESP.restart();
		}
		else if (message == PGM_("break_app"))
		{
			breakloop = true;
		}
		else
		{
			bool dbgMsgHandled{false};
			customDebugHandler->broadcast(this, message, dbgMsgHandled);
				
			if (!dbgMsgHandled)
				respond(PGM_("command not supported: ") + std::string{message});
		}
	}
}
