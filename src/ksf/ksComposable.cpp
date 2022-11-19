/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksComposable.h"
#include "ksComponent.h"

namespace ksf
{
	void ksComposable::queueComponentForRemoval(const std::shared_ptr<ksComponent>& component)
	{
		components.queueRemove(component);
	}
}