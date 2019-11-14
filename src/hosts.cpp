#include "stdafx.h"

#ifdef _DEBUG
std::ofstream g_DNSLog;
std::ofstream g_GetAddrLog;
std::ofstream g_WinHttpLog;
#endif


// support.microsoft.com/en-us/help/831226/
// how-to-use-the-dnsquery-function-to-resolve-host-names-and-host-addres
// blogs.msdn.microsoft.com/winsdk/2014/12/17/
// dnsquery-sample-to-loop-through-multiple-ip-addresses/

bool adguard_dnsblock (const char* nodename) {
	DNS_STATUS dnsStatus;
	PDNS_RECORD QueryResult, p;
	PIP4_ARRAY pSrvList = NULL;
	char resolvedIP[INET6_ADDRSTRLEN]{};
	
	pSrvList = (PIP4_ARRAY)LocalAlloc (LPTR,
		sizeof (IP4_ARRAY));
	if (!pSrvList) return false;

	inet_pton (AF_INET,
		g_DNSIP, // dns server ip
		&pSrvList->AddrArray[0]);
	pSrvList->AddrCount = 1;

	dnsStatus = DnsQuery_A (nodename,
		DNS_TYPE_A,
		DNS_QUERY_WIRE_ONLY,
		pSrvList,
		&QueryResult,
		NULL); // Reserved
	// DnsQuery return 0 - success
	if (dnsStatus) return false;

	p = QueryResult;

	while (p) {
		inet_ntop (AF_INET,
			&p->Data.A.IpAddress,
			resolvedIP,
			sizeof (resolvedIP));
		if (_stricmp (resolvedIP, "0.0.0.0") == 0)
			return true; // AdGuard Block
		p = p->pNext;
	}

	if (pSrvList) LocalFree (pSrvList);
	DnsRecordListFree (QueryResult, DnsFreeRecordList);

#ifdef _DEBUG
	g_DNSLog << "Host: " << nodename
		<< " IP: " << resolvedIP << '\n';
#endif
	return false;
}

bool blacklist_host (const char* nodename) {
	for (size_t i = 0; i < sizeof (g_BlockList) / sizeof (g_BlockList[0]); i++)
	{
		if (strstr (nodename, g_BlockList[i]) != NULL)
			return true;
	}
	return false;
}

int WINAPI getaddrinfohook (DWORD RetAddr,
	pfngetaddrinfo fngetaddrinfo,
	const char* nodename,
	const char* servname,
	const struct addrinfo* hints,
	struct addrinfo** res)
{
	int ret = fngetaddrinfo (nodename, servname, hints, res);

	if (nodename == NULL) return ret;
	// Web Proxy Auto-Discovery (WPAD)
	if (_stricmp (nodename, "wpad") == 0) return ret; // experimental
	// change to WSANO_RECOVERY if you want to mess with proxy.

	// issue free here, in the case that
	// adguard dns can't be reach.
	if (blacklist_host (nodename)) return WSANO_RECOVERY;

	if (adguard_dnsblock (nodename)) return WSANO_RECOVERY;

#ifdef _DEBUG
	g_GetAddrLog << nodename << '\n';
#endif
	return ret;
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
	if (pdest != NULL) {
		return true;
	}

	pdest = strstr ((LPSTR)lpBuffer, "{\"credentials");
	if (pdest != NULL) {
		return true;
	}
#ifdef _DEBUG
	std::string data ((char*)lpBuffer, dwNumberOfBytesToRead);
	g_WinHttpLog << "Byte count: " << dwNumberOfBytesToRead << '\n';
	g_WinHttpLog << data << '\n';
#endif
	SecureZeroMemory (lpBuffer, dwNumberOfBytesToRead);
	return true;
}