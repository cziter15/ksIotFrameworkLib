/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
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
	void ksApplication::markComponentToRemove(const std::shared_ptr<ksComponent> component)
	{
		components.remove(std::move(component));
	}

	bool ksApplication::init()
	{
		/* Synchronize components added before ksApplication base init. */
		components.applyPendingOperations();

		/* Run initialization */
		if (!forEachComponent([&](const std::shared_ptr<ksComponent>& comp) { return comp->init(this); }))
			return false;

		/* Run post-init event for components. */
		return forEachComponent([](const std::shared_ptr<ksComponent>& comp) {
			comp->postInit(); 
			return true;
		});
	}

	bool ksApplication::loop()
	{
		/* Loop through all components and synchronize list at end of scope. */
		{
			ksf::ksSafeListScopedSync{components};
			if (!forEachComponent([](const std::shared_ptr<ksComponent>& comp) { return comp->loop(); } ))
				return false;
		}

		/* This call will keep millis64 on track (handles rollover). */
		updateDeviceUptime();

		return true;
	}
}