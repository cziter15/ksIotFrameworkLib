# flake8: noqa
# Copyright (c) 2020-2025, Krzysztof Strehlau
# This file is part of the ksIotFrameworkLib IoT library.
# All licensing information can be found inside LICENSE.md file
# https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
from logger import ks_print_log, Colors
ks_print_log(Colors.Green, "Running extra script for library.")
try:
	from SCons.Script import Import, DefaultEnvironment
	Import("projenv", "env")
	ks_print_log(Colors.Magenta, "Building library environment list.")
	environments = [env, DefaultEnvironment(), projenv]
	for lb in env.GetLibBuilders():
		environments.append(lb.env)
	for e in environments:
		e.ProcessUnFlags("-std=gnu++11")
		e.ProcessFlags("-std=gnu++2a")
		e.ProcessFlags("-DNO_GLOBAL_ARDUINO_OTA=1")
		e.ProcessFlags("-DWEBSOCKETS_SAVE_RAM=1")
	ks_print_log(Colors.Magenta, "Successfully tweaked platform settings for [" + str(len(environments)) + "] environments.")
	ks_print_log(Colors.Green, "Extra script finished.")
except BaseException as err:
	ks_print_log(Colors.Red, "Error while executing script. Are you on newest platformio core?")
	ks_print_log(Colors.Yellow, str(err))
