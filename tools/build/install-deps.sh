#!/bin/bash
# install-deps.sh - install dependencies for the current platform
set -euo pipefail

mkdir -p "build"

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
elif command -v pacman >/dev/null 2>&1; then
	SYS=arch
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

premake5_linux() {
	# Download Premake5 binary
	if [ ! -f "build/premake5" ]; then
		echo "==== Downloading premake5 binary ===="
		wget -q -O "build/premake5.tar.gz" "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-linux.tar.gz"
		sha256sum -c <<<'5f3fe8731b52270a5222e698ee1144e2474cf17c02fcadc8727796b918b9d2de *build/premake5.tar.gz'
		tar -C "build/" -x -f 'build/premake5.tar.gz'
		rm "build/premake5.tar.gz"
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
		wget -q -O "build/premake5.zip" "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-windows.zip"
		sha256sum -c <<<'070419eedbb4c8737664d05f0e3701055741014c1135a9214c35d6feb03724a7 *build/premake5.zip'
		7z x -o"build/" "build/premake5.zip"
		rm "build/premake5.zip"
	fi

	# Download Innoextract binary
	if [ ! -f "build/innoextract" ]; then
		echo "==== Downloading Innoextract binary ===="
		wget -q -O "build/innoextract.zip" "https://constexpr.org/innoextract/files/innoextract-1.7-windows.zip"
		sha256sum -c <<<'9a0ede947448132c9a8fa390ae92da8fb25a5dffc666306f777e611b60b82fbd *build/innoextract.zip'
		7z x -o"build/" -i'!innoextract.exe' "build/innoextract.zip"
		rm "build/innoextract.zip"
	fi
}

deps_ubuntu() {
	packages 'sudo DEBIAN_FRONTEND=noninteractive apt-get install --yes' \
		p7zip innoextract \
		g++-multilib \
		libsdl2-dev:i386 libsdl2-mixer-dev:i386 libsdl2-image-dev:i386 \
		python3-pip python3-pil

	premake5_linux
}

deps_arch() {
	packages 'sudo pacman -S --needed --quiet' \
		p7zip wget innoextract \
		make gcc lib32-gcc-libs \
		lib32-sdl2 lib32-sdl2_image lib32-sdl2_mixer \
		python-pip

	premake5_linux
}

# Install dependencies for the correct system
deps_$SYS
