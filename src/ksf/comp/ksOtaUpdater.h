/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../ksComponent.h"
#include "../evt/ksEvent.h"
#include "ArduinoOTA.h"

namespace ksf
{
	namespace comps
	{
		class ksOtaUpdater : public ksComponent
		{
			KSF_RTTI_DECLARATIONS(ksOtaUpdater, ksComponent)

			protected:
				ArduinoOTAClass ArduinoOTA;		// Arduino OTA object.

			public:
				DECLARE_KS_EVENT(onUpdateStart)					// onUpdateStart event that user can bind to.
				DECLARE_KS_EVENT(onUpdateEnd)					// onUpdateEnd event that user can bind to.

				/*
					Constructor, used to construct OTA updater component.
					
					@param hostname Hostname of the device.
					@param password Passwor required to flash.
				*/
				ksOtaUpdater(const char* hostname, const char* password = "ota_ksiotframework");

				/*
					Initializes OTA component.

					@param owner Pointer to ownning ksComposable object (application).
					@return True on success, false on fail.
				*/
				bool init(class ksf::ksComposable* owner) override;

				/*
					Handles OTA component loop logic.

					@return True on success, false on fail.
				*/
				bool loop() override;
		};
	}
}