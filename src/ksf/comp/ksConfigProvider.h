/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <utility>
#include <list>
#include "../ksComponent.h"

class WiFiManager;
class WiFiManagerParameter;

namespace ksf
{
	class ksConfig;
}

namespace ksf::comps
{

	class ksConfigProvider : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksConfigProvider, ksComponent)

		protected:
			std::list<std::pair<std::string, std::unique_ptr<WiFiManagerParameter>>> params;	// WiFiManager parameters.
			
			/*
				Adds a new param to the WiFiManager configuration process.
				This method must be called only inside injectManagerParameters function.

				@param manager WiFiManager reference.
				@param label Shared ID/Label to identify parameter.
				@param defaultValue Default value of parameter.
				@param maxLength Maximum length of parameter value.
			*/
			void addNewParam(WiFiManager& manager, std::string label, std::string defaultValue, int maxLength = 50);

			/*
				Adds a new param to the WiFiManager configuration process. Default value is taken from ksConfig object.
				This method must be called only inside injectManagerParameters function.

				@param manager WiFiManager reference.
				@param label Shared ID/Label to identify parameter.
				@param config ksConfig reference.
				@param maxLength Maximum length of parameter value.
			*/
			void addNewParamWithConfigDefault(WiFiManager& manager, ksConfig& config, std::string label, int maxLength = 50);

		public:
			/*
				Overridable method to inject WiFiManager parameters.

				@param manager WiFiManager reference.
			*/
			virtual void injectManagerParameters(WiFiManager& manager) = 0;

			/*
				Overridable method to capture WiFiManager parameters.

				@param manager WiFiManager reference.
			*/
			virtual void captureManagerParameters(WiFiManager& manager) = 0;
	};
}
