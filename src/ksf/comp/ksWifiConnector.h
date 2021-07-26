#pragma once
#include "../ksComponent.h"
namespace ksf
{
	class ksWifiConnector : public ksComponent
	{
		protected:
			uint32_t lastWifiCheckTime = 0;
			uint32_t lastReconnectTryTime = 0;

		public:
			ksWifiConnector(const char* hostname);
			virtual bool init(class ksComposable* owner) override;
			virtual bool loop() override;
	};
}