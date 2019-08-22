#include "stdafx.h"
#include "hosts.h"

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
	//wcscmp (pwszVerb, L"POST") == 0
	if (wcscmp (pwszObjectName, L"/ad-logic/flashpoint") == 0)
		return NULL;
	if (wcscmp (pwszObjectName, L"/ads") == 0)
		return NULL;
	//if (wcscmp (pwszObjectName, L"/ads/v1/ads/leaderboard") == 0)
		//return NULL;

	return fnwinhttpopenrequest (hConnect,
		pwszVerb,
		pwszObjectName,
		pwszVersion,
		pwszReferrer,
		ppwszAcceptTypes,
		dwFlags);
}

int WINAPI connecthook (DWORD RetAddr, 
	pfnconnect fnconnect, 
	SOCKET s, 
	const struct sockaddr* name, 
	int namelen)
{
	return fnconnect (s, name, namelen);

	//for future whitelist or blacklist ip
	//struct sockaddr_in* sa = (struct sockaddr_in*)name;
	//if (sa->sin_family != AF_INET) // check only IPv4
		//return fnconnect (s, name, namelen);


	//char* allowip = "151.101.8.246"; // Image
	//char ipstr[INET_ADDRSTRLEN];
	//inet_ntop (sa->sin_family, &(sa->sin_addr), ipstr, INET_ADDRSTRLEN);
	//if (_stricmp (ipstr, allowip) == 0) {
		//return SOCKET_ERROR;
	//}

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
