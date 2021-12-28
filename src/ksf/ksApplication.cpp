#include "ksApplication.h"
#include "ksComponent.h"

namespace ksf 
{
	bool ksApplication::init()
	{
		// Synchronize components added before ksApplication base init.
		components.synchronizeQueues();

		// Run initialization
		for (auto it = components.items().begin(); it != components.items().end(); ++it)
			if (!(*it)->init(this))
				return false;

		// Run post-init event for components
		for (auto it = components.items().begin(); it != components.items().end(); ++it)
			(*it)->postInit();

		return true;
	}

	bool ksApplication::loop()
	{
		for (auto& comp : components.items())
		{
			if (comp->isMarkedToDestroy)
				components.queueRemove(comp);

			if (!comp->loop())
				return false;
		}

		components.synchronizeQueues();

		return true;
	}
}