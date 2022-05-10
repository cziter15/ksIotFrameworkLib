/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

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