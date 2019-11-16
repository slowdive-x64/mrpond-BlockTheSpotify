#include "stdafx.h"

bool g_UseAdGuard = true;
bool g_Log = false;
bool g_Skip_wpad = false;

std::ofstream Log_DNS;
std::ofstream Log_GetAddr;
std::ofstream Log_WinHttp;

// support.microsoft.com/en-us/help/831226/
// how-to-use-the-dnsquery-function-to-resolve-host-names-and-host-addres
// blogs.msdn.microsoft.com/winsdk/2014/12/17/
// dnsquery-sample-to-loop-through-multiple-ip-addresses/

bool adguard_dnsblock (const char* nodename) {
	DNS_STATUS dnsStatus;
	PDNS_RECORD QueryResult;
	PIP4_ARRAY pSrvList = NULL;
	bool isBlock = false;

	pSrvList = (PIP4_ARRAY)LocalAlloc (LPTR, sizeof (IP4_ARRAY));

	while (pSrvList) {

		if (inet_pton (AF_INET,
					   g_DNSIP, // dns server ip
					   &pSrvList->AddrArray[0]) != 1)
			break;
		pSrvList->AddrCount = 1;

		dnsStatus = DnsQuery_A (nodename,
								DNS_TYPE_A,
								DNS_QUERY_WIRE_ONLY,
								pSrvList,
								&QueryResult,
								NULL); // Reserved

		// DnsQuery return 0 - success
		if (dnsStatus) {
			if (g_Log) {
				Log_DNS << nodename << " DNS status: " << dnsStatus
					<< " GLE: " << GetLastError () << '\n';
			}
			break;
		}

		if (!QueryResult) break;

		for (auto p = QueryResult; p; p = p->pNext) {
			// 0.0.0.0
			if (p->Data.A.IpAddress == 0) isBlock = true; // AdGuard Block
		}
		DnsRecordListFree (QueryResult, DnsFreeRecordList);
		break;
	}

	if (pSrvList) LocalFree (pSrvList);
	return isBlock;
}

int WINAPI getaddrinfohook (DWORD RetAddr,
							pfngetaddrinfo fngetaddrinfo,
							const char* nodename,
							const char* servname,
							const struct addrinfo* hints,
							struct addrinfo** res)
{
	auto result = fngetaddrinfo (nodename,
								 servname,
								 hints,
								 res);

	// GetAddrInfo return 0 on success
	if (result != 0) return result;

	// Web Proxy Auto-Discovery (WPAD)
	if (_stricmp (nodename, "wpad") == 0)
		return g_Skip_wpad ? WSANO_RECOVERY : result;

	if (strstr (nodename, "google") != NULL)
		return WSANO_RECOVERY;

	// AdGuard DNS
	if (g_UseAdGuard) {
		if (adguard_dnsblock (nodename))
			return WSANO_RECOVERY;
	}

	if (g_Log) {
		Log_GetAddr << nodename << '\n';
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
	if (pdest != NULL) {
		return true;
	}

	pdest = strstr ((LPSTR)lpBuffer, "{\"credentials");
	if (pdest != NULL) {
		return true;
	}
	if (g_Log) {
		std::string data ((char*)lpBuffer, dwNumberOfBytesToRead);
		Log_WinHttp << "Byte count: " << dwNumberOfBytesToRead << '\n';
		Log_WinHttp << data << '\n';
	}
	SecureZeroMemory (lpBuffer, dwNumberOfBytesToRead);
	return true;
}

