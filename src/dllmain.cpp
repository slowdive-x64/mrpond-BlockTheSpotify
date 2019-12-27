// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

extern bool g_UseAdGuard;
extern bool g_Log;
extern bool g_Skip_wpad;
extern bool g_WinHttpReadDataFix;

extern std::ofstream Log_DNS;
extern std::ofstream Log_GetAddr;
extern std::ofstream Log_WinHttp;

PIP4_ARRAY pSrvList = NULL;

void Init_config () {
	if (0 == GetPrivateProfileInt ("Config", "AdGuardDNS", 1, "./config.ini"))
		g_UseAdGuard = false;
	if (0 < GetPrivateProfileInt ("Config", "Log", 0, "./config.ini"))
		g_Log = true;
	if (0 < GetPrivateProfileInt ("Config", "Skip_wpad", 0, "./config.ini"))
		g_Skip_wpad = true;
	if (0 < GetPrivateProfileInt ("Config", "WinHttpReadDataFix", 0, "./config.ini"))
		g_WinHttpReadDataFix = true;
}

void Init_log () {
	Log_DNS.open ("log_dnsquery.txt", std::ios::out | std::ios::app);
	Log_GetAddr.open ("log_getaddrinfo.txt", std::ios::out | std::ios::app);
	Log_WinHttp.open ("log_winhttp.txt", std::ios::out | std::ios::app);

	if (!g_UseAdGuard)
		Log_DNS << "AdGuard DNS Disable!\n";
}

void Init_DNS () {
	pSrvList = (PIP4_ARRAY)LocalAlloc (LPTR, sizeof (IP4_ARRAY));
	if (pSrvList) {
		// https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inetptonw
		if (1 == InetPton (AF_INET,
						   "176.103.130.134", // dns server ip
						   &pSrvList->AddrArray[0])) {
			// "Family protection"
			// adguard.com/en/adguard-dns/overview.html 
			pSrvList->AddrCount = 1;
			return;
		}
	}
	//
	g_UseAdGuard = false;
}

BOOL APIENTRY DllMain (HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
)
{
	// only utility process and main process
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (strstr (GetCommandLine (), "--type=utility") || !strstr (GetCommandLine (), "--type=")) {
			Init_config ();
			if (g_UseAdGuard) Init_DNS ();
			if (g_Log) Init_log ();
			// block ads banner by hostname.
			InstallHookApi ("ws2_32.dll", "getaddrinfo", getaddrinfohook);
			// block ads by manipulate json response.
			InstallHookApi ("Winhttp.dll", "WinHttpReadData", winhttpreaddatahook);
		}
		break;
	case DLL_PROCESS_DETACH:
		if (g_Log) {
			Log_DNS.close ();
			Log_GetAddr.close ();
			Log_WinHttp.close ();
		}
		if (pSrvList)
			LocalFree (pSrvList);
		break;
	}
	return TRUE;
}

