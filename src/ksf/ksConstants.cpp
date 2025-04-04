/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#if defined(ESP32)
	#include <WiFi.h>
	#include <esp_phy_init.h>
	#include <nvs_flash.h>
	#include <esp_task_wdt.h>
	#include <esp_arduino_version.h>
	#include "sdkconfig.h"
#elif defined(ESP8266)
	#include <ESP8266WiFi.h>
#else			
	#error Platform not implemented.
#endif

#include <LittleFS.h>

#include "ksConstants.h"
#include "misc/ksConfig.h"

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
#if defined(ESP32)
#if (ESP_ARDUINO_VERSION_MAJOR >= 3)
		esp_task_wdt_config_t twdt_config = {
			.timeout_ms = KSF_WATCHDOG_TIMEOUT_SECS * 1000,
			.idle_core_mask = (1 << CONFIG_SOC_CPU_CORES_NUM) - 1,
			.trigger_panic = false,
		};
		esp_task_wdt_reconfigure(&twdt_config);
#else
		esp_task_wdt_init(KSF_WATCHDOG_TIMEOUT_SECS * 1000, false);
#endif
		/* Initialize filesystem. */
		LittleFS.begin(true);
#endif

#if defined(ESP8266)
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
			otaBootType = (indicatorFile.size() > 0) ? static_cast<EOTAType::Type>(indicatorFile.read()) : EOTAType::OTA_GENERIC;
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
	#if defined(ESP8266)
			auto entryPath{entry.fullName()};
	#else
			auto entryPath{entry.path()};
	#endif
			auto isDirectory{entry.isDirectory()};

			entry.close();
	
			if (entryPath && entryPath[0] != '.' && entryPath[0] != 0)
			{
				if (isDirectory)
				{
					if (!removeDirectory(entryPath)) 
						return false;
				}
				else if (!LittleFS.remove(entryPath))
					return false;
			}
		}

		dir.close();
		return LittleFS.rmdir(path);
	}

	bool eraseConfigData()
	{
		auto success{removeDirectory(getNvsDirectory())};
#if defined(ESP8266)
		success &= !ESP.eraseConfig();
#elif defined(ESP32)
		success &= nvs_flash_erase() != ESP_OK;
#endif

		return success;
	}

	std::string getDeviceUuidHex()
	{
	#if defined(ESP32)
		return ksf::to_hex(ESP.getEfuseMac());
	#elif defined(ESP8266)
		return ksf::to_hex(ESP.getChipId());
	#endif
			
		return {};
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
		auto new_low32{static_cast<uint32_t>(millis())};
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
#if defined(ESP32)
		const auto reason {esp_reset_reason()};

		// Handle software restarts (including OTA)
		if (reason == ESP_RST_SW)
			return (otaBootType != EOTAType::NO_OTA) ? otaTypeToString(otaBootType) : PSTR("Software/System restart");

		// Use a switch statement for the remaining reset reasons.
		switch (reason)
		{
			case ESP_RST_POWERON:   return PSTR("Power On");
			case ESP_RST_PANIC:     return PSTR("Exception");
			case ESP_RST_INT_WDT:   return PSTR("Watchdog (interrupt)");
			case ESP_RST_TASK_WDT:  return PSTR("Watchdog (task)");
			case ESP_RST_WDT:       return PSTR("Watchdog (other)");
			case ESP_RST_DEEPSLEEP: return PSTR("Deep-Sleep Wake");
			case ESP_RST_BROWNOUT:  return PSTR("Brownout");
			case ESP_RST_SDIO:      return PSTR("SDIO");
			default:                return PSTR("Unknown");
		}
#elif defined(ESP8266)
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
