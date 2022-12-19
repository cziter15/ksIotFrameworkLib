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
			auto ksfBrokerParamName{BROKER_TEXT_PGM};
			auto& savedBroker{config_file.getParam(BROKER_TEXT_PGM)};
			auto& port{config_file.getParam(PORT_TEXT_PGM, DEFPORT_AS_TEXT_PGM)};

			auto& login{config_file.getParam(USER_TEXT_PGM)};
			auto& password{config_file.getParam(PASSWORD_TEXT_PGM)};

			auto prefix{config_file.getParam(PREFIX_TEXT_PGM)};
			auto fingerprint{config_file.getParam(FINGERPRINT_TEXT_PGM)};

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
		/* 
			Use move semantics to move underlying strings and cache through injection process.
			This is required because WiFiManager uses pointers to strings and they must be valid.
		*/
		brokerParamName = std::move(BROKER_TEXT_PGM);
		portParamName = std::move(PORT_TEXT_PGM);
		userParamName = std::move(USER_TEXT_PGM);
		fpParamName = std::move(FINGERPRINT_TEXT_PGM);
		passwordParamName = std::move(PASSWORD_TEXT_PGM);
		prefixParamName = std::move(PREFIX_TEXT_PGM);
		
		USING_CONFIG_FILE(MQTT_FILENAME_TEXT)
		{
			addNewParam(manager, brokerParamName.c_str(), config_file.getParam(brokerParamName).c_str());
			addNewParam(manager, portParamName.c_str(), config_file.getParam(portParamName).c_str(), 5);
			addNewParam(manager, userParamName.c_str(), config_file.getParam(userParamName).c_str());
			addNewParam(manager, fpParamName.c_str(), config_file.getParam(fpParamName).c_str(), 110);
			addNewParam(manager, passwordParamName.c_str(), config_file.getParam(passwordParamName).c_str());
			addNewParam(manager, prefixParamName.c_str(), config_file.getParam(prefixParamName).c_str());
		}
	}

	void ksMqttConfigProvider::captureManagerParameters(WiFiManager& manager)
	{
		USING_CONFIG_FILE(MQTT_FILENAME_TEXT)
		{
			for (auto& param : params)
				config_file.setParam(param->getID(), param->getValue());
		}

		params.clear();
	}
}