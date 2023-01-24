#pragma once
#include <memory>
#include <Arduino.h>

/* Helper macro that handles app (appClass) initialization and calls loop method every delayTime ms (wait). */
#define KSF_IMPLEMENT_APP_ROTATOR(...)								\
	ksf::ksAppRotator<__VA_ARGS__> appRotator;						\
	void setup() { KSF_FRAMEWORK_INIT() }							\
	void loop() { appRotator.loop(); }

namespace ksf
{
	template <class... AppTypes>
	class ksAppRotator
	{
		private:
			uint8_t appIndex{0};												// Index of the current application.
			std::unique_ptr<ksApplication> currentApplication{nullptr};			// Pointer to the current application.
			typedef std::unique_ptr<ksApplication> (*TSpawnerFunc)(); 			// Spawner fn type.

			/*
				Helper function template to create an application object.
			*/
			template <class T>
			static std::unique_ptr<ksApplication> spawnApp() 
			{
				return std::make_unique<T>();
			}

			static constexpr std::array<TSpawnerFunc, sizeof...(AppTypes)> appSpawners{spawnApp<AppTypes>...};		// Array of application creators.

		public:
			/*
				Runs the application loop.
			*/
			void loopNoDelay()
			{
				/* If current application is valid and we looped, then we're done. */
				if (currentApplication && currentApplication->loop())
					return;

				/* Spawn application. */
				currentApplication = appSpawners[appIndex]();

				/* If not initialized, then destroy and try to do our business next delay. */
				if (!currentApplication->init()) 
					currentApplication.reset(nullptr);

				/* Set next app index. */
				if (++appIndex >= appSpawners.size())
					appIndex = 0;
			}

			/*
				Runs the application loop with a delay.
			*/
			void loop(unsigned int ms = 1)
			{
				loopNoDelay();
				delay(ms);
			}
	};
}
