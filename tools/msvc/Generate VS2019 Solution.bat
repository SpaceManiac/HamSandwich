@echo off
cd /D "%~dp0\..\.."
call powershell.exe -NoLogo -ExecutionPolicy Bypass -File "tools\msvc\install-dependencies.ps1" %*
echo ==== Generating solution ====
call build\premake5.exe vs2019
pause
