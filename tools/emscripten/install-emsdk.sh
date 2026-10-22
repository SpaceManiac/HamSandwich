#!/bin/bash
# install-emsdk.sh - bootstrap and install a private copy of emsdk
set -euo pipefail

if test -d external/emsdk/upstream; then
	# Subsequent uses (including upgrades), be quiet
	external/emsdk/emsdk install latest >/dev/null
else
	# First use, log loudly
	echo "==== Installing Emscripten ===="
	external/emsdk/emsdk install latest
fi
# NOTE: activate is mandatory! The .emscripten file it outputs is load-bearing.
external/emsdk/emsdk activate latest >/dev/null
