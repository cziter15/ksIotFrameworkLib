/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksWSServer.h"

namespace ksf::misc
{
	ksWSServer::ksWSServer() = default;
	ksWSServer::~ksWSServer() = default;

	void ksWSServer::begin(uint16_t port) 
	{
		WebSocketsServerCore::begin();
		wsListener = std::make_unique<WiFiServer>(port);
		wsListener->begin();
	}

	void ksWSServer::close() 
	{
		WebSocketsServerCore::close();
		wsListener.reset();
	}

	void ksWSServer::loop(void) 
	{
		if(!_runnning)
			return;

		while(wsListener->hasClient()) 
		{
			if(auto client{new WiFiClient(wsListener->available())})
				handleNewClient(client);
			else break;
		}

		WebSocketsServerCore::loop();
	}

	void ksWSServer::handleNonWebsocketConnection(WSclient_t * client) 
	{
		clientDisconnect(client);
	}
}
