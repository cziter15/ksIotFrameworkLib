/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "ksEventInterface.h"
#include "ksEventHandle.h"

 /*!
	 @brief Defines user event. Usage: DECLARE_KS_EVENT( your_event_name, event parameters... ).
	 @param evtName Name of the event field that will be generated inside class.
	 @param params... Event parameters.
 */
#define DECLARE_KS_EVENT(evtName, ...) \
	std::shared_ptr<ksf::evt::ksEvent<__VA_ARGS__>> evtName {std::make_shared<ksf::evt::ksEvent<__VA_ARGS__>>()};

namespace ksf::evt
{
	template <typename... Params>
	class ksEvent : public evt::ksEventInterface
	{
		protected:
			std::vector<std::pair<std::size_t, std::function<void(Params...)>>> callbacks;	//!< List of bond callbacks.
			std::size_t lastCallbackUID{0};													//!< Last unique callback ID for this event (used as counter).

		public:
			/*!
				@brief Returns whether any callback is bound to this event.
				@return True if any callback is bound to this event. False otherwise.
			*/
			bool isBound() const
			{
				return !callbacks.empty();
			}

			/*!
				@brief Registers event (binds to callback list).
				@param outHandle Reference to outHandle shared ptr (destruction of ksEventHandle object will unbind the event)
				@param function R-value reference to callback function
			*/
			void registerEvent(std::unique_ptr<ksf::evt::ksEventHandle>& outHandle, std::function<void(Params...)>&& function)
			{
				++lastCallbackUID;
				outHandle = std::make_unique<ksf::evt::ksEventHandle>(weak_from_this(), lastCallbackUID);
				callbacks.emplace_back(lastCallbackUID, std::move(function));
			}

			/*!
				@brief Unbinds event callback by specified unique ID. 
				
				This ID is automatically assigned by registerEvent function and
				ksEventHandle uses it to unbind automatically upon destruction.

				@param callbackUID Unique id of the callback to unbind.
			*/
			void unbind(std::size_t callbackUID) override
			{
				auto predicate = [callbackUID](const auto& cb) {
					auto& [uid, function] = cb;
					return uid == callbackUID; 
				};

				callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(), predicate), callbacks.end());
			}

			/*!
				@brief Broadcasts event to all bound callbacks.
				@param args Event parameters.
			*/
			void broadcast(Params... params)
			{
				for (const auto& [uid, function] : callbacks)
					function(params...);
			}
	};
}
