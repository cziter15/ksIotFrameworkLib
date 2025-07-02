/*
 *	Copyright (c) 2021-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>

#include "../ksComponent.h"
#include "../misc/ksSimpleTimer.h"

namespace ksf::comps
{
	/*!
		@brief A component that manages AP-based configuration.

		This component handles the Device Portal, allowing users to configure WiFi and MQTT credentials, 
		as well as custom application-specific parameters implemented via the ksConfigProvider interface.
	*/
	class ksWifiConfigurator : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksWifiConfigurator, ksComponent)

		protected:
			ksApplication* app{nullptr};							//!< Pointer to ksApplication object that owns this component.
			std::string deviceName;									//!< Device name (prefix).
			misc::ksSimpleTimer configTimeout{120 * 1000};			//!< Timeout for captive portal in ms.
			misc::ksSimpleTimer periodicTasksTimeout{1000};			//!< Timeout for loop in ms.

			/*!
				@brief Handles periodic tasks like WiFi management.
			*/
			void handlePeriodicTasks();

		public:
			/*!
				@brief Constructs WiFi configurator object.
			*/
			ksWifiConfigurator();

			/*!
				@brief Constructs WiFi configurator object.
				@param devicePrefixName Device prefix name, will be used in AP (DEVPREFIX-112ACB84) and hostname
			*/
			ksWifiConfigurator(std::string devicePrefixName);

			/*!
				@brief Destructs WiFi configurator component.
			*/
			virtual ~ksWifiConfigurator();

			/*!
				@brief Handles ksWifiConfigurator logic.

				It starts and handles Device Portal, where the user can configure the device (that means WiFi or
				MQTT credential as well as custom defined device parameters).

				@param app Pointer to ksApplication object that owns this component.
				@return True if loop succedeed, otherwise false.
			*/
			bool loop(ksApplication* app) override;

			/*!
				@brief Implements ksWifiConfigurator initialization logic.
				@param app Pointer to ksApplication object that owns this component
				@return True on success, false on fail.
			*/
			bool init(ksApplication* app) override;

			/*!
				@brief Implements ksWifiConfigurator post-initialization logic.
				@param app Pointer to ksApplication object that owns this component
				@return True on success, false on fail.
			*/
			bool postInit(ksApplication* app) override;
	};
}
