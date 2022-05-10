/*    
 *	Copyright (c) 2019-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 *
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
}