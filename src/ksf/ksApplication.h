/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <memory>
#include "ksSafeList.h"
#include "ksComponent.h"

namespace ksf 
{
	class ksComponent;
	class ksApplication
	{
		protected:
			ksSafeList<std::shared_ptr<ksComponent>> components;	// An array with shared_ptr of components (holding main reference).

		public:
			/*
				Instantiates a component of passed type, handing over all template parameters as constructor parameters to the component. 
				Keep in mind that passed parameters must match target component constructor parameters.

				@return Weak pointer to the newly created component.
			*/
			template <class _Type, class... _Params>
			std::weak_ptr<_Type> addComponent(_Params... arg)
			{
				static_assert(!std::is_same<decltype(&ksComponent::getInstanceType), decltype(&_Type::getInstanceType)>::value, 
					"Trying to add component without RTTI implemented. Did you miss KSF_RTTI_DECLARATIONS?"
				);

				auto sharedPtr{std::make_shared<_Type>(arg...)};
				auto weakPtr{std::weak_ptr<_Type>(sharedPtr)};
				components.add(std::move(sharedPtr));
				return weakPtr;
			}

			/*
				Looks for all components of passed type and puts them into passed vector.

				@param outComponents Vector of weak pointers to components of passed type.
			*/
			template <class _Type>
			void findComponents(std::vector<std::weak_ptr<_Type>>& outComponents)
			{
				static_assert(!std::is_same<decltype(&ksComponent::getInstanceType), decltype(&_Type::getInstanceType)>::value, 
					"Trying to find components without RTTI implemented. Did you miss KSF_RTTI_DECLARATIONS?"
				);

				outComponents.clear();

				for (const auto& comp : components.getRef())
				{
					if (comp->isA(_Type::getClassType()))
					{
						std::weak_ptr<_Type> castedCompWp{std::static_pointer_cast<_Type>(comp)};
						if (!castedCompWp.expired())
							outComponents.emplace_back(std::move(castedCompWp));
					}
				}
			}

			/*
				Looks for a component of passed type and returns a weak pointer to it.

				@return Weak pointer to the component of passed type.
			*/
			template <class _Type>
			std::weak_ptr<_Type> findComponent()
			{
				static_assert(!std::is_same<decltype(&ksComponent::getInstanceType), decltype(&_Type::getInstanceType)>::value, 
					"Trying to find component without RTTI implemented. Did you miss KSF_RTTI_DECLARATIONS?"
				);

				for (const auto& comp : components.getRef())
					if (comp->isA(_Type::getClassType()))
						return std::static_pointer_cast<_Type>(comp);

				return std::weak_ptr<_Type>();
			}

			/*
				Iterates through all components and calls passed function for each of them.
				
				@param function Function to be called for each component. It must return bool and accept a shared_ptr to ksComponent as a parameter.
				@return True if all components were iterated, false if function returned false for any of them.
			*/
			template <typename _Predicate>
			bool forEachComponent(_Predicate function)
			{
				/* Simply iterate and call passed function for each component. */
				for (auto it{components.getRef().cbegin()}; it != components.getRef().cend(); ++it)
					if (!function(*it)) // If predicate returned false, stop iterating.
						return false;

				return true;
			}

			/*
				Mark a component to be removed. It will be removed on next update cycle.
				This is a safe way to remove a component from a composable object.

				@param component Component to be removed.
			*/
			void markComponentToRemove(const std::shared_ptr<ksComponent> component);

			/* 
				Initializes application.

				@return True on success, false on fail (will break application execution).
			*/
			virtual bool init();

			/* 
				Executes application loop.

				@return True on success, false on fail (will break application execution).
			*/
			virtual bool loop();
	};
}