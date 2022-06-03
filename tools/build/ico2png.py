#!/bin/sh
"exec" """$(dirname "$0")/../bootstrap/python""" "$0" "$@"
# -----------------------------------------------------------------------------
# ico2png.py - convert a named .ico file to .png using Pillow

import sys
from PIL import Image

_, ico_fname, png_fname = sys.argv
Image.open(ico_fname).save(png_fname, 'PNG')
