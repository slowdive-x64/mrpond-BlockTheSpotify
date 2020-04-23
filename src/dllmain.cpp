// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

extern bool g_skip_wpad;
extern bool g_fix_blackbanner;
extern std::wofstream logging;

static const char* configFile = "./config.ini";

void init_log (const char* logFile)
{
	if (1 == GetPrivateProfileInt ("Config", "Log", 0, configFile)) {		
		logging.open (logFile, std::ios::out | std::ios::app);
		logging << "BlockTheSpot - Build date: " << __TIMESTAMP__ << std::endl;
	}
}

BOOL APIENTRY DllMain (HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
)
{
	DisableThreadLibraryCalls (hModule);
	std::string procname = GetCommandLine ();
	// only Spotify process - this help avoid false positive
	if (std::string::npos != procname.find ("Spotify.exe")) {
		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
			// block ads request - main process
			if (std::string::npos == procname.find ("--type=")) {
				InstallHookApi ("Winhttp.dll", "WinHttpOpenRequest", winhttpopenrequesthook);
				init_log ("main_log.txt");
				
			}
			else if (std::string::npos != procname.find ("--type=utility")) {
				// block ads banner by hostname - utility process
				InstallHookApi ("ws2_32.dll", "getaddrinfo", getaddrinfohook);
				init_log ("utility_log.txt");
				if (1 == GetPrivateProfileInt ("Config", "Skip_wpad", 0, configFile))
					g_skip_wpad = true;
				if (1 == GetPrivateProfileInt ("Config", "Fix_Black_Banner", 0, configFile))
					g_fix_blackbanner = true;
			}
			break;
		case DLL_PROCESS_DETACH:
			if (logging.is_open ()) {
				logging.flush ();
				logging.close ();
			}
			break;
		}
	}
	return TRUE;
}

