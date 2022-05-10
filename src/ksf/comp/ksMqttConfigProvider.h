/*    
 *	Copyright (c) 2019-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 *
 */

#pragma once
#include "ksConfigProvider.h"
#include <WiFiManager.h>

class WiFiManager;

namespace ksf
{
	class ksMqttConnector;
	class ksMqttConfigProvider : public ksConfigProvider
	{
		protected:
			static const char ksfMqttConfigFile[];

			static const char ksfBrokerParamName[];
			static const char ksfUserParamName[];
			static const char ksfPortParamName[];
			static const char ksfDefPort[];
			static const char ksfPasswordParamName[];
			static const char ksfPrefixParamName[];

			std::vector<class WiFiManagerParameter*> params;

		public:
			void injectManagerParameters(WiFiManager& manager) override;
			void captureManagerParameters(WiFiManager& manager) override;
			virtual void setupMqttConnector(ksMqttConnector& connector);
	};
}