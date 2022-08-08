/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

namespace ksf
{
	namespace evt
	{
		class ksEventBase : public std::enable_shared_from_this<ksEventBase>
		{
			friend class ksEventHandle;

			protected:
				virtual void unbind(std::size_t Index) = 0;
		};
	}
}