/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

//******************************************************************
// include
//******************************************************************
#include <math.h>

#include "icon.h"
#include "ggxxnet.h"
#include "res/resource.h"

//******************************************************************
// structures
//******************************************************************
class D3DV_ICON
{
public:
	enum { FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1) };
	D3DV_ICON(void)
	{
		m_x			= 0.0f;
		m_y			= 0.0f;
		m_z			= 1.0f;
		m_rhw		= 1.0f;
		m_diffuse	= 0x00000000;
		m_u			= 0.0f;
		m_v			= 0.0f;
	}
	~D3DV_ICON(void) {}
	
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

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	BYTE  width;
	BYTE  height;
	BYTE  colorCount;
	BYTE  reserved;
	WORD  planes;
	WORD  bitCount;
	DWORD size;
	WORD  id;
}ICONDIRENTRY;

typedef struct
{
	WORD		 reserved;
	WORD		 type;
	WORD		 count;
	ICONDIRENTRY entries[1];
}ICONHEADER;

#pragma pack(pop)

//******************************************************************
// method
//******************************************************************
CIcon::CIcon(HINSTANCE p_hInst, LPDIRECT3DDEVICE_ p_d3ddev, int p_resID, D3DFORMAT p_format)
{
	m_D3DDevice = p_d3ddev;
	m_tex		= NULL;

	HRSRC hRes = FindResource(p_hInst, MAKEINTRESOURCE(p_resID), RT_GROUP_ICON);
	if (hRes)
	{
		HGLOBAL hHeapResGrp = LoadResource(p_hInst, hRes);
		ICONHEADER* data = (ICONHEADER*)LockResource(hHeapResGrp);
		if (data->count > 0)
		{
			/* 先頭のアイコンのみ認識する */
			ICONDIRENTRY* icon = (ICONDIRENTRY*)data->entries;
			
			hRes = FindResource(p_hInst, MAKEINTRESOURCE(icon->id), RT_ICON);
			HGLOBAL hHeapRes = LoadResource(p_hInst, hRes);
			BITMAPINFOHEADER*	bmih = (BITMAPINFOHEADER*)LockResource(hHeapRes);
			RGBQUAD*			ctab = (RGBQUAD*)((char*)bmih + sizeof(BITMAPINFOHEADER));
			int					ctsize = (int)pow((float)2, (float)bmih->biBitCount);
			BYTE*				img  = ((BYTE*)ctab + ctsize * sizeof(RGBQUAD));
			BYTE*				mask = img + icon->width * icon->height; 
			
			if (bmih->biBitCount != 8) return;	/* 256色限定にしておく */
			
			/* テクスチャサイズは2^n */
			UINT tw = 1;
			UINT th = 1;
			for (int i = 0; tw < icon->width; i++) tw <<= 1;
			for (int i = 0; th < icon->height; i++) th <<= 1;
			
			if (m_D3DDevice)
			{
#if DIRECT3D_VERSION == 0x0800
				HRESULT hr = m_D3DDevice->CreateTexture(tw, th, 1, 0, p_format, D3DPOOL_MANAGED, &m_tex);
#else
				HRESULT hr = m_D3DDevice->CreateTexture(tw, th, 1, 0, p_format, D3DPOOL_MANAGED, &m_tex, NULL);
#endif
				if (FAILED(hr)) COMERR("CreateTexture()", hr);

				int pixelSize = (p_format == D3DFMT_A8R8G8B8) ? 4 : 2;

				D3DLOCKED_RECT lockRect;
				hr = m_tex->LockRect(0, &lockRect, NULL, D3DLOCK_DISCARD);
				
				int srb	= icon->width + 3 & 0x7ffffffc;
				int mrb	= (icon->width + 31) >> 3 & 0x7ffffffc;
				int drb	= lockRect.Pitch / pixelSize;
				for (UINT y = 0; y < icon->height; y++)
				{
					for (UINT x = 0; x < icon->width; x++)
					{
						if (pixelSize == 4)
						{
							if (mask[(x >> 3) + mrb * y] & 0x80 >> (x & 7))
							{
								/* 透過色 */
								((DWORD*)lockRect.pBits)[x + drb * (icon->height - y - 1)] = 0x00000000;
							}
							else
							{
								RGBQUAD* src = &ctab[img[x + srb * y]];
								((DWORD*)lockRect.pBits)[x + drb * (icon->height - y - 1)] =
									0xff000000 |
									src->rgbRed << 16 |
									src->rgbGreen << 8 |
									src->rgbBlue;
							}
						}
						else
						{
							if (mask[(x >> 3) + mrb * y] & 0x80 >> (x & 7))
							{
								/* 透過色 */
								((WORD*)lockRect.pBits)[x + drb * (icon->height - y - 1)] = 0x0000;
							}
							else
							{
								RGBQUAD* src = &ctab[img[x + srb * y]];
								((WORD*)lockRect.pBits)[x + drb * (icon->height - y - 1)] =
									0xf000 |
									(src->rgbRed & 0xf0) << 4 |
									(src->rgbGreen & 0xf0) |
									(src->rgbBlue & 0xf0) >> 4;
							}
						}

					}
				}

				hr = m_tex->UnlockRect(0);
			}
			//GlobalFree(hHeapRes);
		}
		//GlobalFree(hHeapResGrp);
	}
}

void CIcon::draw(int p_x, int p_y, DWORD p_color)
{
	m_D3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
	m_D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_D3DDevice->SetRenderState(D3DRS_BLENDOP , D3DBLENDOP_ADD);
	m_D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	m_D3DDevice->SetTexture(0, m_tex);
	m_D3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	m_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

#if DIRECT3D_VERSION == 0x0800
	m_D3DDevice->SetVertexShader(D3DV_ICON::FVF);
#else
	m_D3DDevice->SetFVF(D3DV_ICON::FVF);
#endif

	D3DV_ICON	d3dv[4];
	d3dv[0].setPos(p_x, p_y);
	d3dv[0].setColor(p_color);
	d3dv[0].setUV(0.0f, 0.0f);
	d3dv[1].setPos(p_x + 16, p_y);
	d3dv[1].setColor(p_color);
	d3dv[1].setUV(1.0f, 0.0f);
	d3dv[2].setPos(p_x, p_y + 16);
	d3dv[2].setColor(p_color);
	d3dv[2].setUV(0.0f, 1.0f);
	d3dv[3].setPos(p_x + 16, p_y + 16);
	d3dv[3].setColor(p_color);
	d3dv[3].setUV(1.0f, 1.0f);

	m_D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, d3dv, sizeof(D3DV_ICON));
}
