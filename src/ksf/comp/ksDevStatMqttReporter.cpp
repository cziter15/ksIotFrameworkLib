/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
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

		mqttConnSp->publish(PSTR("dstat/rssi"), ksf::to_string(WiFi.RSSI()));
		mqttConnSp->publish(PSTR("dstat/uptimeSec"), ksf::to_string(ksf::millis64()/1000));
		mqttConnSp->publish(PSTR("dstat/connTimeSec"), ksf::to_string(mqttConnSp->getConnectionTimeSeconds()));
		mqttConnSp->publish(PSTR("dstat/reconnCnt"), ksf::to_string(mqttConnSp->getReconnectCounter()));

		onReportCustomStats->broadcast(mqttConnSp);
	}
	
	bool ksDevStatMqttReporter::loop(ksApplication* app)
	{
		if (reporterTimer.triggered())
			reportDevStats();

		return true;
	}
}
