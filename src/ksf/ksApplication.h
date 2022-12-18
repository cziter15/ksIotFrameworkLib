/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "ksComposable.h"

namespace ksf 
{
	class ksComposable;
	class ksApplication : public ksComposable
	{
		public:
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