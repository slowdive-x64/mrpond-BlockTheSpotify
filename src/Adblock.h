#pragma once

// support.microsoft.com/en-us/help/831226/
// how-to-use-the-dnsquery-function-to-resolve-host-names-and-host-addres
// blogs.msdn.microsoft.com/winsdk/2014/12/17/
// dnsquery-sample-to-loop-through-multiple-ip-addresses/

class Adsblock {
public:
	Adsblock::Adsblock ();
	Adsblock::~Adsblock ();
	void setDNSIP (const char* ip);
	bool isblock (const char* nodename);
	void destroy ();

private:
	PIP4_ARRAY pSrvList;
	bool isActive;
	std::vector<std::string> blacklist;
	std::vector<std::string> whitelist;
	int lookup (const char* nodename);
};