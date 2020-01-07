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

int adguard_dnsblock (const char* nodename) {
	DNS_STATUS dnsStatus;
	PDNS_RECORD QueryResult;
	bool isBlock = false;

	dnsStatus = DnsQuery (nodename,
						  DNS_TYPE_A,
						  DNS_QUERY_WIRE_ONLY,
						  pSrvList,
						  &QueryResult,
						  NULL); // Reserved

	if (0 != dnsStatus) return -1;

	if (0 == dnsStatus) {
		if (QueryResult) {
			for (auto p = QueryResult; p; p = p->pNext) {
				if (INADDR_ANY == p->Data.A.IpAddress) {
					isBlock = true; // AdGuard Block
					break;	// no more processing
				}
			}
			DnsRecordListFree (QueryResult, DnsFreeRecordList);
		} // QueryResult
	}

	if (isBlock) {
		return 1;
	}
	return 0;
}

bool checkBlock (const char* nodename) {

	if (nullptr != strstr (nodename, "google"))
		return true;

	// AdGuard DNS
	if (g_UseAdGuard) {
		for (auto allow : whitelist) {
			if (0 == _stricmp (allow.c_str (), nodename))
				return false;
		}

		for (auto block : blacklist) {
			if (0 == _stricmp (block.c_str (), nodename))
				return true;
		}

		int result = adguard_dnsblock (nodename);
		if (1 == result) {
			blacklist.push_back (nodename); // add to blacklist
			return true;
		}
		else if (0 == result) {
			whitelist.push_back (nodename); // add to whitelist
		}
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

	if (0 == result) { // GetAddrInfo return 0 on success
		// Web Proxy Auto-Discovery (WPAD)
		if (g_Skip_wpad && 0 == _stricmp (nodename, "wpad"))
			return WSAHOST_NOT_FOUND;
		if (checkBlock (nodename)) {
			for (auto ptr = *res; nullptr != ptr; ptr = ptr->ai_next) {
				auto ipv4 = (struct sockaddr_in*)ptr->ai_addr;
				ipv4->sin_addr.S_un.S_addr = INADDR_ANY;
			}
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

