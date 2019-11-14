// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#ifdef _DEBUG
extern std::ofstream g_DNSLog;
extern std::ofstream g_GetAddrLog;
extern std::ofstream g_WinHttpLog;
#endif

BOOL APIENTRY DllMain (HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls (hModule);
#ifdef _DEBUG
		g_DNSLog.open ("log_dnsquery.txt",
			std::ios::out | std::ios::app);
		g_GetAddrLog.open ("log_getaddrinfo.txt",
			std::ios::out | std::ios::app);
		g_WinHttpLog.open ("log_winhttp.txt",
			std::ios::out | std::ios::app);
#endif

		// block ads banner by hostname.
		InstallHookApi ("ws2_32.dll", "getaddrinfo", getaddrinfohook);
		// block ads by manipulate json response.
		InstallHookApi ("Winhttp.dll", "WinHttpReadData", winhttpreaddatahook);
		break;
	case DLL_PROCESS_DETACH:
#ifdef _DEBUG
		g_DNSLog.close ();
		g_GetAddrLog.close ();
		g_WinHttpLog.close ();
#endif
		break;
	}

	return TRUE;
}

