// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

extern bool g_Log;
extern bool g_Skip_wpad;

extern std::wofstream Log;

void init_log () {
	Log.open ("log.txt",
				  std::ios::out | std::ios::app);
}

void init_config () {
	const char* configFile = "./config.ini";

	if (1 == GetPrivateProfileInt ("Config", "Log", 0, configFile)) {
		g_Log = true;
		init_log ();
	}
	if (1 == GetPrivateProfileInt ("Config", "Skip_wpad", 0, configFile))
		g_Skip_wpad = true;

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
			break;
		case DLL_PROCESS_DETACH:
			if (g_Log) {
				Log.close ();
			}
			break;
		}
	}
	return TRUE;
}

