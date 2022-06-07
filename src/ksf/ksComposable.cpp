/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksComposable.h"
#include "ksComponent.h"

namespace ksf
{
	void ksComposable::queueRemoveComponent(const std::shared_ptr<ksComponent>& component)
	{
		components.queueRemove(component);
	}

	void ksComposable::forEachComponent(std::function<bool(const std::shared_ptr<ksComponent>&)>& function)
	{
		/* Simply iterate and call passed function for each component. */
		for (auto it = components.getList().begin(); it != components.getList().end(); ++it)
			if (function(*it))
				break;
	}
}