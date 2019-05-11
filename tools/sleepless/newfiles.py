# newfiles.py
# compares game/clean/ and game/ folders

from glob import glob
from sys import argv
from os import path
import os, re

def getFileList(dir, prefix=None):
    if not prefix:
        prefix = dir
    result = []
    for file in map(lambda(x): x.replace('\\', '/'), glob(dir + '/*')):
        if path.isdir(file):
            result += getFileList(file, prefix)
        else:
            result += [file.replace(prefix,"")]
    return result

old = getFileList("game/clean/")
new = [f for f in getFileList("game/") if not f.startswith("clean/")]

for f in new:
    if not f in old and not f.startswith("profiles/"):
        print f
