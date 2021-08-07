#!/bin/bash
# migrate-appdata.sh $project $from $to - copy $from to $to while stripping files that are just as they are in the assets for $project
set -euo pipefail

project=$1
from=$2
to=$3

if test -d "$to"; then
	echo "==== Warning: Appdata for $project ===="
	echo "Both $from/ and $to/ exist; using $to/"
	exit
fi

temp_assets="build/temp-assets/$project"
if test -d "$temp_assets"; then
	rm -r "$temp_assets"
fi
./tools/build/extract-assets.sh "$project" "$temp_assets"

echo "==== Migrating appdata for $project ===="

# Use diff to recursively compare $temp_assets and $from.
# Use Python to parse diff's output to get only the changed filenames.
# Copy only those files into $to.
# shellcheck disable=SC2016
(diff -qr "$temp_assets" "$from" || true) | python3 -c '
import sys, re
_, temp_assets, source = sys.argv
first = re.compile(f"^Only in {re.escape(source)}(.*): (.*)$")
second = re.compile(f"^Files {re.escape(temp_assets)}(.*) and {re.escape(source)}(.*) differ$")
for line in sys.stdin:
	line = line.strip()

	first_match = first.match(line)
	if first_match:
		dirname, fname = first_match.groups()
		print(f"{dirname}/{fname}")

	second_match = second.match(line)
	if second_match:
		fname1, fname2 = second_match.groups()
		assert fname1 == fname2
		print(fname1)
' "$temp_assets" "$from" | while read -r fname; do
	# fname starts with a slash
	echo "$fname"
	mkdir -p "$(dirname "$to.tmp$fname")"
	cp -r "$from$fname" "$to.tmp$fname"
done

# Delete temp assets, move temp output to final output, then move input out of the way
rm -r "$temp_assets"
mv "$to.tmp" "$to"
mv "$from" "$from.backup"
