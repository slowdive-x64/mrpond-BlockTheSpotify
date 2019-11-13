#include "stdafx.h"

/*
	support.microsoft.com/en-us/help/831226/
	how-to-use-the-dnsquery-function-to-resolve-host-names-and-host-addres
	blogs.msdn.microsoft.com/winsdk/2014/12/17/
	dnsquery-sample-to-loop-through-multiple-ip-addresses/
*/
bool adguard_checkblock (const char* nodename) {

	DNS_STATUS dnsStatus;
	PDNS_RECORD result = NULL;
	PIP4_ARRAY pSrvList = NULL;
	const char* dns_server = "176.103.130.134"; // "Family protection"
	// adguard.com/en/adguard-dns/overview.html
	char resolvedIP[INET6_ADDRSTRLEN]{};

	pSrvList = (PIP4_ARRAY)LocalAlloc (LPTR, sizeof (IP4_ARRAY));
	if (!pSrvList) return false;

	inet_pton (AF_INET,
		dns_server,
		&pSrvList->AddrArray[0]);
	pSrvList->AddrCount = 1;

	dnsStatus = DnsQuery_A (nodename,
		DNS_TYPE_A,
		DNS_QUERY_WIRE_ONLY,
		pSrvList, // Documented as reserved, but can take a PIP4_ARRAY for the DNS server
		&result,
		NULL); // Reserved

	if (dnsStatus) return false;

	if (result != NULL) {
		inet_ntop (AF_INET,
			&result->Data.A.IpAddress,
			resolvedIP,
			sizeof (resolvedIP));
	}

	if (pSrvList) LocalFree (pSrvList);

	DnsRecordListFree (result,
		DnsFreeRecordList);

#ifdef _DEBUG
	// log the missing
	std::ofstream logfile;
	logfile.open ("dnslog.txt", std::ios::out | std::ios::app);
	logfile << "Host: " << nodename << " IP: " << resolvedIP << '\n';
	logfile.close ();
#endif
	if (_stricmp (resolvedIP, "0.0.0.0") == 0) {
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

	if (_stricmp (nodename, "wpad") == 0)
		return ret;

	if (adguard_checkblock (nodename)) {
		return WSANO_RECOVERY;
	}
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
	std::ofstream logfile;
	logfile.open ("datalog.txt", std::ios::out | std::ios::app);
	logfile << "Byte count: " << dwNumberOfBytesToRead << '\n';
	logfile << data << '\n';
	logfile.close ();
#else
	SecureZeroMemory (lpBuffer, dwNumberOfBytesToRead);
#endif
	return true;
}