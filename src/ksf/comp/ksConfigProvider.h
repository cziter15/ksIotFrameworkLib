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
#include "../ksComponent.h"
#include <vector>

class WiFiManager;

namespace ksf 
{
	class ksComposable;
	class ksConfigProvider : public ksComponent
	{
		public:
			bool init(ksComposable* owner) override;
			virtual void injectManagerParameters(WiFiManager& manager) = 0;
			virtual void captureManagerParameters(WiFiManager& manager) = 0;
			bool loop() override;
	};
}
