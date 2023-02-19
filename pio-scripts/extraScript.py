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

	environments = [env, DefaultEnvironment(), projenv]
	for lb in env.GetLibBuilders():
		 environments.append(lb.env)

	flagCounter = 0
	for e in environments:
		#e.ProcessUnFlags("-fno-rtti")
		e.ProcessUnFlags("-std=gnu++11")
		flagCounter += 1
	ksPrintLog(Colors.Magenta, "Successfully processed unflags for [" + str(flagCounter) + "] environments.")

	flagCounter = 0
	for e in environments:
		e.ProcessFlags("-std=c++17")
		e.ProcessFlags("-std=gnu++17")
		e.Append(CPPDEFINES=[("NO_GLOBAL_ARDUINOOTA", ), ("WM_NODEBUG",)])
		flagCounter += 1
	ksPrintLog(Colors.Magenta, "Successfully added flags for [" + str(flagCounter) + "] environments.")

	ksPrintLog(Colors.Green, "Extra script finished.")
except BaseException as err:
	ksPrintLog(Colors.Red, "Error while executing script. Are you on newest platformio core?")
	ksPrintLog(Colors.Yellow, str(err))
