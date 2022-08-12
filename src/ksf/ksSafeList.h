/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <list>
#include <algorithm>

namespace ksf 
{
	class ksSafeListInterface
	{
		friend class ksSafeListScopedSync;

		protected:
			/*
				Constructs ksSafeList.
			*/
			ksSafeListInterface();

		public:
			/*
				Unsafely (in context of the purpose) erases elements from underlying lists.
				This method must not be called while iterating ksSafeList (for example it's forbidden to erase components in component loop).
			*/
			virtual void unsafeEraseAllQueues() = 0;

			/*
				This method 'synchronizes' underlying queues. Typically called after iteration to avoid manipulation of the list while iterating.
				Under the hood, it iterates underlying list by looking for items to remove or add (two another underlying lists).
			*/
			virtual void synchronizeQueues() = 0;
	};

	template <typename _EntryType>
	class ksSafeList : public ksSafeListInterface
	{
		protected:
			std::list<_EntryType> list, pendingAdd, pendingRemove;

		public:
			/*
				Retrieves list reference.
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
				Unsafely (in context of the purpose) erases elements from underlying lists.
				This method must not be called while iterating ksSafeList (for example it's forbidden to erase components in component loop).
			*/
			void unsafeEraseAllQueues() override
			{
				pendingAdd.clear();
				pendingRemove.clear();
				list.clear();
			}
			
			/*
				This method 'synchronizes' underlying queues. Typically called after iteration to avoid manipulation of the list while iterating.
				Under the hood, it iterates underlying list by looking for items to remove or add (two another underlying lists).
			*/
			void synchronizeQueues() override
			{
				if (!pendingAdd.empty())
				{
					list.insert(list.end(), pendingAdd.begin(), pendingAdd.end());
					pendingAdd.clear();
				}

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
			ksSafeListInterface* listPtr;				// Unsafe list pointer (this class should be used only in small scopes!).
		
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