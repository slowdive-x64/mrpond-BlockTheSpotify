;;;===,,,@echo off
;;;===,,,findstr /v "^;;;===,,," "%~f0" > "%~dp0ps.ps1"
;;;===,,,PowerShell.exe -ExecutionPolicy Bypass -Command "& '%~dp0ps.ps1'"
;;;===,,,del /s /q "%~dp0ps.ps1" >NUL 2>&1
;;;===,,,pause

# Ignore errors from `Stop-Process`
$PSDefaultParameterValues['Stop-Process:ErrorAction'] = 'SilentlyContinue'

write-host @'
***************** 
Author: @rednek46
***************** 
'@

$SpotifyDirectory = "$env:APPDATA\Spotify"
$SpotifyExecutable = "$SpotifyDirectory\Spotify.exe"

Write-Host 'Terminating Spotify...'
Write-Host("")
Stop-Process -Name Spotify
Stop-Process -Name SpotifyWebHelper

if (Get-AppxPackage -Name SpotifyAB.SpotifyMusic) {
  Write-Host @'
The Microsoft Store version of Spotify has been detected which is not supported.
Please uninstall it first, and Run this file again.

To uninstall, search for Spotify in the start menu and right-click on the result and click Uninstall.
'@
  Pause
  exit
}

mkdir $env:TEMP/BlockTheSpotTEMP `
  | Convert-Path `
  | Set-Location

Write-Host("Downloading latest patch (chrome_elf.zip)")
Write-Host("")

$webClient = New-Object -TypeName System.Net.WebClient
try {
  $webClient.DownloadFile(
    'https://github.com/mrpond/BlockTheSpot/releases/latest/download/chrome_elf.zip',
    "$PWD\chrome_elf.zip"
  )
} catch {
  Write-Output $_
  Sleep
}

if (test-path $SpotifyDirectory/Spotify.exe){

    if (!(test-path $SpotifyDirectory/chrome_elf.dll.bak)){

		move $SpotifyDirectory\chrome_elf.dll $SpotifyDirectory\chrome_elf.dll.bak >$null 2>&1
	}

} else {
	write-host @'
Spotify installation was not detected. Downloading Latest Spotify full setup.
Please wait..
'@
    try {
    $webClient.DownloadFile(
      'https://download.scdn.co/SpotifyFullSetup.exe',
      "$PWD\SpotifyFullSetup.exe"
    )
    } catch {
        Write-Output $_
        Sleep
    }
	
    write-host("Running installation...")

	.\SpotifyFullSetup.exe | Out-Null

	rm $pwd/SpotifyFullSetup.exe >$null 2>&1
    move $SpotifyDirectory\chrome_elf.dll $SpotifyDirectory\chrome_elf.dll.bak >$null 2>&1
}
Write-Host("Patching Spotify..")
Expand-Archive -Force 'chrome_elf.zip' $pwd
$patchFiles = "$PWD\chrome_elf.dll", "$PWD\config.ini"
Copy-Item -LiteralPath $patchFiles -Destination "$SpotifyDirectory"
cd $env:HOMEPATH
rm -Recurse $env:TEMP\BlockTheSpotTEMP
Write-Host("Patching Completed.")
exit 