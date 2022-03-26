# bootstrap/busybox_.ps1
#
# BusyBox bootstrapping script for Windows.
#
# Automatically downloads Frippery's BusyBox for Windows to a cache directory
# and invokes it. See: https://frippery.org/busybox/
#
# The underscore in the name is so that typing `bootstrap/busybox` into
# PowerShell finds the `.bat` file first, which ensures this script executes
# regardless of ExecutionPolicy.
$ArgsOrDefault = if ($args.Length -eq 0) { @("sh") } else { $args }

$OldTitle = $host.ui.RawUI.WindowTitle
$host.ui.RawUI.WindowTitle = "starting :: $ArgsOrDefault"
$ErrorActionPreference = "Stop"

# Change these variables to update Busybox.
$BusyboxVersion = "busybox-w32-FRP-4487-gd239d2d52"
$ExpectedSha256 = "35e2b0db6d57a045188b9afc617aae52a6c8e2aa0205256c049f3537a48f879b"

# Opt in to TLS 1.2 support
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

# Implement sha256sum to verify download success
$sha256 = New-Object System.Security.Cryptography.SHA256CryptoServiceProvider
function CheckHash {
	param([string] $File, [string] $Sha)
	$got = $sha256.ComputeHash([System.IO.File]::ReadAllBytes("$File"))
	$got = [System.BitConverter]::ToString($got).Replace('-', '').ToLower()
	if ($got -ne $Sha) {
		Write-Output "${File}: FAILED"
		exit 1
	} else {
		Write-Output "${File}: OK"
	}
}

# Convenience variables
$Bootstrap = Split-Path $script:MyInvocation.MyCommand.Path
$Cache = "$Bootstrap/.cache"
if ($Env:TG_BOOTSTRAP_CACHE) {
	$Cache = $Env:TG_BOOTSTRAP_CACHE
}
$BusyboxUrl = "https://frippery.org/files/busybox/$BusyboxVersion.exe"
$BusyboxExe = "$Cache/$BusyboxVersion.exe"

# Download Busybox, validate it, and commit it if validation succeeded
if (!(Test-Path $BusyboxExe -PathType Leaf)) {
	$host.ui.RawUI.WindowTitle = "downloading :: $BusyboxVersion"
    $TempExe = "$BusyboxExe.tmp"
	New-Item "$Cache" -ItemType Directory -ErrorAction silentlyContinue | Out-Null
	Invoke-WebRequest "$BusyboxUrl" -OutFile "$TempExe" -ErrorAction Stop
    CheckHash -File "$TempExe" -Sha "$ExpectedSha256"
    Rename-Item "$TempExe" "$BusyboxExe"
}

# Invoke with all command-line arguments
$TitleArgs = [System.Collections.Generic.List[string]]($ArgsOrDefault)
if ("$TitleArgs".StartsWith("sh $Bootstrap\")) {
	$TitleArgs.RemoveAt(0)
	$TitleArgs[0] = $TitleArgs[0].Substring($Bootstrap.Length + 1)
}
$host.ui.RawUI.WindowTitle = "$TitleArgs"
& "$BusyboxExe" $ArgsOrDefault
$host.ui.RawUI.WindowTitle = $OldTitle
exit $LastExitCode
