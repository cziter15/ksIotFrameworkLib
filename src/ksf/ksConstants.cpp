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
	constexpr auto otaBootIndicatorFile{"/ksf.otabooted"};		// Name of the OTA boot indicator file.
	static bool bootedFromOta{false};							// Will be true if this launch is just after OTA flash.

	static uint32_t uptime_low32, uptime_high32;				// Variables for assembling 64-bit version of millis.

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

		bootedFromOta = LittleFS.remove(otaBootIndicatorFile);
	}

	void saveOtaBootIndicator()
	{
		auto indicatorFile{LittleFS.open(otaBootIndicatorFile, "w")};
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

	std::string string_from_progmem(const char* pgm_ptr)
	{
		std::string result;

		ssize_t size{strlen_P(pgm_ptr)};
		result.resize(size);
		memcpy_P(result.data(), pgm_ptr, size);

		return result;
	}
}