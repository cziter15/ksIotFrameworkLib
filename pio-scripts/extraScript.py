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

	unflagEnvCounter = 0
	for e in (env, DefaultEnvironment(), projenv):
		e.ProcessUnFlags("-fno-rtti")
		unflagEnvCounter += 1
	ksPrintLog(Colors.Magenta, "Successfully processed unflags for [" + str(unflagEnvCounter) + "] environments.")

	ksPrintLog(Colors.Green, "Extra script finished.")
except BaseException as err:
	ksPrintLog(Colors.Red, "Error while executing fix_rtti script. Are you on newest platformio core?")
	ksPrintLog(Colors.Yellow, str(err))