/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksConfig.h"
#include "ksConfigProvider.h"

namespace ksf::comps
{
	void ksConfigProvider::addNewParam(const char* id, const char* value, int maxLength)
	{
		ksConfigParameter param
		{
			.id = id,
			.value = value,
			.maxLength = maxLength
		};

		params.push_back(std::move(param));
	}

	void ksConfigProvider::addNewParamWithConfigDefault(ksConfig& config, const char* id, int maxLength)
	{
		return addNewParam(id, config.getParam(id).c_str(), maxLength);
	}

	std::list<ksConfigParameter>& ksConfigProvider::getParameters()
	{
		return params;
	}
}
