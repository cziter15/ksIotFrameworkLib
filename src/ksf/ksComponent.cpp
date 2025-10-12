/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksComponent.h"

namespace ksf 
{
	bool ksComponent::init([[maybe_unused]] ksApplication* app)
	{
		return true;
	}

	bool ksComponent::loop([[maybe_unused]] ksApplication* app)
	{
		return true;
	}

	bool ksComponent::postInit([[maybe_unused]] ksApplication* app) 
	{
		return true;
	}
}