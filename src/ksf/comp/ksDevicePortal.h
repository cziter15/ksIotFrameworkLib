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

class DNSServer;

namespace ksf::comps
{
	class ksDevicePortal : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksDevicePortal, ksComponent)

		protected:
			ArduinoOTAClass ArduinoOTA;							// Arduino OTA object.
			ksApplication* owner{nullptr};						// Pointer to ksApplication.

			std::string password;								// OTA password.
			bool breakApp{false};								// Flag to restart chip.

			DNSServer* dnsServer{nullptr};						// DNS server.
			void* _webServer{nullptr};							// Web server.

			template <typename ServerType>
			ServerType* serverAs() const
			{
				return static_cast<ServerType*>(_webServer);
			}

			/*
				This function starts OTA update server.
			*/
			void setupUpdateWebServer();

			/*
				This function is called when OTA update is finished.
			*/
			void updateFinished(); 

			/*
				This function handles authentication error.
			*/
			bool inRequest_NeedAuthentication() const;

			/*
				This function handles "not found" error.
			*/
			void onRequest_notFound() const;

			/*
				This function serves main page to the client.
			*/
			void onRequest_index() const;

			/*
				This function handles endpoint "/api/online".
				It is called by webpage to check if device is online.
			*/
			void onRequest_online() const;

			/*
				This function handles endpoint "/api/getIdentity"
				It returns device identity to the client.
			*/
			void onRequest_getIdentity() const;

			/*
				This function handles endpoint "/api/getDeviceParams"
				It returns device parameters to the client.
			*/
			void onRequest_getDeviceParams() const;

			/*
				This function handles endpoint "/api/saveConfig"
				It is called when user wants to save configuration.
			*/
			void onRequest_saveConfig();

			/*
				This function handles endpoint "/api/flash".
				It is called when OTA update is started.
			*/
			void onRequest_otaChunk();

			/*
				This function handles endpoint "/api/flash".
				It is called when OTA update is finished.
			*/
			void onRequest_otaFinish();

			/*
				This function handles endpoint "/api/scanNetworks"
				It returns a list of  wireless networks to the client.
			*/
			void onRequest_scanNetworks();

			/*
				This function handles endpoint "/api/goToConfigMode"
				It is called when user wants to go to configuration mode (AP).
			*/
			void onRequest_goToConfigMode();

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

			/*
				Destructor used to cleanup resources.
			*/
			virtual ~ksDevicePortal();
	};
}