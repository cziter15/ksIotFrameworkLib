/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "ksRtti.h"

namespace ksf 
{
	class ksApplication;

	namespace ksComponentState 
	{
		enum TYPE 
		{
			/*!
				Component is not yet initialized.
			*/
			NotInitialized,
			/*!
				Component is initialized (init is called on transition from NonInitialized).
			*/
			Initialized,
			/*!
				Component is "working" (postInit is called on transition from Initialized).
			*/
			Active,
			/*!
				Component is marked to be removed. It will be removed within the next
				component update, which can occur in current or next application loop. 
				It depends on the order of components in the list.
			*/
			ToRemove
		};
	}

	/*!
		@brief Base component class.

		A component is a part of the application that can be configured and managed by the application.
		As it is using ksRtti, it must use KSF_RTTI_DECLARATIONS for proper initialization.
	*/
	class ksComponent : public ksRtti
	{
		KSF_RTTI_DECLARATIONS(ksComponent, ksRtti)

		friend class ksApplication;

		protected:
			/*! Current component state. */
			ksComponentState::TYPE componentState { ksComponentState::NotInitialized };

		public:
			/*!
				@brief Initializes component.
				@param app Pointer to the parent ksApplication.
				@return True on success, false on fail.
			*/
			virtual bool init(ksApplication* app);

			/*!
				@brief Handles component loop logic, called from application loop.
				@param app Pointer to the parent ksApplication.
				@return True on success, false on fail.
			*/
			virtual bool loop(ksApplication* app);

			/*!
				@brief Method called after component initialization, used to setup references to other components.
				@param app Pointer to the parent ksApplication.
				@return True on success, false on fail.
			*/
			virtual bool postInit(ksApplication* app);
	};
}
