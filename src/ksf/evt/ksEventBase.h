/*    
 *	Copyright (c) 2019-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 *
 */
 
#pragma once
namespace ksf
{
	class ksEventBase : public std::enable_shared_from_this<ksEventBase>
	{
		friend class ksEventHandle;

		protected:
			virtual void unbind(size_t Index) = 0;
	};
}