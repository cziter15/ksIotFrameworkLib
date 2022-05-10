/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
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