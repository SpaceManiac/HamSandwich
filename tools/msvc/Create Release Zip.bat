@echo off
cd /D "%~dp0\..\.."
call powershell.exe -NoLogo -ExecutionPolicy Bypass -File "tools\msvc\build-binary-package.ps1" -Zip 1 %*
pause
