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
		for (auto& comp : components)
		{
			if (!comp->isMarkedToDestroy)
				stillValidComponents.push_back(comp);

			if (!comp->loop())
				return false;
		}

		components = std::move(stillValidComponents);
		stillValidComponents.clear();

		return true;
	}
}