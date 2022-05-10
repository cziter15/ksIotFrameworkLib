/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

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

#include <WiFiClientSecure.h>

using namespace std::placeholders;

namespace ksf
{
	bool ksMqttConnector::init(ksComposable* owner)
	{
		ksMqttConfigProvider provider;

		provider.init(owner);
		provider.setupMqttConnector(*this);

		return mqttClient != nullptr;
	}

	void ksMqttConnector::setupConnection(const String& broker, const String& port, const String& login, const String& password, const String& prefix, bool secure)
	{
		mqttWifiClient = secure ? std::make_shared<WiFiClientSecure>() : std::make_shared<WiFiClient>();
		mqttClient = std::make_shared<PubSubClient>(*mqttWifiClient.get());

		savedLogin = login;
		savedPassword = password;
		savedPrefix = prefix;

		IPAddress serverIP;

		if (serverIP.fromString(broker.c_str()))
			mqttClient->setServer(serverIP, port.toInt());
		else
			mqttClient->setServer(broker.c_str(), port.toInt());
	}

	void ksMqttConnector::mqttConnectedInternal()
	{
		mqttWifiClient->setNoDelay(true);
		mqttWifiClient->setTimeout(KSF_MQTT_TIMEOUT);
		mqttClient->setCallback(std::bind(&ksMqttConnector::mqttMessageInternal, this, _1, _2, _3));

		onConnected->broadcast();
	}

	void ksMqttConnector::mqttMessageInternal(const char* topic, const uint8_t* payload, uint32_t length)
	{
		if (onMesssage->hasAnyCallbacks())
		{
			String payloadStr((char*)0);
			payloadStr.reserve(length);

			String topicStr(topic);

			if (topicStr.startsWith(savedPrefix))
				topicStr.remove(0, savedPrefix.length());

			for (uint32_t payload_ch_idx = 0; payload_ch_idx < length; ++payload_ch_idx)
				payloadStr += (char)payload[payload_ch_idx];

			onMesssage->broadcast(topicStr, payloadStr);
		}
	}

	void ksMqttConnector::subscribe(const String& topic, bool skipDevicePrefix)
	{
		if (skipDevicePrefix)
			mqttClient->subscribe(topic.c_str());
		else	
			mqttClient->subscribe(String(savedPrefix + topic).c_str());
	}

	void ksMqttConnector::unsubscribe(const String& topic, bool skipDevicePrefix)
	{
		if (skipDevicePrefix)
			mqttClient->unsubscribe(topic.c_str());
		else
			mqttClient->unsubscribe(String(savedPrefix + topic).c_str());
	}

	void ksMqttConnector::publish(const String& topic, const String& payload, bool retain, bool skipDevicePrefix)
	{
		if (skipDevicePrefix)
			mqttClient->publish(topic.c_str(), (const uint8_t*)payload.c_str(), payload.length(), retain);
		else
			mqttClient->publish(String(savedPrefix + topic).c_str(), (const uint8_t*)payload.c_str(), payload.length(), retain);
	}

	bool ksMqttConnector::loop()
	{
		if (!mqttClient->loop())
		{
			if (wasConnected)
			{
				onDisconnected->broadcast();
				wasConnected = false;

				connectionTimeSeconds = 0;
				lastTryReconnectTime = millis();
			}
				
			if (millis() - lastTryReconnectTime > KSF_MQTT_RECONNECT_DELAY)
			{
				if (WiFi.isConnected())
				{
					if (mqttClient->connect(WiFi.macAddress().c_str(), savedLogin.c_str(), savedPassword.c_str()))
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

	bool ksMqttConnector::isConnected() const
	{
		if (!WiFi.isConnected() || !mqttClient)
			return false;

		return mqttClient->connected();
	}
}