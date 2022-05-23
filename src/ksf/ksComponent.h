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
		friend class ksApplication;

		public:
			/*
				Initializes component.
				@param owner - pointer to owning composable interface (application).
				@return - true on success, false on fail.
			*/
			virtual bool init(ksComposable* owner) = 0;

			/* 
				Called from application loop.
				@return - true on success, false on fail.
			*/
			virtual bool loop();

			/* 
				Method called after component initialization. 
			*/
			virtual void postInit();
	};
}