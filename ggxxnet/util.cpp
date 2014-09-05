/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

//******************************************************************
// include
//******************************************************************
#include "util.h"
#include <iphlpapi.h>

//******************************************************************
// function
//******************************************************************
void getMacAddress(int p_idx, char* p_buf)
{
	PIP_ADAPTER_INFO	adpinf;
	unsigned long		bufsize;
	
	memset(p_buf, 0, 6);

	GetAdaptersInfo(NULL, &bufsize);
	char* buf = new char[bufsize];
	adpinf = (PIP_ADAPTER_INFO)buf;

	if (GetAdaptersInfo(adpinf, &bufsize) == ERROR_SUCCESS)
	{
		for (int i = 0; adpinf != NULL; i++)
		{
			if (i == p_idx)
			{
				memcpy(p_buf, adpinf->Address, 6);
				break;
			}
			adpinf = adpinf->Next;
		}
	}
	delete buf;
}

DWORD getSysDiskSN(void)
{
	char drivelabel[MAX_PATH];
	GetWindowsDirectory(drivelabel, MAX_PATH);
	drivelabel[3] = '\0';

	DWORD sn = 0xffffffff;
	if (GetDriveType(drivelabel) == DRIVE_FIXED)
	{
		GetVolumeInformation(
			drivelabel,	// ルートディレクトリ
			NULL,		// ボリューム名バッファ
			0,			// 名前バッファのサイズ
			&sn,		// ボリュームのシリアル番号
			0,			// ファイル名の最大の長さ
			0,			// ファイルシステムのオプション
			NULL,		// ファイルシステム名を格納するバッファ
			0);			 // ファイルシステム名を格納するバッファのサイズ
		//char str[9];
		//sprintf(str, "%08x", sn);
		//MessageBox(0, str, 0, 0);
	}
	return sn;
}

void hex2str(char* p_ptr, int p_size, char* p_str)
{
	for (int i = 0; i < p_size; i++)
	{
		sprintf(&p_str[i*2], "%02x", (BYTE)p_ptr[i]);
	}
}

void str2hex(char* p_ptr, int p_size, char* p_str)
{
	int len = (int)strlen(p_str);
	for (int i = 0; i < p_size; i++)
	{
		char h = -1;
		char l = -1;
		if (i * 2 < len)	 h = p_str[i * 2];
		if (i * 2 + 1 < len) l = p_str[i * 2 + 1];

		if (h == -1 && l == -1) break;
		//p_ptr[i] = 0;
		switch (h)
		{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			p_ptr[i] = (h - '0') << 4;
			break;
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			p_ptr[i] = (10 + h - 'a') << 4;
			break;
		}
		switch (l)
		{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			p_ptr[i] |= l - '0';
			break;
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			p_ptr[i] |= 10 + l - 'a';
			break;
		}
	}
}

void _strtrim(char* p_str, int p_param)
{
	int st = 0, ed = (int)strlen(p_str) - 1;
	if (p_param != 1)
	{
		for (int i = 0; p_str[i] == ' ' || p_str[i] == '\t'; i++) st = i + 1;
	}
	if (p_param != 2)
	{
		for (int i = (int)strlen(p_str) - 1; p_str[i] == ' ' || p_str[i] == '\t'; i--) ed = i - 1;
	}
	for (int i = 0; p_str[i] != '\0'; i++)
	{
		p_str[i] = (i <= ed - st) ? p_str[i + st] : '\0';
	}
}

void strtrim(char* p_str)
{
	_strtrim(p_str, 0);
}

void strtrimL(char* p_str)
{
	_strtrim(p_str, 2);
}

void strtrimR(char* p_str)
{
	_strtrim(p_str, 1);
}

void strlower(char* p_str)
{
	for (int i = 0; p_str[i] != '\0'; i++)
	{
		if (p_str[i] >= 'A' && p_str[i] <= 'Z') p_str[i] += 'a' - 'A';
	}
}

WORD GETFCW(void)
{
	WORD cw;
	_asm fstcw cw;
	return cw;
}

void SETFCW(WORD p_cw)
{
	_asm fldcw p_cw;
}

#ifdef _DEBUG
void DBGOUT(char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	char buf[1024];
	vsprintf(buf, fmt, ap);

	OutputDebugString(buf);

	va_end(ap);
}
#endif
