/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "../ksConstants.h"
#include "../ksConfig.h"
#include "ksWifiConnector.h"
#include "ksMqttConnector.h"
#include "ksMqttConfigProvider.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

using namespace std::placeholders;

namespace ksf::comps
{
	bool ksMqttConnector::init(ksf::ksComposable* owner)
	{
		ksMqttConfigProvider provider;

		provider.init(owner);
		provider.setupMqttConnector(*this);

		wifiCon_wp = owner->findComponent<ksWifiConnector>();

		return mqttClient != nullptr;
	}

	ksMqttConnector::ksMqttConnector(bool sendConnectionStatus)
		: useConnectionStatusTopic(sendConnectionStatus)
	{}

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

		/* 
		*	There's an inconsistency in setTimeout implementation between Arduino for ESP32 and ESP8266.
		*	ESP32's WiFi client setTimeout method want seconds, while ESP8266's one wants milliseconds.
		*	I've wasted some time to get what's really going on here, but eventually caught that.
		* 
		*	Timeout is required because underlying connect method is blocking. Without timeout it may trigger watchdog.
		*/

		#ifdef ESP32
			mqttWifiClient->setTimeout(KSF_MQTT_TIMEOUT_SEC);
		#else
			mqttWifiClient->setTimeout(KSF_MQTT_TIMEOUT_SEC * KSF_ONE_SECOND_MS);
		#endif
		
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

	bool ksMqttConnector::connectToBroker()
	{
		if (useConnectionStatusTopic)
		{
			String willTopic{savedPrefix + "connected"};
			
			if (mqttClient->connect(WiFi.macAddress().c_str(), savedLogin.c_str(), savedPassword.c_str(), willTopic.c_str(), 0, true, "0"))
			{
				mqttClient->publish(willTopic.c_str(), (const uint8_t*)"1", 1, true);
				return true;
			}

			return false;
		}

		return mqttClient->connect(WiFi.macAddress().c_str(), savedLogin.c_str(), savedPassword.c_str());
	}

	bool ksMqttConnector::loop()
	{
		if (mqttClient->loop())
		{
			if (oneSecTimer.triggered())
				++connectionTimeSeconds;
		}
		else if (wasConnected)
		{
			connectionTimeSeconds = 0;
			reconnectTimer.restart();
			wasConnected = false;
			onDisconnected->broadcast();
		}
		else if (reconnectTimer.triggered())
		{
			if (auto wifiCon_sp = wifiCon_wp.lock())
			{
				if (wifiCon_sp->isConnected() && connectToBroker())
				{
					oneSecTimer.restart();
					++reconnectCounter;
					wasConnected = true;
					mqttConnectedInternal();
				}
			}
		}

		return true;
	}

	bool ksMqttConnector::isConnected() const
	{
		if (mqttClient)
			if (auto wifiCon_sp = wifiCon_wp.lock())
				if (wifiCon_sp->isConnected())
					return mqttClient->connected();
		
		return false;
	}
}