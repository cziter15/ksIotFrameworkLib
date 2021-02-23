/*
	 Project:	ksIotFramework
	 Author:	Krzysztof Strehlau
*/
#include "ksEventHandle.h"
#include "ksEventBase.h"

namespace ksf
{
	ksEventHandle::ksEventHandle(ksEventBase* evt, std::size_t uid) :
		cb_evtbase(evt), cb_uid(uid)
	{
		
	}

	ksEventHandle::~ksEventHandle()
	{
		cb_evtbase->unbind(cb_uid);
	}
}