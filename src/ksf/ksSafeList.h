/*
 *	Copyright (c) 2021-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#pragma once

#include <vector>
#include <algorithm>

namespace ksf 
{
	class ksSafeListInterface
	{
		friend class ksSafeListScopedSync;

		protected:
			ksSafeListInterface();

		public:
			virtual void unsafeEraseAllQueues() = 0;
			virtual void synchronizeQueues() = 0;
	};

	template <typename EntryType>
	class ksSafeList : public ksSafeListInterface
	{
		protected:
			std::vector<EntryType> list, pendingAdd, pendingRemove;

		public:
			/*
				Retrieves list reference.
				@return Reference to vector of items (underlying list).
			*/
			const std::vector<EntryType>& getList() const
			{
				return list;
			}

			/*
				Queues item to be added to the list at next synchronizeQueues call.
				@param item Item reference.
			*/
			void queueAdd(const EntryType& item)
			{
				pendingAdd.push_back(item);
			}

			/*
				Queues item to be removed from the list at next synchronizeQueues call.
				@param item Item reference.
			*/
			void queueRemove(const EntryType& item)
			{
				pendingRemove.push_back(item);
			}

			/*
				Unsafely erases all queues (pendingAdd, pendingRemove and current list).
			*/
			void unsafeEraseAllQueues() override
			{
				pendingAdd.clear();
				pendingRemove.clear();
				list.clear();
			}
			
			/*
				Synchronizes underlying list with queues.
				Adds items from pendingAdd list.
				Removes item by using pendingRemove list.
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
					std::vector< EntryType > remainingItems;

					for (auto& item : list)
					{
						if (std::find(pendingRemove.begin(), pendingRemove.end(), item) == pendingRemove.end())
							remainingItems.push_back(item);
					}

					list = std::move(remainingItems);
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
			ksSafeListInterface* listPtr;
		
		public:
			ksSafeListScopedSync(ksSafeListInterface& listRef);
			~ksSafeListScopedSync();
	};
}