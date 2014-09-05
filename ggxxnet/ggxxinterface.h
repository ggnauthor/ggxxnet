//******************************************************************
// GGXX Vars
//******************************************************************
DWORD* const GGXX_1PDOWN	= (DWORD*)0x608f00;
DWORD* const GGXX_2PDOWN	= (DWORD*)0x608f14;
DWORD* const GGXX_1PJDOWN	= (DWORD*)0x608f0c;
DWORD* const GGXX_2PJDOWN	= (DWORD*)0x608f20;
DWORD* const GGXX_MODE1		= (DWORD*)0x5f9568;
DWORD* const GGXX_MODE2		= (DWORD*)0x5cdfb0;
DWORD* const GGXX_BATTLESTAGE	= (DWORD*)0x5cdf98;
DWORD* const GGXX_STAGESELECTER = (DWORD*)0x602878;

WORD*  const GGXX_1PCOLOR	= (WORD*)0x5f94b4;
WORD*  const GGXX_2PCOLOR	= (WORD*)0x5f94b6;
WORD*  const GGXX_1PCHARA	= (WORD*)0x5f94c8;
WORD*  const GGXX_2PCHARA	= (WORD*)0x5f94ca;
WORD*  const GGXX_1PVOICE	= (WORD*)0x5f94d6;
WORD*  const GGXX_2PVOICE	= (WORD*)0x5f94d8;
DWORD* const GGXX_1PEX		= (DWORD*)0x5F9594;
DWORD* const GGXX_2PEX		= (DWORD*)0x5F9598;

BYTE*  const GGXX_1PCHEAT	= (BYTE*)0x5f958c; /* 0＝ノーマル、1＝金、2＝黒 */
BYTE*  const GGXX_2PCHEAT	= (BYTE*)0x5f9590; /* 0＝ノーマル、1＝金、2＝黒 */

WORD*  const GGXX_ROUND		= (WORD*)0x5f94dc;
DWORD* const GGXX_TIME		= (DWORD*)0x602760;
WORD*  const GGXX_1PLIFE	= (WORD*)0x5ff5a0;
WORD*  const GGXX_2PLIFE	= (WORD*)0x5ff684;
WORD*  const GGXX_1PTENSION	= (WORD*)0x5FF540;
WORD*  const GGXX_2PTENSION	= (WORD*)0x5FF624;
WORD*  const GGXX_1PFAINT	= (WORD*)0x5FF5B0;
WORD*  const GGXX_2PFAINT	= (WORD*)0x5FF694;

DWORD* const GGXX_1PDOWNFLG	= (DWORD*)0x5ff5ce;
DWORD* const GGXX_2PDOWNFLG	= (DWORD*)0x5ff6b2;

DWORD* const GGXX_BTLINFO	= (DWORD*)0x5d2024;

//#define GGXX_1PDOWNFLG2 ((DWORD*)(*GGXX_BTLINFO + 0x04))
//#define GGXX_2PDOWNFLG2 ((DWORD*)(*GGXX_BTLINFO + 0x04 + 0x104))

#define GGXX_1PACT		((DWORD*)(*GGXX_BTLINFO + 0x10))
#define GGXX_2PACT		((DWORD*)(*GGXX_BTLINFO + 0x10 + 0x104))
#define GGXX_1PFRAME	(( WORD*)(*GGXX_BTLINFO + 0xe0))
#define GGXX_2PFRAME	(( WORD*)(*GGXX_BTLINFO + 0xe0 + 0x104))
#define GGXX_1PFRAME2	(( WORD*)(*GGXX_BTLINFO + 0x14))
#define GGXX_2PFRAME2	(( WORD*)(*GGXX_BTLINFO + 0x14 + 0x104))

#define GGXX_1PPOSX ((DWORD*)(*GGXX_BTLINFO + 0x0a4))
#define GGXX_1PPOSY ((DWORD*)(*GGXX_BTLINFO + 0x0a8))
#define GGXX_2PPOSX ((DWORD*)(*GGXX_BTLINFO + 0x0a4 + 0x104))
#define GGXX_2PPOSY ((DWORD*)(*GGXX_BTLINFO + 0x0a8 + 0x104))

HWND* const GGXX_HWND				= (HWND*)0x5f8dfc;

D3DFORMAT* const GGXX_TEXFORMAT		= (D3DFORMAT*)0x5f8e1c;

BYTE*  const GGXX_WINCOUNT1P		= (BYTE*)0x5bd884;
BYTE*  const GGXX_WINCOUNT2P		= (BYTE*)0x5bd885;

DWORD* const GGXX_RANDOMCOUNTER		= (DWORD*)0x5ffb60;
DWORD* const GGXX_RANDOMTABLE		= (DWORD*)0x600540; //0x600540-0x600F04
DWORD  const GGXX_RANDOMTABLESIZE	= 0x9c4;
DWORD* const GGXX_FRAMECOUNTER		= (DWORD*)0x5f9434;
DWORD* const GGXX_ISRANDOMINIT		= (DWORD*)0x5cbfa0;

/* これ合わせないとランダム選択でずれる */
DWORD* const GGXX_RANDOMCSLOG		= (DWORD*)0x580c48; // 4 * 10 byte (init = 0xFFFFFFFF)
DWORD* const GGXX_RANDOMCSLOGPOS	= (DWORD*)0x5cdce8; // 4 byte (init = 0)

/* ボタン設定 */
DWORD* const GGXX_KEYSET_1P		= (DWORD*)0x5ff588;
DWORD* const GGXX_KEYSET_2P		= (DWORD*)0x5ff66c;
DWORD  const GGXX_KEYSETSIZE	= 0x18;

/* キャラセレカーソル記憶 */
WORD* const GGXX_CSSELECT_1P	= (WORD*)0x5f94c8;
WORD* const GGXX_CSSELECT_2P	= (WORD*)0x5f94ca;

WORD  const GGXX_TIME99				= 0x1734;
BYTE*		GGXX_ENABLEPIXELSHADER	= (BYTE*)0x5f9377;
BYTE*		GGXX_FULLSCREEN			= (BYTE*)0x5f9376;

//******************************************************************
// GGXX extend functions
//******************************************************************
/* GGXXから参照される変数・関数はGGXX上のメモリ空間に確保する */
DWORD* const GGXX_ggnf_input				= (DWORD*)0x60a3f8;
DWORD**		 GGXX_ggnv_InputDataPtr			=(DWORD**)0x60a3fc;
DWORD* const GGXX_ggnf_getPalette			= (DWORD*)0x60a400;
DWORD* const GGXX_ggnf_procNetVS			= (DWORD*)0x60a408;
DWORD* const GGXX_ggnf_startCS				= (DWORD*)0x60a40c;
DWORD* const GGXX_ggnf_startBattle			= (DWORD*)0x60a410;
DWORD* const GGXX_ggnv_cfg_enableNet		= (DWORD*)0x60a414;
DWORD* const GGXX_ggnv_cfg_dispInvCmb		= (DWORD*)0x60a418;
DWORD* const GGXX_ggnf_startNetVS			= (DWORD*)0x60a41c;
DWORD* const GGXX_ggnf_vsLoadCompleted		= (DWORD*)0x60a420;
DWORD* const GGXX_ggnf_startVS				= (DWORD*)0x60a424;
DWORD* const GGXX_ggnf_syncRandomTable		= (DWORD*)0x60a428;
DWORD* const GGXX_ggnv_cfg_enableExChara	= (DWORD*)0x60a42c;
DWORD* const GGXX_ggnf_softReset			= (DWORD*)0x60a430;
DWORD* const GGXX_ggnf_drawBattlePlayerName	= (DWORD*)0x60a434;
DWORD* const GGXX_ggnf_endBattle			= (DWORD*)0x60a438;
DWORD* const GGXX_ggnf_drawRankAndWin		= (DWORD*)0x60a43c;
DWORD* const GGXX_ggnf_drawCSPlayerName		= (DWORD*)0x60a440;
DWORD* const GGXX_ggnf_procReplay			= (DWORD*)0x60a444;
DWORD* const GGXX_ggnf_startReplay			= (DWORD*)0x60a448;
DWORD* const GGXX_ggnf_syncKeySetting		= (DWORD*)0x60a44c;
DWORD* const GGXX_ggnf_endCS				= (DWORD*)0x60a450;
DWORD* const GGXX_ggnf_endVS				= (DWORD*)0x60a454;
DWORD* const GGXX_ggnf_randomLog			= (DWORD*)0x60a458;
DWORD* const GGXX_ggnf_useSpecialRandom		= (DWORD*)0x60a45c;
DWORD* const GGXX_ggnf_randomShuffle		= (DWORD*)0x60a460;
DWORD* const GGXX_ggnf_init					= (DWORD*)0x60a464;
DWORD* const GGXX_ggnf_render				= (DWORD*)0x60a468;
DWORD* const GGXX_ggnf_cleanup				= (DWORD*)0x60a46c;
DWORD* const GGXX_ggnv_loadErrMsg			= (DWORD*)0x60a470;
DWORD* const GGXX_ggnv_render_off			= (DWORD*)0x60a474;
char** const GGXX_ggnv_backupdataDir		= (char**)0x60a478;

DWORD const GGXX_ggnv_err_exever			= 0x516c88;	// ggxx.exe version error.
DWORD const GGXX_ggnv_err_netinit			= 0x516ca8;	// failed to initialize network.

//*******************************************************************
// GGXX functions
//*******************************************************************
/* GGXXの関数をggxxnetからコールする */
void GGXX_PlayCmnSound(int p_id)
{
	__asm
	{
		push eax
		mov eax,0x4cb480
		push p_id
		call eax
		add esp,4
		pop eax
	}
}

void GGXX_DrawText1Ex(char* p_str, int p_x, int p_y, float p_z, DWORD p_argb)
{
	__asm
	{
		push eax
		mov eax,0x4fd2c0
		push p_argb
		push p_z
		push p_y
		push p_x
		push p_str
		call eax
		add esp,0x14
		pop eax
	}
}

void GGXX_DrawText2(char* p_str, float p_x, float p_y, int p_type)
{
	__asm
	{
		push eax
		mov eax,0x42e5f0
		push p_type
		push 0x21
		push 0x40000000
		push p_y
		push p_x
		push p_str
		call eax
		add esp,0x18
		pop eax
	}
}

void GGXX_DrawArrowMark(int p_type, int p_x, int p_y, int p_alpha)
{
	__asm
	{
		push eax
		mov eax,0x42ecb0
		push p_alpha
		push 0
		push p_y
		push p_x
		push p_type
		call eax
		add esp,0x14
		pop eax
	}
}

void GGXX_DrawRect(int p_x1, int p_y1, int p_x2, int p_y2, int p_argb)
{
	__asm
	{
		push eax
		mov eax,0x473660
		push p_argb
		push 4
		push p_y2
		push p_x2
		push p_y1
		push p_x1
		call eax
		add esp,0x18
		pop eax
	}
}

void GGXX_InitBattleChara(int p_side)
{
	__asm
	{
		push eax
		mov eax,0x42a7d0
		push p_side
		call eax
		add esp,4
		pop eax
	}
}

void GGXX_InitBattle(void)
{
	__asm
	{
		push eax
		mov eax,0x45ff30
		call eax
		pop eax
	}
}

void GGXX_GetInputFromDI(void)
{
	__asm
	{
		push eax
		mov eax,0x4bd9b0
		call eax
		pop eax
	}
	// アナログ入力をここで統合
	for (int i = 0; i < 2; i++)
	{
		DWORD* analogEnable = (DWORD*)(0x602910 + i * 0x34);
		if (*analogEnable)
		{
			DWORD* analogInput = (DWORD*)(*GGXX_ggnv_InputDataPtr + 1 + i);

			char ay = (char)(*analogInput        & 0xff);
			char ax = (char)((*analogInput >> 8) & 0xff);
			
			if (ay >  64) **GGXX_ggnv_InputDataPtr |= 0x4000 << i*16;
			if (ay < -64) **GGXX_ggnv_InputDataPtr |= 0x1000 << i*16;
			if (ax >  64) **GGXX_ggnv_InputDataPtr |= 0x2000 << i*16;
			if (ax < -64) **GGXX_ggnv_InputDataPtr |= 0x8000 << i*16;
		}
		//if (i==1) DBGOUT("%08x\n", **GGXX_ggnv_InputDataPtr);
	}
}

void GGXX_RebuildRandomTable(void)
{
	__asm
	{
		push eax
		mov eax,0x44c660
		call eax
		pop eax
	}
}

DWORD GGXX_GetRandom(void)
{
	__asm
	{
		mov eax,0x4fd960
		call eax
	}
}

void GGXX_srand(int p_seed)
{
	__asm
	{
		push eax
		push p_seed
		mov eax,0x4cd4df
		call eax
		add esp, 8
	}
}

void GGXX_SetAction(DWORD p_data, int p_actid)
{
	__asm
	{
		push p_actid
		push p_data
		mov eax,0x407490
		call eax
		add esp, 8
		mov eax, p_data
//		mov word ptr [eax + 0xe0], 10	//フレームも変えたい
	}
}
