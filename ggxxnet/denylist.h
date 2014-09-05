#pragma once

//******************************************************************
// include
//******************************************************************
#include "dynArray.h"

//******************************************************************
// class
//******************************************************************

class CDenyEntry
{
public:
	CDenyEntry(char* p_name, char* p_mac);

	char m_name[30];
	char m_id[10];
};

class CDenyListMgr
{
public:
	CDenyListMgr(void);
	~CDenyListMgr(void);

	void add(char* p_name, char* p_mac);
	void remove(char* p_mac);
	int  find(char* p_mac);
	void deleteAll(void);
	
	void readfile(void);
	void savefile(void);

private:
	TDynArray<CDenyEntry*>	m_list;
};

//******************************************************************
// extern
//******************************************************************
extern CDenyListMgr* g_denyListMgr;
