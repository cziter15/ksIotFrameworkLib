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