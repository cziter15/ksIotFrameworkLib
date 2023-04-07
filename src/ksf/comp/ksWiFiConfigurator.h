/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <WiFiManager.h>
#include <string>
#include "../ksComponent.h"

namespace ksf::comps
{
	class ksWiFiConfigurator : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksWiFiConfigurator, ksComponent)

		protected:
			ksApplication* owner{nullptr};		//  Pointer to ksApplication object that owns this component.
			WiFiManager manager;				// WiFiManager object.
			std::string deviceName;				// Device name (prefix).

		public:
			/*
				Constructs WiFi configurator object.
			*/
			ksWiFiConfigurator();

			/*
				Constructs WiFi configurator object.

				@param devicePrefixName Device prefix name, will be used in AP (DEVPREFIX-112ACB84) and hostname.
			*/
			ksWiFiConfigurator(std::string devicePrefixName);

			/*
				ksWiFiConfigurator component loop. Keep in mind that this function is blocking and returns false at the end.
				WiFiManager captive portal is blocking this loop and will unblock on timeout or after setup causing config
				application to break and jump into next one. This usually means it will jump from configuration application
				into target application that provides real device functionality.

				@return True if loop succedeed, otherwise false.
			*/
			bool loop() override;

			/*
				ksWiFiConfigurator postInit function.
				Turns on all registered LEDs in config mode.

				@param owner Pointer to ksApplication object that owns this component.
			*/
			void postInit(ksApplication* owner) override;
	};
}