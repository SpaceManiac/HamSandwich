# monsterlist.py
# extract some useful info from the monsterlist.cpp

import os, re

f = open("source/monsterlist.cpp")
lines = f.read().splitlines()
f.close()

monsname = "Null"
graphicsline = False

monsters = []

def graphicsOf(mons):
    t = monsters[mons][1]
    if t.startswith("!"):
        return graphicsOf(int(t[1:]))
    else:
        return t

for line in lines:
    if line.startswith("\t\t{\""):
        p = line.find("\"", 4)
        monsname = line[4:p]
        graphicsline = True
    elif graphicsline:
        p1 = line.find("\"")
        p2 = line.find("\"", p1+1)
        jsp = line[p1+1:p2].replace("graphics/", "")
        monsters.append((monsname, jsp))
        graphicsline = False

for i, tup in enumerate(monsters):
    print tup[0] + "\t" + graphicsOf(i)
