# build-binary-package.ps1
# Arguments / configuration
param([string] $configuration = "release", [bool] $zip = $false)

$projects = $args

$platform = "x86"
$vs_platform = "Win32"
$sdl_platform = "x86"

# Script
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.Text.Encoding
Add-Type -AssemblyName System.IO.Compression.FileSystem

# Install dependencies (premake, SDL) and load the $ExeProjects metadata
powershell -NoLogo -ExecutionPolicy Bypass -File  ./tools/msvc/install-dependencies.ps1
Write-Output "==== Collecting metadata ===="
./build/premake5.exe binary-package-info
. ./build/binary-package-info.ps1

if ($projects.Length -eq 0) {
	$projects = "mystic", "supreme", "sleepless", "loonyland", "loonyland2"
}

# Invoke vcvars32.bat to get msbuild access
cmd.exe /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" >NUL && set' `
| ForEach-Object {
	if ($_ -match "^(.*?)=(.*)$") {
		Set-Content "env:\$($matches[1])" $matches[2]
	}
}

# Install dependencies, generate the solution, and build it
Write-Output "==== Compiling ===="
./build/premake5.exe vs2019

$logger_arg = @()
if (Test-Path "C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll") {
	# Use the Appveyor logger if available.
	$logger_arg = @('/logger:C:\Program Files\AppVeyor\BuildAgent\Appveyor.MSBuildLogger.dll')
}
foreach ($project in $projects) {
	msbuild ./build/msc-v142/$project.vcxproj /p:Configuration=$configuration /p:Platform=$vs_platform $logger_arg
}

# Create build/pkgroot
Write-Output "==== Preparing package root ===="
$pkgroot = "$PWD/build/pkgroot"
New-Item build -ItemType Directory -ErrorAction SilentlyContinue > $null
Remove-Item -Recurse $pkgroot -ErrorAction SilentlyContinue
New-Item $pkgroot -ItemType Directory > $null

# Include the project EXEs
foreach ($project in $projects) {
	Copy-Item ./build/msc-v142/$configuration-$platform/$project/$project.exe $pkgroot
}
# Include SDL2 DLLs
Copy-Item -Destination $pkgroot -Path (
	"./build/SDL2-msvc/lib/$sdl_platform/SDL2.dll",
	"./build/SDL2_image-msvc/lib/$sdl_platform/SDL2_image.dll",
	"./build/SDL2_mixer-msvc/lib/$sdl_platform/SDL2_mixer.dll",
	"./build/SDL2_mixer-msvc/lib/$sdl_platform/libogg-0.dll",
	"./build/SDL2_mixer-msvc/lib/$sdl_platform/libvorbis-0.dll",
	"./build/SDL2_mixer-msvc/lib/$sdl_platform/libvorbisfile-3.dll",
	"./build/SDL2_mixer-msvc/lib/$sdl_platform/LICENSE.ogg-vorbis.txt"
)
# Include .itch.toml
Copy-Item -Destination $pkgroot/.itch.toml -Path ./tools/itch/windows.itch.toml

# Collate installers and build installers/download-helper.ps1
$download_helper = '
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
Add-Type -AssemblyName System.IO.Compression.FileSystem

$ProgressPreference = "SilentlyContinue"
$sha256 = New-Object System.Security.Cryptography.SHA256CryptoServiceProvider

function CheckHash {
	param([string] $File, [string] $Sha)
	$got = $sha256.ComputeHash([System.IO.File]::ReadAllBytes("$PWD/$FILE"))
	$got = [System.BitConverter]::ToString($got).Replace("-", "").ToLower()
	if ($got -ne $Sha) {
		Write-Output "${File}: FAILED"
		exit 1
	} else {
		Write-Output "${File}: OK"
	}
}

function DoIt {
	param([string] $filename, [string] $link, [string] $sha256sum, [string] $file_id)
	if (-not (Test-Path "installers/$filename")) {
		Invoke-WebRequest `
			-ErrorAction Stop `
			-OutFile "installers/$filename" `
			-Uri (Invoke-WebRequest -Method POST -Uri "$link/file/$file_id" -UseBasicParsing -ErrorAction Stop | ConvertFrom-Json).url
	}
	CheckHash "installers/$filename" $sha256sum
}

switch ($args[0]) {
'
foreach ($project in $projects) {
	$info = $ExeProjects["$project|${configuration}_$platform"]
	$download_helper += "`t$project {`n"
	foreach ($installer in $info["installers"]) {
		$download_helper += "`t`tDoIt -filename " + $installer["filename"] + " -link " + $installer["link"] + " -file_id " + $installer["file_id"] + " -sha256sum " + $installer["sha256sum"] + "`n"
	}
	$download_helper += "`t}`n"

	# Copy assets while we're here
	$asset_destination = "$pkgroot/assets/$($info["appdata_folder_name"])"
	[array]::Reverse($info["assetdirs"])  # Earlier in the list overwrites later
	foreach ($assetdir in $info["assetdirs"]) {
        Write-Output "==== Copying assets from $assetdir ===="
		New-Item $asset_destination -ItemType Directory > $null
		Copy-Item -Recurse $assetdir/* $asset_destination
	}
}
$download_helper += '}'

New-Item $pkgroot/installers -ItemType Directory > $null
$download_helper | Out-File -Encoding utf8 "$pkgroot/installers/download-helper.ps1"

if ($zip) {
	# Zip it up
	Write-Output "==== Zipping ===="
	$zipfile = "$PWD/build/HamSandwich-windows.zip"
	Add-Type -AssemblyName System.IO.Compression.FileSystem

	# Replace backslashes in paths with forward slashes to fix nonstandardness.
	# Based on https://gist.github.com/lantrix/738ebfa616d5222a8b1db947793bc3fc
	class NoBackslashesEncoder : System.Text.UTF8Encoding {
		NoBackslashesEncoder() : base($true) {}

		[byte[]] GetBytes([string] $s) {
			$s = $s.Replace("\", "/");
			return ([System.Text.UTF8Encoding]$this).GetBytes($s);
		}
	}

	Remove-Item $zipfile -ErrorAction SilentlyContinue
	[System.IO.Compression.ZipFile]::CreateFromDirectory($pkgroot, $zipfile, [System.IO.Compression.CompressionLevel]::Optimal, $false, [NoBackslashesEncoder]::new())

	Write-Output "==== Success ===="
	Write-Output "The release .zip has been saved to:"
	Write-Output "    $zipfile"
}
