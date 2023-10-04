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

#include <string_view>

class DNSServer;
class WebSocketsServer;

#if defined(ESP32)
	class WebServer;
	#define WebServerClass WebServer
#elif defined(ESP8266)
	class WiFiServer;
	namespace esp8266webserver
	{
		template<typename ServerType = WiFiServer> class ESP8266WebServerTemplate;
	}
	#define WebServerClass esp8266webserver::ESP8266WebServerTemplate<WiFiServer>
#endif
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

			std::unique_ptr<WebServerClass> webServer;
			std::unique_ptr<WebSocketsServer> webSocket;
			std::unique_ptr<DNSServer> dnsServer;

			void requestAppBreak() { breakApp = true; }

			/*
				This function starts OTA update server.
			*/
			void setupUpdateWebServer();

			/*
				This function is called when OTA update is finished.
			*/
			void updateFinished(bool fromPortal); 

			/*
				This function handles device reboot.
			*/
			void rebootDevice();

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
				This function handles endpoint "/api/flash".
				It is called when OTA update is started.
			*/
			void onRequest_otaChunk();

			/*
				This function handles endpoint "/api/flash".
				It is called when OTA update is finished.
			*/
			void onRequest_otaFinish();


			void onRequest_formatFS();

			void onWebsocketTextMessage(uint8_t clientNum, std::string_view message);

			void handle_scanNetworks(std::string& response);
			void handle_getIdentity(std::string& response);
			void handle_getDeviceParams(std::string& response);

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