#include "stdafx.h"

bool g_Log = false;
bool g_Skip_wpad = false;

std::wofstream Log;

bool is_blockhost (const char* nodename) {

	std::string nnodename (nodename);
	// Web Proxy Auto-Discovery (WPAD)
	if (0 == nnodename.compare("wpad"))
		return g_Skip_wpad ? true : false;
	
	if (std::string::npos != nnodename.find ("google"))
		return true;
	if (std::string::npos != nnodename.find ("doubleclick"))
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
			Log << "blocked - getaddrinfo " << nodename << std::endl;
		}
	}

	return result;
}

bool is_blockrequest (LPCWSTR pwszObjectName) {
	std::wstring npwszObjectName (pwszObjectName);
	if (std::wstring::npos != npwszObjectName.compare (L"/ad-logic/"))
		return true;
	if (std::wstring::npos != npwszObjectName.compare (L"/ads/"))
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
	if (is_blockrequest (pwszObjectName)) {
		if (g_Log) {
			Log << "blocked - WinHttpOpenRequest " << pwszVerb << " " << pwszObjectName << std::endl;
		}
		return 0;
	}
	return fnwinhttpopenrequest (hConnect,
								 pwszVerb,
								 pwszObjectName,
								 pwszVersion,
								 pwszReferrer,
								 ppwszAcceptTypes,
								 dwFlags);
}
