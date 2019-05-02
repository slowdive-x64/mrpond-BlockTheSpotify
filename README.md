# BlockTheSpot

## Video, audio & banner adblock/skip for Spotify

**Current Version:** 0.14 unofficial

**Last updated:** 3rd May 2019

**Last tested version:** 1.1.5.153.gf614956d

### Important Notice

--

#### How do I re-enable automatic updates?

Run Command Prompt as administrator and enter the following command:
```
icacls "%localappdata%\Spotify\Update" /reset /T
```

### Features:
* Windows only
* Set and forget
* Blocks all banner/video/audio ads within the app
* Retains friend, vertical video and radio functionality
* Unlocks the skip function for any track

:warning: This mod is for the [**Desktop release**](https://www.spotify.com/download/windows/) of Spotify on Windows and **not the Microsoft Store version**.

### Install/Uninstall:

#### Easy Installation:
[Download](install.bat) and run install.bat. You don't need to download any other file. 

PS - It's not encrypted at all, it's a batch file with the mod embedded at the end (see the "MZ" header). You can always use the manual installation method.

#### Manual Installation:
[Download](chrome_elf.dll) 

browse to directory inside the Spotify installation folder (next to Spotify.exe)
rename existing chrome_elf.dll to chrome_elf_.dll
and drop chrome_elf.dll. 

The default Spotify installation location is: %APPDATA%/Spotify (copy and paste into Explorer address bar)

#### Uninstall:
To uninstall, simply delete chrome_elf.dll from your Spotify installation
and rename chrome_elf_.dll to chrome_elf.dll
(see above for the location).
