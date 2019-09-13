#include "stdafx.h"

//HINTERNET
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
	return NULL;
	//if ((wcscmp (pwszVerb, L"GET") == 0) || (wcscmp (pwszVerb, L"POST") == 0))
		//return NULL;

	//return fnwinhttpopenrequest (hConnect,
	//	pwszVerb,
	//	pwszObjectName,
	//	pwszVersion,
	//	pwszReferrer,
	//	ppwszAcceptTypes,
	//	dwFlags);
}

int WINAPI getaddrinfohook (DWORD RetAddr, 
	pfngetaddrinfo fngetaddrinfo, 
	const char* nodename, 
	const char* servname, 
	const struct addrinfo* hints, 
	struct addrinfo** res)
{
	for (size_t i = 0; i < sizeof (blockhost) / sizeof (blockhost[0]); i++)
	{
		if (strstr (nodename, blockhost[i]) != NULL)
			return WSANO_RECOVERY;
	}
	return fngetaddrinfo (nodename, servname, hints, res);
}
