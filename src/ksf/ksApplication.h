/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <memory>
#include <list>
#include <string>
#include <functional>

#include "ksComponent.h"

#if APP_LOG_ENABLED
typedef std::function<void(std::string&)> AppLogProviderFunc_t;
typedef std::function<void(std::string&&)> AppLogCallbackFunc_t;
#endif

namespace ksf 
{
	class ksComponent;

	/*!
		@brief A class that is a base for user-defined application.

		ksApplication implements core application logic. It is responsible for initializing and running components.
		
		You must override init method to build component stack. You can return false to stop the initialization.
		In this case, application will be stopped and ksAppRotator will move to the next application.

		Do not override loop method until you have a good reason. If you do so, keep in mind that you should call base 
		loop method and return the result properly. If any component returns false, application logic should be stopped.
		The best way is to add only small piece of code and return the reult of the base loop method. It is also safe to
		return false early, before calling base loop method.
	
		When init or loop returns false, application will be stopped.
	*/
	class ksApplication
	{
		protected:
			std::list<std::shared_ptr<ksComponent>> components;		//!< An array with shared_ptr of components (holding main reference).

#if APP_LOG_ENABLED
			 AppLogCallbackFunc_t appLogCallback;					//!< Callback function for logging
#endif
		public:

			/*!
				@brief Instantiates a component of the type defined by the template instance.
				This function will pass all template-defined parameters to the component constructor.

				@tparam TComponentType A type of the component.
				@tparam TParams... A list of parameters to be passed to the component constructor.
				
				@param arg... A list of parameters to be passed to the component constructor.

				@return Weak pointer to the created component.
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

			/*!
				@brief Iterates through all components and returns a vector of weak pointers with components that matches the type passed as a template parameter.
				@tparam TComponentType A type of the component to look for.
				@param outComponents A vector of weak pointers to components that match the type passed as a template parameter.
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

			/*!
				@brief  Iterates through all components and returns a weak pointer to the first component that matches the type passed as a template parameter.
				@tparam TComponentType A type of the component to look for.
				@return Weak pointer to the first component that matches the type passed as a template parameter.
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

			/*!
				@brief Initializes application.
				@return True on success, false otherwise (will break application execution).
			*/
			virtual bool init() = 0;

			/*!
				@brief Executes application logic loop.
				@return True on success, false otherwise (will break application execution).
			*/
			virtual bool loop();

#if APP_LOG_ENABLED
			/*!
				@brief Calls log callback function with the string to be logged.
				@param provideLogFn Function that appends a string to be logged using the reference provided as a parameter.
			*/
			void log(AppLogProviderFunc_t provideLogFn) const;

			/*!
				@brief Sets log callback function. Called every time a new log is generated.
				@param logCallback Function that is called after every log - ksDevicePortal uses this function to send logs to the browser when using Terminal feature.
			*/
			void setLogCallback(AppLogCallbackFunc_t logCallback);
#endif
	};
}
