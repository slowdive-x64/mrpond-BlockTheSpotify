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

	if (g_Log) {
		std::wstring wvb (pwszVerb);
		std::wstring wobj (pwszObjectName);
		std::string vb (wvb.begin(), wvb.end());
		std::string obj (wobj.begin(), wobj.end());
		Log_WinHttp << __FUNCTION__ << " " << vb << " " << obj << std::endl;
	}

	auto pdest = wcsstr (pwszObjectName, L"/ad-logic/");
	if (nullptr != pdest)
		return 0;

	pdest = wcsstr (pwszObjectName, L"/ads/");
	if (nullptr != pdest)
		return 0;

	return fnwinhttpopenrequest (hConnect,
								 pwszVerb,
								 pwszObjectName,
								 pwszVersion,
								 pwszReferrer,
								 ppwszAcceptTypes,
								 dwFlags);

}

bool check_pod (LPVOID lpBuffer)
{
	char* pdest = strstr ((LPSTR)lpBuffer, "{\"pod");
	if (nullptr != pdest) {
		//"pod" -> "xod"
		pdest += 2;
		*pdest = 'x';
		return true;
	}
	return false;
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

	//auto future_pod = std::async (std::launch::async, check_pod, lpBuffer);

	if (g_Log) {
		std::string data ((char*)lpBuffer, dwNumberOfBytesToRead);
		Log_WinHttp << __FUNCTION__ << " " << data << std::endl;
	}

	//if (future_pod.get ()) {
	if (check_pod (lpBuffer)) {
		//memset (lpBuffer, 0x0, dwNumberOfBytesToRead);
		return g_WinHttpReadDataFix ? false : true;
	}

	return true;
}

