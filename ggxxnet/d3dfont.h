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

#define CreateTexture(p1, p2, p3, p4, p5, p6, p7, p8) CreateTexture(p1, p2, p3, p4, p5, p6, p7)
#define SetFVF(x)	SetVertexShader(x)
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
class CFontObj
{
	friend class CD3DFont;

public:
	CFontObj(wchar_t p_ch, char* p_face, int p_size, int p_weight, int p_antialias, int p_outline);
	~CFontObj(void) { if (m_tex) m_tex->Release(); }

	unsigned char	gethash(void);
	
private:
	wchar_t	m_ch;
	int		m_left;
	int		m_top;
	int		m_width;
	
	char	m_face[64];
	int		m_size;
	int		m_weight;
	int		m_antiAlias;
	int		m_outline;

	LPDIRECT3DTEXTURE_	m_tex;
	int					m_tex_w;
	int					m_tex_h;

	CFontObj*			m_synonym;
};

class CD3DFont
{
public:
	enum
	{
		MaxFontObjCount = 4096,
		FontObjHashTableSize = 256,
	};
	enum EAlpha
	{
		Alpha_Disable = 0,
		Alpha_Normal,
		Alpha_Add,
		Alpha_Sub,
	};
	enum EAlign
	{
		Align_Left = 0,
		Align_Right,
		Align_Center,
	};

	CD3DFont(LPDIRECT3DDEVICE_ p_d3ddev, D3DFORMAT p_format);
	~CD3DFont(void);

	CFontObj*	getfobj(wchar_t p_ch);
	CFontObj*	createfobj(wchar_t p_ch);

	void setFont(char* p_face, int p_size, int p_aalv, int p_weight, int p_margin, int p_outline = 0);

	void drawText(char* p_str, int p_left, int p_top, DWORD p_color = 0xFFFFFFFF, int p_align = Align_Left);
	void drawText(wchar_t* p_str, int p_left, int p_top, DWORD p_color = 0xFFFFFFFF, int p_align = Align_Left);
	
	int getTextWidth(char* p_str, int p_line = -1);
	int getTextWidth(wchar_t* p_str, int p_line = -1);

private:
	void setAlphaMode(EAlpha p_alpha);
	void drawImage(LPDIRECT3DTEXTURE_ p_tex, int p_tex_w, int p_tex_h,
		int p_x, int p_y, DWORD p_color, EAlpha p_alpha);

private:
	LPDIRECT3DDEVICE_	m_d3ddev;
	D3DFORMAT			m_format;

	CFontObj**		m_fobj;
	int					m_fobjpos;
	CFontObj*			m_fobjhashtable[FontObjHashTableSize];

	char	m_face[64];
	int		m_size;
	DWORD	m_color;
	int		m_antiAlias;
	int		m_weight;
	int		m_margin;
	int		m_outline;
};
