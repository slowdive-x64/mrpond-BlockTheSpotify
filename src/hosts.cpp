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
	//L"/ad-logic/flashpoint"	/* this is ad between song/popup */
	//L"/playlist-publish/v1/subscription/playlist/xxxxx" /* sent when change playlist */
	//GET /* payload are base64 */
	//L"/ads/v2/config?payload=xxxxx"
	//L"/ads/v1/ads/hpto?payload=xxxxx"	
	//L"/ads/v1/ads/leaderboard?payload=xxxxx
	//L"/pagead/conversion/?ai=xxxxx"
	//L"/monitoring?pload=xxxxx"
	//L"/pcs/view?xai=xxxxx"
	//HEADER /* add into every request */
	//L"User-Agent: Spotify/111500448 Win32/0 (PC laptop)"
	//L"Authorization: Bearer xxxxx"

	// should whitelist instead of blacklist.. no time yet.
	for (size_t i = 0; i < sizeof (blockrequest) / sizeof (blockrequest[0]); i++)
	{
		if (wcsstr (pwszObjectName, blockrequest[i]) != 0)
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

int WINAPI getaddrinfohook (DWORD RetAddr,
	pfngetaddrinfo fngetaddrinfo,
	const char* nodename,
	const char* servname,
	const struct addrinfo* hints,
	struct addrinfo** res)
{
	for (size_t i = 0; i < sizeof (whitelist) / sizeof (whitelist[0]); i++)
	{
		if (strstr (nodename, whitelist[i]) != 0)
			return fngetaddrinfo (nodename, servname, hints, res);
	}
	for (size_t i = 0; i < sizeof (blockhost) / sizeof (blockhost[0]); i++)
	{
		if (strstr (nodename, blockhost[i]) != 0)
			return WSANO_RECOVERY;
	}
	return fngetaddrinfo (nodename, servname, hints, res);
}

int WINAPI winhttpsendrequesthook (DWORD RetAddr,
	pfnwinhttpsendrequest fnwinhttpsendrequest,
	HINTERNET hRequest,
	LPCWSTR pwszHeaders,
	DWORD dwHeadersLength,
	LPVOID lpOptional,
	DWORD dwOptionalLength,
	DWORD dwTotalLength,
	DWORD_PTR dwContext)
{
	return 1;	// return success but nothing sent
}

int WINAPI winhttpwritedatahook (DWORD RetAddr,
	pfnwinhttpwritedata fnwinhttpwritedata,
	HINTERNET hRequest,
	LPCVOID lpBuffer,
	DWORD dwNumberOfBytesToWrite,
	LPDWORD lpdwNumberOfBytesWritten)
{
	return 1;	// return success but nothing write
}

int WINAPI winhttpreceiveresponsehook (DWORD RetAddr,
	pfnwinhttpreceiveresponse fnwinhttpreceiveresponse,
	HINTERNET hRequest,
	LPVOID lpReserved)
{
	return 1;	// return success but nothing receive
}

int WINAPI winhttpquerydataavailablehook (DWORD RetAddr,
	pfnwinhttpquerydataavailable fnwinhttpquerydataavailable,
	HINTERNET hRequest,
	LPDWORD lpdwNumberOfBytesAvailable)
{
	//lpdwNumberOfBytesAvailable = 0;
	return 1;	// return success with 0 byte to read
}

int WINAPI winhttpsetstatuscallbackhook (DWORD RetAddr,
	pfnwinhttpsetstatuscallback fnwinhttpsetstatuscallback,
	HINTERNET hInternet,
	WINHTTP_STATUS_CALLBACK lpfnInternetCallback,
	DWORD dwNotificationFlags,
	DWORD_PTR dwReserved)
{
	// lpfnInternetCallback - Set this to NULL to remove the existing callback function.
	return fnwinhttpsetstatuscallback (hInternet, NULL, dwNotificationFlags, NULL);
}
