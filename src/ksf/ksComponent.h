#pragma once
#include <memory>

namespace ksf 
{
	class ksComposable;
	class ksComponent
	{
		friend class ksApplication;

		protected:
			bool isMarkedToDestroy = false;

		public:
			bool queueDestroy();

			virtual bool init(ksComposable* owner) = 0;
			virtual bool loop() = 0;
			virtual void postInit() {}
	};
}