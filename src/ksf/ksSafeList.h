#pragma once
#include <vector>
#include <algorithm>

namespace ksf 
{
	template <typename EntryType>
	class ksSafeList
	{
		protected:
			std::vector<EntryType> list, pendingAdd, pendingRemove;

		public:
			const std::vector<EntryType>& getList() const
			{
				return list;
			}

			void queueAdd(const EntryType& item)
			{
				pendingAdd.push_back(item);
			}

			void queueRemove(const EntryType& item)
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
}