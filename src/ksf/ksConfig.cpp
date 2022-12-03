/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksConfig.h"
#include "LittleFS.h"

namespace ksf
{
	ksConfig::ksConfig(const std::string& configFile)
	{
		if (configFile.length() > 0)
		{
			this->configFile = configFile[0] != '/' ? "/" + configFile : configFile;
			auto fileReader{LittleFS.open(this->configFile.c_str(), "r")};

			while (fileReader.available())
			{
				std::string name{fileReader.readStringUntil('\n').c_str()};
				name = name.substr(0, name.length() - 1);

				std::string val{fileReader.readStringUntil('\n').c_str()};
				val = val.substr(0, val.length() - 1);

				setParam(name, val);
			}

			isDirty = false;

			if (fileReader)
				fileReader.close();
		}
	}

	void ksConfig::setParam(const std::string& paramName, const std::string& paramValue)
	{
		isDirty = true;
		configParams[paramName] = paramValue;
	}

	const std::string& ksConfig::getParam(const std::string& paramName, const std::string& defaultValue) const
	{
		const auto& found{configParams.find(paramName)};
		if (found != configParams.end())
			return found->second;

		return defaultValue;
	}

	ksConfig::~ksConfig()
	{
		if (isDirty)
		{
			auto fileWriter{LittleFS.open(configFile.c_str(), "w")};

			for (const auto& entry : configParams)
			{
				fileWriter.println(entry.first.c_str());
				fileWriter.println(entry.second.c_str());
			}

			fileWriter.close();
		}
	}

	ksConfig::operator bool() const
	{
		return configFile.length() > 0;
	}
}