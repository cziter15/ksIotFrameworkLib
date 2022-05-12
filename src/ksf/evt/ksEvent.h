/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <vector>
#include <memory>
#include "ksEventBase.h"
#include "ksEventHandle.h"

 /*
	 Defines user event. Usage: DECLARE_KS_EVENT( your_event_name, event parameters... ).

	 @param evtName - event name to be generated inside class.
	 @param params... - event parameters.
 */
#define DECLARE_KS_EVENT(evtName, ...) \
	std::shared_ptr<ksf::ksEvent<__VA_ARGS__>> evtName = std::make_shared<ksf::ksEvent<__VA_ARGS__>>();

namespace ksf
{
	template <typename... Params>
	class ksEvent : public ksEventBase
	{
		protected:
			std::vector<std::pair<std::size_t, std::function<void(Params...)>>> callbacks;		//< List of bond callbacks.
			std::size_t lastUid = 0;															//< Last unique callback ID for this event (used as counter).

		public:
			/*
				Queries if event has any bound callbacks.
				@return - true if any callback is bound, false if no callback is bound.
			*/
			bool hasAnyCallbacks() const
			{
				return !callbacks.empty();
			}

			/*
				Registers event (binds to callback list).
				@param outHandle - reference to outHandle shared ptr (will be set to shared_ptr of ksEventHandle, will unbind from the list on pointer destruction).
				@param function - lvalue reference to callback function.
			*/
			void registerEvent(std::shared_ptr<ksEventHandle>& outHandle, std::function<void(Params...)>&& function)
			{
				++lastUid;
				outHandle = std::make_shared<ksEventHandle>(shared_from_this(), lastUid);
			
				callbacks.emplace_back(lastUid, std::move(function));
			}

			/*
				Unbinds event callback by specified unique ID. This ID is automatically assigned
				by registerEvent function and ksEventHandle uses it to unbind automatically upon destruction.
				@param cbUid - unique id of the callback to unbind.
			*/
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

			/*
				Broadcast event (calls all bound callbacks).
				@param User defined parameters (va args).
			*/
			void broadcast(Params... params)
			{
				for (auto cb = callbacks.begin(); cb != callbacks.end(); ++cb)
					cb->second(params...);
			}
	};
}