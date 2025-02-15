/*
 *	Copyright (c) 2021-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksConfig.h"
#include "../ksConstants.h"

#include <LittleFS.h>
namespace ksf::misc
{
	ksConfig::ksConfig(const std::string& fileName)
		: configPath(getNvsDirectory())
	{
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

		/* Read config file. */
		if (auto fileReader{LittleFS.open(configPath.c_str(), PSTR("r"))})
		{
			/* Read until EOF. */
			while (fileReader.available())
			{
				/* Read key. */
				std::string name{fileReader.readStringUntil('\n').c_str()};
				name = name.substr(0, name.length() - 1);

				/* Read value. */
				std::string val{fileReader.readStringUntil('\n').c_str()};
				val = val.substr(0, val.length() - 1);

				/* Set param. */
				setParam(name, std::move(val));
			}

			/* Close file. */
			fileReader.close();
		}
	}

	ksConfig::~ksConfig()
	{
		if (!isDirty)
			return;

		/* If marked dirty, save changes to FS. */
		auto fileWriter{LittleFS.open(configPath.c_str(), PSTR("w"))};
		if (!fileWriter)
			return;

		for (const auto&[name, value] : configParams)
		{
			fileWriter.println(name.c_str());
			fileWriter.println(value.c_str());
		}
		fileWriter.close();
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
}