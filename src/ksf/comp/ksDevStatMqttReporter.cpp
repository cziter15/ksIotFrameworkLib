/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "Arduino.h"
#include "../ksApplication.h"
#include "ksDevStatMqttReporter.h"
#include "ksMqttConnector.h"

#if ESP32
	#include <WiFi.h>
	#include <esp_phy_init.h>
#elif ESP8266
	#include <ESP8266WiFi.h>
#else			
	#error Platform not implemented.
#endif
namespace ksf::comps
{
	ksDevStatMqttReporter::ksDevStatMqttReporter(uint8_t intervalInSeconds) 
		: reporterTimer(intervalInSeconds * KSF_ONE_SECOND_MS)
	{}

	void ksDevStatMqttReporter::postInit(ksApplication* owner)
	{
		mqttConnWp = owner->findComponent<ksMqttConnector>();

		if (auto mqttConnSp{mqttConnWp.lock()})
			mqttConnSp->onConnected->registerEvent(connEventHandle, std::bind(&ksDevStatMqttReporter::onConnected, this));
	}
	
	void ksDevStatMqttReporter::onConnected()
	{
		reporterTimer.restart();
	}

	void ksDevStatMqttReporter::reportDevStats() const
	{
		if (auto mqttConnSp{mqttConnWp.lock()})
		{
			mqttConnSp->publish(PGM_("devstat/rssi"), ksf::to_string(WiFi.RSSI()));
			mqttConnSp->publish(PGM_("devstat/ipAddress"), WiFi.localIP().toString().c_str());
			mqttConnSp->publish(PGM_("devstat/uptimeSec"), ksf::to_string(millis64()/1000));
			mqttConnSp->publish(PGM_("devstat/connTimeMs"), ksf::to_string(mqttConnSp->getConnectionTimeSeconds()));
			mqttConnSp->publish(PGM_("devstat/reconnCounter"), ksf::to_string(mqttConnSp->getReconnectCounter()));
		}
	}
	
	bool ksDevStatMqttReporter::loop()
	{
		if (reporterTimer.triggered())
			reportDevStats();

		return true;
	}
}