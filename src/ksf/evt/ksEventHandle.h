/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
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
				std::weak_ptr<ksEventBase> eventBaseWp;		// Weak pointer to object.
				std::size_t callbackUID = 0;				// Unique callback ID.

			public:
				/*
					Constructs event handle.

					@param eventBaseWp Weak pointer to event object.
					@param callbackUID Unique callback ID.
				*/
				ksEventHandle(std::weak_ptr<ksEventBase>&& eventBaseWp, std::size_t callbackUID);

				/*
					Event handle destructor. Unbinds callback (removes from the list by ID).
				*/
				virtual ~ksEventHandle();
		};
	}
}