/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
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
			NotInitialized, 	//!< Component is not yet initialized.
			Initialized, 		//!< Component is initialized (init is called on transition from NonInitialized).
			Active, 			//!< Component is "working" (postInit is called on transition from Initialized).
			ToRemove			//!< Component is marked to be removed on the next update (order dependent)
		};
	}

	/*!
		@brief Base component class.

		A component is a part of the application that can be configured and managed by the application. 
		As it uses ksRtti, it must include KSF_RTTI_DECLARATIONS for proper initialization.
	*/
	class ksComponent : public ksRtti
	{
		KSF_RTTI_DECLARATIONS(ksComponent, ksRtti)

		friend class ksApplication;

		protected:
			ksComponentState::TYPE componentState { ksComponentState::NotInitialized }; //!< Holds current state of the component.

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
