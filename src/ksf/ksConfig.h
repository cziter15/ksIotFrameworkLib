/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <WString.h>
#include <map>

/* Wrapper macro that allows to create nice file-manipulating sections. */
#define USING_CONFIG_FILE(fileName) \
	if (ksf::ksConfig config_file = ksf::ksConfig(fileName))

namespace ksf
{
	class ksConfig
	{
		protected:
			bool isDirty = false;						// True if config params are modiffied and should be saved.
			std::map<String, String> configParams;		// Config parameters as key-value pair.
			String configFile;							// Config filename.

		public:
			/* 
				Constructor that opens specified file.
				@param configFile Filename of config to open/create.
			*/
			ksConfig(const String& configFile);

			/*
				Sets specified parameter (in memory).

				@param paramName Pparameter name.
				@param paramValue Parameter value.
			*/
			void setParam(const String& paramName, const String& paramValue);


			/*
				Retrieves specified parameter's value.

				@param paramName Parameter name.
				@param defaultValue Default value to return (if not found).
			*/
			const String& getParam(const String& paramName, const String& defaultValue = String()) const;

			/*
				Destructor that save changes if isDirty flag is set to true.
			*/
			virtual ~ksConfig();

			/*
				Operator bool override. Returns true if configFilename is not empty.
			*/
			operator bool() const;
	};
}

