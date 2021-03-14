#pragma once
#include "ksComposable.h"

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