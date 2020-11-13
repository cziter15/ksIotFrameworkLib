/*
	 Project:	ksIotFramework
	 Author:	Krzysztof Strehlau
*/
#include "ksEventHandle.h"
#include "ksEventBase.h"

namespace ksf
{
	ksEventHandle::ksEventHandle(ksEventBase* evt, std::size_t index) :
		cb_evtbase(evt), cb_index(index)
	{
		
	}

	ksEventHandle::~ksEventHandle()
	{
		cb_evtbase->unbind(cb_index);
	}
}