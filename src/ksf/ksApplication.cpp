#include "ksApplication.h"
#include "ksComponent.h"

namespace ksf 
{
	bool ksApplication::init()
	{
		// Run initialization
		for (auto it = components.begin(); it != components.end(); ++it)
			if (!(*it)->init(this))
				return false;

		// Run post-init event for components
		for (auto it = components.begin(); it != components.end(); ++it)
			(*it)->postInit();

		return true;
	}

	bool ksApplication::loop()
	{
		for (auto it = components.begin(); it != components.end(); ++it)
		{
			// This will erase components that queued themselves for destroy by queueDestroy() method.
			if ((*it)->isMarkedToDestroy)
			{
				components.erase(it);
				continue;
			}

			// If any component loop failed, break application.
			if (!(*it)->loop())
				return false;
		}

		return true;
	}
}