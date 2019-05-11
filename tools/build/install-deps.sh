#!/bin/bash
# install-deps.sh - install dependencies for the current platform
set -euo pipefail

# Check for supported system
if [ -z "$MSYSTEM" ]; then
	WARNFILE="build/.install-deps-warning"
	if [ ! -f "$WARNFILE" ]; then
		echo "Automatic dependency installation is not available for your platform."
		echo "Press Enter to continue anyways, or Ctrl-C to cancel."
		read
		touch "$WARNFILE"
	fi
	exit
elif [ "$MSYSTEM" != "MINGW32" ]; then
	echo "The project can only be built in 32-bit mode."
	echo "Return to the Start Menu and open 'MSYS2 MinGW 32-bit'."
	exit 1
fi

# Download Allegro binaries
ALLEGRO_DIR="build/allegro"
ALLEGRO_VERSION="allegro-4.4.2-mingw-4.5.2"
ALLEGRO_ZIPNAME="$ALLEGRO_VERSION.7z"
ALLEGRO_MD5="503fc383bd34fca866372f9fade9713a"
if [ ! -d "$ALLEGRO_DIR" ]; then
	echo "==== Downloading Allegro binaries ===="
	mkdir -p "build"
	if [ ! -f "build/$ALLEGRO_ZIPNAME" ]; then
		wget -O "build/$ALLEGRO_ZIPNAME" "http://cdn.allegro.cc/file/library/allegro/4.4.2/$ALLEGRO_ZIPNAME"
		md5sum -c <<<"$ALLEGRO_MD5 *build/$ALLEGRO_ZIPNAME"
	fi
	7z x -o"build/" "build/$ALLEGRO_ZIPNAME"
	mv "build/$ALLEGRO_VERSION" "$ALLEGRO_DIR"
fi
