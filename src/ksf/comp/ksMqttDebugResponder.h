/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include "../evt/ksEvent.h"
#include <string_view>

namespace ksf
{
	namespace comps
	{
		class ksMqttConnector;
		class ksMqttDebugResponder : public ksf::ksComponent, public std::enable_shared_from_this<ksMqttDebugResponder>
		{
			protected:
				std::weak_ptr<ksMqttConnector> mqttConnWp;				// Weak pointer to MQTT connector.
				ksComposable* owner = nullptr;							// Raw pointer to ownning composable (app).

				std::shared_ptr<evt::ksEventHandle> connEventHandle;	// Event handle for connection delegate.
				std::shared_ptr<evt::ksEventHandle> msgEventHandle;		// Event handle for message delegate.

				bool breakloop = false;									// Boolean (set by command) to break from app loop.

				/*
					Function called on MQTT connection.
				*/
				void onConnected();

				/*
					Function called on MQTT message arrival.

					@param topic Topic of arrived message.
					@param message Message/payload.
				*/
				void onMessage(const std::string_view& topic, const std::string_view& message);

				/*
					Returns last known reset reason.
					@return std::string with reset reason description.
				*/
				std::string getResetReason();

			public:
				DECLARE_KS_EVENT(customDebugHandler, ksMqttDebugResponder*, const std::string_view&, bool&) //< Event that provides custom message handler.

				/*
					Initializes MQTT debug responder component.

					@param owner Pointer to ownning ksComposable object (application).
					@return True on success, false on fail.
				*/
				bool init(class ksf::ksComposable* owner) override;

				/*
					Method called after component initialization.
					Used to setup message callbacks.
				*/
				void postInit() override;

				/*
					Handles MQTT debug connector component loop logic.
					@return True on success, false on fail.
				*/
				bool loop() override;

				/*
					Publishes response to command request.
					@param message Message string reference.
				*/
				void respond(const std::string& message) const;
		};
	}
	
}