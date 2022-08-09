#	Copyright (c) 2021-2022, Krzysztof Strehlau
#	
#	This file is part of the ksIotFramework library.
#	All licensing information can be found inside LICENSE.md file
#
#	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE

from logger import *

try:
	Import("projenv", "env")

	ksPrintLog(Colors.Green, "Processing nortti unflag!")

	for e in (env, DefaultEnvironment(), projenv):
		e.ProcessUnFlags("-fno-rtti")

	ksPrintLog(Colors.Green, "Done, unflagged for all environments.")
except BaseException as err:
	ksPrintLog(Colors.Red, "Error while executing fix_rtti script. Are you on newest platformio core?")
	ksPrintLog(Colors.Yellow, str(err))