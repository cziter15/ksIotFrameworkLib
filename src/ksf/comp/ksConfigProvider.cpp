/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
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
		/* If label is empty, we use id as label*/
		if (label.empty())
			label = id;

		ksConfigParameter param
		{
			.id = std::move(id),
			.label = std::move(label),
			.value = std::move(value),
			.type = type,
			.maxLength = maxLength
		};

		params.push_back(std::move(param));
	}

	void ksConfigProvider::addNewParamWithConfigDefault(misc::ksConfig& config, std::string id, std::string label, int maxLength, EConfigParamType::Type type)
	{
		auto& value{config.getParam(id)};
		return addNewParam(std::move(id), std::move(label), std::move(value), maxLength, type);
	}

	std::list<ksConfigParameter>& ksConfigProvider::getParameters()
	{
		return params;
	}
}
