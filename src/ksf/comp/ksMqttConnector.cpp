/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#if ESP32
	#include <WiFi.h>
	#include <esp_wifi.h>
#elif ESP8266
	#include <user_interface.h>
	#include <ESP8266WiFi.h>
#else 			
	#error Platform not implemented.
#endif

#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "../ksApplication.h"
#include "../ksConstants.h"
#include "../ksConfig.h"
#include "../ksCertUtils.h"
#include "ksWifiConnector.h"
#include "ksMqttConnector.h"
#include "ksMqttConfigProvider.h"

using namespace std::placeholders;

namespace ksf::comps
{
	ksMqttConnector::~ksMqttConnector() = default;

	ksMqttConnector::ksMqttConnector(bool sendConnectionStatus, bool usePersistentSession)
		: sendConnectionStatus(sendConnectionStatus), usePersistentSession(usePersistentSession)
	{}

	bool ksMqttConnector::init(ksApplication* app)
	{
		ksMqttConfigProvider cfgProvider;

#if APP_LOG_ENABLED
		this->app = app;
#endif

		cfgProvider.init(app);
		cfgProvider.setupMqttConnector(*this);

		/*
			Object mqttClientSp is created by setupConnection method.
			That means init will return false when no MQTT config file found.
		*/
		return mqttClientSp != nullptr;
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
			auto secureClient{std::make_unique<WiFiClientSecure>()};
			certFingerprint = std::make_unique<ksCertFingerprintHolder>();
			
			if (certFingerprint->setup(secureClient.get(), fingerprint))
				wifiClientSp = std::move(secureClient);
		}
		else
		{
			wifiClientSp = std::make_unique<WiFiClient>();
		}

		/* Whoops, it looks like fingerprint validation failed. */
		if (!wifiClientSp)
			return;

		mqttClientSp = std::make_unique<PubSubClient>(*wifiClientSp.get());

		this->login = std::move(login);
		this->password = std::move(password);
		this->prefix = std::move(prefix);
		this->broker = std::move(broker);

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
		*/
	
#if ESP32
		wifiClientSp->setTimeout(KSF_MQTT_TIMEOUT_SEC);
#elif ESP8266
		wifiClientSp->setTimeout(KSF_SEC_TO_MS(KSF_MQTT_TIMEOUT_SEC));
#else			
		#error Platform not implemented.
#endif

		/* Load MQTT port. */
		ksf::from_chars(port, portNumber);
	}

	void ksMqttConnector::mqttConnectedInternal()
	{
		lastSuccessConnectionTime = millis64();
		mqttClientSp->setCallback(std::bind(&ksMqttConnector::mqttMessageInternal, this, _1, _2, _3));
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
		mqttClientSp->subscribe(skipDevicePrefix ? topic.c_str() : std::string(prefix + topic).c_str(), qosLevel);
	}

	void ksMqttConnector::unsubscribe(const std::string& topic, bool skipDevicePrefix)
	{
		mqttClientSp->unsubscribe(skipDevicePrefix ? topic.c_str() : std::string(prefix + topic).c_str());
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
		mqttClientSp->publish(skipDevicePrefix ? topic.c_str() : std::string(prefix + topic).c_str(), reinterpret_cast<const uint8_t*>(payload.c_str()), payload.length(), retain);
	}

	bool ksMqttConnector::connectToBroker()
	{
		if (sendConnectionStatus)
		{
			std::string willTopic{prefix + PSTR("connected")};
#ifdef APP_LOG_ENABLED
			app->log([&](std::string& out) {
				out += PSTR("[MQTT] Trying to connect to MQTT broker...");
			});
#endif
			/* Handle connection manually. */
			if (IPAddress serverIP; serverIP.fromString(this->broker.c_str()))
				wifiClientSp->connect(serverIP, portNumber);
			else 
				wifiClientSp->connect(this->broker.c_str(), portNumber);

			/* If not connected, return. */
			if (!wifiClientSp->connected())
			{
#ifdef APP_LOG_ENABLED
				app->log([&](std::string& out) {
					out += PSTR("[MQTT] Failed to connect...");
				});
#endif
				return false;
			}

			/* Verify certificate fingerprint. */
			if (certFingerprint && !certFingerprint->verify(reinterpret_cast<WiFiClientSecure*>(wifiClientSp.get())))
			{
#ifdef APP_LOG_ENABLED
				app->log([&](std::string& out) {
					out += PSTR("[MQTT] Certificate verification failed...");
				});
#endif
				wifiClientSp->stop();
				return false;
			}

			if (mqttClientSp->connect(WiFi.macAddress().c_str(), login.c_str(), password.c_str(), willTopic.c_str(), 0, true, "0", !usePersistentSession))
			{
#ifdef APP_LOG_ENABLED
				app->log([&](std::string& out) {
					out += PSTR("[MQTT] Connected successfully to ");
					out += wifiClientSp->remoteIP().toString().c_str();
					out += PSTR(" on port ");
					out += ksf::to_string(wifiClientSp->remotePort());
				});
#endif

				mqttClientSp->publish(willTopic.c_str(), reinterpret_cast<const uint8_t*>("1"), 1, true);
				return true;
			}

			return false;
		}

		return mqttClientSp->connect(WiFi.macAddress().c_str(), login.c_str(), password.c_str(), 0, 0, false, 0, !usePersistentSession);
	}

	bool ksMqttConnector::loop(ksApplication* app)
	{
		if (!mqttClientSp->loop())
		{
			if (wasConnected)
			{
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
						++reconnectCounter;
						wasConnected = true;
						mqttConnectedInternal();
					}

					/* This must be done after connectToBroker, because connect can block for few seconds. */
					reconnectTimer.restart();
				}
			}
		}
		
		return true;
	}

	bool ksMqttConnector::isConnected() const
	{
		return mqttClientSp ? mqttClientSp->connected() : false;
	}

	uint32_t ksMqttConnector::getConnectionTimeSeconds() const
	{
		return isConnected() ? ((millis64() - lastSuccessConnectionTime) / KSF_ONE_SEC_MS) : 0;
	}
}
