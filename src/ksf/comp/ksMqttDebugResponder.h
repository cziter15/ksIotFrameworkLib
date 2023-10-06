/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string_view>
#include "../evt/ksEvent.h"
#include "../ksComponent.h"

namespace ksf::comps
{
	class ksMqttConnector;
	class ksMqttDebugResponder : public ksComponent, public std::enable_shared_from_this<ksMqttDebugResponder>
	{
		KSF_RTTI_DECLARATIONS(ksMqttDebugResponder, ksComponent)

		protected:
			std::weak_ptr<ksMqttConnector> mqttConnWp;				// Weak pointer to MQTT connector.
			ksApplication* owner{nullptr};							// Pointer to application that owns this component.

			std::unique_ptr<evt::ksEventHandle> connEventHandle;	// Event handle for connection delegate.
			std::unique_ptr<evt::ksEventHandle> msgEventHandle;		// Event handle for message delegate.

			bool breakloop{false};									// Boolean (set by command) to break from app loop.

			/*
				@brief Function called on MQTT connection.
			*/
			void onConnected();

			/*
				@brief Function called on MQTT message arrival.
				@param topic Topic of arrived message
				@param message Message/payload
			*/
			void onMessage(const std::string_view& topic, const std::string_view& message);

		public:
			DECLARE_KS_EVENT(customDebugHandler, ksMqttDebugResponder*, const std::string_view&, bool&) //< Event that provides custom message handler.

			/*
				@brief Method called after component initialization.
				
				Used to setup message callbacks.

				@param owner Pointer to ksApplication object that owns this component
				@return True on success, false on fail.
			*/
			bool postInit(ksApplication* owner) override;

			/*
				@brief Handles MQTT debug connector component loop logic.
				@return True on success, false on fail.
			*/
			bool loop() override;

			/*
				@brief Responds to MQTT debug message. Publishes response message to MQTT debug topic.
				@param message Message string reference
			*/
			void respond(const std::string& message) const;
	};
}