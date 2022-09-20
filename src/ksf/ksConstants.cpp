/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksConstants.h"
#include "LittleFS.h"

#if ESP8266
	#include <ESP8266WiFi.h>
#endif
namespace ksf
{
	static uint32_t uptime_low32, uptime_high32;

	void initializeFramework()
	{
		#ifdef ESP32
			/* Initialize filesystem. */
			LittleFS.begin(true);
		#endif

		#if ESP8266
			/* Initialize filesystem. */
			LittleFS.begin();

			/*
				This is workaround for issue that esp8266 doesn't get properly notified about WiFi disconnection.
				See more: https://github.com/esp8266/Arduino/issues/7432
			*/
			WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected & event) {
				WiFi.disconnect();
			});
		#endif
	}

	void updateDeviceUptime()
	{
		uint32_t new_low32 = millis();
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
}