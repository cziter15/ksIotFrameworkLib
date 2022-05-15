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
	namespace evt
	{
		class ksEventBase;
		class ksEventHandle
		{
			protected:
				std::weak_ptr<ksEventBase> cb_evtbase_wp;	//< Weak pointer to ksEventBase.
				std::size_t cb_uid = 0;						//< Unique callback ID.

			public:
				/*
					Constructs event handle.

					@param cb_evtbase_wp - event name to be generated inside class.
					@param uid - unique callback ID.
				*/
				ksEventHandle(const std::weak_ptr<ksEventBase>& cb_evtbase_wp, std::size_t uid);

				/*
					Event handle destructor. Unbinds callback (removes from the list by ID).
				*/
				virtual ~ksEventHandle();
		};
	}
}