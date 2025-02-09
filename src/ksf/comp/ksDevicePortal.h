/*
 *	Copyright (c) 2021-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>
#include <string_view>

#include "../evt/ksEvent.h"
#include "../ksComponent.h"

class DNSServer;
class ArduinoOTAClass;

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

	/*!
		@brief A component thet implements web-based configuration portal accessible from local network.

		The portal can be used to upload OTA update, both in access point and station mode.
		There's also web-based terminal which can be used to debug the device and examine realitme application logs.

		In case of confiugrator application, please don't add this component directly and use ksWifiConfigurator instead.
		In case of "normal application", you have to add the component manually if you want to use it.
	*/
	class ksDevicePortal : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksDevicePortal, ksComponent)

		protected:
			ksf::ksApplication* app{nullptr};							//!< Application pointer.
			struct{
				bool breakApp : 1;										//!< Flag to break app logic.
			} bitflags = {false};

			uint32_t logKeepAliveTimestamp{0};							//!< Flag indicating whether logs are enabled.
			uint32_t lastLoopExecutionTimestamp{0};						//!< Time of last loop execution (us)/
			uint32_t loopExecutionTime{0};								//!< Diff (loop exec time).
			uint32_t scanNetworkTimestamp{0};							//!< Timestamp of last scan.

			std::string portalPassword;									//!< Portal password.
			std::weak_ptr<ksMqttConnector> mqttConnectorWp;				//!< MQTT connector.

			std::unique_ptr<WebServerClass> webServer;					//!< HTTP server.
			std::unique_ptr<misc::ksWSServer> webSocket;				//!< Web socket server.
			std::unique_ptr<DNSServer> dnsServer;						//!< DNS server.
			std::unique_ptr<ArduinoOTAClass> arduinoOTA;				//!< Arduino OTA object.

			/*!
				@brief Causes the application exit (and ksAppRotator to spawn and process next defined application).
			*/
			void requestAppBreak() { bitflags.breakApp = true; }

			/*!
				@brief Triggers factory reset (erase config and reboot).
			*/
			void triggerFactoryReset();

			/*!
				@brief Starts device portal HTTP server.
			*/
			void setupHttpServer();

			/*!
				@brief Starts device portal WebSocket server.
			*/
			void setupWsServer();

			/*!
				@brief Implements post-OTA update actions.
			*/
			void updateFinished(bool fromPortal); 

			/*!
				@brief Reboots the device.
			*/
			void rebootDevice();

			/*!
				@brief  Checks if current HTTP request requires authentication.
			*/
			bool inRequest_NeedAuthentication();

			/*!
				@brief HTTP handler for 404 "not found" endpoint.

				Called when the browser requests an endpoint that doesn't exist.
			*/
			void onRequest_notFound() const;

			/*!
				@brief HTTP handler for index endpoint.

				Called when the browser requests "/" endpoint. This is the default endpoint.
			*/
			void onRequest_index();

			/*!
				@brief HTTP handler for OTA chunk endpoint.

				Called when the browser requests "/api/flash" endpoint with next firmware upload part.
				Used also to notify the browser about firmware uplaod progress.
			*/
			void onRequest_otaChunk();

			/*!
				@brief HTTP handler for OTA finish endpoint.

				Called when the browser requests "/api/flash" endpoint and the upload is complete.
				Used also to tell the browser that the upload is complete (or failed).
			*/
			void onRequest_otaFinish();

			/*!
				@brief Handles websocket text message.
				@param clientNum Number of Websocket client (used as ID to know who's the sender).
				@param message String view of the message content.
			*/
			void onWebsocketTextMessage(uint8_t clientNum, const std::string_view& message);

			/*!
				@brief Websocket handler for network scan endpoint.
				@param response Response reference. This method will output network scan results in JSON format into this string.
			*/
			void handle_scanNetworks(std::string& response);

			/*!
				@brief Websocket handler for identity endpoint (device details).
				@param response Response reference. This method will output device details in JSON format into this string.
			*/
			void handle_getIdentity(std::string& response);

			/*!
				@brief Websocket handler for device parameters endpoint.
				@param response Response reference . This method will output device configuration parameters into this string.
			*/
			void handle_getDeviceParams(std::string& response);

			/*!
				@brief Websocket handler for user commands endpoint.
				@param body Command string from the browser.
				@return String with commadn execution result.
			*/
			std::string handle_executeCommand(const std::string_view& body);

			/*!
				@brief Broadcasts application log message to all connected Websocket clients.
				@param message R-value reference of the message to broadcast.
			*/
			void onAppLog(std::string&& message);

		public:
			/*!
				Update start (OTA) event. No parameters.
			*/
			DECLARE_KS_EVENT(onUpdateStart)
			/*!
				Update end (OTA) event. No parameters.
			*/
			DECLARE_KS_EVENT(onUpdateEnd)
			/*!
				Custom command handler event.
				@param param_1 String view of the command coming from the browser.
				@param param_2 Reference of boolean value indicating if the command was handled. Set to true to stop further processing and return response.
				@param param_3 Reference of the string containing the command response. Append results here.
			*/
			DECLARE_KS_EVENT(onHandlePortalCommand, const std::string_view&, bool&, std::string&)

			/*!
				@brief Constructs the Device Portal component.

				Uses the default password "ota_ksiotframework"
			*/
			ksDevicePortal();

			/*!
				@brief Constructs the Device Portal component with custom password.

				@param password Password used to authenticate the user.
			*/
			ksDevicePortal(std::string portalPassword);

			/*!
				@brief Initializes the device portal component.
				@param app Pointer to the parent ksApplication.
				@return True on success, false otherwise.
			*/
			bool init(ksApplication* app) override;

			/*!
				@brief Post-initializes the device portal component.
				@param app Pointer to the parent ksApplication.
				@return True on success, false otherwise.
			*/
			bool postInit(ksApplication* app) override;

			/*!
				@brief Handles core logic of the device portal component.
				@param app Pointer to the parent ksApplication.
				@return True on success, false otherwise.
			*/
			bool loop(ksApplication* app) override;

			/*!
				@brief Destructs the Device Portal component.
			*/
			virtual ~ksDevicePortal();
	};
}