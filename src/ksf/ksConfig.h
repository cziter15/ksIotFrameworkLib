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

/*
	Wrapper macro that allows to create nice file-manipulating sections. 
*/
#define USING_CONFIG_FILE(fileName) \
	if (ksf::ksConfig config_file{ksf::ksConfig(fileName)})

namespace ksf
{
	class ksConfig
	{
		protected:
			bool isDirty{false};									// True if config contents has been modified (and should be saved).
			std::map<std::string, std::string> configParams;		// Config parameters.
			std::string configFile;									// Config filename.

		public:
			/*
				@brief Constructor that opens (or creates) specified config file and loads its contents into memory.
				@param configFile Config file name
			*/
			ksConfig(const std::string& configFile);

			/*
				@brief Sets parameter value.
				
				If parameter does not exist, it will be created.

				@param paramName Parameter name
				@param paramValue Parameter value (use std::move when possible)
			*/
			void setParam(const std::string& paramName, const std::string paramValue);


			/*
				@brief Retrieves parameter value. 
				
				If parameter does not exist, defaultValue will be returned.

				@param paramName Parameter name
				@param defaultValue Default value to return if parameter does not exist
				@return Parameter value or defaultValue if parameter does not exist.
			*/
			const std::string& getParam(const std::string& paramName, const std::string& defaultValue = std::string()) const;

			/*
				@brief Operator bool override. Returns true if configFilename is not empty.
				@return True if configFilename is not empty, otherwise false.
			*/
			operator bool() const;


			/*
				@brief Saves config content to flash. 

				If config contents has not been modified, nothing will be saved.
			*/
			virtual ~ksConfig();
	};
}

