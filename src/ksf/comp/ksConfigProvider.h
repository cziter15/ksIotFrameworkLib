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
	struct ksConfigParameter
	{
		std::string id;
		std::string defaultValue;
		std::string value;
		int maxLength{0};
	};

	class ksConfigProvider : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksConfigProvider, ksComponent)

		protected:
			std::list<ksConfigParameter> params;
			
			/*
				@brief Defines new parameter.
				@param id Shared ID/Label to identify parameter
				@param value Parameter value
				@param maxLength Maximum length of parameter value
			*/
			void addNewParam(std::string id, std::string value, int maxLength = 50);

			/*
				@brief Defines new parameter (with default value).
				@param id Shared ID/Label to identify parameter
				@param config ksConfig reference
				@param maxLength Maximum length of parameter value
			*/
			void addNewParamWithConfigDefault(ksConfig& config, std::string id, int maxLength = 50);

		public:
			/*
				@brief Retrieves const list of parameters.
			*/
			std::list<ksConfigParameter>& getParameters();

			/*
				@brief Loads parameters from the config file.
			*/
			virtual void readParams() = 0;

			/*
				@brief Moves parameters to the config file.
			*/
			virtual void saveParams() = 0;
	};
}
