#include "stdafx.h"

bool g_Log = false;
bool g_Skip_wpad = false;

std::wofstream Log;

bool is_blockhost (const char* nodename) {

	// Web Proxy Auto-Discovery (WPAD)
	if (0 == _stricmp (nodename, "wpad"))
		return g_Skip_wpad ? true : false;

	if (nullptr != strstr (nodename, "google"))
		return true;

	if (nullptr != strstr (nodename, "doubleclick."))
		return true;

	return false;
}


int WINAPI getaddrinfohook (DWORD RetAddr,
							pfngetaddrinfo fngetaddrinfo,
							const char* nodename,
							const char* servname,
							const struct addrinfo* hints,
							struct addrinfo** res)
{
	auto lookup = std::async (std::launch::async, is_blockhost, nodename);
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
			Log << "getaddrinfo " << nodename << " blocked" << std::endl;
		}
	}
	return result;
}

bool is_blockrequest (LPCWSTR pwszObjectName) {

	auto pdest = wcsstr (pwszObjectName, L"/ad-logic/");
	if (nullptr != pdest)
		return true;

	pdest = wcsstr (pwszObjectName, L"/ads/");
	if (nullptr != pdest)
		return true;

	return false;
}

int WINAPI winhttpopenrequesthook (DWORD RetAddr,
								   pfnwinhttpopenrequest fnwinhttpopenrequest,
								   HINTERNET hConnect,
								   LPCWSTR pwszVerb,
								   LPCWSTR pwszObjectName,
								   LPCWSTR pwszVersion,
								   LPCWSTR pwszReferrer,
								   LPCWSTR* ppwszAcceptTypes,
								   DWORD dwFlags)
{
	//"spclient.wg.spotify.com"

	if (g_Log) {
		Log << "WinHttpOpenRequest " << pwszVerb << " " << pwszObjectName << std::endl;
	}

	if (is_blockrequest(pwszObjectName))
		return 0;

	return fnwinhttpopenrequest (hConnect,
								 pwszVerb,
								 pwszObjectName,
								 pwszVersion,
								 pwszReferrer,
								 ppwszAcceptTypes,
								 dwFlags);
}
