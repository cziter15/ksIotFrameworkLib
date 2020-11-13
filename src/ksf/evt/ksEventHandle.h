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
			std::size_t cb_index = 0;

		public:
			ksEventHandle(ksEventBase* cb_evtbase, std::size_t index);
			virtual ~ksEventHandle();
	};
}