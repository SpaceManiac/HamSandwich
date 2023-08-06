[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
Invoke-WebRequest -OutFile "C:\tmp\vs_buildtools.exe" -Uri https://aka.ms/vs/17/release/vs_buildtools.exe
Start-Process -FilePath C:\tmp\vs_buildtools.exe -ArgumentList (
	'--passive',
	'--wait',
	'--add', 'Microsoft.VisualStudio.Workload.VCTools',
	'--add', 'Microsoft.VisualStudio.Component.VC.CMake.Project',
	'--add', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
	'--add', 'Microsoft.VisualStudio.Component.Windows10SDK.19041'
) -Wait
