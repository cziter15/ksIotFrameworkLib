#include "ksConstants.h"

#ifdef ESP32
#include "SPIFFS.h"
#else
#include "FS.h"
#endif


namespace ksf
{
	const char* gCompileDate = "";

	const char* getCompileDate()
	{
		return gCompileDate;
	}

	void initKsfFramework(const char* compileDate)
	{
		gCompileDate = compileDate;

#ifdef ESP32
		/* Initialize filesystem. */
		SPIFFS.begin(true);
#else
		/* Initialize filesystem. */
		SPIFFS.begin();
#endif
	}
}