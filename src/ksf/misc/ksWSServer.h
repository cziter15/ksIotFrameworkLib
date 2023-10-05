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
			std::unique_ptr<WiFiServer> wsListener;
			void handleNonWebsocketConnection(WSclient_t * client) override;

		public:
			ksWSServer();
			void begin(uint16_t port);
			void close();
			void loop();
			virtual ~ksWSServer();
	};
}