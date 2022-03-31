# HamSandwich

![Screenshots of game title screens](docs/titlescreens.png)

**HamSandwich** is a collection of working source code for several of
[Hamumu Software](http://hamumu.com/)'s titles. Running HamSandwich requires
assets from official versions of the games, available for free from
<https://hamumu.itch.io/>.

Saves and profiles from the retail versions of the games are fully compatible
with HamSandwich saves, except that controls will need to be reconfigured.

The original code releases on which this project is based are
[available from hamumu.com](http://hamumu.com/sourcecode/).

See also:

* [Changelog](docs/Changelog.md)
* [List of published mods](https://github.com/SpaceManiac/HamSandwich/wiki)

## Playing

Builds of HamSandwich for Windows, Linux, MacOS, and Android can be downloaded
from [HamSandwich on itch.io](https://spacemaniac.itch.io/hamsandwich), either
directly or through the [itch.io app](https://itch.io/app) for auto-updates.
Web builds of the game can be played on [GitHub Pages](https://spacemaniac.github.io/HamSandwich/).

## Building

The recommended way to compile the games is [MinGW on Windows](#windows-mingw).
You can also build for one of the platforms listed below. Prefer to use Git to
download the source code rather than downloading a ZIP archive as doing so
makes it much easier to stay up-to-date and to publish your mods.

### Windows (MinGW)

First-time setup:

1. Install MSYS2, an environment for compiling C++ code.
    1. Visit <https://www.msys2.org/> and download and run the latest "msys2-x86_64" installer.
    2. Choose a relatively short path with no spaces (such as `C:\msys64` or `D:\tools\msys2`).
    3. After installation finishes, run `pacman -Syu` in the console that opens.
    4. When asked to do so, close the console and reopen it from the Start Menu (MSYS2 64bit > MSYS2 MinGW x86).
    5. Run `pacman -Syu` again.
2. Use `cd` to select the directory where HamSandwich will go.
    1. For example, if you want the code to go in `C:/projects/HamSandwich`, run `cd C:/projects`.
3. Run `pacman -S git` to install git, a tool used for source code collaboration.
4. Run `git clone https://github.com/SpaceManiac/HamSandwich` to download the code.
5. Run `cd HamSandwich` to enter the directory.

Compiling and running:

1. Run `./run` to compile and run the launcher, or `./run <project>` to compile and run one game.
    1. For example, to run Supreme With Cheese, write `./run supreme`.
    2. To compile without running, write `make` or `make <project>`.
    3. To debug, use `./run tool=gdb <project>`.
    4. See `./run --help` for more.
2. Run `git pull` to fetch changes which have been published here.

### Windows (Visual Studio 2019 or newer)

1. Download the code:
    1. On the Get Started screen, choose "Clone a repository".
    2. For "Repository location", enter `https://github.com/SpaceManiac/HamSandwich`.
    3. Click Clone.
2. *OR,* use Git to clone the code anywhere you like and "Open Folder".
3. After the project loads for the first time, select the game you want to run:
    1. Click the down arrow next to "Select Startup Item".
    2. Uncheck "Select All".
    3. Check "launcher.exe (Install)", "loonyland.exe (Install)", "loonyland2.exe (Install)", "lunatic.exe (Install)", "mystic.exe (Install)", "sleepless.exe (Install)" and "supreme.exe (Install)" and click OK.
    4. Click the down arrow again and pick the game you want to run.
4. Click the button to compile and run the game.
5. To run in windowed mode, go to "Debug" > "Debug and Launch Settings for (game)" and add `, "args": ["window"]`.

### Linux

1. Use `git` to clone the repository.
2. Use `make` and `./run` as described above to build and run the games.
3. If not on Ubuntu, Arch, or another supported distribution, install dependencies manually:
    * CMake, Ninja, and GCC.
    * Development libraries for SDL2, SDL2_mixer, and SDL2_image.

### MacOS

1. In Terminal, run `git clone https://github.com/SpaceManiac/HamSandwich`.
    1. If you get the prompt to install XCode: accept it, wait for it to finish, then run the command again.
3. Enter the repository with `cd HamSandwich`.
2. Use `make` and `./run` as described above to build and run the game.

### Emscripten

1. If on Windows, install and use MSYS2 according to the instructions above.
2. Use `make os=emscripten` (and optionally a project name) to build the game.
3. Use `./run os=emscripten` (and optionally a project name) to build and run the games.
   browser.

See [Modding](#modding) below for information on how to bundle worlds and other
assets, and [Publish to Web](#publish-to-web) for how to publish to GitHub Pages.

[Emscripten SDK]: https://emscripten.org/docs/getting_started/downloads.html

### Android

1. Use [Android Studio] to open the HamSandwich folder as a project.
2. *OR*, build from the command line:
    * Install Java 11 or newer.
    * Use `./gradlew packageDebug` to build.
    * Use `./gradlew installDebug` to install to a connected Android device.
    * Use `tools/bootstrap/android-sdk adb logcat` to view logs.

[Android Studio]: https://developer.android.com/studio/

## Modding

To get started with custom worlds, graphics, music, and so on, place your
custom files inside the `appdata/<project>/` folder alongside your profiles.
When the game loads assets it checks that folder before the installer. Any
worlds you save in the editor will show up in this folder.

To include a finished world or custom assets in your mod, move them to
`assets/<project>/`, creating the folder if necessary, and commit them.

If your mod's saves are not compatible with the base game, such as in the case
of a typical remix world, edit `source/<project>/CMakeLists.txt` and add a line
to the `HamSandwich_add_executable` section like:

```cmake
HamSandwich_add_executable(
    ...
    APPDATA_NAME "SpaceManiac-mystic-remix"
    ...
)
```

To import another game's content, copy the relevant installer from the other
game's `source/<project>/<project>.json` into that of the game you are modding,
for example:
```json
{
    "installers": [
        {
			"filename": "supreme8_install.exe",
            /* ... */
        },
        {
            "filename": "mystic_install.exe",
            /* ... */
        }
    ]
}
```

When the game searches for a world or asset file, it scans first the appdata
folder, then the `assets/<project>/` folder, then the `installers` in order.
That means that overrides should go at the beginning of the list and fallbacks
should go at the end.

Check out existing mods on the [list of published mods](https://github.com/SpaceManiac/HamSandwich/wiki).

## Publishing

Before publishing for the first time, fork this repository on GitHub and commit
and push your changes to your fork.

To limit publishing to only the games you care about, open
`source/CMakeLists.txt` and edit the `# Games` section according to the
comments there.

### Publish to Web

1. In your GitHub repository's Settings > Pages tab,
   change the "Source" from "None" to "gh-pages" and click "Save".
2. Your site will be published at `https://yourusername.github.io/HamSandwich`
3. Future pushes will automatically update your site.

### Publish to itch.io

1. Create a game page on itch.io.
2. In Settings > [API keys], click "Generate new API key" then view and copy it.
3. In your GitHub repository's Settings > Secrets > Actions tab, add two secrets:
    1. Name: "ITCH_GAME", value: `your-itch-username/your-game-name`.
    2. Name: "BUTLER_API_KEY", value: the token from step 2.
3. Future pushes will automatically publish builds to itch.io.

[API keys]: https://itch.io/user/settings/api-keys

### Manual publishing

Delete the `build/install/` folder, build again, then zip and upload that
folder to publish a copy for your platform. Emscripten builds go to
`build/webroot/`.
