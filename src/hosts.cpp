#include "stdafx.h"

// Web Proxy Auto-Discovery (WPAD)
bool g_Skip_wpad = false;
// Logging system
std::wofstream Log;

// check if ads hostname
bool is_blockhost (const char* nodename) {

	std::string nnodename (nodename);

	if (0 == nnodename.compare ("wpad"))
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
	// future/async
	auto isblock = std::async (std::launch::async, is_blockhost, nodename);
	// GetAddrInfo return 0 on success
	auto result = fngetaddrinfo (nodename,
								 servname,
								 hints,
								 res);

	if (0 == result && isblock.get ()) {
		for (auto ptr = *res; nullptr != ptr; ptr = ptr->ai_next) {
			auto ipv4 = (struct sockaddr_in*)ptr->ai_addr;
			//memset (&ipv4->sin_addr.S_un.S_addr, 0x0, sizeof ULONG);
			ipv4->sin_addr.S_un.S_addr = INADDR_ANY;
		}
		if (Log.is_open ())
			Log << "blocked - " << nodename << std::endl;
	}

	return result;
}

// block http request base on URI
bool is_blockrequest (const std::wstring& npwszVerb,const std::wstring& npwszObjectName) {
	if (0 == npwszVerb.compare (L"POST")) {
		if (0 == npwszObjectName.compare (L"/ad-logic/state/config")) {
			return true;
		}
	}
	if (0 == npwszVerb.compare (L"GET")) {
		if (0 == npwszObjectName.compare (0, 5, L"/ads/", 5)) {
			return true;
		}
	}
	return false;
}

// spclient.wg.spotify.com
int WINAPI winhttpopenrequesthook (DWORD RetAddr,
								   pfnwinhttpopenrequest fnwinhttpopenrequest,
								   HINTERNET hConnect,
								   LPCWSTR pwszVerb, // HTTP Method
								   LPCWSTR pwszObjectName, // URI
								   LPCWSTR pwszVersion,
								   LPCWSTR pwszReferrer,
								   LPCWSTR* ppwszAcceptTypes,
								   DWORD dwFlags)
{
	std::wstring npwszVerb (pwszVerb);
	std::wstring npwszObjectName (pwszObjectName);

	if (is_blockrequest (npwszVerb, npwszObjectName)) {
		if (Log.is_open ()) {
			auto pos = npwszObjectName.find_first_of (L"="); // check if had ?payload=
			if (pos != std::string::npos) {
				npwszObjectName.erase (pos + 1); // trim original base64 payload out
				npwszObjectName.append (L"c2VjcmV0"); // append "secret"
			}
			Log << "blocked - " << npwszVerb << " " << npwszObjectName << std::endl;
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
