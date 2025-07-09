/*
 *	Copyright (c) 2020-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksComponent.h"

namespace ksf 
{
	bool ksComponent::init(ksApplication* app)
	{
		return true;
	}

	bool ksComponent::loop(ksApplication* app)
	{
		return true;
	}

	bool ksComponent::postInit(ksApplication* app) 
	{
		return true;
	}
}