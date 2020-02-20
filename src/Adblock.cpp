#include "stdafx.h"

extern bool g_Log;
extern bool g_Skip_wpad;

std::ofstream Log_DNS;

Adsblock g_Adsblock;

Adsblock::Adsblock () {
	isActive = false;
	pSrvList = (PIP4_ARRAY)LocalAlloc (LPTR, sizeof (IP4_ARRAY));
}

void Adsblock::setDNSIP (const char* ip) {

	if (nullptr != pSrvList) {

		// https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inetptonw
		if (1 == InetPton (AF_INET,
						   ip, // dns server ip
						   &pSrvList->AddrArray[0])) {
			pSrvList->AddrCount = 1;
			if (g_Log)
				Log_DNS << "AdGuard DNS Server - " << ip << std::endl;
			isActive = true;
		}
		else {
			if (g_Log)
				Log_DNS << "AdGuard DNS Disable - InetPton "
				<< ip << " failed!" << std::endl;
		}
	}
	else {
		if (g_Log)
			Log_DNS << __FUNCTION__
			<< " pSrvList LocalAlloc failed!" << std::endl;
	}
}

bool Adsblock::isblock (const char* nodename) {

	// Web Proxy Auto-Discovery (WPAD)
	if (0 == _stricmp (nodename, "wpad"))
		return g_Skip_wpad ? true : false;

	if (nullptr != strstr (nodename, "google"))
		return true;

	if (nullptr != strstr (nodename, "doubleclick."))
		return true;

	// AdGuard DNS
	if (isActive) {
		for (auto allow : whitelist) {
			if (0 == _stricmp (allow.c_str (), nodename))
				return false;
		}
		for (auto block : blacklist) {
			if (0 == _stricmp (block.c_str (), nodename))
				return true;
		}
		int result = lookup (nodename);
		if (1 == result) { // return 1 block
			blacklist.push_back (nodename); // add to blacklist
			return true;
		}
		else if (0 == result) { // return 0 not block
			whitelist.push_back (nodename); // add to whitelist
			return false;
		}
	}
	return false;
}

int Adsblock::lookup (const char* nodename) {

	bool isBlock = false;
	DNS_STATUS dnsStatus = 0;
	PDNS_RECORD QueryResult = nullptr;

	dnsStatus = DnsQuery (nodename,
						  DNS_TYPE_A,
						  DNS_QUERY_WIRE_ONLY,
						  Adsblock::pSrvList,
						  &QueryResult,
						  NULL); // Reserved

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
	else {
		if (g_Log)
			Log_DNS << __FUNCTION__ << " host:" << nodename << " status:" << dnsStatus
			<< " GLE: " << GetLastError () << std::endl;
		return -1;
	}

	if (isBlock) {
		return 1;
	}
	return 0;
}

void Adsblock::destroy () {
	if (nullptr != pSrvList) {
		LocalFree (pSrvList);
		pSrvList = nullptr;
	}
}

Adsblock::~Adsblock () {
}