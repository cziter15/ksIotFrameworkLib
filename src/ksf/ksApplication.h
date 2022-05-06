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
			/* Method called to initialize application. */
			virtual bool init();

			/* Called every application tick (see ksConstants). */
			virtual bool loop();
	};
}