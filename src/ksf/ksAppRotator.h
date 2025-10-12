/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <memory>
#include <cstdint>
#include <Arduino.h>

#include "ksApplication.h"

/*!
	Helper macro that handles app (appClass) initialization and calls loop method every delayTime ms (wait).
	@param ... List of application classes.
*/
#define KSF_IMPLEMENT_APP_ROTATOR(...)				\
	ksf::ksAppRotator<__VA_ARGS__> appRotator;		\
	void setup() { KSF_FRAMEWORK_INIT() }			\
	void loop() { appRotator.loop(1); }

/*!
	Helper macro that handles app (appClass) initialization and calls loop method every delayTime ms (wait).
	@param delayBetweenLoops Delay time in milliseconds.
	@param ... List of application classes.
*/
#define KSF_IMPLEMENT_APP_ROTATOR_INTERVAL(delayBetweenLoops, ...)		\
	ksf::ksAppRotator<__VA_ARGS__> appRotator;							\
	void setup() { KSF_FRAMEWORK_INIT() }								\
	void loop() { appRotator.loop(delayBetweenLoops); }

namespace ksf
{
	/*!
		@brief Application rotator component.
		@tparam ... List of application classes to be used.
	*/
	template <class... AppTypes>
	class ksAppRotator
	{
		private:
			uint8_t appIndex{0};												//!< Index of the current application.
			std::unique_ptr<ksApplication> currentApplication{nullptr};			//!< Pointer to the current application.
			typedef std::unique_ptr<ksApplication> (*TSpawnerFunc)(); 			//!< Spawner fn type.

			/*!
				@brief Helper function template to create an application object.
				@tparam TApplicationType A type of the application.
				@return Unique pointer to the created application object.
			*/
			template <class TApplicationType>
			static std::unique_ptr<ksApplication> spawnApp() 
			{
				return std::make_unique<TApplicationType>();
			}

			static constexpr std::array<TSpawnerFunc, sizeof...(AppTypes)> appSpawners{spawnApp<AppTypes>...};		//!< Array of application creators.

		public:
			/*!
				@brief Runs the application loop.
			*/
			void loopNoDelay()
			{
				/* If current application is valid and we looped, then we're done. */
				if (currentApplication)
				{
					/* If loop fails, then destroy and try to do our business next delay. */
					if (!currentApplication->loop())
						currentApplication.reset();
					
					/* If current application is valid, then we're done. */
					return;
				}

				/* Spawn application. */
				currentApplication = appSpawners[appIndex]();

				/* If not initialized, then destroy and try to do our business next delay. */
				if (!currentApplication->init()) 
					currentApplication.reset();

				/* Set next app index. */
				if (++appIndex >= appSpawners.size())
					appIndex = 0;
			}

			/*!
				@brief Runs application loop with a delay between loops.
				@param milliseconds Delay time in milliseconds.
			*/
			void loop(unsigned long milliseconds)
			{
				loopNoDelay();
				delay(milliseconds);
			}
	};
}
