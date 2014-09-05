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
#include "netMgr.h"
#include "settingfile.h"
#include "zlib.h"
#include "util.h"
#include "md5.h"

#include <locale.h>
#include <mbstring.h>

//******************************************************************
// function
//******************************************************************

void readSettingFile(void)
{
	// カレントパスの切り替え前なのでそのままでok
	//char datPath[1024];
	//sprintf(datPath, "%s/"DATNAME, g_moduleDir);
	FILE *fp = fopen(DATNAME, "rb");
	if (fp)
	{
		int datasize = zfsize(fp);
		int ver;
		if (datasize == 310)		ver = 100;	// ver1.00-1.01
		else if (datasize == 311)	ver = 102;	// ver1.02-1.04
		else if (datasize == 376)	ver = 105;	// ver1.05
		else if (datasize == 2936)	ver = 106;	// ver1.06-1.09
		else
		{
			zfread((char*)&ver, 4, fp);			// ver1.10以降
			fseek(fp, 0, SEEK_SET);
		}

		if (ver == 100)
		{
			SettingInfo_v100 si100;
			zfread((char*)&si100, sizeof(si100), fp);
			
			g_setting.ver			= 0;
			memcpy(g_setting.scriptAddress, si100.scriptAddress, 256);
			__strncpy(g_setting.userName, si100.userName, 10);
			__strncpy(g_setting.trip, si100.trip, 10);
			g_setting.enableNet		= si100.enableNet;
			g_setting.port			= si100.port;
			g_setting.delay			= si100.delay;
			g_setting.wait			= si100.wait;
			g_setting.useEx			= si100.useEx;
			g_setting.dispInvCombo	= si100.dispInvCombo;
			g_setting.wins			= si100.wins;
			g_setting.rank			= si100.rank;
		}
		else if (ver == 102)
		{
			SettingInfo_v102 si102;
			zfread((char*)&si102, sizeof(si102), fp);

			g_setting.ver			= 0;
			memcpy(g_setting.scriptAddress, si102.scriptAddress, 256);
			__strncpy(g_setting.userName, si102.userName, 10);
			__strncpy(g_setting.trip, si102.trip, 10);
			g_setting.enableNet		= si102.enableNet;
			g_setting.port			= si102.port;
			g_setting.delay			= si102.delay;
			g_setting.ignoreMisNode	= si102.ignoreMisNode;
			g_setting.wait			= si102.wait;
			g_setting.useEx			= si102.useEx;
			g_setting.dispInvCombo	= si102.dispInvCombo;
			g_setting.wins			= si102.wins;
			g_setting.rank			= si102.rank;
		}
		else if (ver == 105)
		{
			SettingInfo_v105 si105;
			zfread((char*)&si105, sizeof(si105), fp);

			g_setting.ver			= 0;
			memcpy(g_setting.scriptAddress, si105.scriptAddress, 256);
			__strncpy(g_setting.userName, si105.userName, 10);
			__strncpy(g_setting.trip, si105.trip, 10);
			g_setting.enableNet		= si105.enableNet;
			g_setting.port			= si105.port;
			g_setting.delay			= si105.delay;
			g_setting.ignoreMisNode	= si105.ignoreMisNode;
			g_setting.wait			= si105.wait;
			g_setting.useEx			= si105.useEx;
			g_setting.dispInvCombo	= si105.dispInvCombo;
			g_setting.wins			= si105.wins;
			g_setting.rank			= si105.rank;
		}
		else if (ver == 106)
		{
			SettingInfo_v106 si106;
			zfread((char*)&si106, sizeof(si106), fp);

			g_setting.ver			= 0;
			memcpy(g_setting.scriptAddress, si106.scriptAddress, 256);
			memcpy(g_setting.addressList, si106.addressList, 256 * 10);
			__strncpy(g_setting.userName, si106.userName, 10);
			__strncpy(g_setting.trip, si106.trip, 10);
			g_setting.enableNet		= si106.enableNet;
			g_setting.port			= si106.port;
			g_setting.delay			= si106.delay;
			g_setting.ignoreMisNode	= si106.ignoreMisNode;
			g_setting.ignoreSlow	= si106.ignoreSlow;
			g_setting.wait			= si106.wait;
			g_setting.useEx			= si106.useEx;
			g_setting.dispInvCombo	= si106.dispInvCombo;
			g_setting.wins			= si106.wins;
			g_setting.rank			= si106.rank;
		}
		else if (ver == 110)
		{
			SettingInfo_v110 setinf;
			zfread((char*)&setinf, sizeof(setinf), fp);

			g_setting.ver			= setinf.ver;
			memcpy(g_setting.scriptAddress, setinf.scriptAddress, 256);
			memcpy(g_setting.addressList, setinf.addressList, 256 * 10);
			__strncpy(g_setting.userName, setinf.userName, 40);
			g_setting.userName[21] = '\0';	// 40byte分あるが実際には20byteまで
			__strncpy(g_setting.trip, setinf.trip, 10);
			
			g_setting.enableNet		= setinf.enableNet;
			g_setting.port			= setinf.port;
			g_setting.delay			= setinf.delay;
			g_setting.ignoreMisNode	= setinf.ignoreMisNode;
			g_setting.ignoreSlow	= setinf.ignoreSlow;
			g_setting.wait			= setinf.wait;
			g_setting.useEx			= setinf.useEx;
			g_setting.dispInvCombo	= setinf.dispInvCombo;
			g_setting.showfps		= setinf.showfps;
			g_setting.wins			= setinf.wins;
			g_setting.rank			= setinf.rank;
			g_setting.score			= setinf.score;
			g_setting.totalBattle	= setinf.totalBattle;
			g_setting.totalWin		= setinf.totalWin;
			g_setting.totalLose		= setinf.totalLose;
			g_setting.totalDraw		= setinf.totalDraw;
			g_setting.totalError	= setinf.totalError;
			g_setting.slowRate		= setinf.slowRate;
			g_setting.rounds		= setinf.rounds;
			__strncpy(g_setting.msg, setinf.msg, 255);
		}
		else if (ver == DATVERSION)
		{
			zfread((char*)&g_setting, sizeof(SettingInfo), fp);
		}

		// 名無しチェック
		if (g_setting.userName[0] == '\0')
		{
			strcpy(g_setting.userName, "NONAME");
		}
		// 使用不可文字チェック
		unsigned char* invchr = _mbschr((unsigned char*)g_setting.userName, '◆');
		if (invchr)
		{
			invchr[0] = '*';
			invchr[1] = '*';
		}
		invchr = _mbschr((unsigned char*)g_setting.userName, '@');
		if (invchr)
		{
			invchr[0] = '*';
		}

		// MaxRelayNodeのチェック
		if (g_setting.watchMaxNodes > WATCH_MAX_CHILD_BASE) g_setting.watchMaxNodes = WATCH_MAX_CHILD_BASE;
		if (g_setting.watchMaxNodes < 0) g_setting.watchMaxNodes = 0;

		g_setting.wait = 3;

		strlower(g_setting.scriptAddress);
		strtrim(g_setting.scriptAddress);

		/* 照合用にLong値を生成 */
		if (useLobbyServer())
		{
			char md5[33];
			getMD5((BYTE*)g_setting.scriptAddress, strlen(g_setting.scriptAddress), (BYTE*)md5);
			for (int i = 0; i < 4; i++) ((char*)&g_scriptCode)[i] = md5[i] + md5[8+i] + md5[16+i] + md5[24+i];
		}
		else
		{
			// ノードリストは固定値とする
			g_scriptCode = 0xffffffff;
		}
		setlocale(LC_ALL, "");

		fclose(fp);
	}

	// 1.20-2からignoreSlowConnectionsはiniから取得
	g_setting.ignoreSlow = g_iniFileInfo.m_ignoreSlowConnections;
}

void writeSettingFile(void)
{
	char datPath[1024];
	sprintf(datPath, "%s/"DATNAME, g_moduleDir);

	// バックアップ取る
	if (g_setting.ver != DATVERSION)
	{
		char olddatPath[1024];
		sprintf(olddatPath, "%s/"DATNAME"_old", g_moduleDir);

		remove(olddatPath);
		rename(datPath, olddatPath);
	}

	FILE *fp = fopen(datPath, "wb");
	if (fp)
	{
		g_setting.ver = DATVERSION;
		zfwrite((char*)&g_setting, sizeof(SettingInfo), fp, 0);
		fclose(fp);
	}
}
