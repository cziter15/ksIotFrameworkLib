/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "ksSafeList.h"

namespace ksf 
{
	ksSafeListInterface::ksSafeListInterface() 
	{}
	
	ksSafeListScopedSync::ksSafeListScopedSync(ksSafeListInterface& listRef) 
		: listRawPtr(&listRef)
	{}

	ksSafeListScopedSync::~ksSafeListScopedSync()
	{
		listRawPtr->applyPendingOperations();
	}
}