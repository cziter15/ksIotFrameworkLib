/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include <vector>

class WiFiManager;

namespace ksf 
{
	class ksComposable;
	class ksConfigProvider : public ksComponent
	{
		public:
			/*
				Initializes config provider component.
				@param owner - pointer to owning composable interface (application).
				@return - true on success, false on fail.
			*/
			bool init(ksComposable* owner) override;

			/*
				Overridable method to inject WiFiManager parameters.
				@param manager - WiFiManager reference.
			*/
			virtual void injectManagerParameters(WiFiManager& manager) = 0;

			/*
				Overridable method to capture WiFiManager parameters.
				@param manager - WiFiManager reference.
			*/
			virtual void captureManagerParameters(WiFiManager& manager) = 0;

			/*
				Called from application loop.
				@return - true on success, false on fail.
			*/
			bool loop() override;
	};
}
