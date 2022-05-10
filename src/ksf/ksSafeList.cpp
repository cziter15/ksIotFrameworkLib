/*    
 *	Copyright (c) 2019-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 *
 */

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