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
			std::shared_ptr<WiFiClient> mqttWifiClient;
			std::shared_ptr<PubSubClient> mqttClient;

			uint32_t lastConnectionTimeTick = 0;
			uint32_t lastTryReconnectTime = 0;
			bool wasConnected = false;

			virtual void mqttConnectedInternal();
			virtual void mqttMessageInternal(const char* topic, const uint8_t* payload, uint32_t length);

			String savedLogin;
			String savedPassword;
			String savedPrefix;

		public:
			bool init(class ksComposable* owner) override;
			bool loop() override;

			bool isConnected() const;

			DECLARE_KS_EVENT(onMesssage, const String&, const String&)
			DECLARE_KS_EVENT(onConnected)
			DECLARE_KS_EVENT(onDisconnected)

			uint32_t connectionTimeSeconds = 0;
			uint32_t reconnectCounter = 0;

			virtual void subscribe(const String& topic, bool skipDevicePrefix = false);
			virtual void unsubscribe(const String& topic, bool skipDevicePrefix = false);
			virtual void publish(const String& topic, const String& payload, bool retain = false, bool skipDevicePrefix = false);

			virtual void setupConnection(const String& broker, const String& port, const String& login, const String& password, const String& prefix, bool secure = false);
	};
}

