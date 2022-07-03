/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <inttypes.h>

namespace ksf 
{
	class ksSimpleTimer
	{
		protected:
			uint32_t interval = 0;
			uint32_t lastTriggerTime = 0;

		public:
			ksSimpleTimer(uint32_t intervalMs);
			void setInterval(uint32_t intervalMs);
			void restart();
			bool triggered();
	};
}