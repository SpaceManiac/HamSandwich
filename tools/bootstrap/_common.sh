# shellcheck shell=sh
set -eu

# Determine the Cache directory. $Bootstrap must be set already, because $0
# cannot be relied upon in a sourced script.
Cache="${Bootstrap:?}/.cache"
if test -n "${TG_BOOTSTRAP_CACHE:-}"; then
	Cache="$TG_BOOTSTRAP_CACHE"
fi
# Ensure that the cache exists
mkdir -p "${Cache:?}"

case "$(uname)" in
	Linux)
		OS=Linux
		ExeExt=
		;;
	Darwin)
		OS=Darwin
		ExeExt=

		# TODO: replace these temporary compatibility shim with something better.
		wget() {
			curl -L "$1" -o "$3"
		}
		sha256sum() {
			cat >/dev/null
		}
		;;
	MINGW* | MSYS* | CYGWIN* | Windows_NT)
		# shellcheck disable=SC2034
		OS=Windows
		# shellcheck disable=SC2034
		ExeExt=.exe
		;;
	*)
		echo "unrecognized OS type: $(uname)" >&2
		exit 1
		;;
esac

# Provide temporary file creation and cleanup
TempDir=$(mktemp -d "$Cache/tmp.XXXXXXXX")
temp_cleanup() {
	rm -rf "$TempDir"
}
trap temp_cleanup EXIT

temp_file() {
	mktemp "$TempDir/tmp.XXXXXXXX"
}

temp_directory() {
	mktemp -d "$TempDir/tmp.XXXXXXXX"
}

clean_up_and_exec() {
	temp_cleanup
	command exec "$@"
}
alias exec=clean_up_and_exec

# Prepare download info database
download_info() {  # <versionvar> <os> <url> [sha256]
	VarName="$1_$(eval "echo \$$1" | sed 's/[.-]/_/g')_$2"
	eval "Url_$VarName=$3"
	eval "Sha_$VarName=${4:-}"
	unset Version VarName
}

get_download_info() {  # <versionvar> <os> <what>
	eval "echo \$$3_$1_$(eval "echo \$$1" | sed 's/[.-]/_/g')_$2"
}

temp_download_and_verify() {  # <versionvar> <os>
	temp_download_and_verify_File="$(temp_file)"
	wget "$(get_download_info "$1" "$2" Url)" -O "$temp_download_and_verify_File"
	echo "$(get_download_info "$1" "$2" Sha) *$temp_download_and_verify_File" | sha256sum -c >&2
	echo "$temp_download_and_verify_File"
	unset temp_download_and_verify_File
}

download_and_verify() {  # <versionvar> <os> <destination>
	mv "$(temp_download_and_verify "$1" "$2")" "$3"
}

# Untar/unzip helpers
temp_untar() {  # <tar>
	dir=$(temp_directory)
	tar x -f "$1" -C "$dir" >&2
	echo "$dir"
	unset dir
}

temp_unzip() {  # <zip>
	dir=$(temp_directory)
	unzip "$1" -d "$dir" >&2
	echo "$dir"
	unset dir
}

# Load dependencies.sh
OldPWD="$PWD"
cd "${Bootstrap:?}/../.."
# shellcheck source=../../dependencies.sh
. ./dependencies.sh
# shellcheck disable=SC2034
DependenciesSh="$PWD/dependencies.sh"
cd "$OldPWD"
unset OldPWD

# If on Windows, add Busybox as a fallback for various items.
# In particular, GitHub Desktop has a MSYS2 with sh.exe but without coreutils.
if command -v powershell.exe >/dev/null 2>/dev/null; then
	busybox_w32() {
		powershell.exe -NoLogo -ExecutionPolicy Bypass -File "$Bootstrap/busybox_.ps1" "$@"
	}

	if ! command -v sha256sum >/dev/null 2>/dev/null; then
		sha256sum() {
			busybox_w32 sha256sum "$@"
		}
	fi

	if ! command -v wget >/dev/null 2>/dev/null; then
		wget() {
			busybox_w32 wget "$@"
		}
	fi

	if ! command -v unzip >/dev/null 2>/dev/null; then
		unzip() {
			busybox_w32 unzip "$@"
		}
	fi
fi
