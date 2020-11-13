#pragma once
#include "../ksComponent.h"
#include <vector>

class WiFiManager;

namespace ksf 
{
	class ksComposable;
	class ksConfigProvider : public ksComponent
	{
		public:
			bool init(ksComposable* owner) override;
			virtual void injectManagerParameters(WiFiManager& manager) = 0;
			virtual void captureManagerParameters(WiFiManager& manager) = 0;
			bool loop() override;
	};
}
