#!/bin/bash
# sdl2-source.sh - download SDL2 sources
set -euo pipefail

sdl2_lib() {  # <target> <zipfolder> <url> <sha256>
	if [ ! -d "build/$1" ]; then
		echo "==== Downloading $1 sources ===="
		wget -q -O "build/$1.zip" "$3"
		sha256sum -c <<<"$4 *build/$1.zip"
		7z x -o"build/" "build/$1.zip"
		mv "build/$2" "build/$1"
		rm "build/$1.zip"
	fi
}

mkdir -p build
sdl2_lib SDL2 SDL2-2.0.8 https://www.libsdl.org/release/SDL2-2.0.8.zip e6a7c71154c3001e318ba7ed4b98582de72ff970aca05abc9f45f7cbdc9088cb
sdl2_lib SDL2_mixer SDL2_mixer-2.0.4 https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.zip 9affb8c7bf6fbffda0f6906bfb99c0ea50dca9b188ba9e15be90042dc03c5ded
sdl2_lib SDL2_image SDL2_image-2.0.4 https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.4.zip c95229485d6a0bdeccea63ce7580485ce1bfac723ff8a8545837d2d5a3b20446
