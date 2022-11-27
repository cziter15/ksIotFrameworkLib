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
#include "../ksCertUtils.h"
#include "ksWifiConnector.h"
#include "ksMqttConnector.h"
#include "ksMqttConfigProvider.h"
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

using namespace std::placeholders;

namespace ksf::comps
{
	ksMqttConnector::ksMqttConnector(bool sendConnectionStatus, bool usePersistentSession)
		: sendConnectionStatus(sendConnectionStatus), usePersistentSession(usePersistentSession)
	{}

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

	void ksMqttConnector::setupConnection(const std::string& broker, const std::string& port, const std::string& login, const std::string& password, const std::string& prefix, const std::string& fingerprint)
	{
		if (!fingerprint.empty())
		{
			auto secureClient{std::make_shared<WiFiClientSecure>()};
			certFingerprint = std::make_shared<ksCertFingerprintHolder>();
			
			if (certFingerprint->setup(secureClient.get(), fingerprint))
				wifiClientSp = std::move(secureClient);
		}
		else
		{
			wifiClientSp = std::make_shared<WiFiClient>();
		}

		mqttClientSp = std::make_shared<PubSubClient>(*wifiClientSp.get());

		this->login = login;
		this->password = password;
		this->prefix = prefix;

		/* 
		*	Set timeout for blocking calls (eg. connect). MQTT is handled async-like.
		*
		*	There's an inconsistency in setTimeout implementation between Arduino for ESP32 and ESP8266.
		*	ESP32's WiFi client setTimeout method want seconds, while ESP8266's one wants milliseconds.
		*	I've wasted some time to get what's really going on here, but eventually caught that.
		*	Keep in mind that setTimeout wants time in seconds on ESP32 but getTimeout will return milliseconds.
		* 
		*	Timeout is required because underlying connect method is blocking. Without timeout it may trigger watchdog.
		*
		*	Please be aware of bugs present in Arduino-esp32 framework:
		*	-	https://github.com/espressif/arduino-esp32/issues/7350
		*	-	https://github.com/espressif/arduino-esp32/issues/7356
		*	-	https://github.com/espressif/arduino-esp32/issues/7355
		*
		*	In my projects I'm using my own fork of esp32:
		*	-	https://github.com/cziter15/arduino-esp32
		*/
	
		#if ESP32
			wifiClientSp->setTimeout(KSF_MQTT_TIMEOUT_SEC);
		#elif ESP8266
			wifiClientSp->setTimeout(KSF_MQTT_TIMEOUT_SEC * KSF_ONE_SECOND_MS);
		#else			
			#error Platform not implemented.
		#endif

		uint16_t portNumber;
		if (ksf::from_chars(port, portNumber))
		{
			IPAddress serverIP;
			if (serverIP.fromString(broker.c_str()))
				mqttClientSp->setServer(serverIP, portNumber);
			else
				mqttClientSp->setServer(broker.c_str(), portNumber);
		}	
	}

	void ksMqttConnector::mqttConnectedInternal()
	{
		mqttClientSp->setCallback(std::bind(&ksMqttConnector::mqttMessageInternal, this, _1, _2, _3));
		onConnected->broadcast();
	}

	void ksMqttConnector::mqttMessageInternal(const char* topic, const uint8_t* payload, uint32_t length)
	{
		if (onMesssage->isBound())
		{
			std::string_view payloadStr{reinterpret_cast<const char*>(payload), length};
			std::string_view topicStr{topic};

			if (topicStr.find(prefix) != std::string::npos)
				topicStr = topicStr.substr(prefix.length());

			onMesssage->broadcast(topicStr, payloadStr);
		}
	}

	void ksMqttConnector::subscribe(const std::string& topic, bool skipDevicePrefix, ksMqttConnector::QosLevel qos)
	{
		uint8_t qosLevel{static_cast<uint8_t>(qos)};

		if (skipDevicePrefix)
			mqttClientSp->subscribe(topic.c_str(), qosLevel);
		else	
			mqttClientSp->subscribe(std::string(prefix + topic).c_str(), qosLevel);
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

			if (mqttClientSp->connect(WiFi.macAddress().c_str(), login.c_str(), password.c_str(), willTopic.c_str(), 0, true, "0", !usePersistentSession))
			{
				if (certFingerprint && !certFingerprint->verify(reinterpret_cast<WiFiClientSecure*>(wifiClientSp.get())))
				{
					mqttClientSp->disconnect();
					return false;
				}

				mqttClientSp->publish(willTopic.c_str(), reinterpret_cast<const uint8_t*>("1"), 1, true);
				return true;
			}

			return false;
		}

		return mqttClientSp->connect(WiFi.macAddress().c_str(), login.c_str(), password.c_str(), 0, 0, false, 0, !usePersistentSession);
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
			wasConnected = false;
			reconnectTimer.restart();
			onDisconnected->broadcast();
		}
		else if (reconnectTimer.hasTimePassed())
		{
			if (auto wifiConnSp{wifiConnWp.lock()})
			{
				if (wifiConnSp->isConnected() && connectToBroker())
				{
					oneSecTimer.restart();
					++reconnectCounter;
					wasConnected = true;
					mqttConnectedInternal();
				}

				/* This must be done after connectToBroker, because connect can block for few seconds. */
				reconnectTimer.restart();
			}
		}

		return true;
	}

	bool ksMqttConnector::isConnected() const
	{
		return mqttClientSp ? mqttClientSp->connected() : false;
	}
}