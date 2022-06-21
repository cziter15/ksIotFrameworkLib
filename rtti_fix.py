# This script unflags -fno-rtti, because this library requires RTTI support.
try:
	Import("projenv", "env")

	print("\033[92m[ksIotFrameworkLib:] Processing nortti unflag!\033[0m")

	for e in (env, DefaultEnvironment(), projenv):
		e.ProcessUnFlags("-fno-rtti")

	print("\033[92m[ksIotFrameworkLib:] Done, unflagged for all environments!\033[0m")
except BaseException as err:
	print("\033[91m[ksIotFrameworkLib:] Error while trying rtti_fix.py!\033[0m")
	print("\033[33m[ksIotFrameworkLib:] Platformio in version v6.0.3b dev (at least) is required.\033[0m")
