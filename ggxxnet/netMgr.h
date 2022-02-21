#pragma once

//******************************************************************
// libs
//******************************************************************
#pragma comment(lib, "ws2_32.lib")

//******************************************************************
// includes
//******************************************************************
#include <winsock2.h>
#include <ws2tcpip.h>
#include "res/resource.h"

//******************************************************************
// defines
//******************************************************************
#define USE_TCP				FALSE

/* ggxxnetƒRƒlƒNƒVƒ‡ƒ“IDEˆê’v‚µ‚Ä‚¢‚ê‚ÎVerˆá‚¢‚Å‚à’ÊM‰Â”\ */
/* ver1.11ˆÈ~Aver1.12Œn‚ğ—áŠO‚Æ‚µ‚Ä‚·‚×‚Ä12‚ÅŒÅ’è‚µ‚Ä‚¢‚é */
#define CONNECTION_ID		12

#define	LOBBY_VER			6		/* ƒƒr[ƒo[ƒWƒ‡ƒ“ */
#define REPLAY_VER			3		/* Replayƒo[ƒWƒ‡ƒ“ */
// 3 = 1.10-
// 2 = 1.09
// 1 = 1.06-1.08b
// 0 = 1.00-1.05
#define	PACKETMAX_SIZE		1024
#define LOG_SIZE			(1024*1024)	// 1MB

#define MAX_COMPWATCHDATASIZE	30000	// ˆ³kÏ‚È‚Ì‚Å30kb‚ ‚ê‚Î—]—T

#define WATCH_MAX_CHILD_BASE	3
#define WATCH_MAX_CHILD_INC		2
#define WATCH_MAX_CHILD			(WATCH_MAX_CHILD_BASE + WATCH_MAX_CHILD_INC)

#define MSPF_FLOAT	16.66666667f
#define MSPF_INT	17

/* Šeƒ^ƒCƒ€ƒAƒEƒgŠÔi•bj */
#define TIMEOUT_VSLOAD	5000	/* VS‰æ–Ê‚ªI‚í‚Á‚Ä‚©‚ç‚Ì‘Ò‚¿ŠÔ */
#define TIMEOUT_SUSPEND	5000	/* Ú‘±`CSŠJnACSI—¹`VSŠJn‚Ü‚Å‚ÌSuspendƒ^ƒCƒ€ƒAƒEƒg ˆÙíƒP[ƒX‚È‚Ì‚Å’·‚ß‚Å—Ç‚¢ */
#define TIMEOUT_KEY		1000	/* ƒL[“ü—Í */
#define TIMEOUT_KEY2	3000	/* ƒL[“ü—Í(ƒtƒŠ[ƒY‘Îô) */
#define TIMEOUT_REPLY	3000	/* Suspend, Resume, ConnectAData‚Ì•ÔM */
#define TIMEOUT_PING	500		/* ping‘Ò‚¿ */
#define TIMEOUT_BLOCK	3000	/* ƒf[ƒ^ƒuƒƒbƒN“]‘— */

#define TIMEOUT_WATCHDATA		1000	/* ‚¢‚Â‚Ü‚Å‚àŠÏíƒf[ƒ^‚ª—ˆ‚È‚¢ */
#define TIMEOUT_WATCHDATAREPLY	1000	/* ‚¢‚Â‚Ü‚Å‚àŠÏíƒf[ƒ^•ÔM‚ª—ˆ‚È‚¢ */

#define TIMEOUT_WATCHDATAWAIT	4000	/* ŠÏíƒf[ƒ^‘Ò‚¿‚É‚æ‚é’â~ŠÔŒÀŠE */

#define WATCH_RESUME_INTERVAL	2000	/* ŠÏíÚ‘±‚ªØ‚ê‚½‚ÌÄ—v‹ŠÔŠu */

const sockaddr_in NULL_ADDR = { 0, 0,						// family, port
								0, 0, 0, 0,					// addr
								0, 0, 0, 0, 0, 0, 0, 0 };	// zero

enum EPacketDataType
{
	Packet_Connect = 0,		/* Ú‘±—v‹ */
	Packet_ConnectReply,
	Packet_Ping,
	Packet_PingReply112_3,
	Packet_PingReplyLite120,

	Packet_Key,
	Packet_VSLoadCompleted,
	Packet_Suspend,
	Packet_SuspendReply,
	Packet_Resume,
	Packet_ResumeReply,

	Packet_Data,
	Packet_DataReply,

	Packet_WatchIn,			/* ŠÏí—v‹ */
	Packet_WatchInReply,
	Packet_WatchInRoot,		/* ”zMŒ³‚É‘Î‚·‚é’¼ÚŠÏí—v‹ */
	Packet_WatchInRootReply,
	Packet_WatchData,		/* ŠÏí‚Ìˆ×‚Ì’f•Ğ“I‚ÈƒŠƒvƒf[ƒ^BÄ‘—‚Í‚µ‚È‚¢‚ªƒƒXƒg‚µ‚½ê‡AŸ‰ñ‘—M•ª‚Éãæ‚¹‚³‚ê‚é */
							/* ‚ ‚Ü‚è‚É‚à•ÔM‚ª—ˆ‚È‚¢ê‡‚ÍØ’f‚µ‚½‚à‚Ì‚Æ‚İ‚È‚µAˆÈŒã“]‘—‚µ‚È‚¢ */
	Packet_WatchDataReply,	/* ŠÏíƒf[ƒ^‚Ì•ÔMBÄ‘—‚Í‚µ‚È‚¢ */
	Packet_PingReply,
	Packet_NodeAddr115_2,	/* ˆê•”‚Ìƒo[ƒWƒ‡ƒ“‚Å–¢‚¾‚Ég—p‚³‚ê‚Ä‚¢‚é‚Ì‚ÅÄ—˜—p‹Ö~II */
	Packet_NodeAddr,

	Packet_GalleryCount,		/* ‘ÎíÒ“¯m‚ÅŠÏíÒ”‚ğ‘—M‚·‚é */
	Packet_GalleryCountForLobby,/* ƒƒr[‚É‚¢‚éƒm[ƒh‚ÉŠÏí‚Ìî•ñ‚ğ’ñ‹Ÿ */
	Packet_NameRequest,			/* UNKNOWN_NAME‚É‘Î‚µ‚Ä”­sB–¼‘O‚ğ“¾‚é‚½‚ßpingƒƒbƒZ[ƒW‚ğ—v‹ */
	Packet_DebugInfo,			/* SyncError‚ÌŒ´ˆö’²¸—p */

	Packet_PingReply_BusyCasting,	/* 1.20-2‚©‚çpingrep‚ÌŒy—Ê‰» */
	Packet_PingReply_Busy,			/* 1.20-2‚©‚çpingrep‚ÌŒy—Ê‰» */
	Packet_PingReply_Watch,			/* 1.20-2‚©‚çpingrep‚ÌŒy—Ê‰» */
	Packet_PingReply_Idle,			/* 1.20-2‚©‚çpingrep‚ÌŒy—Ê‰» */
	Packet_Comment,					/* 1.20-2‚©‚çƒRƒƒ“ƒg‚ğPingReply‚©‚ç•ª—£ */

	Packet_CompWatchData,		/* ˆ³kŠÏíƒf[ƒ^ */
	Packet_CompWatchDataReply,	/* ˆ³kŠÏíƒf[ƒ^‚Ì•ÔM */

	Packet_BattleInfoRequest,	/* ‘Îí’†‚Ìî•ñ—v‹ */
	Packet_BattleInfo,			/* ‘Îí’†‚Ìî•ñ */
};

enum EBlockDataType
{
	Block_RandomTable = 0,
	Block_PlayerInfo,
	Block_KeySetting,
	Block_Palette,

	Block_NetLog = 124,
	Block_KeyLog = 125,
	Block_RndLog = 126,

	Block_TestData = 127,
};

enum EStateType
{
	State_Idle = 0,
	State_Busy,				// ‘Îí’†
	State_NoResponse,
	State_Mismatch,
	State_VersionError,
	State_NotReady,
	State_Unknown,
	State_PingOver,			// ŠÏí’†‚Å‚àPing‚ªŠî€ˆÈã‚È‚çPingOver‚Æ‚·‚é
	
	State_Watch,			// ŠÏí’†
	State_Watch_Playable,	// ŠÏí’†‚¾‚ª‘Îí—v‹‚É‰‚¶‚é
	State_Busy_Casting,		// ‘Îí’†‚ÅŠÏí‰Â”
	State_Busy_Casting_NG,	// ‘Îí’†‚ÅŠÏí‰Â”\‚¾‚ªA‚Ü‚¾ƒLƒƒƒ‰ƒZƒŒ’†
};

enum EValidFlag{
	VF_RANK		= 0x00000001,
	VF_WINS		= 0x00000002,
	VF_EX		= 0x00000004,
	VF_ROUND	= 0x00000008,
	VF_COUNT	= 0x00000010,
	VF_VERSION	= 0x00000020,
	VF_COMMENT	= 0x00000040,
	VF_CAST		= 0x00000080,
	VF_ID		= 0x00000100,
	VF_DENY		= 0x00000200,
};

#pragma pack(push)
#pragma pack(1)

typedef struct
{
	char	packetType;
	char	data[PACKETMAX_SIZE-1];
}SPacket_Unknown;

enum { BF_IDLE=0, BF_BUSY=1, BF_BUSY_CAST_OK=2, BF_BUSY_CAST_NG=3 };
enum { WF_WATCH=0x80, WF_INTRUSION=0x40 };

typedef struct
{
	enum { SIZE115=66, SIZE120=72, };
	enum {
		VF115 = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_COUNT | VF_VERSION | VF_CAST, // -16‚ÅŒÅ’è‚È‚Ì‚ÅVF_CAST‚à—LŒø‚É‚µ‚Ä‚¨‚­
		VF120 = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_COUNT | VF_VERSION | VF_CAST | VF_ID,
	};

	char	packetType;
	char	cid;

	DWORD	scriptCode;
	char	name[30];
	char	ver[10];
	char	mac[6];

	char	delay;
	char	ex;
	short	wins;
	char	rank;
	char	notready;
	char	ignoreSlow;
	char	round;
	bool	deny;
	bool	needDetail;
	int		gamecount;
	// 1.20
	char	hdid[4];		// mac‚Ì‘±‚«
	BYTE	watchFlags;		// 7bit : watch
							// 6bit : allow intrusion (Setting)
	char	watchMaxNode;
}SPacket_Ping;

typedef struct
{
	enum { VF = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_COUNT | VF_COMMENT | VF_CAST }; // -16‚ÅŒÅ’è‚È‚Ì‚ÅVF_CAST‚à—LŒø‚É‚µ‚Ä‚¨‚­

	// 1.12-3‚Ü‚Å‚Í‚±‚¿‚ç‚ğg—p
	char	packetType;
	char	cid;

	char	msg[256];

	char	delay;
	char	ex;
	short	wins;
	char	rank;
	char	notready;
	char	ignoreSlow;
	char	round;
	bool	deny;
	int		gamecount;
}SPacket_PingReply112_3;

typedef struct
{
	enum { SIZE115=281, SIZE120=285, };
	enum {
		VF115 = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_COMMENT | VF_ID,
		VF120 = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_COUNT | VF_COMMENT | VF_ID,
	};

	char	packetType;
	char	cid;

	// ver1.13‚æ‚èMACƒAƒhƒŒƒX‚Ìd•¡‚ğ–h‚®‚½‚ßHD‚ÌƒVƒŠƒAƒ‹‚à•t‰Á‚·‚é
	// ‹Œƒo[ƒWƒ‡ƒ“‚Æ‚ÌŒİŠ·‚ğæ‚é‚½‚ßA‘Šè‚Ìƒo[ƒWƒ‡ƒ“‚ğ’²‚×‚½ã‚Å10byte•ª‚ÌƒR[ƒh‚ğ‘—‚é
	// ƒR[ƒh‚Ì”äŠr‚Í­‚È‚¢‚Ù‚¤‚É‚ ‚í‚¹‚é(HD‚ÌƒVƒŠƒAƒ‹‚ªffffffff‚È‚çƒƒCƒ‹ƒhƒJ[ƒh‚Æ‚·‚é)
	char	id[10];
	char	msg[256];

	char	delay;
	char	ex;
	short	wins;
	char	rank;
	char	notready;
	char	ignoreSlow;
	char	round;
	bool	deny;
	// 1.20
	int		gamecount;
}SPacket_PingReply;

/* ƒlƒbƒgƒ[ƒN•‰‰×‚ğŒyŒ¸‚·‚é‚½‚ßASPacket_Ping::needDetail‚ªƒIƒt‚Ì‚Æ‚«‚Í‚±‚¿‚ç‚ğg—p */
/* ‚Ç‚¤‚¹©•ª‚Åping‘—‚é‚Æ‚«‚Íˆê’Ê‚è‚Ìî•ñ‚ğ‘—‚Á‚Ä‚¢‚é */
typedef struct
{
	enum { SIZE115=3, SIZE120=98, };
	enum {
		VF115 = 0x00000000,
		VF120 = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_VERSION | VF_COUNT | VF_ID,
	};

	char	packetType;
	char	cid;
	char	busy;
	// 1.20
	char	name[2][30];	// ‘Îí‘Šè‚Ìî•ñ
	char	chara[2];		// ‘ÎíƒLƒƒƒ‰‚Ìî•ñ

	char	id[10];
	char	ver[10];

	char	delay;
	char	ex;
	short	wins;
	char	rank;
	char	round;
	bool	deny;
	int		gamecount;	// ŠÏí‚Ìˆ×‚Ì‡¯•Ê‚Ég—p

	BYTE	watchFlags;	// 7bit : watch
						// 6bit : allow intrusion (Setting)
	char	watchMaxNode;
}SPacket_PingReplyLite120;

// 120-2‚©‚çó‹µ•Ê‚ÉˆÙ‚È‚éƒƒbƒZ[ƒW‚ğ•Ô‚·
typedef struct
{
	enum { VF = VF_RANK | VF_WINS | VF_COUNT };

	char	packetType;
	char	cid;
	bool	casting;
	short	wins;
	char	rank;
	int		gamecount;		// ŠÏí‚Ìˆ×‚Ì‡¯•Ê‚Ég—p
}SPacket_PingReply_BusyCasting;

typedef struct
{
	enum { VF = 0x00000000 };

	char	packetType;
	char	cid;
}SPacket_PingReply_Busy;

typedef struct
{
	enum { VF = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_VERSION | VF_COUNT | VF_CAST | VF_ID };

	char	packetType;
	char	cid;

	char	id[10];
	char	ver[10];

	char	delay;
	char	ex;
	short	wins;
	char	rank;
	char	round;
	char	ignoreSlow;
	bool	deny;
	int		gamecount;		// ŠÏí‚Ìˆ×‚Ì‡¯•Ê‚Ég—p

	bool	allowIntrusion;
	char	watchMaxNode;
}SPacket_PingReply_Watch;

typedef struct
{
	enum { VF = VF_COMMENT };

	// ping‚ÌneedDetail=true‚ÌA
	// pingrep‚Æ“¯‚¶ƒ^ƒCƒ~ƒ“ƒO‚Å‘—‚é
	char	packetType;
	BYTE	size;
	char	msg[256];
}SPacket_Comment;

typedef struct
{
	enum { VF = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_VERSION | VF_COUNT | VF_CAST | VF_ID };

	char	packetType;
	char	cid;
	bool	notready;

	char	ver[10];
	char	id[10];

	char	delay;
	char	ex;
	short	wins;
	char	rank;
	char	round;
	char	ignoreSlow;
	bool	deny;
	int		gamecount;	// ŠÏí‚Ìˆ×‚Ì‡¯•Ê‚Ég—p
	
	bool	allowIntrusion;
	char	watchMaxNode;
}SPacket_PingReply_Idle;

typedef struct
{
	char	packetType;
	DWORD	time;
	struct
	{
		WORD	key;
		BYTE	syncChk;
	}cell[32];
}SPacket_Key;

typedef struct
{
	enum { SIZE120=2, SIZE120_2=4, };

	char	packetType;
	char	cid;
	// 1.20-2
	short	maxPacketSize;	// ƒuƒƒbƒN“]‘—‚Å‚ÌÅ‘åƒpƒPƒbƒgƒTƒCƒY
}SPacket_Connect, SPacket_ConnectReply;

typedef struct
{
	char	packetType;
	char	cid;
}SPacket_VSLoadCompleted, SPacket_Suspend, SPacket_SuspendReply, SPacket_Resume, SPacket_ResumeReply, SPacket_NameRequest;

typedef struct
{
	enum { PACKET_HEADER_SIZE = 12, };

	char	packetType;
	char	type;
	int		seq;
	WORD	dataSize;
	DWORD	dataOffset;
	char	data[PACKETMAX_SIZE - PACKET_HEADER_SIZE];
}SPacket_Data;

typedef struct
{
	char	packetType;
	DWORD	seq;
}SPacket_DataReply;

// ”zMŒ³‚Ö’¼ÚÚ‘±‚·‚é
typedef struct
{
	enum { SIZE120=6, SIZE120_2=13, };

	char	packetType;
	char	cid;
	int		dataOffset;		// —v‹ƒf[ƒ^‚Ìæ“ªƒoƒCƒgi“r’†‚©‚ç—v‹‚³‚ê‚éê‡‚à‚ ‚é‚Ì‚Åj
	// 1.20-2
	int		targetGameCount;// ”zM‡ƒJƒEƒ“ƒg
	char	format;			// ‘Î‰ƒtƒH[ƒ}ƒbƒg(0|–¢’è‹`=RawData‚Ì‚İA1=ˆ³k‚ğƒTƒ|[ƒg)
	short	maxPacketSize;	// Å‘åƒpƒPƒbƒgƒTƒCƒY
}SPacket_WatchInRoot;

typedef struct
{
	enum { SIZE120_FMT1=87, SIZE120_2_FMT1=87 };
	enum { SIZE120_FMT2=19, SIZE120_2_FMT2=27 };
	
	char		packetType;
	char		cid;
	char		accept;
	//sockaddr_in	myAddr;		// ©•ª‚ÌƒAƒhƒŒƒX‚Í’m‚Á‚Ä‚¢‚é‚Í‚¸‚È‚Ì‚Å‘—‚ç‚È‚¢
	sockaddr_in	enemyAddr;		// q‚É‘Šè‚ÌƒAƒhƒŒƒX‚ğ‹³‚¦‚é

	union
	{
		struct
		{
			// accept == 1‚Ì‚Æ‚«
			char myName[30];		// q‚É©•ª‚Ì–¼‘O‚ğ‹³‚¦‚é
			char enemyName[30];		// q‚É‘Šè‚Ì–¼‘O‚ğ‹³‚¦‚é
			int  myGameCount;		// q‚É©•ª‚Ì‡ID‚ğ‹³‚¦‚é
			int  enemyGameCount;	// q‚É‘Šè‚Ì‡ID‚ğ‹³‚¦‚é
		}format1;
		struct
		{
			// 1.20-2ˆÈ~‚Ìaccept == 0‚Ì‚Æ‚«
			int  myGameCount;		// q‚É©•ª‚Ì‡ID‚ğ‹³‚¦‚é
			int  enemyGameCount;	// q‚É‘Šè‚Ì‡ID‚ğ‹³‚¦‚é
		}format2;
	}extra;
}SPacket_WatchInRootReply;

typedef struct
{
	enum { SIZE120=44, SIZE120_2=47, };

	char	packetType;
	char	cid;
	char	targetName[30];
	in_addr	targetIP;		// ”zMŒ³IP ‚¹‚Á‚©‚­’†Œp‚·‚é‚Ì‚¾‚©‚çƒ‹[ƒg‚ªNAT“à‚Å‚àÚ‘±‚Å‚«‚é‚æ‚¤‚É‚µ‚½‚¢
	int		targetGameCount;// ”zM‡ƒJƒEƒ“ƒg
	int		dataOffset;		// —v‹ƒf[ƒ^‚Ìæ“ªƒoƒCƒgi“r’†‚©‚ç—v‹‚³‚ê‚éê‡‚à‚ ‚é‚Ì‚Åj
	// 1.20-2
	char	format;			// ‘Î‰ƒtƒH[ƒ}ƒbƒg(0|–¢’è‹`=RawData‚Ì‚İA1=ˆ³k‚ğƒTƒ|[ƒg)
	short	maxPacketSize;	// Å‘åƒpƒPƒbƒgƒTƒCƒY
}SPacket_WatchIn;

typedef struct
{
	char		packetType;
	char		cid;
	char		rootName[2][30];	// q‚É”zMŒ³‚Ì–¼‘O‚ğ‹³‚¦‚é
	sockaddr_in	rootIP[2];			// ”zMŒ³ƒAƒhƒŒƒX‚ğ‹³‚¦‚é
	int			rootGameCount[2];	// ”zM‡ƒJƒEƒ“ƒg
}SPacket_WatchInReply;

typedef struct
{
	enum {
		MINBUFFERSIZE = 64,
		MAXBUFFERSIZE = 512,
		PACKET_HEADER_SIZE = 8,// sizeof(packetType) + sizeof(offset) + sizeof(size) + sizeof(galleryCount)
	};
	char	packetType;
	DWORD	offset;
	WORD	size;
	BYTE	galleryCount;			/* ‚±‚Ì‘Îí‚Ì‘ŠÏíÒ”‚ğ”zMÒ‚©‚ç’Ê’m */
	char	data[MAXBUFFERSIZE];	/* ÀÛ‚É‚Í•K—v‚È•ª‚¾‚¯‘—‚é */
}SPacket_WatchData;

typedef struct
{
	char	packetType;
	BYTE	reserved;			/* –¢g—pi=0j */
	DWORD	size;
	BYTE	childCount;			/* ©•ª‚ÌqŠÏíÒ”‚ğe‚Ö•ñ */
}SPacket_WatchDataReply;

typedef struct
{
	enum {
		MINBUFFERSIZE = 64,
		MAXBUFFERSIZE = 512,
		PACKET_HEADER_SIZE = 12,
	};
	char	packetType;
	WORD	compblock_offset;		/* ˆ³kƒf[ƒ^ƒuƒƒbƒN‚ÌƒIƒtƒZƒbƒgi¡‰ñ“]‘—‚µ‚½ƒf[ƒ^‚Ìˆ³kƒf[ƒ^ã‚Ìn“_j */
	WORD	compblock_size;			/* ˆ³kƒf[ƒ^ƒuƒƒbƒN‚ÌƒTƒCƒY */
	DWORD	compall_offset;			/* ˆ³kƒf[ƒ^‚Ì‘S‘Ì‚ÌƒIƒtƒZƒbƒgi¶ƒf[ƒ^‚Ì‰½ˆ‚©‚çˆ³k‚µ‚½‚Ì‚©j */
	WORD	compall_size;			/* ˆ³kƒf[ƒ^‚Ì‘S‘Ì‚ÌƒTƒCƒY */
	BYTE	galleryCount;			/* ‚±‚Ì‘Îí‚Ì‘ŠÏíÒ”‚ğ”zMÒ‚©‚ç’Ê’m */
	char	data[MAXBUFFERSIZE];	/* ÀÛ‚É‚Í•K—v‚È•ª‚¾‚¯‘—‚é */
}SPacket_CompWatchData;

typedef struct
{
	char	packetType;
	BYTE	reserved;			/* –¢g—pi=0j */
	WORD	compsize;			/* óMÏˆ³kƒf[ƒ^ƒTƒCƒY */
	DWORD	rawsize;			/* ¶ƒf[ƒ^‚Æ‚µ‚Ä“WŠJ‚Å‚«‚½ƒTƒCƒY‚ğ•ñ */
	BYTE	childCount;			/* ©•ª‚ÌqŠÏíÒ”‚ğe‚Ö•ñ */
}SPacket_CompWatchDataReply;

//typedef struct
//{
//	char	packetType;
//	char	name[30];
//	char	addr[32];
//}SPacket_NodeAddr115_2;

// ’ÊM‰Â”\‚ÈIP:Port‚ğ‘¼‚Ìƒm[ƒh‚É“`‚¦‚é
typedef struct
{
	char	packetType;
	DWORD	scriptCode;
	char	name[30];
	char	addr[32];
}SPacket_NodeAddr;

// ‘Îí’†‚Ìƒm[ƒh‚ª—¼Ò‚ÌŠÏíÒ”‚ğ“Š‚°‡‚¤
typedef struct
{
	char	packetType;
	int		galleryCount;
}SPacket_GalleryCount;

// ŠÏíÒ‚ªƒƒr[‚©‚ç‚ÌPing‚É‘Î‚·‚é•ÔM‚Æ‚µ‚Ä‘Îí‚ÌŠÏíÒ”‚ğ•ñ‚·‚é
typedef struct
{
	enum { SIZE120=73, SIZE120_2=81, };

	char	packetType;
	char	name[2][30];
	DWORD	ip[2];
	int		galleryCount;
	// 1.20-2
	int		gameCount[2];	// ‡¯•Ê—p‚É’Ç‰Á
}SPacket_GalleryCountForLobby;

typedef struct
{
	char	packetType;
	char	cpu_name[49];
	DWORD	cpu_eax;
	DWORD	cpu_edx;
	DWORD	cpu_ecx;
	WORD	fcw;
	char	analog[2];
	char	ggmode;
}SPacket_DebugInfo;

typedef struct
{
	char	packetType;
	char	targetName[30];	// ”zMÒ–¼
	in_addr	targetIP;		// ”zMŒ³IP
	int		targetGameCount;// ”zM‡ƒJƒEƒ“ƒg
}SPacket_BattleInfoRequest;

typedef struct
{
	char	packetType;
	char	name[2][30];
	in_addr	ip[2];
	int		gamecount[2];
	char	chara[2];
}SPacket_BattleInfo;

typedef struct
{
	char	nametrip[30];
	char	rank;
	char	round;
	WORD	wins;
	WORD	oldcs;
	char	ex;
}SBlock_PlayerInfo;

#pragma pack(pop)

class CWatcher
{
public:
	CWatcher(void) { m_compData = new char[MAX_COMPWATCHDATASIZE]; }
	~CWatcher(void) { delete[] m_compData; }

	inline bool isActive(void) { return m_remoteAddr.sin_port != 0; }

	void init(void)
	{
		m_remoteAddr = NULL_ADDR;
		m_sendSize = 0;
		m_sendTime = 0xffffffff;
		m_childCount = 0;
		m_supportedFormat = 0;
		m_compOffset = 0;
		m_compSize = 0;
		m_compSendSize = 0;
	}

public:
	sockaddr_in	m_remoteAddr;
	DWORD		m_sendSize;			// ‘—MÏ‚İƒf[ƒ^ƒTƒCƒY
	int			m_childCount;		// q‚Ì”‘‡ŠÏíÒ”‚ğ’m‚é‚½‚ß‚Éq‚©‚ç\‚³‚¹e‚É•ñ‚·‚é
	DWORD		m_sendTime;			// ÅŒã‚ÉwatchData‚ğ‘—M‚µ‚½ŠÔ watchDataReply‚ğó‚¯‚é‚ÆƒNƒŠƒA‚·‚é
	char		m_supportedFormat;	// ƒTƒ|[ƒgƒtƒH[ƒ}ƒbƒg(0=RawData‚Ì‚İA1=ˆ³kƒTƒ|[ƒg)
	short		m_maxPacketSize;	// Å‘åƒpƒPƒbƒgƒTƒCƒY

// ˆ³kƒf[ƒ^‚ÌƒTƒ|[ƒgiƒf[ƒ^ƒTƒCƒY‚ª128ˆÈã‚¾‚Á‚½‚çˆ³kƒf[ƒ^‚ğg—p‚·‚éj
	char*		m_compData;			// ‘—M‚µI‚í‚é‚Ü‚Åˆ³kƒf[ƒ^‚ğ•Û‚µ‚Ä‚¨‚­
	int			m_compOffset;		// ¶ƒf[ƒ^‚É‘Î‚·‚éˆ³kƒf[ƒ^‚ÌƒIƒtƒZƒbƒg
	int			m_compSize;			// ˆ³kƒf[ƒ^‘S‘Ì‚ÌƒTƒCƒY
	int			m_compSendSize;		// ‘—MÏ‚İˆ³kƒf[ƒ^‚ÌƒTƒCƒY
};

class CNetMgr
{
public:
	CNetMgr(void);
	~CNetMgr(void);
	
	bool init(int p_port, int p_delay, bool p_useLobby);
	void startThread(void);
	void stopThread(void);

	void connect(void);
	void disconnect(char* p_cause);
	void resume(void);
	void suspend(void);

	void setErrMsg(char* p_msg);

	char*		getStringFromAddr(sockaddr_in* p_addr, char* p_output);
	sockaddr_in getAddrFromString(char* p_str);

	bool watch(char* p_targetName, sockaddr_in* p_targetAddr, int p_targetGameCount, bool p_blockingMode);
	int  findFreeWatchEntry(sockaddr_in* p_addr);
	int  getChildWatcherCount(void);

	bool send_connect(sockaddr_in* p_addr);
	void send_connectReply(void);
	void send_key(int p_time);
	bool send_watchInRoot(sockaddr_in* p_addr, int p_targetGameCount, bool& p_success);
	void send_watchInRootReply(bool p_accept);
	bool send_watchIn(char* p_targetName, sockaddr_in* p_targetIP, int p_targetGameCount);
	void send_watchInReply(void);
	void send_watchData(int p_idx);
	void send_watchDataReply(int p_size);
	void send_compWatchDataReply(int p_compsize, int p_rawsize);
	bool send_ping(sockaddr_in* p_addr, int p_selNodeIdx);
	void send_pingReply120(bool p_needDetail, bool p_deny, bool p_underV113);
	void send_pingReply(bool p_deny);
	void send_comment(void);
	void send_vsLoadCompleted(void);
	bool send_suspend(void);
	void send_suspendReply(void);
	bool send_resume(void);
	void send_resumeReply(void);
	void send_dataReply(int p_seq);
	void send_nodeaddr115_3(sockaddr_in* p_addr, class CNode* p_node);
	void send_galleryCount(void);
	void send_galleryCountForLobby(void);
	void send_nameRequest(sockaddr_in* p_addr);
	void send_debugInfo(void);

	bool send_battleInfoRequest(char* p_targetName, sockaddr_in* p_targetIP, int p_targetGameCount);
	void send_battleInfo(char* p_name1, char* p_name2, DWORD p_ip1, DWORD p_ip2, DWORD p_gamecount1, DWORD p_gamecount2, char p_chara1, char p_chara2);

	bool sendDataBlock(char p_type, char* p_data, int p_dataSize, int p_timeout);
	bool recvDataBlock(char p_type, char* p_data, int p_dataSize, int p_timeout);

	void initWatchVars(void);

private:
	bool talking(void);

	int udpsend(sockaddr_in* p_addr, char* p_data, int p_dataSize);
	int udprecv(char* p_buf, int p_bufSize);

#if USE_TCP
	int tcpsend(char* p_data, int p_dataSize, int p_timeout);
	int tcprecv(char* p_buf, int p_bufSize, int p_timeout);
#endif

	friend DWORD WINAPI _recvThreadProc(LPVOID lpParameter);
	friend DWORD WINAPI _lobbyThreadProc(LPVOID lpParameter);

private:
	volatile bool	m_quitApp;
	volatile bool	m_recvThread_end;
	volatile bool	m_lobbyThread_end;
	HANDLE			m_recvThread;
	HANDLE			m_lobbyThread;

public:
	SPacket_Unknown	m_buf;

	sockaddr_in	m_remoteAddr_recv;		/* recvfrom“™‚Åˆê“I‚È‘Šè‚ÌƒAƒhƒŒƒX */
	sockaddr_in	m_remoteAddr_active;	/* connection¬—§Œã‚ÌŒÅ’è“I‚È‘Šè‚ÌƒAƒhƒŒƒX */
										/* watch‚ÌóMŒ³ƒAƒhƒŒƒX‚à‚±‚±‚ÉŠi”[‚³‚ê‚é */
	SOCKET		m_udpSocket;
	SOCKET		m_tcpSocket;

	bool		m_networkEnable;
	bool		m_connect;		/* ‘Îí’†‚©H */
	int			m_queueSize;
	int			m_delay;
	int			m_playSide;		/* 1=1P, 2=2P, 3=Watch */
	DWORD		m_time;
	DWORD*		m_key;
	WORD*		m_syncChk;		/* “¯Šúƒ`ƒFƒbƒN */

	bool		m_suspend;		/* ƒL[‚Ì“¯Šú‚ğæ‚ç‚È‚¢ */
	int			m_suspendFrame;	/* Suspend‚µ‚Ä‚¢‚éŠÔ */
	int			m_vsloadFrame;	/* vsload‚ÌŒo‰ßƒtƒŒ[ƒ€ */
	int			m_totalSlow;	/* ƒlƒbƒgƒ[ƒN‚Ì“s‡‚Å’â~‚µ‚½ŠÔB“Á‚Ég‚¢“¹‚Í–³‚¢ */
	int			m_lobbyFrame;
	bool		m_initKeySet;	/* ƒL[İ’è‚ª‘—óMÏ‚©H */

	int			m_enMaxPacketSize;	/* ‘Šè‚ÌÅ‘åƒpƒPƒbƒgƒTƒCƒY */

	/* ‘Šè‚©‚ç‚Ìƒf[ƒ^‘Ò‚¿ƒtƒ‰ƒO */
	volatile bool	m_waitingConnectReply;
	volatile bool	m_waitingSuspendReply;
	volatile bool	m_waitingResumeReply;
	volatile bool	m_waitingData;
	volatile bool	m_waitingDataReply;
	volatile bool	m_waitingWatchInReply;
	volatile char	m_waitingWatchInRootReply;	/* ‘Ò‹@‚µ‚È‚¢ = 0, ‘Ò‹@’† = 1, Ú‘±‰Â = 2, Ú‘±•s‰Â = 3 */
	volatile bool	m_waitingBattleInfoRequestReply;

	enum { EWIRReply_Idle = 0, EWIRReply_Wait, EWIRReply_Success, EWIRReply_Fail, };

	volatile int	m_waitingDataType;		/* Packet_DataƒRƒ}ƒ“ƒh‚ÅóM‘Ò‚¿‚Ìƒf[ƒ^ƒ^ƒCƒv */

	volatile bool	m_recvSuspend;			/* ‘Šè‚ÌSuspendó‹µ */
	volatile bool	m_recvVSLoadCompleted;	/* ‘Šè‚Ìƒ[ƒhó‹µ */
	
	char*		m_recvDataPtr;			/* Packet_DataƒRƒ}ƒ“ƒh‚É‚æ‚èóM‚·‚éƒoƒbƒtƒ@ƒ|ƒCƒ“ƒ^ */
	DWORD		m_recvDataSize;			/* Packet_DataƒRƒ}ƒ“ƒh‚É‚æ‚èóM‚·‚éƒoƒbƒtƒ@ƒTƒCƒY */

	int			m_sendDataSeq;	/* ‘—Mƒf[ƒ^ƒV[ƒPƒ“ƒX”Ô† */
	int			m_recvDataSeq;	/* óMÏ‚İƒf[ƒ^ƒV[ƒPƒ“ƒX”Ô† */
								/* connect‚Ì‚½‚Ñ‚ÉƒŠƒZƒbƒg‚·‚é */
								/* ‚±‚Ì’lˆÈ‰º‚Ìƒf[ƒ^‚ÍóMÏ‚È‚Ì‚Åó‚¯æ‚ç‚È‚¢ */

	CRITICAL_SECTION	m_csKey;
	CRITICAL_SECTION	m_csNode;
	CRITICAL_SECTION	m_csWatch;

	char		m_errMsg[1024];
	int			m_errMsgTime;

	// for watch client
	bool		m_watch;				// ŠÏíƒNƒ‰ƒCƒAƒ“ƒg‚Æ‚µ‚Ä“®ì’†
	bool		m_1stCaster;			// ˆêŸ”zMÒ‚©H
	bool		m_watchRecvComplete;	// ƒf[ƒ^óM‚ªŠ®—¹‚µ‚Ä‚¢‚é‚©H
	char		m_watchRootName[2][30];	// ”zMŒ³‚Ì–¼‘O
	sockaddr_in	m_watchRootAddr[2];		// ”zMŒ³‚ÌƒAƒhƒŒƒX
	int			m_watchRootGameCount[2];// ”zMŒ³‚Ì‡ID
	int			m_watchRecvSize;		// óMÏƒf[ƒ^ƒTƒCƒY
	DWORD		m_lastWatchDataTime;	// ÅŒã‚ÉPacket_WatchData‚ğó‚¯æ‚Á‚½ŠÔ
	int			m_totalGalleryCount;	// ‘‡ŠÏíÒ”iŠÏí’†‚É”zMÒ‚ª‘ÎíI—¹‚·‚é‚Æ‚»‚Ì‚Ü‚ÜXV‚³‚ê‚È‚­‚È‚é‚ªA‚»‚ê‚Íd—l‚Æ‚·‚éj
	int			m_watchFailCount;		// ˜A‘±‚Å©“®ŠÏí—v‹‚É¸”s‚µ‚½‰ñ”
	sockaddr_in	m_watchParentAddr;		// e‚ÌƒAƒhƒŒƒX

	// ˆ³kƒf[ƒ^‚ÌƒTƒ|[ƒgióMƒf[ƒ^ƒTƒCƒY‚ª128ˆÈã‚¾‚Á‚½‚çˆ³kƒf[ƒ^‚ğg—p‚·‚éj
	char*		m_watchRecvCompData;	// ‚·‚×‚ÄóM‚µI‚í‚é‚Ü‚Åˆ³kƒf[ƒ^‚ğ•Û‚µ‚Ä‚¨‚­
	int			m_watchRecvCompSize;	// óMÏ‚İˆ³kƒf[ƒ^‚ÌƒTƒCƒY

	// for watch server
	CWatcher	m_watcher[WATCH_MAX_CHILD];
	int			m_recvGalleryCount;		// ‘Šè‘¤‚ÌŠÏíÒ”
};

// extern
extern CNetMgr* g_netMgr;
