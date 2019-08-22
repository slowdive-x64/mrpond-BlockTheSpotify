// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

LPVOID Search (char* pPattern, size_t patternSize, uint8_t* scanStart, size_t scanSize)
{
	__try
	{
		auto res = std::search (
			scanStart, scanStart + scanSize, pPattern, pPattern + patternSize,
			[](uint8_t val1, uint8_t val2) { return (val1 == val2); }
		);

		return (res >= scanStart + scanSize) ? nullptr : res;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return nullptr;
	}
}

LPVOID FindFunction (char* hModule, DWORD hModuleSize, char* midFuncPtn, int lenMidFuncPtn, int seekBackCount, char* startFuncPtn, int lenStartFuncPtn)
{
	LPVOID pfnAddr = Search (midFuncPtn, lenMidFuncPtn, (uint8_t*)hModule, hModuleSize);
	if (!pfnAddr) return NULL;
	char* pfnStart = NULL;
	char* pfnCurrent = (char*)pfnAddr - seekBackCount;
	while ((pfnCurrent = (char*)Search (startFuncPtn, lenStartFuncPtn, (uint8_t*)pfnCurrent, hModule + hModuleSize - pfnCurrent)) &&
		pfnCurrent < pfnAddr)
	{
		pfnStart = pfnCurrent;
		pfnCurrent++;
	}

	return pfnStart;
}

// https://www.unknowncheats.me/forum/1064672-post23.html
bool DataCompare (BYTE* pData, BYTE* bSig, char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bSig)
	{
		if (*szMask == 'x' && *pData != *bSig)
			return false;
	}
	return (*szMask) == NULL;
}

BYTE* FindPattern (BYTE* dwAddress, DWORD dwSize, BYTE* pbSig, char* szMask)
{
	DWORD length = strlen (szMask);
	for (DWORD i = NULL; i < dwSize - length; i++)
	{
		__try
		{
			if (DataCompare (dwAddress + i, pbSig, szMask))
				return dwAddress + i;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return nullptr;
		}
	}
	return 0;
}

DWORD WINAPI MainThread (LPVOID)
{
	HMODULE hModule = GetModuleHandle (NULL);
	MODULEINFO mInfo = { 0 };
	if (GetModuleInformation (GetCurrentProcess (), hModule, &mInfo, sizeof (MODULEINFO))) {

		LPVOID skipPod = FindPattern ((uint8_t*)hModule, mInfo.SizeOfImage, (BYTE*) "\xFF\x07\x0F\x85\x00\x00\x00\x00\xE8", "xxxx????x");

		if (skipPod)
		{
			DWORD oldProtect;
			VirtualProtect ((char*)skipPod + 2, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			memset ((char*)skipPod + 2, 0x90, 1);
			VirtualProtect ((char*)skipPod + 2, 1, oldProtect, &oldProtect);

			VirtualProtect ((char*)skipPod + 3, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			memset ((char*)skipPod + 3, 0xE9, 1);
			VirtualProtect ((char*)skipPod + 3, 1, oldProtect, &oldProtect);
		}

	}
	return 0;
}

BOOL APIENTRY DllMain (HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls (hModule);
		// Only patch the main process and none of the other
		if (!wcsstr (GetCommandLine (), L"--type=")) {
			//skip processing ads but ads still downloaded
			CreateThread (NULL, NULL, MainThread, NULL, 0, NULL);
		}
		if (wcsstr (GetCommandLine (), L"--url=")) {
			exit (0);
		}
		// block the ads banner by hostname
		InstallHookApi ("ws2_32.dll", "getaddrinfo", getaddrinfohook);
		// real ads block
		InstallHookApi ("Winhttp.dll", "WinHttpOpenRequest", winhttpopenrequesthook);
		// for future
		//InstallHookApi ("ws2_32.dll", "connect", connecthook);
		break;
	}
	return TRUE;
}

