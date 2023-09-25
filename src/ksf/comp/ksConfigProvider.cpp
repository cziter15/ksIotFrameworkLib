/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksConfig.h"
#include "ksConfigProvider.h"

namespace ksf::comps
{
	void ksConfigProvider::addNewParam(std::string id, std::string value, int maxLength)
	{
		ksConfigParameter param
		{
			.id = std::move(id),
			.value = std::move(value),
			.maxLength = maxLength
		};

		params.push_back(std::move(param));
	}

	void ksConfigProvider::addNewParamWithConfigDefault(ksConfig& config, std::string id, int maxLength)
	{
		auto& value{config.getParam(id)};
		return addNewParam(std::move(id), std::move(value), maxLength);
	}

	std::list<ksConfigParameter>& ksConfigProvider::getParameters()
	{
		return params;
	}
}