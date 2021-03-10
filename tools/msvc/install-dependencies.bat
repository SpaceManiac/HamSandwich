@cd /D "%~dp0\..\.."
@call powershell.exe -NoLogo -ExecutionPolicy Bypass -File "tools\msvc\install-dependencies.ps1" %*
