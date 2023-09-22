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
				@param id Shared ID/Label to identify parameter.
				@param defaultValue Default value of parameter.
				@param maxLength Maximum length of parameter value.
			*/
			void addNewParam(std::string id, std::string defaultValue, int maxLength = 50);

			/*
				@param id Shared ID/Label to identify parameter.
				@param config ksConfig reference.
				@param maxLength Maximum length of parameter value.
			*/
			void addNewParamWithConfigDefault(ksConfig& config, std::string id, int maxLength = 50);

		public:
			/*
				Returns list of parameters.
			*/
			std::list<ksConfigParameter>& getParameters();

			virtual void readParams();
			virtual void saveParams();
	};
}
