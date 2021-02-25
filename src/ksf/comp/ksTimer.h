#pragma once
#include "../ksComponent.h"
#include "../evt/ksEvent.h"

namespace ksf
{
	class ksTimer : public ksComponent
	{
		private:
			bool timerLooping = false;
			unsigned int timerInterval = 0;
			unsigned int lastTick = 0;

		public:
			DECLARE_KS_EVENT(onTimerExpired)

			ksTimer();
			ksTimer(unsigned int interval, bool looping = true);

			void unset();
			void set(unsigned int interval, bool looping = true);

			bool init(ksComposable* owner) override;
			bool loop() override;
	};
}

