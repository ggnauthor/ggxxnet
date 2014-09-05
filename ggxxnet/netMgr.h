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

/* ggxxnetコネクションID・一致していればVer違いでも通信可能 */
/* ver1.11以降、ver1.12系を例外としてすべて12で固定している */
#define CONNECTION_ID		12

#define	LOBBY_VER			6		/* ロビーバージョン */
#define REPLAY_VER			3		/* Replayバージョン */
// 3 = 1.10-
// 2 = 1.09
// 1 = 1.06-1.08b
// 0 = 1.00-1.05
#define	PACKETMAX_SIZE		1024
#define LOG_SIZE			(1024*1024)	// 1MB

#define MAX_COMPWATCHDATASIZE	30000	// 圧縮済なので30kbあれば余裕

#define WATCH_MAX_CHILD_BASE	3
#define WATCH_MAX_CHILD_INC		2
#define WATCH_MAX_CHILD			(WATCH_MAX_CHILD_BASE + WATCH_MAX_CHILD_INC)

#define MSPF_FLOAT	16.66666667f
#define MSPF_INT	17

/* 各タイムアウト時間（秒） */
#define TIMEOUT_VSLOAD	5000	/* VS画面が終わってからの待ち時間 */
#define TIMEOUT_SUSPEND	5000	/* 接続～CS開始、CS終了～VS開始までのSuspendタイムアウト 異常ケースなので長めで良い */
#define TIMEOUT_KEY		1000	/* キー入力 */
#define TIMEOUT_KEY2	3000	/* キー入力(フリーズ対策) */
#define TIMEOUT_REPLY	3000	/* Suspend, Resume, Connect、Dataの返信 */
#define TIMEOUT_PING	500		/* ping待ち */
#define TIMEOUT_BLOCK	3000	/* データブロック転送 */

#define TIMEOUT_WATCHDATA		1000	/* いつまでも観戦データが来ない */
#define TIMEOUT_WATCHDATAREPLY	1000	/* いつまでも観戦データ返信が来ない */

#define TIMEOUT_WATCHDATAWAIT	4000	/* 観戦データ待ちによる停止時間限界 */

#define WATCH_RESUME_INTERVAL	2000	/* 観戦接続が切れた時の再要求間隔 */

const sockaddr_in NULL_ADDR = { 0, 0,						// family, port
								0, 0, 0, 0,					// addr
								0, 0, 0, 0, 0, 0, 0, 0 };	// zero

enum EPacketDataType
{
	Packet_Connect = 0,		/* 接続要求 */
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

	Packet_WatchIn,			/* 観戦要求 */
	Packet_WatchInReply,
	Packet_WatchInRoot,		/* 配信元に対する直接観戦要求 */
	Packet_WatchInRootReply,
	Packet_WatchData,		/* 観戦の為の断片的なリプデータ。再送はしないがロストした場合、次回送信分に上乗せされる */
							/* あまりにも返信が来ない場合は切断したものとみなし、以後転送しない */
	Packet_WatchDataReply,	/* 観戦データの返信。再送はしない */
	Packet_PingReply,
	Packet_NodeAddr115_2,	/* 一部のバージョンで未だに使用されているので再利用禁止！！ */
	Packet_NodeAddr,

	Packet_GalleryCount,		/* 対戦者同士で観戦者数を送信する */
	Packet_GalleryCountForLobby,/* ロビーにいるノードに観戦の情報を提供 */
	Packet_NameRequest,			/* UNKNOWN_NAMEに対して発行。名前を得るためpingメッセージを要求 */
	Packet_DebugInfo,			/* SyncErrorの原因調査用 */

	Packet_PingReply_BusyCasting,	/* 1.20-2からpingrepの軽量化 */
	Packet_PingReply_Busy,			/* 1.20-2からpingrepの軽量化 */
	Packet_PingReply_Watch,			/* 1.20-2からpingrepの軽量化 */
	Packet_PingReply_Idle,			/* 1.20-2からpingrepの軽量化 */
	Packet_Comment,					/* 1.20-2からコメントをPingReplyから分離 */

	Packet_CompWatchData,		/* 圧縮観戦データ */
	Packet_CompWatchDataReply,	/* 圧縮観戦データの返信 */

	Packet_BattleInfoRequest,	/* 対戦中の情報要求 */
	Packet_BattleInfo,			/* 対戦中の情報 */
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
	State_Busy,				// 対戦中
	State_NoResponse,
	State_Mismatch,
	State_VersionError,
	State_NotReady,
	State_Unknown,
	State_PingOver,			// 観戦中でもPingが基準以上ならPingOverとする
	
	State_Watch,			// 観戦中
	State_Watch_Playable,	// 観戦中だが対戦要求に応じる
	State_Busy_Casting,		// 対戦中で観戦可能
	State_Busy_Casting_NG,	// 対戦中で観戦可能だが、まだキャラセレ中
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
		VF115 = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_COUNT | VF_VERSION | VF_CAST, // -16で固定なのでVF_CASTも有効にしておく
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
	char	hdid[4];		// macの続き
	BYTE	watchFlags;		// 7bit : watch
							// 6bit : allow intrusion (Setting)
	char	watchMaxNode;
}SPacket_Ping;

typedef struct
{
	enum { VF = VF_DENY | VF_RANK | VF_WINS | VF_EX | VF_ROUND | VF_COUNT | VF_COMMENT | VF_CAST }; // -16で固定なのでVF_CASTも有効にしておく

	// 1.12-3まではこちらを使用
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

	// ver1.13よりMACアドレスの重複を防ぐためHDのシリアルも付加する
	// 旧バージョンとの互換を取るため、相手のバージョンを調べた上で10byte分のコードを送る
	// コードの比較は少ないほうにあわせる(HDのシリアルがffffffffならワイルドカードとする)
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

/* ネットワーク負荷を軽減するため、SPacket_Ping::needDetailがオフのときはこちらを使用 */
/* どうせ自分でping送るときは一通りの情報を送っている */
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
	char	name[2][30];	// 対戦相手の情報
	char	chara[2];		// 対戦キャラの情報

	char	id[10];
	char	ver[10];

	char	delay;
	char	ex;
	short	wins;
	char	rank;
	char	round;
	bool	deny;
	int		gamecount;	// 観戦の為の試合識別に使用

	BYTE	watchFlags;	// 7bit : watch
						// 6bit : allow intrusion (Setting)
	char	watchMaxNode;
}SPacket_PingReplyLite120;

// 120-2から状況別に異なるメッセージを返す
typedef struct
{
	enum { VF = VF_RANK | VF_WINS | VF_COUNT };

	char	packetType;
	char	cid;
	bool	casting;
	short	wins;
	char	rank;
	int		gamecount;		// 観戦の為の試合識別に使用
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
	int		gamecount;		// 観戦の為の試合識別に使用

	bool	allowIntrusion;
	char	watchMaxNode;
}SPacket_PingReply_Watch;

typedef struct
{
	enum { VF = VF_COMMENT };

	// pingのneedDetail=trueの時、
	// pingrepと同じタイミングで送る
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
	int		gamecount;	// 観戦の為の試合識別に使用
	
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
	short	maxPacketSize;	// ブロック転送での最大パケットサイズ
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

// 配信元へ直接接続する
typedef struct
{
	enum { SIZE120=6, SIZE120_2=13, };

	char	packetType;
	char	cid;
	int		dataOffset;		// 要求データの先頭バイト（途中から要求される場合もあるので）
	// 1.20-2
	int		targetGameCount;// 配信試合カウント
	char	format;			// 対応フォーマット(0|未定義=RawDataのみ、1=圧縮をサポート)
	short	maxPacketSize;	// 最大パケットサイズ
}SPacket_WatchInRoot;

typedef struct
{
	enum { SIZE120_FMT1=87, SIZE120_2_FMT1=87 };
	enum { SIZE120_FMT2=19, SIZE120_2_FMT2=27 };
	
	char		packetType;
	char		cid;
	char		accept;
	//sockaddr_in	myAddr;		// 自分のアドレスは知っているはずなので送らない
	sockaddr_in	enemyAddr;		// 子に相手のアドレスを教える

	union
	{
		struct
		{
			// accept == 1のとき
			char myName[30];		// 子に自分の名前を教える
			char enemyName[30];		// 子に相手の名前を教える
			int  myGameCount;		// 子に自分の試合IDを教える
			int  enemyGameCount;	// 子に相手の試合IDを教える
		}format1;
		struct
		{
			// 1.20-2以降のaccept == 0のとき
			int  myGameCount;		// 子に自分の試合IDを教える
			int  enemyGameCount;	// 子に相手の試合IDを教える
		}format2;
	}extra;
}SPacket_WatchInRootReply;

typedef struct
{
	enum { SIZE120=44, SIZE120_2=47, };

	char	packetType;
	char	cid;
	char	targetName[30];
	in_addr	targetIP;		// 配信元IP せっかく中継するのだからルートがNAT内でも接続できるようにしたい
	int		targetGameCount;// 配信試合カウント
	int		dataOffset;		// 要求データの先頭バイト（途中から要求される場合もあるので）
	// 1.20-2
	char	format;			// 対応フォーマット(0|未定義=RawDataのみ、1=圧縮をサポート)
	short	maxPacketSize;	// 最大パケットサイズ
}SPacket_WatchIn;

typedef struct
{
	char		packetType;
	char		cid;
	char		rootName[2][30];	// 子に配信元の名前を教える
	sockaddr_in	rootIP[2];			// 配信元アドレスを教える
	int			rootGameCount[2];	// 配信試合カウント
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
	BYTE	galleryCount;			/* この対戦の総観戦者数を配信者から通知 */
	char	data[MAXBUFFERSIZE];	/* 実際には必要な分だけ送る */
}SPacket_WatchData;

typedef struct
{
	char	packetType;
	BYTE	reserved;			/* 未使用（=0） */
	DWORD	size;
	BYTE	childCount;			/* 自分の子観戦者数を親へ報告 */
}SPacket_WatchDataReply;

typedef struct
{
	enum {
		MINBUFFERSIZE = 64,
		MAXBUFFERSIZE = 512,
		PACKET_HEADER_SIZE = 12,
	};
	char	packetType;
	WORD	compblock_offset;		/* 圧縮データブロックのオフセット（今回転送したデータの圧縮データ上の始点） */
	WORD	compblock_size;			/* 圧縮データブロックのサイズ */
	DWORD	compall_offset;			/* 圧縮データの全体のオフセット（生データの何処から圧縮したのか） */
	WORD	compall_size;			/* 圧縮データの全体のサイズ */
	BYTE	galleryCount;			/* この対戦の総観戦者数を配信者から通知 */
	char	data[MAXBUFFERSIZE];	/* 実際には必要な分だけ送る */
}SPacket_CompWatchData;

typedef struct
{
	char	packetType;
	BYTE	reserved;			/* 未使用（=0） */
	WORD	compsize;			/* 受信済圧縮データサイズ */
	DWORD	rawsize;			/* 生データとして展開できたサイズを報告 */
	BYTE	childCount;			/* 自分の子観戦者数を親へ報告 */
}SPacket_CompWatchDataReply;

//typedef struct
//{
//	char	packetType;
//	char	name[30];
//	char	addr[32];
//}SPacket_NodeAddr115_2;

// 通信可能なIP:Portを他のノードに伝える
typedef struct
{
	char	packetType;
	DWORD	scriptCode;
	char	name[30];
	char	addr[32];
}SPacket_NodeAddr;

// 対戦中のノードが両者の観戦者数を投げ合う
typedef struct
{
	char	packetType;
	int		galleryCount;
}SPacket_GalleryCount;

// 観戦者がロビーからのPingに対する返信として対戦の観戦者数を報告する
typedef struct
{
	enum { SIZE120=73, SIZE120_2=81, };

	char	packetType;
	char	name[2][30];
	DWORD	ip[2];
	int		galleryCount;
	// 1.20-2
	int		gameCount[2];	// 試合識別用に追加
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
	char	targetName[30];	// 配信者名
	in_addr	targetIP;		// 配信元IP
	int		targetGameCount;// 配信試合カウント
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
	DWORD		m_sendSize;			// 送信済みデータサイズ
	int			m_childCount;		// 子の数総合観戦者数を知るために子から申告させ親に報告する
	DWORD		m_sendTime;			// 最後にwatchDataを送信した時間 watchDataReplyを受けるとクリアする
	char		m_supportedFormat;	// サポートフォーマット(0=RawDataのみ、1=圧縮サポート)
	short		m_maxPacketSize;	// 最大パケットサイズ

// 圧縮データのサポート（データサイズが128以上だったら圧縮データを使用する）
	char*		m_compData;			// 送信し終わるまで圧縮データを保持しておく
	int			m_compOffset;		// 生データに対する圧縮データのオフセット
	int			m_compSize;			// 圧縮データ全体のサイズ
	int			m_compSendSize;		// 送信済み圧縮データのサイズ
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

	sockaddr_in	m_remoteAddr_recv;		/* recvfrom等で一時的な相手のアドレス */
	sockaddr_in	m_remoteAddr_active;	/* connection成立後の固定的な相手のアドレス */
										/* watchの受信元アドレスもここに格納される */
	SOCKET		m_udpSocket;
	SOCKET		m_tcpSocket;

	bool		m_networkEnable;
	bool		m_connect;		/* 対戦中か？ */
	int			m_queueSize;
	int			m_delay;
	int			m_playSide;		/* 1=1P, 2=2P, 3=Watch */
	DWORD		m_time;
	DWORD*		m_key;
	WORD*		m_syncChk;		/* 同期チェック */

	bool		m_suspend;		/* キーの同期を取らない */
	int			m_suspendFrame;	/* Suspendしている時間 */
	int			m_vsloadFrame;	/* vsloadの経過フレーム */
	int			m_totalSlow;	/* ネットワークの都合で停止した時間。特に使い道は無い */
	int			m_lobbyFrame;
	bool		m_initKeySet;	/* キー設定が送受信済か？ */

	int			m_enMaxPacketSize;	/* 相手の最大パケットサイズ */

	/* 相手からのデータ待ちフラグ */
	volatile bool	m_waitingConnectReply;
	volatile bool	m_waitingSuspendReply;
	volatile bool	m_waitingResumeReply;
	volatile bool	m_waitingData;
	volatile bool	m_waitingDataReply;
	volatile bool	m_waitingWatchInReply;
	volatile char	m_waitingWatchInRootReply;	/* 待機しない = 0, 待機中 = 1, 接続可 = 2, 接続不可 = 3 */
	volatile bool	m_waitingBattleInfoRequestReply;

	enum { EWIRReply_Idle = 0, EWIRReply_Wait, EWIRReply_Success, EWIRReply_Fail, };

	volatile int	m_waitingDataType;		/* Packet_Dataコマンドで受信待ちのデータタイプ */

	volatile bool	m_recvSuspend;			/* 相手のSuspend状況 */
	volatile bool	m_recvVSLoadCompleted;	/* 相手のロード状況 */
	
	char*		m_recvDataPtr;			/* Packet_Dataコマンドにより受信するバッファポインタ */
	DWORD		m_recvDataSize;			/* Packet_Dataコマンドにより受信するバッファサイズ */

	int			m_sendDataSeq;	/* 送信データシーケンス番号 */
	int			m_recvDataSeq;	/* 受信済みデータシーケンス番号 */
								/* connectのたびにリセットする */
								/* この値以下のデータは受信済なので受け取らない */

	CRITICAL_SECTION	m_csKey;
	CRITICAL_SECTION	m_csNode;
	CRITICAL_SECTION	m_csWatch;

	char		m_errMsg[1024];
	int			m_errMsgTime;

	// for watch client
	bool		m_watch;				// 観戦クライアントとして動作中
	bool		m_1stCaster;			// 一次配信者か？
	bool		m_watchRecvComplete;	// データ受信が完了しているか？
	char		m_watchRootName[2][30];	// 配信元の名前
	sockaddr_in	m_watchRootAddr[2];		// 配信元のアドレス
	int			m_watchRootGameCount[2];// 配信元の試合ID
	int			m_watchRecvSize;		// 受信済データサイズ
	DWORD		m_lastWatchDataTime;	// 最後にPacket_WatchDataを受け取った時間
	int			m_totalGalleryCount;	// 総合観戦者数（観戦中に配信者が対戦終了するとそのまま更新されなくなるが、それは仕様とする）
	int			m_watchFailCount;		// 連続で自動観戦要求に失敗した回数
	sockaddr_in	m_watchParentAddr;		// 親のアドレス

	// 圧縮データのサポート（受信データサイズが128以上だったら圧縮データを使用する）
	char*		m_watchRecvCompData;	// すべて受信し終わるまで圧縮データを保持しておく
	int			m_watchRecvCompSize;	// 受信済み圧縮データのサイズ

	// for watch server
	CWatcher	m_watcher[WATCH_MAX_CHILD];
	int			m_recvGalleryCount;		// 相手側の観戦者数
};

// extern
extern CNetMgr* g_netMgr;
