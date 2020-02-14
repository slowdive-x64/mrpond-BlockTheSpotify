#include "stdafx.h"

extern bool g_Log;
std::ofstream Log_DNS;

Adblock g_Adsblock;

Adblock::Adblock () {
	SecureZeroMemory (dns_ipaddr, sizeof (dns_ipaddr));
	pSrvList = nullptr;
	enable = false;
}

void Adblock::activate () {
	enable = true;
}
void Adblock::deactivate () {
	enable = false;
}

bool Adblock::init (const char* configFile) {
	if (!isEnable ()) {
		return false;
	}
	pSrvList = (PIP4_ARRAY)LocalAlloc (LPTR, sizeof (IP4_ARRAY));
	if (nullptr != pSrvList) {
		GetPrivateProfileString ("Config",
								 "AdGuardDNS_IP",
								 "176.103.130.134",
								 dns_ipaddr,
								 INET_ADDRSTRLEN,
								 configFile);
		// https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inetptonw
		if (1 == InetPton (AF_INET,
						   dns_ipaddr, // dns server ip
						   &pSrvList->AddrArray[0])) {
			pSrvList->AddrCount = 1;
			if (g_Log)
				Log_DNS << "AdGuard DNS Server - " << dns_ipaddr << std::endl;
			return true;
		}
		else {
			if (g_Log)
				Log_DNS << "AdGuard DNS Disable - InetPton "
				<< dns_ipaddr << " failed!" << std::endl;
		}
	}
	else {
		if (g_Log)
			Log_DNS << "AdGuard DNS Disable - "
			<< "pSrvList LocalAlloc failed!" << std::endl;
	}
	return false;
}

bool Adblock::isblock (const char* nodename) {

	// AdGuard DNS
	if (isEnable ()) {
		for (auto allow : whitelist) {
			if (0 == _stricmp (allow.c_str (), nodename))
				return false;
		}

		for (auto block : blacklist) {
			if (0 == _stricmp (block.c_str (), nodename))
				return true;
		}

		int result = adguardlookup (nodename);
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

int Adblock::adguardlookup (const char* nodename) {

	bool isBlock = false;
	DNS_STATUS dnsStatus = 0;
	PDNS_RECORD QueryResult = nullptr;

	dnsStatus = DnsQuery (nodename,
						  DNS_TYPE_A,
						  DNS_QUERY_WIRE_ONLY,
						  pSrvList,
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
			Log_DNS << "AdGuard DNS Error: " << dnsStatus
			<< " GLE: " << GetLastError () << std::endl;
		return -1;
	}

	if (isBlock) {
		return 1;
	}
	return 0;
}

void Adblock::destroy () {
	if (nullptr != pSrvList)
		LocalFree (pSrvList);
}

Adblock::~Adblock () {
}