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
  checksum = '29B82B56B4FA51F54826C7585E41FA0438EC40ED1E3F6A31BDBA80468795549F'
  checksumType = 'sha256'
}

Install-ChocolateyZipPackage @packageArgs

# Install provided shortcuts
$startupDirectory = "$env:ALLUSERSPROFILE\Microsoft\Windows\Start Menu\Programs\Startup\"
Copy-Item -Path "$env:ProgramFiles\HotCorners\HotCorners*.lnk" -Destination $startupDirectory -Force

# Run each shortcut
$shortcuts = Get-ChildItem -Path $startupDirectory -Filter "HotCorners*.lnk"
foreach ($shortcut in $shortcuts) {
    Start-Process -FilePath $shortcut.FullName
}
