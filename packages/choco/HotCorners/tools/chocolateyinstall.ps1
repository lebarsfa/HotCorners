$ErrorActionPreference = 'Stop' # stop on all errors

# Stop any running HotCorners process
Get-Process -Name "HotCorners" -ErrorAction SilentlyContinue | ForEach-Object { $_.CloseMainWindow() }
Start-Sleep -Seconds 1
Stop-Process -Name "HotCorners" -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 2

$packageArgs = @{
  packageName = $env:ChocolateyPackageName
  unzipLocation = $env:ProgramFiles
  url = 'https://github.com/lebarsfa/HotCorners/releases/download/v0.1.8/HotCorners.zip'
  checksum = '51D6F0DA75120281529F9E450E54C37DBCF81E65A818173AED7A7014D3E3F761'
  checksumType = 'sha256'
}

Install-ChocolateyZipPackage @packageArgs

$programDirectory = "$env:ProgramFiles\HotCorners"

# Install provided shortcuts
$startupDirectory = "$env:ALLUSERSPROFILE\Microsoft\Windows\Start Menu\Programs\Startup"
Copy-Item -Path "$programDirectory\HotCorners*.lnk" -Destination $startupDirectory -Force

# Run each shortcut
$shortcuts = Get-ChildItem -Path $startupDirectory -Filter "HotCorners*.lnk"
foreach ($shortcut in $shortcuts) {
    Start-Process -FilePath $shortcut.FullName
}

# Create group shortcuts
$shortcutsDirectory = "$env:ALLUSERSPROFILE\Microsoft\Windows\Start Menu\Programs\Hot Corners"
Install-ChocolateyShortcut -ShortcutFilePath "$shortcutsDirectory\Hot Corners Kill All.lnk" -TargetPath "$programDirectory\HotCornersKillAll.bat" -WorkingDirectory "$programDirectory"
Install-ChocolateyShortcut -ShortcutFilePath "$shortcutsDirectory\Hot Corners Launch All.lnk" -TargetPath "$programDirectory\HotCornersLaunchAll.bat" -WorkingDirectory "$programDirectory"
