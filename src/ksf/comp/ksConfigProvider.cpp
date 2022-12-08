/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksConfigProvider.h"
#include "../ksConfig.h"
#include "WiFiManager.h"

namespace ksf::comps
{
	bool ksConfigProvider::init(ksf::ksComposable* owner)
	{
		return true;
	}

	bool ksConfigProvider::loop()
	{
		return true;
	}

	void ksConfigProvider::addNewParam(WiFiManager& manager, const char* label, const char* defaultValue, int maxLength)
	{
		auto param{new WiFiManagerParameter(label, label, defaultValue, maxLength)};
		params.emplace_back(param);
		manager.addParameter(param);
	}
}