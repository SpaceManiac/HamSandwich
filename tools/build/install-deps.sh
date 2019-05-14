#!/bin/bash
# install-deps.sh - install dependencies for the current platform
set -euo pipefail

# Check for supported system
if [ "${MSYSTEM:-}" ]; then
    if [ "$MSYSTEM" != "MINGW32" ]; then
        echo "The project can only be built in 32-bit mode."
        echo "Return to the Start Menu and open 'MSYS2 MinGW 32-bit'."
        exit 1
    fi
    SYS=msys2
elif command -v apt-get >/dev/null 2>&1; then
    SYS=ubuntu
else
	WARNFILE="build/.install-deps-warning"
	if [ ! -f "$WARNFILE" ]; then
		echo "Automatic dependency installation is not available for your platform."
		echo "Press Enter to continue anyways, or Ctrl-C to cancel."
		read
		touch "$WARNFILE"
	fi
	exit
fi

mkdir -p "build"

# Helper functions
packages() {
    local CMD="$1"
    shift
    local PACKAGES="$@"
    if [ ! -f "build/.packages" ] || [ "$(cat build/.packages)" != "$PACKAGES" ]; then
        echo "==== Updating system packages ===="
        echo $CMD $PACKAGES
        $CMD $PACKAGES
        echo "$PACKAGES" > build/.packages
    fi
}

deps_msys2() {
    # Install system packages
    packages 'pacman -S --needed --noconfirm --quiet' \
        p7zip make \
        mingw-w64-i686-binutils mingw-w64-i686-gcc \
        mingw-w64-i686-SDL2 mingw-w64-i686-SDL2_mixer mingw-w64-i686-SDL2_image

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
}

deps_ubuntu() {
    packages 'sudo apt-get install' \
        p7zip innoextract \
        g++-multilib \
        libsdl2-dev:i386 libsdl2-mixer-dev:i386 libsdl2-image-dev:i386 \
        libmirclient-dev:i386 libmircommon-dev:i386 libxkbcommon-dev:i386

    # Download Premake5 binary
    if [ ! -f "build/premake5" ]; then
        echo "==== Downloading premake5 binary ===="
        wget -O "build/premake5.tar.gz" "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-linux.tar.gz"
        md5sum -c <<<'575decd4e194280cde2d6253b8e87e43 *build/premake5.tar.gz'
        tar -C "build/" -x -f 'build/premake5.tar.gz'
        rm "build/premake5.tar.gz"
    fi
}

# Install dependencies for the correct system
deps_$SYS
