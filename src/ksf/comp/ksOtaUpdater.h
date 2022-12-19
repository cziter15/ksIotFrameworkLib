/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "ArduinoOTA.h"
#include "../evt/ksEvent.h"
#include "../ksComponent.h"

namespace ksf::comps
{
	class ksOtaUpdater : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksOtaUpdater, ksComponent)

		protected:
			ArduinoOTAClass ArduinoOTA;						// Arduino OTA object.

		public:
			DECLARE_KS_EVENT(onUpdateStart)					// onUpdateStart event that user can bind to.
			DECLARE_KS_EVENT(onUpdateEnd)					// onUpdateEnd event that user can bind to.

			/*
				Constructor, used to construct OTA updater component.
				
				@param hostname Hostname of the device.
				@param password Password required to flash.
			*/
			ksOtaUpdater(const std::string& hostname, const std::string& password);

			/*
				Constructor, used to construct OTA updater component.
				Uses the default password "ota_ksiotframework".
				
				@param hostname Hostname of the device.
			*/
			ksOtaUpdater(const std::string& hostname);

			/*
				Initializes OTA component.

				@param owner Pointer to ksApplication object that owns this component.
				@return True on success, false on fail.
			*/
			bool init(ksApplication* owner) override;

			/*
				Handles OTA component loop logic.

				@return True on success, false on fail.
			*/
			bool loop() override;
	};
}