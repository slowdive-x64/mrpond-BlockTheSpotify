<center>
  <h1 align="center">BlockTheSpot</h1>
  <h4 align="center">A multi-purpose adblocker and skip bypass for the <strong>Windows</strong> Spotify Desktop Application.</h4>
  <h5 align="center">Please support Spotify by purchasing premium</h5>
  <p align="center">
    <strong>Current Version:</strong> 0.43 <br>
    <strong>Last updated:</strong> 11 January 2020<br>
    <strong>Last tested version:</strong> 1.1.22.633.g1bab253a
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
* Starting from v0.42, AdGuardDNS look up in config.ini are disable by default.
* Built-in AdGuard DNS lookup, paranoid people may see additional DNS traffic UDP(53) 
going out to Adguard DNS Server.
* if you can't reach AdGuard DNS for some reason, things still working as expected.
* You can change Ads Check(blocking)DNS via config.ini AdGuardDNS_IP default are '176.103.130.134'

#### Known Issues and Caveats:
* Ads banner maybe appear if you network use 'Web Proxy Auto-Discovery Protocol'
https://en.wikipedia.org/wiki/Web_Proxy_Auto-Discovery_Protocol
set Skip_wpad in config.ini to 1 may help.
* On windows 7, Ads Blocking may not work
set WinHttpReadDataFix in config.ini to 1 may help.
* Built-in AdGuard DNS lookup, maybe cause application slowdown,
set AdGuardDNS in config.ini to 0 to turn this feature off.