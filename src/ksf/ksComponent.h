#pragma once
#include <memory>
#include "ksConstants.h"
namespace ksf 
{
	class ksComposable;
	class ksComponent
	{
#if KSF_NO_RTTI
		friend class ksComposable;
		protected:
			typeid_t comp_type_id;
#endif
		friend class ksApplication;

		public:
			virtual bool init(ksComposable* owner) = 0;
			virtual bool loop();
			virtual void postInit();
	};
}