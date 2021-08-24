# build-binary-package.ps1

$projects = "mystic", "supreme", "sleepless", "loonyland", "loonyland2"

# Invoke vcvars32.bat to get msbuild access
cmd.exe /c "
	call `"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat`" >NUL
	&& set
" | ForEach-Object {
	if ($_ -match "^(.*?)=(.*)$") {
		Set-Content "env:\$($matches[1])" $matches[2]
	}
}

# Install dependencies, generate the solution, and build it
powershell -NoLogo -ExecutionPolicy Bypass -File  ./tools/msvc/install-dependencies.ps1
Write-Output "==== Generating solution ===="
./build/premake5.exe vs2019
foreach ($project in $projects) {
	msbuild ./build/msc-v142/$project.vcxproj /p:Configuration=release /p:Platform=Win32
}

# Create build/pkgroot
Write-Output "==== Preparing package root ===="
$pkgroot = "$PWD/build/pkgroot"
New-Item build -ItemType Directory -ErrorAction SilentlyContinue > $null
Remove-Item -Recurse $pkgroot -ErrorAction SilentlyContinue
New-Item $pkgroot -ItemType Directory > $null

# Copy everything in
foreach ($project in $projects) {
	Copy-Item ./build/msc-v142/release-x86/$project/$project.exe build/pkgroot/
}
Copy-Item -Destination $pkgroot -Path (
	"./build/SDL2-msvc/lib/x86/SDL2.dll",
	"./build/SDL2_image-msvc/lib/x86/SDL2_image.dll",
	"./build/SDL2_mixer-msvc/lib/x86/SDL2_mixer.dll",
	"./build/SDL2_mixer-msvc/lib/x86/libogg-0.dll",
	"./build/SDL2_mixer-msvc/lib/x86/libvorbis-0.dll",
	"./build/SDL2_mixer-msvc/lib/x86/libvorbisfile-3.dll"
)

# Zip it up
Write-Output "==== Zipping ===="
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip = "$PWD/build/HamSandwich-win32.zip"
Remove-Item $zip -ErrorAction SilentlyContinue
[System.IO.Compression.ZipFile]::CreateFromDirectory($pkgroot, $zip)
