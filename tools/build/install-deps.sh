#!/bin/bash
# install-deps.sh - install dependencies for the current platform
set -euo pipefail

mkdir -p "build"

# Check for supported system
if [ "${MSYSTEM:-}" ]; then
	if [ "$MSYSTEM" = "MINGW32" ]; then
		SYS=mingw32
	elif [ "$MSYSTEM" = "MINGW64" ]; then
		SYS=mingw64
	else
		echo "The project cannot be built in Cygwin mode. Return to the Start Menu"
		echo "and open 'MSYS2 MinGW 32-bit' or 'MSYS2 MinGW 64-bit'."
		exit 1
	fi
elif command -v apt-get >/dev/null 2>&1; then
	SYS=ubuntu
elif command -v pacman >/dev/null 2>&1; then
	SYS=arch
elif command -v apk >/dev/null 2>&1; then
	SYS=alpine
elif command -v port >/dev/null 2>&1; then
	SYS=macports
elif command -v brew >/dev/null 2>&1; then
	SYS=homebrew
else
	WARNFILE="build/.install-deps-warning"
	if [ ! -f "$WARNFILE" ]; then
		echo "Automatic dependency installation is not available for your platform."
		echo "Press Enter to continue anyways, or Ctrl-C to cancel."
		read -r
		touch "$WARNFILE"
	fi
	exit
fi

# Helper functions
packages() {
	local CMD="$1"
	shift
	local FNAME="build/.packages-$SYS"
	if [ ! -f "$FNAME" ] || [ "$(cat "$FNAME")" != "$*" ]; then
		echo "==== Updating system packages ===="
		echo "$CMD" "$@"
		$CMD "$@"
		echo "$*" >"$FNAME"
	fi
}

premake5_linux() {
	# Download Premake5 binary
	if [ ! -f "build/premake5" ]; then
		echo "==== Downloading premake5 binary ===="
		wget -q -O "build/premake5.tar.gz" "https://github.com/premake/premake-core/releases/download/v5.0.0-beta1/premake-5.0.0-beta1-linux.tar.gz"
		sha256sum -c <<<'074fc07be17888ef41ca07eea1c527c390a04ea4550bf7962eeb0bea0cc9d2f5 *build/premake5.tar.gz'
		tar -C "build/" -x -f 'build/premake5.tar.gz'
		rm "build/premake5.tar.gz"
	fi
}

premake5_macos() {
	# Download Premake5 binary
	if test ! -f "build/premake5"; then
		echo "==== Downloading premake5 binary ===="
		wget -q -O "build/premake5.tar.gz" "https://github.com/premake/premake-core/releases/download/v5.0.0-beta1/premake-5.0.0-beta1-macosx.tar.gz"
		sha256sum -c <<<'fa71a4f2efc0baa36cd362512a55c0fad980d2044cbce72e57f63d43908f2852 *build/premake5.tar.gz'
		tar -C "build/" -x -f 'build/premake5.tar.gz'
		rm "build/premake5.tar.gz"
	fi
}

deps_mingw_generic() {
	# Download Premake5 binary
	if [ ! -f "build/premake5" ]; then
		echo "==== Downloading premake5 binary ===="
		wget -q -O "build/premake5.zip" "https://github.com/premake/premake-core/releases/download/v5.0.0-beta1/premake-5.0.0-beta1-windows.zip"
		sha256sum -c <<<'d8f958162b202fe88a2fa1897d3c5342dd137f19d1265de8802880126a808a65 *build/premake5.zip'
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

deps_mingw32() {
	# Install system packages
	packages 'pacman -S --needed --noconfirm --quiet' \
		p7zip make \
		mingw-w64-i686-python \
		mingw-w64-i686-binutils mingw-w64-i686-gcc mingw-w64-i686-zlib \
		mingw-w64-i686-SDL2 mingw-w64-i686-SDL2_mixer mingw-w64-i686-SDL2_image

	deps_mingw_generic
}

deps_mingw64() {
	# Install system packages
	packages 'pacman -S --needed --noconfirm --quiet' \
		p7zip make \
		mingw-w64-x86_64-python \
		mingw-w64-x86_64-binutils mingw-w64-x86_64-gcc mingw-w64-x86_64-zlib \
		mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_image

	deps_mingw_generic
}

deps_ubuntu() {
	packages 'sudo DEBIAN_FRONTEND=noninteractive apt-get install --yes' \
		p7zip innoextract \
		make g++ zlib1g-dev \
		libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev \
		python3-pip python3-pil

	premake5_linux
}

deps_arch() {
	packages 'sudo pacman -S --needed --quiet' \
		p7zip wget innoextract \
		make gcc gcc-libs zlib \
		sdl2 sdl2_image sdl2_mixer \
		python-pip

	premake5_linux
}

deps_alpine() {
	# Note: non-autoinstalled dependency on `bash` and `sudo` packages.
	packages 'sudo apk add' \
		p7zip innoextract \
		make g++ zlib-dev \
		sdl2-dev sdl2_mixer-dev sdl2_image-dev \
		python3 py3-pip jpeg-dev python3-dev

	# TODO: premake5's linux binaries are not compatible.
}

deps_macports() {
	# Requires /opt/local/libexec/gnubin on PATH
	packages 'sudo port install' \
		coreutils premake5 \
		libsdl2 libsdl2_image libsdl2_mixer \
		py-Pillow
}

deps_homebrew() {
	packages 'brew install' \
		coreutils wget \
		sdl2 sdl2_image sdl2_mixer \
		pillow

	premake5_macos
}

# Install dependencies for the correct system
deps_"$SYS"
