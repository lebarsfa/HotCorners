﻿$ErrorActionPreference = 'Stop' # stop on all errors

# Uninstall provided shortcuts
Remove-Item -Path "$env:ALLUSERSPROFILE\Microsoft\Windows\Start Menu\Programs\Startup\HotCorners*.lnk" -Force -ErrorAction SilentlyContinue

# Stop any running HotCorners process
Get-Process -Name "HotCorners" -ErrorAction SilentlyContinue | ForEach-Object { $_.CloseMainWindow() }
Start-Sleep -Seconds 1
Stop-Process -Name "HotCorners" -Force -ErrorAction SilentlyContinue
Start-Sleep -Seconds 2

$url64 = 'https://github.com/lebarsfa/HotCorners/releases/download/v0.1.8/HotCorners.zip'

$packageArgs = @{
  packageName   = $env:ChocolateyPackageName
  ZipFileName = 'HotCorners.zip'
}

Uninstall-ChocolateyZipPackage @packageArgs
