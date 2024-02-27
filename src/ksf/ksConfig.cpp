/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "LittleFS.h"
#include "ksConfig.h"
#include "ksConstants.h"

namespace ksf
{
	ksConfig::ksConfig(const std::string& fileName)
		: configPath(getNvsDirectory())
	{
		ets_printf("ksConfig::ksConfig(%s)\n", fileName.c_str());
		/* If no file specified, clear internal path. */
		if (fileName.empty())
		{
			configPath.clear();
			return;
		}

		/* Add leading slash. */
		if (fileName[0] != '/')
			configPath += '/';

		/* Assemble file path. */
		configPath += fileName;

		ets_printf("ksConfig::ksConfig - Full path is: %s\n", configPath.c_str());

		/* Construct reader. */
		auto fileReader{LittleFS.open(configPath.c_str(), "r")};

		/* Read until EOF. */
		while (fileReader.available())
		{
			std::string name{fileReader.readStringUntil('\n').c_str()};
			name = name.substr(0, name.length() - 1);

			std::string val{fileReader.readStringUntil('\n').c_str()};
			val = val.substr(0, val.length() - 1);

			setParam(name, std::move(val));
		}

		if (fileReader)
			fileReader.close();
	}

	void ksConfig::setParam(const std::string& paramName, std::string paramValue)
	{
		isDirty = true;
		configParams.insert_or_assign(paramName, std::move(paramValue));
	}

	const std::string& ksConfig::getParam(const std::string& paramName, const std::string& defaultValue) const
	{
		const auto& found{configParams.find(paramName)};
		return found == configParams.end() ? defaultValue : found->second;
	}

	ksConfig::operator bool() const
	{
		return !configPath.empty();
	}

	ksConfig::~ksConfig()
	{
		if (!isDirty)
			return;

		ets_printf("ksConfig::ksConfig - Saving properties to: %s\n", configPath.c_str());

		/* If marked dirty, save changes to FS. */
		auto fileWriter{LittleFS.open(configPath.c_str(), "w")};
		if (!fileWriter)
		{
			ets_printf("ksConfig::ksConfig - Failed to save properties to: %s\n", configPath.c_str());
			return;
		}

		for (const auto& [name, value] : configParams)
		{
			fileWriter.println(name.c_str());
			fileWriter.println(value.c_str());
		}
		fileWriter.close();
	}
}
