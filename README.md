# HamSandwich

## Building

### Windows

First-time setup:

1. Install MSYS2, an environment for compiling C++ code.
    1. Visit https://www.msys2.org/ and download and run the latest "msys2-x86_64" installer.
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
        its installer from [https://hamumu.itch.io/](https://hamumu.itch.io/);
        follow the instructions shown.
