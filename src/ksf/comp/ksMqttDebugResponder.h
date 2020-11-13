#pragma once
#include "../ksComponent.h"
#include "../evt/ksEvent.h"
#include <WString.h>

namespace ksf
{
	class ksMqttConnector;
	class ksMqttDebugResponder : public ksComponent
	{
		protected:
			static const char cmdChannelName[];
			static const char logChannelName[];

			ksMqttConnector* mqtt = nullptr;
			ksComposable* app = nullptr;

			std::shared_ptr<ksEventHandle> connEventHandle;
			std::shared_ptr<ksEventHandle> msgEventHandle;

			bool breakloop = false;

			virtual void onConnected();

		public:
			virtual bool init(class ksComposable* owner) override;
			virtual bool loop() override;
			virtual void onMessage(const String& topic, const String& message);
	};
}

