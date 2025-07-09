/*
 *	Copyright (c) 2020-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <memory>
#include <cstddef>

namespace ksf::evt
{
	/*!
		@brief Implements interface for multicasting events.
	*/
	class ksEventInterface : public std::enable_shared_from_this<ksEventInterface>
	{
		friend class ksEventHandle;

		protected:
			/*!
				@brief Unbinds callback from the list.
				@param callbackUID Unique callback ID
			*/
			virtual void unbind(std::size_t callbackUID) = 0;

		public:
			virtual ~ksEventInterface() = default;
	};
}
