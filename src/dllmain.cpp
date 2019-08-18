// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

LPVOID Search(char* pPattern, size_t patternSize, uint8_t* scanStart, size_t scanSize)
{
	__try
	{
		auto res = std::search(
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
bool DataCompare(BYTE* pData, BYTE* bSig, char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bSig)
	{
		if (*szMask == 'x' && *pData != *bSig)
			return false;
	}
	return (*szMask) == NULL;
}

BYTE* FindPattern(BYTE* dwAddress, DWORD dwSize, BYTE* pbSig, char* szMask)
{
	DWORD length = strlen(szMask);
	for (DWORD i = NULL; i < dwSize - length; i++)
	{
		__try
		{
			if (DataCompare(dwAddress + i, pbSig, szMask))
				return dwAddress + i;
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			return nullptr;
		}
	}
	return 0;
}

DWORD WINAPI MainThread(LPVOID)
{
	HMODULE hModule = GetModuleHandle(NULL);
	MODULEINFO mInfo = { 0 };
	if (GetModuleInformation(GetCurrentProcess(), hModule, &mInfo, sizeof(MODULEINFO))) {
		
		LPVOID skipPod = FindPattern ((uint8_t*)hModule, mInfo.SizeOfImage, (BYTE*) "\x07\x0F\x85\x00\x00\x00\x00\xE8", "xxx????x");

		if (skipPod)
		{
			DWORD oldProtect;
			VirtualProtect ((char*)skipPod + 1, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			memset ((char*)skipPod + 1, 0x90, 1);
			VirtualProtect ((char*)skipPod + 1, 1, oldProtect, &oldProtect);

			VirtualProtect ((char*)skipPod + 2, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
			memset ((char*)skipPod + 2, 0xE9, 1);
			VirtualProtect ((char*)skipPod + 2, 1, oldProtect, &oldProtect);
		}

	}
	return 0;
}

typedef SOCKET (__stdcall* pfnconnect)(SOCKET s, const struct sockaddr* name, int namelen);


int WINAPI connecthook (DWORD RetAddr, pfnconnect fnconnect, SOCKET s, const struct sockaddr* name, int namelen)
{
	char *allowip = "151.101.8.246"; // Image
	char *allowip2 = "35.186.224.53"; // Ticket concert
	char ipstr[INET_ADDRSTRLEN];
	struct sockaddr_in* ipv4 = (struct sockaddr_in*)name;
	inet_ntop (ipv4->sin_family, &(ipv4->sin_addr), ipstr, INET_ADDRSTRLEN);
	if (_stricmp (ipstr, allowip) == 0) {
		return fnconnect (s, name, namelen);
	}
	if (_stricmp (ipstr, allowip2) == 0) {
		return fnconnect (s, name, namelen);
	}
	Sleep (10);
	return SOCKET_ERROR;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	DisableThreadLibraryCalls (hModule);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// Only patch the main process and none of the renderers/workers
		if (!wcsstr (GetCommandLine (), L"--type=")) {
			CreateThread (NULL, NULL, MainThread, NULL, 0, NULL);
		}
		if (wcsstr (GetCommandLine (), L"--type=utility")) {
			InstallHookApi ("ws2_32.dll", "connect", connecthook);
		}
		break;
	}
	return TRUE;
}

