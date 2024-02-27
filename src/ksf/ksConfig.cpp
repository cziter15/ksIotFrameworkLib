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

namespace ksf
{
	ksConfig::ksConfig(const std::string& configFile) 
		: configFile(PSTR("/nvs/"))
	{
		/* If no file specified, clear internal path. */
		if (configFile.empty())
		{
			this->configFile.clear();
			return;
		}

		/* Assemble file path. */
		this->configFile += configFile;

		/* Construct reader. */
		auto fileReader{LittleFS.open(this->configFile.c_str(), "r")};

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
		return configFile.length() > 0;
	}

	ksConfig::~ksConfig()
	{
		if (!isDirty)
			return;

		/* If marked dirty, save changes to FS. */
		auto fileWriter{LittleFS.open(configFile.c_str(), "w")};
		for (const auto& [name, value] : configParams)
		{
			fileWriter.println(name.c_str());
			fileWriter.println(value.c_str());
		}
		fileWriter.close();
	}
}
