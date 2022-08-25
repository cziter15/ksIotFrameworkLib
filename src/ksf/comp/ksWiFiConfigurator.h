/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include <WiFiManager.h>
#include <string>

namespace ksf
{
	namespace comps 
	{
		class ksWiFiConfigurator : public ksComponent
		{
			KS_RTTI_DECLARATIONS(ksWiFiConfigurator, ksComponent)

			protected:
				class ksf::ksComposable* owner{nullptr};		// Raw pointer to ownning composable (app).
				WiFiManager manager;				 			// WiFiManager object.
				std::string deviceName;					 		// Device name - prefix plus Chip ID. Used as config AP name and hostname.

			public:
				/*
					Constructs ksWiFiConfigurator.
					@param devicePrefixName Device prefix name, will be used in AP (DEVPREFIX-112ACB84) and hostname.
				*/
				ksWiFiConfigurator(const std::string& devicePrefixName = "ksWiFiConfigurator");

				/*
					Initializes ksWiFiConfigurator component.
					@param owner Pointer to ownning ksComposable object (application).
					@return True if init succedeed, otherwise false.
				*/
				bool init(class ksf::ksComposable* owner) override;

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
				*/
				void postInit() override;
		};
	}
}