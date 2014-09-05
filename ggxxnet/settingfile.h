#pragma once

//******************************************************************
// structures
//******************************************************************
#pragma pack(push)
#pragma pack(1)

typedef struct
{
	char	scriptAddress[256];
	char	userName[11];
	char	trip[11];
	char	enableNet;
	WORD	port;
	char	delay;
	short	wait;

	char	useEx;

	char	dispInvCombo;
	char	syncCheck;
	WORD	wins;
	char	rank;
	char	score[20];
}SettingInfo_v100;// ver1.00～1.01

typedef struct
{
	char	scriptAddress[256];
	char	userName[11];
	char	trip[11];
	char	enableNet;
	WORD	port;
	char	delay;

	char	ignoreMisNode;
	short	wait;

	char	useEx;

	char	dispInvCombo;
	char	syncCheck;
	WORD	wins;
	char	rank;
	char	score[20];
}SettingInfo_v102;// ver1.02～1.04

typedef struct
{
	char	scriptAddress[256];
	char	userName[11];
	char	trip[11];
	char	enableNet;
	WORD	port;
	char	delay;

	char	ignoreMisNode;
	char	ignoreSlow;
	short	wait;

	char	useEx;

	char	dispInvCombo;
	char	syncCheck;
	WORD	wins;
	char	rank;
	char	score[20];
	char	reserved[64];
}SettingInfo_v105;// ver1.05

typedef struct
{
	char	scriptAddress[256];
	char	addressList[10][256];

	char	userName[11];
	char	trip[11];
	char	enableNet;
	WORD	port;
	char	delay;

	char	ignoreMisNode;
	char	ignoreSlow;
	short	wait;

	char	useEx;

	char	dispInvCombo;
	char	syncCheck;
	WORD	wins;
	char	rank;
	char	score[20];
	char	reserved[64];
}SettingInfo_v106;// ver1.06～1.09-3

typedef struct
{
	int		ver;
	char	scriptAddress[256];
	char	addressList[10][256];

	char	userName[41];	// 40byte分あるが実際には20byteまで
	char	trip[11];
	char	enableNet;
	WORD	port;
	char	delay;

	char	ignoreMisNode;
	char	ignoreSlow;
	short	wait;

	char	useEx;

	char	dispInvCombo;
	char	showfps;
	WORD	wins;
	char	rank;
	int		score;
	int		totalBattle;
	int		totalWin;
	int		totalLose;
	int		totalDraw;
	int		totalError;
	int		slowRate;	/* 処理落ち率　常時60fpsを上回る場合100%になる */
	char	rounds;
	char	msg[256];
}SettingInfo_v110;	// ver1.10～1.15-3

typedef struct
{
	int		ver;
	char	scriptAddress[256];
	char	addressList[10][256];

	char	userName[41];	// 40byte分あるが実際には20byteまで
	char	trip[11];
	char	enableNet;
	WORD	port;
	char	delay;

	char	ignoreMisNode;
	char	ignoreSlow;
	short	wait;

	char	useEx;

	char	dispInvCombo;
	char	showfps;
	WORD	wins;
	char	rank;
	int		score;
	int		totalBattle;
	int		totalWin;
	int		totalLose;
	int		totalDraw;
	int		totalError;
	int		slowRate;	/* 処理落ち率　常時60fpsを上回る場合100%になる */
	char	rounds;
	char	msg[256];
	char	watchBroadcast;		// 配信する
	char	watchIntrusion;		// 乱入許可
	char	watchSaveReplay;	// 観戦したリプレイを保存するか？
	char	watchMaxNodes;		// 1次配信受付数
}SettingInfo;	// ver1.20～

#pragma pack(pop)

//******************************************************************
// proto types
//******************************************************************
void readSettingFile(void);
void writeSettingFile(void);
