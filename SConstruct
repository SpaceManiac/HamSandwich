import sys
import os

sys.path.append('../LibMuus2')
import muusbuild

qt = muusbuild.QtProject(source_dir='source/')

def program(name, debug):
    env = muusbuild.Environment(std='gnu++11')
    env.set_name(name)
    env.set_debug(debug)
    if not debug:
        env.Append(LINKFLAGS=['-mwindows'])
        
    env.static_stdlib()
    env.allegro('5.0.8', static=True, addons=['dialog', 'font', 'image', 'primitives', 'ttf'])
    env.libmuus()
    
    # in the latest MinGW, this doesn't seem to be defined by default
    # if it's left out, allegro includes windows includes winsock
    # but this conflicts with winsock2 in various bad ways
    env.Append(CPPDEFINES=['__MSYS__'])

    env.sources('source/')
    #env.resource('source/resources.rc')
    qt.extend(env)
    Alias(name, [env.program(), env.install('run/')])

program('jspedit-debug', True)
program('jspedit', False)
qt.save('JspEdit2')
