#pragma once

typedef int (WSAAPI* _getaddrinfo)(
	_In_opt_	const char* pNodeName,
	_In_opt_	const char* pServiceName,
	_In_opt_	const struct addrinfo* pHints,
	_Out_		struct addrinfo** ppResult
	);

int WSAAPI getaddrinfo_hook (
	_In_opt_	const char* pNodeName,
	_In_opt_	const char* pServiceName,
	_In_opt_	const struct addrinfo* pHints,
	_Out_		struct addrinfo** ppResult);

//#endif /* _HOSTS_H */
