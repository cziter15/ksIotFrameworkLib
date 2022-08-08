/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksConfig.h"

#ifdef ESP32
	#include "SPIFFS.h"
#else
	#include "FS.h"
#endif

namespace ksf
{
	ksConfig::ksConfig(const String& configFile)
	{
		if (configFile.length() > 0)
		{
			configFilename = configFile.charAt(0) != '/' ? "/" + configFile : configFile;
			auto fileReader = SPIFFS.open(configFilename.c_str(), "r");

			while (fileReader.available())
			{
				String name = fileReader.readStringUntil('\n');
				name = name.substring(0, name.length() - 1);

				String val = fileReader.readStringUntil('\n');
				val = val.substring(0, val.length() - 1);

				setParam(name, val);
			}

			isDirty = false;

			if (fileReader)
				fileReader.close();
		}
	}

	void ksConfig::setParam(const String& paramName, const String& paramValue)
	{
		isDirty = true;
		configParams[paramName] = paramValue;
	}

	const String& ksConfig::getParam(const String& paramName, const String& defaultValue) const
	{
		const auto& found = configParams.find(paramName);
		if (found != configParams.end())
			return found->second;

		return defaultValue;
	}

	ksConfig::~ksConfig()
	{
		if (isDirty)
		{
			auto fileWriter = SPIFFS.open(configFilename.c_str(), "w");

			for (auto& entry : configParams)
			{
				fileWriter.println(entry.first);
				fileWriter.println(entry.second);
			}

			fileWriter.close();
		}
	}

	ksConfig::operator bool() const
	{
		return configFilename.length() > 0;
	}
}