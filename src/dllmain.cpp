// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

extern bool g_Log;
extern bool g_Skip_wpad;
extern bool g_WinHttpReadDataFix;

extern std::ofstream Log_DNS;
extern std::ofstream Log_GetAddr;
extern std::ofstream Log_WinHttp;
extern Adblock g_Adsblock;

const char* configFile = "./config.ini";

void init_config () {
	if (0 == GetPrivateProfileInt ("Config", "AdGuardDNS", 1, configFile))
		g_Adsblock.deactivate ();
	else
		g_Adsblock.activate ();

	if (1 == GetPrivateProfileInt ("Config", "Log", 0, configFile))
		g_Log = true;
	if (1 == GetPrivateProfileInt ("Config", "Skip_wpad", 0, configFile))
		g_Skip_wpad = true;
	if (1 == GetPrivateProfileInt ("Config", "WinHttpReadDataFix", 0, configFile))
		g_WinHttpReadDataFix = true;
}

void init_log () {
	if (g_Log) {
		Log_DNS.open ("log_dnsquery.txt",
					  std::ios::out | std::ios::app);
		Log_GetAddr.open ("log_getaddrinfo.txt",
						  std::ios::out | std::ios::app);
		Log_WinHttp.open ("log_winhttp.txt",
						  std::ios::out | std::ios::app);
	}
}

BOOL APIENTRY DllMain (HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
)
{
	DisableThreadLibraryCalls(hModule);
	if (strstr (GetCommandLine (), "--type=utility") ||
		!strstr (GetCommandLine (), "--type=")) {

		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
			init_config ();
			init_log ();

			if (false == g_Adsblock.init (configFile)) {
				if (g_Log) Log_DNS << "AdGuard DNS Disable!" << std::endl;
				g_Adsblock.deactivate ();
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
			g_Adsblock.destroy ();
			break;
		}
	}
	return TRUE;
}

