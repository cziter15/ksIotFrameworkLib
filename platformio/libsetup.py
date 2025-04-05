# flake8: noqa
# Copyright (c) 2021-2025, Krzysztof Strehlau
# This file is part of the ksIotFramework library.
# All licensing information can be found inside LICENSE.md file
# https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE

from logger import ksPrintLog, Colors

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
		# add flags to environment
		e.ProcessFlags("-std=gnu++2a")
		e.ProcessFlags("-DNO_GLOBAL_ARDUINO_OTA=1")
		e.ProcessFlags("-DWEBSOCKETS_SAVE_RAM=1")

	ksPrintLog(Colors.Magenta, "Successfully tweaked platform settings for [" + str(len(environments)) + "] environments.")
	ksPrintLog(Colors.Green, "Extra script finished.")
except BaseException as err:
	ksPrintLog(Colors.Red, "Error while executing script. Are you on newest platformio core?")
	ksPrintLog(Colors.Yellow, str(err))