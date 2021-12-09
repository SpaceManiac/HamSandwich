#!/bin/bash
set -euo pipefail

if test ! -f "build/butler"; then
	echo "==== Downloading butler binary ===="
	wget -q -O "build/butler.zip" "https://broth.itch.ovh/butler/linux-amd64/15.21.0/archive/default"
	sha256sum -c <<<'b640fceade5cafc7601b0ee57ba13c85913917eb5440375f7a605126f9ca2e1c *build/butler.zip'
	unzip -o -d build/ build/butler.zip
	rm "build/butler.zip"
fi
echo "butler: $(build/butler --version 2>&1)"
