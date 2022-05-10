/*    
 *	Copyright (c) 2019-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 *
 */

#pragma once
#include "ksComposable.h"

namespace ksf
{
	class ksComposable;
}

namespace ksf 
{
	class ksApplication : public ksComposable
	{
		public:
			/* Method called to initialize application. */
			virtual bool init();

			/* Called every application tick (see ksConstants). */
			virtual bool loop();
	};
}