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

		// ==== archive ====
		// real (ads) block, no data out. bug on facebook login/search.
		//InstallHookApi ("Winhttp.dll", "WinHttpSendRequest", winhttpsendrequesthook);
		//InstallHookApi ("Winhttp.dll", "WinHttpSetStatusCallback", winhttpsetstatuscallbackhook);

		// we don't need it, for future only.
		//InstallHookApi ("Winhttp.dll", "WinHttpWriteData", winhttpwritedatahook);
		//InstallHookApi ("Winhttp.dll", "WinHttpReceiveResponse", winhttpreceiveresponsehook);
		//InstallHookApi ("Winhttp.dll", "WinHttpQueryDataAvailable", winhttpquerydataavailablehook);

		// block ads request but it keep retrying.
		//InstallHookApi ("Winhttp.dll", "WinHttpOpenRequest", winhttpopenrequesthook);
		break;
	}
	return TRUE;
}

