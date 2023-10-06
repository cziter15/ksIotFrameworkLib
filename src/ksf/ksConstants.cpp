/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */


#if ESP32
	#include <WiFi.h>
	#include <esp_phy_init.h>
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
	const char OTA_FILENAME_TEXT[] PROGMEM {"/ksf.otabooted"};			// File name for OTA boot indicator.
	const char WIFI_CRED_FILENAME_TEXT[] PROGMEM {"/ksf.wificred"};		// File name for WiFi credentials.

	const char SSID_PARAM_NAME[] PROGMEM {"ssid"};						// Param name from progmem - ssid
	const char PASSWORD_PARAM_NAME[] PROGMEM {"password"};				// Param name from progmem - password

	static EOTAType::Type otaBootType{EOTAType::NO_OTA};				// Will be true if this launch is just after OTA flash.
	static uint32_t uptime_low32, uptime_high32;						// Variables for assembling 64-bit version of millis.

	void initializeFramework()
	{
#ifdef ESP32
		/* Initialize filesystem. */
		LittleFS.begin(true);
#endif

#if ESP8266
		/* Initialize filesystem. */
		LittleFS.begin();
#endif

		WiFi.persistent(false);
		WiFi.mode(WIFI_OFF);
		WiFi.setAutoConnect(false);
		WiFi.setAutoReconnect(false);
		
		auto indicatorFile{LittleFS.open(OTA_FILENAME_TEXT, "r")};
		if (indicatorFile)
		{
			otaBootType = indicatorFile.size() == 0 ? EOTAType::OTA_GENERIC : static_cast<EOTAType::Type>(indicatorFile.read());
			indicatorFile.close();
			LittleFS.remove(OTA_FILENAME_TEXT);
		}
	}

	void saveOtaBootIndicator(EOTAType::Type type)
	{
		auto indicatorFile{LittleFS.open(OTA_FILENAME_TEXT, "w")};
		if (indicatorFile)
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
		if (new_low32 < uptime_low32) uptime_high32++;
		uptime_low32 = new_low32;
	}

	uint64_t millis64() 
	{
		return (uint64_t) uptime_high32 << 32 | uptime_low32;
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

	const std::string getUptimeString()
	{
		auto uptime{millis64()};
		return 	to_string(uptime / 1000 / 60 / 60 / 24) + "d " + 
				to_string(uptime / 1000 / 60 / 60 % 24) + "h " + 
				to_string(uptime / 1000 / 60 % 60) + "m " + 
				to_string(uptime / 1000 % 60) + "s";
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
			config_file.setParam(PASSWORD_PARAM_NAME, std::move(ssid));
		}
	}
}