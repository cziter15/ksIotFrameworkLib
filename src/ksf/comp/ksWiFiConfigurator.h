/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
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
			class ksComposable* parent = nullptr;	//< Raw pointer to ownning composable (app).
			WiFiManager manager;				 	//< WiFiManager.

			String deviceName;					 	//< Device name - user prefix plus Chip ID. Used as config AP name and WiFi hostname.

		public:
			/*
				Constructs ksWiFiConfigurator.
				@param devicePrefixName - Device prefix name, will be used in AP name like MY_DEVICE_PREFIX-112ACB84 and hostname in WiFi class.
			*/
			ksWiFiConfigurator(const String& devicePrefixName = String("ksWiFiConfigurator"));

			/*
				Initializes ksWiFiConfigurator component.
				@param owner - Pointer to owner composable (application).
				@return - true if init succedeed, otherwise false.
			*/
			bool init(class ksComposable* owner) override;

			/*
				ksWiFiConfigurator component loop. Keep in mind that this function is blocking and returns false.
				It's a little bit exception, but WiFiManager works this way that you start config portal and have to wait for results.
				After configuration portal operation or timeout it will unblock and reach 'return false' which will break config application and
				if properly implemented, will jump to target application.
				@return - true if loop succedeed, otherwise false.
			*/
			bool loop() override;

			/*
				ksWiFiConfigurator postInit function.
				Turns on all registered LEDs in config mode.
			*/
			void postInit() override;
	};
}