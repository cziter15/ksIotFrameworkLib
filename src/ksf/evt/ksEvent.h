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
			std::vector<std::pair<std::size_t, std::function<void(Params...)>>> callbacks;
			std::size_t lastUid = 0;

		public:
			void registerEvent(std::shared_ptr<ksEventHandle>& outHandle, std::function<void(Params...)>&& function)
			{
				++lastUid;
				outHandle = std::make_shared<ksEventHandle>(this, lastUid);
				callbacks.emplace_back(lastUid, std::move(function));
			}

			void unbind(std::size_t cbUid) override
			{
				for (auto cb = callbacks.begin(); cb != callbacks.end(); ++cb)
				{
					if (cb->first == cbUid)
					{
						callbacks.erase(cb);
						break;
					}
				}
			}

			void broadcast(Params... params)
			{
				for (auto cb = callbacks.begin(); cb != callbacks.end(); ++cb)
					cb->second(params...);
			}
	};
}