#include "ksComposable.h"
#include "ksComponent.h"

namespace ksf 
{
	void ksComposable::queueRemoveComponent(const std::shared_ptr<ksComponent>& component)
	{
		components.queueRemove(component);
	}

	void ksComposable::forEachComponent(std::function<bool(std::shared_ptr<ksComponent>&)> functor)
	{
		for (auto it = components.items().begin(); it != components.items().end(); ++it)
			if (functor(*it))
				break;
	}
}