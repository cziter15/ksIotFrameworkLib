/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksConstants.h"
#include "LittleFS.h"

#if ESP32
#include <WiFi.h>
#include <esp_phy_init.h>
#elif ESP8266
#include <ESP8266WiFi.h>
#else			
#error Platform not implemented.
#endif

namespace ksf
{
	#define OTA_FILENAME_TEXT_PGM PGM_("/ksf.otabooted")	// Name of the file that indicates that the device was just flashed with OTA.

	static bool bootedFromOta{false};						// Will be true if this launch is just after OTA flash.
	static uint32_t uptime_low32, uptime_high32;			// Variables for assembling 64-bit version of millis.

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

		WiFi.persistent(true);
		WiFi.setAutoConnect(false);
		WiFi.setAutoReconnect(false);

		WiFi.mode(WIFI_OFF);

		bootedFromOta = LittleFS.remove(OTA_FILENAME_TEXT_PGM.c_str());
	}

	void saveOtaBootIndicator()
	{
		auto indicatorFile{LittleFS.open(OTA_FILENAME_TEXT_PGM.c_str(), "w")};
		indicatorFile.close();
	}

	bool isFirstOtaBoot()
	{
		return bootedFromOta;
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

	std::string string_from_progmem(const char* pgm_str_ptr, size_t pgm_str_len)
	{
#if ESP8266
		std::string result;
		// Resize the string to the correct length.
		result.resize(pgm_str_len);
		// Copy the content from PROGMEM to the string.
		memcpy_P(result.data(), pgm_str_ptr, pgm_str_len);
		return result;
#else
		return {pgm_str_ptr};
#endif
	}

	const std::string getResetReason()
	{
		bool otaBoot{ksf::isFirstOtaBoot()};
#if ESP32
		switch (esp_reset_reason())
		{
			case ESP_RST_POWERON:
				return PGM_("Power On");
			case ESP_RST_SW:
				if (otaBoot)
					return PGM_("OTA Update");
				return PGM_("Software/System restart");
			case ESP_RST_PANIC:
				return PGM_("Exception");
			case ESP_RST_INT_WDT:
				return PGM_("Watchdog (interrupt)");
			case ESP_RST_TASK_WDT:
				return PGM_("Watchdog (task)");
			case ESP_RST_WDT:
				return PGM_("Watchdog (other)");
			case ESP_RST_DEEPSLEEP:
				return PGM_("Deep-Sleep Wake");
			case ESP_RST_BROWNOUT:
				return PGM_("Brownout");
			case ESP_RST_SDIO:
				return PGM_("SDIO");
			default:
				return PGM_("Unknown");
		}
#elif ESP8266
		if (otaBoot && ESP.getResetInfoPtr()->reason == REASON_SOFT_RESTART)
			return PGM_("OTA Update");
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
}