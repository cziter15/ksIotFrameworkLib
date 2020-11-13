#pragma once
#include "ksComposable.h"

#define RUN_APP_BLOCKING_LOOPED(appClass)		\
{												\
	appClass application;						\
	if (application.init())						\
	{											\
		while(application.loop())				\
		{										\
			delay(1);							\
		}										\
	}											\
}

namespace ksf
{
	class ksComposable;
}

namespace ksf 
{
	class ksApplication : public ksComposable
	{
		public:
			virtual bool init();
			virtual bool loop();
	};
}