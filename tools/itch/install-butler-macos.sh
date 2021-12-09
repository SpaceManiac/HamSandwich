#!/bin/bash
set -euo pipefail

if test ! -f "build/butler"; then
	echo "==== Downloading butler binary ===="
	wget -q -O "build/butler.zip" "https://broth.itch.ovh/butler/darwin-amd64/15.21.0/archive/default"
	sha256sum -c <<<'af8fc2e7c4d4a2e2cb9765c343a88ecafc0dccc2257ecf16f7601fcd73a148ec *build/butler.zip'
	unzip -o -d build/ build/butler.zip
	rm "build/butler.zip"
fi
echo "butler: $(build/butler --version 2>&1)"
