#pragma once
#include "../ksComponent.h"
namespace ksf
{
	class ksWifiConnector : public ksComponent
	{
		protected:
			unsigned long lastWifiCheckTime = 0;

		public:
			ksWifiConnector(const char* hostname);
			virtual bool init(class ksComposable* owner) override;
			virtual bool loop() override;
	};
}