#include "ksApplication.h"
#include "ksComponent.h"

namespace ksf 
{
	bool ksApplication::init()
	{
		/* Synchronize components added before ksApplication base init. */
		components.synchronizeQueues();

		/* Run initialization */
		for (auto it = components.getList().begin(); it != components.getList().end(); ++it)
			if (!(*it)->init(this))
				return false;

		/* Run post-init event for components. */
		for (auto it = components.getList().begin(); it != components.getList().end(); ++it)
			(*it)->postInit();

		return true;
	}

	bool ksApplication::loop()
	{
		{
			/* Loop through all components and synchronize list at end of scope. */
			ksf::ksSafeListScopedSync scoped(components);

			for (auto& comp : components.getList())
			{
				if (!comp->loop())
					return false;
			}
		}

		return true;
	}
}