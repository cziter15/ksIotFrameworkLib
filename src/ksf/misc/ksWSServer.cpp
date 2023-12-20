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

		
		onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
			if (type == WStype_TEXT && onWebsocketTextMessage) {
				auto sv{std::string_view(reinterpret_cast<char*>(payload), length)};
				onWebsocketTextMessage(num, std::move(sv));
			}
		});

		/* Start WS server that listens for incoming clients. */
		wsListener->begin();
	}

	void ksWSServer::loop() 
	{
		if(!_runnning)
			return;

		/* Listen for new clients. */
		while (wsListener->hasClient()) 
		{
			auto client{new WiFiClient(wsListener->accept())};
			if (!client)
				break;
			
			handleNewClient(client);
		}

		WebSocketsServerCore::loop();
	}

	void ksWSServer::setMessageHandler(ksWsServerMessageFunc_t func)
	{ 
		onWebsocketTextMessage = func;
	}

	uint64_t ksWSServer::getRequiredAuthToken() const 
	{ 
		return requriedAuthToken; 
	}

	void ksWSServer::setRequiredAuthToken(uint64_t authToken) 
	{ 
		requriedAuthToken = authToken; 
	}

	void ksWSServer::handleNonWebsocketConnection(WSclient_t * client) 
	{
		clientDisconnect(client);
	}
}
