/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "ksConfigProvider.h"
namespace ksf::comps
{
	class ksMqttConnector;
	/*!
		@brief A component that manages MQTT-related configuration.

		This configuration provider handles MQTT properties .
		Detailed documentation on the provider's behavior can be fond on ksConfigProvider sub-page.
	*/
	class ksMqttConfigProvider : public ksConfigProvider
	{
		KSF_RTTI_DECLARATIONS(ksMqttConfigProvider, ksConfigProvider)

		public:
			/*!
				@brief Retrieves MQTT parameters.

				This method reads the configuation file to retrieve MQTT broker information from file.
 				The config contains data like MQTT address, device prefix, optional: credentials and expected SSL fingerprint.  
			*/
			void readParams() override;

			/*!
				@brief Saves MQTT aprameters into the configuration file.

				This method saves current (in-memory) MQTT properties into the configuration file.
			*/
			void saveParams() override;

			/*!
				@brief Setup MQTT connector with captured parameters.
				@param connector MQTT connector reference
			*/
			void setupMqttConnector(ksMqttConnector& connector);
	};
}
