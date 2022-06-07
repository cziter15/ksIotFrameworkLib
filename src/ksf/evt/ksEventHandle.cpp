/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksEventHandle.h"
#include "ksEventBase.h"

namespace ksf::evt
{
	ksEventHandle::ksEventHandle(const std::weak_ptr<ksEventBase>& evtbase_wp, std::size_t uid) :
		cb_evtbase_wp(evtbase_wp), cb_uid(uid)
	{}

	ksEventHandle::~ksEventHandle()
	{
		if (auto cb_evtbase_sp = cb_evtbase_wp.lock())
			cb_evtbase_sp->unbind(cb_uid);
	}
}