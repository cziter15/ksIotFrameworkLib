# Unflag RTTI from global env.
global_env = DefaultEnvironment()
global_env['CXXFLAGS'].remove("-fno-rtti")
global_env['CXXFLAGS'].append("-frtti")

# Unflag RTTI from this env.
Import('env')
env['CXXFLAGS'].remove("-fno-rtti")
env['CXXFLAGS'].append("-frtti")
