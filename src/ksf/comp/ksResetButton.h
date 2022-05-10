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

