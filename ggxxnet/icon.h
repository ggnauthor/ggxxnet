#pragma once

//******************************************************************
// libs
//******************************************************************
#if DIRECT3D_VERSION == 0x0800
#pragma comment(lib, "d3d8.lib")
#else
#pragma comment(lib, "d3d9.lib")
#endif

//******************************************************************
// includes
//******************************************************************
#if DIRECT3D_VERSION == 0x0800
#include <d3d8.h>
#include <dxerr8.h>
#define LPDIRECT3DTEXTURE_	LPDIRECT3DTEXTURE8
#define LPDIRECT3DDEVICE_	LPDIRECT3DDEVICE8
#else
#include <d3d9.h>
#include <dxerr9.h>
#define LPDIRECT3DTEXTURE_	LPDIRECT3DTEXTURE9
#define LPDIRECT3DDEVICE_	LPDIRECT3DDEVICE9
#endif

#include <stdio.h>
#include <assert.h>

//******************************************************************
// defines
//******************************************************************
#ifndef APPERR
#define APPERR(x)		{ char msg[256]; sprintf(msg, "Application Error!\nfile:%s\nfunction:%s\nline:%d\n%s", __FILE__, __FUNCTION__, __LINE__, x); MessageBoxA(0, msg, "", 0); exit(1); }
#endif
#ifndef COMERR
#define COMERR(api, hr)	{ char msg[256]; sprintf(msg, "COM Error!\nfile:%s\nfunction:%s\nline:%d\nAPI : %s\nHRESULT : %x", __FILE__, __FUNCTION__, __LINE__, api, hr); MessageBoxA(0, msg, "", 0); exit(1); }
#endif

//******************************************************************
// class
//******************************************************************
class CIcon
{
public:
	CIcon(HINSTANCE p_hInst, LPDIRECT3DDEVICE_ p_d3ddev, int p_resID, D3DFORMAT p_format);
	~CIcon(void) {}
	
	void draw(int p_x, int p_y, DWORD p_color = 0xFFFFFFFF);

private:
	LPDIRECT3DDEVICE_	m_D3DDevice;
	LPDIRECT3DTEXTURE_	m_tex;
};
