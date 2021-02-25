/*
	 Project:	ksIotFramework
	 Author:	Krzysztof Strehlau
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