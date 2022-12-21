/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksApplication.h"
#include "../ksConfig.h"
#include "ksWiFiConfigurator.h"
#include "ksMqttConnector.h"
#include "ksMqttConfigProvider.h"

namespace ksf::comps
{
	/* Assign PGM returned strings to a set of macros. */
	#define MQTT_FILENAME_TEXT 			PGM_("mqtt.conf")
	#define DEFPORT_AS_TEXT_PGM 		PGM_("1883")
	#define BROKER_TEXT_PGM				PGM_("broker")
	#define USER_TEXT_PGM 				PGM_("user")
	#define FINGERPRINT_TEXT_PGM 		PGM_("SSLFingerprint")
	#define PASSWORD_TEXT_PGM 			PGM_("password")
	#define PREFIX_TEXT_PGM 			PGM_("prefix")
	#define PORT_TEXT_PGM	 			PGM_("port")

	void ksMqttConfigProvider::setupMqttConnector(ksMqttConnector& connector)
	{
		USING_CONFIG_FILE(MQTT_FILENAME_TEXT)
		{
			const auto& savedBroker{config_file.getParam(BROKER_TEXT_PGM)};
			const auto& port{config_file.getParam(PORT_TEXT_PGM, DEFPORT_AS_TEXT_PGM)};

			const auto& login{config_file.getParam(USER_TEXT_PGM)};
			const auto& password{config_file.getParam(PASSWORD_TEXT_PGM)};
			const auto& fingerprint{config_file.getParam(FINGERPRINT_TEXT_PGM)};

			auto prefix{config_file.getParam(PREFIX_TEXT_PGM)};

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
				connector.setupConnection(savedBroker, port, login, password, prefix, fingerprint);
		}
	}

	void ksMqttConfigProvider::injectManagerParameters(WiFiManager& manager)
	{		
		USING_CONFIG_FILE(MQTT_FILENAME_TEXT)
		{
			auto brokerParamName{BROKER_TEXT_PGM};
			addNewParam(manager, brokerParamName, config_file.getParam(brokerParamName));

			auto portParamName{PORT_TEXT_PGM};
			addNewParam(manager, portParamName, config_file.getParam(portParamName), 5);
			
			auto userParamName{USER_TEXT_PGM};
			addNewParam(manager, userParamName, config_file.getParam(userParamName));

			auto fpParamName{FINGERPRINT_TEXT_PGM};
			addNewParam(manager, fpParamName, config_file.getParam(fpParamName), 110);

			auto passwordParamName{PASSWORD_TEXT_PGM};
			addNewParam(manager, passwordParamName, config_file.getParam(passwordParamName));

			auto prefixParamName{PREFIX_TEXT_PGM};
			addNewParam(manager, prefixParamName, config_file.getParam(prefixParamName));
		}
	}

	void ksMqttConfigProvider::captureManagerParameters(WiFiManager& manager)
	{
		USING_CONFIG_FILE(MQTT_FILENAME_TEXT)
		{
			for (auto& param : params)
				config_file.setParam(param.second->getID(), param.second->getValue());
		}

		params.clear();
	}
}