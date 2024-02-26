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
#include <string_view>
#include <memory>

class Preferences;

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
			bool touched{false};									// True if config contents has been modified (and should be saved).
			std::string configFile;									// Config filename.
			std::unique_ptr<Preferences> config;					// Config object.

		public:
			/*
				@brief Constructor that opens (or creates) specified config file and loads its contents into memory.
				@param configFile Config file name
			*/
			ksConfig(const char* configFile);

			/*
				@brief Sets parameter value.
				
				If parameter does not exist, it will be created.

				@param paramName Parameter name
				@param paramValue Parameter value (use std::move when possible)
			*/
			void setParam(const char* paramName, const std::string_view& value);

			/*
				@brief Retrieves parameter value. 
				
				If parameter does not exist, defaultValue will be returned.

				@param paramName Parameter name
				@param defaultValue Default value to return if parameter does not exist
				@return Parameter value or defaultValue if parameter does not exist.
			*/
			std::string getParam(const char* paramName, const std::string_view& defaultValue = std::string_view()) const;

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

