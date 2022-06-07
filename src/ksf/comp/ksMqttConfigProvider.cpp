/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
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
			String savedBroker = config_file.getParam(ksfBrokerParamName);
			String port = config_file.getParam(ksfPortParamName, ksfDefPort);

			String login = config_file.getParam(ksfUserParamName);
			String password = config_file.getParam(ksfPasswordParamName);
			String prefix = config_file.getParam(ksfPrefixParamName);

			String slash(("/"));

			// Apply prefix correction
			if (!prefix.startsWith(slash))
				prefix = slash + prefix;

			// Apply prefix suffix correction
			if (!prefix.endsWith(slash))
				prefix += slash;

			if (savedBroker.length() > 0)
				connector.setupConnection(savedBroker, port, login, password, prefix);
		}
	}

	void ksMqttConfigProvider::injectManagerParameters(WiFiManager& manager)
	{
		USING_CONFIG_FILE(ksfMqttConfigFile)
		{
			params.push_back(new WiFiManagerParameter(ksfBrokerParamName, ksfBrokerParamName, config_file.getParam(ksfBrokerParamName, "").c_str(), 50));
			manager.addParameter(params.back());

			params.push_back(new WiFiManagerParameter(ksfPortParamName, ksfPortParamName, config_file.getParam(ksfPortParamName, ksfDefPort).c_str(), 5));
			manager.addParameter(params.back());

			params.push_back(new WiFiManagerParameter(ksfUserParamName, ksfUserParamName, config_file.getParam(ksfUserParamName, "").c_str(), 25));
			manager.addParameter(params.back());

			params.push_back(new WiFiManagerParameter(ksfPasswordParamName, ksfPasswordParamName, config_file.getParam(ksfPasswordParamName, "").c_str(), 50));
			manager.addParameter(params.back());

			params.push_back(new WiFiManagerParameter(ksfPrefixParamName, ksfPrefixParamName, config_file.getParam(ksfPrefixParamName, "").c_str(), 50));
			manager.addParameter(params.back());
		}
	}

	void ksMqttConfigProvider::captureManagerParameters(WiFiManager& manager)
	{
		USING_CONFIG_FILE(ksfMqttConfigFile)
		{
			for (auto& param : params)
				config_file.setParam(param->getID(), param->getValue());
		}

		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"

		for	(auto& param : params)
			delete param;

		#pragma GCC diagnostic pop

		params.clear();
	}
}