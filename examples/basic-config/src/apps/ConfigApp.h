#pragma once

#include <ksIotFrameworkLib.h>

namespace apps
{
	class ConfigApp : public ksf::ksApplication
	{
		public:
			static const char myDeviceName[];

			bool init() override;
			bool loop() override;
	};
}