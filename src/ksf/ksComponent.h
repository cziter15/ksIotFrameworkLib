/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <memory>
#include "ksConstants.h"

namespace ksf 
{
	class ksComposable;
	class ksComponent
	{
#if KSF_NO_RTTI
		friend class ksComposable;
		protected:
			typeid_t comp_type_id;		//< A part of Type ID functionality, will contain a pointer to instantiated variable (by template).
#endif
		friend class ksApplication;

		public:
			/* Method called to initialize component. */
			virtual bool init(ksComposable* owner) = 0;

			/* Called from application loop. */
			virtual bool loop();

			/* Method called after component initialization. */
			virtual void postInit();
	};
}