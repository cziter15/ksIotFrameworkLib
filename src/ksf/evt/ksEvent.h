/*
	 Project:	ksIotFramework
	 Author:	Krzysztof Strehlau
*/

#pragma once

#include <vector>
#include <memory>
#include "ksEventBase.h"
#include "ksEventHandle.h"

namespace ksf
{
	template <typename... Params>
	class ksEvent : public ksEventBase
	{
		protected:
			std::vector<std::function<void(Params...)>> callbacks;

		public:
			void registerEvent(std::shared_ptr<ksEventHandle>& outHandle, std::function<void(Params...)> function)
			{
				outHandle = std::make_shared<ksEventHandle>(this, (int)callbacks.size());
				callbacks.push_back(function);
			}

			void unbind(std::size_t index) override
			{
				if (index >= 0 && index <= callbacks.size())
					callbacks.erase(callbacks.begin() + index);
			}

			void broadcast(Params... params)
			{
				for (auto cb = callbacks.begin(); cb != callbacks.end(); ++cb)
					(*cb)(params...);
			}
	};
}