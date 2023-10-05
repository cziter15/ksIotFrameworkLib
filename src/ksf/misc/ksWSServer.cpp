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
	const char PROGMEM_COOKIE [] PROGMEM {"Cookie"};

	ksWSServer::ksWSServer(uint16_t port)
	{
		wsListener = std::make_unique<WiFiServer>(port);
	}

	ksWSServer::~ksWSServer()
	{
		wsListener->close();
	}

	void ksWSServer::begin() 
	{
		WebSocketsServerCore::begin();
		wsListener->begin();

		/* Setup headers we want to validate. */
		const char* headerkeys[] 
		{ 
			PROGMEM_COOKIE
		};

		/* Validate cookie set by main index page. */
		onValidateHttpHeader([this](String headerName, String headerValue) {
			if (requriedAuthToken != 0 && headerName.equalsIgnoreCase(PROGMEM_COOKIE))
			{
				String WSACookie(FPSTR("WSA="));
				WSACookie += String(requriedAuthToken);
				return headerValue.indexOf(WSACookie) != -1;
			}
			return true;
		}, headerkeys, sizeof(headerkeys)/sizeof(char*));
	}

	void ksWSServer::loop() 
	{
		if(!_runnning)
			return;

		while (wsListener->hasClient()) 
		{
			if(auto client{new WiFiClient(wsListener->accept())})
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
