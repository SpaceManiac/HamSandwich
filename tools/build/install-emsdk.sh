#!/bin/bash
# install-emsdk.sh - bootstrap and install a private copy of emsdk
set -euo pipefail

TAG=2.0.7

mkdir -p "build"

FIRST_TIME=
if [ ! -d build/emsdk ]; then
	FIRST_TIME=1
	echo "==== Bootstrapping Emscripten SDK ===="
	git clone -n https://github.com/emscripten-core/emsdk build/emsdk
fi
pushd build/emsdk >/dev/null

if [ -n "$FIRST_TIME" ] || [ "$(git log -n1 --pretty=%H)" != "$(git log -n1 --pretty=%H $TAG)" ]; then
	echo "==== Installing emsdk $TAG ===="
	git fetch -q
	git checkout -q $TAG
	./emsdk install latest-upstream
	./emsdk activate latest-upstream >/dev/null
fi

source ./emsdk_env.sh 2>/dev/null
popd >/dev/null
