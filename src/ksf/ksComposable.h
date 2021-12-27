#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <memory>

namespace ksf 
{
	class ksComponent;

	class ksComposable
	{
		protected:
			std::vector<std::shared_ptr<ksComponent>> components, stillValidComponents;

		public:
			template <class Type, class... Params>
			std::weak_ptr<Type> addComponent(Params...rest)
			{
				std::shared_ptr<Type> ptr = std::make_shared<Type>(rest...);
				components.push_back(ptr);
				return std::weak_ptr<Type>(ptr);
			}

			template <class Type>
			std::weak_ptr<Type> addComponent()
			{
				std::shared_ptr<Type> ptr = std::make_shared<Type>();
				components.push_back(ptr);
				return ptr;
			}

			template <class Type>
			void findComponents(std::vector<std::weak_ptr<Type>>& outComponents)
			{
				outComponents.clear();

				for (auto& comp : components)
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

			bool forceRemoveComponent(std::weak_ptr<ksComponent> component);
			void forEachComponent(std::function<bool(std::shared_ptr<ksComponent>&)> functor);
	};
}
