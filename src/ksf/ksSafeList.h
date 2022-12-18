/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
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
	template <typename _EntryType>
	class ksSafeList
	{
		protected:
			std::list<_EntryType> list;					// Underlying list.
			std::list<_EntryType> pendingAdditions;		// List of items waiting to be added.
			std::vector<_EntryType> pendingRemovals;	// List of items waiting to be removed.

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
				Adds an item to the list of pending additions.
				The item will be added to the list by applyPendingOperations method.

				@param item Item reference.
			*/
			void add(const _EntryType& item)
			{
				pendingAdditions.push_back(item);
			}

			/*
				Adds an item to the list of pending removals.
				The item will be removed from the list by applyPendingOperations method.

				@param item Item reference.
			*/
			void remove(const _EntryType& item)
			{
				pendingRemovals.push_back(item);
			}

			/*
				Removes all items from the list.
				Please don't call while iterating or accessing elements on the list.
			*/
			void clearAll()
			{
				pendingAdditions.clear();
				pendingRemovals.clear();
				list.clear();
			}
			
			/*
				Applies pending operations (additions and removals).
				Please don't call while iterating or accessing elements on the list.
			*/
			void applyPendingOperations()
			{
				list.splice(list.end(), pendingAdditions);

				list.remove_if([&](const _EntryType& item) {
					return std::find(pendingRemovals.begin(), pendingRemovals.end(), item) != pendingRemovals.end();
				});

				pendingRemovals.clear();
			}
	};

	/* 
		Scoped synchronization class for ksSafeList.
		Applies pending operations on the list when the object goes out of scope.
	*/
	template <typename _ListType>
	class ksSafeListScopedSync
	{
		protected:
			_ListType& listReference;	// Reference to ksSafeList instance.
		
		public:
			/* 
				ksSafeListScopedSync constructor.
				@param listRef Reference to ksSafeList instance.
			*/
			ksSafeListScopedSync(_ListType& listRef)
				: listReference(listRef)
			{}

			/* 
				ksSafeListScopedSync destructor.
				Applies pending operations on the list.
			*/
			~ksSafeListScopedSync()
			{
				listReference.applyPendingOperations();
			}
	};
}