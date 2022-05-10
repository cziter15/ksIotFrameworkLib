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

class WiFiClient;
class PubSubClient;

namespace ksf
{
	class ksMqttConnector : public ksComponent
	{
		protected:
			std::shared_ptr<WiFiClient> mqttWifiClient;		//< Shared pointer to WiFiClient used to connect to MQTT.
			std::shared_ptr<PubSubClient> mqttClient;		//< Shared pointer to PubSubClient (MQTT client).

			uint32_t lastConnectionTimeTick = 0;			//< Last connection tick time (ms since boot).
			uint32_t lastTryReconnectTime = 0;				//< Last connection try time (ms since boot).
			bool wasConnected = false;						//< True if connected in previous loop.

			String savedLogin;								//< Saved MQTT login.
			String savedPassword;							//< Saved MQTT password.
			String savedPrefix;								//< Saved MQTT prefix.

			/*
				Called on MQTT connected (internal function).
				Binds onMessage callback, calls bound onConnected callbacks, configures some parameters.
			*/
			virtual void mqttConnectedInternal();

			/*
				Called on MQTT message arrival (internal function).
				@param topic - message topic.
				@param payload - message payload.
				@param length - payload length.
			*/
			virtual void mqttMessageInternal(const char* topic, const uint8_t* payload, uint32_t length);

		public:
			DECLARE_KS_EVENT(onMesssage, const String&, const String&)		//< onMesssage event that user can bind to.
			DECLARE_KS_EVENT(onConnected)									//< onConnected event that user can bind to.
			DECLARE_KS_EVENT(onDisconnected)								//< onDisconnected event that user can bind to.

			uint32_t connectionTimeSeconds = 0;								//< MQTT connection time counter (in seconds).
			uint32_t reconnectCounter = 0;									//< MQTT reconnection counter.

			/*
				Initializes MQTT connector component.
				@param owner - pointer to owning composable interface (application).
				@return - true on success, false on fail.
			*/
			bool init(class ksComposable* owner) override;

			/*
				Called from application loop. Handles MqttConnector logic.
				@return - true on success, false on fail.
			*/
			bool loop() override;

			/*
				Retrieves connection state.
				@return - true if connected, otherwise false.
			*/
			bool isConnected() const;

			/*
				Subscribes to MQTT topic.
				@param topic - topic to subscribe.
				@param skipDevicePrefix - true if device prefix should be skipped (not concatened / topic = raw topic), otherwise false.
			*/
			virtual void subscribe(const String& topic, bool skipDevicePrefix = false);

			/*
				Un-subscribes to MQTT topic.
				@param topic - topic to unsubscribe.
				@param skipDevicePrefix - true if device prefix should be skipped (not concatened / topic = raw topic), otherwise false.
			*/
			virtual void unsubscribe(const String& topic, bool skipDevicePrefix = false);

			/*
				Publishes payload to MQTT topic.
				@param topic - topic to publish to.
				@param payload - payload to be published.
				@param retain - true if this publish should be retained, otherwise false.
				@param skipDevicePrefix - true if device prefix should be skipped (not concatened / topic = raw topic), otherwise false.
			*/
			virtual void publish(const String& topic, const String& payload, bool retain = false, bool skipDevicePrefix = false);

			/*
				Configures MQTT connection parameters.
				@param broker - MQTT broker IP / domain.
				@param port - MQTT broker port.
				@param login - MQTT user login.
				@param password - MQTT user password.
				@param secure - true if use SSL, otherwise false.
			*/
			virtual void setupConnection(const String& broker, const String& port, const String& login, const String& password, const String& prefix, bool secure = false);
	};
}

