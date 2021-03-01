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

			unsigned long lastConnectionTimeTick = 0;
			unsigned long lastTryReconnectTime = 0;
			bool wasConnected = false;

			virtual void mqttConnectedInternal();
			virtual void mqttMessageInternal(char* topic, unsigned char* payload, unsigned int length);

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

			unsigned long connectionTimeSeconds = 0;
			unsigned long reconnectCounter = 0;

			virtual void subscribe(const String& topic);
			virtual void unsubscribe(const String& topic);
			virtual void publish(const String& topic, const String& payload, bool retain = false);

			virtual void setupConnection(String broker, String port, String login, String password, String prefix);
	};
}

