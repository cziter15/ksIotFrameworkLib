#pragma once
#include "../ksComponent.h"
#include <Arduino.h>

namespace ksf
{
	class ksResetButton : public ksComponent
	{
		protected:
			uint8_t buttonPin = 0;
			uint8_t triggerState = 0;
			uint8_t lastState = 0;
			uint8_t pmode = INPUT;
			uint32_t pressedTime = 0;
			uint32_t releasedTime = 0;

		public:
			ksResetButton(uint8_t pin, uint8_t triggerBy, uint8_t mode = INPUT);

			bool init(class ksComposable* owner) override;
			bool loop() override;

			virtual ~ksResetButton();
	};
}

