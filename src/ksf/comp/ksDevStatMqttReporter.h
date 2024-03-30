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

	/*!
		@brief ksDevStatMqttReporter is a component that periodocally reports device state to the broker.
		
		Upon instantiated, the component will look for ksMqrrConnector component.
		The interval between each update is defined by the construction parameter.
		
		Each update contain values like RSSI, device uptime, connection time and IP address.
		The subtopic used is "dstat", so for example RSSI will be sent to the topic named "deviceprefix/dstat/rssi". 
	*/
	class ksDevStatMqttReporter : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksDevStatMqttReporter, ksComponent)

		protected:
			std::weak_ptr<ksMqttConnector> mqttConnWp;				//!< Weak pointer to MQTT connector.
			std::unique_ptr<evt::ksEventHandle> connEventHandle;	//!< Event handle for connection delegate.
			ksSimpleTimer reporterTimer;							//!< Timer to report device stats.

			/*!
				@brief Calback executed on MQTT connection.

    				This callback is used to restart reporter timer to match
				the intervals between individual reports. The timer will be processed even if
		 		the MQTT is not connected, but reportDevStats will quickly return.
			*/
			void onConnected();

			/*!
				@brief Reports device statistics to the broker.
			*/
			void reportDevStats() const;

		public:
			ksDevStatMqttReporter(uint8_t intervalInSeconds = 60);

			/*!
				@brief Handles component post-initialization.

				This method is responsible for reference gathering (component lookup) as well as binding to the events.

				@param app Pointer to the parent, which is ksApplication object.
				@return True on success, false on fail.
			*/
			bool postInit(ksApplication* app) override;

			/*!
				@brief Handles MQTT debug connector component loop logic.
				@param app Pointer to the parent, which is ksApplication.
				@return True on success, false on fail.
			*/
			bool loop(ksApplication* app) override;
	};
}
