// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

extern bool g_UseAdGuard;
extern bool g_Log;
extern bool g_Skip_wpad;
extern bool g_WinHttpReadDataFix;

extern std::ofstream Log_DNS;
extern std::ofstream Log_GetAddr;
extern std::ofstream Log_WinHttp;

BOOL APIENTRY DllMain (HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls (hModule);
		if (0 == GetPrivateProfileIntA ("Config", "AdGuardDNS", 1, "./config.ini"))
			g_UseAdGuard = false;
		if (0 < GetPrivateProfileIntA ("Config", "Log", 0, "./config.ini"))
			g_Log = true;
		if (0 < GetPrivateProfileIntA ("Config", "Skip_wpad", 0, "./config.ini"))
			g_Skip_wpad = true;
		if (0 < GetPrivateProfileIntA ("Config", "WinHttpReadDataFix", 0, "./config.ini"))
			g_WinHttpReadDataFix = true;

		if (g_Log) {
			Log_DNS.open ("log_dnsquery.txt",
						  std::ios::out | std::ios::app);
			Log_GetAddr.open ("log_getaddrinfo.txt",
							  std::ios::out | std::ios::app);
			Log_WinHttp.open ("log_winhttp.txt",
							  std::ios::out | std::ios::app);
			if (!g_UseAdGuard)
				Log_DNS << "AdGuard DNS Disable!\n";
		}
		
		// block ads banner by hostname.
		InstallHookApi ("ws2_32.dll", "getaddrinfo", getaddrinfohook);
		// block ads by manipulate json response.
		InstallHookApi ("Winhttp.dll", "WinHttpReadData", winhttpreaddatahook);
		break;
	case DLL_PROCESS_DETACH:
		if (g_Log) {
			Log_DNS.close ();
			Log_GetAddr.close ();
			Log_WinHttp.close ();
		}
		break;
	}

	return TRUE;
}

