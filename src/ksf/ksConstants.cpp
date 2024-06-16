/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#if ESP32
	#include <WiFi.h>
	#include <esp_phy_init.h>
	#include <nvs_flash.h>
	#include <esp_task_wdt.h>
	#include "sdkconfig.h"
#elif ESP8266
	#include <ESP8266WiFi.h>
#else			
	#error Platform not implemented.
#endif

#include "LittleFS.h"

#include "ksConstants.h"
#include "ksConfig.h"

namespace ksf
{
	const char NVS_DIRECTORY[] PROGMEM {"/nvs"};						// NVS directory name.

	const char OTA_FILENAME_TEXT[] PROGMEM {"/ksf.otabooted"};			// File name for OTA boot indicator.
	const char WIFI_CRED_FILENAME_TEXT[] PROGMEM {"ksf.wificred"};		// File name for WiFi credentials.

	const char SSID_PARAM_NAME[] PROGMEM {"ssid"};						// Param name from progmem - ssid
	const char PASSWORD_PARAM_NAME[] PROGMEM {"password"};				// Param name from progmem - password

	static EOTAType::Type otaBootType{EOTAType::NO_OTA};				// Will be true if this launch is just after OTA flash.
	static uint32_t uptime_low32, uptime_high32;						// Variables for assembling 64-bit version of millis.

	void initializeFramework()
	{
#ifdef ESP32
#if DARDUINO_3_OR_ABOVE
		esp_task_wdt_config_t twdt_config = {
			.timeout_ms = KSF_WATCHDOG_TIMEOUT_SECS * 1000,
			.idle_core_mask = (1 << CONFIG_SOC_CPU_CORES_NUM) - 1,		// Bitmask of all cores
			.trigger_panic = false,
		};
		esp_task_wdt_init(&twdt_config);
#else
		/* Setup watchdog. */
		esp_task_wdt_init(KSF_WATCHDOG_TIMEOUT_SECS, true);
#endif
		/* Initialize filesystem. */
		LittleFS.begin(true);
#endif

#if ESP8266
		/* Setup watchdog. */
		ESP.wdtEnable(KSF_WATCHDOG_TIMEOUT_SECS * 1000);

		/* Initialize filesystem. */
		LittleFS.begin();
#endif

		/* Initialize WiFi defaults. */
		WiFi.persistent(false);
		WiFi.mode(WIFI_OFF);
		WiFi.setAutoReconnect(false);

		/* Create NVS directory. */
		if (auto nvsDir{getNvsDirectory()}; !LittleFS.exists(nvsDir))
			LittleFS.mkdir(nvsDir);

		/* Handle OTA boot indicator. */
		if (auto indicatorFile{LittleFS.open(OTA_FILENAME_TEXT, PSTR("r"))})
		{
			otaBootType = indicatorFile.size() == 0 ? EOTAType::OTA_GENERIC : static_cast<EOTAType::Type>(indicatorFile.read());
			indicatorFile.close();
			LittleFS.remove(OTA_FILENAME_TEXT);
		}
	}

	const char* getNvsDirectory()
	{
		return NVS_DIRECTORY;
	}

	bool removeDirectory(const char* path)
	{
		auto dir{LittleFS.open(path, PSTR("r"))};
		if (!dir || !dir.isDirectory())
			return false;
		for (auto entry{dir.openNextFile()}; entry; entry = dir.openNextFile())
		{
			#if ESP8266
				std::string path{entry.fullName()};
			#else
				std::string path{entry.path()};
			#endif

			bool isDirectory{entry.isDirectory()};
			entry.close();

			if (!path.empty() && path[0] != '.' && path[0] != 0)
			{
				if (isDirectory)
				{
					if (!removeDirectory(path.c_str())) 
						return false;
				}
				else if (!LittleFS.remove(path.c_str()))
					return false;
			}
		}
		dir.close();
		return LittleFS.rmdir(path);
	}

	bool eraseConfigData()
	{
		bool success{removeDirectory(getNvsDirectory())};

		#if ESP8266
			success &= !ESP.eraseConfig();
		#elif ESP32
			success &= nvs_flash_erase() != ESP_OK;
		#endif

		return success;
	}

	void saveOtaBootIndicator(EOTAType::Type type)
	{
		if (auto indicatorFile{LittleFS.open(OTA_FILENAME_TEXT, PSTR("w"))})
		{
			indicatorFile.write(static_cast<uint8_t>(type));
			indicatorFile.close();
		}
	}

	EOTAType::Type getOtaBootType()
	{
		return otaBootType;
	}

	void updateDeviceUptime()
	{
		uint32_t new_low32{millis()};
		
		if (new_low32 < uptime_low32) 
			uptime_high32++;

		uptime_low32 = new_low32;
	}

	uint64_t millis64() 
	{
		return (static_cast<uint64_t>(uptime_high32) << 32) | uptime_low32;
	}

	std::string to_string(double value, const int base)
	{
		char buf[33];
		return dtostrf(value, (base + 2), base, buf);
	}

	std::string to_string(float value, const int base)
	{
		return to_string(static_cast<double>(value), base);
	}

	inline const std::string otaTypeToString(EOTAType::Type type)
	{
		switch (type)
		{
			case EOTAType::OTA_GENERIC:
				return PSTR("OTA Update (generic)");
			case EOTAType::OTA_PORTAL:
				return PSTR("OTA Update (via Portal)");
			default:
				return PSTR("OTA Update");
		}
	}

	const std::string getResetReason()
	{
		auto otaBootType{getOtaBootType()};
#if ESP32
		switch (esp_reset_reason())
		{
			case ESP_RST_POWERON:
				return PSTR("Power On");
			case ESP_RST_SW:
				if (otaBootType != EOTAType::NO_OTA)
					return otaTypeToString(otaBootType);
				return PSTR("Software/System restart");
			case ESP_RST_PANIC:
				return PSTR("Exception");
			case ESP_RST_INT_WDT:
				return PSTR("Watchdog (interrupt)");
			case ESP_RST_TASK_WDT:
				return PSTR("Watchdog (task)");
			case ESP_RST_WDT:
				return PSTR("Watchdog (other)");
			case ESP_RST_DEEPSLEEP:
				return PSTR("Deep-Sleep Wake");
			case ESP_RST_BROWNOUT:
				return PSTR("Brownout");
			case ESP_RST_SDIO:
				return PSTR("SDIO");
			default:
				return PSTR("Unknown");
		}
#elif ESP8266
		if (otaBootType != EOTAType::NO_OTA && ESP.getResetInfoPtr()->reason == REASON_SOFT_RESTART)
			return otaTypeToString(otaBootType);

		return {ESP.getResetReason().c_str()};
#else			
		#error Platform not implemented.
#endif
	}

	const std::string getUptimeFromSeconds(uint32_t sec)
	{
		return 	to_string(sec / 60 / 60 / 24) + "d " +
				to_string(sec / 60 / 60 % 24) + "h " +
				to_string(sec / 60 % 60) + "m " +
				to_string(sec % 60) + "s";
	}

	const std::string getUptimeString()
	{
		auto uptime{millis64()/1000};
		return getUptimeFromSeconds(uptime);
	}

	void loadCredentials(std::string& ssid, std::string& password)
	{
		USING_CONFIG_FILE(WIFI_CRED_FILENAME_TEXT)
		{
			ssid = config_file.getParam(SSID_PARAM_NAME);
			password = config_file.getParam(PASSWORD_PARAM_NAME);
		}
	}

	void saveCredentials(std::string ssid, std::string password)
	{
		USING_CONFIG_FILE(WIFI_CRED_FILENAME_TEXT)
		{
			config_file.setParam(SSID_PARAM_NAME, std::move(ssid));
			config_file.setParam(PASSWORD_PARAM_NAME, std::move(password));
		}
	}
}
