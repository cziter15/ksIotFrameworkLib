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
#include <cstdint>
#include <functional>
#include <string_view>
#include <cstddef>

namespace ksf::misc
{
	/*!
		@brief Callback function typedef for Websocket messages.
		@param client Websocket client number that identifies the client.
		@param message Incoming websocket message (text / string view).
	*/
	typedef std::function<void(uint8_t client, std::string_view message)> ksWsServerMessageFunc_t;

	/*!
		@brief A WebSocket server with authentication and text message handling.
	*/
	class ksWSServer
	{
		private:
			class Impl;
			std::unique_ptr<Impl> impl;

		public:
			/*!
				@brief Prepares ksWSServer on specified port without actually starting it.
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

			/*! @brief Sends a text message to one client. A zero length uses a null-terminated payload. */
			bool sendTXT(uint8_t client, const char* payload, std::size_t length = 0);

			/*! @brief Sends a text message to all clients. A zero length uses a null-terminated payload. */
			bool broadcastTXT(const char* payload, std::size_t length = 0);

			/*! @brief Configures heartbeat intervals in milliseconds and the missed-pong limit. */
			void enableHeartbeat(uint32_t pingInterval, uint32_t pongTimeout, uint8_t disconnectTimeoutCount);
	};
}
