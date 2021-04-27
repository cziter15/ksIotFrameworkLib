#include "../ksComposable.h"
#include "ksResetButton.h"
#include "Arduino.h"

namespace ksf
{
	ksResetButton::ksResetButton(unsigned char pin, unsigned char triggerBy) 
		: buttonPin(pin), triggerState(triggerBy)
	{
	}

	bool ksResetButton::init(ksComposable* owner)
	{
		pinMode(buttonPin, INPUT);
		return true;
	}

	bool ksResetButton::loop()
	{
		// If push button is pressed, then then break app (goes to config app)
		if (digitalRead(buttonPin) == triggerState)
			return false;

		return true;
	}

	ksResetButton::~ksResetButton()
	{
		pinMode(buttonPin, INPUT);
	}
}
