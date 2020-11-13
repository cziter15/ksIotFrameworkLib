/*
	 Project:	ksIotFramework
	 Author:	Krzysztof Strehlau
*/

#include "ksConfig.h"

#ifdef ESP32
#include "SPIFFS.h"
#else
#include "FS.h"
#endif

namespace ksf
{
	ksConfig::ksConfig(String configFile)
	{
		if (configFile.length() > 0)
		{
			configFilename = configFile.charAt(0) != '/' ? "/" + configFile : configFile;
			File fileReader = SPIFFS.open(configFilename.c_str(), "r");

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

	void ksConfig::setParam(const String& paramName, String paramValue)
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
			File fileWriter = SPIFFS.open(configFilename.c_str(), "w");

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