#include "stdafx.h"

int WINAPI getaddrinfohook (DWORD RetAddr,
	pfngetaddrinfo fngetaddrinfo,
	const char* nodename,
	const char* servname,
	const struct addrinfo* hints,
	struct addrinfo** res)
{
	int ret = fngetaddrinfo (nodename, servname, hints, res);
	for (size_t i = 0; i < sizeof (whitelist) / sizeof (whitelist[0]); i++)
	{
		if (strstr (nodename, whitelist[i]) != NULL)
			return ret;
	}
	for (size_t i = 0; i < sizeof (blockhost) / sizeof (blockhost[0]); i++)
	{
		if (strstr (nodename, blockhost[i]) != NULL)
			return WSANO_RECOVERY;
	}
#ifdef _DEBUG
	// log the missing
	std::ofstream logfile;
	logfile.open ("hostlog.txt", std::ios::out | std::ios::app);
	logfile << nodename << '\n';
	logfile.close ();
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