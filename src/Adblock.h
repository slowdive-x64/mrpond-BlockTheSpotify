#pragma once

// support.microsoft.com/en-us/help/831226/
// how-to-use-the-dnsquery-function-to-resolve-host-names-and-host-addres
// blogs.msdn.microsoft.com/winsdk/2014/12/17/
// dnsquery-sample-to-loop-through-multiple-ip-addresses/

class Adblock {
public:
	Adblock::Adblock ();
	Adblock::~Adblock ();
	bool init (const char* configFile);
	bool isblock (const char* nodename);
	bool isEnable () { return enable; };
	void destroy ();
	void activate ();
	void deactivate ();

private:
	bool enable;
	PIP4_ARRAY pSrvList;
	char dns_ipaddr[INET_ADDRSTRLEN];
	std::vector<std::string> blacklist;
	std::vector<std::string> whitelist;
	int adguardlookup (const char* nodename);
};