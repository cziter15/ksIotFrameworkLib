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
			typeid_t comp_type_id;		//< A part of Type ID functionality, will contain a pointer to instantiated variable (by template).
#endif
		friend class ksApplication;

		public:
			/* Method called to initialize component. */
			virtual bool init(ksComposable* owner) = 0;

			/* Called from application loop. */
			virtual bool loop();

			/* Method called after component initialization. */
			virtual void postInit();
	};
}