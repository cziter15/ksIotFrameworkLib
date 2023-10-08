/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "ksConstants.h"
#include "ksRtti.h"

namespace ksf 
{
	class ksApplication;

	namespace ksComponentState 
	{
		enum TYPE 
		{
			NotInitialized,
			Initialized,
			Looping,
			ToBeRemoved
		};
	}

	class ksComponent : public ksRtti
	{
		KSF_RTTI_DECLARATIONS(ksComponent, ksRtti)

		friend class ksApplication;

		protected:
			ksComponentState::TYPE componentState { ksComponentState::NotInitialized };

		public:
			/*
				@brief Initializes component.
				@param owner Pointer to the owning application
				@return True on success, false on fail.
			*/
			virtual bool init(ksApplication* app);

			/*
				Called from application loop.

				@return True on success, false on fail.
			*/
			virtual bool loop(ksApplication* app);

			/*
				@brief Method called after component initialization.
				@param owner Pointer to the owning application
				@return True on success, false on fail.
			*/
			virtual bool postInit(ksApplication* app);
	};
}