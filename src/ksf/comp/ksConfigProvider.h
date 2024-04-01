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
		/*!
			@brief Configuration parameter type enum.
		*/
		enum Type
		{
			Text,
			Password,
			Number
		};
	};

	/*!
		@brief A structure that defines configuration parameter.
	*/
	struct ksConfigParameter
	{
		std::string id;
		std::string defaultValue;
		std::string value;
		EConfigParamType::Type type{};
		int maxLength{};
	};

	/*!
		@brief A component that provides a set of parameters to the ksWifiConfigurator and is also responsible for handling its storage.
		
		In the configurator application, it should be instantiated like a typical component, and ksWifiConfigurator must be added to the component stack. 
		In this case, it will manage the correct flow of data reading and saving.
		
		In contrast, in a normal application, it can be created for a short period of time, usually inside the application's 
		init method, to read the data and return the required value.
		Refer to the EnergySensor init method for an example. Note that the component will be destroyed upon reaching the function return. 
		Also, note that the setupRotations method can return false in case of a problem with the value retrieval (for example, no config present yet), thus 
		causing the application to break and triggering the app rotator to move to the next defined application, usually AP-based configurator.
	*/
	class ksConfigProvider : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksConfigProvider, ksComponent)

		protected:
			std::list<ksConfigParameter> params;
			
			/*!
				@brief Defines new configutation parameter.

				@param id Unique identificator that will be also used as property label by Device Portal.
				@param value Default parameter value.
				@param maxLength Maximum length of the value.
				@param type Type of parameter, which tells the Device Portal which field type should be used.
			*/
			void addNewParam(std::string id, std::string value, int maxLength = 50, EConfigParamType::Type type = {});

			/*!
				@brief Defines new configuation parameter (with default value).
				
				@param config ksConfig reference.
				@param id Unique identificator that will be also used as property label by Device Portal.
				@param value Default parameter value.
				@param maxLength Maximum length of the value.
				@param type Type of parameter, which tells the Device Portal which field type should be used.		
			*/
			void addNewParamWithConfigDefault(ksConfig& config, std::string id, int maxLength = 50, EConfigParamType::Type type = {});

		public:
			/*!
				@brief Provides const reference to the list of managed parameters.
			*/
			std::list<ksConfigParameter>& getParameters();

			/*!
				@brief Populates the parameter list by processing the configuration file.
			*/
			virtual void readParams() = 0;

			/*!
				@brief Outputs the parameter list to the configuration file.
			*/
			virtual void saveParams() = 0;
	};
}
