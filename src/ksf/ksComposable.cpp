#include "ksComposable.h"
#include "ksComponent.h"

namespace ksf 
{
	void ksComposable::queueRemoveComponent(const std::shared_ptr<ksComponent>& component)
	{
		components.queueRemove(component);
	}

	void ksComposable::forEachComponent(std::function<bool(const std::shared_ptr<ksComponent>&)>& functor)
	{
		for (auto it = components.getList().begin(); it != components.getList().end(); ++it)
			if (functor(*it))
				break;
	}
}