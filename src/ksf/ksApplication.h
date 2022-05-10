/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
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
				@return - true on success, false on fail (will break application initialization).
			*/
			virtual bool init();

			/* 
				Called every application tick (see ksConstants). 
				@return - true on success, false on fail (will break application execution).
			*/
			virtual bool loop();
	};
}