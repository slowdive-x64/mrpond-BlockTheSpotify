// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

extern bool g_UseAdGuard;
extern bool g_Log;
extern bool g_Skip_wpad;
extern bool g_WinHttpReadDataFix;

extern std::ofstream Log_DNS;
extern std::ofstream Log_GetAddr;
extern std::ofstream Log_WinHttp;

PIP4_ARRAY pSrvList = nullptr;
const char* configFile = "./config.ini";

void init_config () {
	if (0 == GetPrivateProfileInt ("Config", "AdGuardDNS", 1, configFile))
		g_UseAdGuard = false;
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

bool init_DNS () {
	if (!g_UseAdGuard) {
		if (g_Log) Log_DNS << "AdGuard DNS Disable!" << std::endl;
		return true;
	}
	pSrvList = (PIP4_ARRAY)LocalAlloc (LPTR, sizeof (IP4_ARRAY));
	if (nullptr != pSrvList) {
		char DNS_IP[256];
		GetPrivateProfileString ("Config",
								 "AdGuardDNS_IP",
								 "176.103.130.134",
								 DNS_IP,
								 256,
								 configFile);
		// https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inetptonw
		if (1 == InetPton (AF_INET,
						   DNS_IP, // dns server ip
						   &pSrvList->AddrArray[0])) {
			pSrvList->AddrCount = 1;
			if (g_Log)
				Log_DNS << "AdGuard DNS Server - " << DNS_IP << std::endl;
			return true;
		}
		else {
			if (g_Log)
				Log_DNS << "AdGuard DNS Disable - InetPton " 
				<< DNS_IP << " failed!" << std::endl;
		}
	}
	else {
		if (g_Log)
			Log_DNS << "AdGuard DNS Disable - "
			<< "pSrvList LocalAlloc failed!" << std::endl;
	}
	return false;
}

BOOL APIENTRY DllMain (HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
)
{
	if (strstr (GetCommandLine (), "--type=utility") ||
		!strstr (GetCommandLine (), "--type=")) {

		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
			init_config ();
			init_log ();
			if (!init_DNS ()) {
				g_UseAdGuard = false;
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
			if (nullptr != pSrvList)
				LocalFree (pSrvList);
			break;
		}
	}
	return TRUE;
}

