/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
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

namespace ksf::comps
{
	class ksMqttConnector;
	class ksMqttConfigProvider : public ksConfigProvider
		{
			KSF_RTTI_DECLARATIONS(ksMqttConfigProvider, ksConfigProvider)

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