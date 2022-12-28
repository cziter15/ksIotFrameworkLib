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

	std::string string_from_progmem(const char* pgm_str_ptr, ssize_t pgm_str_len)
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
}