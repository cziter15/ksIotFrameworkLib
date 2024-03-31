/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <string>
#include "../ksComponent.h"
#include "../ksSimpleTimer.h"
namespace ksf::comps
{
	class ksWifiConfigurator : public ksComponent
	{
		KSF_RTTI_DECLARATIONS(ksWifiConfigurator, ksComponent)

		protected:
			ksApplication* owner{nullptr};							//!< Pointer to ksApplication object that owns this component.
			std::string deviceName;									//!< Device name (prefix).
			ksf::ksSimpleTimer configTimeout{120 * 1000};			//!< Timeout for captive portal in ms.
			ksf::ksSimpleTimer periodicTasksTimeout{1000};			//!< Timeout for loop in ms.

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
				@brief Handles ksWifiConfigurator logic.

				It starts and handles Device Portal, where the user can configure the device (that means WiFi or
				MQTT credential as well as custom defined device parameters).

				@return True if loop succedeed, otherwise false.
			*/
			bool loop(ksApplication* app) override;

			/*!
				@brief ksWifiConfigurator init function.
				@param owner Pointer to the application object
				@return True on success, false on fail.
			*/
			bool init(ksApplication* owner) override;

			/*!
				@brief ksWifiConfigurator postInit function.

				Turns on all registered LEDs in config mode.

				@param owner Pointer to ksApplication object that owns this component
				@return True on success, false on fail.
			*/
			bool postInit(ksApplication* owner) override;

			/*!
				@brief Destructor for WiFi configurator component.
			*/
			virtual ~ksWifiConfigurator();
	};
}
