#include "ksSafeList.h"

namespace ksf 
{

	/* ------------------------------------------------------------------------------------------------------------------------------------------ */
	//  ksSafeListInterface
	/* ------------------------------------------------------------------------------------------------------------------------------------------ */
	ksSafeListInterface::ksSafeListInterface() {}

	/* ------------------------------------------------------------------------------------------------------------------------------------------ */
	//  ksSafeListScopedSync - Handles GetAlarmAckTime request.
	/* ------------------------------------------------------------------------------------------------------------------------------------------ */
	ksSafeListScopedSync::ksSafeListScopedSync(ksSafeListInterface& listRef)
	{
		listPtr = &listRef;
	}

	ksSafeListScopedSync::~ksSafeListScopedSync()
	{
		if (listPtr)
			listPtr->synchronizeQueues();
	}
}