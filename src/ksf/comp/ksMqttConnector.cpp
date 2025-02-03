/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#if defined(ESP32)
	#include <esp_wifi.h>
	#include <WiFi.h>
	#include <WiFiClientSecure.h>
#elif defined(ESP8266)
	#include <user_interface.h>
	#include <ESP8266WiFi.h>
	#include <WiFiClientSecure.h>
#else 			
	#error Platform not implemented.
#endif

#include <PubSubClient.h>

#include "../ksApplication.h"
#include "../ksConstants.h"
#include "../ksConfig.h"
#include "../misc/ksCertUtils.h"
#include "ksWifiConnector.h"
#include "ksMqttConnector.h"
#include "ksMqttConfigProvider.h"

using namespace std::placeholders;

namespace ksf::comps
{
	ksMqttConnector::~ksMqttConnector() = default;

	ksMqttConnector::ksMqttConnector(bool sendConnectionStatus, bool usePersistentSession)
	{
		bitflags.sendConnectionStatus = sendConnectionStatus;
		bitflags.usePersistentSession = usePersistentSession;
	}

	bool ksMqttConnector::init(ksApplication* app)
	{
		ksMqttConfigProvider cfgProvider;

#if APP_LOG_ENABLED
		this->app = app;
#endif

		cfgProvider.init(app);
		cfgProvider.setupMqttConnector(*this);

		/*
			Object mqttClientUq is created by setupConnection method.
			That means init will return false when no MQTT config file found.
		*/
		return mqttClientUq != nullptr;
	}

	bool ksMqttConnector::postInit(ksApplication* app)
	{
		wifiConnWp = app->findComponent<ksWifiConnector>();
		return true;
	}

	void ksMqttConnector::setupConnection(const std::string broker, const std::string& port, std::string login, std::string password, std::string prefix, const std::string& fingerprint)
	{
		if (!fingerprint.empty())
		{
			auto secureClient{std::make_unique<ksMqttConnectorNetClientSecure_t>()};
			certFingerprint = std::make_unique<ksCertFingerprintHolder>();
			if (certFingerprint->setup(secureClient.get(), fingerprint))
				netClientUq = std::move(secureClient);
		}
		else netClientUq = std::make_unique<ksMqttConnectorNetClient_t>();

		/* Whoops, it looks like fingerprint validation failed. */
		if (!netClientUq)
			return;
		
		/* Set socket timeouts. */
		netClientUq->setTimeout(KSF_MQTT_TIMEOUT_MS);

		/* Load MQTT parameters. */
		this->login = std::move(login);
		this->password = std::move(password);
		this->prefix = std::move(prefix);
		this->domainResolver.setDomain(std::move(broker));
		ksf::from_chars(port, portNumber);

		/* Create MQTT client. */
		mqttClientUq = std::make_unique<PubSubClient>(*netClientUq.get());
	}

	/*!
		@brief Called when MQTT connection is established.
		
		This method is called after MQTT connection is established successfully.
		It sets up the callback for incoming MQTT messages and broadcasts the onConnected event.
	*/
	void ksMqttConnector::mqttConnectedInternal()
	{
		lastSuccessConnectionTime = ksf::millis64();
		mqttClientUq->setCallback(std::bind(&ksMqttConnector::mqttMessageInternal, this, _1, _2, _3));
		onConnected->broadcast();
	}

	void ksMqttConnector::mqttMessageInternal(const char* topic, const uint8_t* payload, uint32_t length)
	{
		bool handlesDeviceMessage{onDeviceMessage->isBound()};
		bool handlesAnyMessage{onAnyMessage->isBound()};

		if (!handlesDeviceMessage && !handlesAnyMessage)
			return;

		std::string_view payloadStr{reinterpret_cast<const char*>(payload), length};
		std::string_view topicStr{topic};
		
#ifdef APP_LOG_ENABLED
		app->log([&](std::string& out) {
			out += PSTR("[MQTT] Reveived from: ");
			out += topicStr;
			out += PSTR(", value: ");
			out += payloadStr;
		});
#endif

		if (handlesDeviceMessage && ksf::starts_with(topicStr, prefix))
		{
			topicStr = topicStr.substr(prefix.length());
			onDeviceMessage->broadcast(topicStr, payloadStr);
		}
		
		if (handlesAnyMessage)
			onAnyMessage->broadcast(topicStr, payloadStr);
	}

	void ksMqttConnector::subscribe(const std::string& topic, bool skipDevicePrefix, ksMqttConnector::QosLevel qos)
	{
		uint8_t qosLevel{static_cast<uint8_t>(qos)};
		mqttClientUq->subscribe(skipDevicePrefix ? topic.c_str() : std::string(prefix + topic).c_str(), qosLevel);
	}

	void ksMqttConnector::unsubscribe(const std::string& topic, bool skipDevicePrefix)
	{
		mqttClientUq->unsubscribe(skipDevicePrefix ? topic.c_str() : std::string(prefix + topic).c_str());
	}

	void ksMqttConnector::publish(const std::string& topic, const std::string& payload, bool retain, bool skipDevicePrefix)
	{
#ifdef APP_LOG_ENABLED
		app->log([&](std::string& out) {
			out += PSTR("[MQTT] ");
			if (retain)
				out += PSTR("(Retained) ");
			out += PSTR("Publish to: ");
			out += prefix;
			out += topic;
			out += PSTR(", value: ");
			out += payload;
		});
#endif
		mqttClientUq->publish(skipDevicePrefix ? topic.c_str() : std::string(prefix + topic).c_str(), reinterpret_cast<const uint8_t*>(payload.c_str()), payload.length(), retain);
	}

	bool ksMqttConnector::connectToBroker()
	{
#ifdef APP_LOG_ENABLED
		app->log([&](std::string& out) {
			out += PSTR("[MQTT] Connecting to MQTT broker...");
		});
#endif
		IPAddress serverIP;
		if (!domainResolver.getResolvedIP(serverIP))
		{
#ifdef APP_LOG_ENABLED
			app->log([&](std::string& out) {
				out += PSTR("[MQTT] Failed to resolve MQTT broker IP address!");
			});
#endif
			return false;
		} 

		if (serverIP.operator uint32_t() != 0)
		{
#ifdef APP_LOG_ENABLED
			app->log([&](std::string& out) {
				out += PSTR("[MQTT] Connecting to the resolved IP address: ");
				out += std::string(serverIP.toString().c_str());
			});
#endif
#if defined(ESP32)
			netClientUq->connect(serverIP, portNumber, KSF_MQTT_TIMEOUT_MS);
#elif defined(ESP8266)
			netClientUq->connect(serverIP, portNumber);
#else
			#error "Unsupported platform"
#endif
		}

		/* If not connected, return. */
		if (!netClientUq->connected())
			return false;

		/* Verify certificate fingerprint. */
		if (certFingerprint && !certFingerprint->verify(reinterpret_cast<ksMqttConnectorNetClientSecure_t*>(netClientUq.get())))
		{
#ifdef APP_LOG_ENABLED
			app->log([&](std::string& out) {
				out += PSTR("[MQTT] Invalid certificate fingerprint! Disconnecting.");
			});
#endif
			netClientUq->stop();
			return false;
		}

#ifdef APP_LOG_ENABLED
		app->log([&](std::string& out) {
			out += PSTR("[MQTT] Connected to the server, processing with MQTT...");
		});
#endif

		if (bitflags.sendConnectionStatus)
		{
			std::string willTopic{prefix + PSTR("connected")};
			if (mqttClientUq->connect(WiFi.macAddress().c_str(), login.c_str(), password.c_str(), willTopic.c_str(), 0, true, "0", !bitflags.usePersistentSession))
			{
				mqttClientUq->publish(willTopic.c_str(), reinterpret_cast<const uint8_t*>("1"), 1, true);
				return true;
			}
		}
		
		return mqttClientUq->connect(WiFi.macAddress().c_str(), login.c_str(), password.c_str(), 0, 0, false, 0, !bitflags.usePersistentSession);
	}

	bool ksMqttConnector::loop(ksApplication* app)
	{
		domainResolver.process();
		
		if (!mqttClientUq->loop())
		{
			if (bitflags.wasConnected)
			{
				bitflags.wasConnected = false;
				reconnectTimer.restart();
				domainResolver.invalidate();
				onDisconnected->broadcast();
			}
			else if (reconnectTimer.hasTimePassed())
			{
				if (auto wifiConnSp{wifiConnWp.lock()})
				{
					if (wifiConnSp->isConnected() && connectToBroker())
					{
						++reconnectCounter;
						bitflags.wasConnected = true;
						mqttConnectedInternal();
					}

					domainResolver.invalidate();
				}
				
				/* This must be done after connectToBroker, because connect can block for few seconds. */
				reconnectTimer.restart();			
			}
		}
		
		return true;
	}

	bool ksMqttConnector::isConnected() const
	{
		return mqttClientUq ? mqttClientUq->connected() : false;
	}

	uint32_t ksMqttConnector::getConnectionTimeSeconds() const
	{
		return isConnected() ? ((ksf::millis64() - lastSuccessConnectionTime) / KSF_ONE_SEC_MS) : 0;
	}
}
