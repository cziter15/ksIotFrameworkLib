/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include <vector>

class WiFiManager;

namespace ksf
{
	namespace comps
	{
		class ksComposable;
		class ksConfigProvider : public ksf::ksComponent
		{
			public:
				/*
					Initializes config provider component.

					@param owner Pointer to ownning ksComposable object (application).
					@return True on success, false on fail.
				*/
				virtual bool init(ksf::ksComposable* owner) override;

				/*
					Overridable method to inject WiFiManager parameters.
					@param manager WiFiManager reference.
				*/
				virtual void injectManagerParameters(WiFiManager& manager) = 0;

				/*
					Overridable method to capture WiFiManager parameters.
					@param manager WiFiManager reference.
				*/
				virtual void captureManagerParameters(WiFiManager& manager) = 0;

				/*
					Called from application loop.
					@return True on success, false on fail.
				*/
				virtual bool loop() override;
		};
	}
}
