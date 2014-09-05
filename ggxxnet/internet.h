#pragma once

//******************************************************************
// libs
//******************************************************************
#pragma comment(lib, "wininet.lib")

//******************************************************************
// include
//******************************************************************
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <wininet.h>

//******************************************************************
// prototypes
//******************************************************************
int internet_get(char* p_buf, int p_bufsize, char* p_server, char* p_target);
int internet_post(char* p_buf, int p_sendsize, int p_bufsize, char* p_server, char* p_target);
