/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>
#include <utility>
#include <list>
#include "../ksComponent.h"

namespace ksf::misc
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
			Text,		//!< Renders a text field in the configurator. Value will be read as string.
			Password,	//!< Renders a password field in the configurator. Text field will be hidden but value will still be read.
			Number,		//!< Renders a number field in the configurator. Value will be read as string.
			Checkbox	//!< Renders a checkbox in the configurator, the value will be read as "enabled" or "disabled" string.
		};
	};

	/*!
		@brief A structure that defines configuration parameter.
	*/
	struct ksConfigParameter
	{
		std::string id;					//!< Unique parameter identifier.
		std::string label;				//!< Value label.
		std::string value;				//!< Default parameter value.
		EConfigParamType::Type type{}; 	//!< Parameter type.
		int maxLength{}; 				//!< Maximum length of the parameter value.
	};

	/*!
		@brief A component responsible for providing a set of parameters to ksWifiConfigurator and managing its storage.

		In the configurator application, this component should be instantiated like any other, with ksWifiConfigurator included in the component stack. 
		In this setup, it ensures the proper flow of data reading and saving.

		In a standard application, however, it is typically created temporarily—often within the application's init method—to read the necessary data and return the required value. 
		For an example, refer to the EnergySensor init method. The component is destroyed once the function returns.

		Additionally, be aware that the setupRotations method may return false if there is an issue retrieving the value (e.g., if no configuration is present). 
		This could cause the application to fail, triggering the app rotator to switch to the next defined application, which is usually an AP-based configurator.
	*/
	class ksConfigProvider : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksConfigProvider, ksComponent)

		protected:
			std::list<ksConfigParameter> params;	//!< List of configuration parameters.
			
			/*!
				@brief Defines new configutation parameter.

				@param id Unique identificator that will be also used as property label by Device Portal.
				@param label Parameter label.
				@param defaultValue Default parameter value.
				@param maxLength Maximum length of the value.
				@param type Type of parameter, which tells the Device Portal which field type should be used.
			*/
			void addNewParam(std::string id, std::string label, std::string defaultValue, int maxLength = 50, EConfigParamType::Type type = {});

			/*!
				@brief Defines new configuation parameter (with default value).

				@param config ksConfig reference.
				@param id Unique identificator that will be also used as property label by Device Portal.
				@param label Parameter label.
				@param maxLength Maximum length of the value.
				@param type Type of parameter, which tells the Device Portal which field type should be used.
			*/
			void addNewParamWithConfigDefault(misc::ksConfig& config, std::string id, std::string label = {}, int maxLength = 50, EConfigParamType::Type type = {});

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
