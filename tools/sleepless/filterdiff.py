# Lunatic lines.py
# Tad Hardesty, aka SpaceManiac, 2011
# Counts lines of code
# usage: 'python tools/lines.py'

from glob import glob
from sys import argv
from os import path
import os, re

f = open("tools/diff.diff")
lines = f.read().splitlines()
f.close()

diff = []
only = []
same = []

for line in lines:
    if line.startswith("diff"):
        diff.append(line)
    elif line.startswith("Only"):
        only.append(line)

print "\n".join(diff)
print "\n".join(only)
