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
	
	bool ksDevStatMqttReporter::loop()
	{
		return true;
	}
}