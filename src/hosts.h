#ifndef _HOSTS_H
#define _HOSTS_H

typedef int (__stdcall* pfngetaddrinfo)(const char* nodename,
	const char* servname,
	const struct addrinfo* hints,
	struct addrinfo** res);

typedef int (__stdcall* pfnwinhttpopenrequest)(HINTERNET hConnect,
	LPCWSTR pwszVerb,
	LPCWSTR pwszObjectName,
	LPCWSTR pwszVersion,
	LPCWSTR pwszReferrer,
	LPCWSTR* ppwszAcceptTypes,
	DWORD dwFlags);

typedef int (__stdcall* pfnwinhttpquerydataavailable)(HINTERNET hRequest,
	LPDWORD lpdwNumberOfBytesAvailable);

typedef int (__stdcall* pfnwinhttpsendrequest)(HINTERNET hRequest,
	LPCWSTR pwszHeaders,
	DWORD dwHeadersLength,
	LPVOID lpOptional,
	DWORD dwOptionalLength,
	DWORD dwTotalLength,
	DWORD_PTR dwContext);

typedef int (__stdcall* pfnwinhttpwritedata) (HINTERNET hRequest,
	LPCVOID lpBuffer,
	DWORD dwNumberOfBytesToWrite,
	LPDWORD lpdwNumberOfBytesWritten);

typedef int (__stdcall* pfnwinhttpreceiveresponse) (HINTERNET hRequest,
	LPVOID lpReserved);

typedef int (__stdcall* pfnwinhttpsetstatuscallback) (HINTERNET hInternet,
	WINHTTP_STATUS_CALLBACK lpfnInternetCallback,
	DWORD dwNotificationFlags,
	DWORD_PTR dwReserved);

typedef int (__stdcall* pfnwinhttpreaddata) (HINTERNET hRequest,
	LPVOID lpBuffer,
	DWORD dwNumberOfBytesToRead,
	LPDWORD lpdwNumberOfBytesRead);

int WINAPI winhttpopenrequesthook (DWORD RetAddr,
	pfnwinhttpopenrequest fnwinhttpopenrequest,
	HINTERNET hConnect,
	LPCWSTR pwszVerb,
	LPCWSTR pwszObjectName,
	LPCWSTR pwszVersion,
	LPCWSTR pwszReferrer,
	LPCWSTR* ppwszAcceptTypes,
	DWORD dwFlags
);

int WINAPI getaddrinfohook (DWORD RetAddr,
	pfngetaddrinfo fngetaddrinfo,
	const char* nodename,
	const char* servname,
	const struct addrinfo* hints,
	struct addrinfo** res);

int WINAPI winhttpquerydataavailablehook (DWORD RetAddr,
	pfnwinhttpquerydataavailable fnwinhttpquerydataavailable,
	HINTERNET hRequest,
	LPDWORD lpdwNumberOfBytesAvailable);

int WINAPI winhttpsendrequesthook (DWORD RetAddr,
	pfnwinhttpsendrequest fnwinhttpsendrequest,
	HINTERNET hRequest,
	LPCWSTR pwszHeaders,
	DWORD dwHeadersLength,
	LPVOID lpOptional,
	DWORD dwOptionalLength,
	DWORD dwTotalLength,
	DWORD_PTR dwContext);

int WINAPI winhttpwritedatahook (DWORD RetAddr,
	pfnwinhttpwritedata fnwinhttpwritedata,
	HINTERNET hRequest,
	LPCVOID lpBuffer,
	DWORD dwNumberOfBytesToWrite,
	LPDWORD lpdwNumberOfBytesWritten);

int WINAPI winhttpreceiveresponsehook (DWORD RetAddr,
	pfnwinhttpreceiveresponse fnwinhttpreceiveresponse,
	HINTERNET hRequest,
	LPVOID lpReserved);

int WINAPI winhttpsetstatuscallbackhook (DWORD RetAddr,
	pfnwinhttpsetstatuscallback fnwinhttpsetstatuscallback,
	HINTERNET hInternet,
	WINHTTP_STATUS_CALLBACK lpfnInternetCallback,
	DWORD dwNotificationFlags,
	DWORD_PTR dwReserved);

int WINAPI winhttpreaddatahook (DWORD RetAddr,
	pfnwinhttpreaddata fnwinhttpreaddata,
	HINTERNET hRequest,
	LPVOID lpBuffer,
	DWORD dwNumberOfBytesToRead,
	LPDWORD lpdwNumberOfBytesRead);

static const char* blockhost[] = {
	// fork this if you found more...  I'll check if had time.
	"google.",
	"2mdn.net",
	"admeld.",
	"admob.",
	"adwhirl.",
	"ampproject.",
	"android.com",
	"cdn.ampproject.org",
	"chromecast.com",
	"chromeexperiments.com",
	"chromestatus.com",
	"chromium.org",
	"cloudendpointsapis.com",
	"dartmotif.",
	"dartsearch.",
	"doubleclick.",
	"doubleclickusercontent.",
	"g.co",
	"ggpht.com",
	"gmodules.com",
	"goo.gl",
	"google-syndication.com",
	"googleadapis.com",
	"googleadservices.com",
	"googleadsserving.cn",
	"googlealumni.com",
	"googleapis.cn",
	"googleapps.com",
	"googlecbs.com",
	"googlecode.com",
	"googlecommerce.com",
	"googledrive.com",
	"googleenterprise.com",
	"googlefiber.com",
	"googlefiber.net",
	"googlegoro.com",
	"googlehosted.com",
	"googlepayments.com",
	"googlesource.com",
	"googlesyndication.com",
	"googletagmanager.com",
	"googletagservices.com",
	"googleusercontent.com",
	"googlezip.net",
	"gstatic.",
	"gvt2.com",
	"gvt3.com",
	"gvt9.com",
	"picasa.com",
	"recaptcha.net",
	"stackdriver.com",
	"waze.com",
	"youtu.be",
	"youtube-3rd-party.com",
	"youtube-nocookie.com",
	"youtube.",
	"youtubeeducation.com",
	"youtubemobilesupport.com",
	"ytimg.com",
	"mail-attachment.googleusercontent.com",
	"gmail.com",
	"redirector.googlevideo.com",
	"crashdump.spotify.com",
	"invitemedia.com",
	"adeventtracker.spotify.com" /* ad tracker */,
	"tubemogul.com" /* EU, France : Peuuuur-Noel */
};

static const char* whitelist[] = {
	"wpad",
	"spclient.wg.spotify.com",
	"ap.spotify.com",
	"scdn.co",
	"cache.spotify.com",
	/* spotify connect */
	"dealer.spotify.com",
	"apresolve.spotify.com",
	/* podcast */
	"podbean.com",
	"akamaized.net",
	"megaphone.fm",
	"art19.com",
	"simplecast.com",
	"acast.com",
	"bbc.co.uk"
};

static const wchar_t* blockrequest[] = {
	L"/ad-logic/",
	L"/ads/"
};

#endif /* _HOSTS_H */
