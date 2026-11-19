/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include <WebSocketsServer.h>
#include <utility>

#include "ksWSServer.h"

namespace ksf::misc
{
	class ksWSServer::Impl : public WebSocketsServerCore
	{
		private:
			std::unique_ptr<WEBSOCKETS_NETWORK_SERVER_CLASS> wsListener;
			uint64_t requiredAuthToken{0};
			ksWsServerMessageFunc_t onWebsocketTextMessage;

			void handleNonWebsocketConnection(WSclient_t* client) override;

		public:
			Impl(uint16_t port);
			~Impl() override;
			void begin();
			void loop();
			void setMessageHandler(ksWsServerMessageFunc_t func);
			uint64_t getRequiredAuthToken() const;
			void setRequiredAuthToken(uint64_t authToken);
	};

	/* Error code returned to the frontend on validation fail. Frontend should reload the page in this case. */
	constexpr auto WEBSOCKET_VALIDATION_FAIL{1008};

	ksWSServer::Impl::Impl(uint16_t port)
		: wsListener(std::make_unique<WiFiServer>(port))
	{}

	ksWSServer::Impl::~Impl()
	{
		wsListener->close();
	}

	void ksWSServer::Impl::begin()
	{
		/* Setup headers we want to validate. */
		static constexpr char COOKIE_STR[] PROGMEM {"Cookie"};
		const char* headerkeys[] { COOKIE_STR };

		/* Begin with the core. */
		WebSocketsServerCore::begin();

		/* Set onValidateHttpHeader validator, which will validate WSA cookie. */
		onValidateHttpHeader([this](String headerName, String headerValue) {
			if (requiredAuthToken != 0 && headerName.equalsIgnoreCase(COOKIE_STR))
			{
				String WSACookie(FPSTR("WSA="));
				WSACookie += String(requiredAuthToken);
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

	void ksWSServer::Impl::loop()
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

	void ksWSServer::Impl::setMessageHandler(ksWsServerMessageFunc_t func)
	{
		onWebsocketTextMessage = std::move(func);
	}

	uint64_t ksWSServer::Impl::getRequiredAuthToken() const
	{
		return requiredAuthToken;
	}

	void ksWSServer::Impl::setRequiredAuthToken(uint64_t authToken)
	{
		requiredAuthToken = authToken;
	}

	void ksWSServer::Impl::handleNonWebsocketConnection(WSclient_t * client)
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

	ksWSServer::ksWSServer(uint16_t port)
		: impl(std::make_unique<Impl>(port))
	{}

	ksWSServer::~ksWSServer() = default;

	void ksWSServer::begin()
	{
		impl->begin();
	}

	void ksWSServer::loop()
	{
		impl->loop();
	}

	uint64_t ksWSServer::getRequiredAuthToken() const
	{
		return impl->getRequiredAuthToken();
	}

	void ksWSServer::setRequiredAuthToken(uint64_t authToken)
	{
		impl->setRequiredAuthToken(authToken);
	}

	void ksWSServer::setMessageHandler(ksWsServerMessageFunc_t func)
	{
		impl->setMessageHandler(std::move(func));
	}

	bool ksWSServer::sendTXT(uint8_t client, const char* payload, std::size_t length)
	{
		return impl->sendTXT(client, payload, length);
	}

	bool ksWSServer::broadcastTXT(const char* payload, std::size_t length)
	{
		return impl->broadcastTXT(payload, length);
	}

	void ksWSServer::enableHeartbeat(uint32_t pingInterval, uint32_t pongTimeout, uint8_t disconnectTimeoutCount)
	{
		impl->enableHeartbeat(pingInterval, pongTimeout, disconnectTimeoutCount);
	}
}
