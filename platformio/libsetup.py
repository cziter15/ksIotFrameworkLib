#	Copyright (c) 2021-2022, Krzysztof Strehlau
#	
#	This file is part of the ksIotFramework library.
#	All licensing information can be found inside LICENSE.md file
#
#	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE

from logger import *
ksPrintLog(Colors.Green, "Running extra script for library.")

try:
	Import("projenv", "env")
	ksPrintLog(Colors.Magenta, "Building library environment list.")
	environments = [env, DefaultEnvironment(), projenv]
	
	for lb in env.GetLibBuilders():
		environments.append(lb.env)

	for e in environments:
		# remove unused flags
		e.ProcessUnFlags("-std=gnu++11")
		e.ProcessUnFlags("-fexceptions")
		# add flags to environment
		e.ProcessFlags("-std=gnu++2a")
		e.ProcessFlags("-fno-exceptions")
		e.ProcessFlags("-DCORE_DEBUG_LEVEL=0")
		e.ProcessFlags("-DNO_GLOBAL_ARDUINO_OTA=1")
		e.ProcessFlags("-DWEBSOCKETS_SAVE_RAM=1")
		
		# ESP32 version of Arduino has some changes for class names
		if e["PIOPLATFORM"].startswith("espressif32"):
			cpp_defines_dict = {}
			for item in  e["CPPDEFINES"]:
				if isinstance(item, tuple):
					key, value = item
					cpp_defines_dict[key] = value
				elif isinstance(item, str):
					cpp_defines_dict[item] = 1
			if (int(cpp_defines_dict.get('ARDUINO', None)) >= 10812):
				e.ProcessFlags("-DWiFiClientSecure=NetworkClientSecure")
				e.ProcessFlags("-DWiFiClient=NetworkClient")
				e.ProcessFlags("-DWiFiServer=NetworkServer")
				e.ProcessFlags("-DWiFiUDP=NetworkUDP")
				e.ProcessFlags("-DARDUINO_3_OR_ABOVE=1")

	ksPrintLog(Colors.Magenta, "Successfully tweaked platform settings for [" + str(len(environments)) + "] environments.")
	ksPrintLog(Colors.Green, "Extra script finished.")
except BaseException as err:
	ksPrintLog(Colors.Red, "Error while executing script. Are you on newest platformio core?")
	ksPrintLog(Colors.Yellow, str(err))
