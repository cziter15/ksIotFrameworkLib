/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../evt/ksEvent.h"
#include "../ksComponent.h"
#include "../ksSimpleTimer.h"

namespace ksf::comps
{
	class ksMqttConnector;
	class ksDevStatMqttReporter : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksDevStatMqttReporter, ksComponent)

		protected:
			std::weak_ptr<ksMqttConnector> mqttConnWp;				// Weak pointer to MQTT connector.
			std::unique_ptr<evt::ksEventHandle> connEventHandle;	// Event handle for connection delegate.
			ksSimpleTimer reporterTimer;							// Timer to report device stats.

			/*
				@brief Function called on MQTT connection.
			*/
			void onConnected();

			/*
				@brief Function called periodically when MQTT is connected to report device stats to the broker.
			*/
			void reportDevStats() const;

		public:
			ksDevStatMqttReporter(uint8_t intervalInSeconds = 60);

			/*
				@brief Method called after component initialization.

				Used to setup message callbacks.

				@param app Pointer to ksApplication object that owns this component
				@return True on success, false on fail.
			*/
			bool postInit(ksApplication* app) override;

			/*
				@brief Handles MQTT debug connector component loop logic.
				@param app Pointer to ksApplication
				@return True on success, false on fail.
			*/
			bool loop(ksApplication* app) override;
	};
}