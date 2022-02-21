/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

//******************************************************************
// include
//******************************************************************
#include "ggxxnet.h"
#include "node.h"
#include "netMgr.h"
#include "zlib.h"
#include "denylist.h"
#include "sharedMemory.h"
#include "internet.h"
#include "icon.h"
#include "rc4.h"
#include "md5.h"
#include "d3dfont.h"
#include "util.h"
#include "res/resource.h"

#include <mbstring.h>
#include <d3d8.h>
#include <math.h>

#include <iostream>
#include <string>
#include <fstream>
#include <string.h>
std::string getRadminAddress() {
	std::string line;
	std::ifstream IPFile;
	int offset;
	char search0[] = "IPv4 Address. . . . . . . . . . . :";      // search pattern
	char search1[] = "Ethernet adapter Radmin VPN:";      // search pattern
	system("ipconfig > ip.txt");
	bool foundRadmin = false;
	IPFile.open("ip.txt");
	if (IPFile.is_open())
	{
		while (!IPFile.eof())
		{
			getline(IPFile, line);
			if ((offset = line.find(search1, 0)) != std::string::npos) {
				foundRadmin = true;
			}
			if (foundRadmin == true && (offset = line.find(search0, 0)) != std::string::npos)
			{
				line.erase(0, 39);
				break;
			}
		}
	}
	IPFile.close();
	remove("ip.txt");
	return line;
}
//******************************************************************
// const
//******************************************************************
char g_paletteNames[CHARACOUNT][256] = {
	"P(Default)",
	"K",
	"S",
	"HS",
	"Start",
	"D",
	"P(Sp)",
	"K(Sp)",
	"S(Sp)",
	"HS(Sp)",
	"Start(Sp)",
	"D(Sp)",
};

char g_charaNames[CHARACOUNT][256] = {
	"Sol",
	"Ky",
	"May",
	"Millia",
	"Axl",
	"Potemkin",
	"Chipp",
	"Eddie",
	"Baiken",
	"Faust",
	"Testament",
	"Jam",
	"Anji",
	"Johnny",
	"Venom",
	"Dizzy",
	"Slayer",
	"I-No",
	"Zappa",
	"Bridget",
	"Robo-Ky",
	"Kliff",
	"Justice",
};

char g_sortstr[32][256] = {
	"Name  Å™", "Name  Å´",
	"Rank  Å™", "Rank  Å´",
	"Win   Å™", "Win   Å´",
	"Count Å™", "Count Å´",
	"Ping    ", "Ex      ",
	"Watch   ", "Status  "
};

class D3DV_GGN
{
public:
	enum { FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1) };
	D3DV_GGN(void)
	{
		m_x			= 0.0f;
		m_y			= 0.0f;
		m_z			= 1.0f;
		m_rhw		= 1.0f;
		m_diffuse	= 0x00000000;
		m_u			= 0.0f;
		m_v			= 0.0f;
	}
	~D3DV_GGN(void) {}
	
	void setPos(float p_x, float p_y)	{ m_x = p_x; m_y = p_y; }
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
// global
//******************************************************************
CVsNetModeInfo		g_vsnet;
CReplayModeInfo		g_replay;
EnemyInfo			g_enemyInfo;
SettingInfo			g_setting;
CD3DFont*			g_d3dfont;
LPDIRECT3DDEVICE8	g_d3dDev;
CIcon*				g_dirIcon;
DWORD*				g_myPalette[CHARACOUNT][PALCOUNT];
CSharedMemory*		g_smPallette = NULL;
IniFileInfo			g_iniFileInfo;

HINSTANCE			g_dllInst = NULL;
char				g_machineID[10];
DWORD				g_scriptCode;
DWORD				g_startBattleTime;
WORD				g_oldCS = 1;

//-------------------------------------------------------debug
CRITICAL_SECTION	g_csLogOut;
char*				g_netLog = NULL;
char*				g_keyLog = NULL;
char*				g_rndLog = NULL;
char				g_syncErrLog[10][256];
char				g_moduleDir[256];
CCpuID				g_cpuid;
bool				g_ignoreWatchIn = false;

//******************************************************************
// proto types
//******************************************************************
void ggn_syncRandomTable(int p_timing);

void enterServer(bool p_busy);
void readServer(void);
void leaveServer(void);

void readNodeList(void);

void getscpiptaddr(char* &p_server, char* &p_script);
void replaceUserPalette(int p_chara, int p_pal, char* p_data);
void readUserPalette(void);
void deleteUserPalette(void);
void initUserPalette(void);
void initSetting(void);

void readIniFile(void);
void writeIniFile(void);
void addScore(char p_mywc, char p_enwc, char p_enRank, WORD p_enwin);
char getRankChar(int p_rank);

void saveReplayFile(void);
void getReplayFileList(char* p_dir);

void drawGGXXWindow(char* p_str, int p_select, int p_left, int p_top, int p_right, int p_bottom);

BYTE getSyncCheckValue(void);
void getMachineID(char* p_id, char* p_key);

void getWindowSize(int p_clientw, int p_clienth, int* p_windoww, int* p_windowh);

void drawText(char* p_str, int p_x, int p_y, DWORD p_color, CD3DFont::EAlign p_align);

#if TESTER
	void __stdcall tester_input(void);
#endif

#include "ggxxinterface.h"

//******************************************************************
// function
//******************************************************************
BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	//BYTE id1[10] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff };
	//BYTE id2[10] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xef, 0xff };
	//bool result = 0;
	//result = idcmp(id1, id2);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		ENABLE_DUMP_MEM_LEAK;

		g_dllInst = hDLL;

		InitializeCriticalSection(&g_csLogOut);
#if DEBUG_OUTPUT_NET
		g_netLog = new char[LOG_SIZE];
		memset(g_netLog, 0, LOG_SIZE);
#endif
#if DEBUG_OUTPUT_KEY
		g_keyLog = new char[LOG_SIZE];
		memset(g_keyLog, 0, LOG_SIZE);
#endif
#if DEBUG_OUTPUT_RND
		g_rndLog = new char[LOG_SIZE];
		memset(g_rndLog, 0, LOG_SIZE);
#endif

#if DEBUG_OUTPUT_LOG
		/* ÉçÉOÉtÉ@ÉCÉãÇÉNÉäÉA */
		FILE *fp = fopen("log.txt", "w");
		if (fp) fclose(fp);
#endif
		GetCurrentDirectory(256, g_moduleDir);

#if !TESTER
		initSetting();
		readIniFile();
		readSettingFile();

		// ?ÉâÉìÉNëŒçÙ
		if (g_setting.rank < Rank_S || g_setting.rank > Rank_F)
		{
			DBGOUT_LOG("initialized ggn_setting.dat, because it is broken!!\n");
			g_setting.rank			= Rank_F;
			g_setting.wins			= 0;
			g_setting.totalBattle	= 0;
			g_setting.totalDraw		= 0;
			g_setting.totalError	= 0;
			g_setting.totalLose		= 0;
			g_setting.totalWin		= 0;
		}
		g_smPallette = new CSharedMemory("ggxxnet_pal", 1024);

#if !_DEBUG
		/* àÍâûÉfÉoÉbÉKÉ`ÉFÉbÉN */
		if (IsDebuggerPresent()) g_setting.enableNet = false;

		/* ggxx.exeÇÉ`ÉFÉbÉN */
		BYTE md5a[33];
		BYTE md5b[33] = "5bd13ec7c7bb4d8de35c316d108883fb";
		getFileMD5("ggxx.exe", md5a);
		if (memcmp(md5a, md5b, 32) != 0)
		{
			*GGXX_ggnv_loadErrMsg = (DWORD)GGXX_ggnv_err_exever;
			return 0;
		}
#endif

#endif
		DBGOUT_LOG("readSetting ok!!\n");

		g_netMgr = new CNetMgr;
		if (g_netMgr->init(g_setting.port, g_setting.delay, useLobbyServer()) == false)
		{
#if !TESTER
			DBGOUT_LOG("netMgr init failed!!\n");
			*GGXX_ggnv_loadErrMsg = (DWORD)GGXX_ggnv_err_netinit;
#endif
			return FALSE;
		}
		DBGOUT_LOG("netMgr init ok!!\n");

		g_nodeMgr = new CNodeMgr;
		DBGOUT_LOG("nodeMgr init ok!!\n");
		
		g_denyListMgr = new CDenyListMgr;
		g_denyListMgr->readfile();

		/* ñ≥éãÉmÅ[ÉhéØï ópÇ…MACÉAÉhÉåÉXÇégóp */
		getMachineID(g_machineID, "ggxx#reload");

		initUserPalette();
		readUserPalette();
		DBGOUT_LOG("read palette ok!!\n");

		g_netMgr->startThread();

		timeBeginPeriod(1);

		g_d3dDev = NULL;
		g_d3dfont = NULL;
		g_dirIcon = NULL;

#if !TESTER

		*GGXX_ggnf_input			= (DWORD)GetProcAddress(hDLL, "ggn_input");
		*GGXX_ggnf_getPalette		= (DWORD)GetProcAddress(hDLL, "ggn_getPalette");
		*GGXX_ggnf_procNetVS		= (DWORD)GetProcAddress(hDLL, "ggn_procNetVS");
		*GGXX_ggnf_startCS			= (DWORD)GetProcAddress(hDLL, "ggn_startCS");
		*GGXX_ggnf_startNetVS		= (DWORD)GetProcAddress(hDLL, "ggn_startNetVS");
		*GGXX_ggnf_vsLoadCompleted	= (DWORD)GetProcAddress(hDLL, "ggn_vsLoadCompleted");
		*GGXX_ggnf_startBattle		= (DWORD)GetProcAddress(hDLL, "ggn_startBattle");
		*GGXX_ggnf_startVS			= (DWORD)GetProcAddress(hDLL, "ggn_startVS");
		*GGXX_ggnf_syncRandomTable	= (DWORD)GetProcAddress(hDLL, "ggn_syncRandomTable");
		
		*GGXX_ggnf_softReset			= (DWORD)GetProcAddress(hDLL, "ggn_softReset");
		*GGXX_ggnf_drawBattlePlayerName	= (DWORD)GetProcAddress(hDLL, "ggn_drawBattlePlayerName");
		*GGXX_ggnf_endBattle			= (DWORD)GetProcAddress(hDLL, "ggn_endBattle");
		*GGXX_ggnf_drawRankAndWin		= (DWORD)GetProcAddress(hDLL, "ggn_drawRankAndWin");
		*GGXX_ggnf_drawCSPlayerName		= (DWORD)GetProcAddress(hDLL, "ggn_drawCSPlayerName");
		*GGXX_ggnf_procReplay			= (DWORD)GetProcAddress(hDLL, "ggn_procReplay");
		*GGXX_ggnf_syncKeySetting		= (DWORD)GetProcAddress(hDLL, "ggn_syncKeySetting");
		
		*GGXX_ggnf_startReplay			= (DWORD)GetProcAddress(hDLL, "ggn_startReplay");
		*GGXX_ggnf_endCS				= (DWORD)GetProcAddress(hDLL, "ggn_endCS");
		*GGXX_ggnf_endVS				= (DWORD)GetProcAddress(hDLL, "ggn_endVS");
		*GGXX_ggnf_randomLog			= (DWORD)GetProcAddress(hDLL, "ggn_randomLog");
		*GGXX_ggnf_useSpecialRandom		= (DWORD)GetProcAddress(hDLL, "ggn_useSpecialRandom");
		*GGXX_ggnf_randomShuffle		= (DWORD)GetProcAddress(hDLL, "ggn_randomShuffle");
		*GGXX_ggnf_init					= (DWORD)GetProcAddress(hDLL, "ggn_init");
		*GGXX_ggnf_render				= (DWORD)GetProcAddress(hDLL, "ggn_render");
		*GGXX_ggnf_cleanup				= (DWORD)GetProcAddress(hDLL, "ggn_cleanup");

		*GGXX_ggnv_cfg_enableNet	= (DWORD)&g_setting.enableNet;
		*GGXX_ggnv_cfg_dispInvCmb	= (DWORD)&g_setting.dispInvCombo;
		*GGXX_ggnv_cfg_enableExChara= 0;
		*GGXX_ggnv_render_off		= 0;
#endif
#if !TESTER
		*GGXX_ggnv_backupdataDir = new char[256];
		sprintf(*GGXX_ggnv_backupdataDir, "%s/backupdata", g_moduleDir);
#if _DEBUG // iniÉtÉ@ÉCÉãÇ≈éwíËÇµÇΩÉpÉXÇ…ë∂ç›Ç∑ÇÈÉfÅ[É^ÇéQè∆Ç∑ÇÈ
		SetCurrentDirectory(g_iniFileInfo.m_dataDir);
#endif
#endif
		DBGOUT_LOG("dll load ok!!\n");
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
#if !TESTER
		WRITE_DEBUGLOG("exit");
#if _DEBUG
		SetCurrentDirectory(g_moduleDir);
#endif
		if (*GGXX_ggnv_backupdataDir != (char*)0x51ab00)
		{
			delete[] *GGXX_ggnv_backupdataDir;
			*GGXX_ggnv_backupdataDir = (char*)0x51ab00; // å≥ÅXÇÃíl"backupdata"ÇéwÇ∑
		}
		writeIniFile();
#endif
		if (g_d3dfont) { delete g_d3dfont; g_d3dfont = NULL; }

		void ggn_cleanup(void);
		ggn_cleanup();

		timeEndPeriod(1);

		if (g_dirIcon) { delete g_dirIcon; g_dirIcon = NULL; }
		if (g_netMgr) { delete g_netMgr; g_netMgr = NULL; }
		if (g_nodeMgr) { delete g_nodeMgr; g_nodeMgr = NULL; }
		if (g_denyListMgr) { delete g_denyListMgr; g_denyListMgr = NULL; }
		if (g_smPallette) delete g_smPallette;

		deleteUserPalette();

		DeleteCriticalSection(&g_csLogOut);

		if (g_keyLog) delete g_keyLog;
		if (g_netLog) delete g_netLog;
		if (g_rndLog) delete g_rndLog;

		DBGOUT_LOG("ggn terminated!!\n");
	}

	return TRUE;
}

void ggn_input(void)
{
	//GGXX_srand(0);

	/*
	è≠êîÇÃä€ÇﬂåÎç∑Ç…ÇÊÇÈìØä˙ÉYÉåÇ…ëŒèàÇ∑ÇÈÇΩÇﬂCWÇÉZÉbÉg
	î≠ê∂èåèÇ™HttpSendRequestÇåƒÇÒÇæèÍçáÇæÇ™
	ïKÇ∏î≠ê∂Ç∑ÇÈÇÌÇØÇ≈Ç‡Ç»Ç≠Ç±Ç±ÇæÇØÇ≈ÇÕïsà¿
	îOÇÃÇΩÇﬂget_Input()Ç≈Ç‡çsÇ¢ÅAñàÉtÉåÅ[ÉÄåƒÇŒÇÍÇÈÇÊÇ§Ç…ÇµÇƒÇ®Ç≠
	*/
	SETFCW(DEFAULT_CW);

#if !TESTER
	if (*GGXX_MODE1 & 0xe00000 && g_netMgr->m_errMsg[0] != '\0')
	{
		if (g_netMgr->m_errMsgTime++ > 120)
		{
			if (*GGXX_MODE1 & 0x200000)		 g_netMgr->disconnect(g_netMgr->m_errMsg);
			else if (*GGXX_MODE1 & 0x400000) *GGXX_MODE2 = 0x38;
			else if (*GGXX_MODE1 & 0x800000) g_netMgr->disconnect(g_netMgr->m_errMsg);
			g_netMgr->setErrMsg("");	// ÉÅÉbÉZÅ[ÉWÉNÉäÉA
		}
		**GGXX_ggnv_InputDataPtr = 0;
		g_replay.m_skipFrame = 0;
		*GGXX_ggnv_render_off = 0;
		return;
	}
#endif

	// äœêÌé“Ç…ÉfÅ[É^ëóêMÇ∑ÇÈÅBÅiì¡Ç…ñàâÒëóÇÈïKóvÇ‡Ç»Ç¢ÇÃÇ≈1FÇ≤Ç∆Ç…ÉçÅ[ÉeÅ[ÉgÅj
	
	// êÿífÇ≥ÇÍÇΩèuä‘Ç…ëóÇÁÇ»Ç≠Ç»ÇÈÇÃÇÕÇ‹Ç∏Ç¢
	// àÍå©ìÆÇ¢ÇƒÇ¢ÇÈÇÊÇ§Ç≈ç≈å„ÇÃÉfÅ[É^Ç™ëóÇÁÇÍÇƒÇ¢Ç»Ç¢
	//if (g_netMgr->m_connect || g_netMgr->m_watch)
	{
		static int sendidx = 0;

		ENTERCS(&g_netMgr->m_csWatch);

		if (g_netMgr->m_watcher[sendidx].isActive())
		{
			// ëóêMíºå„Ç©ÇÁÇPïbë“Ç¡ÇƒÇ‡ï‘ìöÇ™Ç»Ç¢
			if (g_netMgr->m_watcher[sendidx].m_sendTime != 0xffffffff &&
				timeGetTime() - g_netMgr->m_watcher[sendidx].m_sendTime > TIMEOUT_WATCHDATAREPLY)
			{
				g_netMgr->m_watcher[sendidx].init();
			}
			else
			{
				g_netMgr->send_watchData(sendidx);
			}
		}

		LEAVECS(&g_netMgr->m_csWatch);

		sendidx = (sendidx + 1) % WATCH_MAX_CHILD;
	}

	// äœêÌíÜÇ≈1ïbà»è„ÉfÅ[É^Ç™óàÇ»Ç¢èÍçáÇÕÅAÇQïbä‘äuÇ≈çƒìxPacket_WatchInÇÇ∑Ç◊ÇƒÇÃäœêÌíÜÉmÅ[ÉhÇ…ëóÇËçƒê⁄ë±ÇééÇ›ÇÈ
	static int lasttime = 0;
	int time = timeGetTime() - g_netMgr->m_lastWatchDataTime;
	if (g_netMgr->m_watch && g_netMgr->m_watchRecvComplete == false &&
		time > TIMEOUT_WATCHDATA && (timeGetTime() - lasttime) > WATCH_RESUME_INTERVAL)
	{
		DBGOUT_NET("watch data timeout %d sec\n", time / 1000);
		if (g_netMgr->watch(g_netMgr->m_watchRootName[0], &g_netMgr->m_watchRootAddr[0], g_netMgr->m_watchRootGameCount[0], false) == false)
		{
			g_netMgr->m_watchFailCount++;
			if (g_netMgr->m_watchFailCount == 5)
			{
				// àÍíËâÒêîé∏îsÇµÇΩÇÁÇ«Ç§Ç∑ÇÈÅH
			}
		}
		else g_netMgr->m_watchFailCount = 0;

		lasttime = timeGetTime();
	}

	if (g_netMgr->m_connect && g_netMgr->m_suspend)
	{
		/* vsloadà»äOÇ≈í∑éûä‘suspendÇµÇƒÇ¢ÇΩÇÁêÿíf */
		if (g_netMgr->m_vsloadFrame == -1 && MSPF_INT * g_netMgr->m_suspendFrame++ > TIMEOUT_SUSPEND)
		{
			g_netMgr->disconnect("suspend freeze");
		}
		
		/* suspendíÜÇ≈Ç‡recvSuspendÇ™trueÇ…Ç»ÇÈÇ‹Ç≈ÉLÅ[ÇëóÇËë±ÇØÇÈ */
		if (g_netMgr->m_recvSuspend == false)
		{
			g_netMgr->send_key(g_netMgr->m_time);
		}
		**GGXX_ggnv_InputDataPtr = 0;
	}
	else
	{
#if !TESTER
		if (*GGXX_MODE1 & 0x400000 && *GGXX_MODE2 == 6) /* Replay(ëŒêÌíÜ) */
		{
			GGXX_GetInputFromDI();

			if (**GGXX_ggnv_InputDataPtr & 0x00400040)
			{
				// çƒê∂íÜé~
				for (int i = 0; i < g_netMgr->m_queueSize; i++)
				{
					g_netMgr->m_key[i] = **GGXX_ggnv_InputDataPtr;
				}
				*GGXX_MODE2 = 0x38;
			}
			else if ((**GGXX_ggnv_InputDataPtr & 0xffff0000) == 0x090f0000 ||
					 (**GGXX_ggnv_InputDataPtr & 0x0000ffff) == 0x0000090f)
			{
				// soft reset
				for (int i = 0; i < g_netMgr->m_queueSize; i++)
				{
					g_netMgr->m_key[i] = **GGXX_ggnv_InputDataPtr;
				}
			}
			else
			{
				if (**GGXX_ggnv_InputDataPtr & 0x00200020 && g_replay.m_skipFrame == 0 && g_netMgr->m_errMsg[0] == '\0')
				{
					// ëÅëóÇË
					g_replay.m_skipFrame = 5;
				}
				// ä™Ç´ñﬂÇµÇÕñ≥óù
				//else if (**GGXX_ggnv_InputDataPtr & 0x80008000 && g_replay.m_skipFrame == 0)
				//{
				//	//GGXX_InitBattle();
				//	//GGXX_InitBattleChara(0);
				//	//GGXX_InitBattleChara(1);
				//	_asm
				//	{
				//		push eax

				//		push 0x00566CB4
				//		push 0x38
				//		push 2
				//		push 0x10000
				//		push 0x4C7E60
				//		mov eax, 0x0045F8DB
				//		call eax


				//		//push 0x0056365c
				//		//push 0x1111
				//		//push 1
				//		//push 0x2000
				//		//push 0x0043d890
				//		//mov eax, 0x004cbc70
				//		//call eax

				//		//push 0x0056363c
				//		//push 0x1111
				//		//push 1
				//		//push 0x2000
				//		//push 0x0043d7b0
				//		//mov eax, 0x004cbc70
				//		//call eax
				//		//add esp, 0x14

				//		//mov eax, 0x0046AC40
				//		//call eax

				//		//push 0x00585E24
				//		//push 0xe5001111
				//		//push 1
				//		//push 0x600
				//		//push 0x43D390
				//		//mov eax, 0x004cbc70
				//		//call eax
				//		//add esp, 0x14

				//		pop eax
				//	}
				//	//g_replay.m_skipFrame = g_replay.m_frameCount;
				//	g_replay.m_frameCount = 0;
				//	*GGXX_MODE2 = 6;
				//	return;
				//}
				/* replayÉfÅ[É^Ç©ÇÁÇ±ÇÃÉtÉåÅ[ÉÄÇ≈égópÇ∑ÇÈì¸óÕÇéÊÇËèoÇ∑ */
				if (g_replay.m_data.inputData[g_replay.m_frameCount] == 0xffffffff)
				{
					**GGXX_ggnv_InputDataPtr = 0;
					for (int i = 0; i < g_netMgr->m_queueSize; i++) g_netMgr->m_key[i] = 0;
					g_netMgr->setErrMsg("END OF REPLAY ?");
				}
				else if (g_replay.m_data.inputData[g_replay.m_frameCount] == INPUT_SYNCERROR)
				{
					**GGXX_ggnv_InputDataPtr = 0;
					for (int i = 0; i < g_netMgr->m_queueSize; i++) g_netMgr->m_key[i] = 0;
					g_netMgr->setErrMsg("SYNC ERROR!!");
				}
				else if (g_replay.m_data.inputData[g_replay.m_frameCount] == INPUT_DISCONNECT)
				{
					**GGXX_ggnv_InputDataPtr = 0;
					for (int i = 0; i < g_netMgr->m_queueSize; i++) g_netMgr->m_key[i] = 0;
					g_netMgr->setErrMsg("DISCONNECT!!");
				}
				else if (g_replay.m_data.inputData[g_replay.m_frameCount] == INPUT_COMPLETE)
				{
					**GGXX_ggnv_InputDataPtr = 0;
					g_netMgr->setErrMsg("END OF REPLAY");
				}
				else
				{
					**GGXX_ggnv_InputDataPtr = g_replay.m_data.inputData[g_replay.m_frameCount];
					if ((**GGXX_ggnv_InputDataPtr & 0xffff0000) == 0x090f0000 ||
						(**GGXX_ggnv_InputDataPtr & 0x0000ffff) == 0x0000090f)
					{
						if ((**GGXX_ggnv_InputDataPtr & 0x0000ffff) == 0x0000090f)
						{
							g_netMgr->setErrMsg("SOFT RESET BY PLAYER1");
						}
						else
						{
							g_netMgr->setErrMsg("SOFT RESET BY PLAYER2");
						}

						// ÉQÅ[ÉÄíÜÇÃÉ\ÉtÉgÉäÉZÉbÉgÇÕñ≥éã
						**GGXX_ggnv_InputDataPtr = 0;
						for (int i = 0; i < g_netMgr->m_queueSize; i++)
						{
							g_netMgr->m_key[i] = 0;
						}
					}
					g_replay.m_frameCount++;
				}
				
			}

	#if DEBUG_OUTPUT_KEY
			char str[1024];
			//sprintf(str, "%.3d(rnd=%3d) : %08x\n", g_netMgr->m_time, *GGXX_RANDOMCOUNTER, **GGXX_ggnv_InputDataPtr);

			DWORD*	time  = (DWORD*)0x602760;
			WORD*	life1 = (WORD*)0x5ff5a0;
			WORD*	life2 = (WORD*)0x5ff684;
			sprintf(str, "frm=%4d cnt=%4d rndcnt=%3d time=%2d life%d-%d : %08x\n",
				g_replay.m_frameCount, *GGXX_FRAMECOUNTER, *GGXX_RANDOMCOUNTER,
				*GGXX_TIME, *GGXX_1PLIFE, *GGXX_2PLIFE,
				**GGXX_ggnv_InputDataPtr);
			
			ENTERCS(&g_csLogOut);
			if (strlen(g_keyLog) + strlen(str) < LOG_SIZE) strcat(g_keyLog, str);
			LEAVECS(&g_csLogOut);
	#endif //DEBUG_OUTPUT_KEY
		}
		else if (*GGXX_MODE1 & 0x800000 && *GGXX_MODE2 == 6) /* äœêÌ(ëŒêÌíÜ) */
		{
			GGXX_GetInputFromDI();

			if (**GGXX_ggnv_InputDataPtr & 0x00400040)
			{
				// çƒê∂íÜé~
				for (int i = 0; i < g_netMgr->m_queueSize; i++)
				{
					g_netMgr->m_key[i] = **GGXX_ggnv_InputDataPtr;
				}
				*GGXX_MODE1 = 0x200000;
				*GGXX_MODE2 = 0x37;
				g_netMgr->m_lobbyFrame = -1;
				g_replay.m_playing = false;

				// äœêÌä÷òAèâä˙âªÅAîzêMí‚é~
				g_netMgr->initWatchVars();
			}
			else if ((**GGXX_ggnv_InputDataPtr & 0xffff0000) == 0x090f0000 ||
					 (**GGXX_ggnv_InputDataPtr & 0x0000ffff) == 0x0000090f)
			{
				// soft reset
				for (int i = 0; i < g_netMgr->m_queueSize; i++)
				{
					g_netMgr->m_key[i] = **GGXX_ggnv_InputDataPtr;
				}
			}
			else
			{
				// 60ÉtÉåÅ[ÉÄíˆìxÇÃó]óTÇ™Ç†ÇÍÇŒëÅëóÇËâ¬î
				if (**GGXX_ggnv_InputDataPtr & 0x00200020 && g_replay.m_skipFrame == 0 &&
					g_netMgr->m_watchRecvSize - g_replay.m_frameCount * sizeof(DWORD) >= REPLAY_HEADER_SIZE + sizeof(DWORD)*60)
				{
					// ëÅëóÇË
					g_replay.m_skipFrame = 5;
				}
				
				// ÉfÅ[É^Ç™óàÇÈÇ‹Ç≈ë“ã@
				int waittime = 0;
				while (g_replay.m_data.inputData[g_replay.m_frameCount] == 0xffffffff)
				{
					if (g_netMgr->m_watch == false || waittime > TIMEOUT_WATCHDATAWAIT)
					{
						g_netMgr->disconnect("watch data timeout");
						break;
					}
					Sleep(50); waittime += 50;
					DBGOUT("wait to receive watch data...\n");
				}
				
				if (g_replay.m_data.inputData[g_replay.m_frameCount] == 0xffffffff)
				{
					**GGXX_ggnv_InputDataPtr = 0;
					for (int i = 0; i < g_netMgr->m_queueSize; i++) g_netMgr->m_key[i] = 0;
					g_netMgr->setErrMsg("END OF REPLAY ?");
				}
				else if (g_replay.m_data.inputData[g_replay.m_frameCount] == INPUT_SYNCERROR)
				{
					**GGXX_ggnv_InputDataPtr = 0;
					for (int i = 0; i < g_netMgr->m_queueSize; i++) g_netMgr->m_key[i] = 0;
					g_netMgr->setErrMsg("SYNC ERROR!!");
				}
				else if (g_replay.m_data.inputData[g_replay.m_frameCount] == INPUT_DISCONNECT)
				{
					**GGXX_ggnv_InputDataPtr = 0;
					for (int i = 0; i < g_netMgr->m_queueSize; i++) g_netMgr->m_key[i] = 0;
					g_netMgr->setErrMsg("DISCONNECT!!");
				}
				else if (g_replay.m_data.inputData[g_replay.m_frameCount] == INPUT_COMPLETE)
				{
					**GGXX_ggnv_InputDataPtr = 0;
					for (int i = 0; i < g_netMgr->m_queueSize; i++) g_netMgr->m_key[i] = 0;
					g_netMgr->setErrMsg("END OF REPLAY");
				}
				else
				{
					// ReplayÉfÅ[É^Ç©ÇÁÇ±ÇÃÉtÉåÅ[ÉÄÇ≈égópÇ∑ÇÈì¸óÕÇéÊÇËèoÇ∑
					**GGXX_ggnv_InputDataPtr = g_replay.m_data.inputData[g_replay.m_frameCount];
					if ((**GGXX_ggnv_InputDataPtr & 0xffff0000) == 0x090f0000 || (**GGXX_ggnv_InputDataPtr & 0x0000ffff) == 0x0000090f)
					{
						if ((**GGXX_ggnv_InputDataPtr & 0x0000ffff) == 0x0000090f)
						{
							g_netMgr->setErrMsg("SOFT RESET BY PLAYER1");
						}
						else
						{
							g_netMgr->setErrMsg("SOFT RESET BY PLAYER2");
						}

						// ÉQÅ[ÉÄíÜÇÃÉ\ÉtÉgÉäÉZÉbÉgÇÕñ≥éã
						**GGXX_ggnv_InputDataPtr = 0;
						for (int i = 0; i < g_netMgr->m_queueSize; i++)
						{
							g_netMgr->m_key[i] = 0;
						}
					}
					g_replay.m_frameCount++;
				}
			}

	#if DEBUG_OUTPUT_KEY
			char str[1024];
			//sprintf(str, "%.3d(rnd=%3d) : %08x\n", g_netMgr->m_time, *GGXX_RANDOMCOUNTER, **GGXX_ggnv_InputDataPtr);

			DWORD*	time  = (DWORD*)0x602760;
			WORD*	life1 = (WORD*)0x5ff5a0;
			WORD*	life2 = (WORD*)0x5ff684;
			sprintf(str, "frm=%4d cnt=%4d rndcnt=%3d time=%2d life%d-%d : %08x\n",
				g_replay.m_frameCount, *GGXX_FRAMECOUNTER, *GGXX_RANDOMCOUNTER,
				*GGXX_TIME, *GGXX_1PLIFE, *GGXX_2PLIFE,
				**GGXX_ggnv_InputDataPtr);
			
			ENTERCS(&g_csLogOut);
			if (strlen(g_keyLog) + strlen(str) < LOG_SIZE) strcat(g_keyLog, str);
			LEAVECS(&g_csLogOut);
	#endif //DEBUG_OUTPUT_KEY
		}
		else if (g_setting.enableNet == false)	/* Replay,äœêÌà»äO(Netñ≥å¯) */
		{
			GGXX_GetInputFromDI();
		}
		else									/* Replay,äœêÌà»äO(NetóLå¯) */
#endif
		{
			/* åªç›ÇÃÉtÉåÅ[ÉÄÇ≈ïKóvÇ»ì¸óÕÇ™Ç‹ÇæéÛêMÇ≈Ç´ÇƒÇ¢Ç»Ç¢ */
			int slow = 0;
			while (g_netMgr->m_connect &&
				((g_netMgr->m_key[g_netMgr->m_delay - 1] & 0x0000FFFF) == 0x0000FFFF ||
				 (g_netMgr->m_key[g_netMgr->m_delay - 1] & 0xFFFF0000) == 0xFFFF0000))
			{
				if (slow % 5 == 0)
				{
					if (g_netMgr->m_recvSuspend == false) g_netMgr->send_key(g_netMgr->m_time);
				}

				Sleep(3);
				
				/* àÍíËéûä‘åoâﬂÇµÇƒÇ‡ïúãAÇµÇ»ÇØÇÍÇŒêÿíf */
				if (g_netMgr->m_recvSuspend == true && slow >= TIMEOUT_KEY2)
				{
					DBGOUT_NET("m_suspend = %d,m_recvSuspend = %d\n", g_netMgr->m_suspend, g_netMgr->m_recvSuspend);
					g_netMgr->disconnect("key timeout2");
				}
				if (g_netMgr->m_recvSuspend == false && slow >= TIMEOUT_KEY)
				{
					DBGOUT_NET("m_suspend = %d,m_recvSuspend = %d\n", g_netMgr->m_suspend, g_netMgr->m_recvSuspend);
					g_netMgr->disconnect("key timeout");
				}
				slow += 3;
			}
			g_netMgr->m_totalSlow += slow;	/* ÉlÉbÉgÉèÅ[ÉNÇÃìsçáÇ≈í‚é~ÇµÇΩéûä‘ */

			/* ÉçÅ[ÉJÉãÇÃì¸óÕÇéÊìæÇµÅAÉLÉÖÅ[Ç÷äiî[Ç∑ÇÈ */
#if TESTER
			tester_input();
#else
			GGXX_GetInputFromDI();
#endif

			//static int xxx = 0;
			//if (xxx++ % 2 == 0) **GGXX_ggnv_InputDataPtr = 0;

			ENTERCS(&g_netMgr->m_csKey);

			for (int i = g_netMgr->m_queueSize - 1; i > 0; i--)
			{
				g_netMgr->m_key[i]		= g_netMgr->m_key[i - 1];
				g_netMgr->m_syncChk[i]	= g_netMgr->m_syncChk[i - 1];
			}
			g_netMgr->m_key[0]		= **GGXX_ggnv_InputDataPtr;
			g_netMgr->m_syncChk[0]	= 0;

#if DEBUG_INQUIRY_MODE
			// ÉfÉbÉhÉAÉìÉOÉãÉeÉXÉgóp
			// èÌéûÉeÉìÉVÉáÉìÉ}ÉbÉNÉX & ÉZÉåÉNÉgÉ{É^ÉìÇ≈ëäéËÇ…çUåÇÇµÇƒÇ‡ÇÁÇ§
			if (g_netMgr->m_connect)
			{
				*GGXX_1PTENSION = 0x2710;
				*GGXX_2PTENSION = 0x2710;
				static int tim = -1;
				if (g_netMgr->m_playSide == 1)
				{
					if (g_netMgr->m_key[g_netMgr->m_delay] & 0x01000000) tim = 0;
					if (tim > 10)
					{
						tim = -1;
						g_netMgr->m_key[0] |= 0x00000020;
					}
				}
				else if (g_netMgr->m_playSide == 2)
				{
					if (g_netMgr->m_key[g_netMgr->m_delay] & 0x00000100) tim = 0;
					if (tim > 10)
					{
						tim = -1;
						g_netMgr->m_key[0] |= 0x00200000;
					}
				}
				if (tim != -1) tim++;
				//DBGOUT("%d\n", tim);
			}
#endif

			if (g_netMgr->m_connect)
			{
				/* ÉlÉbÉgëŒêÌíÜÇÕ1P,2PÇ«ÇøÇÁë§ÇÃÉRÉìÉgÉçÅ[ÉãÇ≈Ç‡ëÄçÏâ¬î\Ç∆Ç∑ÇÈ */
				/* ñ¢éÛêMÇÃÉlÉbÉgÉèÅ[ÉNì¸óÕÇÕ0xFFFFÇ∆Ç»ÇÈ */
				if (g_netMgr->m_playSide == 1)
				{
					g_netMgr->m_key[0] |= g_netMgr->m_key[0] >> 16;
					g_netMgr->m_key[0] |= 0xFFFF0000;
					g_netMgr->m_syncChk[0] = getSyncCheckValue();
				}
				if (g_netMgr->m_playSide == 2)
				{
					g_netMgr->m_key[0] |= g_netMgr->m_key[0] << 16;
					g_netMgr->m_key[0] |= 0x0000FFFF;
					g_netMgr->m_syncChk[0] = getSyncCheckValue() << 8;
				}

				if (g_netMgr->m_recvSuspend == false) g_netMgr->send_key(g_netMgr->m_time + 1);

				/* ÉLÉÖÅ[Ç©ÇÁÇ±ÇÃÉtÉåÅ[ÉÄÇ≈égópÇ∑ÇÈì¸óÕÇéÊÇËèoÇ∑ */
				**GGXX_ggnv_InputDataPtr = g_netMgr->m_key[g_netMgr->m_delay];

				WORD sync = g_netMgr->m_syncChk[g_netMgr->m_delay];
				if (g_netMgr->m_suspend == false && g_netMgr->m_recvSuspend == false && (sync&0xff) != (sync>>8))
				{
					/* ìØä˙ÉYÉå */
					g_netMgr->setErrMsg("SYNC ERROR!!");
					DBGOUT_NET(g_syncErrLog[g_netMgr->m_delay]);
					g_netMgr->send_debugInfo();

					/* ÉLÅ[ÇreplayÇ…ãLò^ */
					if (g_replay.m_repRecording && g_replay.m_frameCount < MAXREPSIZE)
					{
						g_replay.m_data.inputData[g_replay.m_frameCount++] = INPUT_SYNCERROR;
					}
				}
				else
				{
					/* ÉLÅ[ÇreplayÇ…ãLò^ */
					if (g_replay.m_repRecording && g_replay.m_frameCount < MAXREPSIZE)
					{
						g_replay.m_data.inputData[g_replay.m_frameCount++] = **GGXX_ggnv_InputDataPtr;
					}
				}
				g_netMgr->m_time++;

#if DEBUG_OUTPUT_KEY
				char str[1024];
				sprintf(str, "%.3d(rnd=%3d) : %08x\n", g_netMgr->m_time, *GGXX_RANDOMCOUNTER, **GGXX_ggnv_InputDataPtr);

				ENTERCS(&g_csLogOut);
				if (strlen(g_keyLog) + strlen(str) < LOG_SIZE) strcat(g_keyLog, str);
				LEAVECS(&g_csLogOut);
#endif
#if !TESTER
				// ëŒêÌíÜÇ…äœêÌÇ™óLå¯Ç»ÇÁëäéËÇ…äœêÌé“êîÇëóÇÈ
				// ëŒêÌíÜÇ…àÍíËÇÃïpìxÇ≈äœêÌé“êîÇëóÇÈ
				if (*GGXX_MODE2 == 6 && g_netMgr->m_time % 60 == 0 &&
					(g_setting.watchBroadcast ? g_setting.watchMaxNodes : -16) + g_enemyInfo.m_watchMaxNode > 0)
				{
					// é©ï™ë§Ç≈äœêÌÇîFÇﬂÇƒÇ¢Ç»ÇØÇÍÇŒÇOÇÃÇ‹Ç‹Ç»ÇÃÇ≈ëóÇÁÇ»Ç¢
					if (g_setting.watchMaxNodes > 0) g_netMgr->send_galleryCount();
				}
				// äœêÌé“êîÇçXêV
				g_netMgr->m_totalGalleryCount = g_netMgr->m_recvGalleryCount + g_netMgr->getChildWatcherCount();
#endif
			}
			else
			{
				/* ÉLÉÖÅ[Ç©ÇÁÇ±ÇÃÉtÉåÅ[ÉÄÇ≈égópÇ∑ÇÈì¸óÕÇéÊÇËèoÇ∑ */
				**GGXX_ggnv_InputDataPtr = g_netMgr->m_key[g_netMgr->m_delay];
			}

			LEAVECS(&g_netMgr->m_csKey);
		}
	}

#if DEBUG_INQUIRY_MODE
	// ïœêîèëÇ´ä∑Ç¶é¿å±
	if (*GGXX_BTLINFO)
	{
		if (GetAsyncKeyState(VK_F3) & 0x80000000)
		{
			GGXX_SetAction(*GGXX_BTLINFO, 0x00000018);
		}
	}
#endif

	// ÉäÉvÉåÉCÇÃÉXÉLÉbÉv
#if !TESTER
	*GGXX_ggnv_render_off = 0;
	if (*GGXX_MODE1 & 0xc00000 && *GGXX_MODE2 == 6)
	{
		if (g_replay.m_skipFrame > 1) *GGXX_ggnv_render_off = 1;
		if (g_replay.m_skipFrame > 0) g_replay.m_skipFrame--;
	}
#endif

#if !TESTER
	// Ç¢Ç¢åƒÇ—èoÇµÉ^ÉCÉ~ÉìÉOÇ™Ç»Ç¢ÇÃÇ≈ggn_inputÇ≈â‰ñùÇ∑ÇÈ
	if ((*GGXX_MODE1 & 0x200000) == 0)
	{
		/* ã§óLÉÅÉÇÉäÇóòópÇµcoloreditÇìÆìIÇ…îΩâfÇ∑ÇÈ */
		char sm_pal[2048];
		g_smPallette->get(sm_pal, 0, 2048);
		if (sm_pal[0] == 1)
		{
			/* çXêVÉtÉâÉOè¡ãé */
			sm_pal[0] = 0;
			g_smPallette->set(sm_pal, 0, 1);

			replaceUserPalette(sm_pal[1], sm_pal[2], &sm_pal[3]);

			DWORD pal1P = (DWORD)g_myPalette[*GGXX_1PCHARA-1][*GGXX_1PCOLOR];
			DWORD pal2P = (DWORD)g_myPalette[*GGXX_2PCHARA-1][*GGXX_2PCOLOR];
			if (pal1P)
			{
				_asm
				{
					push 0x00
					push pal1P
					mov eax, 0x4c7760
					call eax			/* 1PÉpÉåÉbÉgçXêV */
					add esp, 8
				}
			}

			if (pal2P)
			{
				_asm
				{
					push 0x10
					push pal2P
					mov eax, 0x4c7760
					call eax			/* 2PÉpÉåÉbÉgçXêV */
					add esp, 8
				}
			}

			if (pal1P || pal2P)
			{
				_asm
				{
					push 0x5f8640
					mov eax, 0x4c0200
					call eax			/* éÊìæçœÇ›ÉXÉvÉâÉCÉgÇÃîjä¸ */
					add esp, 4
				}
			}
		}
	}
#endif
	
#if _DEBUG
	static bool space = false;
	if (GetForegroundWindow() == *GGXX_HWND && (GetAsyncKeyState(VK_F2) & 0x80000000))
	{
		if (space == false) g_ignoreWatchIn = !g_ignoreWatchIn;
		space = true;
	}
	else space = false;
#endif
}

DWORD ggn_getPalette(DWORD p_info, DWORD p_pidx, DWORD p_side)
{
	int cid = *((WORD*)p_info);
	
	/* ÉfÉtÉHÉãÉgÉpÉåÉbÉg */
	DWORD palList = *((DWORD*)(0x5d2400 + cid * 4));
	void* defpal  = (palList) ? *((void**)(palList + p_pidx * 4)) : NULL;

	if (*GGXX_MODE1 & 0x200000)	// NetVs
	{
		if (g_netMgr->m_playSide == p_side+1)
		{
			if (g_myPalette[cid - 1][p_pidx])
			{
				/* ÉIÉäÉWÉiÉãÉJÉâÅ[Ç™Ç†ÇÍÇŒÇªÇÍÇégóp */
				if (defpal) memcpy(g_myPalette[cid - 1][p_pidx], defpal, 16);
				return (DWORD)g_myPalette[cid - 1][p_pidx];
			}
			else return (DWORD)defpal;	/* ÉIÉäÉWÉiÉãÉJÉâÅ[Ç™ñ≥Ç¢ÇÃÇ≈ÉfÉtÉHÉãÉgÉpÉåÉbÉgégóp */
		}
		else
		{
			/* ÉpÉåÉbÉgÇ†Ç¡ÇƒÇ‡ñ≥Ç≠ÇƒÇ‡ëäéËÇ©ÇÁéÛêMÇµÇΩÇ‡ÇÃÇégóp */
			if (defpal) memcpy(g_enemyInfo.m_palette, defpal, 16);
			return (DWORD)g_enemyInfo.m_palette;
		}
	}
	else if (*GGXX_MODE1 & 0xc00000)// Replay, Watch
	{
		if (p_side == 0)
		{
			if (defpal) memcpy(g_replay.m_data.palette1P, defpal, 16);
			return (DWORD)g_replay.m_data.palette1P;
		}
		else
		{
			if (defpal) memcpy(g_replay.m_data.palette2P, defpal, 16);
			return (DWORD)g_replay.m_data.palette2P;
		}
	}
	else /* è]óàÇÃèàóù */
	{
		if (g_myPalette[cid - 1][p_pidx])
		{
			/* ÉIÉäÉWÉiÉãÉJÉâÅ[Ç™Ç†ÇÍÇŒÇªÇÍÇégóp */
			if (defpal) memcpy(g_myPalette[cid - 1][p_pidx], defpal, 16);
			return (DWORD)g_myPalette[cid - 1][p_pidx];
		}
		else return (DWORD)defpal;	/* ÉIÉäÉWÉiÉãÉJÉâÅ[Ç™ñ≥Ç¢ÇÃÇ≈ÉfÉtÉHÉãÉgÉpÉåÉbÉgégóp */
	}
}

void ggn_startNetVS(void)
{
	if (g_netMgr->m_connect) return;	// äœêÌÇ©ÇÁíºê⁄åqÇ™ÇÈÇ∆Ç±Ç±Ç÷óàÇƒÇµÇ‹Ç§

	DBGOUT_NET("************************* start lobby *************************\n");
	g_netMgr->m_networkEnable = true;
	g_netMgr->setErrMsg("");

	g_vsnet.m_itemPerPage = 16;

	ENTERCS(&g_netMgr->m_csNode);
	g_nodeMgr->removeAllNode();
	LEAVECS(&g_netMgr->m_csNode);

	DBGOUT_NET("remove all nodes\n");

	if (useLobbyServer())
	{
		enterServer(0);
		readServer();
	}
	else
	{
		ENTERCS(&g_netMgr->m_csNode);
		readNodeList();
		LEAVECS(&g_netMgr->m_csNode);
	}

	ENTERCS(&g_netMgr->m_csNode);
	g_nodeMgr->sortNodeList(g_vsnet.m_sortType);
	LEAVECS(&g_netMgr->m_csNode);

	if (g_vsnet.m_selectItemIdx >= g_nodeMgr->getNodeCount()) g_vsnet.m_selectItemIdx = g_nodeMgr->getNodeCount() - 1;
	if (g_vsnet.m_dispItemHead >= g_nodeMgr->getNodeCount() - g_vsnet.m_itemPerPage) g_vsnet.m_dispItemHead = g_nodeMgr->getNodeCount() - g_vsnet.m_itemPerPage;
	if (g_vsnet.m_selectItemIdx < 0) g_vsnet.m_selectItemIdx = 0;
	if (g_vsnet.m_dispItemHead < 0) g_vsnet.m_dispItemHead = 0;

	g_netMgr->m_recvGalleryCount = 0;
	g_netMgr->m_totalGalleryCount = 0;
	//DBGOUT_LOG("xxx clear m_totalGalleryCount\n");
	g_netMgr->m_lobbyFrame = 0;
	g_replay.m_playing = false;

	g_vsnet.m_menu_visible = false;
	g_vsnet.m_menu_cursor = 0;

	DBGOUT_NET("read server ok\n");
}

bool ggn_procNetVS(void)
{
	g_netMgr->m_lobbyFrame++;

	int input = (*GGXX_1PJDOWN & 0xf300) | (*GGXX_2PJDOWN & 0xf300);
	static int pressKeyTime[6] = {0,0,0,0,0,0};
	for (int i = 0; i < 6; i++)
	{
		int key;
		switch (i)
		{
		case 0: key = 0x0010; break;
		case 1: key = 0x0020; break;
		case 2: key = 0x0040; break;
		case 3: key = 0x0080; break;
		case 4: key = 0x0400; break;
		case 5: key = 0x0800; break;
		}
		if ((*GGXX_1PDOWN | *GGXX_2PDOWN) & key)
		{
			if ((*GGXX_1PJDOWN | *GGXX_2PJDOWN) & key)
			{
				input |= key;
				pressKeyTime[i] = 0;
			}
			else if (pressKeyTime[i] >= 0x11)
			{
				input |= key;
				pressKeyTime[i] = 0x0e;
			}
			else pressKeyTime[i]++;
		}
		else pressKeyTime[i] = 0;
	}

/* ï`âÊ */
	GGXX_DrawText2("VS NET", 40, 53, 5);
	
	ENTERCS(&g_netMgr->m_csNode);

	static int c1 = 0, c2 = 0x08;

	if (g_vsnet.m_dispItemHead > 0)
	{
		GGXX_DrawArrowMark(4, -5, 150, c1 + 1);
	}
	if (g_nodeMgr->getNodeCount() > g_vsnet.m_dispItemHead + g_vsnet.m_itemPerPage)
	{
		GGXX_DrawArrowMark(8, -5, 360, c1 + 1);
	}
	if (c1 == 0xe0) c2 = -0x08;
	if (c1 == 0x00) c2 =  0x08;
	c1 += c2;

	LEAVECS(&g_netMgr->m_csNode);

	if (g_netMgr->m_watch == false)
	{
	/* ì¸óÕ */
		if (input & 0x0010)			/* Å™ÉLÅ[ */
		{
			if (g_vsnet.m_menu_visible)
			{
				g_vsnet.m_menu_cursor--;
				if (g_vsnet.m_menu_cursor < 0) g_vsnet.m_menu_cursor = g_vsnet.m_menu_cursor = 3;
				GGXX_PlayCmnSound(0x37);
			}
			else
			{
				if (g_vsnet.m_selectItemIdx > 0)
				{
					g_vsnet.m_selectItemIdx--;
					if (g_vsnet.m_selectItemIdx < g_vsnet.m_dispItemHead) g_vsnet.m_dispItemHead--;
					GGXX_PlayCmnSound(0x37);
				}
			}
		}
		else if (input & 0x0040)	/* Å´ÉLÅ[ */
		{
			if (g_vsnet.m_menu_visible)
			{
				g_vsnet.m_menu_cursor++;
				if (g_vsnet.m_menu_cursor > 3) g_vsnet.m_menu_cursor = g_vsnet.m_menu_cursor = 0;
				GGXX_PlayCmnSound(0x37);
			}
			else
			{
				if (g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount() - 1)
				{
					g_vsnet.m_selectItemIdx++;
					if (g_vsnet.m_selectItemIdx >= g_vsnet.m_dispItemHead + g_vsnet.m_itemPerPage) g_vsnet.m_dispItemHead++;
					GGXX_PlayCmnSound(0x37);
				}
			}
		}
		else if (input & 0x0080)	/* Å©ÉLÅ[ */
		{
			if (!g_vsnet.m_menu_visible)
			{
				if (g_vsnet.m_sortType > 0) g_vsnet.m_sortType--;
				else g_vsnet.m_sortType = SORTTYPECOUNT - 1;

				GGXX_PlayCmnSound(0x37);
			}
		}
		else if (input & 0x0020)	/* Å®ÉLÅ[ */
		{
			if (!g_vsnet.m_menu_visible)
			{
				if (g_vsnet.m_sortType < SORTTYPECOUNT - 1) g_vsnet.m_sortType++;
				else g_vsnet.m_sortType = 0;

				GGXX_PlayCmnSound(0x37);
			}
		}
		else if (input & 0x0400)	/* L1ÉLÅ[ */
		{
			if (g_vsnet.m_selectItemIdx > 0)
			{
				for (int i = 0; i < g_vsnet.m_itemPerPage; i++)
				{
					if (g_vsnet.m_selectItemIdx > 0)
					{
						g_vsnet.m_selectItemIdx--;
						if (g_vsnet.m_selectItemIdx < g_vsnet.m_dispItemHead) g_vsnet.m_dispItemHead--;
					}
				}
				GGXX_PlayCmnSound(0x37);
			}
		}
		else if (input & 0x0800)	/* R1ÉLÅ[ */
		{
			if (g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount() - 1)
			{
				for (int i = 0; i < g_vsnet.m_itemPerPage; i++)
				{
					if (g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount() - 1)
					{
						g_vsnet.m_selectItemIdx++;
						if (g_vsnet.m_selectItemIdx >= g_vsnet.m_dispItemHead + g_vsnet.m_itemPerPage) g_vsnet.m_dispItemHead++;
					}
				}
				GGXX_PlayCmnSound(0x37);
			}
		}
		else if (input & 0x2000)	/* åàíË */
		{
			if (g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount() && g_netMgr->m_lobbyFrame >= 30)
			{
				if (g_vsnet.m_menu_visible)
				{
					CNode* node = g_nodeMgr->getNode(g_vsnet.m_selectItemIdx);

					if (g_vsnet.m_menu_cursor == 0)
					{
						// Play Game
						if ((node->m_state == State_Idle || node->m_state == State_Watch_Playable) &&
							node->m_deny == false && g_denyListMgr->find(node->m_id) == -1)
						{
							/* ÉAÉNÉeÉBÉuÉAÉhÉåÉXÇ…ê›íË */
							g_netMgr->m_remoteAddr_active = g_netMgr->getAddrFromString(node->m_addr);
							g_netMgr->send_connect(&g_netMgr->m_remoteAddr_active);
						}
						// Watch Game
						else if (node->m_state == State_Busy_Casting)
						{
							g_netMgr->m_watch = false;
							g_netMgr->m_watchRootAddr[0] = NULL_ADDR;
							g_netMgr->m_watchRootAddr[1] = NULL_ADDR;
							g_netMgr->m_watchRecvComplete = false;
							g_netMgr->m_watchRecvSize = 0;
							for (int i = 0; i < MAXREPSIZE; i++) g_replay.m_data.inputData[i] = 0xffffffff;

							sockaddr_in addr = g_netMgr->getAddrFromString(node->m_addr);
							if (g_netMgr->watch(node->m_name, &addr, node->m_gamecount, true) == false)
							{
								node->clearPing();
								node->m_state = State_NoResponse;
							}
						}
					}
					else if (g_vsnet.m_menu_cursor == 1)
					{
						/* Deny */
						int idx = g_denyListMgr->find(node->m_id);
						if (idx == -1)
						{
							/* ñ≥éãÉäÉXÉgÇ…í«â¡ */
							g_denyListMgr->add(node->m_name, node->m_id);
							g_denyListMgr->savefile();
							GGXX_PlayCmnSound(0x39);
						}
						else if (idx >= 0)
						{
							/* ñ≥éãÉäÉXÉgÇ©ÇÁèúäO */
							g_denyListMgr->remove(node->m_id);
							g_denyListMgr->savefile();
							GGXX_PlayCmnSound(0x39);
						}
					}
					else if (g_vsnet.m_menu_cursor == 2)
					{
						/* Send Message */
					}
					else if (g_vsnet.m_menu_cursor == 3)
					{
						/* Config */
					}
				}
				else
				{
					g_vsnet.m_menu_visible = true;
					g_vsnet.m_menu_cursor = 0;
					GGXX_PlayCmnSound(0x39);
				}
			}
		}
		else if (input & 0x4000)	/* ÉLÉÉÉìÉZÉã */
		{
			if (g_vsnet.m_menu_visible)
			{
				GGXX_PlayCmnSound(0x3B);

				g_vsnet.m_menu_visible = false;
				g_vsnet.m_menu_cursor = 0;
			}
			else
			{
				g_netMgr->m_networkEnable = false;
				if (useLobbyServer()) leaveServer();

				g_netMgr->m_lobbyFrame = -1;

				GGXX_PlayCmnSound(0x3B);
				*GGXX_MODE2 = 0x19;
				return 1;
			}
		}
		else if (input & 0x1000)	/* É\Å[Ég */
		{
			if (!g_vsnet.m_menu_visible)
			{
				if (g_nodeMgr->getNodeCount() == 0)
				{
					/* ÉmÅ[ÉhÉäÉXÉgÇ™ãÛÇæÇ¡ÇΩÇÁÉäÉçÅ[ÉhÇ‡åìÇÀÇÈ */
					if (useLobbyServer()) readServer();
				}

				GGXX_PlayCmnSound(0x3B);
				
				ENTERCS(&g_netMgr->m_csNode);
				g_nodeMgr->sortNodeList(g_vsnet.m_sortType);
				LEAVECS(&g_netMgr->m_csNode);
			}
		}
		else if (input & 0x8000)
		{
		}

		// ëŒêÌèÓïÒÇÃóvãÅ
		// êÊÇ…Idle, WatchÉmÅ[ÉhÇ…ëŒÇµÇƒóvãÅÇµÅAìæÇÁÇÍÇ»ÇØÇÍÇŒíºê⁄óvãÅÇ∑ÇÈ
		
		if (g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount() && g_netMgr->m_lobbyFrame >= 30 &&
			g_vsnet.m_menu_visible && g_vsnet.m_menu_cursor == 0)
		{
			CNode* node = g_nodeMgr->getNode(g_vsnet.m_selectItemIdx);
			if (node->m_state == State_Busy_Casting && node->isExistBattleInfo() == false)
			{
				sockaddr_in addr = g_netMgr->getAddrFromString(node->m_addr);
				g_netMgr->send_battleInfoRequest(node->m_name, &addr, node->m_gamecount);
			}
		}

	/* ÉlÉbÉgÉèÅ[ÉN */
		static bool oldConnect = false;
		if (g_netMgr->m_connect && !oldConnect)
		{
			/* ééçáäJén */
			if (useLobbyServer()) enterServer(1);
			
			g_netMgr->setErrMsg("");

			GGXX_PlayCmnSound(0x39);
			*GGXX_MODE1		= 0x200803;
			*GGXX_MODE2		= 0x0f;
			GGXX_InitBattleChara(0);
			GGXX_InitBattleChara(1);

			// ÉLÉÉÉâÉZÉåÇÃéûì_Ç≈ÇÕÉtÉåÅ[ÉÄÉJÉEÉìÉgÇ™èâä˙âªÇ≥ÇÍÇƒÇ¢Ç»Ç¢Ç©Ç‡ÅH
			g_replay.m_frameCount = 0;

			char round = 5;
			/* ñºëOÅEÉâÉìÉNìôÇëóéÛêM */
			SBlock_PlayerInfo	pinf;
			if (g_netMgr->m_playSide == 1)
			{
				getNameTrip(pinf.nametrip);
				pinf.rank  = g_setting.rank;
				pinf.wins  = g_setting.wins;
				pinf.ex = g_setting.useEx;
				pinf.oldcs = g_oldCS;
				pinf.round = g_setting.rounds;	/* é©ï™Ç©ÇÁì¸ÇÈèÍçáÇÕégópÇµÇ»Ç¢Ç™Åc */

				if (g_netMgr->sendDataBlock(Block_PlayerInfo, (char*)&pinf, sizeof(SBlock_PlayerInfo), TIMEOUT_BLOCK) == false)
				{
					g_netMgr->disconnect("send block  playerinfo");
					return 0;
				}
				DBGOUT_NET("send block  playerinfo ok!\n");

				if (g_netMgr->recvDataBlock(Block_PlayerInfo, (char*)&pinf, sizeof(SBlock_PlayerInfo), TIMEOUT_BLOCK) == false)
				{
					g_netMgr->disconnect("recv blockplayerinfo");
					return 0;
				}
				DBGOUT_NET("recv blockplayerinfo ok!\n");
				__strncpy(g_enemyInfo.m_name, pinf.nametrip, 29);
				g_enemyInfo.m_rank = pinf.rank;
				g_enemyInfo.m_wins = pinf.wins;
				g_enemyInfo.m_ex  = pinf.ex;
				*GGXX_CSSELECT_1P = g_oldCS;
				*GGXX_CSSELECT_2P = pinf.oldcs;
				round			  = pinf.round;

				// å›ä∑ï€Ç¬ÇΩÇﬂédï˚Ç»Ç≠ÉmÅ[ÉhåüçıÇµÅAExÇ∆BroadcastÇÃÉIÉvÉVÉáÉìÇéÊìæÇ∑ÇÈ
				// ÉvÉåÉCÉÑÅ[èÓïÒÇÃëóêMÇ…ÉuÉçÉbÉNì]ëóÇégÇ§ïKóvÇ‡ñ≥Ç≠ÅA
				// å›ä∑ÇÃà€éùÇçlÇ¶ÇÈÇ∆ç°çXï˚ñ@ÇêÿÇËë÷Ç¶ÇÈÇ±Ç∆Ç‡ìÔÇµÇ¢ÅBäÆëSÇ…ê›åvÇåÎÇ¡ÇΩorz
				char addrstr[32];
				int nodeidx = g_nodeMgr->findNodeIdx_address(g_netMgr->getStringFromAddr(&g_netMgr->m_remoteAddr_active, addrstr));
				if (nodeidx == -1)
				{
					g_netMgr->disconnect("node not found.");
					return 0;
				}
				if (strcmp(g_nodeMgr->getNode(nodeidx)->m_ver, "1.13") < 0)
				{
					g_enemyInfo.m_ex = g_nodeMgr->getNode(nodeidx)->m_ex;
				}
				g_enemyInfo.m_watchMaxNode = g_nodeMgr->getNode(nodeidx)->m_watchMaxNode;
				g_enemyInfo.m_gameCount = g_nodeMgr->getNode(nodeidx)->m_gamecount + 1;
			}
			else
			{
				if (g_netMgr->recvDataBlock(Block_PlayerInfo, (char*)&pinf, sizeof(SBlock_PlayerInfo), TIMEOUT_BLOCK) == false)
				{
					g_netMgr->disconnect("recv block playerinfo");
					return 0;
				}
				DBGOUT_NET("recv blockplayerinfo ok!\n");
				__strncpy(g_enemyInfo.m_name, pinf.nametrip, 29);
				g_enemyInfo.m_rank = pinf.rank;
				g_enemyInfo.m_wins = pinf.wins;
				g_enemyInfo.m_ex  = pinf.ex;
				*GGXX_CSSELECT_1P = pinf.oldcs;
				*GGXX_CSSELECT_2P = g_oldCS;
				round		= g_setting.rounds;

				// å›ä∑ï€Ç¬ÇΩÇﬂédï˚Ç»Ç≠ÉmÅ[ÉhåüçıÇµÅAExÇ∆BroadcastÇÃÉIÉvÉVÉáÉìÇéÊìæÇ∑ÇÈ
				// ÉvÉåÉCÉÑÅ[èÓïÒÇÃëóêMÇ…ÉuÉçÉbÉNì]ëóÇégÇ§ïKóvÇ‡ñ≥Ç≠ÅA
				// å›ä∑ÇÃà€éùÇçlÇ¶ÇÈÇ∆ç°çXï˚ñ@ÇêÿÇËë÷Ç¶ÇÈÇ±Ç∆Ç‡ìÔÇµÇ¢ÅBäÆëSÇ…ê›åvÇåÎÇ¡ÇΩorz
				char addrstr[32];
				int nodeidx = g_nodeMgr->findNodeIdx_address(g_netMgr->getStringFromAddr(&g_netMgr->m_remoteAddr_active, addrstr));
				if (nodeidx == -1)
				{
					g_netMgr->disconnect("node not found.");
					return 0;
				}
				if (strcmp(g_nodeMgr->getNode(nodeidx)->m_ver, "1.13") < 0)
				{
					g_enemyInfo.m_ex = g_nodeMgr->getNode(nodeidx)->m_ex;
				}
				g_enemyInfo.m_watchMaxNode = g_nodeMgr->getNode(nodeidx)->m_watchMaxNode;
				g_enemyInfo.m_gameCount = g_nodeMgr->getNode(nodeidx)->m_gamecount + 1;

				getNameTrip(pinf.nametrip);
				pinf.rank  = g_setting.rank;
				pinf.wins  = g_setting.wins;
				pinf.ex = g_setting.useEx;
				pinf.oldcs = g_oldCS;
				pinf.round = g_setting.rounds;	/* é©ï™ÇÃÉãÅ[ÉãÇ…è]ÇÌÇπÇÈ */
				
				if (g_netMgr->sendDataBlock(Block_PlayerInfo, (char*)&pinf, sizeof(SBlock_PlayerInfo), TIMEOUT_BLOCK) == false)
				{
					g_netMgr->disconnect("send block playerinfo");
					return 0;
				}
				DBGOUT_NET("send block playerinfo ok!\n");
			}
			
			g_setting.totalBattle++;

			// ExÇÃâ¬ïsâ¬Çê›íË
			*GGXX_ggnv_cfg_enableExChara = g_setting.useEx & g_enemyInfo.m_ex;

			// ÉNÉäÉtÅEÉWÉÉÉXÉeÉBÉXÇ™ÉfÉtÉHÉãÉgÇæÇ¡ÇΩÇÁÉ\ÉãÇ…ïœçX
			if (*GGXX_ggnv_cfg_enableExChara == 0)
			{
				if (*GGXX_CSSELECT_1P == 0x16 || *GGXX_CSSELECT_1P == 0x17) *GGXX_CSSELECT_1P = 1;
				if (*GGXX_CSSELECT_2P == 0x16 || *GGXX_CSSELECT_2P == 0x17) *GGXX_CSSELECT_2P = 1;
			}

			if (round == 3)	*GGXX_ROUND = 2;
			else			*GGXX_ROUND = 3;

#if 1 // óêì¸ÉGÉtÉFÉNÉg
			// 0x440590ÇÅumov dword ptr ds:[5CDFB0], 0FÅvÇ∆èCê≥
			// 0x4405a5ÇNOPÇ…èCê≥
			*GGXX_MODE2		= 0x0a;
			_asm
			{
				push 0x00596f68		// "ITBT"
				push 0x1111
				push 3
				push 0x2000
				push 0x004403E0		// Show DareDevil
				mov eax, 0x004cbc70	// CreateFiberB
				call eax
				add esp, 0x14
			}
#endif
			oldConnect = g_netMgr->m_connect;
			return 1;
		}
		oldConnect = g_netMgr->m_connect;
	}
	else // äœêÌÉäÉvÇ™äJénâ¬î\Ç©ÅH
	{
		if (input & 0x4000)	/* ÉLÉÉÉìÉZÉã */
		{
			// éÛêMÇíÜé~
			GGXX_PlayCmnSound(0x3B);
			g_netMgr->initWatchVars();
		}

		/* ééçáäJén */
		if (g_netMgr->m_watchRecvSize >= REPLAY_HEADER_SIZE + sizeof(DWORD)*60)	// 60ÉtÉåÅ[ÉÄíˆìxÇÃó]óT
		{
			/* ééçáäJén */
			if (useLobbyServer()) enterServer(1);

			g_replay.m_skipFrame = 0;
			g_replay.m_frameCount = 0;

			g_netMgr->m_suspend = false;
			
			g_netMgr->setErrMsg("");

			GGXX_PlayCmnSound(0x39);
			*GGXX_MODE1		= 0x800803;
			*GGXX_MODE2		= 0x11;
			GGXX_InitBattle();
			GGXX_InitBattleChara(0);
			GGXX_InitBattleChara(1);
			
			int chara1P = g_replay.m_data.chara1P;
			int chara2P = g_replay.m_data.chara2P;
			int stageID = g_replay.m_data.battleStage;
			
			if (g_replay.m_data.round == 3)	*GGXX_ROUND = 2;
			else							*GGXX_ROUND = 3;

			_asm
			{
				push 0
				push 0	// color
				push dword ptr ds:[chara1P]	// chara
				push 0
				mov eax, 0x45edd0
				call eax
				
				push 0
				push 0	// color
				push dword ptr ds:[chara2P]	// chara
				push 1
				mov eax, 0x45edd0
				call eax

				push 0	// ai
				push 0
				mov eax, 0x45eea0
				call eax

				push 0	// ai
				push 1
				mov eax, 0x45eea0
				call eax

				push stageID
				mov eax, 0x410B80 // stageinit
				call eax
				add esp, 0x34
			}
			g_replay.m_playing = true;
			return 1;
		}
	}
	return 0;
}

void ggn_startCS(void)
{
	if (*GGXX_MODE1 & 0x200000)
	{
		DBGOUT_NET("ggn_startCS...       (VS %s)\n", g_enemyInfo.m_name);

		/* ÉXÉeÅ[ÉWÉZÉåÉNÉ^Å[ÇÉNÉäÉA */
		*GGXX_STAGESELECTER = 0;

		for (int i = 0; i < 10; i++) *(GGXX_RANDOMCSLOG+i) = 0xFFFFFFFF;
		*GGXX_RANDOMCSLOGPOS = 0;

		ggn_syncRandomTable(0);
		g_netMgr->resume();
	}
}

void ggn_startVS(void)
{
	if (*GGXX_MODE1 & 0xe00000)
	{
		*GGXX_1PCHEAT = 0;
		*GGXX_2PCHEAT = 0;
		*GGXX_1PDOWNFLG = 0;
		*GGXX_2PDOWNFLG = 0;

		if (*GGXX_MODE1 & 0x200000)
		{
			DBGOUT_NET("ggn_startVS...\n");
		}
		else if (*GGXX_MODE1 & 0xc00000)
		{
			*GGXX_1PVOICE = g_replay.m_data.voice1P;
			*GGXX_2PVOICE = g_replay.m_data.voice2P;
			*GGXX_1PEX = g_replay.m_data.ex1P;
			*GGXX_2PEX = g_replay.m_data.ex2P;
		}
		g_netMgr->m_initKeySet = false;
	}
}

DWORD ggn_vsLoadCompleted(void)
{
	/* VSÉfÉÇíÜÇ…ÉçÅ[ÉhÇ™äÆóπÇ∑ÇÈÇ∆Ç±Ç±Ç…óàÇÈ */
	if (*GGXX_MODE1 & 0x200000)
	{
		/* é©ï™Ç™ÉçÅ[ÉhäÆóπÇ…Ç»Ç¡ÇΩÇ±Ç∆ÇëäéËÇ…ì`Ç¶ÇÈ */
		if (g_netMgr->m_vsloadFrame % 10 == 0) DBGOUT_NET("vsload waiting = %d\n", g_netMgr->m_vsloadFrame);
		if (g_netMgr->m_vsloadFrame % 5  == 0) g_netMgr->send_vsLoadCompleted(); /* 5ÉtÉåÅ[ÉÄÇ…1âÒëóêM */
		g_netMgr->m_vsloadFrame++;

		/* VSNETÇÃèÍçáëoï˚Ç™ÉçÅ[ÉhäÆóπÇµÇΩéûì_Ç≈ÉfÉÇÇèIóπÇ∑ÇÈÅBShortCutÇÃon/offÇ…ä÷ÇÌÇÁÇ∏ÅAÉLÅ[ì¸óÕÇÕë“ÇΩÇ»Ç¢ */
		if (g_netMgr->m_recvVSLoadCompleted) return 2;

		return 1; /* ëäéËë“Çø */
	}
	return 0; /* í èÌèàóù */
}

void ggn_startBattle(void)
{
	/* ÉçÉOÉNÉäÉA */
#if DEBUG_OUTPUT_KEY
	memset(g_keyLog, 0, LOG_SIZE);
#endif
#if DEBUG_OUTPUT_RND
	memset(g_rndLog, 0, LOG_SIZE);
#endif

	if (*GGXX_MODE1 & 0x200000) // NetVs
	{
		DBGOUT_NET("ggn_startBattle...\n");
		
		/* ëIëÉLÉÉÉâÇï€ë∂ */
		g_oldCS = (g_netMgr->m_playSide == 1) ? *GGXX_CSSELECT_1P : *GGXX_CSSELECT_2P;

		char* palPtr1P = NULL;
		char* palPtr2P = NULL;
		if (g_netMgr->m_playSide == 1)
		{
			if (g_myPalette[*GGXX_1PCHARA-1][*GGXX_1PCOLOR])
			{
				/* é©çÏpaletteÇ™Ç†ÇÍÇŒÇªÇÍÇëóêM */
				palPtr1P = (char*)g_myPalette[*GGXX_1PCHARA-1][*GGXX_1PCOLOR];
			}
			else
			{
				/* ÉfÉtÉHÉãÉgÉpÉåÉbÉgÇëóêM */
				DWORD palList = *((DWORD*)(0x5d2400 + *GGXX_1PCHARA * 4));
				palPtr1P = (char*)*((DWORD*)(palList + *GGXX_1PCOLOR * 4));
			}
			palPtr2P = (char*)g_enemyInfo.m_palette;

			/* ëŒêÌëäéËÇ…ÉpÉåÉbÉgÇëóêM */
			if (g_netMgr->sendDataBlock(Block_Palette, palPtr1P, PALLEN*4, TIMEOUT_BLOCK) == false)
			{
				g_netMgr->disconnect("send block  palette");
				return;
			}
			DBGOUT_NET("send block  palette ok!\n");

			/* ëŒêÌëäéËÇÃÉpÉåÉbÉgÇéÛêM */
			if (g_netMgr->recvDataBlock(Block_Palette, palPtr2P, PALLEN*4, TIMEOUT_BLOCK) == false)
			{
				g_netMgr->disconnect("recv blockpalette");
				return;
			}
			DBGOUT_NET("recv blockpalette ok!\n");
		}
		else
		{
			if (g_myPalette[*GGXX_2PCHARA-1][*GGXX_2PCOLOR])
			{
				/* é©çÏpaletteÇ™Ç†ÇÍÇŒÇªÇÍÇëóêM */
				palPtr2P = (char*)g_myPalette[*GGXX_2PCHARA-1][*GGXX_2PCOLOR];
			}
			else
			{
				/* ÉfÉtÉHÉãÉgÉpÉåÉbÉgÇëóêM */
				DWORD palList = *((DWORD*)(0x5d2400 + *GGXX_2PCHARA * 4));
				palPtr2P = (char*)*((DWORD*)(palList + *GGXX_2PCOLOR * 4));
			}
			palPtr1P = (char*)g_enemyInfo.m_palette;

			/* ëŒêÌëäéËÇÃÉpÉåÉbÉgÇéÛêM */
			if (g_netMgr->recvDataBlock(Block_Palette, palPtr1P, PALLEN*4, TIMEOUT_BLOCK) == false)
			{
				g_netMgr->disconnect("recv blockpalette");
				return;
			}
			DBGOUT_NET("recv blockpalette ok!\n");

			/* ëŒêÌëäéËÇ…ÉpÉåÉbÉgÇëóêM */
			if (g_netMgr->sendDataBlock(Block_Palette, palPtr2P, PALLEN*4, TIMEOUT_BLOCK) == false)
			{
				g_netMgr->disconnect("send block  palette");
				return;
			}
			DBGOUT_NET("send block  palette ok!\n");
		}
		
		/* ÉpÉåÉbÉgÇreplayÇ…ï€ë∂ */
		memcpy(g_replay.m_data.palette1P, palPtr1P, PALLEN*4);
		memcpy(g_replay.m_data.palette2P, palPtr2P, PALLEN*4);

		g_netMgr->resume();
	}
	else if (*GGXX_MODE1 & 0x400000) // Replay
	{
	}
	else if (*GGXX_MODE1 & 0x800000) // Watch
	{
	}

	*GGXX_FRAMECOUNTER = 0;
	g_startBattleTime = timeGetTime() / 1000;
}

void ggn_syncRandomTable(int p_timing)
{
	/* timing 2,3ÇÕÇ¢ÇÁÇ»Ç¢ÅBåƒÇ—èoÇµå≥ÇÕçÌèúÇµÇΩÇ©ÅH */
	if (p_timing == 2) return;
	if (p_timing == 3) return;

	if (*GGXX_MODE1 & 0x200000)
	{
		if (g_netMgr->m_playSide == 1)
		{
			GGXX_GetRandom(); /* èâä˙âªÇ≥ÇÍÇƒÇ¢Ç»Ç¢èÍçáÇÕÇ±Ç±Ç≈èâä˙âªÇ≥ÇÍÇÈ */
			if (g_netMgr->sendDataBlock(Block_RandomTable, (char*)GGXX_RANDOMTABLE, GGXX_RANDOMTABLESIZE, TIMEOUT_BLOCK) == false)
			{
				g_netMgr->disconnect("send block  randomtable");
				return;
			}
			DBGOUT_NET("send block  randomtable%d ok!\n", p_timing);
		}
		else
		{
			*GGXX_ISRANDOMINIT = 1;
			if (g_netMgr->recvDataBlock(Block_RandomTable, (char*)GGXX_RANDOMTABLE, GGXX_RANDOMTABLESIZE, TIMEOUT_BLOCK) == false)
			{
				g_netMgr->disconnect("recv blockrandomtable");
				return;
			}
			DBGOUT_NET("recv blockrandomtable%d ok!\n", p_timing);
		}
		// offline test
		//for (int i = 0; i < GGXX_RANDOMTABLESIZE/4; i++)
		//{
		//	GGXX_RANDOMTABLE[i] = i;
		//}

		/* óêêîÉeÅ[ÉuÉãÇreplayÇ…ï€ë∂ */
		memcpy(g_replay.m_data.randTable, GGXX_RANDOMTABLE, GGXX_RANDOMTABLESIZE);

		GGXX_RebuildRandomTable();
	}
	else if (*GGXX_MODE1 & 0xc00000)
	{
		/* replayÇ©ÇÁéÊìæ */
		GGXX_GetRandom(); /* èâä˙âªÇ≥ÇÍÇƒÇ¢Ç»Ç¢èÍçáÇÕÇ±Ç±Ç≈èâä˙âªÇ≥ÇÍÇÈ */
		if (p_timing == 1)
		{
			memcpy(GGXX_RANDOMTABLE, g_replay.m_data.randTable, GGXX_RANDOMTABLESIZE);
		}
		GGXX_RebuildRandomTable();
	}
}

void ggn_softReset(void)
{
	// íºê⁄éÛêMé“Ç™Ç¢ÇÈèÛë‘Ç≈ë¶êÿífÇ∑ÇÈÇ∆ÉäÉvÉåÉCÇ™ç≈å„Ç‹Ç≈ëóêMÇ≥ÇÍÇ»Ç¢ÇÃÇ≈óPó\éûä‘Ç™ïKóv
	// ñ‚ëËÇPÅDäœêÌé“ÇÃÉfÅ[É^éÛêMÇ™í«Ç¢Ç¬Ç¢ÇƒÇ¢Ç»Ç¢Ç∆ä‘Ç…çáÇÌÇ»Ç¢
	// ñ‚ëËÇQÅDexeé©ëÃÇóéÇ∆Ç≥ÇÍÇÈÇ∆ç≈å„Ç‹Ç≈ëóÇÁÇÍÇ»Ç¢ÅiÇ±ÇÍÇÕédï˚Ç»Ç¢Ç™ÅcÅj
	Sleep(300);
	if (*GGXX_MODE1 & 0x200000)			// NetPlay
	{
		g_netMgr->m_networkEnable = false;
		g_netMgr->disconnect("softreset");
		
		g_netMgr->m_lobbyFrame = -1;

		if (useLobbyServer()) leaveServer();
	}
	else if (*GGXX_MODE1 & 0x400000)	// Replay
	{
		g_replay.m_playing = false;
		g_netMgr->m_lobbyFrame = -1;
	}
	else if (*GGXX_MODE1 & 0x800000)	// Watch
	{
		g_netMgr->m_networkEnable = false;
		g_netMgr->disconnect("softreset");

		g_replay.m_playing = false;
		g_netMgr->m_lobbyFrame = -1;

		if (useLobbyServer()) leaveServer();
	}
}

void ggn_drawBattlePlayerName(DWORD p_side)
{
	//if (*GGXX_MODE1 & 0x200000)
	//{
	//	char name[256];
	//	if (g_netMgr->m_playSide == p_side+1) getNameTrip(name);
	//	else strcpy(name, g_enemyInfo.m_name);
	//	
	//	int posx = (p_side == 0) ? 40 : (640 - 40 - strlen(name) * 12);
	//	GGXX_DrawText1Ex(name, posx, 64, 200.0f, 0xFFFFFFFF);
	//}
	//else if (*GGXX_MODE1 & 0x400000)
	//{
	//	if (p_side == 0)
	//	{
	//		GGXX_DrawText1Ex(g_replay.m_data.name1P, 40, 64, 200.0f, 0xFFFFFFFF);
	//	}
	//	else
	//	{
	//		GGXX_DrawText1Ex(g_replay.m_data.name2P, 640 - 40 - strlen(g_replay.m_data.name2P) * 12, 64, 200.0f, 0xFFFFFFFF);
	//	}
	//}
}

void ggn_drawCSPlayerName(void)
{
	//if (*GGXX_MODE1 & 0x200000)
	//{
	//	char name[256];
	//	
	//	if (g_netMgr->m_playSide == 1)
	//	{
	//		getNameTrip(name);
	//		GGXX_DrawText1Ex(name, 40, 32, 10.0f, 0xFFFFFFFF);
	//		GGXX_DrawText1Ex(g_enemyInfo.m_name, 640 -  40 - strlen(g_enemyInfo.m_name) * 12, 32, 10.0f, 0xFFFFFFFF);
	//	}
	//	else
	//	{
	//		GGXX_DrawText1Ex(g_enemyInfo.m_name, 40, 32, 10.0f, 0xFFFFFFFF);
	//		getNameTrip(name);
	//		GGXX_DrawText1Ex(name, 640 -  40 - strlen(name) * 12, 32, 10.0f, 0xFFFFFFFF);
	//	}
	//}
}

void ggn_drawRankAndWin(DWORD p_side)
{
	if ((*GGXX_MODE1 & 0xe00000) == 0) return;
	
	char rank[32];
	if (*GGXX_MODE1 & 0x200000)
	{
		if (g_netMgr->m_playSide == p_side+1)
		{
			sprintf(rank, "RANK:%c WIN:%d", getRankChar(g_setting.rank), g_setting.wins);
		}
		else
		{
			sprintf(rank, "RANK:%c WIN:%d", getRankChar(g_enemyInfo.m_rank), g_enemyInfo.m_wins);
		}
	}
	else if (*GGXX_MODE1 & 0x400000)
	{
		if (p_side == 0) GGXX_DrawText1Ex("-REPLAY MODE-", 320 - 13 * 6, 3, 8.0f, 0xFFFFFFFF);

		if (p_side == 0)
		{
			sprintf(rank, "RANK:%c WIN:%d", getRankChar(g_replay.m_data.rank1P), g_replay.m_data.wins1P);
		}
		else
		{
			sprintf(rank, "RANK:%c WIN:%d", getRankChar(g_replay.m_data.rank2P), g_replay.m_data.wins2P);
		}
	}
	else if (*GGXX_MODE1 & 0x800000)
	{
		if (p_side == 0) GGXX_DrawText1Ex("-WATCH MODE-", 320 - 13 * 6, 3, 8.0f, 0xFFFFFFFF);

		if (p_side == 0)
		{
			sprintf(rank, "RANK:%c WIN:%d", getRankChar(g_replay.m_data.rank1P), g_replay.m_data.wins1P);
		}
		else
		{
			sprintf(rank, "RANK:%c WIN:%d", getRankChar(g_replay.m_data.rank2P), g_replay.m_data.wins2P);
		}
	}

	if (p_side == 0)
	{
		// 1P
		GGXX_DrawText1Ex(rank, 40, 3, 8.0f, 0xFFFFFFFF);
		
		/* ÉGÉâÅ[ÉÅÉbÉZÅ[ÉWÇ™Ç†ÇÍÇŒï\é¶ */
		if (g_netMgr->m_errMsg[0] != '\0') GGXX_DrawText1Ex(g_netMgr->m_errMsg, 320 - strlen(g_netMgr->m_errMsg) * 6, 240, 1.0f, 0xFFFFFFFF);
	}
	else
	{
		// 2P
		GGXX_DrawText1Ex(rank, 640 -  40 - strlen(rank) * 12, 3, 8.0f, 0xFFFFFFFF);
	}
}

void ggn_endBattle(void)
{
	if (*GGXX_MODE1 & 0x200000)
	{
		/* â¡ì_ÅEå∏ì_ */
		if (g_netMgr->m_playSide == 1)		addScore(*GGXX_WINCOUNT1P, *GGXX_WINCOUNT2P, g_enemyInfo.m_rank, g_enemyInfo.m_wins);
		else if (g_netMgr->m_playSide == 2)	addScore(*GGXX_WINCOUNT2P, *GGXX_WINCOUNT1P, g_enemyInfo.m_rank, g_enemyInfo.m_wins);

		/* òAèüÉJÉEÉìÉg */
		char win = 0; // lose = 0, win = 1, draw = 2
		if (g_netMgr->m_playSide == 1 && *GGXX_WINCOUNT1P > *GGXX_WINCOUNT2P) win = 1;
		else if (g_netMgr->m_playSide == 2 && *GGXX_WINCOUNT2P > *GGXX_WINCOUNT1P) win = 1;
		else if (*GGXX_WINCOUNT1P == *GGXX_WINCOUNT2P) win = 2;
		
		if (win == 1)
		{
			g_setting.wins++;
			g_setting.totalWin++;
		}
		else if (win == 0)
		{
			g_setting.wins = 0;
			g_setting.totalLose++;
		}
		else if (win == 2)
		{
			g_setting.totalDraw++;
		}

		writeSettingFile();
	
		// ÉäÉvÉåÉCÇ…èIí[Çïtâ¡Ç∑ÇÈ
		g_netMgr->disconnect("endbattle");
	}
	else if (*GGXX_MODE1 & 0x800000) // äœêÌèIóπ
	{
		// ÉäÉvÉåÉCÇ…èIí[Çïtâ¡Ç∑ÇÈ
		g_netMgr->disconnect("endbattle");
	}
}

bool ggn_procReplay(void)
{
	g_netMgr->m_lobbyFrame++;

	int input = (*GGXX_1PJDOWN & 0xf300) | (*GGXX_2PJDOWN & 0xf300);
	static int pressKeyTime[6] = {0,0,0,0,0,0};
	for (int i = 0; i < 6; i++)
	{
		int key;
		switch (i)
		{
		case 0: key = 0x0010; break;
		case 1: key = 0x0020; break;
		case 2: key = 0x0040; break;
		case 3: key = 0x0080; break;
		case 4: key = 0x0400; break;
		case 5: key = 0x0800; break;
		}
		if ((*GGXX_1PDOWN | *GGXX_2PDOWN) & key)
		{
			if ((*GGXX_1PJDOWN | *GGXX_2PJDOWN) & key)
			{
				input |= key;
				pressKeyTime[i] = 0;
			}
			else if (pressKeyTime[i] >= 0x11)
			{
				input |= key;
				pressKeyTime[i] = 0x0e;
			}
			else pressKeyTime[i]++;
		}
		else pressKeyTime[i] = 0;
	}
	
/* ï`âÊ */
	GGXX_DrawText2("REPLAY", 40, 53, 5);

	static int c1 = 0, c2 = 0x08;

	if (g_replay.m_dispItemHead > 0)
	{
		GGXX_DrawArrowMark(4, -5, 150, c1 + 1);
	}
	if (g_replay.m_itemlist.size() > g_replay.m_dispItemHead + g_replay.m_itemPerPage)
	{
		GGXX_DrawArrowMark(8, -5, 410, c1 + 1);
	}
	if (c1 == 0xe0) c2 = -0x08;
	if (c1 == 0x00) c2 =  0x08;
	c1 += c2;

/* ì¸óÕ */
	if (input & 0x0010)			/* Å™ÉLÅ[ */
	{
		if (g_replay.m_selectItemIdx > 0)
		{
			g_replay.m_selectItemIdx--;
			if (g_replay.m_selectItemIdx < g_replay.m_dispItemHead) g_replay.m_dispItemHead--;
			GGXX_PlayCmnSound(0x37);
		}
	}
	else if (input & 0x0040)	/* Å´ÉLÅ[ */
	{
		if (g_replay.m_selectItemIdx < g_replay.m_itemlist.size() - 1)
		{
			g_replay.m_selectItemIdx++;
			if (g_replay.m_selectItemIdx >= g_replay.m_dispItemHead + g_replay.m_itemPerPage) g_replay.m_dispItemHead++;
			GGXX_PlayCmnSound(0x37);
		}
	}
	else if (input & 0x0400)	/* L1ÉLÅ[ */
	{
		if (g_replay.m_selectItemIdx > 0)
		{
			for (int i = 0; i < g_replay.m_itemPerPage; i++)
			{
				if (g_replay.m_selectItemIdx > 0)
				{
					g_replay.m_selectItemIdx--;
					if (g_replay.m_selectItemIdx < g_replay.m_dispItemHead) g_replay.m_dispItemHead--;
				}
			}
			GGXX_PlayCmnSound(0x37);
		}
	}
	else if (input & 0x0800)	/* R1ÉLÅ[ */
	{
		if (g_replay.m_selectItemIdx < g_replay.m_itemlist.size() - 1)
		{
			for (int i = 0; i < g_replay.m_itemPerPage; i++)
			{
				if (g_replay.m_selectItemIdx < g_replay.m_itemlist.size() - 1)
				{
					g_replay.m_selectItemIdx++;
					if (g_replay.m_selectItemIdx >= g_replay.m_dispItemHead + g_replay.m_itemPerPage) g_replay.m_dispItemHead++;
				}
			}
			GGXX_PlayCmnSound(0x37);
		}
	}
	else if (input & 0x2000)	/* åàíË */
	{
		if (g_replay.m_selectItemIdx != -1 && g_replay.m_selectItemIdx < g_replay.m_itemlist.size())
		{
			/* replayÉfÅ[É^Çì«Ç›çûÇ› */
			if (g_replay.m_itemlist[g_replay.m_selectItemIdx]->dir)
			{
				__strncpy(g_replay.m_curdir, g_replay.m_itemlist[g_replay.m_selectItemIdx]->fname, 255);
				g_replay.m_itemlist.deleteAll();
				getReplayFileList(g_replay.m_curdir);
				GGXX_PlayCmnSound(0x39);

				g_replay.m_stackSelect.set(g_replay.m_level, g_replay.m_selectItemIdx);
				g_replay.m_stackPageHead.set(g_replay.m_level, g_replay.m_dispItemHead);
				g_replay.m_level++;
				g_replay.m_selectItemIdx = 0;
				g_replay.m_dispItemHead = 0;

				//if (g_replay.m_selectItemIdx >= g_replay.m_itemlist.size()) g_replay.m_selectItemIdx = g_replay.m_itemlist.size() - 1;
				//if (g_replay.m_dispItemHead >= g_replay.m_itemlist.size() - g_replay.m_itemPerPage) g_replay.m_dispItemHead = g_replay.m_itemlist.size() - g_replay.m_itemPerPage;
				//if (g_replay.m_selectItemIdx < 0) g_replay.m_selectItemIdx = 0;
				//if (g_replay.m_dispItemHead < 0) g_replay.m_dispItemHead = 0;
			}
			else
			{
				char reppath[1024];
				sprintf(reppath, "%s/%s", g_moduleDir, g_replay.m_itemlist[g_replay.m_selectItemIdx]->fname);
				FILE* fp = fopen(reppath, "rb");
				if (fp)
				{
					/* çƒê∂Ç∑ÇÈÉäÉvÉåÉCÇÃÉtÉHÅ[É}ÉbÉg */
					int repsize = 0;
					g_replay.m_format = zfsig(fp);
					if (g_replay.m_format == 0)
					{
						/* å›ä∑ê´ÇÃñ≥Ç¢ÉäÉvÉåÉCÇÕñ≥óù */
						repsize = -1;
						fclose(fp);
						GGXX_PlayCmnSound(0x3B);
					}
					else if (g_replay.m_format == 1 || g_replay.m_format == 2) // 108b, 109
					{
						ReplayFile_v109* rep109 = new ReplayFile_v109;
						for (int i = 0; i < MAXREPSIZE; i++) rep109->inputData[i] = 0xffffffff;
						zfread((char*)rep109, sizeof(ReplayFile_v109), fp);
						repsize = zfsize(fp);
						fclose(fp);

						g_replay.m_data.battleStage	= rep109->battleStage;
						g_replay.m_data.round		= 5;
						g_replay.m_data.time		= GGXX_TIME99;

						__strncpy(g_replay.m_data.name1P, rep109->name1P, 19);
						g_replay.m_data.rank1P	= rep109->rank1P;
						g_replay.m_data.wins1P	= rep109->wins1P;
						g_replay.m_data.chara1P	= rep109->chara1P;
						g_replay.m_data.ex1P	= rep109->ex1P;
						g_replay.m_data.voice1P	= rep109->voice1P;

						__strncpy(g_replay.m_data.name2P, rep109->name2P, 19);
						g_replay.m_data.rank2P	= rep109->rank2P;
						g_replay.m_data.wins2P	= rep109->wins2P;
						g_replay.m_data.chara2P	= rep109->chara2P;
						g_replay.m_data.ex2P	= rep109->ex2P;
						g_replay.m_data.voice2P	= rep109->voice2P;

						memcpy(g_replay.m_data.keySetting1P, rep109->keySetting1P, 0x18);
						memcpy(g_replay.m_data.keySetting2P, rep109->keySetting2P, 0x18);
						memcpy(g_replay.m_data.randTable, rep109->randTable, 0x9c4);
						memcpy(g_replay.m_data.palette1P, rep109->palette1P, 0x410);
						memcpy(g_replay.m_data.palette2P, rep109->palette2P, 0x410);
						memcpy(g_replay.m_data.inputData, rep109->inputData, repsize);
						
						delete rep109;
					}
					else
					{
						for (int i = 0; i < MAXREPSIZE; i++) g_replay.m_data.inputData[i] = 0xffffffff;

						zfread((char*)&g_replay.m_data, sizeof(ReplayFile), fp);
						repsize = zfsize(fp);
						fclose(fp);
					}

					/* ééçáäJén */
					if (repsize >= 0)
					{
						//WRITE_REPLAY_RAWDATA(repsize);

						g_replay.m_skipFrame = 0;
						g_replay.m_frameCount = 0;

						g_netMgr->m_suspend = false;
						
						g_netMgr->setErrMsg("");

						GGXX_PlayCmnSound(0x39);
						*GGXX_MODE1		= 0x400803;
						*GGXX_MODE2		= 0x11;
						GGXX_InitBattle();
						GGXX_InitBattleChara(0);
						GGXX_InitBattleChara(1);

						Sleep(500);
						
						int chara1P = g_replay.m_data.chara1P;
						int chara2P = g_replay.m_data.chara2P;
						int stageID = g_replay.m_data.battleStage;
						
						if (g_replay.m_data.round == 3)	*GGXX_ROUND = 2;
						else						*GGXX_ROUND = 3;

						_asm
						{
							push 0
							push 0	// color
							push dword ptr ds:[chara1P]	// chara
							push 0
							mov eax, 0x45edd0
							call eax
							
							push 0
							push 0	// color
							push dword ptr ds:[chara2P]	// chara
							push 1
							mov eax, 0x45edd0
							call eax

							push 0	// ai
							push 0
							mov eax, 0x45eea0
							call eax

							push 0	// ai
							push 1
							mov eax, 0x45eea0
							call eax

							push stageID
							mov eax, 0x410B80 // stageinit
							call eax
							add esp, 0x34
						}
						g_replay.m_playing = true;
						return 1;
					}
					else
					{
						/* ReplayâÊñ äJÇ¢ÇΩå„Ç…ÉtÉ@ÉCÉãçÌèúÇµÇΩèÍçáÇ∆Ç©Åc */
					}
				}
			}
		}
	}
	else if (input & 0x4000)	/* ÉLÉÉÉìÉZÉã */
	{
		if (strcmp(g_replay.m_curdir, "REPLAY") == 0)
		{
			g_netMgr->m_lobbyFrame = -1;

			GGXX_PlayCmnSound(0x3B);
			*GGXX_MODE2 = 0x19;
			return 1;
		}
		else
		{
			char* pdir = strrchr(g_replay.m_curdir, '/');
			if (pdir)
			{
				*pdir = '\0';
				g_replay.m_itemlist.deleteAll();
				getReplayFileList(g_replay.m_curdir);
				GGXX_PlayCmnSound(0x3B);
			}
			else
			{
				/* îOÇÃÇΩÇﬂ */
				g_replay.m_itemlist.deleteAll();
				__strncpy(g_replay.m_curdir, "REPLAY", 255);
				getReplayFileList(g_replay.m_curdir);
				g_replay.m_selectItemIdx = 0;
				g_replay.m_dispItemHead	= 0;
				GGXX_PlayCmnSound(0x3B);
			}

			if (g_replay.m_level > 0)
			{
				g_replay.m_selectItemIdx = g_replay.m_stackSelect[g_replay.m_level - 1];
				g_replay.m_dispItemHead = g_replay.m_stackPageHead[g_replay.m_level - 1];
				if (g_replay.m_selectItemIdx >= g_replay.m_itemlist.size()) g_replay.m_selectItemIdx = g_replay.m_itemlist.size() - 1;
				if (g_replay.m_dispItemHead >= g_replay.m_itemlist.size() - g_replay.m_itemPerPage) g_replay.m_dispItemHead = g_replay.m_itemlist.size() - g_replay.m_itemPerPage;
				if (g_replay.m_selectItemIdx < 0) g_replay.m_selectItemIdx = 0;
				if (g_replay.m_dispItemHead < 0) g_replay.m_dispItemHead = 0;
				g_replay.m_level--;
			}

		}
	}
	
	return 0;
}

void ggn_syncKeySetting(void)
{
	/* RoundÇ≤Ç∆Ç…çsÇ¡ÇƒÇ¢ÇÈÅBÉtÉâÉOópà”ÇµÇƒèââÒÇÃÇPâÒÇÃÇ›ëóéÛêMÇ∑ÇÈÇÊÇ§Ç…Ç∑ÇÈ */
	DBGOUT_NET("round time = %d-%d\n", g_netMgr->m_time, *GGXX_FRAMECOUNTER);

	/* ÉLÅ[ê›íËÇçáÇÌÇπÇÈ */
	if (*GGXX_MODE1 & 0x200000)
	{
		DBGOUT_NET("random counter = %d\n", *GGXX_RANDOMCOUNTER);
		DBGOUT_NET("sync keyset\n");
		if (g_netMgr->m_initKeySet == false)
		{
			if (g_netMgr->m_playSide == 1)
			{
				if (g_netMgr->sendDataBlock(Block_KeySetting, (char*)GGXX_KEYSET_1P, GGXX_KEYSETSIZE, TIMEOUT_BLOCK) == false)
				{
					g_netMgr->disconnect("send block  keyset");
					return;
				}
				if (g_netMgr->recvDataBlock(Block_KeySetting, (char*)GGXX_KEYSET_2P, GGXX_KEYSETSIZE, TIMEOUT_BLOCK) == false)
				{
					g_netMgr->disconnect("recv blockkeyset");
					return;
				}
			}
			else
			{
				/* é©ï™Ç™2PÇ…Ç»Ç¡ÇΩèÍçáÅA1PÇ≈égÇ¡ÇƒÇ¢ÇÈÉLÅ[ê›íËÇégópÇ∑ÇÈÅBVSNetÇ≈ÇÕ2Pë§ÇÃÉLÅ[ê›íËÇÕégópÇ≥ÇÍÇ»Ç¢ */
				memcpy(GGXX_KEYSET_2P, GGXX_KEYSET_1P, GGXX_KEYSETSIZE);

				if (g_netMgr->recvDataBlock(Block_KeySetting, (char*)GGXX_KEYSET_1P, GGXX_KEYSETSIZE, TIMEOUT_BLOCK) == false)
				{
					g_netMgr->disconnect("recv blockkeyset");
					return;
				}
				if (g_netMgr->sendDataBlock(Block_KeySetting, (char*)GGXX_KEYSET_2P, GGXX_KEYSETSIZE, TIMEOUT_BLOCK) == false)
				{
					g_netMgr->disconnect("send block  keyset");
					return;
				}
			}
			/* ÉLÅ[ê›íËÇreplayÇ…ï€ë∂ */
			memcpy(g_replay.m_data.keySetting1P, GGXX_KEYSET_1P, GGXX_KEYSETSIZE);
			memcpy(g_replay.m_data.keySetting2P, GGXX_KEYSET_2P, GGXX_KEYSETSIZE);
		}
		else
		{
			/* ëOÉâÉEÉìÉhÇ≈replayÇ…ï€ë∂ÇµÇƒÇ†ÇÈÇÃÇ≈ÇªÇÍÇçƒóòóp */
			memcpy(GGXX_KEYSET_1P, g_replay.m_data.keySetting1P, GGXX_KEYSETSIZE);
			memcpy(GGXX_KEYSET_2P, g_replay.m_data.keySetting2P, GGXX_KEYSETSIZE);
		}
	}
	else if (*GGXX_MODE1 & 0xc00000)
	{
		memcpy(GGXX_KEYSET_1P, g_replay.m_data.keySetting1P, GGXX_KEYSETSIZE);
		memcpy(GGXX_KEYSET_2P, g_replay.m_data.keySetting2P, GGXX_KEYSETSIZE);
	}
	
	g_netMgr->m_initKeySet = true;
}

void ggn_startReplay(void)
{
	g_netMgr->setErrMsg("");
	
	g_replay.m_itemPerPage = 20;

	g_replay.m_itemlist.deleteAll();
	getReplayFileList(g_replay.m_curdir);

	if (g_replay.m_playing == false)
	{
		g_replay.m_stackSelect.removeAll();
		g_replay.m_stackPageHead.removeAll();
		g_replay.m_level = 0;
	}
	g_replay.m_playing = false;

	if (g_replay.m_selectItemIdx >= g_replay.m_itemlist.size()) g_replay.m_selectItemIdx = g_replay.m_itemlist.size() - 1;
	if (g_replay.m_dispItemHead >= g_replay.m_itemlist.size() - g_replay.m_itemPerPage) g_replay.m_dispItemHead = g_replay.m_itemlist.size() - g_replay.m_itemPerPage;
	if (g_replay.m_selectItemIdx < 0) g_replay.m_selectItemIdx = 0;
	if (g_replay.m_dispItemHead < 0) g_replay.m_dispItemHead = 0;
	
	g_netMgr->m_lobbyFrame = 0;
}

void ggn_endCS(void)
{
	if (*GGXX_MODE1 & 0x200000)
	{
		DBGOUT_NET("ggn_endCS...\n");
		g_netMgr->suspend();
		g_netMgr->m_recvVSLoadCompleted = false;
		g_netMgr->m_vsloadFrame = 0; /* 0à»è„Ç»ÇÁvsloadíÜ */
	}
}

void ggn_endVS(void)
{
	if (*GGXX_MODE1 & 0x200000)
	{
		DBGOUT_NET("ggn_endVS...\n");
		
		/*
		Ç±Ç±Ç≈Ç‡ëäéËÇÃVSLoadÇ™èIÇÌÇÈÇ‹Ç≈ë“Ç¬
		VSâÊñ Ç™èIÇÌÇÈÇ‹Ç≈Ç…ÉçÅ[ÉhäÆóπÇµÇƒÇ¢Ç»Ç¢Ç∆ggn_vsLoadCompleted()Ç™é¿çsÇ≥ÇÍÇ»Ç¢
		ÇªÇÃèÍçáVSLoadCompletedÇ™ëäéËÇ…ëóÇÁÇÍÇ»Ç¢
		*/
		while (g_netMgr->m_vsloadFrame * 10 < TIMEOUT_VSLOAD)
		{
			if (g_netMgr->m_vsloadFrame % 5 == 0) g_netMgr->send_vsLoadCompleted();
			Sleep(10);
			g_netMgr->m_vsloadFrame++;

			/* îOÇÃÇΩÇﬂè≠ÇµëóÇ¡ÇƒÇ®Ç≠ */
			if (g_netMgr->m_vsloadFrame > 30 && g_netMgr->m_recvVSLoadCompleted) break;
		}
		
		g_netMgr->m_vsloadFrame = -1;

		g_replay.m_data.battleStage	= *GGXX_BATTLESTAGE;
		g_replay.m_data.round		= *GGXX_ROUND == 2 ? 3 : 5;
		g_replay.m_data.time		= GGXX_TIME99;

		char str[1024];
		getNameTrip(str);
		if (g_netMgr->m_playSide == 1)
		{
			__strncpy(g_replay.m_data.name1P, str, 29);
			g_replay.m_data.rank1P		= g_setting.rank;
			g_replay.m_data.wins1P		= g_setting.wins;
			g_replay.m_data.chara1P		= *GGXX_1PCHARA;
			g_replay.m_data.ex1P		= (char)*GGXX_1PEX;
			g_replay.m_data.voice1P		= (char)*GGXX_1PVOICE;
			
			__strncpy(g_replay.m_data.name2P, g_enemyInfo.m_name, 29);
			g_replay.m_data.rank2P		= g_enemyInfo.m_rank;
			g_replay.m_data.wins2P		= g_enemyInfo.m_wins;
			g_replay.m_data.chara2P		= *GGXX_2PCHARA;
			g_replay.m_data.ex2P		= (char)*GGXX_2PEX;
			g_replay.m_data.voice2P		= (char)*GGXX_2PVOICE;
		}
		else
		{
			__strncpy(g_replay.m_data.name1P, g_enemyInfo.m_name, 29);
			g_replay.m_data.rank1P		= g_enemyInfo.m_rank;
			g_replay.m_data.wins1P		= g_enemyInfo.m_wins;
			g_replay.m_data.chara1P		= *GGXX_1PCHARA;
			g_replay.m_data.ex1P		= (char)*GGXX_1PEX;
			g_replay.m_data.voice1P		= (char)*GGXX_1PVOICE;

			__strncpy(g_replay.m_data.name2P, str, 29);
			g_replay.m_data.rank2P		= g_setting.rank;
			g_replay.m_data.wins2P		= g_setting.wins;
			g_replay.m_data.chara2P		= *GGXX_2PCHARA;
			g_replay.m_data.ex2P		= (char)*GGXX_2PEX;
			g_replay.m_data.voice2P		= (char)*GGXX_2PVOICE;
		}
		/* Ç±Ç±Ç©ÇÁReplayÇãLò^Ç∑ÇÈ */
		g_replay.m_repRecording = true;
		g_replay.m_frameCount = 0;

	}
}

void ggn_randomLog(DWORD p_eip)
{
#if DEBUG_OUTPUT_RND
	char str[256];

	char label[256];
	strcpy(label, "");
	if (p_eip == 0x00408a59)		strcpy(label, "èüóòÉ|Å[ÉYA");
	else if (p_eip == 0x004034af)	strcpy(label, "í èÌçUåÇî≠ê∫");
	else if (p_eip == 0x0042255e)	strcpy(label, "Ç≠ÇÁÇ¢A");
	else if (p_eip == 0x0044e4c1)	strcpy(label, "Ç≠ÇÁÇ¢É{ÉCÉX");
	else if (p_eip == 0x00439c41)	strcpy(label, "Ç»Ç…Ç™Ç≈ÇÈÇ©Ç»ÅH");
	else if (p_eip == 0x004fda07)	strcpy(label, "ggna_useSpecialRandom");
	else if (p_eip == 0x0043acf4)	strcpy(label, "ÉÅÉeÉIA");
	else if (p_eip == 0x0043ad1b)	strcpy(label, "ÉÅÉeÉIB");
	else if (p_eip == 0x0043ad2c)	strcpy(label, "ÉÅÉeÉIC");
	else if (p_eip == 0x0041bba5)	strcpy(label, "É`ÉbÉvíßî≠");
	else if (p_eip == 0x0041bbd5)	strcpy(label, "É`ÉbÉvåhà”");
	else if (p_eip == 0x0041f74d)	strcpy(label, "ÉjÉÖÅ[ÉgÉâÉã");
	else if (p_eip == 0x00438e5e)	strcpy(label, "üØí∞A");
	else if (p_eip == 0x00438d4c)	strcpy(label, "üØí∞B");
	else if (p_eip == 0x004227d2)	strcpy(label, "ÉUÉbÉpì∆ÇËåæA");
	else if (p_eip == 0x004227ed)	strcpy(label, "ÉUÉbÉpì∆ÇËåæB");
	else if (p_eip == 0x0042269b)	strcpy(label, "ÉKÅ[ÉhÉ{ÉCÉX");

	if (*GGXX_MODE1 & 0x200000)
	{
		sprintf(str, "%04d : eip=%08x : rnd=%08x(%03d)", *GGXX_FRAMECOUNTER, p_eip, GGXX_RANDOMTABLE[*GGXX_RANDOMCOUNTER], *GGXX_RANDOMCOUNTER);
	}
	else if (*GGXX_MODE1 & 0xc00000)
	{
		sprintf(str, "%04d : eip=%08x : rnd=%08x(%03d)", *GGXX_FRAMECOUNTER, p_eip, GGXX_RANDOMTABLE[*GGXX_RANDOMCOUNTER], *GGXX_RANDOMCOUNTER);
	}
	else return;

	if (label[0] == 0) sprintf(str, "%s\n", str);
	else sprintf(str, "%s%s\n", str, label);

	ENTERCS(&g_csLogOut);
	if (strlen(g_rndLog) + strlen(str) < LOG_SIZE) strcat(g_rndLog, str);
	LEAVECS(&g_csLogOut);
#endif
}

bool ggn_useSpecialRandom(char* p_data)
{
	/* ãLò^óêêîÇégÇ§Ç©ÅH */
	int id = 0;
	if (p_data[0] == 0x59 && p_data[12] == 0x1b) id =  1;/* ÉUÉbÉpóHóÏ è¨êŒ */
	if (p_data[0] == 0x59 && p_data[12] == 0x20) id =  2;/* ÉUÉbÉpóHóÏ î´êAÇ¶ */
	if (p_data[0] == 0x59 && p_data[12] == 0x21) id =  3;/* ÉUÉbÉpóHóÏ ÉoÉiÉi */
	if (p_data[0] == 0x4a && p_data[12] == 0x01) id =  4;/* ÉtÉ@ÉEÉXÉg ÇøÇ—ÉtÉ@ÉEÉXÉg */
	if (p_data[0] == 0x4a && p_data[12] == 0x1f) id =  5;/* ÉtÉ@ÉEÉXÉg ÇøÇ—É|É`Éá */
	if (p_data[0] == 0x2b && p_data[12] == 0x04) id =  6;/* ÉtÉ@ÉEÉXÉg ÉnÉìÉ}Å[ */
	if (p_data[0] == 0x2b && p_data[12] == 0x1e) id =  7;/* ÉtÉ@ÉEÉXÉg ì≈ */
	if (p_data[0] == 0x2b && p_data[12] == 0x06) id =  8;/* ÉtÉ@ÉEÉXÉg îöíe */
	if (p_data[0] == 0x2b && p_data[12] == 0x03) id =  9;/* ÉtÉ@ÉEÉXÉg ÉhÅ[ÉiÉc */
	if (p_data[0] == 0x2b && p_data[12] == 0x02) id = 10;/* ÉtÉ@ÉEÉXÉg É`ÉáÉR */
	if (p_data[0] == 0x3b && p_data[12] == 0x14) id = 11;/* É`ÉbÉvÅ@Å@ éËó†åï */
	if (p_data[0] == 0x51 && p_data[12] == 0x16) id = 12;/* ÉAÉNÉZÉãÅ@ àÍåÇç≈å„ */
	if (p_data[0] == 0x51 && p_data[12] == 0x14) id = 12;/* ÉAÉNÉZÉãÅ@ àÍåÇç≈å„ */
	if (p_data[0] == 0x4d && p_data[12] == 0x0b) id = 13;/* ÉAÉNÉZÉãÅ@ àÍåÇA */
	if (p_data[0] == 0x4d && p_data[12] == 0x05) id = 13;/* ÉAÉNÉZÉãÅ@ àÍåÇB */
	if (p_data[0] == 0x2e && p_data[12] == 0x04) id = 14;/* É~ÉäÉAÅ@Å@ àÍåÇ */
	if (p_data[0] == 0x2e && p_data[12] == 0x05) id = 14;/* É~ÉäÉAÅ@Å@ àÍåÇ */
	if (p_data[0] == 0x2e && p_data[12] == 0x06) id = 14;/* É~ÉäÉAÅ@Å@ àÍåÇ */
	if (p_data[0] == 0x2e && p_data[12] == 0x07) id = 14;/* É~ÉäÉAÅ@Å@ àÍåÇ */
	if (p_data[0] == 0x68 && p_data[12] == 0x04) id = 15;/* ÉWÉÉÉXÉeÉBÉX NB HS */
	if (p_data[0] == 0x68 && p_data[12] == 0x05) id = 15;/* ÉWÉÉÉXÉeÉBÉX NB S */

#if DEBUG_OUTPUT_RND
	char str[256];
	strcpy(str, "");
	switch (id)
	{
	case  1: strcpy(str, "ÉUÉbÉpóHóÏ è¨êŒ\n");				break;
	case  2: strcpy(str, "ÉUÉbÉpóHóÏ î´êAÇ¶\n");			break;
	case  3: strcpy(str, "ÉUÉbÉpóHóÏ ÉoÉiÉi\n");			break;
	case  4: strcpy(str, "ÉtÉ@ÉEÉXÉg ÇøÇ—ÉtÉ@ÉEÉXÉg\n");	break;
	case  5: strcpy(str, "ÉtÉ@ÉEÉXÉg ÇøÇ—É|É`Éá\n");		break;
	case  6: strcpy(str, "ÉtÉ@ÉEÉXÉg ÉnÉìÉ}Å[\n");			break;
	case  7: strcpy(str, "ÉtÉ@ÉEÉXÉg ì≈\n");				break;
	case  8: strcpy(str, "ÉtÉ@ÉEÉXÉg îöíe\n");				break;
	case  9: strcpy(str, "ÉtÉ@ÉEÉXÉg ÉhÅ[ÉiÉc\n");			break;
	case 10: strcpy(str, "ÉtÉ@ÉEÉXÉg É`ÉáÉR\n");			break;
	case 11: strcpy(str, "É`ÉbÉvÅ@Å@ éËó†åï\n");			break;
	case 12: strcpy(str, "ÉAÉNÉZÉãÅ@ àÍåÇA\n");				break;
	case 13: strcpy(str, "ÉAÉNÉZÉãÅ@ àÍåÇB\n");				break;
	case 14: strcpy(str, "É~ÉäÉAÅ@Å@ àÍåÇ\n");				break;
	case 15: strcpy(str, "ÉWÉÉÉXÉeÉBÉX NB\n");				break;
	}
	ENTERCS(&g_csLogOut);
	if (strlen(g_rndLog) + strlen(str) < LOG_SIZE) strcat(g_rndLog, str);
	LEAVECS(&g_csLogOut);
#endif

	return (id != 0);
}

void ggn_randomShuffle(void)
{
	// óêêîÇãÛâÒÇµ
	if ((*GGXX_MODE1 & 0xc00000) && g_replay.m_format == 1)
	{
		// å›ä∑ê´à€éùÇÃÇΩÇﬂà»ëOÇÃÇ‹Ç‹
	}
	else
	{
		GGXX_GetRandom();
	}
}

void ggn_init(LPDIRECT3DDEVICE8* p_d3dDev)
{
	if (g_d3dfont == NULL)
	{
		g_d3dDev = *p_d3dDev;
		g_d3dfont = new CD3DFont(*p_d3dDev, *GGXX_ENABLEPIXELSHADER ? D3DFMT_A8R8G8B8 : *GGXX_TEXFORMAT);
		g_dirIcon = new CIcon(g_dllInst ,g_d3dDev, IDI_ICON1, *GGXX_ENABLEPIXELSHADER ? D3DFMT_A8R8G8B8 : *GGXX_TEXFORMAT);
	}
	
	if (*GGXX_FULLSCREEN == 0) // windowÉÇÅ[ÉhéûÇÃÇ›
	{
		int w, h;
		getWindowSize((int)(640.0f * g_iniFileInfo.m_zoomx), (int)(480.0f * g_iniFileInfo.m_zoomy), &w, &h);

		UINT flag = 0;
		if (g_iniFileInfo.m_posx  == -999 && g_iniFileInfo.m_posy  == -999) flag |= SWP_NOMOVE;
		if (g_iniFileInfo.m_zoomx == 1.0f && g_iniFileInfo.m_zoomy == 1.0f) flag |= SWP_NOSIZE;
		
		if (flag == (SWP_NOMOVE | SWP_NOSIZE))
		{
			flag = 0;
		}
		else
		{
			SetWindowPos(*GGXX_HWND, NULL, g_iniFileInfo.m_posx, g_iniFileInfo.m_posy, w, h, flag | SWP_NOZORDER | SWP_SHOWWINDOW);
		}
	}

#if _DEBUG
	char title[256];
	sprintf(title, "name:%s port:%d", g_setting.userName, g_setting.port);
	SetWindowText(*GGXX_HWND, title);
#endif

	srand(timeGetTime());
	while (rand() % 16 != 0) GGXX_GetRandom();
}

void ggn_cleanup(void)
{
	/* ÉfÉoÉCÉXÉçÉXÉgéûëOÇ…åƒÇŒÇÍÇÈ */
	if (*GGXX_FULLSCREEN == 0) // windowÉÇÅ[ÉhéûÇÃÇ›
	{
		RECT rect;
		GetClientRect(*GGXX_HWND, &rect);
		g_iniFileInfo.m_zoomx = (float)(rect.right - rect.left) / 640.0f;
		g_iniFileInfo.m_zoomy = (float)(rect.bottom - rect.top) / 480.0f;

		int w, h;
		getWindowSize(640, 480, &w, &h);
		SetWindowPos(*GGXX_HWND, NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}
}

void ggn_render(void)
{
	char str[256];
	const int LINESIZE = 13;
	
	g_d3dDev->SetPixelShader(NULL);

	if (*GGXX_MODE1 & 0x200000 && !g_netMgr->m_connect && !(g_netMgr->m_watch && g_replay.m_playing) && g_netMgr->m_lobbyFrame >= 5)
	{
		D3DV_GGN	d3dv[4];
		
		enum
		{
			X_NAME = 20,
			X_RANK = 230,
			X_WIN  = 265,
			X_COUNT= 300,
			X_PING = 340,
			X_VER  = 390,
			X_RND  = 440,
			X_EX   = 480,
			X_GALLERY = 520,
			X_STAT = 590,
		};
		//drawGGXXWindow("", -1, 15, 145, 620, 370);

		g_d3dfont->setFont(g_iniFileInfo.m_fontName, g_iniFileInfo.m_fontSize, g_iniFileInfo.m_fontAntialias, FONT_W * 2, 0);
		g_d3dfont->drawText("Player Name", X_NAME, 150, 0xffffff00);
		g_d3dfont->drawText("Rank", X_RANK, 150, 0xffffff00, CD3DFont::Align_Center);
		g_d3dfont->drawText("Win", X_WIN, 150, 0xffffff00, CD3DFont::Align_Center);
		g_d3dfont->drawText("Count", X_COUNT, 150, 0xffffff00, CD3DFont::Align_Center);
		g_d3dfont->drawText("Ping", X_PING, 150, 0xffffff00, CD3DFont::Align_Center);
		g_d3dfont->drawText("Version", X_VER, 150, 0xffffff00, CD3DFont::Align_Center);
		g_d3dfont->drawText("Round", X_RND, 150, 0xffffff00, CD3DFont::Align_Center);
		g_d3dfont->drawText("Ex", X_EX, 150, 0xffffff00, CD3DFont::Align_Center);
		g_d3dfont->drawText("Gallery", X_GALLERY, 150, 0xffffff00, CD3DFont::Align_Center);
		g_d3dfont->drawText("Status", X_STAT, 150, 0xffffff00, CD3DFont::Align_Center);

		g_d3dfont->drawText("User Count", 485, 440, 0xffffff00);

		drawGGXXWindow("", -1, 15, 103, 350, 140);
		g_d3dfont->drawText("Player Name", 20, 108, 0xffffff00);
		g_d3dfont->drawText("Game Count", 20, 124, 0xffffff00);
		g_d3dfont->drawText("Rank", 250, 108, 0xffffff00);
		g_d3dfont->drawText("Win", 250, 124, 0xffffff00);

		g_d3dfont->setFont(g_iniFileInfo.m_fontName, g_iniFileInfo.m_fontSize, g_iniFileInfo.m_fontAntialias, FONT_W, 0);
		sprintf(str, "Åõ - Menu\nÅ¢ - Sort (%s)\nÅ~ - Exit", g_sortstr[g_vsnet.m_sortType]);
		drawGGXXWindow(str, -1, 470, 94, 620, 140);
		
		sprintf(str, ": %s", g_setting.userName);
		g_d3dfont->drawText(str, 110, 108, 0xffffffff);

		sprintf(str, ": %d", g_setting.totalBattle);
		g_d3dfont->drawText(str, 110, 124, 0xffffffff);

		float point = (float)g_setting.score / 16000.0f;
		char  sign  = ' ';
		if (point <= -0.01) sign = '-';
		if (point >=  0.01) sign = '+';
		sprintf(str, ": %c  %c%d%%", getRankChar(g_setting.rank), sign, (int)fabs(point * 100));
		g_d3dfont->drawText(str, 290, 108, 0xffffffff);

		sprintf(str, ": %d", g_setting.wins);
		g_d3dfont->drawText(str, 290, 124, 0xffffffff);

		/* usercount */
		if (g_nodeMgr->getNodeCount() > 0)
		{
			sprintf(str, "%4d/%4d", g_vsnet.m_selectItemIdx + 1, g_nodeMgr->getNodeCount());
			g_d3dfont->drawText(str, 630, 440, 0xffffffff, CD3DFont::Align_Right);
		}
		else g_d3dfont->drawText("----/----", 630, 440, 0xffffffff, CD3DFont::Align_Right);

		ENTERCS(&g_netMgr->m_csNode);

		/* ëIëîwåi */
		if (g_vsnet.m_selectItemIdx != -1 && g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount())
		{
			int row = (g_vsnet.m_selectItemIdx - g_vsnet.m_dispItemHead);
			g_d3dDev->SetTexture(0, NULL);
			d3dv[0].setPos(20.0f, (float)(167 + row * LINESIZE));
			d3dv[0].setColor(0x800000ff);
			d3dv[1].setPos(630.0f, (float)(167 + row * LINESIZE));
			d3dv[1].setColor(0x800000ff);
			d3dv[2].setPos(20.0f, (float)(167 + row * LINESIZE + 12));
			d3dv[2].setColor(0x800000ff);
			d3dv[3].setPos(630.0f, (float)(167 + row * LINESIZE + 12));
			d3dv[3].setColor(0x800000ff);
			g_d3dDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, d3dv, sizeof(D3DV_GGN));
		}
		
		int line = 0;
		for (int i = 0; i < g_nodeMgr->getNodeCount(); i++)
		{
			CNode* node = g_nodeMgr->getNode(i);

			if (i < g_vsnet.m_dispItemHead) continue;
			if (i >= g_vsnet.m_dispItemHead + g_vsnet.m_itemPerPage) break;

			DWORD argb;
			int denyidx = g_denyListMgr->find(node->m_id);
			if (denyidx == -1)		argb = 0xffffffff;
			else if (denyidx >= 0)	argb = 0xffff0000;
			else					argb = 0x80ffffff;

			// äœêÌÇÕãÊï Ç≈Ç´ÇÈÇÊÇ§Ç…êFï™ÇØÅ@ÇΩÇæÇµÅAãëî€Ç≥ÇÍÇƒÇ¢ÇΩÇÁÇªÇÍÇóDêÊ
			if (denyidx < 0)
			{
				if (node->m_state == State_Busy_Casting ||
					node->m_state == State_Busy_Casting_NG)
				{
					argb = 0xff80ff80;
				}
			}

			if (node->m_state != State_Idle &&
				node->m_state != State_Watch_Playable &&
				node->m_state != State_Busy_Casting)
			{
				argb &= 0x80ffffff;
			}
			
			g_d3dfont->drawText(node->m_name, X_NAME, 167 + line * LINESIZE, argb);

			sprintf(str, "%c", (node->m_validInfo & VF_RANK) ? getRankChar(node->m_rank) : '-');
			g_d3dfont->drawText(str, X_RANK, 167 + line * LINESIZE, argb, CD3DFont::Align_Center);

			if (node->m_validInfo & VF_WINS) sprintf(str, "%d", node->m_win);
			else sprintf(str, "-");
			g_d3dfont->drawText(str, X_WIN, 167 + line * LINESIZE, argb, CD3DFont::Align_Center);
			
			if (node->m_validInfo & VF_COUNT) sprintf(str, "%d", node->m_gamecount);
			else sprintf(str, "-");
			g_d3dfont->drawText(str, X_COUNT, 167 + line * LINESIZE, argb, CD3DFont::Align_Center);

			int ping = node->getPingAv();
			if (ping >= 0) sprintf(str, "%3dms", ping);
			else sprintf(str, "-----");
			g_d3dfont->drawText(str, X_PING, 167 + line * LINESIZE, argb, CD3DFont::Align_Center);
			
			g_d3dfont->drawText(node->m_ver, X_VER, 167 + line * LINESIZE, argb, CD3DFont::Align_Center);

			if (node->m_validInfo & VF_ROUND) sprintf(str, "%d", node->m_round);
			else sprintf(str, "-");
			g_d3dfont->drawText(str, X_RND, 167 + line * LINESIZE, argb, CD3DFont::Align_Center);

			if (node->m_validInfo & VF_EX)
			{
				if (g_setting.useEx == 0) sprintf(str, "Å~");
				else sprintf(str, "%s", node->m_ex == 1 ? "Åõ" : "Å~");
			}
			else sprintf(str, "-");
			g_d3dfont->drawText(str, X_EX, 167 + line * LINESIZE, argb, CD3DFont::Align_Center);

			if (node->m_state == State_Busy_Casting) sprintf(str, "%d", node->m_galleryCount);
			else if (node->m_validInfo & VF_CAST) sprintf(str, node->m_watchMaxNode == -16 ? "Å~" : "----");
			else sprintf(str, "----");
			g_d3dfont->drawText(str, X_GALLERY, 167 + line * LINESIZE, argb, CD3DFont::Align_Center);

			switch (node->m_state)
			{
			case State_Mismatch:
				if (node->m_delay != g_setting.delay)
				{
					sprintf(str, "Delay %d", node->m_delay);
				}
				else
				{
					strcpy(str, node->m_ex ? "Ex Enable" : "Ex Disable");
				}
				break;
			case State_VersionError:strcpy(str, "Version Error");	break;
			case State_PingOver:	strcpy(str, "Ping Over");		break;
			case State_Idle:		strcpy(str, "Idle");			break;
			case State_Busy:		strcpy(str, "Busy");			break;
			case State_NoResponse:	strcpy(str, "No Response");		break;
			case State_NotReady:	strcpy(str, "Not Ready");		break;
			case State_Unknown:		strcpy(str, "No Response");		break;	/* txtÇ…Ç©Ç©ÇÍÇƒÇ¢ÇÈÇ™Ç‹ÇæâûìöÇ™ñ≥Ç¢èÍçá */
			case State_Watch:			strcpy(str, "Watch");		break;
			case State_Watch_Playable:	strcpy(str, "Watch");		break;
			case State_Busy_Casting:	strcpy(str, "Busy(Casting)");	break;
			case State_Busy_Casting_NG:	strcpy(str, "Busy(Casting)");	break;
			}
			
			if ((node->m_validInfo & VF_ID) && node->m_deny) strcpy(str, "Denied");

			g_d3dfont->drawText(str, X_STAT, 167 + line * LINESIZE, argb, CD3DFont::Align_Center);

			line++;
		}
		
		if (g_vsnet.m_selectItemIdx != -1 && g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount())
		{
			CNode* selnode = g_nodeMgr->getNode(g_vsnet.m_selectItemIdx);
		
			drawGGXXWindow(selnode->m_msg, -1, 20, 385, 480, 460);
		
			if (g_vsnet.m_menu_visible)
			{
				// ÉoÉbÉNÇà√Ç≠Ç∑ÇÈ
				g_d3dDev->SetTexture(0, NULL);
				d3dv[0].setPos(0.0f, 0.0f);
				d3dv[0].setColor(0x40000000);
				d3dv[1].setPos(640.0f, 0.0f);
				d3dv[1].setColor(0x40000000);
				d3dv[2].setPos(0.0f, 480.0f);
				d3dv[2].setColor(0x40000000);
				d3dv[3].setPos(640.0f, 480.0f);
				d3dv[3].setColor(0x40000000);
				g_d3dDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, d3dv, sizeof(D3DV_GGN));

				char str[256] = "";
				bool playable = selnode->m_state == State_Idle || selnode->m_state == State_Watch_Playable;
				if (g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount() &&
				   (playable == false ||
				   selnode->m_deny ||							// ëäéËÇ©ÇÁãëî€
				   g_denyListMgr->find(selnode->m_id) != -1))	// é©ï™Ç≈ãëî€ñîÇÕÉRÅ[ÉhÇ™INVALID_MID
				{
					if (selnode->m_state == State_Busy_Casting)
					{
						if (selnode->m_battleInfoChara[0]-1 >= 0 && selnode->m_battleInfoChara[0]-1 < CHARACOUNT &&
							selnode->m_battleInfoChara[1]-1 >= 0 && selnode->m_battleInfoChara[1]-1 < CHARACOUNT)
						{
							sprintf_s(str, 256, "Watch game [%s(%c%c) vs %s(%c%c)]\n",
								selnode->m_battleInfoName[0],
								g_charaNames[selnode->m_battleInfoChara[0]-1][0],
								g_charaNames[selnode->m_battleInfoChara[0]-1][1],
								selnode->m_battleInfoName[1],
								g_charaNames[selnode->m_battleInfoChara[1]-1][0],
								g_charaNames[selnode->m_battleInfoChara[1]-1][1]);
						}
						else strcpy(str, "Watch game\n");
					}
					else if (selnode->m_state == State_Busy_Casting_NG)
					{
						strcpy(str, "!Watch game\n");
					}
					else
					{
						/* Åu!ÅvÇÕÉOÉåÉCï\é¶ */
						strcpy(str, "!Play game\n");
					}
				}
				else strcpy(str, "Play game\n");

				if (g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount() && idcmp((BYTE*)selnode->m_id, INVALID_MID))
				{
					strcat(str, "!");/* Åu!ÅvÇÕÉOÉåÉCï\é¶ */
				}

				if (g_vsnet.m_selectItemIdx < g_nodeMgr->getNodeCount() &&
					g_denyListMgr->find(selnode->m_id) >= 0)	// ãëî€ÇµÇƒÇ¢ÇÈ
				{
					strcat(str, "Permit this player\n");
				}
				else
				{
					strcat(str, "Deny this player\n");
				}

				strcat(str, "!Send message (Not support)\n");
				strcat(str, "-\n");
				strcat(str, "!Config (Not support)");
				int halfwidth = g_d3dfont->getTextWidth(str) / 2;
				drawGGXXWindow(str, g_vsnet.m_menu_cursor, 320 - halfwidth, 180, 330 + halfwidth, 247);
			}

			if (g_netMgr->m_watch)
			{
				// ÉwÉbÉ_ï™ÇÃÉçÅ[Éhë“Çø
				char str[256];
				int  per = g_netMgr->m_watchRecvSize * 100 / REPLAY_HEADER_SIZE;
				if (per > 100) per = 100;
				sprintf(str, "Buffering... %3d %%", per);
				drawGGXXWindow(str, -1, 240, 230, 380, 250);
			}
		}
		else drawGGXXWindow("", -1, 20, 380, 480, 460);

		LEAVECS(&g_netMgr->m_csNode);
	}
	else if (*GGXX_MODE1 & 0x400000 && g_replay.m_playing == false && g_netMgr->m_lobbyFrame >= 5)
	{
		D3DV_GGN	d3dv[4];
		
		g_d3dfont->setFont(g_iniFileInfo.m_fontName, g_iniFileInfo.m_fontSize, g_iniFileInfo.m_fontAntialias, FONT_W * 2, 0);
		g_d3dfont->drawText("Replay File/Directory Name", 20, 150, 0xffffff00);
		//g_d3dfont->drawText("Version", 580, 150, 0xffffff00, CD3DFont::Align_Center);

		g_d3dfont->drawText("Item Count", 485, 440, 0xffffff00);
		
		drawGGXXWindow("", -1, 15, 103, 400, 140);
		g_d3dfont->drawText("Current Directory", 20, 108, 0xffffff00);

		g_d3dfont->setFont(g_iniFileInfo.m_fontName, g_iniFileInfo.m_fontSize, g_iniFileInfo.m_fontAntialias, FONT_W, 0);

		sprintf(str, "./%s/", g_replay.m_curdir);
		g_d3dfont->drawText(str, 20, 124, 0xffffffff);

		if (strcmp(g_replay.m_curdir, "REPLAY") == 0)
		{
			sprintf(str, "Åõ - Play\nÅ~ - Exit");
		}
		else
		{
			sprintf(str, "Åõ - Play\nÅ~ - Parent Directory");
		}
		drawGGXXWindow(str, -1, 470, 106, 620, 140);
		
		/* itemcount */
		if (g_replay.m_itemlist.size() > 0)
		{
			sprintf(str, "%4d/%4d", g_replay.m_selectItemIdx + 1, g_replay.m_itemlist.size());
			g_d3dfont->drawText(str, 630, 440, 0xffffffff, CD3DFont::Align_Right);
		}
		else g_d3dfont->drawText("----/----", 630, 440, 0xffffffff, CD3DFont::Align_Right);

		/* ëIëîwåi */
		if (g_replay.m_selectItemIdx != -1 && g_replay.m_selectItemIdx < g_replay.m_itemlist.size())
		{
			int row = (g_replay.m_selectItemIdx - g_replay.m_dispItemHead);
			g_d3dDev->SetTexture(0, NULL);
			d3dv[0].setPos(20.0f, (float)(167 + row * LINESIZE));
			d3dv[0].setColor(0x800000ff);
			d3dv[1].setPos(620.0f, (float)(167 + row * LINESIZE));
			d3dv[1].setColor(0x800000ff);
			d3dv[2].setPos(20.0f, (float)(167 + row * LINESIZE + 12));
			d3dv[2].setColor(0x800000ff);
			d3dv[3].setPos(620.0f, (float)(167 + row * LINESIZE + 12));
			d3dv[3].setColor(0x800000ff);
			g_d3dDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, d3dv, sizeof(D3DV_GGN));
		}

		int line = 0;
		for (int i = 0; i < g_replay.m_itemlist.size(); i++)
		{
			DWORD argb = (1) ? 0xffffffff : 0x80ffffff;

			if (i < g_replay.m_dispItemHead) continue;
			if (i >= g_replay.m_dispItemHead + g_replay.m_itemPerPage) break;
			
			char* fname = (char*)_mbsrchr((BYTE*)g_replay.m_itemlist[i]->fname, '/');
			if (fname == NULL) continue;
			
			strcpy(str, fname + 1);
			g_d3dfont->drawText(str, 36, 167 + line * LINESIZE, argb);
			if (g_replay.m_itemlist[i]->dir) g_dirIcon->draw(20, 165 + line * LINESIZE, argb);
			line++;
		}
	}
	else if (g_netMgr->m_connect || g_replay.m_playing)
	{
		char* name1P = NULL, *name2P = NULL;

		if (*GGXX_MODE1 & 0xc00000)
		{
			name1P = g_replay.m_data.name1P;
			name2P = g_replay.m_data.name2P;
		}
		else if (*GGXX_MODE1 & 0x200000)
		{
			char name[256];
			getNameTrip(name);

			if (g_netMgr->m_playSide == 1)
			{
				name1P = name;
				name2P = g_enemyInfo.m_name;
			}
			else
			{
				name1P = g_enemyInfo.m_name;
				name2P = name;
			}
		}
		if (name1P && name2P)
		{
			drawText(name1P,  21, 69, 0xffffffff, CD3DFont::Align_Left);
			drawText(name2P, 621, 69, 0xffffffff, CD3DFont::Align_Right);
		}
	}

	if (g_setting.showfps)
	{
		sprintf(str, "Ver%s", GGNVERSTR);
		drawText(str, 638, 465, 0xffffffff, CD3DFont::Align_Right);

		static int	 fps = 0;
		static DWORD oldTime = 0;
		static int	 frame = 0;

		frame++;
		DWORD t = timeGetTime() - oldTime;
		if (t >= 1000)
		{
			fps = frame;
			oldTime = timeGetTime();
			frame = 0;
		}
		sprintf(str, "FPS : %d", fps);
		drawText(str, 638, 0, 0xffffffff, CD3DFont::Align_Right);
	}

	// ëçäœêÌé“êîÇï\é¶
	if ((*GGXX_MODE1 & 0xa00000) && *GGXX_MODE2 == 6)
	{
		sprintf(str, "Gallery : %d", g_netMgr->m_totalGalleryCount);
		drawText(str, 2, 465, 0xffffffff, CD3DFont::Align_Left);
	}

#ifdef _DEBUG
	drawText("Debug", 2, 0, 0xffffffff, CD3DFont::Align_Left);

	if (*GGXX_MODE1 & 0xc00000)
	{
		sprintf(str, "rep frame = %d", g_replay.m_frameCount);
		drawText(str, 320, 465, 0xffffffff, CD3DFont::Align_Center);
	}

	if (*GGXX_BTLINFO)
	{
		sprintf(str, "1P pos(%8d, %8d) 2P pos(%8d, %8d)\n1P act:%08x(%3d) 2P act:%08x(%3d)\n1P faint:%4d 2P faint:%4d\n\n1P dflg:%08x 2P dflg:%08x",
			*GGXX_1PPOSX, *GGXX_1PPOSY,
			*GGXX_2PPOSX, *GGXX_2PPOSY,
			*GGXX_1PACT,
			*GGXX_1PFRAME,
			*GGXX_2PACT,
			*GGXX_2PFRAME,
			*GGXX_1PFAINT, *GGXX_2PFAINT,
			*GGXX_1PDOWNFLG, *GGXX_2PDOWNFLG);
		g_d3dfont->drawText(str, 10, 300, 0xffffffff);

		if (*GGXX_1PACT == 0x48 || *GGXX_1PACT == 0x32) DBGOUT("1P:%x-%d\n", *GGXX_1PACT, *GGXX_1PFRAME);
		if (*GGXX_2PACT == 0x48 || *GGXX_2PACT == 0x32) DBGOUT("2P:%x-%d\n", *GGXX_2PACT, *GGXX_2PFRAME);
	}

	if (*GGXX_MODE1 & 0x200000 || *GGXX_MODE1 & 0x800000)
	{
		D3DV_GGN	d3dv[4];
		
		d3dv[0].setColor(0x80000000);
		d3dv[1].setColor(0x80000000);
		d3dv[2].setColor(0x80000000);
		d3dv[3].setColor(0x80000000);

		// datÇÃèÓïÒ
		d3dv[0].setPos(10.0f,  30.0f);
		d3dv[1].setPos(110.0f, 30.0f);
		d3dv[2].setPos(10.0f,  85.0f);
		d3dv[3].setPos(110.0f, 85.0f);
		g_d3dDev->SetTexture(0, NULL);
		g_d3dDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, d3dv, sizeof(D3DV_GGN));
		
		sprintf(str, "IgnoreWatchIn = %d\nWatchNode = %d\nIntrusion = %d\nScore = %d\n",
			g_ignoreWatchIn,
			g_setting.watchBroadcast ? g_setting.watchMaxNodes : -16,
			g_setting.watchIntrusion,
			g_setting.score);
		g_d3dfont->drawText(str, 10, 30, 0xffffff00);

		// äœêÌÉäÉåÅ[èÓïÒ
		d3dv[0].setPos(320.0f, 10.0f);
		d3dv[1].setPos(630.0f, 10.0f);
		d3dv[2].setPos(320.0f, 100.0f);
		d3dv[3].setPos(630.0f, 100.0f);
		g_d3dDev->SetTexture(0, NULL);
		g_d3dDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, d3dv, sizeof(D3DV_GGN));

		g_d3dfont->setFont(g_iniFileInfo.m_fontName, g_iniFileInfo.m_fontSize, g_iniFileInfo.m_fontAntialias, FONT_W, 1);

		sprintf(str, "buffering size = %d bytes", g_netMgr->m_watchRecvSize);
		g_d3dfont->drawText(str, 330, 10 + LINESIZE*0, 0xffffffff, CD3DFont::Align_Left);

		char addrstr[32];

		sprintf(str, "root1[%d]%s %s", g_netMgr->m_watchRootGameCount[0],
			g_netMgr->m_watchRootName[0], g_netMgr->getStringFromAddr(&g_netMgr->m_watchRootAddr[0], addrstr));
		g_d3dfont->drawText(str, 330, 10 + LINESIZE*1, 0xffffffff, CD3DFont::Align_Left);

		sprintf(str, "root2[%d]%s %s", g_netMgr->m_watchRootGameCount[1],
			g_netMgr->m_watchRootName[1], g_netMgr->getStringFromAddr(&g_netMgr->m_watchRootAddr[1], addrstr));
		g_d3dfont->drawText(str, 330, 10 + LINESIZE*2, 0xffffffff, CD3DFont::Align_Left);
		
		for (int i = 0; i < WATCH_MAX_CHILD; i++)
		{
			DWORD color = 0xffffffff;
			if (i >= g_setting.watchMaxNodes + (g_netMgr->m_watch ? WATCH_MAX_CHILD_INC : 0)) color = 0xff808080;

			char addrstr[32];
			sprintf(str, "child[%d](%d) = %s (%d bytes)", i,
				g_netMgr->m_watcher[i].m_childCount,
				g_netMgr->getStringFromAddr(&g_netMgr->m_watcher[i].m_remoteAddr, addrstr),
				g_netMgr->m_watcher[i].m_sendSize);
			g_d3dfont->drawText(str, 330, 10 + LINESIZE * (i+4), color, CD3DFont::Align_Left);
		}
	}
#endif
}

void enterServer(bool p_busy)
{
	char *server, *script;
	getscpiptaddr(server, script);
	
	char nametrip[256];
	getNameTrip(nametrip);

	char buf[1024];
	std::string radminAddress = getRadminAddress();
	sprintf(buf, "cmd=enter|port=%d|name=%s|param=%d%02x%d%d%d%d%d%d%c%d|win=%d|radminAddress=%s",
		g_setting.port, nametrip, p_busy, LOBBY_VER,
		g_setting.useEx, 0, 0, 0, 0, 0,
		getRankChar(g_setting.rank), g_setting.delay, g_setting.wins, radminAddress);
	internet_post(buf, strlen(buf), 1024, server, script);

	/*
	è≠êîÇÃä€ÇﬂåÎç∑Ç…ÇÊÇÈìØä˙ÉYÉåÇ…ëŒèàÇ∑ÇÈÇΩÇﬂCWÇÉZÉbÉg
	î≠ê∂èåèÇ™internet_postì‡ÇÃHttpSendRequestÇåƒÇÒÇæèÍçáÇæÇ™
	ïKÇ∏î≠ê∂Ç∑ÇÈÇÌÇØÇ≈Ç‡Ç»Ç≠Ç±Ç±ÇæÇØÇ≈ÇÕïsà¿
	îOÇÃÇΩÇﬂggn_input()Ç≈Ç‡çsÇ¢ÅAñàÉtÉåÅ[ÉÄåƒÇŒÇÍÇÈÇÊÇ§Ç…ÇµÇƒÇ®Ç≠
	*/
	SETFCW(DEFAULT_CW);

	/* ÉwÉbÉ_Å[Å`ÉtÉbÉ^Å[ÇÃãÊä‘ÇíäèoÅiñ≥óøéIÇ»Ç«Ç≈ïtâ¡Ç≥ÇÍÇÈâ¬î\ê´Ç™Ç†ÇÈÇΩÇﬂÅj */
	char* ptr = strstr(buf, "##head##");
	if (ptr==NULL) return;
	char* end = strstr(buf, "##foot##");
	if (end==NULL) return;
	ptr += 8;
	*end = '\0';
	
	g_nodeMgr->setOwnNode(ptr);

	DBGOUT_NET("enterServer end\n");
}

void readNodeList(void)
{
	FILE* fp = fopen(g_setting.scriptAddress, "r");
	if (fp == NULL) return;
	
	while (1)
	{
		char buf[1024];
		if (fgets(buf, 1024, fp) == NULL) break;
		{
			/*
			äeçsaddr:portÇÃå`éÆ
			ÉXÉyÅ[ÉXÅAÉ^ÉuÇéÊÇËèúÇ´ÅAâ¸çsÇ≈é~ÇﬂÇÈÅA;à»ç~ÇÉRÉÅÉìÉgàµÇ¢
			*/
			char temp[1024];
			int cnt = 0;
			
			for (int i = 0; i < 1024 && buf[i] != '\0'; i++)
			{
				switch (buf[i])
				{
				case ' '  : break;
				case '\t' : break;
				case ';'  : temp[cnt++] = '\0';   break;
				case '\n' : temp[cnt++] = '\0';   break;
				case '\r' : temp[cnt++] = '\0';   break;
				default   : temp[cnt++] = buf[i]; break;
				}
				if (temp[cnt - 1] == '\0') break;
			}
			temp[cnt] = '\0';

			if (temp[0] != '\0') g_nodeMgr->addNode(temp, UNKNOWN_NAME, false, false);
		}
	}

	fclose(fp);
}

bool useLobbyServer(void)
{
	int len = strlen(g_setting.scriptAddress);
	
	return !(strncmp(&g_setting.scriptAddress[len-4], ".txt", 4) == 0);
}

void readServer(void)
{
	char *server, *script;
	getscpiptaddr(server, script);

	const int bufsize = 1024*1024;
	char* buf = new char[bufsize]; // 1M
	sprintf(buf, "cmd=read|");
	int readsize = internet_post(buf, strlen(buf), bufsize, server, script);

	/*
	è≠êîÇÃä€ÇﬂåÎç∑Ç…ÇÊÇÈìØä˙ÉYÉåÇ…ëŒèàÇ∑ÇÈÇΩÇﬂCWÇÉZÉbÉg
	î≠ê∂èåèÇ™internet_postì‡ÇÃHttpSendRequestÇåƒÇÒÇæèÍçáÇæÇ™
	ïKÇ∏î≠ê∂Ç∑ÇÈÇÌÇØÇ≈Ç‡Ç»Ç≠Ç±Ç±ÇæÇØÇ≈ÇÕïsà¿
	îOÇÃÇΩÇﬂggn_input()Ç≈Ç‡çsÇ¢ÅAñàÉtÉåÅ[ÉÄåƒÇŒÇÍÇÈÇÊÇ§Ç…ÇµÇƒÇ®Ç≠
	*/
	SETFCW(DEFAULT_CW);

	/* ÉwÉbÉ_Å[Å`ÉtÉbÉ^Å[ÇÃãÊä‘ÇíäèoÅiñ≥óøéIÇ»Ç«Ç≈ïtâ¡Ç≥ÇÍÇÈâ¬î\ê´Ç™Ç†ÇÈÇΩÇﬂÅj */
	char* ptr = strstr(buf, "##head##");
	if (ptr==NULL) { delete buf; return; }
	char* end = strstr(buf, "##foot##");
	if (end==NULL) { delete buf; return; }
	
	ptr = __mbschr(ptr, '\n');
	if (ptr) ptr += 1;
	else { delete buf; return; }
	*end = '\0';

	/* â¸çsÇéÊÇËèúÇ´ÉmÅ[Éhí«â¡ */
	int pos = 0;
	while (pos < readsize)
	{
		if (ptr[pos] == '\0') break;

		char *ret = __mbschr(&ptr[pos], '\n');
		if (ret) *ret = '\0';

		/* name@addr:port%paramÇÃå`éÆ */
		char* name = &ptr[pos];
		char* dlm1 = __mbschr(&ptr[pos], '@');
		char* dlm2 = __mbschr(&ptr[pos], '%');
		char* dlm3 = __mbschr(&ptr[pos], '#');
		char* addr = dlm1+1;
		char* prm  = dlm2+1;
		char* win  = dlm3+1;

		/* ÉçÉrÅ[ÉpÉâÉÅÅ[É^Ç…å›ä∑ê´Ç™ñ≥Ç¢ÉoÅ[ÉWÉáÉìÇñ≥éã */
		BYTE ver = CHR_HEX2INT(prm[1]) * 16 + CHR_HEX2INT(prm[2]);
		if (ver == LOBBY_VER)
		{
			if (dlm1 && dlm2 && dlm3)
			{
				*dlm1 = '\0';
				*dlm2 = '\0';
				*dlm3 = '\0';
				
				ENTERCS(&g_netMgr->m_csNode);

				g_nodeMgr->addNode(addr, name, prm[0] == 1, false);

				LEAVECS(&g_netMgr->m_csNode);
			}
		}
		pos = ret - ptr + 1;
	}

	delete buf;

	DBGOUT_NET("readServer end\n");
}

void leaveServer(void)
{
	char *server, *script;
	getscpiptaddr(server, script);

	char nametrip[30];
	char buf[256];
	getNameTrip(nametrip);
	sprintf(buf, "cmd=leave|port=%d|name=%s", g_setting.port, nametrip);
	internet_post(buf, strlen(buf), 256, server, script);

	/*
	è≠êîÇÃä€ÇﬂåÎç∑Ç…ÇÊÇÈìØä˙ÉYÉåÇ…ëŒèàÇ∑ÇÈÇΩÇﬂCWÇÉZÉbÉg
	î≠ê∂èåèÇ™internet_postì‡ÇÃHttpSendRequestÇåƒÇÒÇæèÍçáÇæÇ™
	ïKÇ∏î≠ê∂Ç∑ÇÈÇÌÇØÇ≈Ç‡Ç»Ç≠Ç±Ç±ÇæÇØÇ≈ÇÕïsà¿
	îOÇÃÇΩÇﬂggn_input()Ç≈Ç‡çsÇ¢ÅAñàÉtÉåÅ[ÉÄåƒÇŒÇÍÇÈÇÊÇ§Ç…ÇµÇƒÇ®Ç≠
	*/
	SETFCW(DEFAULT_CW);

	DBGOUT_NET("leaveServer end\n");
}

void getscpiptaddr(char* &p_server, char* &p_script)
{
	static char tmp[256];

	strcpy(tmp, g_setting.scriptAddress);
	
	p_server = tmp;
	p_script = NULL;
	char* p = strchr(tmp, '/');
	if (p)
	{
		*p = '\0';
		p_script = p + 1;
	}
}

void replaceUserPalette(int p_chara, int p_pal, char* p_data)
{
	if (g_myPalette[p_chara][p_pal])
	{
		delete g_myPalette[p_chara][p_pal];
		g_myPalette[p_chara][p_pal] = NULL;
	}
	DWORD* palette = new DWORD[PALLEN];

	memcpy(palette, p_data, 1024);

	palette[4] &= 0x00FFFFFF; /* î≤Ç´êF */
	for (int k = 1; k < 256; k++)
	{
		palette[4 + k] |= 0x80000000; /* óLå¯êF */
	}
	/* ÉXÉèÉbÉv */
	DWORD temp[8];
	for (int k = 0; k < 8; k++)
	{
		memcpy(temp, &palette[4 + 8+32*k], 32);
		memcpy(&palette[4 + 8+32*k], &palette[4 + 16+32*k], 32);
		memcpy(&palette[4 + 16+32*k], temp, 32);
	}
	g_myPalette[p_chara][p_pal] = palette;
}

void readUserPalette(void)
{
	for (int i = 0; i < CHARACOUNT; i++)
	{
		for (int j = 0; j < PALCOUNT; j++)
		{
			char fname[1024];

			sprintf(fname, "pal\\%s_%s.pal", g_charaNames[i], g_paletteNames[j]);

			FILE *fp = fopen(fname, "rb");
			if (fp)
			{
				DWORD* palette = new DWORD[PALLEN];

				zfread((char*)palette, PALLEN * 4, fp);

				palette[4] &= 0x00FFFFFF; /* î≤Ç´êF */
				for (int k = 1; k < 256; k++)
				{
					palette[4 + k] |= 0x80000000; /* óLå¯êF */
				}
				/* ÉXÉèÉbÉv */
				DWORD temp[8];
				for (int k = 0; k < 8; k++)
				{
					memcpy(temp, &palette[4 + 8+32*k], 32);
					memcpy(&palette[4 + 8+32*k], &palette[4 + 16+32*k], 32);
					memcpy(&palette[4 + 16+32*k], temp, 32);
				}
				g_myPalette[i][j] = palette;

				fclose(fp);
			}
		}
	}
}

void deleteUserPalette(void)
{
	for (int i = 0; i < CHARACOUNT; i++)
	{
		for (int j = 0; j < PALCOUNT; j++)
		{
			if (g_myPalette[i][j])
			{
				delete g_myPalette[i][j];
				g_myPalette[i][j] = NULL;
			}
		}
	}
}

void initUserPalette(void)
{
	memset(g_myPalette, 0, 4 * CHARACOUNT * PALCOUNT);
}

void initSetting(void)
{
	/* ÉfÉtÉHÉãÉg */
	__strncpy(g_setting.scriptAddress, "localhost/ggnlobby.cgi", 255);
	memset(g_setting.addressList, 0, 256 * 10);

	g_setting.ver = DATVERSION;
	__strncpy(g_setting.userName, "NONAME", 20);
	__strncpy(g_setting.trip, "", 10);
	g_setting.enableNet = 1;
	g_setting.port		= DEFAULT_PORT;
	g_setting.delay		= DEFAULT_DELAY;

	g_setting.ignoreMisNode = 0;
	g_setting.ignoreSlow	= 1;
	g_setting.wait			= 5;

	g_setting.useEx			= 0;

	g_setting.dispInvCombo	= 1;
	g_setting.showfps		= 0;
	g_setting.wins			= 0;
	g_setting.rank			= Rank_F;
	g_setting.score			= 0;
	g_setting.totalBattle	= 0;
	g_setting.totalWin		= 0;
	g_setting.totalLose		= 0;
	g_setting.totalDraw		= 0;
	g_setting.totalError	= 0;
	g_setting.slowRate		= 0;
	g_setting.rounds		= 3;

	memset(g_setting.msg, 0, 256);

	g_setting.watchBroadcast	= 1;
	g_setting.watchIntrusion	= 0;
	g_setting.watchSaveReplay	= 0;
	g_setting.watchMaxNodes		= 1;

	g_scriptCode = 0;
}

char* getIniFilePath(void)
{
	static char result[1024] = "";
	if (result[0] == 0)
	{
		char tmp[MAX_PATH];
		::GetModuleFileName(NULL, tmp, MAX_PATH);
		*strrchr(tmp, '\\') = '\0';
		sprintf(result, "%s/ggxxnet.ini", tmp);
	}
	return result;
}

void readIniFile(void)
{
	char buf[1024];
	
	::GetPrivateProfileString("Font", "FontName", "", buf, 1024, getIniFilePath());
	__strncpy(g_iniFileInfo.m_fontName, buf, 255);
	
	g_iniFileInfo.m_fontSize			= ::GetPrivateProfileInt("Font", "FontSize", 12, getIniFilePath());
	if (g_iniFileInfo.m_fontSize <  5) g_iniFileInfo.m_fontSize = 5;
	if (g_iniFileInfo.m_fontSize > 40) g_iniFileInfo.m_fontSize = 40;

	g_iniFileInfo.m_fontAntialias		= (::GetPrivateProfileInt("Font", "Antialias", 0, getIniFilePath()) != 0);

	g_iniFileInfo.m_posx				= ::GetPrivateProfileInt("Window", "PosX", -999, getIniFilePath());
	g_iniFileInfo.m_posy				= ::GetPrivateProfileInt("Window", "PosY", -999, getIniFilePath());
	::GetPrivateProfileString("Window", "ZoomX", "1.0", buf, 1024, getIniFilePath());
	g_iniFileInfo.m_zoomx				= (float)atof(buf);
	::GetPrivateProfileString("Window", "ZoomY", "1.0", buf, 1024, getIniFilePath());
	g_iniFileInfo.m_zoomy				= (float)atof(buf);

	g_iniFileInfo.m_recvThreadPriority	= ::GetPrivateProfileInt("Network", "ReceiveThreadPriority", 0, getIniFilePath());
	if (g_iniFileInfo.m_recvThreadPriority < -1) g_iniFileInfo.m_recvThreadPriority = -1;
	if (g_iniFileInfo.m_recvThreadPriority >  1) g_iniFileInfo.m_recvThreadPriority = 1;

	g_iniFileInfo.m_recvThreadInterval	= ::GetPrivateProfileInt("Network", "ReceiveThreadInterval", 3, getIniFilePath());
	if (g_iniFileInfo.m_recvThreadInterval <  1) g_iniFileInfo.m_recvThreadInterval = 1;
	if (g_iniFileInfo.m_recvThreadInterval > 16) g_iniFileInfo.m_recvThreadInterval = 16;

	g_iniFileInfo.m_recvThreadMethod	= ::GetPrivateProfileInt("Network", "ReceiveThreadMethod", 0, getIniFilePath());
	if (g_iniFileInfo.m_recvThreadMethod < 0) g_iniFileInfo.m_recvThreadMethod = 0;
	if (g_iniFileInfo.m_recvThreadMethod > 1) g_iniFileInfo.m_recvThreadMethod = 1;

	g_iniFileInfo.m_maxPacketSize		= ::GetPrivateProfileInt("Network", "MaxPacketSize", 256, getIniFilePath());
	if (g_iniFileInfo.m_maxPacketSize < SPacket_WatchData::MINBUFFERSIZE) g_iniFileInfo.m_maxPacketSize = SPacket_WatchData::MINBUFFERSIZE;
	if (g_iniFileInfo.m_maxPacketSize > SPacket_WatchData::MAXBUFFERSIZE) g_iniFileInfo.m_maxPacketSize = SPacket_WatchData::MAXBUFFERSIZE;

	//g_iniFileInfo.m_watchDataFrequency	= ::GetPrivateProfileInt("Network", "WatchDataFrequency", 1, getIniFilePath());
	//if (g_iniFileInfo.m_watchDataFrequency < 0) g_iniFileInfo.m_watchDataFrequency = 0;
	//if (g_iniFileInfo.m_watchDataFrequency > 4) g_iniFileInfo.m_watchDataFrequency = 4;
	//g_iniFileInfo.m_watchDataFrequency = 4;

	g_iniFileInfo.m_ignoreSlowConnections = (::GetPrivateProfileInt("Network", "IgnoreSlowConnections", 1, getIniFilePath()) != 0);
	if (g_iniFileInfo.m_ignoreSlowConnections != 0) g_iniFileInfo.m_ignoreSlowConnections = 1;

#if _DEBUG
	::GetPrivateProfileString("Debug", "DataDir", "", buf, 1024, getIniFilePath());
	strcpy(g_iniFileInfo.m_dataDir, buf);
#endif
}

void writeIniFile(void)
{
	// èÛë‘Çï€ë∂
	if (*GGXX_FULLSCREEN == 0) // windowÉÇÅ[ÉhéûÇÃÇ›
	{
		RECT rect;
		GetClientRect(*GGXX_HWND, &rect);
		g_iniFileInfo.m_zoomx = (float)(rect.right - rect.left) / 640.0f;
		g_iniFileInfo.m_zoomy = (float)(rect.bottom - rect.top) / 480.0f;
	}

	// èëÇ´çûÇ›
	char str[256];
	::WritePrivateProfileString("Font", "FontName", g_iniFileInfo.m_fontName, getIniFilePath());
	_itoa(g_iniFileInfo.m_fontSize, str, 10);
	::WritePrivateProfileString("Font", "FontSize", str, getIniFilePath());

	::WritePrivateProfileString("Font", "Antialias", g_iniFileInfo.m_fontAntialias ? "1" : "0", getIniFilePath());
	
	_itoa(g_iniFileInfo.m_posx, str, 10);
	::WritePrivateProfileString("Window", "PosX", str, getIniFilePath());
	_itoa(g_iniFileInfo.m_posy, str, 10);
	::WritePrivateProfileString("Window", "PosY", str, getIniFilePath());

	sprintf(str, "%2.2f", g_iniFileInfo.m_zoomx);
	::WritePrivateProfileString("Window", "ZoomX", str, getIniFilePath());
	sprintf(str, "%2.2f", g_iniFileInfo.m_zoomy);
	::WritePrivateProfileString("Window", "ZoomY", str, getIniFilePath());

	_itoa(g_iniFileInfo.m_recvThreadPriority, str, 10);
	::WritePrivateProfileString("Network", "ReceiveThreadPriority", str, getIniFilePath());
	_itoa(g_iniFileInfo.m_recvThreadInterval, str, 10);
	::WritePrivateProfileString("Network", "ReceiveThreadInterval", str, getIniFilePath());
	_itoa(g_iniFileInfo.m_recvThreadMethod, str, 10);
	::WritePrivateProfileString("Network", "ReceiveThreadMethod", str, getIniFilePath());

	_itoa(g_iniFileInfo.m_maxPacketSize, str, 10);
	::WritePrivateProfileString("Network", "MaxPacketSize", str, getIniFilePath());

	//_itoa(g_iniFileInfo.m_watchDataFrequency, str, 10);
	//::WritePrivateProfileString("Network", "WatchDataFrequency", str, getIniFilePath());

	::WritePrivateProfileString("Network", "IgnoreSlowConnections", g_iniFileInfo.m_ignoreSlowConnections ? "1" : "0", getIniFilePath());

#if _DEBUG
	::WritePrivateProfileString("Debug", "DataDir", g_iniFileInfo.m_dataDir, getIniFilePath());
#endif
}

void getNameTrip(char* p_str)
{
	// p_strÇ…ÇÕ30ÉoÉCÉgà»è„ÇÃï∂éöóÒÇ™ämï€Ç≥ÇÍÇƒÇ¢ÇÈÇ±Ç∆!!
	if (strlen(g_setting.trip) > 0)
	{
		static char trip[5] = "";
		
		if (trip[0] == '\0')
		{
			/* åªç›ÇÃédólÇ≈ÇÕãNìÆíÜÇ…é©ï™ÇÃTripÉLÅ[Ç™ïœçXÇ≥ÇÍÇÈÇ±Ç∆ÇÕñ≥Ç¢ */
			char md5[33];
			getMD5((BYTE*)g_setting.trip, strlen(g_setting.trip), (BYTE*)md5);
			
			char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
			for (int i = 0; i < 4; i++) trip[i] = table[(md5[i] + md5[8+i] + md5[16+i] + md5[24+i]) % strlen(table)];
			trip[4] = '\0';
		}
		// g_setting.userNameÇÕ20ÉoÉCÉgÇ‹Ç≈ÇµÇ©Ç¢ÇÍÇÁÇÍÇ»Ç¢
		sprintf(p_str, "%sÅü%s", g_setting.userName, trip);
	}
	else __strncpy(p_str, g_setting.userName, 20);
}

void onDisconnect(char* p_cause)
{
#if DEBUG_INQUIRY_MODE
	// ÉVÉìÉNÉGÉâÅ[éûÇ…ä÷òAÇ∑ÇÈÉçÉOÇèoóÕ
	if (strcmp(p_cause, "SYNC ERROR!!") == 0)
	{
		WRITE_DEBUGLOG(p_cause);
	}
#endif
	/* ìríÜÇ≈Ç‡replayÇï€ë∂Ç∑ÇÈ */
	saveReplayFile();

	if (strcmp(p_cause, "endbattle") != 0) g_setting.totalError++;
	writeSettingFile();

	/* disconnectÇ≥ÇÍÇΩÇ∆Ç´Ç…åƒÇŒÇÍÇÈ */
	*GGXX_MODE1 = 0x200000;
	*GGXX_MODE2 = 0x37;
	
	//if (useLobbyServer()) enterServer(0);
}

void addScore(char p_mywc, char p_enwc, char p_enRank, WORD p_enwin)
{
	if (p_enRank < Rank_S || p_enRank > Rank_F) return;

	int table[2][RankCount][RankCount] = {
// lose    S    A    B    C    D    E    F (en)
		-250,-300,-400,-500,-600,-600,-600, // S (me)
		-150,-200,-300,-400,-500,-600,-600, // A
		-100,-150,-200,-250,-300,-400,-500, // B
		 -50,-100,-150,-200,-250,-300,-400, // C
		 -10, -50,-100,-150,-200,-250,-300, // D
		 -10, -10, -50,-100,-150,-200,-250, // E
		 -10, -10, -10, -50,-100,-150,-200, // F

// win     S    A    B    C    D    E    F (en)
		 200, 150, 100,  80,  40,   0,   0, // S (me)
		 250, 200, 150, 100,  80,  40,   0, // A
		 300, 350, 200, 150, 100,  80,  40, // B
		 400, 300, 250, 200, 150, 100,  80, // C
		 600, 400, 300, 250, 200, 150, 100, // D
		 800, 600, 400, 300, 250, 200, 150, // E
		 800, 800, 600, 400, 300, 250, 200, // F
	};

	int score = 0;
	/* ÉâÉEÉìÉhÉXÉRÉA */
	for (int i = 0; i < p_mywc; i++) score += table[1][g_setting.rank][p_enRank];
	for (int i = 0; i < p_enwc; i++) score += table[0][g_setting.rank][p_enRank];
	/* ëççáÉXÉRÉA */
	if (p_mywc != p_enwc) score += table[p_mywc > p_enwc][g_setting.rank][p_enRank] * 7;

	// èüÇ¡ÇƒÇÍÇŒå∏ì_Ç»Çµ
	if (p_mywc > p_enwc && score < 0) score = 0;

	/* ëäéËÇÃòAèüçló∂ */
	//int per = (score > 0) ? (100 + p_enwin) : (100 - p_enwin);
	//if (per < 0) per = 0;
	//score = score * per / 100;

	g_setting.score += score;
	DBGOUT_NET("score = %d, total = %d\n", score, g_setting.score);

	/* ÉâÉìÉNîªíË */
	if (g_setting.score >= 16000 && g_setting.rank > Rank_S)
	{
		// rank up
		g_setting.rank--;
		g_setting.score = 0;
		DBGOUT_NET("rank up to %c\n", getRankChar(g_setting.rank));
	}
	
	if (g_setting.score <= -16000 && g_setting.rank < Rank_F)
	{
		// rank down
		g_setting.rank++;
		g_setting.score = 0;
		DBGOUT_NET("rank down to %c\n", getRankChar(g_setting.rank));
	}

	if (g_setting.score >=  16000) g_setting.score =  16000;
	if (g_setting.score <= -16000) g_setting.score = -16000;
}

char getRankChar(int p_rank)
{
	switch(p_rank)
	{
	case Rank_S: return 'S'; break;
	case Rank_A: return 'A'; break;
	case Rank_B: return 'B'; break;
	case Rank_C: return 'C'; break;
	case Rank_D: return 'D'; break;
	case Rank_E: return 'E'; break;
	case Rank_F: return 'F'; break;
	}
	return '?';
}

void saveReplayFile(void)
{
	// äœêÌÉäÉvÉåÉCÇï€ë∂Ç∑ÇÈÅH
	if (g_netMgr->m_watch && g_setting.watchSaveReplay == 0) return;

	// ééçáäJénëOÇÕï€ë∂ÇµÇ»Ç¢ÅBç≈í·Ç≈Ç‡èIí[ÉRÅ[ÉhÇ™ì¸ÇÈÇÃÇ≈m_frameCountÇÕ1à»è„Ç…Ç»ÇÈ
	if (g_replay.m_frameCount > 1)
	{
		DBGOUT_NET("save replay start\n");

		char dirPath[1024];
		sprintf(dirPath, "%s/replay", g_moduleDir);
		CreateDirectory(dirPath, NULL);

		if (g_netMgr->m_watch)
		{
			sprintf(dirPath, "%s/replay/watch", g_moduleDir);
			CreateDirectory(dirPath, NULL);
		}

		char name1P[30], name2P[30];
		strcpy(name1P, g_replay.m_data.name1P);
		strcpy(name2P, g_replay.m_data.name2P);
		int i;
		for (i = 0; name1P[i] != 0 && name1P[i] != 'Åü'; i++);
		name1P[i] = 0;
		for (i = 0; name2P[i] != 0 && name2P[i] != 'Åü'; i++);
		name2P[i] = 0;
		
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		char str[1024];

		for (i = 0; i < 999;i++)
		{
			WIN32_FIND_DATA fd;
			sprintf(str, "%s/%02d%02d%02d-%03d*", dirPath, systime.wYear, systime.wMonth, systime.wDay, i);
			HANDLE hdl = FindFirstFile(str, &fd);
			if (hdl == INVALID_HANDLE_VALUE)
			{
				sprintf(str, "%s/%02d%02d%02d-%03d %s(%c%c) vs %s(%c%c).rep", dirPath,
					systime.wYear, systime.wMonth, systime.wDay, i,
					name1P, g_charaNames[g_replay.m_data.chara1P-1][0], g_charaNames[g_replay.m_data.chara1P-1][1],
					name2P, g_charaNames[g_replay.m_data.chara2P-1][0], g_charaNames[g_replay.m_data.chara2P-1][1]);

				/* ÉtÉ@ÉCÉãñºÇ…égÇ¶Ç»Ç¢ï∂éöïœä∑ */
				int len = (int)strlen(str);
				for (int i = strlen(dirPath) + 1; i < len; i++)
				{
					if (IS_MB_CHAR(str[i])) { i++; continue; }

					switch (str[i])
					{
					case '\\': case '/': case ':': case '?': case '*': case '"': case '<': case '>': case '|':
						str[i] = ' ';
						break;
					}
				}

				FILE *fp = fopen(str, "wb");
				if (fp)
				{
					int size = REPLAY_HEADER_SIZE + sizeof(DWORD) * g_replay.m_frameCount;
					zfwrite((char*)&g_replay.m_data, size, fp, REPLAY_VER);
					fclose(fp);
					DBGOUT_NET("save replay success\n");
				}
				else
				{
					DBGOUT_NET("save replay failed at fopen()\n");
				}
				break;
			}
		}
		g_replay.m_repRecording = false;
		//g_replay.m_frameCount = -1;
	}
	else
	{
		DBGOUT_NET("save replay failed size = 0\n");
	}
}

void getReplayFileList(char* p_dir)
{
	char str[1024];

	WIN32_FIND_DATA fd;
	sprintf(str, "%s/%s/*", g_moduleDir, p_dir);
	HANDLE hdl = FindFirstFile(str, &fd);
	if (hdl == INVALID_HANDLE_VALUE) return;

	do
	{
		if (strcmp(fd.cFileName, ".") == 0 ||
			strcmp(fd.cFileName, "..") == 0)
		{
			continue;
		}
		
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			ReplayItem* ri = new ReplayItem;
			sprintf(ri->fname, "%s/%s", p_dir, fd.cFileName);
			ri->dir = true;
			ri->ver = -1;
			g_replay.m_itemlist.insert(0, ri);
		}
		else
		{
			if (strlen(fd.cFileName) >= 4 && strcmp(&fd.cFileName[strlen(fd.cFileName) - 4], ".rep") == 0)
			{
				ReplayItem* ri = new ReplayItem;
				sprintf(ri->fname, "%s/%s", p_dir, fd.cFileName);
				ri->dir = false;
				ri->ver = -1; /* ÉtÉ@ÉCÉãÉIÅ[ÉvÉìÇ™ïKóvÇ»ÇÃÇ≈ÅAå„Ç≈ï\é¶Ç∑ÇÈÇ‡ÇÃÇæÇØéÊìæÇ∑ÇÈ */
				g_replay.m_itemlist.add(ri);
			}
		}
	} while (FindNextFile(hdl, &fd));

	FindClose(hdl);
}

DWORD WINAPI _recvThreadProc(LPVOID lpParameter)
{
	CNetMgr* netMgr = (CNetMgr*)lpParameter;
	int pingcount = 0;
	int count = 0;

	while (!netMgr->m_quitApp)
	{
		if (g_iniFileInfo.m_recvThreadMethod == 0)
		{
			// 108b, 109-4, 111-2Ç»Ç«
			// äÓñ{ìIÇ…ÇÕÇ±ÇøÇÁÇégópÇµÇƒÇ´ÇΩ
			netMgr->talking();
		}
		else
		{
			// v1.09-1Ç≈àÍéûìIÇ…ì±ì¸ÇµÇƒÇ›ÇΩÇ™ÅAÇ†Ç‹ÇËï]îªÇÕó«Ç≠Ç»Ç¢
			// íËä˙ìIÇ…Sleepì¸ÇÍÇ»Ç¢Ç∆ggxxñ{ëÃÇÃèàóùÇ™ëjäQÇ≥ÇÍÇÈÇÃÇæÇÎÇ§Ç©ÅH
			// ÉXÉåÉbÉhóDêÊìxÇÃä÷åWÇ≈Ç±ÇÍÇ‡égÇ¶ÇÈÇ©Ç‡ÇµÇÍÇ»Ç¢ÇÃÇ≈iniÉtÉ@ÉCÉãÇ≈égópâ¬î\Ç…ÇµÇƒÇ®Ç≠
			while (netMgr->talking());
		}

		if (count == 0 && !g_netMgr->m_connect && !g_netMgr->m_watch)
		{
#if !TESTER
			if (*GGXX_MODE1 & 0x200000)
#endif
			{
				/* ñàÉãÅ[ÉvÇPÇ¬pingëóÇËsendpingtimeÇ™àÍíËéûä‘åoâﬂÇµÇƒÇ¢ÇÈÇ‡ÇÃÇ…ÇÕNoResponseÇ∆Ç∑ÇÈ */
				ENTERCS(&g_netMgr->m_csNode);
				while (1)
				{
					if (g_nodeMgr->getNodeCount() <= 0) break;
					
					pingcount = (pingcount + 1) % g_nodeMgr->getNodeCount();
					
					CNode* node = g_nodeMgr->getNode(pingcount);

					/* nodelistégópÇ≈ñºëOÇ™éÊìæÇ≥ÇÍÇƒÇ¢Ç»ÇØÇÍÇŒÅAñºëOóvãÅ */
					if (node->m_state != State_VersionError && strcmp(node->m_name, UNKNOWN_NAME) == 0)
					{
						sockaddr_in addr = g_netMgr->getAddrFromString(node->m_addr);
						g_netMgr->send_nameRequest(&addr);
					}

					/* àÍíËéûä‘galleryCountÇ™çXêVÇ≥ÇÍÇƒÇ¢Ç»Ç¢ÉmÅ[ÉhÇÕgalleryCountÇÉNÉäÉA */
					if (node->m_galleryUpdateTime + 3000 < timeGetTime())
					{
						node->m_galleryCount = 0;
						node->m_galleryUpdateTime = timeGetTime();
					}

					if (node->m_state == State_VersionError ||
						//node->m_state == State_Mismatch || // pingë≈ÇΩÇ»Ç¢Ç∆GalleryÇÃèÓïÒÇéÊìæÇ≈Ç´Ç»Ç¢
						node->m_state == State_Busy ||
						node->m_state == State_Busy_Casting)
					{
						if (0 == pingcount) break;	/* àÍé¸ÇµÇΩÅBï Ç…åµñßÇ…ÇPé¸ÇµÇ»Ç≠ÇƒÇ‡Ç¢Ç¢ */

						/* Ç±ÇÍÇÁÇÃÉmÅ[ÉhÇ…ëŒÇµÇƒÇÕÇ±ÇøÇÁÇ©ÇÁpingë≈ÇΩÇ»Ç¢ */
						/* ëäéËë§Ç©ÇÁpingÇ™óàÇÈÇÊÇ§Ç≈Ç†ÇÍÇŒÇªÇÍÇéÛÇØstateÇçXêVÇ∑ÇÈ */
						continue;
					}

					if (node->m_sendpingtime == -1)
					{
						/* pingReplyë“ÇøÇ≈ÇÕÇ»Ç¢ */
						if (node->m_state == State_NoResponse)
						{
							for (int i = 0; i < node->m_tmpaddr.size(); i++)
							{
								sockaddr_in addr = g_netMgr->getAddrFromString(node->m_tmpaddr[i]);
								g_netMgr->send_ping(&addr, pingcount);
							}
						}
						else
						{
							sockaddr_in addr = g_netMgr->getAddrFromString(node->m_addr);
							g_netMgr->send_ping(&addr, pingcount);

							/* pingÇ∆ã§Ç…é©ï™ÇÃéùÇ¬Idle, NotReadyÇ»ÉmÅ[ÉhÇÃÉAÉhÉåÉXÇì]ëóÇ∑ÇÈ */
							/* Ç≥ÇÁÇ…ì]ëóÇµÇΩIdle, NotReadyÇ»ÉmÅ[ÉhÇ…pingÇëóÇ¡ÇΩÉmÅ[ÉhÇÃÉAÉhÉåÉXÇëóêMÇ∑ÇÈ */
							bool send = false;
							for (int i = node->m_uhpNodeIdx; i < g_nodeMgr->getNodeCount(); i++)
							{
								if (g_nodeMgr->getNode(i) == node) continue;

								if (g_nodeMgr->getNode(i)->m_state == State_Idle ||
									g_nodeMgr->getNode(i)->m_state == State_NotReady)
								{
									g_netMgr->send_nodeaddr115_3(&addr, g_nodeMgr->getNode(i));
									
									sockaddr_in addr2 = g_netMgr->getAddrFromString(g_nodeMgr->getNode(i)->m_addr);
									g_netMgr->send_nodeaddr115_3(&addr2, node);
									send = true;
								}
								/* éüÇÃÉmÅ[ÉhÇ÷ */
								node->m_uhpNodeIdx++;
								if (node->m_uhpNodeIdx >= g_nodeMgr->getNodeCount()) node->m_uhpNodeIdx = 0;

								if (send) break;
							}
						}
					}
					else
					{
						/* pingReplyë“ÇøÇ»ÇÃÇ≈pingëóÇÁÇ»Ç¢ */
						if (timeGetTime() - node->m_sendpingtime > TIMEOUT_PING)
						{
							/* ÉpÉPÉbÉgÉçÉXÉgÇÃâ¬î\ê´Ç‡çlÇ¶ÅAÉtÉâÉOÇâ∫ÇÎÇ∑ÇæÇØÇ…ÇµÇƒÇ®Ç≠ */
							//node->clearPing();
							//node->m_state = State_NoResponse;
							node->m_sendpingtime = -1;
						}
					}
					break;
				}
				LEAVECS(&g_netMgr->m_csNode);
			}
		}
		// ñÒ20msä‘äuÇ≈pingëóÇÈ
		// êlÇ™ëΩÇ¢Ç∆ÇPêlÇ…ëóÇÈä‘äuÇÕêLÇ—ÇÈ
		count = (count+1) % (21 / g_iniFileInfo.m_recvThreadInterval);

		Sleep(g_iniFileInfo.m_recvThreadInterval);
	}
	netMgr->m_recvThread_end = true;
	DBGOUT_LOG("recv thread end.\n");

	return 0;
}

DWORD WINAPI _lobbyThreadProc(LPVOID lpParameter)
{
	CNetMgr* netMgr = (CNetMgr*)lpParameter;
	int time = timeGetTime();

	while (!netMgr->m_quitApp)
	{
		/* NetVSÇ≈ëŒêÌÇµÇƒÇ¢Ç»Ç¢èÍçá */
#if !TESTER
		if ((*GGXX_MODE1 & 0x200000) != 0 && !g_netMgr->m_connect && !g_netMgr->m_watch)
#else
		if (!g_netMgr->m_connect && !g_netMgr->m_watch)
#endif
		{
			/* ï˙Ç¡ÇƒÇ®Ç≠Ç∆êÿÇÍÇƒÇµÇ‹Ç§ÇÃÇ≈íËä˙ìIÇ…çƒÉGÉìÉgÉä */
			if (timeGetTime() - time > ENTERLOBBY_INTERVAL)
			{
				enterServer(0);

				/* êVãKéQì¸ÇÕä˘Ç…Ç¢ÇÈÉmÅ[ÉhÇ…pingëóÇ¡ÇƒÇÈÇÕÇ∏Ç»ÇÃÇ≈çƒéÊìæÇÕïsóv */
				//readServer();
				//ENTERCS(&g_netMgr->m_csNode);
				//g_nodeMgr->sortNodeList(g_vsnet.m_sortType);
				//LEAVECS(&g_netMgr->m_csNode);

				time = timeGetTime();
			}
		}
		Sleep(50);
	}
	netMgr->m_lobbyThread_end = true;
	DBGOUT_LOG("lobby thread end.\n");

	return 0;
}

BYTE getSyncCheckValue(void)
{
	int value = 0;

	if (g_netMgr && g_netMgr->m_connect)
	{
#if !TESTER
		//if (*GGXX_1PBTLINFO)
		//{
		//	DWORD* xxx = ((DWORD*)(*GGXX_1PBTLINFO + 0xa4));
		//	*xxx = 0;
		//}

		if (*GGXX_MODE1 & 0x200000 && *GGXX_MODE2 == 6 && *GGXX_BTLINFO)
		{
#if DEBUG_INQUIRY_MODE
			// ÇÊÇËëΩÇ≠ÇÃÉQÅ[ÉÄì‡èÓïÒÇÉVÉìÉNÉGÉâÅ[É`ÉFÉbÉNÇ…égópÇ∑ÇÈ
			// Ç±ÇÃà◊ÅAåªçsâ“ìÆî≈(1.20-3)à»ëOÇ∆ÇÕëŒêÌÇ≈Ç´Ç»Ç¢ÇÃÇ≈íçà”ÅI
			value = *GGXX_TIME +
				*GGXX_1PPOSX + 
				*GGXX_2PPOSX + 
				*GGXX_1PPOSY +
				*GGXX_2PPOSY +
				*GGXX_1PACT +
				*GGXX_2PACT +
				*GGXX_1PFRAME +
				*GGXX_2PFRAME +
				*GGXX_1PLIFE + 
				*GGXX_2PLIFE + 
				*GGXX_1PTENSION +
				*GGXX_2PTENSION;
			value = value + (value>>8) + (value>>16) + (value>>24);
#else
			value = *GGXX_TIME +
				*GGXX_1PLIFE + 
				*GGXX_2PLIFE + 
				*GGXX_1PTENSION +
				*GGXX_2PTENSION;
#endif
#ifdef _DEBUG
			/* ìØä˙ÉYÉåÇÃÇΩÇﬂÇÃÉçÉO */
			for (int i = 0; i < 9; i++)
			{
				strcpy(g_syncErrLog[9-i], g_syncErrLog[8-i]);
			}
			//sprintf(g_syncErrLog[0], "nettime=%d, time=%d, frmcnt=%d, rnd=%d\nLife=%d:%d, tg=%d:%d\nfaint=%d:%d\n",
			//	g_netMgr->m_time, *GGXX_TIME, *GGXX_FRAMECOUNTER, GGXX_RANDOMTABLE[*GGXX_RANDOMCOUNTER], *GGXX_1PLIFE, *GGXX_2PLIFE,
			//	*GGXX_1PTENSION, *GGXX_2PTENSION, *GGXX_1PFAINT, *GGXX_2PFAINT);
			sprintf(g_syncErrLog[0], "nettime=%d, time=%d, frmcnt=%d, rnd=%d\nposx=%d:%d, posy=%d:%d\n",
				g_netMgr->m_time, *GGXX_TIME, *GGXX_FRAMECOUNTER, GGXX_RANDOMTABLE[*GGXX_RANDOMCOUNTER], *GGXX_1PPOSX, *GGXX_2PPOSX,
				*GGXX_1PPOSY, *GGXX_2PPOSY);
#endif
		}
		else
		{
			value = 0;
		}
#else
		value = g_netMgr->m_time;
#endif
	}

#if _DEBUG
	// à”ê}ìIÇ…SYNCERRORÇà¯Ç´ãNÇ±Ç∑
	if (GetForegroundWindow() == *GGXX_HWND && (GetAsyncKeyState(VK_F1) & 0x80000000))
	{
		value = rand();
	}
#endif

	return (BYTE)(value % 0xff);
}

void drawGGXXWindow(char* p_str, int p_select, int p_left, int p_top, int p_right, int p_bottom)
{
	g_d3dDev->SetRenderState(D3DRS_ALPHATESTENABLE, true);
	g_d3dDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_d3dDev->SetRenderState(D3DRS_BLENDOP , D3DBLENDOP_ADD);
	g_d3dDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_d3dDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_d3dDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	g_d3dDev->SetTexture(0, NULL);
	g_d3dDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	g_d3dDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	g_d3dDev->SetVertexShader(D3DV_GGN::FVF);

	D3DV_GGN	d3dv[5];
	d3dv[0].setPos((float)p_left, (float)p_top);
	d3dv[0].setColor(0x80000080);
	d3dv[1].setPos((float)p_right, (float)p_top);
	d3dv[1].setColor(0x80000080);
	d3dv[2].setPos((float)p_left, (float)p_bottom);
	d3dv[2].setColor(0x80000080);
	d3dv[3].setPos((float)p_right, (float)p_bottom);
	d3dv[3].setColor(0x80000080);
	g_d3dDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, d3dv, sizeof(D3DV_GGN));
	
	d3dv[0].setPos((float)p_left, (float)p_top);
	d3dv[0].setColor(0xffff0000);
	d3dv[1].setPos((float)p_right, (float)p_top);
	d3dv[1].setColor(0xffff0000);
	d3dv[2].setPos((float)p_right, (float)p_bottom);
	d3dv[2].setColor(0xffff0000);
	d3dv[3].setPos((float)p_left, (float)p_bottom);
	d3dv[3].setColor(0xffff0000);
	d3dv[4].setPos((float)p_left, (float)p_top);
	d3dv[4].setColor(0xffff0000);
	g_d3dDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, d3dv, sizeof(D3DV_GGN));

	if (p_select != -1)
	{
		bool eos = false;
		int y = 4, line = 0;
		char tmp[256], *p;
		strcpy(tmp, p_str);
		p = tmp;
		char* dlm = __mbschr(p, '\r');
		if (!dlm) dlm = __mbschr(p, '\n');
		if (!dlm) { eos = true; dlm = __mbschr(p, '\0'); }
		while (dlm)
		{
			*dlm = '\0';
			if (p[0] == '-')
			{
				g_d3dDev->SetTexture(0, NULL);

				d3dv[0].setPos((float)p_left, (float)p_top + y + 4);
				d3dv[0].setColor(0xffff0000);
				d3dv[1].setPos((float)p_right, (float)p_top + y + 4);
				d3dv[1].setColor(0xffff0000);
				g_d3dDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, d3dv, sizeof(D3DV_GGN));
				y += 8;
			}
			else
			{
				/* ëIëîwåi */
				if (line == p_select)
				{
					g_d3dDev->SetTexture(0, NULL);
					d3dv[0].setPos((float)p_left+1, (float)p_top + y);
					d3dv[0].setColor(0x800000ff);
					d3dv[1].setPos((float)p_right, (float)p_top + y);
					d3dv[1].setColor(0x800000ff);
					d3dv[2].setPos((float)p_left+1, (float)p_top + y + 12);
					d3dv[2].setColor(0x800000ff);
					d3dv[3].setPos((float)p_right, (float)p_top + y + 12);
					d3dv[3].setColor(0x800000ff);
					g_d3dDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, d3dv, sizeof(D3DV_GGN));
				}

				DWORD col = 0xffffffff;
				if (p[0] == '!')
				{
					/* ÉOÉåÉCï\é¶ÅiëIëïsî\Åj */
					col = 0xff808080;
					p++;
				}
				if (*p != '\0') g_d3dfont->drawText(p, p_left + 4, p_top + y, col);
				y += 13;
				line++;
			}
			if (eos) break;
			p = dlm + 1;
			if (*p == '\n') p++;
			dlm = __mbschr(p, '\r');
			if (!dlm) dlm = __mbschr(p, '\n');
			if (!dlm) { eos = true; dlm = __mbschr(p, '\0'); }
		}
	}
	else g_d3dfont->drawText(p_str, p_left + 4, p_top + 4, 0xffffffff);
}

void getMachineID(char* p_id, char* p_key)
{
	char orgid[10];

#if _DEBUG
	/* äJî≠ópÇ…MACÉAÉhÉåÉXÇñºëOÇ…íuÇ´ä∑Ç¶ */
	int sz = (int)strlen(g_setting.userName);
	memcpy(orgid, g_setting.userName, sz > 10 ? 10 : sz);
#else
	getMacAddress(0, orgid);
#endif
	
	BYTE tmp;
	tmp = orgid[0]; orgid[0] = orgid[3]; orgid[3] = tmp;
	tmp = orgid[1]; orgid[1] = orgid[4]; orgid[4] = tmp;
	tmp = orgid[2]; orgid[2] = orgid[5]; orgid[5] = tmp;

	*((DWORD*)&orgid[6]) = getSysDiskSN();
	getRC4(p_id, 10, orgid, 10, p_key, strlen(p_key));
}

bool idcmp(const BYTE* p_id1, const BYTE* p_id2)
{
	return memcmp(p_id1, p_id2, 10) == 0;
}

void getWindowSize(int p_clientw, int p_clienth, int* p_windoww, int* p_windowh)
{
	RECT rect, rect2;
	GetClientRect(*GGXX_HWND, &rect2);
	GetWindowRect(*GGXX_HWND, &rect);
	int marginx = (rect.right - rect.left) - (rect2.right - rect2.left);
	int marginy = (rect.bottom - rect.top) - (rect2.bottom - rect2.top);
	*p_windoww = p_clientw + marginx;
	*p_windowh = p_clienth + marginy;
}

void convertModulePath(char* p_out, char* p_in)
{
	sprintf(p_out, "%s/%s", g_moduleDir, p_in);
}

DWORD getGGXXMODE2(void)
{
	return *GGXX_MODE2;
}

void drawText(char* p_str, int p_x, int p_y, DWORD p_color, CD3DFont::EAlign p_align)
{
	g_d3dfont->setFont(g_iniFileInfo.m_fontName, g_iniFileInfo.m_fontSize, g_iniFileInfo.m_fontAntialias, FONT_W, 0, 1);
	g_d3dfont->drawText(p_str, p_x, p_y, 0xc0000000, p_align);
	g_d3dfont->setFont(g_iniFileInfo.m_fontName, g_iniFileInfo.m_fontSize, g_iniFileInfo.m_fontAntialias, FONT_W, 0, 0);
	g_d3dfont->drawText(p_str, p_x + 1, p_y + 1, p_color, p_align);
}

//-------------------------------log
void DBGOUT_LOG(char* fmt, ...)
{
#if DEBUG_OUTPUT_LOG
	va_list ap;
	va_start(ap, fmt);
	
	char buf[1024];
	vsprintf(buf, fmt, ap);

	OutputDebugString(buf);
	
	char fname[256];
	convertModulePath(fname, "log.txt");
	FILE *fp = fopen(fname, "a");
	if (fp)
	{
		fprintf(fp, buf);
		fclose(fp);
	}

	va_end(ap);
#endif
}

void DBGOUT_NET(char* fmt, ...)
{
#if DEBUG_OUTPUT_NET
	va_list ap;
	va_start(ap, fmt);
	
	char buf[1024];
	vsprintf(buf, fmt, ap);

	char buf2[1024];
	if (g_netMgr)
	{
		sprintf(buf2, "%d(%d) : %s", timeGetTime(), g_netMgr->m_time, buf);
	}
	else
	{
		sprintf(buf2, "%d(xxx) : %s", timeGetTime(), buf);
	}
	OutputDebugString(buf2);
	
	//çƒãNÇµÇƒÇµÇ‹Ç§ÇÃÇ≈Ç±Ç±Ç≈ÇÕÉçÉOÇ»Çµ
	EnterCriticalSection(&g_csLogOut);
	if (strlen(g_netLog) + strlen(buf2) < LOG_SIZE) strcat(g_netLog, buf2);
	LeaveCriticalSection(&g_csLogOut);

	va_end(ap);
#endif
}

void WRITE_DEBUGLOG(char* p_cause)
{
	p_cause;
#if DEBUG_OUTPUT_NET || DEBUG_OUTPUT_KEY || DEBUG_OUTPUT_RND
	FILE *fp;
	char basename[1024];
#if !TESTER
	sprintf(basename, "%s/%08d_%d_%s", g_moduleDir, g_startBattleTime, g_setting.port, p_cause);
#else
	sprintf(basename, "test_%d", g_setting.port);
#endif
	char fname[256];
#endif

#if DEBUG_OUTPUT_NET
	sprintf(fname, "%s_dbg.txt", basename);
	fp = fopen(fname, "w");
	ENTERCS(&g_csLogOut);
	fprintf(fp, g_netLog);
	LEAVECS(&g_csLogOut);
	fclose(fp);
#endif

#if DEBUG_OUTPUT_KEY
	sprintf(fname, "%s_key.txt", basename);
	fp = fopen(fname, "w");
	ENTERCS(&g_csLogOut);
	fprintf(fp, g_keyLog);
	LEAVECS(&g_csLogOut);
	fclose(fp);
#endif

#if DEBUG_OUTPUT_RND
	sprintf(fname, "%s_rnd.txt", basename);
	fp = fopen(fname, "w");
	ENTERCS(&g_csLogOut);
	fprintf(fp, g_rndLog);
	LEAVECS(&g_csLogOut);
	fclose(fp);
#endif
}

void WRITE_REPLAY_RAWDATA(int p_size)
{
//#if _DEBUG
//	FILE *fp;
//	char name[256];
//#if !TESTER
//	sprintf(name, "unz%08d_%d.rep", g_startBattleTime, g_setting.port);
//#else
//	sprintf(name, "unz%d.rep", g_setting.port);
//#endif
//	fp = fopen(name, "wb");
//	fwrite(&g_replay.m_data, p_size, 1, fp);
//	fclose(fp);
//#endif
}
