# HamSandwich

![Screenshots of game title screens](docs/titlescreens.png)

## Building

The recommended way to compile the games is [MinGW on Windows](#windows-mingw).
The following platforms also have various levels of support:

Platform | Toolchain | Status | Notes
-|-|-|-
Windows | MinGW | Primary | CI: [Appveyor](https://ci.appveyor.com/project/SpaceManiac/hamsandwich)
Windows | MSVC | Secondary | CI: [Appveyor](https://ci.appveyor.com/project/SpaceManiac/hamsandwich)
Linux | GCC | Secondary | CI: [Appveyor](https://ci.appveyor.com/project/SpaceManiac/hamsandwich)
Windows | Clang | Tertiary | No app icons
Linux | Clang | Tertiary
Android | NDK | Tertiary | Incomplete/buggy

### Windows (MinGW)

First-time setup:

1. Install MSYS2, an environment for compiling C++ code.
    1. Visit <https://www.msys2.org/> and download and run the latest "msys2-x86_64" installer.
    2. Choose a relatively short path with no spaces (say, `C:\msys64` or `D:\tools\msys2`).
    3. After installation finishes, run `pacman -Syu` in the console that opens.
    4. When asked to do so, close the console and reopen it from the Start Menu (MSYS2 64bit > MSYS2 MinGW 32-bit).
    5. Run `pacman -Syu` again.
2. Use `cd` to select the directory where HamSandwich will go.
    1. For example, if you want the code to go in `C:\projects\HamSandwich`, run `cd C:\projects`.
3. Run `pacman -S git` to install git, a tool used for source code collaboration.
4. Run `git clone https://github.com/SpaceManiac/HamSandwich` to download the code.
5. Run `cd HamSandwich` to enter the directory.

Compiling and running:

1. Run `./run` to see a list of games in the project, and use `./run <project>` to compile and play.
    1. For example, to run Supreme with Cheese, write `./run supreme`.
    2. To compile without running, write `make` or `make <projectname>`.
    3. The first time you try to run a game, you will be prompted to download
        its installer from <https://hamumu.itch.io/>;
        follow the instructions shown.
2. Run `git pull` to fetch changes which have been published here.

### Windows (MSVC)

1. Download dependencies:
    1. Run `powershell tools/build/msvc-install-deps.ps1`.
    2. Or, download and extract them manually:
        1. `build/premake5.exe`: premake-5.0.0-alpha14-windows.zip
           from <https://github.com/premake/premake-core/releases/tag/v5.0.0-alpha14>.
        2. `build/SDL2-msvc/`: SDL2-devel-2.0.9-VC.zip from <https://www.libsdl.org/download-2.0.php>.
        3. `build/SDL2_mixer-msvc/`: SDL2_mixer-devel-2.0.4-VC.zip from <https://www.libsdl.org/projects/SDL_mixer/>.
        4. `build/SDL2_image-msvc/`: SDL2_image-devel-2.0.4-VC.zip from <https://www.libsdl.org/projects/SDL_image/>.
2. Run `build/premake5.exe vs2017` (or appropriate VS version) to generate solution.
3. Open and compile `build/HamSandwich.sln` in Visual Studio.
    1. For command-line builds, run `msbuild build/HamSandwich.sln /p:Configuration=debug /p:Platform=Win32`.
4. Use installers from <https://hamumu.itch.io/> to extract game assets to `build/game/<gamename>`.
5. Debug from within Visual Studio to launch a game.

### Linux

1. Install dependencies (handled automatically on Ubuntu, Arch):
    1. premake-5.0.0-alpha14-linux.tar.gz from <https://github.com/premake/premake-core/releases/tag/v5.0.0-alpha14>.
    2. 32-bit G++ (or Clang) toolchain.
    3. 32-bit SDL2, SDL2_mixer, and SDL2_image development packages.
    4. 7-Zip and Innoextract to extract game assets.
2. Use `make` and `./run` as described above to build and run the games.

### Android

1. Install Gradle and the Android SDK and NDK and set `ANDROID_HOME`.
2. Run `./tools/build/sdl2-source.sh`.
3. Run `premake5 android-studio`.
4. Change directory to `build/android`.
5. Run `gradle packageDebug` to compile APKs.
6. Run `gradle installDebug` to install to a connected Android device.
