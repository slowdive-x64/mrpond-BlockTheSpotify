#include "stdafx.h"

bool g_UseAdGuard = true;
bool g_Log = false;
bool g_Skip_wpad = false;
bool g_WinHttpReadDataFix = false;

std::ofstream Log_DNS;
std::ofstream Log_GetAddr;
std::ofstream Log_WinHttp;
extern PIP4_ARRAY pSrvList;

std::vector<std::string> blacklist;
std::vector<std::string> whitelist;

// support.microsoft.com/en-us/help/831226/
// how-to-use-the-dnsquery-function-to-resolve-host-names-and-host-addres
// blogs.msdn.microsoft.com/winsdk/2014/12/17/
// dnsquery-sample-to-loop-through-multiple-ip-addresses/

bool adguard_dnsblock (const char* nodename) {
	DNS_STATUS dnsStatus;
	PDNS_RECORD QueryResult;
	bool isBlock = false;
	static int fail_count = 0;
	if (!g_UseAdGuard) return false;

	if (fail_count > 5) {
		if (g_Log) {
			Log_DNS << "AdGuard DNS lookup disable! fail resolve > 5 times" << std::endl;
		}
		g_UseAdGuard = false;
		return false;
	}

	dnsStatus = DnsQuery (nodename,
						  DNS_TYPE_A,
						  DNS_QUERY_WIRE_ONLY,
						  pSrvList,
						  &QueryResult,
						  NULL); // Reserved

	if (0 == dnsStatus) {
		if (QueryResult) {
			for (auto p = QueryResult; p; p = p->pNext) {
				if (0 == p->Data.A.IpAddress) {
					isBlock = true; // AdGuard Block
					break;	// no more processing
				}
			}
			DnsRecordListFree (QueryResult, DnsFreeRecordList);
		} // QueryResult
	}
	else { // dnsStatus
		fail_count++;
	}
	if (g_Log && isBlock) {
		Log_DNS << nodename << " blocked" << std::endl;
	}

	return isBlock;
}

bool checkBlock (const char* nodename) {

	if (nullptr != strstr (nodename, "google"))
		return true;

	for (auto allow : whitelist) {
		if (0 == _stricmp (allow.c_str (), nodename))
			return false;
	}

	for (auto block : blacklist) {
		if (0 == _stricmp (block.c_str (), nodename))
			return true;
	}

	// AdGuard DNS
	if (adguard_dnsblock (nodename)) {
		blacklist.push_back (nodename); // add to blacklist
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
	auto result = fngetaddrinfo (nodename,
								 servname,
								 hints,
								 res);

	addrinfo* p = *res;
	if (0 == result) { // GetAddrInfo return 0 on success
		if (nullptr != p && checkBlock (nodename)) {
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
			InetPton (AF_INET, "0.0.0.0", &(ipv4->sin_addr));
			if (g_Log) {
				Log_GetAddr << nodename << " blocked" << std::endl;
			}
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
	if (nullptr != pdest) {
		return true;
	}

	pdest = strstr ((LPSTR)lpBuffer, "{\"credentials");
	if (nullptr != pdest) {
		return true;
	}

	if (g_Log) {
		std::string data ((char*)lpBuffer, dwNumberOfBytesToRead);
		Log_WinHttp << data << std::endl;
	}
	if (g_WinHttpReadDataFix) return false;

	std::fill ((LPSTR)lpBuffer, (LPSTR)lpBuffer + dwNumberOfBytesToRead, 0x20);
	return true;
}

