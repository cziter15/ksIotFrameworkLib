#pragma once
#include <vector>
#include <algorithm>

namespace ksf 
{
	template <typename EntryType>
	class ksSafeList
	{
		protected:
			std::vector< EntryType > list, pendingAdd, pendingRemove;

		public:
			std::vector< EntryType >& items()
			{
				return list;
			}

			void queueAdd(EntryType&& item)
			{
				pendingAdd.emplace_back(item);
			}

			void queueRemove(EntryType&& item)
			{
				pendingRemove.emplace_back(item);
			}

			void queueAdd(EntryType item)
			{
				pendingAdd.push_back(item);
			}

			void queueRemove(EntryType item)
			{
				pendingRemove.push_back(item);
			}
			
			void unsafeEraseAllQueues()
			{
				pendingAdd.clear();
				pendingRemove.clear();
				list.clear();
			}
			
			void synchronizeQueues()
			{
				if (pendingRemove.size() > 0)
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

				if (pendingAdd.size() > 0)
				{
					list.insert(list.end(), pendingAdd.begin(), pendingAdd.end());
					pendingAdd.clear();
				}
			}
	};
}