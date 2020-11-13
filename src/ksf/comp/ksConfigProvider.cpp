#include "ksConfigProvider.h"
#include "../ksConfig.h"
#include <WiFiManager.h>

namespace ksf
{
	bool ksConfigProvider::init(ksComposable* owner)
	{
		return true;
	}

	bool ksConfigProvider::loop()
	{
		return true;
	}
}