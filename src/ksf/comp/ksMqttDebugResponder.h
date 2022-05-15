/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include "../evt/ksEvent.h"
#include <WString.h>

namespace ksf
{
	namespace comps
	{
		class ksMqttConnector;
		class ksMqttDebugResponder : public ksf::ksComponent, public std::enable_shared_from_this<ksMqttDebugResponder>
		{
			protected:
				uint32_t secondTimer = 0;							//< Timestamp used to count seconds (ms).
				uint32_t deviceUptimeSeconds = 0;					//< Device uptime in seconds.

				std::weak_ptr<ksMqttConnector> mqtt_wp;				//< Weak pointer to MQTT connector.
				ksComposable* app = nullptr;						//< Raw pointer to ownning composable (app).

				std::shared_ptr<evt::ksEventHandle> connEventHandle;		//< Event handle for connection delegate.
				std::shared_ptr<evt::ksEventHandle> msgEventHandle;		//< Event handle for message delegate.

				bool breakloop = false;								//< Boolean (settable by command) to break execution from loop.

				/*
					Function called on MQTT connection.
				*/
				virtual void onConnected();

				/*
					Function called on MQTT message.
					@param topic - topic of arrived message.
					@param message - message/payload.
				*/
				virtual void onMessage(const String& topic, const String& message);

				/*
					Returns last known reset reason.
					@return - string describing reset reason.
				*/
				String getResetReason();

			public:
				DECLARE_KS_EVENT(customDebugHandler, ksMqttDebugResponder*, const String&, bool&) //< Event that provides custom message handler.

				/*
					Initializes MQTT debug responder component.
					@param owner - pointer to owning composable interface (application).
					@return - true on success, false on fail.
				*/
				virtual bool init(class ksf::ksComposable* owner) override;

				/*
					Method called after component initialization.
					Used to setup message callbacks.
				*/
				virtual void postInit() override;

				/*
					Handles MQTT debug connector component loop logic.
					@return - true on success, false on fail.
				*/
				virtual bool loop() override;

				/*
					Publishes response to command request.
				*/
				void respond(const String& message) const;
		};
	}
	
}