#	Copyright (c) 2021-2022, Krzysztof Strehlau
#	
#	This file is part of the ksIotFramework library.
#	All licensing information can be found inside LICENSE.md file
#
#	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE

from logger import *

try:
	Import("projenv", "env")
	ksPrintLog(Colors.Green, "Running extra script for library.")

	ksPrintLog(Colors.Magenta, "Building library environment list.")
	environments = [env, DefaultEnvironment(), projenv]
	for lb in env.GetLibBuilders():
		environments.append(lb.env)

	for e in environments:
		# remove unused flags
		e.ProcessUnFlags("-std=gnu++11")
		# add flags to environment
		e.ProcessFlags("-std=gnu++17")
		e.ProcessFlags("-fno-exceptions")
		e.ProcessFlags("-DCORE_DEBUG_LEVEL=0")
		e.ProcessFlags("-DWEBSOCKETS_SAVE_RAM=1")

	ksPrintLog(Colors.Magenta, "Successfully manipulated flags on [" + str(len(environments)) + "] environments.")

	ksPrintLog(Colors.Green, "Extra script finished.")
except BaseException as err:
	ksPrintLog(Colors.Red, "Error while executing script. Are you on newest platformio core?")
	ksPrintLog(Colors.Yellow, str(err))
