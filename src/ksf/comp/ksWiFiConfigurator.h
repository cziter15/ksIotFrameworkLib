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
#include <WString.h>
#include <WiFiManager.h>

namespace ksf
{
	class ksWiFiConfigurator : public ksComponent
	{
		protected:
			class ksComposable* parent = nullptr;
			WiFiManager manager;

			String deviceName;

		public:
			ksWiFiConfigurator(const String& devicePrefixName = String("ksWiFiConfigurator"));
			bool init(class ksComposable* owner) override;
			bool loop() override;
			void postInit() override;
	};
}