/*
	 Project:	ksIotFramework
	 Author:	Krzysztof Strehlau
*/
#include "ksEventHandle.h"
#include "ksEventBase.h"

namespace ksf
{
	ksEventHandle::ksEventHandle(std::weak_ptr<ksEventBase> evtbase_wp, std::size_t uid) :
		cb_evtbase_wp(evtbase_wp), cb_uid(uid)
	{

	}

	ksEventHandle::~ksEventHandle()
	{
		if (auto cb_evtbase_sp = cb_evtbase_wp.lock())
			cb_evtbase_sp->unbind(cb_uid);
	}
}