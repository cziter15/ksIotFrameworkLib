/*
 *	Copyright (c) 2019-2022, Krzysztof Strehlau
 *	
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside  LICENSE.md file
 *
 * 	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 *
 */

#pragma once

#ifndef __GXX_RTTI
	#error RTTI support is required for ksIotFrameworkLib. Please edit your platformio.ini file or board.txt file to enable RTTI!
#endif

#include "ksf/ksApplication.h"
#include "ksf/ksSafeList.h"
#include "ksf/ksComponent.h"
#include "ksf/ksComposable.h"
#include "ksf/ksConfig.h"
#include "ksf/ksConstants.h"
#include "ksf/evt/ksEvent.h"
#include "ksf/comp/ksConfigProvider.h"
#include "ksf/comp/ksLed.h"
#include "ksf/comp/ksResetButton.h"
#include "ksf/comp/ksMqttConfigProvider.h"
#include "ksf/comp/ksMqttConnector.h"
#include "ksf/comp/ksMqttDebugResponder.h"
#include "ksf/comp/ksTimer.h"
#include "ksf/comp/ksWiFiConfigurator.h"
#include "ksf/comp/ksWifiConnector.h"
#include <WiFiManager.h>
#include "WString.h"