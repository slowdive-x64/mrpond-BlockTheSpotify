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
		// block ads banner by hostname.
		InstallHookApi ("ws2_32.dll", "getaddrinfo", getaddrinfohook);
		// block ads by manipulate json response.
		InstallHookApi ("Winhttp.dll", "WinHttpReadData", winhttpreaddatahook);
		break;
	}
	return TRUE;
}

