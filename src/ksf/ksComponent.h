#pragma once
#include <memory>

namespace ksf 
{
	class ksComposable;
	class ksComponent : public std::enable_shared_from_this<ksComponent>
	{
		public:
			virtual bool init(ksComposable* owner) = 0;
			virtual bool loop() = 0;
			virtual void postInit() {}
	};
}