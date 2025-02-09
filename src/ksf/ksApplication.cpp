/*
 *	Copyright (c) 2021-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksApplication.h"
#include "ksComponent.h"
#include "ksConstants.h"

namespace ksf
{
	bool ksApplication::loop()
	{
		/* This call will keep millis64 on track (handles rollover). */
		updateDeviceUptime();

		/* Process all components. */
		for (auto it{components.begin()}; it != components.end();)
		{
			auto& comp{*it};

			switch (comp->componentState)
			{
				case ksComponentState::Active:
				{
					if (!comp->loop(this))
						return false;
				}
				break;

				case ksComponentState::ToRemove:
				{
					 it = components.erase(it);
					 continue;
				}
				break;

				case ksComponentState::NotInitialized:
				{
					if (!comp->init(this))
						return false;
					comp->componentState = ksComponentState::Initialized;
				}
				break;
				
				case ksComponentState::Initialized:
				{
					if (!comp->postInit(this))
						return false;
					comp->componentState = ksComponentState::Active;
				}
				break;
				
				default: break;
			}
			++it;
		}

		return true;
	}

#if APP_LOG_ENABLED
	void ksApplication::log(AppLogProviderFunc_t provideLogFn) const
	{
		if (appLogCallback && provideLogFn)
		{
			std::string theLog;
			provideLogFn(theLog);
			appLogCallback(std::move(theLog));
		}
	}

	void ksApplication::setLogCallback(AppLogCallbackFunc_t logCallback)
	{
		appLogCallback = std::move(logCallback);
	}
#endif
}
