/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksConstants.h"

#ifdef ESP32
#include "SPIFFS.h"
#else
#include "FS.h"
#endif


namespace ksf
{
	static unsigned long uptime_low32, uptime_high32;

	void initKsfFramework()
	{
#ifdef ESP32
		/* Initialize filesystem. */
		SPIFFS.begin(true);
#else
		/* Initialize filesystem. */
		SPIFFS.begin();
#endif
	}

	void updateDeviceUptime()
	{
		unsigned long new_low32 = millis();
		if (new_low32 < uptime_low32) uptime_high32++;
		uptime_low32 = new_low32;
	}

	unsigned long long millis64() 
	{
		return (unsigned long long) uptime_high32 << 32 | uptime_low32;
	}
}