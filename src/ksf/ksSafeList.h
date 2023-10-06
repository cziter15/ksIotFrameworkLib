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
	template <typename TEntryType>
	class ksSafeList
	{
		protected:
			std::list<TEntryType> list;					// Underlying list.
			std::list<TEntryType> pendingAdditions;		// List of items waiting to be added.
			std::vector<TEntryType> pendingRemovals;	// List of items waiting to be removed.

		public:
			/*
				@brief Retrieves const underlting list reference.
				@return Reference to vector of items (underlying list).
			*/
			const std::list<TEntryType>& getRef() const
			{
				return list;
			}

			/*
				@brief Adds an item to the list of pending additions.

				The item will be added to the list by applyPendingOperations method.

				@param item Item r-value reference
			*/
			template <typename TRefType>
			void add(TRefType&& item)
			{
				pendingAdditions.emplace_back(std::forward<TRefType>(item));
			}

			/*
				@brief Adds an item to the list of pending removals.

				The item will be removed from the list by applyPendingOperations method.

				@param item Item r-value reference
			*/
			template <typename TRefType>
			void remove(TRefType&& item)
			{
				pendingRemovals.emplace_back(std::forward<TRefType>(item));
			}

			/*
				@brief Removes all items from the list.

				Please don't call while iterating or accessing elements on the list.
			*/
			void clearAll()
			{
				pendingAdditions.clear();
				pendingRemovals.clear();
				list.clear();
			}
			
			/*
				@brief Applies pending operations (additions and removals).

				Please don't call while iterating or accessing elements on the list.
			*/
			void applyPendingOperations()
			{
				list.splice(list.end(), pendingAdditions);

				list.remove_if([&](const TEntryType& item) {
					return std::find(pendingRemovals.begin(), pendingRemovals.end(), item) != pendingRemovals.end();
				});

				pendingRemovals.clear();
			}
	};

	/* 
		Scoped synchronization class for ksSafeList.
		Applies pending operations on the list when the object goes out of scope.
	*/
	template <typename TListType>
	class ksSafeListScopedSync
	{
		protected:
			TListType& listReference;	// Reference to ksSafeList instance.
		
		public:
			/*
				@brief ksSafeListScopedSync constructor.
				@param listRef Reference to ksSafeList instance
			*/
			ksSafeListScopedSync(TListType& listRef)
				: listReference(listRef)
			{}

			/*
				@brief ksSafeListScopedSync destructor.

				Applies pending operations on the list.
			*/
			~ksSafeListScopedSync()
			{
				listReference.applyPendingOperations();
			}
	};
}