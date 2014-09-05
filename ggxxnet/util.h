#pragma once

//******************************************************************
// libs
//******************************************************************
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib, "winmm.lib")

//******************************************************************
// include
//******************************************************************
#include <assert.h>
#include <crtdbg.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>

//******************************************************************
// defines
//******************************************************************
#if _DEBUG
	#define ENABLE_DUMP_MEM_LEAK	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#else
	#define ENABLE_DUMP_MEM_LEAK
#endif

#define IS_MB_CHAR(x) (((unsigned char)x > 0x80 && (unsigned char)x < 0xa0) || ((unsigned char)x > 0xdf && (unsigned char)x < 0xfd))
#define FIXED2FLOAT(x) ((float)x.value + (float)x.fract / 65536)

//******************************************************************
// prototypes
//******************************************************************
void  getMacAddress(int p_idx, char* p_buf);
DWORD getSysDiskSN(void);
void hex2str(char* p_ptr, int p_size, char* p_str);
void str2hex(char* p_ptr, int p_size, char* p_str);
void strtrim(char* p_str);
void strtrimL(char* p_str);
void strtrimR(char* p_str);
void strlower(char* p_str);

WORD GETFCW(void);
void SETFCW(WORD p_cw);

#ifdef _DEBUG
void DBGOUT(char* fmt, ...);
#else
#define DBGOUT
#endif
