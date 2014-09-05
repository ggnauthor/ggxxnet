/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

//******************************************************************
// includes
//******************************************************************
#include "d3dfont.h"

#include <math.h>
#include <locale.h>

//******************************************************************
// structures
//******************************************************************
class D3DV_FONT
{
public:
	enum { FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1) };
	D3DV_FONT(void)
	{
		m_x			= 0.0f;
		m_y			= 0.0f;
		m_z			= 1.0f;
		m_rhw		= 1.0f;
		m_diffuse	= 0x00000000;
		m_u			= 0.0f;
		m_v			= 0.0f;
	}
	~D3DV_FONT(void) {}
	
	void setPos(int p_x, int p_y)		{ m_x = (float)p_x; m_y = (float)p_y; }
	void setColor(DWORD p_diffuse)		{ m_diffuse = p_diffuse; }
	void setUV(float p_u, float p_v)	{ m_u = p_u; m_v = p_v; }

private:
	float	m_x;
	float	m_y;
	float	m_z;
	float	m_rhw;
	DWORD	m_diffuse;
	float	m_u;
	float	m_v;
};

//******************************************************************
// methods
//******************************************************************
CFontObj::CFontObj(wchar_t p_ch, char* p_face, int p_size, int p_weight, int p_antialias, int p_outline)
{
	m_ch		= p_ch;
	m_left		= 0;
	m_top		= 0;
	m_width		= 0;

	strcpy(m_face, p_face);
	m_size		= p_size;
	m_weight	= p_weight;
	m_antiAlias	= p_antialias;
	m_outline	= p_outline;

	m_tex		= NULL;
	m_tex_w		= 0;
	m_tex_h		= 0;

	m_synonym	= NULL;
}

unsigned char CFontObj::gethash(void)
{
	int num = m_ch + m_size + m_weight + m_antiAlias + m_outline;
	for (int i = 0; m_face[i] != '\0'; i++) num += m_face[i];
	//num = 0;
	return (unsigned char)(num % CD3DFont::FontObjHashTableSize);
}

///////////////////////////////////////////////////////////////////////////

CD3DFont::CD3DFont(LPDIRECT3DDEVICE_ p_d3ddev, D3DFORMAT p_format)
{
	setlocale(LC_ALL, "");

	m_d3ddev = p_d3ddev;
	m_format = p_format;

	m_fobjpos = 0;
	m_fobj = new CFontObj*[MaxFontObjCount];
	for (int i = 0; i < MaxFontObjCount; i++) m_fobj[i] = NULL;

	for (int i = 0; i < FontObjHashTableSize; i++) m_fobjhashtable[i] = NULL;

	setFont("", 12, 0, FW_NORMAL, 0, 0);
}

CD3DFont::~CD3DFont(void)
{
	if (m_fobj)
	{
		for (int i = 0; i < FontObjHashTableSize; i++) m_fobjhashtable[i] = NULL;

		for (int i = 0; i < MaxFontObjCount; i++) if (m_fobj[i]) delete m_fobj[i];
		delete[] m_fobj;
	}
}

CFontObj* CD3DFont::getfobj(wchar_t p_ch)
{
	CFontObj	tmp(p_ch, m_face, m_size, m_weight, m_antiAlias, m_outline);
	for (CFontObj* p = m_fobjhashtable[tmp.gethash()]; p != NULL; p = p->m_synonym)
	{
		/* 完全一致するか？ */
		if (p->m_ch == p_ch && p->m_size == m_size && p->m_weight == m_weight &&
			p->m_antiAlias == m_antiAlias && p->m_outline == m_outline &&
			strcmp(p->m_face, m_face) == 0)
		{
			return p;
		}
	}
	return NULL;
}

void CD3DFont::setFont(char* p_face, int p_size, int p_aalv, int p_weight, int p_margin, int p_outline)
{
	strcpy(m_face, p_face);
	m_size		= p_size;
	m_antiAlias	= p_aalv;
	if (m_antiAlias < 0) m_antiAlias = 0;
	if (m_antiAlias > 3) m_antiAlias = 3;
	m_weight	= p_weight;
	m_margin	= p_margin;
	m_outline	= p_outline;
}

void CD3DFont::setAlphaMode(EAlpha p_alpha)
{
	switch (p_alpha)
	{
	case Alpha_Disable:
		m_d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, false);
		m_d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		break;
	case Alpha_Normal:
		m_d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		m_d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		m_d3ddev->SetRenderState(D3DRS_BLENDOP , D3DBLENDOP_ADD);
		m_d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		break;
	case Alpha_Add:
		m_d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		m_d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		m_d3ddev->SetRenderState(D3DRS_BLENDOP , D3DBLENDOP_ADD);
		m_d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		break;
	case Alpha_Sub:
		m_d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, true);
		m_d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		m_d3ddev->SetRenderState(D3DRS_BLENDOP , D3DBLENDOP_REVSUBTRACT);
		m_d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		break;
	}
}

void CD3DFont::drawImage(LPDIRECT3DTEXTURE_ p_tex, int p_tex_w, int p_tex_h,
						   int p_x, int p_y, DWORD p_color, EAlpha p_alpha)
{
	setAlphaMode(p_alpha);
	m_d3ddev->SetTexture(0, p_tex);
	m_d3ddev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_d3ddev->SetFVF(D3DV_FONT::FVF);

	D3DV_FONT	d3dv[4];
	d3dv[0].setPos(p_x, p_y);
	d3dv[0].setColor(p_color);
	d3dv[0].setUV(0.0f, 0.0f);
	d3dv[1].setPos(p_x + p_tex_w, p_y);
	d3dv[1].setColor(p_color);
	d3dv[1].setUV(1.0f, 0.0f);
	d3dv[2].setPos(p_x, p_y + p_tex_h);
	d3dv[2].setColor(p_color);
	d3dv[2].setUV(0.0f, 1.0f);
	d3dv[3].setPos(p_x + p_tex_w, p_y + p_tex_h);
	d3dv[3].setColor(p_color);
	d3dv[3].setUV(1.0f, 1.0f);

	m_d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, d3dv, sizeof(D3DV_FONT));
}

void CD3DFont::drawText(char* p_str, int p_left, int p_top, DWORD p_color, int p_align)
{
	wchar_t wstr[1024];
	int size = _mbstrlen(p_str);
	if (size != -1)
	{
		mbstowcs(wstr, p_str, size + 1);
		drawText(wstr, p_left, p_top, p_color, p_align);
	}
}

void CD3DFont::drawText(wchar_t* p_str, int p_left, int p_top, DWORD p_color, int p_align)
{
	int row = 0, col, line = 0;
	if (p_align == Align_Left)			col = 0;
	else if (p_align == Align_Right)	col = -getTextWidth(p_str, line);
	else if (p_align == Align_Center)	col = -getTextWidth(p_str, line) / 2;
	
	int len = (int)wcslen(p_str);
	for (int i = 0; i < len; i++)
	{
		while (p_str[i] == L'\r' || 0) i++;

		if (p_str[i] == L'\n')
		{
			row += m_size;
			line++;
			if (p_align == Align_Left)			col = 0;
			else if (p_align == Align_Right)	col = -getTextWidth(p_str, line);
			else if (p_align == Align_Center)	col = -getTextWidth(p_str, line) / 2;
			continue;
		}

		CFontObj* fop = getfobj(p_str[i]);
		if (!fop) fop = createfobj(p_str[i]);
		
		if (fop)
		{
			drawImage(fop->m_tex, fop->m_tex_w, fop->m_tex_h,
				p_left + fop->m_left + col, p_top + fop->m_top + row, p_color, Alpha_Normal);
			col += fop->m_width + m_margin;
		}
	}
}

int CD3DFont::getTextWidth(char* p_str, int p_line)
{
	wchar_t wstr[1024];
	mbstowcs(wstr, p_str, _mbstrlen(p_str) + 1);
	return getTextWidth(wstr, p_line);
}

int CD3DFont::getTextWidth(wchar_t* p_str, int p_line)
{
	int w = 0, maxw = 0, line = 0;
	int len = (int)wcslen(p_str);
	for (int i = 0; i < len; i++)
	{
		if (p_str[i] == L'\n')
		{
			w = 0;
			line++;
			if (p_line != -1 && line > p_line) break;	/* 目的の行を終えた */
			continue;
		}
		if (line != p_line && p_line != -1) continue;	/* 無関係な行は読まない */
		
		CFontObj* fop = getfobj(p_str[i]);
		if (!fop) fop = createfobj(p_str[i]);
		
		if (fop)
		{
			w += fop->m_width + m_margin;

			if (p_line == -1 || line == p_line)
			{
				if (w > maxw) maxw = w;
			}
		}
	}
	return maxw;
}

CFontObj* CD3DFont::createfobj(wchar_t p_ch)
{
	if (p_ch == 0xffff) return NULL;

	CFontObj* fop = new CFontObj(p_ch, m_face, m_size, m_weight, m_antiAlias, m_outline);

	const MAT2 mat2 = { { 0, 1 }, { 0, 0 },
						{ 0, 0 }, { 0, 1 } }; /* 単位行列 */
	GLYPHMETRICS	gm;
	HDC				hdc = GetDC(NULL);
	
	LOGFONT	logFont;
	logFont.lfHeight		= m_size;
	logFont.lfWidth			= 0;
	logFont.lfEscapement	= 0;
	logFont.lfOrientation	= 0;
	logFont.lfWeight		= m_weight;
	logFont.lfItalic		= FALSE;
	logFont.lfUnderline		= FALSE;
	logFont.lfStrikeOut		= FALSE;
	logFont.lfCharSet		= SHIFTJIS_CHARSET;
	logFont.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	logFont.lfQuality		= DEFAULT_QUALITY;
	logFont.lfPitchAndFamily= DEFAULT_PITCH || FF_DONTCARE;
	strcpy(logFont.lfFaceName, m_face);
	HFONT font = CreateFontIndirect(&logFont);
	HGDIOBJ oldfont = SelectObject(hdc, font);

	int aalv;
	if (m_antiAlias == 1)		aalv = GGO_GRAY2_BITMAP;
	else if (m_antiAlias == 2)	aalv = GGO_GRAY4_BITMAP;
	else if (m_antiAlias == 3)	aalv = GGO_GRAY8_BITMAP;
	else						aalv = GGO_BITMAP;

	int bufsize = GetGlyphOutlineW(hdc, p_ch, aalv, &gm, 0, NULL, &mat2);
	if (bufsize <= 0)
	{
		SelectObject(hdc, oldfont);
		ReleaseDC(NULL, hdc);
		if (font) DeleteObject(font);
		delete fop;
		return NULL;
	}
	char* buf = new char[bufsize];
	GetGlyphOutlineW(hdc, p_ch, aalv, &gm, bufsize, buf, &mat2);
	
	TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);

	SelectObject(hdc, oldfont);
	ReleaseDC(NULL, hdc);
	if (font) DeleteObject(font);

	/* テクスチャサイズは2^n */
	UINT tw = 1;
	UINT th = 1;
	for (int i = 0; tw < gm.gmBlackBoxX + m_outline * 2; i++) tw <<= 1;
	for (int i = 0; th < gm.gmBlackBoxY + m_outline * 2; i++) th <<= 1;

	HRESULT hr = m_d3ddev->CreateTexture(tw, th, 1, 0, m_format, D3DPOOL_MANAGED, &fop->m_tex, NULL);
	if (FAILED(hr)) COMERR("CreateTexture()", hr);
	
	int pixelSize = (m_format == D3DFMT_A8R8G8B8) ? 4 : 2;

	D3DLOCKED_RECT lockRect;
	fop->m_tex->LockRect(0, &lockRect, NULL, D3DLOCK_DISCARD);
	
	memset(lockRect.pBits, 0, tw * th * pixelSize);

	if (m_antiAlias == 0)
	{
		/* AntiAlias無効 */
		int srb = ((gm.gmBlackBoxX + 7) >> 3) + 3 & 0x7ffffffc;
		int drb = lockRect.Pitch / pixelSize;
		for (UINT y = 0; y < gm.gmBlackBoxY; y++)
		{
			for (UINT x = 0; x < gm.gmBlackBoxX; x++)
			{
				if ((buf[(x >> 3) + srb * y] & (0x80 >> (x & 7))) == 0) continue;

				for (int i = 0; i <= m_outline << 1; i++)
				{
					int tmp = x + drb * (y + i);
					for (int j = 0; j <= m_outline << 1; j++)
					{
						if (pixelSize == 4) ((DWORD*)lockRect.pBits)[j + tmp] = 0xffffffff;
						else				((WORD*)lockRect.pBits)[j + tmp] = 0xffff;
					}
				}
			}
		}
		
		if (m_outline > 0)	/* 中抜き */
		{
			for (UINT y = 0; y < gm.gmBlackBoxY; y++)
			{
				int tmp = drb * (y + m_outline);
				for (UINT x = 0; x < gm.gmBlackBoxX; x++)
				{
					if ((buf[(x >> 3) + srb * y] & (0x80 >> (x & 7))))
					{
						if (pixelSize == 4) ((DWORD*)lockRect.pBits)[x + m_outline + tmp] = 0x00000000;
						else				((WORD*)lockRect.pBits)[x + m_outline + tmp] = 0x0000;
					}
				}
			}
		}
	}
	else
	{
		/* AntiAlias有効 */
		int srb = gm.gmBlackBoxX + 3 & 0x7ffffffc;
		int drb = lockRect.Pitch / pixelSize;
		for (UINT y = 0; y < gm.gmBlackBoxY; y++)
		{
			for (UINT x = 0; x < gm.gmBlackBoxX; x++)
			{
				int alpha = buf[x + srb * y] << (8 - m_antiAlias * 2);
				if (alpha == 0) continue;
				if (alpha > 0xff) alpha = 0xff;

				for (int i = 0; i <= m_outline << 1; i++)
				{
					int tmp = x + drb * (y + i);
					for (int j = 0; j <= m_outline << 1; j++)
					{
						if (pixelSize == 4)
						{
							int newalpha = (((DWORD*)lockRect.pBits)[j + tmp] >> 24) + alpha;
							if (newalpha > 0xff) newalpha = 0xff;
							((DWORD*)lockRect.pBits)[j + tmp] = (newalpha << 24) | 0x00ffffff;
						}
						else
						{
							// そもそも1bitしかαチャンネルが無いのでAAは表現不能
							((WORD*)lockRect.pBits)[j + tmp] = 0xffff;
						}
					}
				}
			}
		}

		if (m_outline > 0)	/* 中抜き */
		{
			for (UINT y = 0; y < gm.gmBlackBoxY; y++)
			{
				int tmp = drb * (y + m_outline);
				for (UINT x = 0; x < gm.gmBlackBoxX; x++)
				{
					int alpha = buf[x + srb * y] << (8 - m_antiAlias * 2);
					if (alpha == 0) continue;
					if (alpha > 0xff) alpha = 0xff;

					if (pixelSize == 4)
					{
						int newalpha = (((DWORD*)lockRect.pBits)[x + m_outline + tmp] >> 24) - alpha;
						if (newalpha < 0) newalpha = 0;
						((DWORD*)lockRect.pBits)[x + m_outline + tmp] = (newalpha << 24) | 0x00ffffff;
					}
					else
					{
						// そもそも1bitしかαチャンネルが無いのでAAは表現不能
						((WORD*)lockRect.pBits)[x + m_outline + tmp] = 0x0000;
					}
				}
			}
		}
	}
	fop->m_tex->UnlockRect(0);

	delete[] buf;

	D3DSURFACE_DESC sdesc;
	fop->m_tex->GetLevelDesc(0, &sdesc);
	fop->m_tex_w	= sdesc.Width;
	fop->m_tex_h	= sdesc.Height;

	fop->m_left		= gm.gmptGlyphOrigin.x;
	fop->m_top		= tm.tmAscent - gm.gmptGlyphOrigin.y;
	fop->m_width	= gm.gmCellIncX;
	fop->m_synonym	= NULL;

	/* fobjを登録 既にある場合は元あるものを削除 */
	if (m_fobj[m_fobjpos])
	{
		/* ハッシュテーブルから削除 */
		int hash = m_fobj[m_fobjpos]->gethash();
		CFontObj*	parent = NULL;
		CFontObj*	delobj = m_fobj[m_fobjpos];
		for (CFontObj* p = m_fobjhashtable[hash]; p != NULL; p = p->m_synonym)
		{
			/* 完全一致するか？ */
			if (p->m_ch == delobj->m_ch && p->m_size == delobj->m_size && p->m_weight == delobj->m_weight &&
				p->m_antiAlias == delobj->m_antiAlias && p->m_outline == delobj->m_outline &&
				strcmp(p->m_face, delobj->m_face) == 0)
			{
				if (parent) parent->m_synonym = p->m_synonym;
				else		m_fobjhashtable[hash] = p->m_synonym;
				break;
			}
			parent = p;
		}

		delete m_fobj[m_fobjpos];
	}
	m_fobj[m_fobjpos] = fop;
	m_fobjpos = (m_fobjpos + 1) % MaxFontObjCount;

	/* ハッシュテーブルに追加 */
	int hash = fop->gethash();
	for (CFontObj** p = &m_fobjhashtable[hash]; ; p = &(*p)->m_synonym)
	{
		if (*p == NULL)
		{
			*p = fop;
			break;
		}
	}
	
	return fop;
}
