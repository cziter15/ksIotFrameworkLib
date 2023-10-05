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

typedef std::function<void(uint8_t client, std::string_view message)> ksWsServerMessageFunc_t;
namespace ksf::misc
{
	class ksWSServer : public WebSocketsServerCore 
	{
		protected:
			std::unique_ptr<WiFiServer> wsListener;								// WS server (listener).
			uint64_t requriedAuthToken{0};										// WS auth token.
			ksWsServerMessageFunc_t onWebsocketTextMessage;						// Callback function to receive messages.

			/*
				Handles a situation wher the socket is not authorized

				@param client WSclient_t *  ptr to the client struct
			*/
			void handleNonWebsocketConnection(WSclient_t * client) override;

		public:
			/*
				Creates WS server object, but does not start the server.
			*/
			ksWSServer(uint16_t port);

			/*
				Returns auth token used for WebSocket authentication.
				The value 0 is considered to be invalid (no auth token).
			*/
			uint64_t getRequiredAuthToken() const;

			/*
				Sets auth token used for WebSocket authentication.
				The value 0 is considered to be invalid (no auth token).

				@param authToken Token to use for authentication
			*/
			void setRequiredAuthToken(uint64_t authToken);

			/*
				Starts the server listening on the specified port.
			*/
			void begin();

			/*
				Handles all WS server logic.
			*/
			void loop();

			/*
				Sets message callback for WS.

				@param func Callback function to receive messages.
			*/
			void setMessageHandler(ksWsServerMessageFunc_t func);

			/*
				Destructor. Should release all resources.
			*/
			virtual ~ksWSServer();
	};
}