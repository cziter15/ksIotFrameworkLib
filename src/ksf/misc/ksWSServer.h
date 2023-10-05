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

namespace ksf::misc
{
	class ksWSServer : public WebSocketsServerCore 
	{
		protected:
			std::unique_ptr<WiFiServer> wsListener;								// WS listener.
			uint64_t requriedAuthToken{0};

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

			uint64_t getRequiredAuthToken() const { return requriedAuthToken; }
			void setRequiredAuthToken(uint64_t authToken) { requriedAuthToken = authToken; }

			/*
				Starts the server listening on the specified port.
			*/
			void begin();

			/*
				Handles all WS server logic.
			*/
			void loop();

			/*
				Destructor. Should release all resources.
			*/
			virtual ~ksWSServer();
	};
}