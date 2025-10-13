/*
 *	Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksComponent.h"
#include "ksConstants.h"

#include "ksApplication.h"

namespace ksf
{
	ksApplication::~ksApplication() = default;

	bool ksApplication::loop()
	{
		/* Process all components. */
		for (auto it{components.begin()}; it != components.end();)
		{
			auto& comp{*it};
			switch (comp->componentState)
			{
				case ksComponentState::Active:
					if (__builtin_expect(!comp->loop(this), false))
						return false;
				break;

				case ksComponentState::ToRemove:
					 it = components.erase(it);
				continue;

				case ksComponentState::NotInitialized:
					if (__builtin_expect(!comp->init(this), false))
						return false;
					comp->componentState = ksComponentState::Initialized;
				break;
				
				case ksComponentState::Initialized:
					if (__builtin_expect(!comp->postInit(this), false))
						return false;
					comp->componentState = ksComponentState::Active;
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
