/*
 *	Copyright (c) 2021-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksWSServer.h"

namespace ksf::misc
{
	/* Error code returned to the frontend on validation fail. Frontend should reload the page in this case. */
	constexpr auto WEBSOCKET_VALIDATION_FAIL{1008};

	ksWSServer::ksWSServer(uint16_t port)
		: wsListener(std::make_unique<WiFiServer>(port))
	{}

	ksWSServer::~ksWSServer()
	{
		wsListener->close();
	}

	void ksWSServer::begin() 
	{
		/* Setup headers we want to validate. */
		static constexpr char COOKIE_STR[] PROGMEM {"Cookie"};
		const char* headerkeys[] { COOKIE_STR };

		/* Begin with the core. */
		WebSocketsServerCore::begin();

		/* Set onValidateHttpHeader validator, which will validate WSA cookie. */
		onValidateHttpHeader([this](String headerName, String headerValue) {
			if (requriedAuthToken != 0 && headerName.equalsIgnoreCase(COOKIE_STR))
			{
				String WSACookie(FPSTR("WSA="));
				WSACookie += String(requriedAuthToken);
				return headerValue.indexOf(WSACookie) != -1;
			}
			return true;
		}, headerkeys, sizeof(headerkeys)/sizeof(char*));

		/* Setup WStype_TEXT message handler. */
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
		/* Exit if not running. */
		if(!_runnning)
			return;

		/* Listen for new clients. */
		while (wsListener->hasClient()) 
		{
			if (auto client{new WiFiClient(wsListener->accept())})
				handleNewClient(client);
			else break;
		}

		/* Execute core logic. */
		WebSocketsServerCore::loop();
	}

	void ksWSServer::setMessageHandler(ksWsServerMessageFunc_t func)
	{ 
		onWebsocketTextMessage = std::move(func);
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
		/* 
			Needed to implement custom handler here. The string should be the same as in WebSockets library to 
			avoid duplication inside final binary image (this will be optimized out by the linker).
		 */
		String handshake{WEBSOCKETS_STRING(
			"HTTP/1.1 101 Switching Protocols\r\n"
			"Upgrade: websocket\r\n"
			"Connection: Upgrade\r\n"
			"Sec-WebSocket-Version: 13\r\n"
			"Sec-WebSocket-Accept: "
		)};
		
		/* Add accept key. */
		handshake += acceptKey(client->cKey);
		handshake += WEBSOCKETS_STRING("\r\n\r\n");

		/* Send handshake. */
		write(client, (uint8_t *)handshake.c_str(), handshake.length());
		headerDone(client);

		/* Now disconnect with proper error code. Frontend should handle it and reload the page. */
		WebSockets::clientDisconnect(client, WEBSOCKET_VALIDATION_FAIL);
	}
}
