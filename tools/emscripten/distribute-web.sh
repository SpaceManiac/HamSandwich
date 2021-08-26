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
	source ./tools/emscripten/install-emsdk.sh
fi

# Build specified project, or all
if test -d "$WEBROOT"; then
	rm -r "$WEBROOT"
fi
mkdir -p "$WEBROOT"
nproc="$(nproc)"
if test $# -eq 0; then
	# `make all` works fine.
	make config="${mode}_$platform" toolset="$toolset" -j"$nproc"
else
	# `make mystic loonyland` has concurrency issues due to how premake works,
	# so build each project serially.
	for project in "$@"; do
		make config="${mode}_$platform" toolset="$toolset" -j"$nproc" "$project"
	done
fi

# If no project was specified, detect them
if test $# -eq 0; then
	DIRS=("$TARGETDIR"/*)
	set "${DIRS[@]##*/}"
fi

echo "==== Preparing webroot ($WEBROOT) ===="
if test $# -eq 1; then
	# One project: put everything in the root
	mapfile -t FILES < <(find "$TARGETDIR/$1" -maxdepth 1 -type f)
	cp "${FILES[@]}" "$WEBROOT"
else
	# Many projects: put everything in subfolders
	(
		printf '['
		printf '"%s",' "$@" | sed 's/,$//'
		printf ']'
	) >build/webroot.meta.json
	./tools/emscripten/embed-metadata.py __HOMEPAGE_METADATA__ build/webroot.meta.json <assets/homepage/index.html >"$WEBROOT"/index.html
	cp source/supreme/lunatic.ico "$WEBROOT"/favicon.ico
	for PROJECT in "$@"; do
		mapfile -t FILES < <(find "$TARGETDIR/$PROJECT" -maxdepth 1 -type f -not -name '*.a')
		if test ${#FILES[@]} -ne 0; then
			mkdir -p "$WEBROOT/$PROJECT"
			cp "${FILES[@]}" "$WEBROOT/$PROJECT"
		fi
	done
fi
