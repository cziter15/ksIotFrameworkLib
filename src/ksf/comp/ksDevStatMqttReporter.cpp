/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include <Arduino.h>

#include "../ksConstants.h"
#include "../ksApplication.h"
#include "ksMqttConnector.h"
#if defined(ESP32)
	#include <WiFi.h>
#elif defined(ESP8266)
	#include <ESP8266WiFi.h>
#else			
	#error Platform not implemented.
#endif

#include "ksDevStatMqttReporter.h"

namespace ksf::comps
{
	static constexpr char RSSI_TOPIC[] 			PROGMEM {"dstat/rssi"};
	static constexpr char UPTIME_TOPIC[] 		PROGMEM {"dstat/uptimeSec"};
	static constexpr char CONN_TIME_TOPIC[] 	PROGMEM {"dstat/connTimeSec"};
	static constexpr char RECONN_CNT_TOPIC[] 	PROGMEM {"dstat/reconnCnt"};

	ksDevStatMqttReporter::ksDevStatMqttReporter(uint8_t intervalInSeconds) 
		: reporterTimer(KSF_SEC_TO_MS(intervalInSeconds))
	{}

	bool ksDevStatMqttReporter::postInit(ksApplication* app)
	{
		mqttConnWp = app->findComponent<ksMqttConnector>();

		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->onConnected->registerEvent(connEventHandle, std::bind(&ksDevStatMqttReporter::onConnected, this));

		return true;
	}
	
	void ksDevStatMqttReporter::onConnected()
	{
		reporterTimer.restart();
	}

	void ksDevStatMqttReporter::reportDevStats() const
	{
		auto mqttConnSp{mqttConnWp.lock()};
		if (!mqttConnSp || !mqttConnSp->isConnected())
			return;

		mqttConnSp->publish(RSSI_TOPIC, ksf::to_string(WiFi.RSSI()));
		mqttConnSp->publish(UPTIME_TOPIC, ksf::to_string(ksf::millis64()/1000));
		mqttConnSp->publish(CONN_TIME_TOPIC, ksf::to_string(mqttConnSp->getConnectionTimeSeconds()));
		mqttConnSp->publish(RECONN_CNT_TOPIC, ksf::to_string(mqttConnSp->getReconnectCounter()));

		onReportCustomStats->broadcast(mqttConnSp);
	}
	
	bool ksDevStatMqttReporter::loop(ksApplication* app)
	{
		if (reporterTimer.triggered())
			reportDevStats();

		return true;
	}
}
