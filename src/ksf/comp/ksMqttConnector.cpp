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

	void ksMqttConnector::setupConnection(const std::string& broker, const std::string& port, const std::string& login, const std::string& password, const std::string& prefix, const std::string& fingerprint)
	{
		if (!fingerprint.empty())
		{
			#if ESP32
				bool fingerprintValid{fingerprint.size() / 2 == 32};
			#elif ESP8266
				bool fingerprintValid{fingerprint.size() / 2 == 20};
				uint8_t fingerprintData[20];
				if (fingerprintValid)
				{
					for (uint8_t idx = 0; idx < 20;)
					{
						uint8_t c = ksf::htoi(fingerprint[idx*2]);
						uint8_t d = ksf::htoi(fingerprint[idx*2+1]);

						if ((c>15) || (d>15))
							return;

						fingerprintData[idx++] = (c<<4)|d;
					}
				}
			#endif

			if (fingerprintValid)
			{
				auto secureClient = std::make_shared<WiFiClientSecure>();
				
				#if ESP8266
					secureClient->setFingerprint(fingerprintData);
				#elif ESP32
					this->fingerprint = fingerprint;
					secureClient->setInsecure();
				#endif

				wifiClientSp = std::move(secureClient);
			}
		}
		else
		{
			wifiClientSp = std::make_shared<WiFiClient>();
		}

		mqttClientSp = std::make_shared<PubSubClient>(*wifiClientSp.get());

		this->login = login;
		this->password = password;
		this->prefix = prefix;

		IPAddress serverIP;

		if (serverIP.fromString(broker.c_str()))
			mqttClientSp->setServer(serverIP, std::stoi(port));
		else
			mqttClientSp->setServer(broker.c_str(), std::stoi(port));
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
		if (onMesssage->isBound())
		{
			std::string_view payloadStr(reinterpret_cast<const char*>(payload), length);
			std::string_view topicStr(topic);

			if (topicStr.find(prefix) != std::string::npos)
				topicStr = topicStr.substr(prefix.length());

			onMesssage->broadcast(topicStr, payloadStr);
		}
	}

	void ksMqttConnector::subscribe(const std::string& topic, bool skipDevicePrefix)
	{
		if (skipDevicePrefix)
			mqttClientSp->subscribe(topic.c_str());
		else	
			mqttClientSp->subscribe(std::string(prefix + topic).c_str());
	}

	void ksMqttConnector::unsubscribe(const std::string& topic, bool skipDevicePrefix)
	{
		if (skipDevicePrefix)
			mqttClientSp->unsubscribe(topic.c_str());
		else
			mqttClientSp->unsubscribe(std::string(prefix + topic).c_str());
	}

	void ksMqttConnector::publish(const std::string& topic, const std::string& payload, bool retain, bool skipDevicePrefix)
	{
		if (skipDevicePrefix)
			mqttClientSp->publish(topic.c_str(), (const uint8_t*)payload.c_str(), payload.length(), retain);
		else
			mqttClientSp->publish(std::string(prefix + topic).c_str(), (const uint8_t*)payload.c_str(), payload.length(), retain);
	}

	bool ksMqttConnector::connectToBroker()
	{
		if (sendConnectionStatus)
		{
			std::string willTopic{prefix + "connected"};

			if (mqttClientSp->connect(WiFi.macAddress().c_str(), login.c_str(), password.c_str(), willTopic.c_str(), 0, true, "0"))
			{
				#if ESP32
					if (fingerprint.size() > 0 && !reinterpret_cast<WiFiClientSecure*>(wifiClientSp.get())->verify(fingerprint.c_str(), nullptr))
					{
						mqttClientSp->disconnect();
						return true;
					}
				#endif

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