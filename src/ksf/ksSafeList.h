/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <vector>
#include <list>
#include <algorithm>

namespace ksf 
{
	class ksSafeListInterface
	{
		friend class ksSafeListScopedSync;

		protected:
			ksSafeListInterface();

		public:
			virtual void forceErase() = 0;
			virtual void synchronizeQueues() = 0;
	};

	template <typename _EntryType>
	class ksSafeList : public ksSafeListInterface
	{
		protected:
			std::list<_EntryType> list, pendingAdd;			// Underlying lists of elements.
			std::vector<_EntryType> pendingRemove;			// Vector of items waiting to be removed.

		public:
			/*
				Retrieves const underlting list reference.

				@return Reference to vector of items (underlying list).
			*/
			const std::list<_EntryType>& getList() const
			{
				return list;
			}

			/*
				Queues item to be added to the list at next synchronizeQueues call.

				@param item Item reference.
			*/
			void queueAdd(const _EntryType& item)
			{
				pendingAdd.push_back(item);
			}

			/*
				Queues item to be removed from the list at next synchronizeQueues call.

				@param item Item reference.
			*/
			void queueRemove(const _EntryType& item)
			{
				pendingRemove.push_back(item);
			}

			/*
				Forcibly erases all underlying lists and queues.
				Please don't call while iterating or accessing elements on the list.
			*/
			void forceErase() override
			{
				pendingAdd.clear();
				pendingRemove.clear();
				list.clear();
			}
			
			/*
				Inserts pending-add items at the end of the lists then 
				removes pending-remove items from the list
				Please don't call while iterating or accessing elements on the list.
			*/
			void synchronizeQueues() override
			{
				if (!pendingAdd.empty())
					list.splice(list.end(), pendingAdd);

				if (!pendingRemove.empty())
				{
					list.remove_if([&](const _EntryType& item) {
						return std::find(pendingRemove.begin(), pendingRemove.end(), item) != pendingRemove.end();
					});

					pendingRemove.clear();
				}
			}
	};

	/* 
		Wrapper that simply calls synchronizeQueues on passed ksSafeList at the end of scope.
	*/
	class ksSafeListScopedSync
	{
		protected:
			ksSafeListInterface* listRawPtr;	// Unsafe list pointer (this class should be used only in small scopes!).
		
		public:
			/* 
				ksSafeListScopedSync constructor.
				@param listRef Reference to ksSafeListInterface / ksSafeList (casted internally to a pointer).
			*/
			ksSafeListScopedSync(ksSafeListInterface& listRef);

			/* 
				ksSafeListScopedSync destructor.
				Calls synchronizeQueues on passed list instance.
			*/
			virtual ~ksSafeListScopedSync();
	};
}