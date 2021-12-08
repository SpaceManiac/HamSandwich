# Setup
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
Add-Type -AssemblyName System.IO.Compression.FileSystem

# Fetch Butler binary
if (-not (Test-Path "build/butler.exe")) {
	New-Item build -ItemType Directory -ErrorAction SilentlyContinue > $null

	Write-Output "==== Downloading Butler ===="
	Invoke-WebRequest `
		-Uri "https://broth.itch.ovh/butler/windows-amd64/LATEST/archive/default" `
		-OutFile "build/butler.zip" `
		-ErrorAction Stop

	[System.IO.Compression.ZipFile]::ExtractToDirectory("$PWD/build/butler.zip", "$PWD/build")
	rm build/butler.zip -ErrorAction Stop
}

$ButlerVersion = build/butler.exe --version 2>&1
Write-Output "butler: $ButlerVersion"
