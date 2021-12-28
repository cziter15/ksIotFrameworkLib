#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <memory>
#include "ksSafeList.h"

namespace ksf 
{
	class ksComponent;

	class ksComposable
	{
		protected:
			ksSafeList<std::shared_ptr<ksComponent>> components;

		public:
			template <class Type, class... Params>
			std::weak_ptr<Type> addComponent(Params...rest)
			{
				std::shared_ptr<Type> ptr = std::make_shared<Type>(rest...);
				components.queueAdd(ptr);
				return std::weak_ptr<Type>(ptr);
			}

			template <class Type>
			std::weak_ptr<Type> addComponent()
			{
				std::shared_ptr<Type> ptr = std::make_shared<Type>();
				components.queueAdd(ptr);
				return ptr;
			}

			void queueRemoveComponent(const std::shared_ptr<ksComponent>& component);

			template <class Type>
			void findComponents(std::vector<std::weak_ptr<Type>>& outComponents)
			{
				outComponents.clear();

				for (auto& comp : components.items())
				{
					std::weak_ptr<Type> castedComp_wp = std::dynamic_pointer_cast<Type>(comp);

					if (!castedComp_wp.expired())
					{
						outComponents.push_back(castedComp_wp);
					}
				}
			}

			template <class Type>
			std::weak_ptr<Type> findComponent()
			{
				std::vector<std::weak_ptr<Type>> comps_wp;
				findComponents<Type>(comps_wp);
				return comps_wp.size() > 0 ? comps_wp[0] : std::weak_ptr<Type>();
			}

			void forEachComponent(std::function<bool(std::shared_ptr<ksComponent>&)> functor);
	};
}
