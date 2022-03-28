#!/bin/sh
set -eu

itch_dir="build/itch-android"
rm -rf "$itch_dir"
mkdir "$itch_dir"

apk="$itch_dir/hamsandwich-android.apk"
cp \
	build/android/android-project/outputs/apk/release/android-project-release-unsigned.apk \
	"$apk"

tools/bootstrap/butler push --if-changed "$apk" "$ITCH_GAME:android" \
	--userversion="$(git rev-parse --short=10 HEAD)"
