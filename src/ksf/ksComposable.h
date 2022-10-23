/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <vector>
#include <functional>
#include <algorithm>
#include "ksSafeList.h"
#include "ksComponent.h"

namespace ksf 
{
	class ksComponent;
	class ksComposable
	{
		protected:
			ksSafeList<std::shared_ptr<ksComponent>> components;	// An array with shared_ptr of components (holding main reference).

		public:
			/*
				Instantiates a component of passed type, handing over all template parameters as constructor parameters to the component. 
				Keep in mind that passed parameters must match target component constructor parameters.
			*/
			template <class _Type, class... _Params>
			std::weak_ptr<_Type> addComponent(_Params... arg)
			{
				auto ptr = std::make_shared<_Type>(arg...);
				components.queueAdd(ptr);
				return std::weak_ptr<_Type>(ptr);
			}

			/*
				Tries to find components of passed template type.

				@param outComponents Vector of weak pointers to be filled with matching components, might be empty as well.
			*/
			template <class _Type>
			void findComponents(std::vector<std::weak_ptr<_Type>>& outComponents)
			{
				outComponents.clear();

				for (const auto& comp : components.getList())
				{
					if (comp->isA(_Type::getClassType()))
					{
						std::weak_ptr<_Type> castedCompWp{std::static_pointer_cast<_Type>(comp)};
						if (!castedCompWp.expired())
							outComponents.push_back(castedCompWp);
					}
				}
			}

			/*
				Tries to find components by passed template type. 
				See findComponents, as findComponent calls it directly and returns first element.

				@return Weak pointer of component, might be invalid/expired (unable to lock to get shared ptr).
			*/
			template <class _Type>
			std::weak_ptr<_Type> findComponent()
			{
				std::vector<std::weak_ptr<_Type>> foundCompsWp;
				findComponents<_Type>(foundCompsWp);
				return foundCompsWp.empty() ? std::weak_ptr<_Type>() : foundCompsWp[0];
			}

			/*
				Queues component to be removed. It will happen on queue synchronization (synchronizeQueues).
				@param component Component to be removed.
			*/
			void queueRemoveComponent(const std::shared_ptr<ksComponent>& component);

			/*
				Executes a function on each component.
				
				@param function Function to be executed. Should return true to continue iteration.
				@return True if successfully iterated through all components, otherwise false.
			*/
			template <typename _Predicate>
			bool forEachComponent(_Predicate function)
			{
				/* Simply iterate and call passed function for each component. */
				for (auto it{components.getList().cbegin()}; it != components.getList().cend(); ++it)
					if (!function(*it)) // If predicate returned false, stop iterating.
						return false;

				return true;
			}
	};
}