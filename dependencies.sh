# shellcheck shell=sh

export NINJA_VERSION=1.10.2
export CMAKE_VERSION=3.21.2
export BUTLER_VERSION=15.24.0
export ANDROID_CLI_VERSION=8092744_latest

# After editing the above, run `tools/bootstrap/update-checksums` to automatically update the following section.
if command -v download_info >/dev/null 2>/dev/null; then
	download_info NINJA_VERSION Windows "https://github.com/ninja-build/ninja/releases/download/v$NINJA_VERSION/ninja-win.zip" bbde850d247d2737c5764c927d1071cbb1f1957dcabda4a130fa8547c12c695f
	download_info NINJA_VERSION Linux "https://github.com/ninja-build/ninja/releases/download/v$NINJA_VERSION/ninja-linux.zip" 763464859c7ef2ea3a0a10f4df40d2025d3bb9438fcb1228404640410c0ec22d
	download_info NINJA_VERSION Darwin "https://github.com/ninja-build/ninja/releases/download/v$NINJA_VERSION/ninja-mac.zip" 6fa359f491fac7e5185273c6421a000eea6a2f0febf0ac03ac900bd4d80ed2a5
	download_info CMAKE_VERSION Windows "https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-windows-x86_64.zip" 213a4e6485b711cb0a48cbd97b10dfe161a46bfe37b8f3205f47e99ffec434d2
	download_info CMAKE_VERSION Linux "https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-linux-x86_64.tar.gz" d5517d949eaa8f10a149ca250e811e1473ee3f6f10935f1f69596a1e184eafc1
	download_info CMAKE_VERSION Darwin "https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-macos10.10-universal.tar.gz" 85553c3b79d44cea501e2266235be7d8dcd57f21140217efc41175014c32b098
	download_info BUTLER_VERSION Windows "https://broth.itch.zone/butler/windows-amd64/$BUTLER_VERSION/archive/default" 03ddb047b62bf40319897ef539853d2f08ea2636002eed0e245c4a5668268240
	download_info BUTLER_VERSION Linux "https://broth.itch.zone/butler/linux-amd64/$BUTLER_VERSION/archive/default" bee1d708b5ed3dc7efcda3b5416ad5ca87a04d7e5fb6ebada510f3ba0cba3b69
	download_info BUTLER_VERSION Darwin "https://broth.itch.zone/butler/darwin-amd64/$BUTLER_VERSION/archive/default" 94c176f408e8a785f72da4daa35c15212b033b4c29c742bac5420e3e3aa9face
	download_info ANDROID_CLI_VERSION Windows "https://dl.google.com/android/repository/commandlinetools-win-$ANDROID_CLI_VERSION.zip" 5de99ed67cb2e30fe443baf8b282d1b0b6247d0c25c6d888a7e8657b3b35c281
	download_info ANDROID_CLI_VERSION Linux "https://dl.google.com/android/repository/commandlinetools-linux-$ANDROID_CLI_VERSION.zip" d71f75333d79c9c6ef5c39d3456c6c58c613de30e6a751ea0dbd433e8f8b9cbf
	download_info ANDROID_CLI_VERSION Darwin "https://dl.google.com/android/repository/commandlinetools-mac-$ANDROID_CLI_VERSION.zip" 1de25523d595198d29666f9976eed65d99bbc5e4a3e8e48e5d6c98bb7e9030cc
fi
