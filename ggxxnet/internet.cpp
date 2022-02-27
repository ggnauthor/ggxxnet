/* for Visual Studio 8.0 */
#ifdef _MSC_VER
#if (_MSC_VER >= 1400)
#define POINTER_64 __ptr64
#endif
#endif

//******************************************************************
// include
//******************************************************************
#include "internet.h"
#include "util.h"
#include <string>

//******************************************************************
// functions
//******************************************************************
int makePost(char* p_buf, int p_sendsize, int p_bufsize, std::string p_server, std::string p_target, char* response)
{

	HINTERNET hInternet = InternetOpen("xxx", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hInternet == NULL)
	{
		OutputDebugString("InternetOpen");
		return 0;
	}
	HINTERNET hHttpSession = InternetConnect(hInternet, p_server.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

	if (hHttpSession == NULL)
	{
		OutputDebugString("InternetConnect");
		InternetCloseHandle(hInternet);
		return 0;
	}
	PCTSTR  rgpszAcceptTypes[] = { ("application/json"), NULL };
	HINTERNET hHttpRequest = HttpOpenRequest(hHttpSession, "POST", p_target.c_str(), NULL, NULL, rgpszAcceptTypes, INTERNET_FLAG_RELOAD, 0);
	HttpAddRequestHeaders(hHttpRequest, "Content-Type: application/json\r\n", -1, HTTP_ADDREQ_FLAG_ADD);
	if (hHttpRequest == NULL)
	{
		OutputDebugString("HttpOpenRequest");
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hInternet);
		return 0;
	}

	if (!HttpSendRequest(hHttpRequest, NULL, 0, p_buf, strlen(p_buf)))
	{
		OutputDebugString("HttpSendRequest");
		InternetCloseHandle(hHttpRequest);
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hInternet);
		return 0;
	}

	int readsize = 0;
	while (true)
	{
		int size;
		if (!InternetQueryDataAvailable(hHttpRequest, (DWORD*)&size, 0, 0))
		{
			OutputDebugString("InternetQueryDataAvailable");
			InternetCloseHandle(hHttpRequest);
			InternetCloseHandle(hHttpSession);
			InternetCloseHandle(hInternet);
			return 0;
		}
		if (size == 0) break;
		if (p_bufsize < readsize + size) break;

		int actualsize;
		if (!InternetReadFile(hHttpRequest, &response[readsize], size, (DWORD*)&actualsize))
		{
			OutputDebugString("InternetReadFile");
			InternetCloseHandle(hHttpRequest);
			InternetCloseHandle(hHttpSession);
			InternetCloseHandle(hInternet);
			return 0;
		}
		readsize += actualsize;
	}

	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hInternet);

	return readsize;
}