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
#include <list>
#include "ksComponent.h"

namespace ksf 
{
	class ksComponent;
	class ksApplication
	{
		protected:
			std::list<std::shared_ptr<ksComponent>> components;	// An array with shared_ptr of components (holding main reference).
		public:
			/*
				@brief Instantiates a component of passed type.

				This function will pass all template-defined parameters as constructor parameters to the component. 
				Keep in mind that passed parameters must match target component constructor parameters.

				@param ... - user (template) defined parameters

				@return Weak pointer to the newly created component.
			*/
			template <class TComponentType, class... TParams>
			std::weak_ptr<TComponentType> addComponent(TParams... arg)
			{
				static_assert(!std::is_same_v<decltype(&ksComponent::getInstanceType), decltype(&TComponentType::getInstanceType)>, 
					"You're calling addComponent, but provided type lacks RTTI implementation. Did you miss KSF_RTTI_DECLARATIONS?"
				);

				auto componentSp{std::make_shared<TComponentType>(arg...)};
				auto componentWp{std::weak_ptr<TComponentType>(componentSp)};
				components.push_front(std::move(componentSp));
				return componentWp;
			}

			/*
				@brief Looks for all components of passed type and puts them into passed vector.
				@param outComponents Vector of weak pointers to components of passed type.
			*/
			template <class TComponentType>
			void findComponents(std::vector<std::weak_ptr<TComponentType>>& outComponents)
			{
				static_assert(!std::is_same_v<decltype(&ksComponent::getInstanceType), decltype(&TComponentType::getInstanceType)>, 
					"You're calling findComponents, but provided type lacks RTTI implementation. Did you miss KSF_RTTI_DECLARATIONS?"
				);

				outComponents.clear();

				for (const auto& comp : components)
				{
					if (comp->isA(TComponentType::getClassType()))
					{
						std::weak_ptr<TComponentType> castedCompWp{std::static_pointer_cast<TComponentType>(comp)};
						if (!castedCompWp.expired())
							outComponents.push_back(std::move(castedCompWp));
					}
				}
			}

			/*
				@brief Looks for a component of passed type and returns a weak pointer to it.
				@return Weak pointer to the component of passed type.
			*/
			template <class TComponentType>
			std::weak_ptr<TComponentType> findComponent()
			{
				static_assert(!std::is_same_v<decltype(&ksComponent::getInstanceType), decltype(&TComponentType::getInstanceType)>, 
					"You're calling findComponent, but provided type lacks RTTI implementation. Did you miss KSF_RTTI_DECLARATIONS?"
				);

				for (const auto& comp : components)
					if (comp->isA(TComponentType::getClassType()))
						return std::static_pointer_cast<TComponentType>(comp);

				return std::weak_ptr<TComponentType>();
			}

			/*
				@brief Initializes application.
				@return True on success, false on fail (will break application execution).
			*/
			virtual bool init() = 0;

			/*
				@brief Executes application loop.
				@return True on success, false on fail (will break application execution).
			*/
			virtual bool loop();
	};
}