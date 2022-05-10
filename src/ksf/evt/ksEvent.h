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

#include <vector>
#include <memory>
#include "ksEventBase.h"
#include "ksEventHandle.h"

#define DECLARE_KS_EVENT(evtName, ...) \
	std::shared_ptr<ksf::ksEvent<__VA_ARGS__>> evtName = std::make_shared<ksf::ksEvent<__VA_ARGS__>>();

namespace ksf
{
	template <typename... Params>
	class ksEvent : public ksEventBase
	{
		protected:
			std::vector<std::pair<std::size_t, std::function<void(Params...)>>> callbacks;
			std::size_t lastUid = 0;

		public:
			bool hasAnyCallbacks() const
			{
				return !callbacks.empty();
			}

			void registerEvent(std::shared_ptr<ksEventHandle>& outHandle, std::function<void(Params...)>&& function)
			{
				++lastUid;
				outHandle = std::make_shared<ksEventHandle>(shared_from_this(), lastUid);
			
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