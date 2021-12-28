#pragma once
#include "../ksComponent.h"
#include "../evt/ksEvent.h"
#include <WString.h>

namespace ksf
{
	class ksMqttConnector;
	class ksMqttDebugResponder : public ksComponent, public std::enable_shared_from_this<ksMqttDebugResponder>
	{
		protected:
			static const char cmdChannelName[];
			static const char logChannelName[];

			std::weak_ptr<ksMqttConnector> mqtt_wp;
			ksComposable* app = nullptr;

			std::shared_ptr<ksEventHandle> connEventHandle;
			std::shared_ptr<ksEventHandle> msgEventHandle;

			bool breakloop = false;

			virtual void onConnected();
			virtual void onMessage(const String& topic, const String& message);

		public:
			virtual bool init(class ksComposable* owner) override;
			virtual void postInit() override;
			virtual bool loop() override;

			void respond(String message) const;

			DECLARE_KS_EVENT(customDebugHandler, ksf::ksMqttDebugResponder*, const String&, bool&)
	};
}

