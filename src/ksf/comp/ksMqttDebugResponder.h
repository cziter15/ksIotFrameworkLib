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
#include "../evt/ksEvent.h"
#include <WString.h>

namespace ksf
{
	class ksMqttConnector;
	class ksMqttDebugResponder : public ksComponent, public std::enable_shared_from_this<ksMqttDebugResponder>
	{
		protected:
			uint32_t secondTimer = 0;
			uint32_t deviceUptimeSeconds = 0;

			std::weak_ptr<ksMqttConnector> mqtt_wp;
			ksComposable* app = nullptr;

			std::shared_ptr<ksEventHandle> connEventHandle;
			std::shared_ptr<ksEventHandle> msgEventHandle;

			bool breakloop = false;

			virtual void onConnected();
			virtual void onMessage(const String& topic, const String& message);

			String getResetReason();

		public:
			virtual bool init(class ksComposable* owner) override;
			virtual void postInit() override;
			virtual bool loop() override;

			void respond(String message) const;

			DECLARE_KS_EVENT(customDebugHandler, ksf::ksMqttDebugResponder*, const String&, bool&)
	};
}