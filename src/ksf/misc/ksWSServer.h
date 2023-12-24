/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <WebSocketsServer.h>
#include <WiFiServer.h>
#include <functional>
#include <string_view>

namespace ksf::misc
{
	/*
		@brief Callback function type for Websocket messsages.
		@param client Websocket client ID.
		@param message Incomming websocket message (text / string view).
	*/
	typedef std::function<void(uint8_t client, std::string_view message)> ksWsServerMessageFunc_t;

	class ksWSServer : public WebSocketsServerCore 
	{
		protected:
			std::unique_ptr<WiFiServer> wsListener;								// WS server (listener).
			uint64_t requriedAuthToken{0};										// WS auth token.
			ksWsServerMessageFunc_t onWebsocketTextMessage;						// Callback function to receive messages.

			/*
				@brief Handles a situation wher the socket is not authorized
				@param client WSclient_t *  ptr to the client struct
			*/
			void handleNonWebsocketConnection(WSclient_t * client) override;

		public:
			/*
				@brief Prepares ksWebServer on specified port but does not start it (call begin).
				@param port WebSocket listening port.
			*/
			ksWSServer(uint16_t port);

			/*
				@brief Returns simple authtoken for WebSocket authentication.

				The value 0 is considered to be invalid (no auth token). Auth token is set via setRequiredAuthToken().
				Generally, this token should be generated earlier, then the WebServer should use Cookie to transmit it to
				the browser and then the browser should pass it with WebSocket requests (via HTTP Cookie header).

				@return Auth token for WebSocket
			*/
			uint64_t getRequiredAuthToken() const;

			/*
				@brief Sets simple authtoken for WebSocket authentication.

				The value 0 is considered to be invalid (no auth token). Auth token is set via setRequiredAuthToken().
				Generally, this token should be generated earlier, then the WebServer should use Cookie to transmit it to
				the browser and then the browser should pass it with WebSocket requests (via HTTP Cookie header).

				@param authToken The token to set
			*/
			void setRequiredAuthToken(uint64_t authToken);

			/*
				Starts the server listening on the specified port.
			*/
			void begin();

			/*
				@brief Handles server logic.
			*/
			void loop();

			/*
				@brief Installs a message handler callback to WebSocket text receive messages.
				@param func Callback function to receive messages
			*/
			void setMessageHandler(ksWsServerMessageFunc_t func);

			/*
				@brief Releases server resources.
			*/
			virtual ~ksWSServer();
	};
}