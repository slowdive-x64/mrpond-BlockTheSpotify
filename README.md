<center>
  <h1 align="center">BlockTheSpot</h1>
  <h4 align="center">A multi-purpose adblocker and skip bypass for the <strong>Windows</strong> Spotify Desktop Application.</h4>
  <h5 align="center">Please support Spotify by purchasing premium</h5>
  <p align="center">
    <strong>Current Version:</strong> 0.38 <br>
    <strong>Last updated:</strong> 11 December 2019 <br>
    <strong>Last tested version:</strong> 1.1.21.1654.g282a2807
  </p>
  <h4 align="center">Important Notice(s)</h4>
  <p align="center">
    This .dll is virus free, false checks may appear. This can be reassured via the source code. <br>
    "chrome_elf.dll" gets replaced by Spotify Installer each time it updates, make sure to replace it again.
  </p>
</center>

### Features:
* Blocks all banner/video/audio ads within the app
* Retains friend, vertical video and radio functionality
* Unlocks the skip function for any track
* Built-in AdGuard DNS for ads blocking!

:warning: This mod is for the [**Desktop Application**](https://www.spotify.com/download/windows/) of Spotify on Windows, **not the Microsoft Store version**.

#### Installation/Update:
1. Browse to Spotify installation folder `%APPDATA%/Spotify`
2. Backup existing `chrome_elf.dll` by renaming it (e.g. chrome_elf.bak)
3. Download [chrome_elf.zip](chrome_elf.zip)
4. Extract chrome_elf.dll, config.ini from chrome_elf.zip to that folder. 

#### Uninstall:
1. Browse to Spotify installation folder `%APPDATA%/Spotify` 
2. Simply delete chrome_elf.dll, config.ini from your Spotify installation
3. Rename your backup dll to chrome_elf.dll

#### Note:
* Built-in AdGuard DNS lookup, paranoid people may see additional DNS traffic UDP(53) 
going out to Adguard DNS Server.
* if you can't reach AdGuard DNS for some reason, things still working as expected.

#### Known Issues and Caveats:
* Ads Blocking may not work if you had proxy use in network.
set Skip_wpad in config.ini to 1 may help.
* Ads Blocking on system may not work e.g.(windows 7)
set WinHttpReadDataFix in config.ini to 1 may help.
* Built-in AdGuard DNS lookup, maybe cause application slowdown,
set AdGuardDNS in config.ini to 0 to turn this feature off.