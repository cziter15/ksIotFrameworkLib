/*
	 Project:	ksIotFramework
	 Author:	Krzysztof Strehlau
*/

#pragma once
#include <memory>
#include <cstddef>

namespace ksf
{
	class ksEventBase;
	class ksEventHandle
	{
		protected:
			std::weak_ptr<ksEventBase> cb_evtbase_wp;
			std::size_t cb_uid = 0;

		public:
			ksEventHandle(std::weak_ptr<ksEventBase> cb_evtbase_wp, std::size_t uid);
			virtual ~ksEventHandle();
	};
}