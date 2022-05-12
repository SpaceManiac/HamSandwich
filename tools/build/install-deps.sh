#!/bin/bash
# install-deps.sh - install dependencies for the current platform
set -euo pipefail

mkdir -p "build"

# Check for supported system
if test "${MSYSTEM:-}"; then
	if test "$MSYSTEM" = "MINGW32"; then
		SYS=mingw32
	elif test "$MSYSTEM" = "MINGW64"; then
		SYS=mingw64
	else
		echo "The project cannot be built in '$MSYSTEM' mode. Return to the Start Menu" >&2
		echo "and open 'MSYS2 MinGW x86' or 'MSYS2 MinGW x64'." >&2
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
packages() {
	local cmd="$1"
	shift
	local fname="build/.packages-$SYS"
	if test "$force" || ! test -f "$fname" || test "$(cat "$fname")" != "$*"; then
		echo "==== Updating system packages ===="
		echo "$cmd" "$@"
		$cmd "$@"
		echo "$*" >"$fname"
	fi
}

deps_mingw32() {
	packages 'pacman -S --needed --noconfirm --quiet' \
		make \
		mingw-w64-i686-{cmake,ninja,gcc}
}

deps_mingw64() {
	packages 'pacman -S --needed --noconfirm --quiet' \
		make \
		mingw-w64-x86_64-{cmake,ninja,gcc}
}

deps_ubuntu() {
	sudo DEBIAN_FRONTEND=noninteractive apt-get update
	packages 'sudo DEBIAN_FRONTEND=noninteractive apt-get install --yes' \
		make g++ \
		zlib1g-dev libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev \
		python3-pip
}

deps_arch() {
	packages 'sudo pacman -S --needed --quiet' \
		wget \
		make gcc gcc-libs \
		zlib sdl2 sdl2_image sdl2_mixer \
		python-pip

	premake5_linux
}

deps_alpine() {
	# Note: non-autoinstalled dependency on `bash` and `sudo` packages.
	packages 'sudo apk add' \
		make g++ zlib-dev \
		sdl2-dev sdl2_mixer-dev sdl2_image-dev \
		python3 py3-pip jpeg-dev python3-dev

	# TODO: premake5's linux binaries are not compatible.
}

deps_macports() {
	:
}

deps_homebrew() {
	:
}

# Install dependencies for the correct system
force=
while test $# -gt 0; do
	arg=$1; shift
	case "$arg" in
		--force)
			force=1
			;;
	esac
done

deps_"$SYS"
