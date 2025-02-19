/*
 *	Copyright (c) 2021-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
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
	/*!
		@brief A component that manages WiFi connection.

		This component handles WiFi connection, reconnecting automatically in case of disconnection.

		There are many things that this component manages:
		- MAC address generation (to easily identify ksIotFrameworkLib based devices)
		- Simple power management (DTIM low-power mode)
		- IP address validation, used to determine if WiFi connection was successful (not only using WiFi PHY state)
		- Timeout management, used to break in case of longer WiFi connection issues (causing application to exit and swittch to configuration mode)
	*/
	class ksWifiConnector : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksWifiConnector, ksComponent)
		
		protected:
			misc::ksSimpleTimer wifiTimeoutTimer;						//!< Wifi timer - long timeout in case of issues.
			misc::ksSimpleTimer wifiReconnectTimer;						//!< Wifi timer - reconnection timeout.
			misc::ksSimpleTimer wifiIpCheckTimer;						//!< Wifi timer - IP check interval.

			struct
			{
				bool wasConnected : 1;									//!< True if connected in previous loop.
				bool savePower : 1;										//!< True to save power.
				bool gotIpAddress : 1;									//!< True if IP address is set.
			} bitflags = {false, true, false};

			/*!
				@brief Internal method that generates MAC address for the device.
				
				This MAC is generated by using original ChipID / MAC, but starts with 0xFA 0xF1 and then there are four Chip ID bytes.
				0xFA 0xF1 works like prefix / signature to help identyfying devices in router properties / Wireshark (debugging) etc.
			*/
			void setupMacAddress();

		public:
			DECLARE_KS_EVENT(onConnected)								// onConnected event that user can bind to.
			DECLARE_KS_EVENT(onDisconnected)							// onDisconnected event that user can bind to.

			/*!
				@brief Constructs WiFi connector component.
				@param hostname Hostname of the device, used also by mDNS service.
				@param savePower If set, will put the device in modem sleep mode. This is useful to reduce power consumption, but increases reaction time.
			*/
			ksWifiConnector(const char* hostname, bool savePower = true);

			/*!
				@brief Destructos WiFi connector component.
			*/
			virtual ~ksWifiConnector();

			/*!
				@brief Initializes WiFi connector component.
				@param app Pointer to the parent ksApplication.
				@return True on success, false on fail.
			*/
			bool init(ksApplication* app) override;

			/*!
				@brief Handles WiFi connector component loop logic.
				@param app Pointer to the parent ksApplication.
				@return True on success, false on fail.
			*/
			bool loop(ksApplication* app) override;

			/*!
				@brief Returns whether WiFi is connected or not.
				@return True if connected. False otherwise.
			*/
			bool isConnected() const;
	};
}
