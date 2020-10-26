#!/bin/bash
# distribute-web.sh - build and prepare a webroot
set -euo pipefail
shopt -s nullglob

toolset=emcc
config=release

OBJDIR="build/$toolset-$config"
WEBROOT="build/webroot"

# If there's no Emscripten SDK active, then activate a private one.
if ! command -v em++ >/dev/null 2>&1; then
	source ./tools/build/install-emsdk.sh
fi

# Build specified project, or all
if [ -d "$WEBROOT" ]; then
	rm -r "$WEBROOT"
fi
mkdir -p "$WEBROOT"
make config="$config" toolset="$toolset" -j8 "$@"

# If no project was specified, detect them
if [ $# -eq 0 ]; then
	DIRS=("$OBJDIR"/*)
	set "${DIRS[@]##*/}"
fi

if [ $# -eq 1 ]; then
	# One project: put everything in the root
	cp "$OBJDIR/$1"/*.{js,wasm,css,html} "$WEBROOT"
else
	# Many projects: put everything in subfolders
	for PROJECT in "$@"; do
		FILES=("$OBJDIR/$PROJECT"/*.{js,wasm,css,html})
		if [ ${#FILES[@]} -ne 0 ]; then
			mkdir -p "build/webroot/$PROJECT"
			cp "${FILES[@]}" "build/webroot/$PROJECT"
		fi
	done
fi
