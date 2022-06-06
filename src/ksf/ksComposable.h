/*
 *	Copyright (c) 2019-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

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
			ksSafeList<std::shared_ptr<ksComponent>> components;	//< An array with shared_ptr of components (holding main reference).

		public:
			/*
				Instantiates a component of passed type, handing over all template parameters as constructor parameters to the component. 
				Keep in mind that passed parameters must match target component constructor parameters.
			*/
			template <class Type, class... Params>
			std::weak_ptr<Type> addComponent(Params... arg)
			{
				std::shared_ptr<Type> ptr = std::make_shared<Type>(arg...);
				components.queueAdd(ptr);
				return std::weak_ptr<Type>(ptr);
			}

			/*
				Tries to find components of passed template type.

				RTTI variant uses dynamic_pointer_cast, which will respect all C++ underlying mechanisms of pointer casting.
				NON-RTTI variant simply compares individual type (static casting and direct type check, won't respect inheritance).

				@param outComponents - vector of weak pointers to matching components, might be empty as well.
			*/
			template <class Type>
			void findComponents(std::vector<std::weak_ptr<Type>>& outComponents)
			{
				outComponents.clear();

				for (auto& comp : components.getList())
				{
					std::weak_ptr<Type> castedComp_wp = std::dynamic_pointer_cast<Type>(comp);
					if (!castedComp_wp.expired())
						outComponents.push_back(castedComp_wp);
				}
			}

			/*
				Tries to find components by passed template type. See findComponents, as findComponent calls it directly and returns first element.
				@return - weak pointer of component, might be invalid/expired (unable to lock to get shared ptr).
			*/
			template <class Type>
			std::weak_ptr<Type> findComponent()
			{
				std::vector<std::weak_ptr<Type>> comps_wp;
				findComponents<Type>(comps_wp);
				return comps_wp.empty() ? std::weak_ptr<Type>() : comps_wp[0];
			}

			/*
				Queues component to be removed. It will happen on queue synchronization (synchronizeQueues).
				@param component - component to remove.
			*/
			void queueRemoveComponent(const std::shared_ptr<ksComponent>& component);

			/*
				Executes a functor for each component.
				@param functor - Function to execute.
			*/
			void forEachComponent(std::function<bool(const std::shared_ptr<ksComponent>&)>& functor);
	};
}