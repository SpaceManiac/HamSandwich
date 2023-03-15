#!/bin/bash
# install-deps.sh - install dependencies for the current platform
set -euo pipefail

if test "${HSW_NO_INSTALL_DEPS:-}"; then
	exit
fi

mkdir -p "build"

# Check for supported system
if test "${MSYSTEM:-}"; then
	if test "$MSYSTEM" = "MINGW32"; then
		SYS=mingw32
	elif test "$MSYSTEM" = "MINGW64"; then
		SYS=mingw64
	else
		echo "The project cannot be built in '$MSYSTEM' mode. Return to the Start Menu and open 'MSYS2 MinGW x86' or 'MSYS2 MinGW x64'." >&2
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
	if ! test -f "$WARNFILE"; then
		echo "Automatic dependency installation is not available for your platform."
		echo "Press Enter to continue anyways, or Ctrl-C to cancel."
		read -r
		touch "$WARNFILE"
	fi
	exit
fi

# Helper functions
show_banner() {
	echo "==== Installing missing dependencies ===="
}

echo_and() {
	echo + "$@"
	"$@"
}

pacman_needed() {
	env LC_ALL=C pacman -Q "$@" 2>&1 \
	| sed -nr "s/^error: package '(.+)' was not found\$/\1/p" \
	|| true
}

# Systems
deps_mingw32() {
	local wanted=(
		make
		mingw-w64-i686-cmake
		mingw-w64-i686-ninja
		mingw-w64-i686-gcc
	)
	local needed
	needed=$(pacman_needed "${wanted[@]}")
	if test "$needed"; then
		show_banner
		# shellcheck disable=SC2086
		echo_and pacman -S --noconfirm $needed
	fi
}

deps_mingw64() {
	local wanted=(
		make
		mingw-w64-x86_64-cmake
		mingw-w64-x86_64-ninja
		mingw-w64-x86_64-gcc
	)
	local needed
	needed=$(pacman_needed "${wanted[@]}")
	if test "$needed"; then
		show_banner
		# shellcheck disable=SC2086
		echo_and pacman -S --noconfirm $needed
	fi
}

deps_ubuntu() {
	local wanted=(
		unzip
		make
		g++
		patchelf
		zlib1g-dev
		libsdl2-dev
		libsdl2-mixer-dev
		libsdl2-image-dev
		libsdl2-ttf-dev
	)
	local needed
	needed=$(
		env LC_ALL=C dpkg-query -W "${wanted[@]}" \
		2>&1 | sed -nr 's/^dpkg-query: no packages found matching (.+)$/\1/p' || true
	)
	if test "$needed"; then
		show_banner
		echo "+ sudo apt-get update && sudo apt-get install" "${needed[@]}"
		sudo DEBIAN_FRONTEND=noninteractive apt-get update
		sudo DEBIAN_FRONTEND=noninteractive apt-get install "${needed[@]}"
	fi
}

deps_arch() {
	local wanted=(
		unzip
		make
		gcc
		gcc-libs
		patchelf
		zlib
		sdl2
		sdl2_image
		sdl2_mixer
		sdl2_ttf
	)
	local needed
	needed=$(pacman_needed "${wanted[@]}")
	if test "$needed"; then
		show_banner
		# shellcheck disable=SC2086
		echo_and sudo pacman -S $needed
	fi
}

deps_alpine() {
	# Note: non-autoinstalled dependency on `bash` and `sudo` packages.
	wanted=(
		make
		g++
		patchelf
		zlib-dev
		sdl2-dev
		sdl2_mixer-dev
		libvorbis-dev
		sdl2_image-dev
		libpng-dev
		sdl2_ttf-dev
		openssl-dev
	)
	if ! apk info -e "${wanted[@]}" >/dev/null; then
		show_banner
		echo_and sudo apk add -t hamsandwich-deps "${wanted[@]}"
	fi
	# Use `sudo apk del hamsandwich-deps` to revert.
}

deps_macports() {
	:
}

deps_homebrew() {
	:
}

# Install dependencies for the correct system
deps_"$SYS"
