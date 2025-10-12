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
		e.ProcessFlags("-Wunused-variable")           # Warn about unused local variables
		e.ProcessFlags("-Wunused-parameter")          # Warn about unused function parameters
		e.ProcessFlags("-Wunused-function")           # Warn about unused static functions
		e.ProcessFlags("-Wunused-but-set-variable")   # Warn about variables set but never read
		e.ProcessFlags("-Wsign-compare")              # Warn about signed/unsigned comparisons
		e.ProcessFlags("-Wtype-limits")               # Warn about type limit comparisons
		e.ProcessFlags("-Wmissing-field-initializers") # Warn about missing field initializers
		e.ProcessFlags("-Wuninitialized")             # Warn about uninitialized variables
		e.ProcessFlags("-Wmaybe-uninitialized")       # Warn about potentially uninitialized variables
		e.ProcessFlags("-Wformat")                    # Warn about printf/scanf format issues
		e.ProcessFlags("-Wreturn-type")               # Warn about missing return statements
		e.ProcessFlags("-Wswitch")                    # Warn about missing switch cases
		e.ProcessFlags("-Wparentheses")               # Warn about ambiguous parentheses
		e.ProcessFlags("-Wsequence-point")            # Warn about undefined behavior
		e.ProcessFlags("-Wstrict-aliasing")           # Warn about strict aliasing violations
		e.ProcessFlags("-Wextra")                     # Enable additional warnings beyond -Wall
	ks_print_log(Colors.Magenta, "Successfully tweaked platform settings for [" + str(len(environments)) + "] environments.")
	ks_print_log(Colors.Green, "Extra script finished.")
except BaseException as err:
	ks_print_log(Colors.Red, "Error while executing script. Are you on newest platformio core?")
	ks_print_log(Colors.Yellow, str(err))
