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
#include "../ksSimpleTimer.h"
#include "../evt/ksEvent.h"

namespace ksf
{
	namespace comps
	{
		class ksWifiConnector : public ksComponent
		{
			KSF_RTTI_DECLARATIONS(ksWifiConnector, ksComponent)
			
			protected:
				ksSimpleTimer wifiTimeoutTimer{KSF_WIFI_TIMEOUT_MS};				// Wifi timer - timeout.
				ksSimpleTimer wifiReconnectTimer{KSF_WIFI_RECONNECT_TIME_MS};		// Wifi timer - reconnect.
				ksSimpleTimer wifiIpCheckTimer{KSF_ONE_SECOND_MS};					// Wifi timer - IP check.

				bool wasConnected{false};											// True if connected in previous loop.
				bool gotIpAddress{false};											// IP address state.

				/*
					Configures MAC address for devices using ksIotFrameworkLib.

					This MAC is generated by using original ChipID / MAC, but starts with 0xFA 0xF1 and then there are four Chip ID bytes.
					0xFA 0xF1 works like prefix / signature to help identyfying devices in router properties / Wireshark (debugging) etc.
				*/
				void setupMacAddress();

				/*
					Internal method called on WiFi connection.
				*/
				void wifiConnectedInternal();

				/*
					Internal method called on WiFi diconnection.
				*/
				void wifiDisconnectedInternal();

				/*
					Internal method to connect to AP.

					@return True if connect succedeed, otherwise false.
				*/
				bool connectStation();

				/*
					Internal method to disconnect from AP.

					@return True if disconnected properly, otherwise false.
				*/
				bool disconnectStation();

			public:
				DECLARE_KS_EVENT(onConnected)									// onConnected event that user can bind to.
				DECLARE_KS_EVENT(onDisconnected)								// onDisconnected event that user can bind to.

				/*
					Constructs ksWifiConnector component.

					@param hostname Hostname to be used by WiFi class.
				*/
				ksWifiConnector(const char* hostname);

				/*
					Initializes ksWifiConnector component.

					@param owner Pointer to ownning ksComposable object (application).
					@return True if init succedeed, otherwise false.
				*/
				bool init(class ksf::ksComposable* owner) override;

				/*
					Handles WiFi connector component loop logic.

					@return True on success, false on fail.
				*/
				bool loop() override;

				/*
					Returns connection state.

					@return True if connected, false if not.
				*/
				bool isConnected() const;
		};
	}
}