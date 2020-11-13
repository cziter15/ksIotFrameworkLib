#pragma once
#include "../ksComponent.h"
#include <WString.h>
#include <WiFiManager.h>

namespace ksf
{
	class ksWiFiConfigurator : public ksComponent
	{
		protected:
			class ksComposable* parent;
			WiFiManager manager;

			String deviceName;

		public:
			ksWiFiConfigurator(const String& devicePrefixName = String("ksWiFiConfigurator"));
			bool init(class ksComposable* owner) override;
			bool loop() override;
			void postInit() override;
	};
}