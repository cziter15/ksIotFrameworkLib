/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <memory>
#include <cstddef>

namespace ksf::evt
{
	class ksEventInterface;
	class ksEventHandle
	{
		protected:
			std::weak_ptr<ksEventInterface> eventBaseWp;	//!< Weak pointer to event object.
			std::size_t callbackUID{0};						//!< Unique callback ID.

		public:
			/*!
				@brief Constructs event handle.
				@param eventBaseWp R-value reference to weak pointer to event object.
				@param callbackUID Unique callback ID.
			*/
			ksEventHandle(std::weak_ptr<ksEventInterface>&& eventBaseWp, std::size_t callbackUID);

			/*!
				@brief Destructs event handle. Unbinds assigned callback from the list.
			*/
			virtual ~ksEventHandle();
	};
}