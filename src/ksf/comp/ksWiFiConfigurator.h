/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>
#include "../ksComponent.h"
#include "../ksSimpleTimer.h"
namespace ksf::comps
{
	class ksWiFiConfigurator : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksWiFiConfigurator, ksComponent)

		protected:
			ksApplication* owner{nullptr};					//  Pointer to ksApplication object that owns this component.
			std::string deviceName;							// Device name (prefix).
			ksf::ksSimpleTimer configTimeout{120 * 1000};	// Timeout for captive portal in ms.

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
				ksWiFiConfigurator init function.
				@param owner Pointer to the application object.
				@return True on success, false on fail.
			*/
			bool init(ksApplication* owner) override;

			/*
				ksWiFiConfigurator postInit function.
				Turns on all registered LEDs in config mode.

				@param owner Pointer to ksApplication object that owns this component.
				@return True on success, false on fail.
			*/
			bool postInit(ksApplication* owner) override;

			/*
				Destructor for WiFi configurator component.
			*/
			virtual ~ksWiFiConfigurator();
	};
}