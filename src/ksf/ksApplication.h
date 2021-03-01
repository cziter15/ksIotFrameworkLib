#pragma once
#include "ksComposable.h"

#define RUN_APP_BLOCKING_LOOPED_TIME(appClass, delayTime)	\
{															\
	appClass application;									\
	if (application.init())									\
	{														\
		while(application.loop())							\
		{													\
			if (delayTime == 0)								\
				yield();									\
			else											\
				delay(delayTime);							\
		}													\
	}														\
}

#define RUN_APP_BLOCKING_LOOPED(appClass) RUN_APP_BLOCKING_LOOPED_TIME(appClass, 1)
#define RUN_APP_BLOCKING_LOOPED_NODELAY(appClass)  RUN_APP_BLOCKING_LOOPED_TIME(appClass, 0)

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