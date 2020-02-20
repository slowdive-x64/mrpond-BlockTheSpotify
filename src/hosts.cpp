#include "stdafx.h"

bool g_Log = false;
bool g_Skip_wpad = false;
bool g_WinHttpReadDataFix = false;

std::ofstream Log_WinHttp;
extern std::ofstream Log_DNS;

extern Adsblock g_Adsblock;

int WINAPI getaddrinfohook (DWORD RetAddr,
							pfngetaddrinfo fngetaddrinfo,
							const char* nodename,
							const char* servname,
							const struct addrinfo* hints,
							struct addrinfo** res)
{
	auto lookup = std::async (std::launch::async, &Adsblock::isblock, g_Adsblock, nodename);
	// future/async
	auto result = fngetaddrinfo (nodename,
								 servname,
								 hints,
								 res);

	bool isBlock = lookup.get ();
	if (0 == result && isBlock) { // GetAddrInfo return 0 on success

		for (auto ptr = *res; nullptr != ptr; ptr = ptr->ai_next) {
			auto ipv4 = (struct sockaddr_in*)ptr->ai_addr;
			//memset (&ipv4->sin_addr.S_un.S_addr, 0x0, sizeof ULONG);
			ipv4->sin_addr.S_un.S_addr = INADDR_ANY;
		}
		if (g_Log) {
			Log_DNS << nodename << " blocked" << std::endl;
		}
	}
	return result;
}

// withthis you can replace other json response as well
int WINAPI winhttpreaddatahook (DWORD RetAddr,
								pfnwinhttpreaddata fnwinhttpreaddata,
								HINTERNET hRequest,
								LPVOID lpBuffer,
								DWORD dwNumberOfBytesToRead,
								LPDWORD lpdwNumberOfBytesRead)
{
	if (!fnwinhttpreaddata (hRequest,
							lpBuffer,
							dwNumberOfBytesToRead,
							lpdwNumberOfBytesRead)) {
		return false;
	}

	char* pdest = strstr ((LPSTR)lpBuffer, "{\"login_url");
	if (nullptr != pdest) {
		return true;
	}

	pdest = strstr ((LPSTR)lpBuffer, "{\"credentials");
	if (nullptr != pdest) {
		return true;
	}

	if (g_Log) {
		std::string data ((char*)lpBuffer, dwNumberOfBytesToRead);
		Log_WinHttp << data << std::endl;
	}
	if (g_WinHttpReadDataFix) return false;
	memset (lpBuffer, 0x0, dwNumberOfBytesToRead);
	//SecureZeroMemory (lpBuffer, dwNumberOfBytesToRead);
	return true;
}

