#pragma once
#include "../ksComponent.h"

namespace ksf
{
	class ksLed : public ksComponent
	{
		protected:
			unsigned short ledPin = 0;
			unsigned int lastBlinkTime = 0;
			unsigned int blinkInterval = 0;
			unsigned int blinkLoops = 0;

		public:
			ksLed(unsigned short pin);

			bool init(class ksComposable* owner) override;
			bool loop() override;

			virtual void setBlinking(unsigned int interval, unsigned int loops = 0);
			virtual bool isBlinking() const;

			virtual bool isEnabled() const;
			virtual void setEnabled(bool enabled);

			virtual ~ksLed();
	};
}

