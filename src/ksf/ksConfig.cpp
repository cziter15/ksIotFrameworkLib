/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include <Preferences.h>
#include "ksConfig.h"

namespace ksf
{
	ksConfig::ksConfig(const char* configFile)
	{
		if (!configFile)
			return;

		config = std::make_unique<Preferences>();
		config->begin(configFile, false);
	}

	ksConfig::~ksConfig() = default;

	void ksConfig::setParam(const char* paramName, const std::string_view& paramValue)
	{
		config->putBytes(paramName, paramValue.data(), paramValue.length());
	}

	std::string ksConfig::getParam(const char* paramName, const std::string_view& defaultValue) const
	{
		std::string outVal;
		if (auto bytesLength{config->getBytesLength(paramName)}; bytesLength > 0)
		{
			outVal.resize(bytesLength);
			config->getBytes(paramName, outVal.data(), bytesLength);
		}
		else outVal = defaultValue;
		
		return std::move(outVal);
	}

	ksConfig::operator bool() const
	{
		return config != nullptr;
	}
}
