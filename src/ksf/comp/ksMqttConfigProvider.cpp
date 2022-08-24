/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksComposable.h"
#include "../ksConfig.h"
#include "ksWiFiConfigurator.h"
#include "ksMqttConnector.h"
#include "ksMqttConfigProvider.h"

namespace ksf::comps
{
	const char ksMqttConfigProvider::ksfMqttConfigFile[] = "mqtt.conf";

	const char ksMqttConfigProvider::ksfBrokerParamName[] = "broker";
	const char ksMqttConfigProvider::ksfUserParamName[] = "user";
	const char ksMqttConfigProvider::ksfPortParamName[] = "port";
	const char ksMqttConfigProvider::ksfDefPort[] = "1883";
	const char ksMqttConfigProvider::ksfPasswordParamName[] = "password";
	const char ksMqttConfigProvider::ksfPrefixParamName[] = "prefix";

	void ksMqttConfigProvider::setupMqttConnector(ksMqttConnector& connector)
	{
		USING_CONFIG_FILE(ksfMqttConfigFile)
		{
			auto& savedBroker = config_file.getParam(ksfBrokerParamName);
			auto& port = config_file.getParam(ksfPortParamName, ksfDefPort);

			auto& login = config_file.getParam(ksfUserParamName);
			auto& password = config_file.getParam(ksfPasswordParamName);

			auto prefix = config_file.getParam(ksfPrefixParamName);

			if (prefix.length() > 0)
			{
				/* MQTT topic delimeter (slash). */
				const char topicDelimeter{'/'};

				/* Apply prefix correction. */
				if (prefix.front() != topicDelimeter)
					prefix = topicDelimeter + prefix;

				/* Apply suffix correction. */
				if (prefix.back() != topicDelimeter)
					prefix += topicDelimeter;
			}

			if (savedBroker.length() > 0)
				connector.setupConnection(savedBroker, port, login, password, prefix);
		}
	}

	void ksMqttConfigProvider::injectManagerParameters(WiFiManager& manager)
	{
		USING_CONFIG_FILE(ksfMqttConfigFile)
		{
			addNewParam(manager, ksfBrokerParamName, config_file.getParam(ksfBrokerParamName).c_str());
			addNewParam(manager, ksfPortParamName, config_file.getParam(ksfPortParamName).c_str());
			addNewParam(manager, ksfUserParamName, config_file.getParam(ksfUserParamName).c_str());
			addNewParam(manager, ksfPasswordParamName, config_file.getParam(ksfPasswordParamName).c_str());
			addNewParam(manager, ksfPrefixParamName, config_file.getParam(ksfPrefixParamName).c_str());
			addNewParam(manager, ksfPasswordParamName, config_file.getParam(ksfPasswordParamName).c_str());
		}
	}

	void ksMqttConfigProvider::captureManagerParameters(WiFiManager& manager)
	{
		USING_CONFIG_FILE(ksfMqttConfigFile)
		{
			for (auto& param : params)
				config_file.setParam(param->getID(), param->getValue());
		}

		params.clear();
	}
}