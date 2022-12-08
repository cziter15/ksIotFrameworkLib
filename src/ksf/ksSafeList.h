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
			virtual void applyPendingOperations() = 0;
	};

	template <typename _EntryType>
	class ksSafeList : public ksSafeListInterface
	{
		protected:
			std::list<_EntryType> list, pendingAdditions;			// Underlying lists of elements.
			std::vector<_EntryType> pendingRemovals;				// Vector of items waiting to be removed.

		public:
			/*
				Retrieves const underlting list reference.

				@return Reference to vector of items (underlying list).
			*/
			const std::list<_EntryType>& getRef() const
			{
				return list;
			}

			/*
				Queues item to be added to the list at next synchronizeQueues call.

				@param item Item reference.
			*/
			void add(const _EntryType& item)
			{
				pendingAdditions.push_back(item);
			}

			/*
				Queues item to be removed from the list at next synchronizeQueues call.

				@param item Item reference.
			*/
			void remove(const _EntryType& item)
			{
				pendingRemovals.push_back(item);
			}

			/*
				Clears all underlying lists.
				Please don't call while iterating or accessing elements on the list.
			*/
			void clearAll()
			{
				pendingAdditions.clear();
				pendingRemovals.clear();
				list.clear();
			}
			
			/*
				Applies pending operations.
				Please don't call while iterating or accessing elements on the list.
			*/
			void applyPendingOperations() override
			{
				if (!pendingAdditions.empty())
					list.splice(list.end(), pendingAdditions);

				if (!pendingRemovals.empty())
				{
					list.remove_if([&](const _EntryType& item) {
						return std::find(pendingRemovals.begin(), pendingRemovals.end(), item) != pendingRemovals.end();
					});

					pendingRemovals.clear();
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