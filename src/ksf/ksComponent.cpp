#pragma once
#include "ksComponent.h"

namespace ksf 
{
	bool ksComponent::queueDestroy()
	{
		if (!isMarkedToDestroy)
		{
			isMarkedToDestroy = true;
			return true;
		}

		return false;
	}
}