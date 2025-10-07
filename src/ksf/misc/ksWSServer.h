/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <memory>
#include <inttypes.h>
#include <functional>
#include <string_view>
#include <WebSocketsServer.h>

namespace ksf::misc
{
	/*!
		@brief Callback function typedef for Websocket messages.
		@param client Websocket client number that identifies the client.
		@param message Incoming websocket message (text / string view).
	*/
	typedef std::function<void(uint8_t client, std::string_view message)> ksWsServerMessageFunc_t;

	/*!
		@brief A wrapper around WebSocketsServerCore that adds WebSocket authentication and enhanced message handling.
	*/
	class ksWSServer : public WebSocketsServerCore 
	{
		protected:
			std::unique_ptr<WEBSOCKETS_NETWORK_SERVER_CLASS> wsListener;		//!< WS server (listener).
			uint64_t requiredAuthToken{0};										//!< WS auth token.
			ksWsServerMessageFunc_t onWebsocketTextMessage;						//!< Callback function to receive messages.

			/*!
				@brief Handler for non-WebSocket connections on websocket port.
				@param client Pointer to the WSclient_t object.
			*/
			void handleNonWebsocketConnection(WSclient_t* client) override;

		public:
			/*!
				@brief Prepares ksWebServer on specified port without actually starting it.
				@param port Port to listen for incoming WebSocket connections.
			*/
			ksWSServer(uint16_t port);

			/*!
				@brief Destructs the server, releasing the resources.
			*/
			virtual ~ksWSServer();

			/*!
				@brief Returns simple authentication token for WebSocket authentication process.

				The value 0 is considered to be empty token. Authentication token is set via setRequiredAuthToken().

				Generally, this token should be generated earlier, then the WebServer should use Cookie to transmit it to
				the browser and then the browser should pass it with WebSocket requests (via HTTP Cookie header).

				@return Auth token for WebSocket
			*/
			uint64_t getRequiredAuthToken() const;

			/*!
				@brief Sets simple authtoken for WebSocket authentication.

				The value 0 is considered to be invalid (no auth token). Auth token is set via setRequiredAuthToken().
				Generally, this token should be generated earlier, then the WebServer should use Cookie to transmit it to
				the browser and then the browser should pass it with WebSocket requests (via HTTP Cookie header).

				@param authToken The token to set.
			*/
			void setRequiredAuthToken(uint64_t authToken);

			/*!
				@brief Starts the server.
			*/
			void begin();

			/*!
				@brief Handles WebSocket server logic.
			*/
			void loop();

			/*!
				@brief Installs a message handler to receive WebSocket text messages.
				@param func The message handler function.
			*/
			void setMessageHandler(ksWsServerMessageFunc_t func);
	};
}