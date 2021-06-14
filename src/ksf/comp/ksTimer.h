#pragma once
#include "../ksComponent.h"
#include "../evt/ksEvent.h"

namespace ksf
{
	class ksTimer : public ksComponent
	{
		private:
			bool timerLooping = false;
			uint32_t timerInterval = 0;
			uint32_t lastTick = 0;

		public:
			DECLARE_KS_EVENT(onTimerExpired)

			ksTimer();
			ksTimer(uint32_t interval, bool looping = true);

			void unset();
			void set(uint32_t interval, bool looping = true);

			bool init(ksComposable* owner) override;
			bool loop() override;
	};
}

