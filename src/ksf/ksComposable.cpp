#include "ksComposable.h"
#include "ksComponent.h"

namespace ksf 
{
	bool ksComposable::forceRemoveComponent(std::weak_ptr<ksComponent> component)
	{
		if (auto sp = component.lock())
		{
			auto it = std::find(components.begin(), components.end(), sp);

			if (it != components.end())
			{
				it->reset();
				components.erase(it);
				return true;
			}
		}

		return false;
	}

	void ksComposable::forEachComponent(std::function<bool(std::shared_ptr<ksComponent>&)> functor)
	{
		for (auto it = components.begin(); it != components.end(); ++it)
			if (functor(*it) == true)
				break;
	}
}