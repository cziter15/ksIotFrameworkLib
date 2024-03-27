/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
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
	namespace EConfigParamType
	{
		enum Type
		{
			Text,
			Password,
			Number
		};
	};

	struct ksConfigParameter
	{
		std::string id;
		std::string defaultValue;
		std::string value;
		EConfigParamType::Type type{};
		int maxLength{};
	};

	class ksConfigProvider : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksConfigProvider, ksComponent)

		protected:
			std::list<ksConfigParameter> params;
			
			/*!
				@brief Defines new parameter.

				@param id Shared ID/Label to identify parameter
				@param value Default value
				@param maxLength Maximum length of parameter value
				@param type Type of parameter
			*/
			void addNewParam(std::string id, std::string value, int maxLength = 50, EConfigParamType::Type type = {});

			/*!
				@brief Defines new parameter (with default value).

				@param config Reference to config
				@param id Shared ID/Label to identify parameter
				@param maxLength Maximum length of parameter value
				@param type Type of parameter
			*/
			void addNewParamWithConfigDefault(ksConfig& config, std::string id, int maxLength = 50, EConfigParamType::Type type = {});

		public:
			/*!
				@brief Retrieves const list of parameters.
			*/
			std::list<ksConfigParameter>& getParameters();

			/*!
				@brief Loads parameters from the config file.
			*/
			virtual void readParams() = 0;

			/*!
				@brief Moves parameters to the config file.
			*/
			virtual void saveParams() = 0;
	};
}
