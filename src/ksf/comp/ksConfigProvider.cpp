/*
 *	Copyright (c) 2020-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../misc/ksConfig.h"

#include "ksConfigProvider.h"

namespace ksf::comps
{
	void ksConfigProvider::addNewParam(std::string id, std::string label, std::string value, int maxLength, EConfigParamType::Type type)
	{
		if (label.empty())
			label = id;

		params.emplace_back(std::move(id), std::move(label), std::move(value), type, maxLength);
	}

	void ksConfigProvider::addNewParamWithConfigDefault(misc::ksConfig& config, std::string id, std::string label, int maxLength, EConfigParamType::Type type)
	{
		auto value{config.getParam(id)};
		addNewParam(std::move(id), std::move(label), std::move(value), maxLength, type);
	}

	std::list<ksConfigParameter>& ksConfigProvider::getParameters()
	{
		return params;
	}
}
