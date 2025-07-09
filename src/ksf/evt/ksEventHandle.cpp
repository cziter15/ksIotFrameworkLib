/*
 *	Copyright (c) 2020-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksEventHandle.h"

#include "ksEventInterface.h"

namespace ksf::evt
{
	ksEventHandle::ksEventHandle(std::weak_ptr<ksEventInterface>&& eventBaseWp, std::size_t callbackUID) 
		: eventBaseWp(std::move(eventBaseWp)), callbackUID(callbackUID)
	{}

	ksEventHandle::~ksEventHandle()
	{
		if (auto eventBaseSp{eventBaseWp.lock()})
			eventBaseSp->unbind(callbackUID);
	}
}
