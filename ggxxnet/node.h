#pragma once

//******************************************************************
// include
//******************************************************************
#include "dynArray.h"
#include "ggxxnet.h"

//******************************************************************
// class
//******************************************************************
class CNode
{
public:
	CNode(void) {}
	CNode(CNode& p_org)
	{
		*this = p_org;
	}
	~CNode(void)
	{
		m_tmpaddr.deleteAll();
	}	/* メモリリークになる */

	bool isNetSpeedGood(void);
	int  getPingAv(void);
	void recordPing(void);
	void clearPing(void);

	void operator=(CNode& p_org)
	{
		memcpy(m_name, p_org.m_name, 30);
		memcpy(m_addr, p_org.m_addr, 32);
		memcpy(m_ping, p_org.m_ping, sizeof(int) * 10);
		m_state		= p_org.m_state;
		m_rank		= p_org.m_rank;
		m_delay		= p_org.m_delay;
		m_ex		= p_org.m_ex;
		m_win		= p_org.m_win;
		m_ignoreSlow= p_org.m_ignoreSlow;
		m_round		= p_org.m_round;
		memcpy(m_ver, p_org.m_ver, 10);
		memcpy(m_id, p_org.m_id, 10);
		memcpy(m_msg, p_org.m_msg, 256);

		memcpy(m_battleInfoName[0], p_org.m_battleInfoName[0], 30);
		memcpy(m_battleInfoName[1], p_org.m_battleInfoName[1], 30);
		m_battleInfoIP[0] = p_org.m_battleInfoIP[0];
		m_battleInfoIP[1] = p_org.m_battleInfoIP[1];
		m_battleInfoGameCount[0] = p_org.m_battleInfoGameCount[0];
		m_battleInfoGameCount[1] = p_org.m_battleInfoGameCount[1];
		m_battleInfoChara[0] = p_org.m_battleInfoChara[0];
		m_battleInfoChara[1] = p_org.m_battleInfoChara[1];

		m_deny		= p_org.m_deny;
		m_needDetail= p_org.m_needDetail;
		m_gamecount	= p_org.m_gamecount;
		m_uhpNodeIdx= p_org.m_uhpNodeIdx;

		m_allowIntrusion	= p_org.m_allowIntrusion;
		m_watchMaxNode		= p_org.m_watchMaxNode;
		m_galleryCount		= p_org.m_galleryCount;
		m_galleryUpdateTime = p_org.m_galleryUpdateTime;

		m_sendpingtime		= p_org.m_sendpingtime;
		m_scriptCode		= p_org.m_scriptCode;

		m_tmpaddr.deleteAll();
		for (int i = 0; i < p_org.m_tmpaddr.size(); i++)
		{
			m_tmpaddr.add(new char[32]);
			memcpy(m_tmpaddr[i], p_org.m_tmpaddr[i], 32);
		}
		m_validInfo = p_org.m_validInfo;
	}

	bool isExistBattleInfo(void) { return m_battleInfoChara[0] != -1; }
	void clearBattleInfo(void)
	{
		m_battleInfoName[0][0] = '\0';
		m_battleInfoName[1][0] = '\0';
		m_battleInfoIP[0] = NULL;
		m_battleInfoIP[1] = NULL;
		m_battleInfoGameCount[0] = 0;
		m_battleInfoGameCount[1] = 0;
		m_battleInfoChara[0] = -1;
		m_battleInfoChara[1] = -1;
	}

public:
	char	m_name[30];
	char	m_addr[32];
	int		m_ping[10];		/* 過去１０回のPING平均で回線の状態を判断する */
	int		m_state;
	char	m_rank;
	char	m_delay;
	char	m_ex;
	short	m_win;
	char	m_ignoreSlow;
	char	m_round;
	char	m_ver[10];
	int		m_idsize;
	char	m_id[10];
	char	m_msg[256];
	bool	m_deny;			/* このノードが自分を拒否していることを表す */
	bool	m_needDetail;	/* pingReplyでコメントなどの詳細を要求 */
	int		m_gamecount;	/* 観戦の試合識別用にも利用 */
	int		m_uhpNodeIdx;	/* アドレス拡散を要求するノードインデックス */

	bool	m_allowIntrusion;		/* 乱入許可 */
	char	m_watchMaxNode;			/* 直接接続許可するノード数 */
	int		m_galleryCount;			/* 観戦者数 */
	DWORD	m_galleryUpdateTime;	/* m_galleryCountを更新した時間 */

	/* 対戦情報 */
	char	m_battleInfoName[2][30];
	DWORD	m_battleInfoIP[2];
	DWORD	m_battleInfoGameCount[2];
	char	m_battleInfoChara[2];

	DWORD	m_sendpingtime;	/* pingを送った時間 */
	DWORD	m_scriptCode;
	
	/* UDP Hole Punchingのために複数のポートをまとめて管理できる構造が必要になった */
	/* Noresへのping送信にはm_tmpaddrをすべて使用し、それ以外には通常通りm_addrを使用する。 */
	
	TDynArray<char*>	m_tmpaddr;	/* 接続を確立するまでの仮アドレス */

	// 取得済みの情報はフラグを立てる EValidFlagを組み合わせ
	DWORD	m_validInfo;
};

class CNodeMgr
{
public:
	CNodeMgr(void);
	~CNodeMgr(void);
	
	CNode*	getNode(int p_idx) { return &m_nodeList[p_idx]; }
	int		getNodeCount(void) { return m_nodeList.size(); }
	
	int		addNode(char* p_addr, char* p_name, bool p_busy, bool p_needUHP);

	void	removeNode(int p_idx);

	void	removeAllNode(void);
	int		findNodeIdx_address(char* p_addr);
	int		findNodeIdx_name_ip(char* p_name, DWORD p_ip);

	void	sortNodeList(int p_type);

	static void getNodeInfoFromString(char* p_str, char* p_ip, int* p_port);
	
	char*	getOwnNode(void) { return m_ownNode; }
	void	setOwnNode(char* p_str) { __strncpy(m_ownNode, p_str, 31); }

private:
	TDynArray<CNode>	m_nodeList;
	char			m_ownNode[32];
};

//******************************************************************
// extern
//******************************************************************
extern CNodeMgr* g_nodeMgr;
