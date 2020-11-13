#include "ksComposable.h"
#include "ksComponent.h"

namespace ksf 
{
	bool ksComposable::removeComponent(std::shared_ptr<ksComponent> component)
	{
		auto it = std::find(components.begin(), components.end(), component);
	
		if (it != components.end())
		{
			it->reset();
			components.erase(it);
			return true;
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