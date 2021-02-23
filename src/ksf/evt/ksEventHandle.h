/*
	 Project:	ksIotFramework
	 Author:	Krzysztof Strehlau
*/

#pragma once

#include <cstddef>

namespace ksf
{
	class ksEventBase;
	class ksEventHandle
	{
		protected:
			ksEventBase* cb_evtbase = nullptr;
			std::size_t cb_uid = 0;

		public:
			ksEventHandle(ksEventBase* cb_evtbase, std::size_t uid);
			virtual ~ksEventHandle();
	};
}