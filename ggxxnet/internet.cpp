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

//******************************************************************
// functions
//******************************************************************
int internet_get(char* p_buf, int p_bufsize, char* p_server, char* p_target)
{
	HINTERNET hInternet = InternetOpen(
		"xxx",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0);
	if (hInternet == NULL)
	{
		OutputDebugString("InternetOpen");
		return 0;
	}

	HINTERNET hHttpSession = InternetConnect(
		hInternet,
		p_server,
		INTERNET_DEFAULT_HTTP_PORT,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		0);
	if (hHttpSession == NULL)
	{
		OutputDebugString("InternetConnect");
		InternetCloseHandle(hInternet);
		return 0;
	}

	HINTERNET hHttpRequest = HttpOpenRequest(
		hHttpSession,
		"GET",
		p_target,
		NULL,
		NULL,
		NULL,
		INTERNET_FLAG_RESYNCHRONIZE,
		0);
	if (hHttpRequest == NULL)
	{
		OutputDebugString("HttpOpenRequest");
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hInternet);
		return 0;
	}
	
	int ms = 1000;
	InternetSetOption(hHttpRequest, INTERNET_OPTION_CONNECT_TIMEOUT, &ms, sizeof(ms));
	InternetSetOption(hHttpRequest, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &ms, sizeof(ms));
	InternetSetOption(hHttpRequest, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &ms, sizeof(ms));
	InternetSetOption(hHttpRequest, INTERNET_OPTION_DATA_SEND_TIMEOUT, &ms, sizeof(ms));
	InternetSetOption(hHttpRequest, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, &ms, sizeof(ms));

	if (!HttpSendRequest(
		hHttpRequest,
		NULL,
		-1,
		NULL,
		0))
	{
		OutputDebugString("HttpSendRequest");
		InternetCloseHandle(hHttpRequest);
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hInternet);
		return 0;
	}
	
	int readsize = 0;
	if (p_buf && p_bufsize > 0)
	{
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
			if (!InternetReadFile(hHttpRequest, &p_buf[readsize], size, (DWORD*)&actualsize))
			{
				OutputDebugString("InternetReadFile");
				InternetCloseHandle(hHttpRequest);
				InternetCloseHandle(hHttpSession);
				InternetCloseHandle(hInternet);
				return 0;
			}
			readsize += actualsize;
		}
	}

	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hInternet);

	return readsize;
}

int internet_post(char* p_buf, int p_sendsize, int p_bufsize, char* p_server, char* p_target)
{
	HINTERNET hInternet = InternetOpen(
		"xxx",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0);
	if (hInternet == NULL)
	{
		OutputDebugString("InternetOpen");
		return 0;
	}

	HINTERNET hHttpSession = InternetConnect(
		hInternet,
		p_server,
		INTERNET_DEFAULT_HTTP_PORT,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		0);
	if (hHttpSession == NULL)
	{
		OutputDebugString("InternetConnect");
		InternetCloseHandle(hInternet);
		return 0;
	}

	HINTERNET hHttpRequest = HttpOpenRequest(
		hHttpSession,
		"POST",
		p_target,
		NULL,
		NULL,
		NULL,
		INTERNET_FLAG_RELOAD,
		0);
	if (hHttpRequest == NULL)
	{
		OutputDebugString("HttpOpenRequest");
		InternetCloseHandle(hHttpSession);
		InternetCloseHandle(hInternet);
		return 0;
	}
	
	//int ms = 1000;
	//InternetSetOption(hHttpRequest, INTERNET_OPTION_CONNECT_TIMEOUT, &ms, sizeof(ms));
	//InternetSetOption(hHttpRequest, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &ms, sizeof(ms));
	//InternetSetOption(hHttpRequest, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &ms, sizeof(ms));
	//InternetSetOption(hHttpRequest, INTERNET_OPTION_DATA_SEND_TIMEOUT, &ms, sizeof(ms));
	//InternetSetOption(hHttpRequest, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, &ms, sizeof(ms));

	if (!HttpSendRequest(
		hHttpRequest,
		NULL,
		0,
		p_buf,
		p_sendsize))
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
		if (!InternetReadFile(hHttpRequest, &p_buf[readsize], size, (DWORD*)&actualsize))
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
