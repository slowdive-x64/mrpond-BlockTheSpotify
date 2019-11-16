#ifndef _HOSTS_H
#define _HOSTS_H

// "Family protection"
// adguard.com/en/adguard-dns/overview.html 
static const char* g_DNSIP = "176.103.130.134";

typedef int (__stdcall* pfngetaddrinfo)(const char* nodename,
										const char* servname,
										const struct addrinfo* hints,
										struct addrinfo** res);


typedef int (__stdcall* pfnwinhttpreaddata) (HINTERNET hRequest,
											 LPVOID lpBuffer,
											 DWORD dwNumberOfBytesToRead,
											 LPDWORD lpdwNumberOfBytesRead);


int WINAPI getaddrinfohook (DWORD RetAddr,
							pfngetaddrinfo fngetaddrinfo,
							const char* nodename,
							const char* servname,
							const struct addrinfo* hints,
							struct addrinfo** res);

int WINAPI winhttpreaddatahook (DWORD RetAddr,
								pfnwinhttpreaddata fnwinhttpreaddata,
								HINTERNET hRequest,
								LPVOID lpBuffer,
								DWORD dwNumberOfBytesToRead,
								LPDWORD lpdwNumberOfBytesRead);

#endif /* _HOSTS_H */
