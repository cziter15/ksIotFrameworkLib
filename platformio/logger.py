# flake8: noqa
# Copyright (c) 2021-2025, Krzysztof Strehlau
# This file is part of the ksIotFramework library.
# All licensing information can be found inside LICENSE.md file
# https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE

class Colors:
	End 				= "\033[0m"

	Bold				= "\033[1m"
	Dim					= "\033[2m"
	Underlined			= "\033[4m"
	Blink				= "\033[5m"
	Reverse				= "\033[7m"
	Hidden				= "\033[8m"

	ResetBold			= "\033[21m"
	ResetDim			= "\033[22m"
	ResetUnderlined 	= "\033[24m"
	ResetBlink			= "\033[25m"
	ResetReverse		= "\033[27m"
	ResetHidden			= "\033[28m"

	Default				= "\033[39m"
	Black				= "\033[30m"
	Red					= "\033[31m"
	Green				= "\033[32m"
	Yellow				= "\033[33m"
	Blue				= "\033[34m"
	Magenta				= "\033[35m"
	Cyan				= "\033[36m"
	LightGray			= "\033[37m"
	DarkGray			= "\033[90m"
	LightRed			= "\033[91m"
	LightGreen			= "\033[92m"
	LightYellow			= "\033[93m"
	LightBlue			= "\033[94m"
	LightMagenta		= "\033[95m"
	LightCyan			= "\033[96m"
	White				= "\033[97m"

def ksPrintLog(color, text):
	print(Colors.Cyan + "[ksIotFrameworkLib] " + color + text + Colors.End)