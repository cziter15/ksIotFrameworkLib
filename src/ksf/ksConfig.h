/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>
#include <map>

/*!
	@brief Wrapper macro that allows to create nice file-manipulating sections. 
	@param fileName Config filename.
*/
#define USING_CONFIG_FILE(fileName) \
	if (ksf::ksConfig config_file{ksf::ksConfig(fileName)})

namespace ksf
{
	/*!
		@brief Implements low-level configuration file handling.

		Uses std::map to store configuration parameters. When any parameter is modified, the config file is marked as dirty.
		Then on destruction, the config file is saved if it has been modified (marked as dirty) into the filesystem.
	*/
	class ksConfig
	{
		protected:
			bool isDirty{false};									//!< True if config contents has been modified (and should be saved).
			std::map<std::string, std::string> configParams;		//!< Config parameters.
			std::string configPath;									//!< Config filename.

		public:
			/*!
				@brief Constructor tha opens or creates configuration file.
				@param fileName Config filename.
			*/
			ksConfig(const std::string& fileName);

			/*!
				@brief Sets parameter value (creates new parameter if it does not exist).
				@param paramName Parameter name.
				@param paramValue Parameter value (use std::move when possible).
			*/
			void setParam(const std::string& paramName, const std::string paramValue);

			/*!
				@brief Retrieves parameter value.
				@param paramName Name of the parameter to retrieve.
				@param defaultValue Default value to return if parameter does not exist.
				@return Parameter value string (or defaultValue).
			*/
			const std::string& getParam(const std::string& paramName, const std::string& defaultValue = std::string()) const;

			/*!
				@brief Operator bool override. Returns true if configFilename is not empty.
				@return True if configFilename is not empty, otherwise false.
			*/
			operator bool() const;

			/*!
				@brief Saves config content on the device filesystem.
				In case there is no modification, nothing should actually happen.
			*/
			virtual ~ksConfig();
	};
}

