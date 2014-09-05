/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

// include
#include "../ggxxnet.h"
#include "../node.h"
#include "../netMgr.h"
#include "../zlib.h"
#include "../denylist.h"
#include "../icon.h"
#include "../util.h"

#include "../res/resource.h"

// const
#define DEFAULT_FPS	60
#define DEFAULT_X	20
#define DEFAULT_Y	20

#define WINDOW_W	310
#define WINDOW_H	140

// global
DWORD	g_tester_input;
DWORD*	g_tester_inputPtr = &g_tester_input;	// ggxxとのコード共通化の都合でポインタがいる
DWORD	g_tester_fps;
int		g_tester_port;
HWND	g_tester_wnd;
HDC		g_tester_windc;
HDC		g_tester_backdc;
HBITMAP	g_tester_backbmp;
HBITMAP	g_tester_backoldbmp;

int		g_selectItemIdx;
int		g_itemPerPage;
int		g_dispItemHead;

// proto
BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved);
void initSetting(void);
void ggn_input(void);
void enterServer(bool p_busy);
void readServer(void);
void leaveServer(void);

void readNodeList(void);
bool useLobbyServer(void);

void getscpiptaddr(char* &p_server, char* &p_script);
int  internet_post(char* p_buf, int p_sendsize, int p_bufsize, char* p_server, char* p_target);

void saveReplayFile(void);

void readIniFile(void);
void writeIniFile(void);

// extern
extern DWORD				g_startBattleTime;
extern LPDIRECT3DDEVICE8	g_d3dDev;
extern DWORD**				GGXX_ggnv_InputDataPtr;

// function
void __stdcall tester_input(void)
{
	g_tester_input = 0;
	if (GetAsyncKeyState(VK_UP) & 0x80000000)		g_tester_input |= 0x00001000;
	if (GetAsyncKeyState(VK_DOWN) & 0x80000000)		g_tester_input |= 0x00002000;
	if (GetAsyncKeyState(VK_LEFT) & 0x80000000)		g_tester_input |= 0x00004000;
	if (GetAsyncKeyState(VK_RIGHT) & 0x80000000)	g_tester_input |= 0x00008000;
	
	if (GetAsyncKeyState(VK_NUMPAD8) & 0x80000000)	g_tester_input |= 0x10000000;
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x80000000)	g_tester_input |= 0x20000000;
	if (GetAsyncKeyState(VK_NUMPAD4) & 0x80000000)	g_tester_input |= 0x40000000;
	if (GetAsyncKeyState(VK_NUMPAD6) & 0x80000000)	g_tester_input |= 0x80000000;

	g_tester_input  = (g_netMgr->m_time << 12) & 0xF000;
	g_tester_input |= g_tester_input << 16;
}

void syncfps(int p_fps)
{
	LARGE_INTEGER freq;
	int wait = 1000000 / p_fps;

	if (QueryPerformanceFrequency(&freq))
	{
		static LARGE_INTEGER last = { 0 };
		LARGE_INTEGER now;

		while (1)
		{
			QueryPerformanceCounter(&now);
			if (last.QuadPart == 0) break; // 初回はスルー
			int time = (int)((now.QuadPart - last.QuadPart) * 1000000 / freq.QuadPart);
			if (time > wait) break;
			else if (time < 15000) Sleep(1);
		}
		last = now;
	}
	else
	{
		static DWORD last = 0;
		DWORD now;

		while (1)
		{
			now = timeGetTime();
			if (last == 0) break; // 初回はスルー
			int time = (now - last) * 1000;
			if (time > wait) break;
			else if (time < 15000) Sleep(1);
		}
		last = now;
	}
}

bool tester_getkey(int p_vk)
{
	return (GetAsyncKeyState(p_vk) & 0x80000000 && GetActiveWindow() == g_tester_wnd);
}

LRESULT CALLBACK wndproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 1;
	case WM_ACTIVATE:
		//m_active = ((wp & 0xFFFF) != WA_INACTIVE);
		break;
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

bool getArgValue(char* p_arg, char* p_type, int p_idx, char* p_value)
{
	// xxx.exe -a yyy zzz -b -c iii

	char arg[1024] = " ";
	char typ[32] = " ";
	strcat(arg, p_arg);
	strcat(typ, p_type);
	if (p_value) strcpy(p_value, "");
	
	int idx = p_idx;
	char* ptr = strstr(arg, typ);
	if (ptr)
	{
		char* st = ptr + strlen(typ);
		/* p_value == NULLなら取得しない */
		while (p_value)
		{
			while (*st == ' ' || *st == '\t') st++;
			if (*st == '-' || *st == '\0') break;
			
			char* ed = st;
			while (*ed != '\0' && *ed != ' ' && *ed != '\t') ed++;
			if (*ed == '\0')
			{
				if (p_idx-- == 0)
				{
					strcpy(p_value, st);
				}
				break;
			}
			*ed = '\0';
			
			if (p_idx-- == 0)
			{
				strcpy(p_value, st);
				break;
			}
			st = ed + 1;
		}
		return true; /* find */
	}
	return false; /* not found */
}

void __stdcall tester_draw(void)
{
	const int LINE_H	= 8;
	const int HEADER	= 5;

	RECT		rect = {0,0,0,0};
	
	GetClientRect(g_tester_wnd, &rect);
	HBRUSH brush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	FillRect(g_tester_backdc, &rect, brush);

	SetBkColor(g_tester_backdc, RGB(255, 255, 255));
	SetBkMode(g_tester_backdc, TRANSPARENT);

	SetTextColor(g_tester_backdc, RGB(0, 127, 0));
	rect.top = 0;
	DrawText(g_tester_backdc, "F1:connect F2:disconnect F3:w_ignore F4:watch F5:n/a", -1, &rect, 0);

	SetTextColor(g_tester_backdc, RGB(0, 0, 0));

	char		state[256];
	if (g_netMgr->m_connect)		strcpy(state, "connect   ");
	else if (g_netMgr->m_watch)		strcpy(state, "watch     ");
	else							strcpy(state, "disconnect");

	char		str[256];
	sprintf(str, "time=%.4d frame\nstate=%s %s\nslow=%.2d ms/f",
		g_netMgr->m_time / 10,
		state,
		g_netMgr->m_suspend ? "suspend" : "",
		g_netMgr->m_time == 0 ? 0 : g_netMgr->m_totalSlow / g_netMgr->m_time);
	rect.top = LINE_H;
	DrawText(g_tester_backdc, str, -1, &rect, 0);

	// watch info
	WORD port[] = {
		htons(g_netMgr->m_watchRootAddr[0].sin_port),
		htons(g_netMgr->m_watchRootAddr[1].sin_port),
		htons(g_netMgr->m_watcher[0].m_remoteAddr.sin_port),
		htons(g_netMgr->m_watcher[1].m_remoteAddr.sin_port),
		htons(g_netMgr->m_watcher[2].m_remoteAddr.sin_port),
	};

	if (g_ignoreWatchIn)
	{
		sprintf(str, "root1 = %5d root2 = %5d\nchild1= ----- child2= -----\nchild3= ----- recv  =%6d bytes", port[0], port[1], g_netMgr->m_watchRecvSize);
	}
	else
	{
		sprintf(str, "root1 = %5d root2 = %5d\nchild1= %5d child2= %5d\nchild3= %5d recv  =%6d bytes", port[0], port[1], port[2], port[3], port[4], g_netMgr->m_watchRecvSize);
	}
	rect.left = 140; rect.top = LINE_H;
	DrawText(g_tester_backdc, str, -1, &rect, 0);

	rect.left =   0; rect.top = LINE_H*HEADER;
	DrawText(g_tester_backdc, "-----------------------------------------------------------", -1, &rect, 0);

	rect.left =  10; rect.top = LINE_H*(HEADER+1);
	DrawText(g_tester_backdc, "id", -1, &rect, 0);

	rect.left =  25; rect.top = LINE_H*(HEADER+1);
	DrawText(g_tester_backdc, "name", -1, &rect, 0);

	rect.left =  70; rect.top = LINE_H*(HEADER+1);
	DrawText(g_tester_backdc, "address", -1, &rect, 0);

	rect.left = 150; rect.top = LINE_H*(HEADER+1);
	DrawText(g_tester_backdc, "win", -1, &rect, 0);

	rect.left = 180; rect.top = LINE_H*(HEADER+1);
	DrawText(g_tester_backdc, "state", -1, &rect, 0);

	rect.left = 230; rect.top = LINE_H*(HEADER+1);
	DrawText(g_tester_backdc, "ping", -1, &rect, 0);

	rect.left = 270; rect.top = LINE_H*(HEADER+1);
	DrawText(g_tester_backdc, "ver", -1, &rect, 0);

	//サイズ確認用
	//for (int i = 0; i < 6; i++)
	//{
	//	rect.top = LINE_H*(HEADER+2+i);
	//	rect.left =   0;
	//	DrawText(g_tester_backdc, "test", -1, &rect, 0);
	//}

	int line = 0;
	for (int i = 0; i < g_nodeMgr->getNodeCount(); i++)
	{
		if (i < g_dispItemHead) continue;
		if (i >= g_dispItemHead + g_itemPerPage) break;

		rect.top = LINE_H*(HEADER+2+line);

		rect.left =   0;
		DrawText(g_tester_backdc, (g_selectItemIdx == i) ? ">" : " ", -1, &rect, 0);
		
		rect.left =   10;
		sprintf(str, "%2d", i);
		DrawText(g_tester_backdc, str, -1, &rect, 0);

		rect.left =   25;
		sprintf(str, "%s", g_nodeMgr->getNode(i)->m_name);
		DrawText(g_tester_backdc, str, -1, &rect, 0);

		rect.left =  70;
		DrawText(g_tester_backdc, g_nodeMgr->getNode(i)->m_addr, -1, &rect, 0);
		
		rect.left = 150;
		sprintf(str, "%d", g_nodeMgr->getNode(i)->m_win);
		DrawText(g_tester_backdc, str, -1, &rect, 0);

		rect.left = 180;
		if (g_nodeMgr->getNode(i)->m_state == State_Idle)				strcpy(str, "idle ");
		else if (g_nodeMgr->getNode(i)->m_state == State_Busy)			strcpy(str, "busy ");
		else if (g_nodeMgr->getNode(i)->m_state == State_Mismatch)		strcpy(str, "mis");
		else if (g_nodeMgr->getNode(i)->m_state == State_VersionError)	strcpy(str, "vererr");
		else if (g_nodeMgr->getNode(i)->m_state == State_NotReady)		strcpy(str, "notrdy");
		else if (g_nodeMgr->getNode(i)->m_state == State_NoResponse)	strcpy(str, "nores");
		else if (g_nodeMgr->getNode(i)->m_state == State_Unknown)		strcpy(str, "unknown");
		else if (g_nodeMgr->getNode(i)->m_state == State_PingOver)		strcpy(str, "pingover");
		else if (g_nodeMgr->getNode(i)->m_state == State_Watch)			strcpy(str, "watch");
		else if (g_nodeMgr->getNode(i)->m_state == State_Watch_Playable)strcpy(str, "watch(play)");
		else if (g_nodeMgr->getNode(i)->m_state == State_Busy_Casting)	strcpy(str, "busy(cast)");
		else if (g_nodeMgr->getNode(i)->m_state == State_Busy_Casting_NG)strcpy(str, "busy(castng)");
		else															strcpy(str, "???");
		DrawText(g_tester_backdc, str, -1, &rect, 0);

		rect.left = 230;
		sprintf(str, "%3d ms", g_nodeMgr->getNode(i)->m_ping[0]);
		DrawText(g_tester_backdc, str, -1, &rect, 0);
		
		rect.left = 270;
		sprintf(str, "%s", g_nodeMgr->getNode(i)->m_ver);
		DrawText(g_tester_backdc, str, -1, &rect, 0);

		line++;
	}

	if (g_dispItemHead > 0)
	{
		rect.left = 285; rect.top = 50;
		DrawText(g_tester_backdc, "↑", -1, &rect, 0);
	}

	if (g_nodeMgr->getNodeCount() > g_dispItemHead + g_itemPerPage)
	{
		rect.left = 285; rect.top = 40 + g_itemPerPage * 10;
		DrawText(g_tester_backdc, "↓", -1, &rect, 0);
	}
	BitBlt(g_tester_windc, 0, 0, WINDOW_W, WINDOW_H, g_tester_backdc, 0, 0, SRCCOPY);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE /*hPrevInst*/, LPSTR lpCmdLine, int /*nCmdShow*/)
{
	//int x;
	//x = strcmp("1.20-2", "1.20");
	//x = strcmp("1.15-3", "1.20");

	// 解凍てすと
	//int size;
	//char org[9999];
	//char comp[9999];
	//for (int i = 0; i < 9999; i++) org[i] = i & 0xff;
	//FILE* fp = fopen("test.dat", "rb");
	//size = 7198;//ds_util::fsize32(fp);
	//fread(org, 1, size, fp);
	//fclose(fp);
	//int cmpsize = ds_zlib::zmwrite(org, size, comp, 9999); // == 4731
	//
	//for (int i = 0; i < 9999; i++) org[i] = 0;
	//
	//for (int i = 0; i < 111; i++)
	//{
	//	int z;
	//	for (z = 0; z < 3584; z += 256)
	//	{
	//		int unzsize = ds_zlib::zmread(comp, z, org, 9999);
	//		unzsize=0;
	//	}
	//	ds_zlib::zmread(comp, z, org, 9999);
	//	assert(*((DWORD*)(org+3627)) == 0x49c2c955);
	//}
	//int csz = 0;
	//while (size > 0)
	//{
	//	if (size > 11)
	//	{
	//		csz += 11;
	//		size -= 11;
	//	}
	//	else size = 0;
	//	
	//	int asz = ds_zlib::zmread(comp, csz, org, 9999);
	//}

	readIniFile();
	writeIniFile();
	g_iniFileInfo.m_maxPacketSize = 256;

	initSetting();

	// settingのread/writeテスト
	void readSettingFile(void);
	readSettingFile();
	//void writeSettingFile(void);
	//writeSettingFile();

	//g_denyListMgr = new CDenyListMgr;
	//g_denyListMgr->readfile();
	//char d1[] = "avfdsd";
	//char d2[] = "avfddd";
	//char d3[6];
	//g_denyListMgr->add(d1);
	//getMachineID(d3, "ggxx#r");
	//g_denyListMgr->add(d3);
	////g_denyListMgr->add(mac);
	//g_denyListMgr->savefile();
	//g_denyListMgr->remove(d2);
	//delete g_denyListMgr;

	g_tester_fps = DEFAULT_FPS;
	int winx = DEFAULT_X, winy = DEFAULT_Y;
	
	// 引数解析
	bool find;
	char value[256];
	find = getArgValue(lpCmdLine, "-f", 0, value);
	if (find) g_tester_fps = atoi(value);

	find = getArgValue(lpCmdLine, "-p", 0, value);
	g_tester_port = (find) ? atoi(value) : DEFAULT_PORT;

	find = getArgValue(lpCmdLine, "-b", 0, value);
	g_setting.watchBroadcast = find ? (atoi(value) != 0) : 1;

	find = getArgValue(lpCmdLine, "-i", 0, value);
	g_setting.watchIntrusion = find ? (atoi(value) != 0) : 1;

	find = getArgValue(lpCmdLine, "-s", 0, value);
	g_iniFileInfo.m_maxPacketSize = find ? atoi(value) : 64;

	find = getArgValue(lpCmdLine, "-x", 0, value);
	if (find) winx = atoi(value);

	find = getArgValue(lpCmdLine, "-y", 0, value);
	if (find) winy = atoi(value);
	
	// ウインドウ作成
	WNDCLASS	wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= wndproc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInst;
	wc.hIcon			= LoadIcon(hInst, NULL);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName		= "";
	wc.lpszClassName	= "wc";
	if (RegisterClass(&wc) == FALSE) return 0;
	
	g_tester_wnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_APPWINDOW, "wc", "", ~WS_SYSMENU & WS_OVERLAPPEDWINDOW, winx, winy, WINDOW_W, WINDOW_H, NULL, NULL, hInst, NULL);
	if (g_tester_wnd == NULL) return 0;
	ShowWindow(g_tester_wnd, SW_SHOW);

	g_tester_windc		= GetDC(g_tester_wnd);

	// BITMAPINFO設定
	BITMAPINFO	bmInfo;
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = WINDOW_W;
	bmInfo.bmiHeader.biHeight = -WINDOW_H;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	
	// バックバッファ作成
	void	*backbits;
	g_tester_backdc		= CreateCompatibleDC(g_tester_windc);
	g_tester_backbmp	= CreateDIBSection(NULL, &bmInfo, DIB_RGB_COLORS, &backbits, NULL, 0);
	g_tester_backoldbmp	= (HBITMAP)SelectObject(g_tester_backdc, g_tester_backbmp);

	/* フォントの初期化 */
	LOGFONT	logFont;
	logFont.lfHeight      = 10;
	logFont.lfWidth       = 0;
	logFont.lfEscapement  = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight      = FW_NORMAL;
	logFont.lfItalic      = FALSE;
	logFont.lfUnderline   = FALSE;
	logFont.lfStrikeOut   = FALSE;
	logFont.lfCharSet     = SHIFTJIS_CHARSET;
	logFont.lfOutPrecision  = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality        = DEFAULT_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH || FF_DONTCARE;
	strcpy(logFont.lfFaceName, "ＭＳ ゴシック");
	HFONT font = CreateFontIndirect(&logFont);
	HGDIOBJ oldobj = SelectObject(g_tester_backdc, font);

	// ウインドウタイトルに情報を表示
	char str[32] = "";
	sprintf(str, "port:%d", g_tester_port);
	SetWindowText(g_tester_wnd, str);

	sprintf(str, "%d", g_tester_port);
	strcpy(g_setting.userName, str);

	//strcpy(g_setting.scriptAddress, "www33.tok2.com/home/x200612/ggnlobby.cgi");
	//strcpy(g_setting.scriptAddress, "localhost/ggnlobby.cgi");
	//strcpy(g_setting.scriptAddress, "localhost/cgi-bin/ggn/ggnlobby.cgi");
	//strcpy(g_setting.scriptAddress, "www.nurupodensetsu.com/ggn/delay2/ggnlobby.cgi");
	//strcpy(g_setting.scriptAddress, "nodelist.txt");
	
	// 転送確認のため適当なデータをセット
	__strncpy(g_replay.m_data.name1P, "test1p", 29);
	__strncpy(g_replay.m_data.name2P, "test2p", 29);
	for (int i = 0; i < 0x9c4; i++) g_replay.m_data.randTable[i] = (rand() & 0xff);
	for (int i = 0; i < 0x410; i++) g_replay.m_data.palette1P[i] = (rand() & 0xff);
	for (int i = 0; i < 0x410; i++) g_replay.m_data.palette2P[i] = (rand() & 0xff);

	g_setting.wait		= 1;
	g_setting.port		= g_tester_port;
	g_setting.delay		= DEFAULT_DELAY;
	g_setting.useEx		= 0;
	sprintf(g_setting.msg, "%s:comment", g_setting.userName);
	GGXX_ggnv_InputDataPtr = &g_tester_inputPtr;
	g_itemPerPage = 6;
	
	if (DllMain(0, DLL_PROCESS_ATTACH, 0) == false)
	{
		APPERR("failed to initialize network!!");
		goto NETWORK_ERROR;
	}
	g_netMgr->m_networkEnable = true;
	g_netMgr->m_lobbyFrame = 0;

	int testerTimer = 0;

	SetTimer(g_tester_wnd, 0, 100, NULL);
	
	int connectFrame = 0;

	bool done = false;
	while (!done)
	{
		g_netMgr->m_lobbyFrame++;

		ggn_input();

		if (useLobbyServer())
		{
			if (testerTimer % (60 * 60) == 0)	// 60 sec
			{
				// 放っておくと切れてしまうので定期的に再エントリ
				enterServer(g_netMgr->m_connect);
			}
			if (testerTimer % (30 * 60) == 15)	// 30 sec
			{
				readServer();
			}
		}
		testerTimer++;

		static bool oldConnect = false;
		if (g_netMgr->m_connect && !oldConnect)
		{
			connectFrame = 0;
			// 接続成功！！
			g_netMgr->resume();
/*ブロック転送のテスト*/
#if 1
			char* sendbuf = new char[3000];
			char* recvbuf = new char[3000];
			for (int i = 0; i < 3000; i++)
			{
				sendbuf[i] = (rand() & 0xff);
				recvbuf[i] = 0;
			}
			if (g_netMgr->m_playSide == 1)
			{
				g_netMgr->sendDataBlock(Block_TestData, (char*)sendbuf, 3000, TIMEOUT_BLOCK);
				g_netMgr->recvDataBlock(Block_TestData, (char*)recvbuf, 3000, TIMEOUT_BLOCK);
			}
			else
			{
				g_netMgr->recvDataBlock(Block_TestData, (char*)recvbuf, 3000, TIMEOUT_BLOCK);
				g_netMgr->sendDataBlock(Block_TestData, (char*)sendbuf, 3000, TIMEOUT_BLOCK);
			}
			if (memcmp(sendbuf, recvbuf, 3000) != 0)
			{
				g_netMgr->disconnect("send block data was broken.");
			}
			delete[] sendbuf;
			delete[] recvbuf;
#endif
			g_startBattleTime = timeGetTime() / 1000;
			g_replay.m_repRecording = true;
			g_replay.m_frameCount = 0;
		}
		oldConnect = g_netMgr->m_connect;

// 入力処理

		if (!g_netMgr->m_connect && tester_getkey(VK_F1) && g_selectItemIdx != -1)
		{
			// 接続要求を送信
			if (g_selectItemIdx < g_nodeMgr->getNodeCount())
			{
				g_netMgr->m_remoteAddr_active = g_netMgr->getAddrFromString(g_nodeMgr->getNode(g_selectItemIdx)->m_addr);
				
				g_netMgr->send_connect(&g_netMgr->m_remoteAddr_active);
			}
		}
		if ((g_netMgr->m_connect || g_netMgr->m_watch) && tester_getkey(VK_F2))
		{
			// 強制切断
			g_netMgr->disconnect("tester F2");
		}
		if (tester_getkey(VK_F3))
		{
			//// Serverから再取得
			//if (useLobbyServer()) readServer();
			//else readNodeList();
			//g_nodeMgr->sortNodeList(SORT_STATUS_A);
			g_ignoreWatchIn = !g_ignoreWatchIn;
			Sleep(200);
		}
		if (!g_netMgr->m_connect && !g_netMgr->m_watch && tester_getkey(VK_F4) && g_selectItemIdx != -1)
		{
			if (g_selectItemIdx < g_nodeMgr->getNodeCount())
			{
				sockaddr_in targetIP = g_netMgr->getAddrFromString(g_nodeMgr->getNode(g_selectItemIdx)->m_addr);

				g_netMgr->m_watch = false;
				g_netMgr->m_watchRootAddr[0] = NULL_ADDR;
				g_netMgr->m_watchRootAddr[1] = NULL_ADDR;
				g_netMgr->m_watchRecvComplete = false;
				g_netMgr->m_watchRecvSize = 0;
				for (int i = 0; i < MAXREPSIZE; i++) g_replay.m_data.inputData[i] = 0xffffffff;

				g_netMgr->watch(g_nodeMgr->getNode(g_selectItemIdx)->m_name, &targetIP, 0, false);
			}
		}
		//if (!g_netMgr->m_connect && tester_getkey(VK_F5))
		//{
		//	// 確認用
		//	memset(&g_replay.m_data, 0x00, sizeof(ReplayFile));
		//}

		if (tester_getkey(VK_UP))
		{
			if (g_selectItemIdx > 0)
			{
				g_selectItemIdx--;
				if (g_selectItemIdx < g_dispItemHead) g_dispItemHead--;
				Sleep(100);
			}
		}
		if (tester_getkey(VK_DOWN))
		{
			if (g_selectItemIdx < g_nodeMgr->getNodeCount() - 1)
			{
				g_selectItemIdx++;
				if (g_selectItemIdx >= g_dispItemHead + g_itemPerPage) g_dispItemHead++;
				Sleep(100);
			}
		}
/////////////////////////////////////////

#if 0	// lobbyEnter/LeaveTest 
		if (rand() % 2 == 0)
		{
			// ロビーのテスト用にエンターだけ行う
			char *server, *script;
			getscpiptaddr(server, script);

			char buf[1024];
			sprintf(buf, "cmd=enter|port=%d|name=%s|param=%d%02x%d%d%d%d%d%d%c%d|win=%d",
				rand() % 20 + 1024, "test", 1, 0x02, 0, 0, 0, 0, 0, 0, 'S', 2, 99);
			internet_post(buf, strlen(buf), 1024, server, script);
			Sleep(10);
		}
		else
		{
			// ロビーのテスト用にリーブだけ行う
			char *server, *script;
			getscpiptaddr(server, script);

			char buf[256];
			sprintf(buf, "cmd=leave|port=%d|name=%s", rand() % 20 + 1024, "test");
			internet_post(buf, strlen(buf), 256, server, script);
			Sleep(10);
		}
#endif

#if 0	// Suspend/ResumeTest (通信相手と同じフレームでSuspend)
		if (g_netMgr->m_connect && connectFrame % 300 == 180)
		{
			g_netMgr->suspend();

			// TCPによるデータ受信（接続中のみ可能）
			if (g_netMgr->m_playSide == 1)
			{
				char *data = new char[10240];
				for (int i = 0; i < 10240; i++) data[i] = i & 0xFF;
				if (g_netMgr->sendDataBlock(Block_TestData, data, 10240, 3))
				{
					OutputDebugString("udp_sendData ok\n");
				}
				else OutputDebugString("udp_sendData failed!!\n");
				delete data;
			}
			else
			{
				char *data = new char[10240];
				for (int i = 0; i < 10240; i++) data[i] = 5;
				if (g_netMgr->recvDataBlock(Block_TestData, data, 10240, 3))
				{
					bool baddata = false;
					for (int i = 0; i < 10240; i++)
					{
						if (data[i] != (i & 0xFF)) baddata = true;
					}
					OutputDebugString(baddata ? "udp_recvData damaged!!\n" : "udp_recvData ok\n");
				}
				else OutputDebugString("udp_recvData failed!!\n");
				delete data;
			}
		}
		if (g_netMgr->m_connect && connectFrame % 300 == 220)
		{
			g_netMgr->resume();
		}
		connectFrame++;
#endif

		tagMSG	msg;
		while (PeekMessage(&msg, g_tester_wnd, 0, 0, PM_REMOVE))
		{
			if (msg.hwnd != g_tester_wnd) continue;
			if (msg.message == WM_QUIT || msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
			{
				done = true;
			}
			else if (msg.message == WM_TIMER)
			{
				tester_draw();
			}
			else { TranslateMessage(&msg); DispatchMessage(&msg); }
		}
		syncfps(g_tester_fps);
	}

	WRITE_DEBUGLOG("END TESTER");

	//WRITE_REPLAY_RAWDATA();

	KillTimer(g_tester_wnd, 0);
	
	leaveServer();

NETWORK_ERROR:

	DllMain(0, DLL_PROCESS_DETACH, 0);

	SelectObject(g_tester_backdc, g_tester_backoldbmp);
	DeleteDC(g_tester_backdc);
	DeleteObject(g_tester_backbmp);

	SelectObject(g_tester_backdc, oldobj);
	ReleaseDC(g_tester_wnd, g_tester_windc);
	

	DestroyWindow(g_tester_wnd);
	UnregisterClass("wc", hInst);

	return 0;
}
