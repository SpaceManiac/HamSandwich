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

mkdir -p "build"

# Install system packages
PACKAGES="p7zip make mingw-w64-i686-binutils mingw-w64-i686-gcc mingw-w64-i686-SDL2 mingw-w64-i686-SDL2_mixer mingw-w64-i686-SDL2_image"
if [ ! -f "build/.packages" ] || [ "$(cat build/.packages)" != "$PACKAGES" ]; then
	echo "==== Updating MSYS2 packages ===="
	pacman -S --needed --noconfirm --quiet $PACKAGES
	echo "$PACKAGES" > build/.packages
fi

# Download Premake5 binary
if [ ! -f "build/premake5" ]; then
	echo "==== Downloading premake5 binary ===="
	wget -O "build/premake5.zip" "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-windows.zip"
	md5sum -c <<<'93865b1bd6e0719f37b3acb07d60572b *build/premake5.zip'
	7z x -o"build/" "build/premake5.zip"
	rm "build/premake5.zip"
fi

# Download Innoextract binary
if [ ! -f "build/innoextract" ]; then
	echo "==== Downloading Innoextract binary ===="
	wget -O "build/innoextract.zip" "https://constexpr.org/innoextract/files/innoextract-1.7-windows.zip"
	md5sum -c <<<'b801b0740b4ab19d69a739ab4a9180ae *build/innoextract.zip'
	7z x -o"build/" -i'!innoextract.exe' "build/innoextract.zip"
	rm "build/innoextract.zip"
fi
