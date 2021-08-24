@echo off
cd /D "%~dp0\..\.."
call powershell.exe -NoLogo -ExecutionPolicy Bypass -File "tools\msvc\install-dependencies.ps1" %*
echo ==== Generating solution ====
call build\premake5.exe vs2019
rem Make the various directories that instructions will call for
mkdir installers 2>nul
mkdir appdata 2>nul
mkdir appdata\lunatic 2>nul
mkdir appdata\supreme 2>nul
mkdir appdata\sleepless 2>nul
mkdir appdata\mystic 2>nul
mkdir appdata\loonyland 2>nul
mkdir appdata\loonyland2 2>nul
pause
