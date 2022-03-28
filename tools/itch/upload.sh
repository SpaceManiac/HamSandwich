#!/bin/sh
set -eu

preset=$1
os=${preset%%-*}

itch_dir="build/itch-$preset"
rm -rf "$itch_dir"
tools/bootstrap/cmake --install "build/cmake-$preset" --prefix "$itch_dir"
cp "tools/itch/$os.itch.toml" "$itch_dir/.itch.toml"
mkdir -p "$itch_dir/installers"
cp "tools/itch/distraction.sh" "$itch_dir/installers/itch-bug-workaround-1.sh"
cp "tools/itch/distraction.sh" "$itch_dir/installers/itch-bug-workaround-2.sh"
tools/bootstrap/butler validate "$itch_dir"
tools/bootstrap/butler push --if-changed "$itch_dir" "$ITCH_GAME:$os" \
	--userversion="$(git rev-parse --short=10 HEAD)"
