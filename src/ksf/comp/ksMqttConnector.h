/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>
#include <string_view>

#include "../ksComponent.h"
#include "../evt/ksEvent.h"
#include "../ksSimpleTimer.h"

class WiFiClient;
class PubSubClient;
class ksCertFingerprint;

namespace ksf
{
	namespace comps
	{
		class ksWifiConnector;
		/*!
			@brief A component which is responsible for MQTT connection management.
		
			The component is responsible for MQTT prefix handling, connection time counting as well as
			connection status reporting. This component provides public methods that can be used to publish a message.
		
			The user can register event handlers to receive connection state events as well as message arrival event.
		*/
		class ksMqttConnector : public ksComponent
		{
			KSF_RTTI_DECLARATIONS(ksMqttConnector, ksComponent)

			protected:

#if APP_LOG_ENABLED
				ksApplication* app{nullptr};									//!< Application pointer.
#endif

				std::unique_ptr<WiFiClient> wifiClientSp;						//!< Shared pointer to WiFiClient used to connect to MQTT.
				std::unique_ptr<PubSubClient> mqttClientSp;						//!< Shared pointer to PubSubClient used to connect to MQTT.

				std::weak_ptr<ksWifiConnector> wifiConnWp;						//!< Weak pointer to WiFi connector.

				uint64_t lastSuccessConnectionTime{0};							//!< Time of connection to MQTT broker in seconds.
				uint32_t reconnectCounter{0};									//!< MQTT reconnection counter.

				bool sendConnectionStatus : 1 {true};							//!< Send connection status to MQTT or not.
				bool usePersistentSession : 1 {false};							//!< Use persistent session or not.
				bool wasConnected : 1 {false};									//!< True if connected in previous loop.

				ksSimpleTimer reconnectTimer{KSF_MQTT_RECONNECT_DELAY_MS};		//!< Timer that counts time between reconnection attempts.

				std::string login;												//!< Saved MQTT login.
				std::string password;											//!< Saved MQTT password.
				std::string prefix;												//!< Saved MQTT prefix.

				std::unique_ptr<ksCertFingerprint> certFingerprint;				//!< Shared pointer to fingerprint validator.

				/*!
					@brief Connects to the MQTT broker.
     
					@return True on success, false on fail.
				*/
				bool connectToBroker();

				/*!
					@brief Connects to the MQTT broker (internal function).
     
					Binds onMessage callback, calls bound onConnected callbacks and is used to configures parameters.
				*/
				void mqttConnectedInternal();

				/*!
					@brief Called when MQTT message arrives (internal function).
					@param topic Topic that received the message.
					@param payload Payload of the message.
					@param length Length of the payload.
				*/
				void mqttMessageInternal(const char* topic, const uint8_t* payload, uint32_t length);

			public:
				enum class QosLevel
				{
					QOS_AT_LEAST_ONCE,
					QOS_EXACTLY_ONCE
				};

				DECLARE_KS_EVENT(onDeviceMessage, const std::string_view&, const std::string_view&)		//!< onDeviceMessage event that user can bind to.
				DECLARE_KS_EVENT(onAnyMessage, const std::string_view&, const std::string_view&)		//!< onAnyMessage event that user can bind to.

				DECLARE_KS_EVENT(onConnected)															//!< onConnected event that user can bind to.
				DECLARE_KS_EVENT(onDisconnected)														//!< onDisconnected event that user can bind to.

				/*!
					@brief Constructs ksMqttConnector object.
     
					@param sendConnectionStatus Indicates whether last will message should be used to signal device state.
					@param usePersistentSession Indicates whether the session should be persistent.
				*/
				ksMqttConnector(bool sendConnectionStatus = true, bool usePersistentSession = false);

				/*!
					@brief Instantiates the MQTT connector component.
     
					@param owner Pointer to the ksApplication object (parent).
					@return True on success, false on fail.
				*/
				bool init(ksApplication* owner) override;
					
				/*!
					@brief Method that handles component post-initialization.
					
					Used to setup callbacks.

					@param owner Pointer to the parent ksApplication.
					@return True on success, false on fail.
				*/
				bool postInit(ksApplication* owner) override;

				/*!
					@brief Executes MQTT connection logic.
					@return True on success, false on fail.
				*/
				bool loop(ksApplication* app) override;

				/*!
					@brief Retrieves connection state.
					@return True if connected, otherwise false.
				*/
				bool isConnected() const;

				/*!
					@brief Retrieves connection time in seconds.
					@return MQTT connection time in seconds.
				*/
				uint32_t getConnectionTimeSeconds() const;

				/*!
					@brief Retrieves MQTT reconnect counter.
					@return Number of MQTT reconnects.
				*/
				uint32_t getReconnectCounter() const { return reconnectCounter; }

				/*!
					@brief Subscribes to MQTT topic.
					@param topic Topic to subscribe
					@param skipDevicePrefix True if device prefix shouldn't be inserted before passed topic, false otherwise.
					@param qos Quality of service level (QOS).
				*/
				void subscribe(const std::string& topic, bool skipDevicePrefix = false, ksMqttConnector::QosLevel = ksMqttConnector::QosLevel::QOS_AT_LEAST_ONCE);

				/*!
					@brief Unsubscribes the MQTT topic.
					@param topic Topic to be unsubscribed.
					@param skipDevicePrefix True if the device prefix shouldn't be inserted to the topic, false othereide.
				*/
				void unsubscribe(const std::string& topic, bool skipDevicePrefix = false);

				/*!
					@brief Publishes a message to the MQTT topic.
					@param topic Target message`s topic name.
					@param payload Payload to be transmitted.
					@param retain True if this publish should be retained, otherwise false.
					@param skipDevicePrefix True if device prefix shouldn't be inserted to the topic, false otherwise.
				*/
				void publish(const std::string& topic, const std::string& payload, bool retain = false, bool skipDevicePrefix = false);

				/*!
					@brief Sets up MQTT connection.
					@param broker MQTT broker address. Can be IP or hostname
					@param port MQTT broker port
					@param login MQTT user login
					@param password MQTT user password
					@param fingerprint MQTT broker certificate fingerprint - if empty, secure connection won't be used
				*/
				void setupConnection(const std::string& broker, const std::string& port, std::string login, std::string password, std::string prefix, const std::string& fingerprint);
		
				/*!
					@brief Destructor (for uniqueptr purposes).
				*/
				virtual ~ksMqttConnector();
		};
	}
}
