#!/bin/bash
# distribute-web.sh - build and prepare a webroot
set -euo pipefail
shopt -s nullglob

toolset=emcc
mode=release
platform=x86

TARGETDIR="build/$toolset/$mode-$platform"
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
make config="${mode}_$platform" toolset="$toolset" -j8 "$@"

# If no project was specified, detect them
if [ $# -eq 0 ]; then
	DIRS=("$TARGETDIR"/*)
	set "${DIRS[@]##*/}"
fi

echo "==== Preparing webroot ($WEBROOT) ===="
if [ $# -eq 1 ]; then
	# One project: put everything in the root
	FILES=($(find "$TARGETDIR/$1" -maxdepth 1 -type f))
	cp "${FILES[@]}" "$WEBROOT"
else
	# Many projects: put everything in subfolders
	(
		printf '['
		printf '"%s",' "$@" | sed 's/,$//'
		printf ']'
	) >build/webroot.meta.json
	./tools/build/embed-metadata.py __HOMEPAGE_METADATA__ build/webroot.meta.json <assets/homepage/index.html >"$WEBROOT"/index.html
	cp source/supreme/lunatic.ico "$WEBROOT"/favicon.ico
	for PROJECT in "$@"; do
		FILES=($(find "$TARGETDIR/$PROJECT" -maxdepth 1 -type f))
		if [ ${#FILES[@]} -ne 0 ]; then
			mkdir -p "$WEBROOT/$PROJECT"
			cp "${FILES[@]}" "$WEBROOT/$PROJECT"
		fi
	done
fi
