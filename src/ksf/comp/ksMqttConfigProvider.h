/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "ksConfigProvider.h"
#include <WiFiManager.h>

class WiFiManager;

namespace ksf
{
	namespace comps
	{
		class ksMqttConnector;
		class ksMqttConfigProvider : public ksConfigProvider
		{
			KSF_RTTI_DECLARATIONS(ksMqttConfigProvider, ksConfigProvider)

			protected:
				static const char ksfMqttConfigFile[];					// Static char-string "mqtt.conf".

				static const char ksfBrokerParamName[];					// Static char-string "broker".
				static const char ksfUserParamName[];					// Static char-string "user".
				static const char ksfPortParamName[];					// Static char-string "port".
				static const char ksfFingerprintParamName[];			// Static char-string "fingerprint".
				static const char ksfDefPort[];							// Static char-string "port".
				static const char ksfPasswordParamName[];				// Static char-string "password".
				static const char ksfPrefixParamName[];					// Static char-string "prefix".

			public:
				/*
					Injects MQTT config provider's WiFiManager parameters.
					@param manager WiFiManager reference.
				*/
				void injectManagerParameters(WiFiManager& manager) override;

				/*
					Captures MQTT config provider's WiFiManager parameters.
					@param manager WiFiManager reference.
				*/
				void captureManagerParameters(WiFiManager& manager) override;

				/*
					Configures MQTT connector from config provider.
					@param connector ksMqttConnector reference.
				*/
				void setupMqttConnector(ksMqttConnector& connector);
		};
	}
}