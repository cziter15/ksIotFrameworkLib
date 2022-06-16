try:
	Import("projenv", "env")
	
	for e in (env, projenv):
		e.ProcessUnFlags("-fno-rtti")

except BaseException as err:
	print("\033[91m[ksIotFrameworkLib:] Error while trying unflag_nortti.py!\033[0m")
	print("\033[33m[ksIotFrameworkLib:] Are you on latest platformio dev?\033[0m")
	print(f"\033[33m[ksIotFrameworkLib:] {err}\033[0m")
	pass