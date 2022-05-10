/*    
 *	Copyright (c) 2019-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 *
 */

#pragma once
#include "../ksComponent.h"

namespace ksf
{
	class ksLed : public ksComponent
	{
		protected:
			uint8_t ledPin = 0;

			uint32_t lastBlinkTime = 0;
			uint32_t blinkInterval = 0;
			uint32_t blinkLoops = 0;

		public:
			ksLed(uint8_t pin);

			bool init(class ksComposable* owner) override;
			bool loop() override;

			virtual void setBlinking(uint32_t interval, uint32_t loops = 0);
			virtual bool isBlinking() const;

			virtual bool isEnabled() const;
			virtual void setEnabled(bool enabled);

			virtual ~ksLed();
	};
}

