#include "stdafx.h"

bool g_UseAdGuard = true;
bool g_Log = false;
bool g_Skip_wpad = false;
bool g_WinHttpReadDataFix = false;

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
	char resolvedIP[INET6_ADDRSTRLEN]{};
	if (!g_UseAdGuard) return false;

	pSrvList = (PIP4_ARRAY)LocalAlloc (LPTR, sizeof (IP4_ARRAY));

	if (pSrvList) {
		// https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inetptonw
		if (1 == InetPtonA (AF_INET,
							"176.103.130.134", // dns server ip
							&pSrvList->AddrArray[0])) {
			// "Family protection"
			// adguard.com/en/adguard-dns/overview.html 
			pSrvList->AddrCount = 1;

			dnsStatus = DnsQuery_A (nodename,
									DNS_TYPE_A,
									DNS_QUERY_WIRE_ONLY,
									pSrvList,
									&QueryResult,
									NULL); // Reserved
			if (0 == dnsStatus) {
				if (QueryResult) {
					for (auto p = QueryResult; p; p = p->pNext) {
						// 0.0.0.0
						inet_ntop (AF_INET,
								   &p->Data.A.IpAddress,
								   resolvedIP,
								   sizeof (resolvedIP));
						if (_stricmp (resolvedIP, "0.0.0.0") == 0)
							isBlock = true; // AdGuard Block		
					}
					DnsRecordListFree (QueryResult, DnsFreeRecordList);
				} // QueryResult
			} // dnsStatus
		} // inet_pton
		LocalFree (pSrvList);
	} // pSrvList
	if (g_Log && isBlock) {
		Log_DNS << nodename << " blocked" << '\n';
	}
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
	if (0 == result) { // GetAddrInfo return 0 on success
		if (NULL != strstr (nodename, "google"))
			return WSANO_RECOVERY;

		// Web Proxy Auto-Discovery (WPAD)
		if (0 == _stricmp (nodename, "wpad"))
			return g_Skip_wpad ? WSANO_RECOVERY : result;

		// AdGuard DNS
		if (adguard_dnsblock (nodename))
			return WSANO_RECOVERY;

		if (g_Log) {
			Log_GetAddr << nodename << '\n';
		}
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
	if (g_WinHttpReadDataFix) return false;

	SecureZeroMemory (lpBuffer, dwNumberOfBytesToRead);
	return true;
}

