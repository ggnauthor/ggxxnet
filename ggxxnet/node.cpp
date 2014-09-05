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
#include "denyList.h"

//******************************************************************
// global
//******************************************************************
CNodeMgr* g_nodeMgr = NULL;

//******************************************************************
// function
//******************************************************************
bool CNode::isNetSpeedGood(void)
{
	int ping = getPingAv();
	if (ping == -1) return false;
	return (ping < g_setting.delay * 33 - 3);
}

int CNode::getPingAv(void)
{
	int pingav = 0;
	int pingcount = 0;
	for (int j = 0; j < 10; j++)
	{
		if (m_ping[j] != -1)
		{
			pingav += m_ping[j];
			pingcount++;
		}
	}

	/* １つとかでは信憑性に欠ける。かといって増やしすぎるとつながらなくなるかも */
	if (pingcount > 0) return pingav / pingcount;
	else return -1;
}

void CNode::recordPing(void)
{
	if (m_sendpingtime != -1)
	{
		DWORD time = timeGetTime() - m_sendpingtime;
		if (time < TIMEOUT_PING)
		{
			for (int j = 8; j >= 0; j--) m_ping[j+1] = m_ping[j];
			m_ping[0] = timeGetTime() - m_sendpingtime;
			m_sendpingtime = -1;
		}
	}
}

void CNode::clearPing(void)
{
	m_sendpingtime = -1;
	for (int i = 0; i < 10; i++) m_ping[i] = -1;
}

CNodeMgr::CNodeMgr(void)
{
	setOwnNode("");
}

CNodeMgr::~CNodeMgr(void)
{
}

int CNodeMgr::addNode(char* p_addr, char* p_name, bool p_busy, bool p_needUHP)
{
	/* 外で排他かけること！！ */

	/* 簡単に書式チェック */
	char* p = p_addr;
	if (strlen(p_addr) == 0)			return -1;
	if ((p = strchr(p+1, '.')) == NULL) return -1;
	if ((p = strchr(p+1, '.')) == NULL) return -1;
	if ((p = strchr(p+1, '.')) == NULL) return -1;
	if ((p = strchr(p+1, ':')) == NULL) return -1;

	// 自分は無視するm_ownNodeが未取得ならやはり無視
	if (useLobbyServer())
	{
		char* pos = strchr(m_ownNode, ':');
		if (m_ownNode[0] == '\0' || strncmp(m_ownNode, p_addr, (int)(pos - p_addr)) == 0) return -1;
	}

	int idx = -1;
	// UNKNOWN_NAMEは無条件に不一致とする
	if (strncmp(p_name, UNKNOWN_NAME, sizeof(UNKNOWN_NAME)) != 0)
	{
		sockaddr_in addr = g_netMgr->getAddrFromString(p_addr);
		idx = findNodeIdx_name_ip(p_name, addr.sin_addr.S_un.S_addr);
	}

	if (idx == -1)
	{
		//DBGOUT_NET("addNode %s:%s\n", p_name, p_addr);

		/* 重複していなければ追加 */
		CNode node;
		__strncpy(node.m_addr, p_addr, 31);
		__strncpy(node.m_name, p_name, 29);
		node.m_state		= p_busy ? State_Busy : State_NoResponse;
		node.m_ex			= 0;
		node.m_rank			= Rank_F;
		node.m_delay		= 0;
		node.m_win			= 0;
		node.m_ignoreSlow	= 1;
		node.m_round		= 0;
		node.m_deny			= true;
		node.m_needDetail	= true;
		node.m_gamecount	= 0;
		node.m_scriptCode	= 0;
		
		node.m_allowIntrusion	= false;
		node.m_watchMaxNode		= -16;
		node.m_galleryCount		= 0;
		node.m_galleryUpdateTime= timeGetTime();
		
		/* 定期的に他のノードにポートの拡散を依頼する */
		node.m_uhpNodeIdx	= 0;

		__strncpy(node.m_ver, "----", 9);
		memcpy(node.m_id, INVALID_MID, sizeof(INVALID_MID));
		memset(node.m_msg, 0, 256);
		node.clearBattleInfo();
		
		node.m_validInfo = 0x00000000;

		node.clearPing();

		m_nodeList.add(node);

		CNode* newnode = &m_nodeList[m_nodeList.size() - 1];
		newnode->m_tmpaddr.add(new char[32]);
		__strncpy(newnode->m_tmpaddr[newnode->m_tmpaddr.size() - 1], p_addr, 31);

		return (m_nodeList.size() - 1);
	}
	else
	{
		if (findNodeIdx_address(p_addr) == -1)
		{
			// ポートを追加
			m_nodeList[idx].m_tmpaddr.add(new char[32]);
			__strncpy(m_nodeList[idx].m_tmpaddr[m_nodeList[idx].m_tmpaddr.size() - 1], p_addr, 31);
			// return idx; //新規追加ではないのでidxを返してはいけない
			//DBGOUT_NET("addNode : multi port %s:%s\n", p_name, p_addr);
		}
		//else DBGOUT_NET("addNode : duplicate error %s:%s\n", p_name, p_addr);
	}

	return -1;
}

void CNodeMgr::removeNode(int p_idx)
{
	/* 外で排他かけること！！ */
	m_nodeList[p_idx].m_tmpaddr.deleteAll();
	m_nodeList.remove(p_idx);
}

void CNodeMgr::removeAllNode(void)
{
	/* 外で排他かけること！！ */
	for (int i = 0; i < m_nodeList.size(); i++) m_nodeList[i].m_tmpaddr.deleteAll();
	m_nodeList.removeAll();
}

int CNodeMgr::findNodeIdx_address(char* p_addr)
{
	/* 外で排他かけること！！ */
	for (int i = 0; i < m_nodeList.size(); i++)
	{
		// 1.15-3よりどれかのtmpaddrと一致していればokとした
		for (int j = 0; j < m_nodeList[i].m_tmpaddr.size(); j++)
		{
			if (strcmp(m_nodeList[i].m_tmpaddr[j], p_addr) == 0) return i;
		}
	}
	return -1;
}

int CNodeMgr::findNodeIdx_name_ip(char* p_name, DWORD p_ip)
{
	int idx = -1;
	for (int i = 0; i < m_nodeList.size(); i++)
	{
		sockaddr_in addr = g_netMgr->getAddrFromString(m_nodeList[i].m_addr);
		if (strncmp(m_nodeList[i].m_name, p_name, 29) == 0 && addr.sin_addr.S_un.S_addr == p_ip)
		{
			idx = i;
			break;
		}
	}
	return idx;
}

/* sortNodeListで使用する比較関数 */
int cmp_name_a(CNode *a, CNode *b) { return strcmp(a->m_name, b->m_name); }

int cmp_name_d(CNode *a, CNode *b) { return -cmp_name_a(a, b); }

int cmp_rank_a(CNode *a, CNode *b)
{
	if ((a->m_validInfo & VF_RANK) == 0) return 1;
	if ((b->m_validInfo & VF_RANK) == 0) return -1;

	return (a->m_rank - b->m_rank);
}

int cmp_rank_d(CNode *a, CNode *b)
{
	if ((a->m_validInfo & VF_RANK) == 0) return 1;
	if ((b->m_validInfo & VF_RANK) == 0) return -1;

	return -(a->m_rank - b->m_rank);
}

int cmp_win_a(CNode *a, CNode *b)
{
	if ((a->m_validInfo & VF_WINS) == 0) return 1;
	if ((b->m_validInfo & VF_WINS) == 0) return -1;

	return (a->m_win - b->m_win);
}

int cmp_win_d(CNode *a, CNode *b)
{
	if ((a->m_validInfo & VF_WINS) == 0) return 1;
	if ((b->m_validInfo & VF_WINS) == 0) return -1;

	return -(a->m_win - b->m_win);
}

int cmp_count_a(CNode *a, CNode *b)
{
	if ((a->m_validInfo & VF_COUNT) == 0) return 1;
	if ((b->m_validInfo & VF_COUNT) == 0) return -1;

	return (a->m_gamecount - b->m_gamecount);
}

int cmp_count_d(CNode *a, CNode *b)
{
	if ((a->m_validInfo & VF_COUNT) == 0) return 1;
	if ((b->m_validInfo & VF_COUNT) == 0) return -1;

	return -(a->m_gamecount - b->m_gamecount);
}

int cmp_ping(CNode *a, CNode *b)
{
	int pinga = a->getPingAv();
	int pingb = b->getPingAv();
	if (pinga == -1) return 1;
	if (pingb == -1) return -1;
	return -(pingb - pinga);
}

int cmp_ex(CNode *a, CNode *b)
{
	if (a->m_validInfo & VF_DENY)
	{
		if (a->m_deny || g_denyListMgr->find(a->m_id) >= 0)	return 1;
	}
	if (b->m_validInfo & VF_DENY)
	{
		if (b->m_deny || g_denyListMgr->find(b->m_id) >= 0)	return -1;
	}

	if ((a->m_validInfo & VF_EX) == 0) return 1;
	if ((b->m_validInfo & VF_EX) == 0) return -1;

	return -(a->m_ex - b->m_ex);
}

int cmp_watch(CNode *a, CNode *b)
{
	if (a->m_validInfo & VF_DENY)
	{
		if (a->m_deny || g_denyListMgr->find(a->m_id) >= 0)	return 1;
	}
	if (b->m_validInfo & VF_DENY)
	{
		if (b->m_deny || g_denyListMgr->find(b->m_id) >= 0)	return -1;
	}

	int scorea = 0;
	int scoreb = 0;

	if (a->m_state == State_Busy_Casting)			scorea += 00000;
	else if (a->m_state == State_Busy_Casting_NG)	scorea +=  5000;
	else if (a->m_state == State_Idle)				scorea += 10000;
	else if (a->m_state == State_Watch_Playable)	scorea += 15000;
	else											scorea += 20000;

	//if (a->m_watchMaxNode == -16) scorea = 30000;
	//if (a->m_validInfo & VF_DENY)
	//{
	//	if (a->m_deny || g_denyListMgr->find(a->m_id) >= 0)	scorea = 40000;
	//}

	if (a->m_state == State_Busy_Casting) scorea -= a->m_galleryCount;

	if (b->m_state == State_Busy_Casting)			scoreb += 00000;
	else if (b->m_state == State_Busy_Casting_NG)	scoreb +=  5000;
	else if (b->m_state == State_Idle)				scoreb += 10000;
	else if (b->m_state == State_Watch_Playable)	scoreb += 15000;
	else											scoreb += 20000;

	//if (b->m_watchMaxNode == -16) scoreb = 30000;
	//if (b->m_validInfo & VF_DENY)
	//{
	//	if (b->m_deny || g_denyListMgr->find(b->m_id) >= 0)	scoreb = 40000;
	//}

	if (b->m_state == State_Busy_Casting) scoreb -= b->m_galleryCount;

	return (scorea - scoreb);
}

int cmp_status(CNode *a, CNode *b)
{
	if (a->m_validInfo & VF_DENY)
	{
		if (a->m_deny || g_denyListMgr->find(a->m_id) >= 0)	return 1;
	}
	if (b->m_validInfo & VF_DENY)
	{
		if (b->m_deny || g_denyListMgr->find(b->m_id) >= 0)	return -1;
	}

	int scorea = 0;
	int scoreb = 0;

	if (a->m_state == State_Idle)					scorea += 00000;
	else if (a->m_state == State_Watch_Playable)	scorea +=  5000;
	else if (a->m_state == State_Busy_Casting)		scorea += 15000;
	else if (a->m_state == State_Busy_Casting_NG)	scorea += 15000;
	else if (a->m_state == State_Busy)				scorea += 20000;
	else if (a->m_state == State_Watch)				scorea += 25000;
	else if (a->m_state == State_NoResponse)		scorea += 30000;
	else if (a->m_state == State_Mismatch)			scorea += 40000;
	else if (a->m_state == State_VersionError)		scorea += 40000;
	else if (a->m_state == State_NotReady)			scorea += 10000;
	else if (a->m_state == State_PingOver)			scorea +=  8000;

	if (b->m_state == State_Idle)					scoreb += 00000;
	else if (b->m_state == State_Watch_Playable)	scoreb +=  5000;
	else if (b->m_state == State_Busy_Casting)		scoreb += 15000;
	else if (b->m_state == State_Busy_Casting_NG)	scoreb += 15000;
	else if (b->m_state == State_Busy)				scoreb += 20000;
	else if (b->m_state == State_Watch)				scoreb += 25000;
	else if (b->m_state == State_NoResponse)		scoreb += 30000;
	else if (b->m_state == State_Mismatch)			scoreb += 40000;
	else if (b->m_state == State_VersionError)		scoreb += 40000;
	else if (b->m_state == State_NotReady)			scoreb += 10000;
	else if (b->m_state == State_PingOver)			scoreb +=  8000;

	/* rank差を影響 */
	if (idcmp((BYTE*)a->m_id, INVALID_MID) == false)
	{
		scorea += abs(g_setting.rank - a->m_rank) * 10;
	}
	else scorea += 10;

	if (idcmp((BYTE*)b->m_id, INVALID_MID) == false)
	{
		scoreb += abs(g_setting.rank - b->m_rank) * 10;
	}
	else scoreb += 10;

	if (a->isNetSpeedGood() == false)
	{
		if (a->m_ignoreSlow || g_setting.ignoreSlow) scorea += 100;
		else scorea += 100;
	}
	if (b->isNetSpeedGood() == false)
	{
		if (b->m_ignoreSlow || g_setting.ignoreSlow) scoreb += 100;
		else scoreb += 100;
	}

	return (scorea - scoreb);
}

int(*g_sortfunc[])(CNode*, CNode*) = {
	cmp_name_a,
	cmp_name_d,
	cmp_rank_a,
	cmp_rank_d,
	cmp_win_a,
	cmp_win_d,
	cmp_count_a,
	cmp_count_d,
	cmp_ping,
	cmp_ex,
	cmp_watch,
	cmp_status,
};

void CNodeMgr::sortNodeList(int p_type)
{
	/* 外で排他かけること！！ */
	int count = getNodeCount();
	if (count > 1)
	{
		CNode*	data = new CNode[count];
		for (int i = 0; i < count; i++) data[i] = *getNode(i);
		
		assert(p_type < SORTTYPECOUNT);
		qsort(data, count, sizeof(CNode), (int(*)(const void*, const void*))g_sortfunc[p_type]);

		for (int i = 0; i < count; i++) m_nodeList.set(i, data[i]);

		delete[] data;
	}
}

void CNodeMgr::getNodeInfoFromString(char* p_str, char* p_ip, int* p_port)
{
	char temp[256];
	strcpy(temp, p_str);
	
	char* p = strchr(temp, ':');
	if (p)
	{
		*p_port = atoi(p + 1);
		*p = '\0';
	}
	__strncpy(p_ip, temp, 31);
}
