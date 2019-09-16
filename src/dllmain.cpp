// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

BOOL APIENTRY DllMain (HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls (hModule);
		// block the ads banner by hostname.
		// but I like new song ad.
		//InstallHookApi ("ws2_32.dll", "getaddrinfo", getaddrinfohook);
		
		// real ads block, no data out
		InstallHookApi ("Winhttp.dll", "WinHttpSendRequest", winhttpsendrequesthook);
		InstallHookApi ("Winhttp.dll", "WinHttpSetStatusCallback", winhttpsetstatuscallbackhook);
		// we don't need it, for future only.
		//InstallHookApi ("Winhttp.dll", "WinHttpWriteData", winhttpwritedatahook);
		//InstallHookApi ("Winhttp.dll", "WinHttpReceiveResponse", winhttpreceiveresponsehook);
		//InstallHookApi ("Winhttp.dll", "WinHttpQueryDataAvailable", winhttpquerydataavailablehook);

		// archive
		// sent only clicked playlist data back to Spotify.
		//InstallHookApi ("Winhttp.dll", "WinHttpOpenRequest", winhttpopenrequesthook);
		break;
	}
	return TRUE;
}

