#!/bin/sh
# The Itch launcher has a bug where if installers/ contains exactly one file
# that looks runnable, it shows up as an alternative. We work around this by
# placing two dummy shell scripts to trick it into ignoring both.
