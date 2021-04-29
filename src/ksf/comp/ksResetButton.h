#pragma once
#include "../ksComponent.h"

namespace ksf
{
	class ksResetButton : public ksComponent
	{
		protected:
			unsigned char buttonPin = 0;
			unsigned char triggerState = 0;
			unsigned char lastState = 0;
			unsigned long pressedTime = 0;
			unsigned long releasedTime = 0;

		public:
			ksResetButton(unsigned char pin, unsigned char triggerBy);

			bool init(class ksComposable* owner) override;
			bool loop() override;

			virtual ~ksResetButton();
	};
}

