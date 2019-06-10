# install-msvc-deps.ps1

mkdir build -ErrorAction SilentlyContinue > $null

[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
Add-Type -AssemblyName System.IO.Compression.FileSystem

$sha256 = New-Object System.Security.Cryptography.SHA256CryptoServiceProvider

function CheckHash {
	param([string] $File, [string] $Sha)
	$got = $sha256.ComputeHash([System.IO.File]::ReadAllBytes("$PWD/$FILE"))
	$got = [System.BitConverter]::ToString($got).Replace('-', '').ToLower()
	if ($got -ne $Sha) {
		echo "${File}: FAILED"
		exit 1
	} else {
		echo "${File}: OK"
	}
}

# Fetch premake5 binary
if (-not (Test-Path "build/premake5.exe")) {
	echo "==== Downloading premake5 binary ===="
	Invoke-WebRequest `
		-Uri "https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-windows.zip" `
		-OutFile "build/premake5.zip" `
		-ErrorAction Stop
	CheckHash -File "build/premake5.zip" -Sha "070419eedbb4c8737664d05f0e3701055741014c1135a9214c35d6feb03724a7"

	[System.IO.Compression.ZipFile]::ExtractToDirectory("$PWD/build/premake5.zip", "$PWD/build")
	rm build/premake5.zip -ErrorAction Stop
}

# Official SDL2, SDL2_mixer, SDL2_image MSVC binaries
if (-not (Test-Path "build/SDL2-msvc")) {
	echo "==== Downloading SDL2 MSVC binaries ===="
	Invoke-WebRequest `
		-Uri "https://www.libsdl.org/release/SDL2-devel-2.0.9-VC.zip" `
		-OutFile "build/SDL2.zip" `
		-ErrorAction Stop
	CheckHash -File "build/SDL2.zip" -Sha "ea266ef613f88433f493498f9e72e6bed5d03e4f3fde5b571a557a754ade9065"

	[System.IO.Compression.ZipFile]::ExtractToDirectory("$PWD/build/SDL2.zip", "$PWD/build")
	mv build/SDL2-2.0.9 build/SDL2-msvc -ErrorAction Stop
	rm build/SDL2.zip -ErrorAction Stop
}

if (-not (Test-Path "build/SDL2_mixer-msvc")) {
	echo "==== Downloading SDL2_mixer MSVC binaries ===="
	Invoke-WebRequest `
		-Uri "https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.0.4-VC.zip" `
		-OutFile "build/SDL2_mixer.zip" `
		-ErrorAction Stop
	CheckHash -File "build/SDL2_mixer.zip" -Sha "258788438b7e0c8abb386de01d1d77efe79287d9967ec92fbb3f89175120f0b0"

	[System.IO.Compression.ZipFile]::ExtractToDirectory("$PWD/build/SDL2_mixer.zip", "$PWD/build")
	mv build/SDL2_mixer-2.0.4 build/SDL2_mixer-msvc -ErrorAction Stop
	rm build/SDL2_mixer.zip -ErrorAction Stop
}

if (-not (Test-Path "build/SDL2_image-msvc")) {
	echo "==== Downloading SDL2_image MSVC binaries ===="
	Invoke-WebRequest `
		-Uri "https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.4-VC.zip" `
		-OutFile "build/SDL2_image.zip" `
		-ErrorAction Stop
	CheckHash -File "build/SDL2_image.zip" -Sha "4e15fad9de43d738b476e422eef1910432443cead60d2084b3ef01d3f4a76087"

	[System.IO.Compression.ZipFile]::ExtractToDirectory("$PWD/build/SDL2_image.zip", "$PWD/build")
	mv build/SDL2_image-2.0.4 build/SDL2_image-msvc -ErrorAction Stop
	rm build/SDL2_image.zip -ErrorAction Stop
}
