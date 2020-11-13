#pragma once
#include "../ksComponent.h"
#include "../evt/ksEvent.h"

namespace ksf
{
	class ksTimer : public ksComponent
	{
		private:
			bool timer_looping = false;
			unsigned int timer_inteval = 0;
			unsigned int last_tick = 0;

		public:
			ksEvent<> onTimerExpired;

			ksTimer();
			ksTimer(unsigned int interval, bool looping = true);

			void unset();
			void set(unsigned int interval, bool looping = true);

			bool init(ksComposable* owner) override;
			bool loop() override;
	};
}

