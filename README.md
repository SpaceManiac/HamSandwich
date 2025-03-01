# HamSandwich

![Screenshots of game title screens](docs/titlescreens.png)

**HamSandwich** is a collection of working source code for several of
[Hamumu Software](https://hamumu.com/)'s titles from 1998 through 2007.
Running HamSandwich requires assets from official versions of the games,
available for free from <https://hamumu.itch.io/>.

Saves and profiles from the retail versions of the games are fully compatible
with HamSandwich saves, except that controls will need to be reconfigured.

The original code releases on which this project is based are
[available from hamumu.com](https://hamumu.com/sourcecode/).

See also:

* [Changelog](docs/Changelog.md)
* [List of published mods](https://github.com/SpaceManiac/HamSandwich/wiki)

## Playing

<a href="https://spacemaniac.itch.io/hamsandwich">
<img alt="Available on itch.io" src="docs/available-on-itch.svg" height="100" align="right">
</a>

Builds of HamSandwich for Windows, Linux, MacOS, and Android can be downloaded
from [HamSandwich on itch.io](https://spacemaniac.itch.io/hamsandwich), either
directly or through the [itch.io app](https://itch.io/app) for auto-updates.
Web builds of the game can be played on [GitHub Pages](https://spacemaniac.github.io/HamSandwich/)
or on itch.io.

## Getting help

Support is available:

* In *#sourcecode* in the [LoonyChat Discord][loonychat].
* By email at hamsandwich@platymuus.com.
* On the GitHub issue tracker, for confirmed bugs or deficiencies.

[loonychat]: https://discord.gg/DcWFMzeSXy

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

1. When installing Visual Studio, choose the "Desktop development with C++" workload.
2. Download the code:
    1. On the Get Started screen, choose "Clone a repository".
    2. For "Repository location", enter `https://github.com/SpaceManiac/HamSandwich`.
    3. Click Clone.
3. *OR,* use Git to clone the code anywhere you like and "Open Folder" the "HamSandwich" folder.
4. After the project loads for the first time, select the game you want to run:
    1. Click the down arrow next to "Select Startup Item".
    2. Click "Show/Hide Debug Targets...".
    3. Uncheck "Select All".
    4. Check "launcher.exe (Install)" as well as "*gamename*.exe (Install)" for each game you care about, and click OK.
        * If the games are missing from this list, try using the Visual Studio Installer to install "C++ CMake tools for Windows" and reopening the project.
    5. Click the down arrow again and pick the game you want to run.
5. Click the button to compile and run the game.
6. To run in windowed mode, go to "Debug" > "Debug and Launch Settings for (game)" and add `, "args": ["window"]`.

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

### Android

1. Use [Android Studio] to open the HamSandwich folder as a project.
2. *OR*, build from the command line:
    * Install Java 11 or newer.
    * Use `./gradlew packageDebug` to build.
    * Use `./gradlew installDebug` to install to a connected Android device.
    * Use `tools/bootstrap/android-sdk adb logcat` to view logs.

[Android Studio]: https://developer.android.com/studio/

### Emscripten (web)

1. If on Windows, install and use MSYS2 according to the instructions above.
2. Use `./run os=emscripten` (and optionally a project name) to build and run the games.
3. Use `make os=emscripten` (and optionally a project name) to only build the games.

See [Modding](#modding) below for information on how to bundle worlds and other
assets, and [Publish to Web](#publish-to-web) for how to publish to GitHub Pages.

[Emscripten SDK]: https://emscripten.org/docs/getting_started/downloads.html

## Modding

Check out existing mods on the [list of published mods](https://github.com/SpaceManiac/HamSandwich/wiki).

To get started with custom worlds, graphics, music, and so on, place your
custom files inside the `appdata/<project>/` folder alongside your profiles.
When the game loads assets it checks that folder before the installer. Any
worlds you save in the editor will show up in this folder.

To include a finished world or custom assets in your mod, move them to
`assets/<project>/`, creating the folder if necessary, and use Git to commit
them.

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

### Importing another game's assets

To import another game's content, copy the relevant installer from the other
game's `source/<project>/<project>.json` into that of the game you are modding,
for example:
```jsonc
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

You can add a line like `"mountpoint": "mystic"` to an installer to access its
files by a path like `mystic/graphics/items.jsp` instead of it overriding the
original files.

When the game searches for a world or asset file, it scans in a particular order:

1. The `appdata/<project>` folder.
2. Inside any `.zip` files in the `addons/<project>` folder.
3. The `assets/<project>` folder.
4. The listed `"installers"` in reverse order.

That means that overrides should go at the end of the list and base assets
should go at the beginning.

### Creating `.bmp` images with the correct palette

Most image editors offer a facility to choose what palette (sometimes called a
colormap) to use when authoring 256-color bitmaps. The palette can be imported
from any asset from the games, or from the provided `source/lunaticpal/example.bmp`. In case you are
having trouble, you can use the included LunaticPal tool with `./run lunaticpal` to
convert images.

**However,** note that `.bmp` images shown with the "Show Pic/Movie" special
effect can use *any* palette of up to 256 colors and are not limited to the
palette used by LunaticPal. You should use an image editor such as [GIMP] to
convert such images to Indexed mode, which will give superior results compared
to LunaticPal.

LunaticPal is mainly useful for tiles, backgrounds, menus, and so on which
will appear on-screen at the same time as other graphics.

[GIMP]: https://www.gimp.org/

### Editing `.jft` fonts

HamSandwich includes a quick-and-dirty JFT tool. Use it from the command line like:

- `./run jfttool path/to/font.jft` to extract a `.txt` and a series of `.png` files from a font
- `./run jfttool path/to/font.txt` to re-pack a `.txt` and `.png`s into a font

There is also an [Aseprite extension](source/jspedit_aseprite/) for JSP and JFT support.

### Editing `.jsp` sprite packs

HamSandwich includes JspEdit, a GUI tool for editing `.jsp` files. The
interface is reasonably self-explanatory. Run it with `./run jspedit`.

There is also an [Aseprite extension](source/jspedit_aseprite/) for JSP and JFT support.

### Creating `.jsp` sprite packs from 3D models

We do not currently have a 3D prerendering pipeline. If you are a 3D modeller
and could use one, please [get in touch](#getting-help).

### Creating `.flc` ([FLIC](https://en.wikipedia.org/wiki/FLIC_(file_format))) movies

FLIC movies originated with Autodesk Animator. The movies included with Hamumu
games were created with [3D Studio Max](https://en.wikipedia.org/wiki/Autodesk_3ds_Max).
FLIC export is also provided by contemporary tools like Jasc Animation Shop, and
the modern pixel art and animation editor [Aseprite](https://www.aseprite.org/).
Note that HamSandwich fixes a bug in the engine's FLIC decoder which may cause
some exported animations to render incorrectly on the retail versions of the games.

For programmers, libraries like [Aseprite FLIC Library](https://github.com/aseprite/flic) and
[LibFLIC](https://github.com/wangds/libflic) may be of use.

## Publishing

Before publishing for the first time, fork this repository on GitHub and commit
and push your changes to your fork.

To limit publishing to only the games you care about, open
`source/CMakeLists.txt` and edit the `# Games` section according to the
comments there.

### Publish to itch.io

1. Create a game page on itch.io.
2. In Settings > [API keys], click "Generate new API key" then view and copy it.
3. In your GitHub repository's Settings > Secrets > Actions tab, add two secrets:
    1. Name: "ITCH_GAME", value: `your-itch-username/your-game-name`.
    2. Name: "BUTLER_API_KEY", value: the API key from step 2.
3. Future pushes will automatically publish builds to itch.io.

[API keys]: https://itch.io/user/settings/api-keys

#### Publish Android builds to itch.io

1. Open `android-project/build.gradle` and change the `applicationId` to
   something different according to the comments in that file.
2. Create a keystore file in a safe place:
   ```sh
   tools/bootstrap/android-sdk keytool -genkey -v -keystore ../hamsandwich.keystore -alias hamsandwich -keyalg RSA -keysize 4096 -validity 10000
   ```
   You will be prompted to set a password. You can leave the rest of the options blank.
3. Base64-encode that keystore:
   ```sh
   base64 ../hamsandwich.keystore > ../hamsandwich.keystore.txt
   ```
4. In your GitHub repository's Settings > Secrets > Actions tab, add two secrets:
    1. Name: "ANDROID_KEYSTORE_B64", value: copy and paste the contents of `hamsandwich.keystore.txt` from step 3.
    2. Name: "ANDROID_KEYSTORE_PASSWORD", value: the password you set in step 2.
5. Future pushes will automatically publish APKs to itch.io.

### Publish to GitHub Pages

1. In your GitHub repository's Settings > Pages tab,
   ensure the "Source" is set to "GitHub Actions".
2. Your site will be published at `https://YourGithubUsername.github.io/HamSandwich`
3. Future pushes will automatically update your site.

### Manual publishing

Delete the `build/install/` folder, run `make install`, then zip and upload that
folder's contents to publish a copy for your platform. For Emscripten builds,
use the `build/webroot/` folder instead.
