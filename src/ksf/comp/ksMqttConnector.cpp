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
		ksMqttConfigProvider cfgProvider;

		cfgProvider.init(owner);
		cfgProvider.setupMqttConnector(*this);

		wifiConnWp = owner->findComponent<ksWifiConnector>();

		/*
			Object mqttClientSp is created by setupConnection method.
			That means init will return false when no MQTT config file found.
		*/
		return mqttClientSp != nullptr;
	}

	ksMqttConnector::ksMqttConnector(bool sendConnectionStatus)
		: sendConnectionStatus(sendConnectionStatus)
	{}

	void ksMqttConnector::setupConnection(const String& broker, const String& port, const String& login, const String& password, const String& prefix, bool secure)
	{
		wifiClientSp = secure ? std::make_shared<WiFiClientSecure>() : std::make_shared<WiFiClient>();
		mqttClientSp = std::make_shared<PubSubClient>(*wifiClientSp.get());

		this->login = login;
		this->password = password;
		this->prefix = prefix;

		IPAddress serverIP;

		if (serverIP.fromString(broker.c_str()))
			mqttClientSp->setServer(serverIP, port.toInt());
		else
			mqttClientSp->setServer(broker.c_str(), port.toInt());
	}

	void ksMqttConnector::mqttConnectedInternal()
	{
		wifiClientSp->setNoDelay(true);

		/* 
		*	There's an inconsistency in setTimeout implementation between Arduino for ESP32 and ESP8266.
		*	ESP32's WiFi client setTimeout method want seconds, while ESP8266's one wants milliseconds.
		*	I've wasted some time to get what's really going on here, but eventually caught that.
		* 
		*	Timeout is required because underlying connect method is blocking. Without timeout it may trigger watchdog.
		*/

		#ifdef ESP32
			wifiClientSp->setTimeout(KSF_MQTT_TIMEOUT_SEC);
		#else
			wifiClientSp->setTimeout(KSF_MQTT_TIMEOUT_SEC * KSF_ONE_SECOND_MS);
		#endif
		
		mqttClientSp->setCallback(std::bind(&ksMqttConnector::mqttMessageInternal, this, _1, _2, _3));
		onConnected->broadcast();
	}

	void ksMqttConnector::mqttMessageInternal(const char* topic, const uint8_t* payload, uint32_t length)
	{
		if (onMesssage->hasAnyCallbacks())
		{
			String payloadStr((char*)0);
			payloadStr.reserve(length);

			String topicStr(topic);

			if (topicStr.startsWith(prefix))
				topicStr.remove(0, prefix.length());

			for (uint32_t payload_ch_idx = 0; payload_ch_idx < length; ++payload_ch_idx)
				payloadStr += (char)payload[payload_ch_idx];

			onMesssage->broadcast(topicStr, payloadStr);
		}
	}

	void ksMqttConnector::subscribe(const String& topic, bool skipDevicePrefix)
	{
		if (skipDevicePrefix)
			mqttClientSp->subscribe(topic.c_str());
		else	
			mqttClientSp->subscribe(String(prefix + topic).c_str());
	}

	void ksMqttConnector::unsubscribe(const String& topic, bool skipDevicePrefix)
	{
		if (skipDevicePrefix)
			mqttClientSp->unsubscribe(topic.c_str());
		else
			mqttClientSp->unsubscribe(String(prefix + topic).c_str());
	}

	void ksMqttConnector::publish(const String& topic, const String& payload, bool retain, bool skipDevicePrefix)
	{
		if (skipDevicePrefix)
			mqttClientSp->publish(topic.c_str(), (const uint8_t*)payload.c_str(), payload.length(), retain);
		else
			mqttClientSp->publish(String(prefix + topic).c_str(), (const uint8_t*)payload.c_str(), payload.length(), retain);
	}

	bool ksMqttConnector::connectToBroker()
	{
		if (sendConnectionStatus)
		{
			String willTopic{prefix + "connected"};
			
			if (mqttClientSp->connect(WiFi.macAddress().c_str(), login.c_str(), password.c_str(), willTopic.c_str(), 0, true, "0"))
			{
				mqttClientSp->publish(willTopic.c_str(), (const uint8_t*)"1", 1, true);
				return true;
			}

			return false;
		}

		return mqttClientSp->connect(WiFi.macAddress().c_str(), login.c_str(), password.c_str());
	}

	bool ksMqttConnector::loop()
	{
		if (mqttClientSp->loop())
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
			if (auto wifiConnSp = wifiConnWp.lock())
			{
				if (wifiConnSp->isConnected() && connectToBroker())
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
		if (mqttClientSp)
			if (auto wifiConnSp = wifiConnWp.lock())
				if (wifiConnSp->isConnected())
					return mqttClientSp->connected();
		
		return false;
	}
}