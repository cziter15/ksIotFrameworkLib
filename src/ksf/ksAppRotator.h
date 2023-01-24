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
	template <class... Types>
	class ksAppRotator
	{
		private:
			uint8_t appIndex{0};												// Index of the current application.
			std::unique_ptr<ksApplication> currentApplication{nullptr};			// Pointer to the current application.

			/*
				Helper function template to create an application object.
			*/
			template <class T>
			static std::unique_ptr<ksApplication> create() 
			{
				return std::make_unique<T>();
			}

			static constexpr std::array<std::unique_ptr<ksApplication>(*)(), sizeof...(Types)> creators{create<Types>...};		// Array of application creators.

		public:
			/*
				Runs the application loop.
			*/
			void loopNoDelay()
			{
				/* If current application is valid and we looped, then we're done. */
				if (currentApplication && currentApplication->loop())
					return;

				/* Switch to the next application. */
				currentApplication = creators[appIndex++ % creators.size()]();

				/* If not initialized, then destroy and try to do our business next delay. */
				if (!currentApplication->init()) 
					currentApplication.reset();
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
