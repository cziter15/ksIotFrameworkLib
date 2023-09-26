/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "ArduinoOTA.h"
#include "../evt/ksEvent.h"
#include "../ksComponent.h"

class AsyncWebServer;
class AsyncWebServerRequest;
class DNSServer;
namespace ksf::comps
{
	class ksDevicePortal : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksDevicePortal, ksComponent)

		protected:
			ArduinoOTAClass ArduinoOTA;						// Arduino OTA object.
			ksApplication* owner{nullptr};					// Pointer to ksApplication.

			bool breakApp{false};							// Flag to break app loop.
			std::string password;							// OTA password.
			
			unsigned long rebootRequestMillis{0};			// Last reboot request time.

			std::shared_ptr<AsyncWebServer> server;			// Web server.
			std::shared_ptr<DNSServer> dnsServer;			// DNS server.

			/*
				This function starts OTA update server.
			*/
			void setupUpdateWebServer();

			/*
				This function is called when OTA update is finished.
			*/
			void updateFinished();

		public:
			DECLARE_KS_EVENT(onUpdateStart)					// onUpdateStart event that user can bind to.
			DECLARE_KS_EVENT(onUpdateEnd)					// onUpdateEnd event that user can bind to.

			/*
				Constructor, used to construct OTA updater component.
				Uses the default password "ota_ksiotframework"
			*/
			ksDevicePortal();

			/*
				Constructor, used to construct OTA updater component.
				
				@param password Password required to flash.
			*/
			ksDevicePortal(const std::string& password);

			/*
				Initializes device portal component.

				@param owner Pointer to the application object.
				@return True on success, false on fail.
			*/
			bool init(ksApplication* owner) override;

			/*
				Post initialization method for device portal with integrated OTA.
			*/
			bool postInit(ksApplication* owner) override;

			/*
				Handles device portal component loop logic.

				@return True on success, false on fail.
			*/
			bool loop() override;
	};
}