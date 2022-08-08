/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksApplication.h"
#include "ksComponent.h"

namespace ksf 
{
	bool ksApplication::init()
	{
		/* Synchronize components added before ksApplication base init. */
		components.synchronizeQueues();

		/* Run initialization */
		if (!forEachComponent([&](const std::shared_ptr<ksComponent>& comp) -> bool { return comp->init(this); }))
			return false;

		/* Run post-init event for components. */
		return forEachComponent([&](const std::shared_ptr<ksComponent>& comp) -> bool {
			comp->postInit(); 
			return true;
		});
	}

	bool ksApplication::loop()
	{
		/* Loop through all components and synchronize list at end of scope. */
		{
			ksf::ksSafeListScopedSync scoped(components);
			if (!forEachComponent([&](const std::shared_ptr<ksComponent>& comp) -> bool { return comp->loop(); } ))
				return false;
		}

		/* This call will keep millis64 on track (handles rollover). */
		updateDeviceUptime();

		return true;
	}
}