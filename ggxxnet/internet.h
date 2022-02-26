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
#include <string>

//******************************************************************
// prototypes
//******************************************************************
int makePost(char* p_buf, int p_sendsize, int p_bufsize, std::string p_server, char* p_target,char* response);