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
	//"spclient.wg.spotify.com"
	//POST
	//L"/ad-logic/state/config"
	//L"/playlist-publish/v1/subscription/playlist/xxxxx"
	//GET
	//L"/ads/v2/config?payload=xxxxx"
	//L"/ads/v1/ads/leaderboard?payload=xxxxx
	//HEADER
	//L"User-Agent: Spotify/111500448 Win32/0 (PC laptop)"
	//L"Authorization: Bearer xxxxx"

	if (wcsstr (pwszObjectName, L"/playlist-publish/") != 0) {
		return fnwinhttpopenrequest (hConnect,
			pwszVerb,
			pwszObjectName,
			pwszVersion,
			pwszReferrer,
			ppwszAcceptTypes,
			dwFlags);
	}
	return NULL;
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
		if (strstr (nodename, blockhost[i]) != 0)
			return WSANO_RECOVERY;
	}
	return fngetaddrinfo (nodename, servname, hints, res);
}
