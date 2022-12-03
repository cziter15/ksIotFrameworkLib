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
#include <memory>
#include <vector>

class WiFiManager;
class WiFiManagerParameter;

namespace ksf
{
	namespace comps
	{
		class ksComposable;
		class ksConfigProvider : public ksComponent
		{
			KSF_RTTI_DECLARATIONS(ksConfigProvider, ksComponent)

			protected:
				std::vector<std::unique_ptr<WiFiManagerParameter>> params;	// Unique pointers of parameters (deletes them safely).
				
				/*
					Adds a new param to the WiFiManager configuration process.
					This method must be called only inside injectManagerParameters function.

					@param manager WiFiManager reference.
					@param label Label and ID to identify parameter.
					@param defaultValue Default value (visible as hint for the user).
					@param maxLength Maximum string length.
				*/
				void addNewParam(WiFiManager& manager, const char* label, const char* defaultValue, int maxLength = 50);

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
