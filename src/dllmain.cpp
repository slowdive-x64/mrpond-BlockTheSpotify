// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

extern bool g_Log;
extern bool g_Skip_wpad;
extern bool g_WinHttpReadDataFix;

extern std::ofstream Log_DNS;
extern std::ofstream Log_WinHttp;
extern Adsblock g_Adsblock;

void init_log () {
	Log_DNS.open ("log_dnsquery.txt",
				  std::ios::out | std::ios::app);
	Log_WinHttp.open ("log_winhttp.txt",
					  std::ios::out | std::ios::app);
}

void init_config () {
	const char* configFile = "./config.ini";
	char IP[INET_ADDRSTRLEN];

	if (1 == GetPrivateProfileInt ("Config", "Log", 0, configFile)) {
		g_Log = true;
		init_log ();
	}
	if (1 == GetPrivateProfileInt ("Config", "Skip_wpad", 0, configFile))
		g_Skip_wpad = true;
	if (1 == GetPrivateProfileInt ("Config", "WinHttpReadDataFix", 0, configFile))
		g_WinHttpReadDataFix = true;

	if (1 == GetPrivateProfileInt ("Config", "AdGuardDNS", 1, configFile)) {
		GetPrivateProfileString ("Config",
								 "AdGuardDNS_IP",
								 "176.103.130.134",
								 IP,
								 INET_ADDRSTRLEN,
								 configFile);
		g_Adsblock.setDNSIP (IP);
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
			// block ads request
			InstallHookApi ("Winhttp.dll", "WinHttpOpenRequest", winhttpopenrequesthook);
			// block ads banner by hostname.
			InstallHookApi ("ws2_32.dll", "getaddrinfo", getaddrinfohook);
			// block ads by manipulate json response.
			InstallHookApi ("Winhttp.dll", "WinHttpReadData", winhttpreaddatahook);
			break;
		case DLL_PROCESS_DETACH:
			if (g_Log) {
				Log_DNS.close ();
				Log_WinHttp.close ();
			}
			g_Adsblock.destroy ();
			break;
		}
	}
	return TRUE;
}

