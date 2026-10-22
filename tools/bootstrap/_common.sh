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

# Determine value of OS variable
case "$(uname)" in
	Linux)
		OS=Linux
		ExeExt=
		;;
	Darwin)
		OS=Darwin
		ExeExt=

		# An equivalent to sha256sum is available under a different name.
		sha256sum() {
			shasum -a 256 "$@"
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

# If on Windows, add Busybox as a fallback for various items.
# In particular, GitHub Desktop has a MSYS2 with sh.exe but without coreutils.
if command -v powershell.exe >/dev/null 2>/dev/null; then
	busybox_w32() {
		"$Bootstrap/busybox.exe" "$@"
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

# download_file function
if command -v curl >/dev/null 2>/dev/null; then
	# curl implementation
	download_file() {  # <url> <outfile>
		echo "Downloading: $1" >&2
		curl -L "$1" -o "$2" -s
	}

	download_file_quiet() {  # <url> <outfile>
		curl -L "$1" -o "$2" -s
	}
elif command -v wget >/dev/null 2>/dev/null; then
	# wget implementation
	download_file() {  # <url> <outfile>
		echo "Downloading: $1" >&2
		wget "$1" -O "$2" -q
	}

	download_file_quiet() {  # <url> <outfile>
		wget "$1" -O "$2" -q
	}
else
	# lazy-erroring fallback implementation
	download_file() {
		echo "Cannot download files without either curl or wget" >&2
		exit 1
	}
	download_file_quiet() {
		echo "Cannot download files without either curl or wget" >&2
		exit 1
	}
fi

# Provide temporary file creation and cleanup
TempDir=$(mktemp -d "$Cache/tmp.XXXXXXXX")
LockedOn=
temp_cleanup() {
	rm -rf "$TempDir" "$LockedOn"
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
	unset VarName
}

get_download_info() {  # <versionvar> <os> <what>
	eval "echo \$$3_$1_$(eval "echo \$$1" | sed 's/[.-]/_/g')_$2"
}

temp_download_and_verify() {  # <versionvar> <os>
	temp_download_and_verify_File="$(temp_file)"
	download_file "$(get_download_info "$1" "$2" Url)" "$temp_download_and_verify_File"
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

# Attempt to acquire a lock while doing a task. If another process holds the
# lock, wait for it to finish and then proceed *without* doing the task.
one_at_a_time() {  # <callback>
	# Can we acquire the lock?
	my_pid=$$
	if ( set -o noclobber; echo "$my_pid" > "$Cache/lock.$1"; ) 2>/dev/null; then
		# We got the lock, so do the thing.
		LockedOn="$Cache/lock.$1"
		#echo "$$: locked $1"
		"$1"
		rm "$Cache/lock.$1"
		LockedOn=
		#echo "$$: released $1"
	else
		# We didn't get the lock, but we also want to wait until the process that did finishes.
		# Also we need to clear it if that process crashed.
		#echo "$$: waiting on $1"

		while true; do
			# If the file went away, we're good.
			locked_pid=$(cat "$Cache/lock.$1" 2>/dev/null) || break
			#echo "$$ waits on $locked_pid"
			# If the lock file now contains OUR pid, do the thing and return.
			if test "$locked_pid" = "$$"; then
				# We got the lock, so do the thing.
				LockedOn="$Cache/lock.$1"
				#echo "$$: B-locked $1"
				"$1"
				rm "$Cache/lock.$1"
				LockedOn=
				#echo "$$: B-released $1"
			fi
			# If that pid no longer exists, attempt to take the lock for ourselves.
			if ! kill -0 "$locked_pid" 2>/dev/null; then
				new_locked_pid=$(temp_file)
				echo "$$" >"$new_locked_pid"
				#echo "$$: trying to conquer"
				mv "$new_locked_pid" "$Cache/lock.$1"
			fi
			# Wait a bit.
			sleep 1
		done
		#echo "$$: wait finished on $1"
	fi
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
