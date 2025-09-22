/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include "../evt/ksEvent.h"
#include "../ksComponent.h"

#include "../misc/ksSimpleTimer.h"

namespace ksf::comps
{
	class ksMqttConnector;

	/*!
		@brief A component that periodically reports the device state to the broker.

		Upon instantiation, the component searches for the ksMqttConnector component. 
		The update interval is specified as a construction parameter.

		Each update includes values such as RSSI, device uptime, connection time, and IP address. 
		The subtopic used is "dstat" so, for example, RSSI is published to the topic "deviceprefix/dstat/rssi".
	*/
	class ksDevStatMqttReporter : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksDevStatMqttReporter, ksComponent)

		protected:
			std::weak_ptr<ksMqttConnector> mqttConnWp;				//!< Weak pointer to MQTT connector.
			std::unique_ptr<evt::ksEventHandle> connEventHandle;	//!< Event handle for connection delegate.
			misc::ksSimpleTimer reporterTimer;						//!< Timer to report device stats.

			/*!
				@brief Calback executed on MQTT connection.

				This callback is used to restart reporter timer to match
				the intervals between individual reports. The timer will be processed even if
		 		the MQTT is not connected, but reportDevStats will quickly return.
			*/
			void onConnected();

			/*!
				@brief Reports device statistics to the MQTT broker.
			*/
			void reportDevStats() const;

		public:
			/*!
				@brief Called when Dev Stat Reporter timer is triggered. Users can bind to this event to add their own stats.
				@param param_1 Shared pointer to the MQTT connector.
			*/
			DECLARE_KS_EVENT(onReportCustomStats, std::shared_ptr<ksMqttConnector>&)

			/*!
				@brief Constructs device statistics reporter component.
				@param intervalInSeconds Interval in seconds between each report.
			*/
			ksDevStatMqttReporter(uint8_t intervalInSeconds = 60);

			/*!
				@brief Handles component post-initialization.

				This method is responsible for reference gathering (component lookup) as well as binding to the events.

				@param app Pointer to the paren ksApplication.
				@return True on success, false on fail.
			*/
			bool postInit(ksApplication* app) override;

			/*!
				@brief Handles MQTT debug connector component loop logic.
				@param app Pointer to the parent ksApplication.
				@return True on success, false on fail.
			*/
			bool loop(ksApplication* app) override;
	};
}
