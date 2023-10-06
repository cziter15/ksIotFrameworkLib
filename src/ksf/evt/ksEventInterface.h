/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <memory>

namespace ksf::evt
{
	class ksEventInterface : public std::enable_shared_from_this<ksEventInterface>
	{
		friend class ksEventHandle;

		protected:
			/*
				@brief Unbinds callback from the list.
				@param callbackUID Unique callback ID
			*/
			virtual void unbind(std::size_t callbackUID) = 0;
	};
}