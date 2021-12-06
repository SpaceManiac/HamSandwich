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

INSTDIR="installers"
OUTDIR="${2:-build/game/$PROJECT}"

itch_download() {  # <url> <upload_id> <filename> <hash>
	if test ! -f "$INSTDIR/$3"; then
		curl -o "$INSTDIR/$3" "$(curl -s -X POST "$1/file/$2" | jq -r .url)"
	fi
	while ! sha256sum -c <<<"$4 *$INSTDIR/$3"; do
		echo "That looks like the wrong file, please download '$3' again:"
		echo "    $1"
		echo "Press Enter when finished, or Ctrl-C to cancel."
		read -r
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
		itch_download 'https://hamumu.itch.io/dr-lunatic' 710437 lunatic_install.exe b8013176ea8050db20a2b170a5273d5287ccde4b4923affb7c610bda89326c84
		extract_nsis "$INSTDIR/lunatic_install.exe" "$OUTDIR"
		;;
	"supreme")
		itch_download 'https://hamumu.itch.io/dr-lunatic-supreme-with-cheese' 700882 supreme8_install.exe 1c105ad826be1e0697b5de8483c71ff943d04bce91fe3547b6f355e9bc1c42d4
		# 824077 all_supreme_worlds.zip
		extract_nsis "$INSTDIR/supreme8_install.exe" "$OUTDIR"
		;;
	"sleepless")
		itch_download 'https://hamumu.itch.io/sleepless-hollow' 708909 hollow_betainstall.exe 41660802318356fba53a21b4d368e191b3197030fb9e8eb833788f45c01c6f99
		extract_nsis "$INSTDIR/hollow_betainstall.exe" "$OUTDIR"
		7z x -o"$OUTDIR" "$OUTDIR/worlds/sleepiest_world.zip"
		;;
	"loonyland")
		itch_download 'https://hamumu.itch.io/loonyland-halloween-hill' 702760 loonyland_install.EXE cf3cdc555297e41f6c2da61d89815dbbc740d6fc677c83ec6c6e1acfa117de34
		extract_inno "$INSTDIR/loonyland_install.EXE" "$OUTDIR"
		rm "$OUTDIR/loonyland.exe"

		itch_download 'https://hamumu.itch.io/loonyland-halloween-hill' 782965 loonyland_editor.exe 865550d077e984ca28324aaf4291211aa4009cdad9f2b74144179c6342f2be39
		extract_nsis "$INSTDIR/loonyland_editor.exe" "$OUTDIR"
		;;
	"loonyland2")
		itch_download 'https://hamumu.itch.io/loonyland-2-winter-woods' 705653 LL2CEinstall.exe 0806e1615eb94332bf805128d2d3857af420e93ee6f48692eebf17c05e9b14e2
		# 712518 Add-on World: Challenge
		# 712516 Add-on World: Epic Adventure
		# 712519 Add-on World: Knittel's Adventure
		# 712517 Add-on World: Summer Woods
		# 712520 Add-on World: Winter Wackiness 6 by Redbone
		# 712521 Add-on World: Winter Wackiness 8 by Qwertybub
		extract_nsis "$INSTDIR/LL2CEinstall.exe" "$OUTDIR"
		;;
	"mystic")
		itch_download 'https://hamumu.itch.io/kid-mystic' 704052 mystic_install.exe c2d618176d23b974c01c00690b6afb0aaebd4c863dfff0bf8b1f66db1bdc2f65
		extract_inno "$INSTDIR/mystic_install.exe" "$OUTDIR"
		;;
	*)
		echo "$0: unknown project: $PROJECT"
		exit 1
		;;
esac
