# shellcheck shell=sh

export NINJA_VERSION=1.10.2
export CMAKE_VERSION=3.21.2
export BUTLER_VERSION=15.21.0
export ANDROID_CLI_VERSION=8092744_latest

# See discussion in `tools/bootstrap/python`.
export PYTHON_VERSION=3.8
export PYTHON_WINDOWS_PATCH=10
export PYTHON_LINUX_PATCH=13
export PYTHON_MANYLINUX=manylinux2010

# After editing the above, run `tools/bootstrap/update-checksums` to automatically update the following section.
if command -v download_info >/dev/null 2>/dev/null; then
	PYTHON_MAJOR=${PYTHON_VERSION%%.*}
	PYTHON_MINOR=${PYTHON_VERSION#*.}

	download_info NINJA_VERSION Windows "https://github.com/ninja-build/ninja/releases/download/v$NINJA_VERSION/ninja-win.zip" bbde850d247d2737c5764c927d1071cbb1f1957dcabda4a130fa8547c12c695f
	download_info NINJA_VERSION Linux "https://github.com/ninja-build/ninja/releases/download/v$NINJA_VERSION/ninja-linux.zip" 763464859c7ef2ea3a0a10f4df40d2025d3bb9438fcb1228404640410c0ec22d
	download_info NINJA_VERSION Darwin "https://github.com/ninja-build/ninja/releases/download/v$NINJA_VERSION/ninja-mac.zip" 6fa359f491fac7e5185273c6421a000eea6a2f0febf0ac03ac900bd4d80ed2a5
	download_info CMAKE_VERSION Windows "https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-windows-x86_64.zip" 213a4e6485b711cb0a48cbd97b10dfe161a46bfe37b8f3205f47e99ffec434d2
	download_info CMAKE_VERSION Linux "https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-linux-x86_64.tar.gz" d5517d949eaa8f10a149ca250e811e1473ee3f6f10935f1f69596a1e184eafc1
	download_info CMAKE_VERSION Darwin "https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-macos10.10-universal.tar.gz" 85553c3b79d44cea501e2266235be7d8dcd57f21140217efc41175014c32b098
	download_info PYTHON_VERSION Windows "https://www.python.org/ftp/python/$PYTHON_VERSION.$PYTHON_WINDOWS_PATCH/python-$PYTHON_VERSION.$PYTHON_WINDOWS_PATCH-embed-amd64.zip" abbe314e9b41603dde0a823b76f5bbbe17b3de3e5ac4ef06b759da5466711271
	download_info PYTHON_VERSION Linux "https://github.com/niess/python-appimage/releases/download/python$PYTHON_VERSION/python$PYTHON_VERSION.$PYTHON_LINUX_PATCH-cp$PYTHON_MAJOR$PYTHON_MINOR-cp$PYTHON_MAJOR$PYTHON_MINOR-${PYTHON_MANYLINUX}_x86_64.AppImage" 323bff40b7222b8b43b118fa76d3ca0ffd77e7101f288dfa7bdec3cd7a56e3f3
	download_info BUTLER_VERSION Windows "https://broth.itch.ovh/butler/windows-amd64/$BUTLER_VERSION/archive/default" e5381a1ec38abe8c2f3bf742dc22e897901e7def22d20ebc8fd4490d52038d35
	download_info BUTLER_VERSION Linux "https://broth.itch.ovh/butler/linux-amd64/$BUTLER_VERSION/archive/default" b640fceade5cafc7601b0ee57ba13c85913917eb5440375f7a605126f9ca2e1c
	download_info BUTLER_VERSION Darwin "https://broth.itch.ovh/butler/darwin-amd64/$BUTLER_VERSION/archive/default" af8fc2e7c4d4a2e2cb9765c343a88ecafc0dccc2257ecf16f7601fcd73a148ec
	download_info ANDROID_CLI_VERSION Windows "https://dl.google.com/android/repository/commandlinetools-win-$ANDROID_CLI_VERSION.zip" 5de99ed67cb2e30fe443baf8b282d1b0b6247d0c25c6d888a7e8657b3b35c281
	download_info ANDROID_CLI_VERSION Linux "https://dl.google.com/android/repository/commandlinetools-linux-$ANDROID_CLI_VERSION.zip" d71f75333d79c9c6ef5c39d3456c6c58c613de30e6a751ea0dbd433e8f8b9cbf
	download_info ANDROID_CLI_VERSION Darwin "https://dl.google.com/android/repository/commandlinetools-mac-$ANDROID_CLI_VERSION.zip" 1de25523d595198d29666f9976eed65d99bbc5e4a3e8e48e5d6c98bb7e9030cc
fi
