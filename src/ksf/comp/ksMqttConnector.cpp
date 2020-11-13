#include "../ksComposable.h"
#include "../ksConstants.h"
#include "../ksConfig.h"
#include "ksMqttConnector.h"
#include "ksMqttConfigProvider.h"
#include <PubSubClient.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

using namespace std::placeholders;

namespace ksf
{
	bool ksMqttConnector::init(ksComposable* owner)
	{
		ksMqttConfigProvider provider;

		provider.init(owner);
		provider.setupMqttConnector(*this);

		return MqttClient != nullptr;
	}

	void ksMqttConnector::setupConnection(String broker, String port, String login, String password, String prefix)
	{
		MqttWifiClient = std::make_shared<WiFiClient>();
		MqttClient = std::make_shared<PubSubClient>(*MqttWifiClient.get());

		saved_login = login;
		saved_password = password;
		saved_prefix = prefix;

		IPAddress serverIP;

		if (serverIP.fromString(broker.c_str()))
			MqttClient->setServer(serverIP, port.toInt());
		else
			MqttClient->setServer(broker.c_str(), port.toInt());
	}

	void ksMqttConnector::mqttConnectedInternal()
	{
		MqttWifiClient->setNoDelay(true);
		MqttWifiClient->setTimeout(KSF_MQTT_TIMEOUT);
		MqttClient->setCallback(std::bind(&ksMqttConnector::mqttMessageInternal, this, _1, _2, _3));

		onConnected.broadcast();
	}

	void ksMqttConnector::mqttMessageInternal(char* topic, unsigned char* payload, unsigned int length)
	{
		String topicStr, payloadStr;
		topicStr = String(topic).substring(saved_prefix.length());

		for (unsigned int payload_ch_idx = 0; payload_ch_idx < length; ++payload_ch_idx)
			payloadStr += (char)payload[payload_ch_idx];

		onMesssage.broadcast(topicStr, payloadStr);
	}

	void ksMqttConnector::subscribe(const String& topic)
	{
		MqttClient->subscribe(String(saved_prefix + topic).c_str());
	}

	void ksMqttConnector::unsubscribe(const String& topic)
	{
		MqttClient->unsubscribe(String(saved_prefix + topic).c_str());
	}

	void ksMqttConnector::publish(const String& topic, const String& payload, bool retain)
	{
		String publishTopic = saved_prefix + topic;
		MqttClient->publish(publishTopic.c_str(), (const uint8_t*)payload.c_str(), payload.length(), retain);
	}

	bool ksMqttConnector::loop()
	{
		if (!MqttClient->loop())
		{
			if (wasConnected)
			{
				onDisconnected.broadcast();
				wasConnected = false;

				connectionTimeSeconds = 0;
				lastTryReconnectTime = millis();
			}
				
			if (millis() - lastTryReconnectTime > KSF_MQTT_RECONNECT_DELAY)
			{
				if (WiFi.isConnected())
				{
					if (MqttClient->connect(WiFi.macAddress().c_str(), saved_login.c_str(), saved_password.c_str()))
					{
						++reconnectCounter;
						wasConnected = true;
						mqttConnectedInternal();
					}

					lastTryReconnectTime = millis();
				}
			}
		}
		else if (millis() - lastConnectionTimeTick > KSF_ONE_SECOND_MS)
		{
			lastConnectionTimeTick = millis();
			++connectionTimeSeconds;
		}

		return true;
	}
}