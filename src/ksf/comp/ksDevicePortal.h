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
#include <string_view>
#include "ArduinoOTA.h"
#include "../evt/ksEvent.h"
#include "../ksComponent.h"

class DNSServer;

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

namespace ksf::misc
{
	class ksWSServer;
}

namespace ksf::comps
{
	class ksMqttConnector;
	class ksDevicePortal : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksDevicePortal, ksComponent)

		protected:
			ArduinoOTAClass ArduinoOTA;									// Arduino OTA object.
			ksf::ksApplication* app{nullptr};							// Application pointer.
			bool breakApp{false};										// Flag to restart chip.

			uint32_t lastLoopExecutionTimestamp{0};						// Time of last loop execution (us)/
			uint32_t loopExecutionTime{0};								// Diff (loop exec time).
			uint32_t scanNetworkTimestamp{0};							// Timestamp of last scan.

			uint32_t logKeepAliveTimestamp{0};							// Keep alive timestamp.

			std::string portalPassword;									// Portal password.

			std::weak_ptr<ksMqttConnector> mqttConnectorWp;				// MQTT connector.

			std::unique_ptr<WebServerClass> webServer;					// HTTP server.
			std::unique_ptr<misc::ksWSServer> webSocket;				// Web socket server.
			std::unique_ptr<DNSServer> dnsServer;							// DNS server.

			/*
				@brief Causes the app to break from loop with false status.
			*/
			void requestAppBreak() { breakApp = true; }

			/*
				@brief This function starts OTA update server.
			*/
			void setupHttpServer();

			/*
				@brief This function starts WebSocket server.
			*/
			void setupWsServer();

			/*
				@brief This function is called when OTA update is finished.
			*/
			void updateFinished(bool fromPortal); 

			/*
				@brief This function handles device reboot.
			*/
			void rebootDevice();

			/*
				@brief This function handles authentication error.
			*/
			bool inRequest_NeedAuthentication();

			/*
				@brief This function handles "not found" error.
			*/
			void onRequest_notFound() const;

			/*
				@brief This function serves main page to the client.
			*/
			void onRequest_index();

			/*
				@brief This function is called for each OTA chunk.

				Handles endpoint "/api/flash".
			*/
			void onRequest_otaChunk();

			/*
				@brief This function is called when OTA update is finished.

				Handles endpoint "/api/flash".
			*/
			void onRequest_otaFinish();

			/*
				@brief This function will format flash.
			*/
			void handle_formatFS();

			/*
				@brief This function handles incomming websocket messages.
				@param clientNum Client number
				@param message Message string view
			*/
			void onWebsocketTextMessage(uint8_t clientNum, const std::string_view& message);

			/*
				@brief This function handles scanning networks.
				@param response Response reference, where the response should be appended
			*/
			void handle_scanNetworks(std::string& response);

			/*
				@brief This function handles identity request.
				@param response Response reference, where the response should be appended
			*/
			void handle_getIdentity(std::string& response);

			/*
				@brief This function handles device params request.
				@param response Response reference, where the response should be appended
			*/
			void handle_getDeviceParams(std::string& response);

			/*
				@brief This function handles logging.
				@param msgRef Log message
			*/
			void onAppLog(std::string& msgRef);

		public:
			DECLARE_KS_EVENT(onUpdateStart)		// onUpdateStart event that user can bind to.
			DECLARE_KS_EVENT(onUpdateEnd)		// onUpdateEnd event that user can bind to.

			/*
				@brief Constructor, used to construct Device Portal component.

				Uses the default password "ota_ksiotframework"
			*/
			ksDevicePortal();

			/*
				@brief Constructor, used to construct Device Portal component.
				@param password Password used to authenticate the user.
			*/
			ksDevicePortal(const std::string& portalPassword);

			/*
				@brief Initializes device portal component.
				@param app Pointer to ksApplication
				@return True on success, false on fail.
			*/
			bool init(ksApplication* app) override;

			/*
				@brief Post initialization method for device portal with integrated OTA.
				@param app Pointer to ksApplication
				@return True on success, false on fail.
			*/
			bool postInit(ksApplication* app) override;

			/*
				@brief Handles device portal component loop logic.
				@param app Pointer to ksApplication.
				@return True on success, false on fail.
			*/
			bool loop(ksApplication* app) override;

			/*
				@brief Destructor used to cleanup resources.
			*/
			virtual ~ksDevicePortal();
	};
}