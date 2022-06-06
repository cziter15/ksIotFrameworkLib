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

	unsigned long long millis64() 
	{
		static unsigned long low32, high32;
		unsigned long new_low32 = millis();
		if (new_low32 < low32) high32++;
		low32 = new_low32;
		return (unsigned long long) high32 << 32 | low32;
	}
}