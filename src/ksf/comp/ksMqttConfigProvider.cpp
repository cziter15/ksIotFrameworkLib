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
	const char MQTT_FILENAME_TEXT[] 	PROGMEM {"mqtt.conf"};
	
	const char DEFPORT_AS_TEXT_PGM[] 	PROGMEM {"1883"};
	const char BROKER_TEXT_PGM[]		PROGMEM {"broker"};
	const char USER_TEXT_PGM[] 			PROGMEM {"user"};
	const char FINGERPRINT_TEXT_PGM[] 	PROGMEM {"SSLFingerprint"};
	const char PASSWORD_TEXT_PGM[] 		PROGMEM {"password"};
	const char PREFIX_TEXT_PGM[] 		PROGMEM {"prefix"};
	const char PORT_TEXT_PGM[]	 		PROGMEM {"port"};

	constexpr const char TOPIC_DELIMETER{'/'};

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
				/* Apply suffix correction. */
				if (prefix.back() != TOPIC_DELIMETER)
					prefix += TOPIC_DELIMETER;
			}

			if (savedBroker.length() > 0)
				connector.setupConnection(savedBroker, port, std::move(login), std::move(password), std::move(prefix), fingerprint);
		}
	}

	void ksMqttConfigProvider::readParams()
	{		
		USING_CONFIG_FILE(MQTT_FILENAME_TEXT)
		{
			addNewParamWithConfigDefault(config_file, BROKER_TEXT_PGM);
			addNewParamWithConfigDefault(config_file, PORT_TEXT_PGM, 5);
			addNewParamWithConfigDefault(config_file, USER_TEXT_PGM);
			addNewParamWithConfigDefault(config_file, FINGERPRINT_TEXT_PGM, 110);
			addNewParamWithConfigDefault(config_file, PASSWORD_TEXT_PGM);
			addNewParamWithConfigDefault(config_file, PREFIX_TEXT_PGM);
		}
	}

	void ksMqttConfigProvider::saveParams()
	{
		USING_CONFIG_FILE(MQTT_FILENAME_TEXT)
		{
			for (auto& param : params)
				config_file.setParam(param.id, std::move(param.value));
		}

		params.clear();
	}
}