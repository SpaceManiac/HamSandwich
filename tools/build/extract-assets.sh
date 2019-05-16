#!/bin/bash
# extract-assets.sh - extract game/ folder from an installer file
set -euo pipefail

if [ $# -eq 0 ]; then
	echo "$0: must specify project"
	exit 1
fi
PROJECT=$1
PATH="$PWD/build:$PATH"
echo "==== Extracting assets for $PROJECT ===="

INSTDIR="build/installers"
OUTDIR="build/game/$PROJECT"

itch_download() {  # <url> <filename> <hash>
	while [ ! -f "$INSTDIR/$2" ]; do
		echo "Please download '$2' from the following page, and save it inside '$INSTDIR/':"
		echo "    $1"
		echo "Press Enter when finished, or Ctrl-C to cancel."
		read
	done
	md5sum -c <<<"$3 *$INSTDIR/$2"
	while [ $? -ne 0 ]; do
		echo "That looks like the wrong file, please download '$2' again:"
		echo "    $1"
		echo "Press Enter when finished, or Ctrl-C to cancel."
		read
		md5sum -c <<<"$3 *$INSTDIR/$2"
	done
}

extract_nsis() {  # <exe> <destination>
	mkdir -p "$2"
	7z x -o"$2" -x'!$PLUGINSDIR' -x'!*.exe' -x'!*.dll' "$1"
}

extract_inno() {
	mkdir -p "$2"
	7z x -o"build/innotemp" "$1"
	innoextract -d "build" "build/innotemp/setup.0"
	cp -r build/app/* "$2"
	rm -r "build/innotemp" "build/app"
}

mkdir -p "$INSTDIR"
case "$PROJECT" in
	"lunatic")
		itch_download 'https://hamumu.itch.io/dr-lunatic' lunatic_install.exe 0206bf9b7474243ebf0c03b5aefabab0
		extract_nsis "$INSTDIR/lunatic_install.exe" "$OUTDIR"
		;;
	"supreme")
		itch_download 'https://hamumu.itch.io/dr-lunatic-supreme-with-cheese' supreme8_install.exe 1d3b28e9303af9387869b7c79661aca5
		extract_nsis "$INSTDIR/supreme8_install.exe" "$OUTDIR"
		;;
	"sleepless")
		itch_download 'https://hamumu.itch.io/sleepless-hollow' hollow_betainstall.exe be32780401a0d9c4565c1d779c56d143
		extract_nsis "$INSTDIR/hollow_betainstall.exe" "$OUTDIR"
		;;
	"loonyland")
		itch_download 'https://hamumu.itch.io/loonyland-halloween-hill' loonyland_install.EXE 1be5fb03d3f6bc86aa1a4a5dea4f80f8
		extract_inno "$INSTDIR/loonyland_install.EXE" "$OUTDIR"
		rm "$OUTDIR/loonyland.exe"

		itch_download 'https://hamumu.itch.io/loonyland-halloween-hill' loonyland_editor.exe 39a7f048724cf941b6a7bf1848bf68fb
		extract_nsis "$INSTDIR/loonyland_editor.exe" "$OUTDIR"
		;;
	"loonyland2")
		itch_download 'https://hamumu.itch.io/loonyland-2-winter-woods' LL2CEinstall.exe a5bd3dde483787c7a637130306d99639
		extract_nsis "$INSTDIR/LL2CEinstall.exe" "$OUTDIR"
		;;
	"mystic")
		itch_download 'https://hamumu.itch.io/kid-mystic' mystic_install.exe 35cbe3271843aadb860e068cd3f1f9ca
		extract_inno "$INSTDIR/mystic_install.exe" "$OUTDIR"
		;;
	*)
		echo "$0: unknown project: $PROJECT"
		exit 1
		;;
esac
