/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include "../evt/ksEvent.h"
#include "../ksSimpleTimer.h"

class WiFiClient;
class PubSubClient;

namespace ksf
{
	namespace comps
	{
		class ksWifiConnector;
		class ksMqttConnector : public ksf::ksComponent
		{
			protected:
				std::shared_ptr<WiFiClient> mqttWifiClient;						// Shared pointer to WiFiClient used to connect to MQTT.
				std::shared_ptr<PubSubClient> mqttClient;						// Shared pointer to PubSubClient (MQTT client).

				std::weak_ptr<ksWifiConnector> wifiCon_wp;						// Weak pointer to WiFi connector.

				uint32_t connectionTimeSeconds = 0;								// MQTT connection time counter (in seconds).
				uint32_t reconnectCounter = 0;									// MQTT reconnection counter.

				ksSimpleTimer oneSecTimer{KSF_ONE_SECOND_MS};					// Timer that counts seconds.
				ksSimpleTimer reconnectTimer{KSF_MQTT_RECONNECT_DELAY_MS};		// Timer used to reconnect MQTT.

				bool wasConnected = false;										// True if connected in previous loop.

				String savedLogin;												// Saved MQTT login.
				String savedPassword;											// Saved MQTT password.
				String savedPrefix;												// Saved MQTT prefix.

				/*
					Called on MQTT connected (internal function).
					Binds onMessage callback, calls bound onConnected callbacks, configures some parameters.
				*/
				void mqttConnectedInternal();

				/*
					Called on MQTT message arrival (internal function).

					@param topic Message topic.
					@param payload Message payload.
					@param length Payload length.
				*/
				void mqttMessageInternal(const char* topic, const uint8_t* payload, uint32_t length);

			public:
				DECLARE_KS_EVENT(onMesssage, const String&, const String&)		// onMesssage event that user can bind to.
				DECLARE_KS_EVENT(onConnected)									// onConnected event that user can bind to.
				DECLARE_KS_EVENT(onDisconnected)								// onDisconnected event that user can bind to.

				/*
					Initializes MQTT connector component.

					@param owner Pointer to ownning ksComposable object (application).
					@return True on success, false on fail.
				*/
				bool init(class ksf::ksComposable* owner) override;

				/*
					Called from application loop. Handles MqttConnector logic.
					@return True on success, false on fail.
				*/
				bool loop() override;

				/*
					Retrieves connection state.
					@return True if connected, otherwise false.
				*/
				bool isConnected() const;

				/*
					Retrieves connection time in seconds.
					@return MQTT connection time in seconds.
				*/
				uint32_t getConnectionTimeSeconds() const { return connectionTimeSeconds; }

				/*
					Retrieves MQTT reconnect counter.
					@return Number of MQTT reconnects.
				*/
				uint32_t getReconnectCounter() const { return reconnectCounter; }

				/*
					Subscribes to MQTT topic.

					@param topic Topic to subscribe.
					@param skipDevicePrefix True if device prefix shouldn't be inserted before passed topic, otherwise false.
				*/
				void subscribe(const String& topic, bool skipDevicePrefix = false);

				/*
					Unsubscribes to MQTT topic.

					@param topic Topic to unsubscribe.
					@param skipDevicePrefix True if device prefix shouldn't be inserted before passed topic, otherwise false.
				*/
				void unsubscribe(const String& topic, bool skipDevicePrefix = false);

				/*
					Publishes payload to MQTT topic.

					@param topic Topic to publish to.
					@param payload Payload to be published.
					@param retain True if this publish should be retained, otherwise false.
					@param skipDevicePrefix True if device prefix shouldn't be inserted before passed topic, otherwise false.
				*/
				void publish(const String& topic, const String& payload, bool retain = false, bool skipDevicePrefix = false);

				/*
					Configures MQTT connection parameters.

					@param broker MQTT broker IP / domain.
					@param port MQTT broker port.
					@param login MQTT user login.
					@param password MQTT user password.
					@param secure True if use SSL, otherwise false.
				*/
				void setupConnection(const String& broker, const String& port, const String& login, const String& password, const String& prefix, bool secure = false);
		};
	}
}