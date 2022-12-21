/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "WiFiManager.h"
#include "../ksConfig.h"
#include "ksConfigProvider.h"

namespace ksf::comps
{
	bool ksConfigProvider::init(ksApplication* owner)
	{
		return true;
	}

	bool ksConfigProvider::loop()
	{
		return true;
	}

	void ksConfigProvider::addNewParam(WiFiManager& manager, std::string label, std::string defaultValue, int maxLength)
	{
		auto& par{params.emplace_back(std::move(label), nullptr)};
		par.second = std::make_unique<WiFiManagerParameter>(par.first.c_str(), par.first.c_str(), defaultValue.c_str(), maxLength);
		manager.addParameter(par.second.get());
	}

	void ksConfigProvider::addNewParamWithConfigDefault(WiFiManager& manager, ksConfig& config, std::string label, int maxLength)
	{
		auto& defaultValue{config.getParam(label)};
		return addNewParam(manager, std::move(label), std::move(defaultValue), maxLength);
	}
}