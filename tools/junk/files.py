#!/usr/bin/env python3
# Lunatic files.py
# Tad Hardesty, aka SpaceManiac, 2011
# Fills 'Lunatic.files' with all files in source/
# usage: 'python tools/files.py'

import os

with open('Lunatic.files', 'w', encoding='utf-8') as f:
    for dirpath, dirnames, fnames in os.walk('source'):
        for file in fnames:
            f.write(os.path.join(dirpath, file).replace('\\', '/') + '\n')
