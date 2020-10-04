#!/bin/bash
# hollowify.sh - convert a supreme/ commit to a sleepless/ commit and apply it
set -euo pipefail

fname="$(git format-patch -1 "$@")"
sed s/supreme/sleepless/g "$fname" >"$fname.2"
git am -3 "$fname.2" || true
rm "$fname" "$fname.2"
