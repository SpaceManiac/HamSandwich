# Dr. Lunatic Supreme with Cheese
# SCons configuration script

# imports
import sys
sys.path.append('/projects/LibMuus2')
import muusbuild

# constants
no_warnings = ['write-strings', 'char-subscripts', 'unused-variable', 'unused-but-set-variable']
platform_def = 'ALLEGRO_' + ('MINGW32' if muusbuild.is_windows else 'UNIX')

# don't compile certain files
blacklist = [
    'source/monsterlist.cpp',
    'source/monsterai1.cpp',
    'source/monsterai2.cpp',
    'source/monsterai3.cpp',
    'source/monsterai4.cpp',
    'source/textitems.cpp',
    'source/textrooms.cpp',
    'source/options.cpp',
    'source/spcldialog.cpp',
]

qt = muusbuild.QtProject(source_dir='source/')

# main definitions
def program(name, debug, scanning=False):
    env = muusbuild.Environment(std='gnu++11')

    # basic setup
    env.Append(CPPDEFINES=['EXPANDO'])
    if debug:
        env.Append(CPPDEFINES=['_DEBUG', 'LOG'])
    elif muusbuild.is_windows:
        env.Append(LINKFLAGS=['-mwindows'])

    # scanning lunatic
    if scanning:
        env.Append(CPPDEFINES=['LEVELLIST'])

    #env.Append(CPPDEFINES=['stricmp=strcasecmp'])

    # general setup
    env.set_name(name)
    env.set_debug(debug)
    env.ignore_warnings(no_warnings)
    env.static_stdlib()

    # use our local libraries, not muusbuild's
    env.Append(CPPPATH=['include'])
    env.Append(LIBPATH=['include'])
    env.Append(LIBS=muusbuild.allegro4_libraries(sockets=True, logg=True))
    env.Append(CPPDEFINES=[platform_def])

    # source files
    for file in muusbuild.list_files('source/', '.cpp'):
        if not file in blacklist:
            env.source(file)

    env.resource('source/lunatic.rc')

    qt.extend(env)
    Alias(name, env.program())

# set things up
program('lunatic', False)
program('lunatic_debug', True)
#program('scanninglunatic', True, scanning=True)

qt.save('Supreme')
