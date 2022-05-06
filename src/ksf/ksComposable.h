#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include <memory>
#include "ksSafeList.h"
#include "ksConstants.h"

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
				#if KSF_NO_RTTI
					ptr->comp_type_id = get_type_id<Type>();
				#endif
				components.queueAdd(ptr);
				return std::weak_ptr<Type>(ptr);
			}

			template <class Type>
			std::weak_ptr<Type> addComponent()
			{
				std::shared_ptr<Type> ptr = std::make_shared<Type>();
				#if KSF_NO_RTTI
					ptr->comp_type_id = get_type_id<Type>();
				#endif
				components.queueAdd(ptr);
				return ptr;
			}

			void queueRemoveComponent(const std::shared_ptr<ksComponent>& component);

			template <class Type>
			void findComponents(std::vector<std::weak_ptr<Type>>& outComponents)
			{
				outComponents.clear();

				for (auto& comp : components.getList())
				{
					#if KSF_NO_RTTI
						std::weak_ptr<Type> castedComp_wp = std::static_pointer_cast<Type>(comp);
						if (!castedComp_wp.expired() && castedComp_wp.lock()->comp_type_id == get_type_id<Type>())
							outComponents.push_back(castedComp_wp);
					#else
						std::weak_ptr<Type> castedComp_wp = std::dynamic_pointer_cast<Type>(comp);
						if (!castedComp_wp.expired())
							outComponents.push_back(castedComp_wp);
					#endif
				}
			}

			template <class Type>
			std::weak_ptr<Type> findComponent()
			{
				std::vector<std::weak_ptr<Type>> comps_wp;
				findComponents<Type>(comps_wp);
				return comps_wp.empty() ? std::weak_ptr<Type>() : comps_wp[0];
			}

			void forEachComponent(std::function<bool(const std::shared_ptr<ksComponent>&)>& functor);
	};
}