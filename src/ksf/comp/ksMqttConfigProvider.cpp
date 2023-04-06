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
			/*
				These parameters are taken by reference as no transfer of the data is needed or possible.
				Port is converted to int, fingerprint is mapped to a vector of bytes and broker is copied internally.
			*/
			const auto& savedBroker{config_file.getParam(BROKER_TEXT_PGM)};
			const auto& port{config_file.getParam(PORT_TEXT_PGM, DEFPORT_AS_TEXT_PGM)};
			const auto& fingerprint{config_file.getParam(FINGERPRINT_TEXT_PGM)};
			
			/*
				These parameters are copied here. Later they are moved directly to the fields of the connector.
			*/
			auto login{config_file.getParam(USER_TEXT_PGM)};
			auto password{config_file.getParam(PASSWORD_TEXT_PGM)};
			auto prefix{config_file.getParam(PREFIX_TEXT_PGM)};

			if (prefix.length() > 0)
			{
				/* MQTT topic delimeter (slash). */
				const char topicDelimeter{'/'};

				/* Apply suffix correction. */
				if (prefix.back() != '/')
					prefix += topicDelimeter;
			}


			if (savedBroker.length() > 0)
				connector.setupConnection(savedBroker, port, std::move(login), std::move(password), std::move(prefix), fingerprint);
		}
	}

	void ksMqttConfigProvider::injectManagerParameters(WiFiManager& manager)
	{		
		USING_CONFIG_FILE(MQTT_FILENAME_TEXT)
		{
			addNewParamWithConfigDefault(manager, config_file, BROKER_TEXT_PGM);
			addNewParamWithConfigDefault(manager, config_file, PORT_TEXT_PGM, 5);
			addNewParamWithConfigDefault(manager, config_file, USER_TEXT_PGM);
			addNewParamWithConfigDefault(manager, config_file, FINGERPRINT_TEXT_PGM, 110);
			addNewParamWithConfigDefault(manager, config_file, PASSWORD_TEXT_PGM);
			addNewParamWithConfigDefault(manager, config_file, PREFIX_TEXT_PGM);
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