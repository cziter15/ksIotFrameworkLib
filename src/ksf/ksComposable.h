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
			std::vector<std::shared_ptr<ksComponent>> components;

		public:
			template <class Type, class... Params>
			std::shared_ptr<Type> addComponent(Params...rest)
			{
				std::shared_ptr<Type> ptr = std::make_shared<Type>(rest...);
				components.push_back(ptr);
				return ptr;
			}

			template <class Type>
			std::shared_ptr<Type> addComponent()
			{
				std::shared_ptr<Type> ptr = std::make_shared<Type>();
				components.push_back(ptr);
				return ptr;
			}

			template <class Type>
			void findComponents(std::vector<Type*>& outComponents)
			{
				outComponents.clear();

				for (auto& comp : components)
					if (Type* castedComp = dynamic_cast<Type*>(comp.get()))
						outComponents.push_back(castedComp);
			}

			template <class Type>
			Type* findComponent()
			{
				std::vector<Type*> comps;
				findComponents<Type>(comps);
				return comps.size() > 0 ? comps[0] : nullptr;
			}

			bool removeComponent(std::shared_ptr<ksComponent> component);
			void forEachComponent(std::function<bool(std::shared_ptr<ksComponent>&)> functor);
	};
}
