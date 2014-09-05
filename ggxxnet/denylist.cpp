/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

//******************************************************************
// include
//******************************************************************
#include <string.h>
#include <stdio.h>

#include "ggxxnet.h"
#include "denylist.h"
#include "rc4.h"
#include "util.h"

//******************************************************************
// global
//******************************************************************
CDenyListMgr* g_denyListMgr = NULL;

//******************************************************************
// function
//******************************************************************
CDenyEntry::CDenyEntry(char* p_name, char* p_id)
{
	__strncpy(m_name, p_name, 29);
	memcpy(m_id, p_id, 10);
}

CDenyListMgr::CDenyListMgr(void)
{
}

CDenyListMgr::~CDenyListMgr(void)
{
	deleteAll();
}

void CDenyListMgr::add(char* p_name, char* p_id)
{
	int idx = find(p_id);
	if (idx == -1)
	{
		m_list.add(new CDenyEntry(p_name, p_id));
	}
}

void CDenyListMgr::remove(char* p_mac)
{
	int idx = find(p_mac);
	if (idx != -1)
	{
		delete m_list[idx];
		m_list.remove(idx);
	}
}

int CDenyListMgr::find(char* p_id)
{
	if (idcmp((BYTE*)p_id, INVALID_MID)) return -2;

	for (int i = 0; i < m_list.size(); i++)
	{
		if (*(DWORD*)&p_id[6] == 0xffffffff)
		{
			// 拡張コードが不完全なら６バイトだけで比較する
			if (memcmp((BYTE*)m_list[i]->m_id, (BYTE*)p_id, 6) == 0) return i;
		}
		else
		{
			if (idcmp((BYTE*)m_list[i]->m_id, (BYTE*)p_id)) return i;
		}
	}
	return -1;
}

void CDenyListMgr::deleteAll(void)
{
	m_list.deleteAll();
}

void CDenyListMgr::readfile(void)
{
	FILE *fp = fopen("denylist.txt", "r");
	if (fp) 
	{
		char str[1024];
		char* p = fgets(str, 1024, fp);
		while (p)
		{
			if (str[strlen(str)-1] == '\n') str[strlen(str)-1] = '\0';
			if (str[0] != '\0')
			{
				char* sharp = strchr(str, '#');
				
				char data[10];
				if (sharp - str >= 10)
				{
					str2hex(data, 10, str);
				}
				else
				{
					memcpy(data, INVALID_MID, sizeof(INVALID_MID));
				}

				if (sharp)	add(sharp + 1, data);
				else		add(UNKNOWN_NAME, data);
			}
			p = fgets(str, 1024, fp);
		}
		fclose(fp);
	}
}

void CDenyListMgr::savefile(void)
{
	FILE *fp = fopen("denylist.txt", "w");
	if (fp) 
	{
		for (int i = 0; i < m_list.size(); i++)
		{
			if (idcmp((BYTE*)m_list[i]->m_id, INVALID_MID))
			{
				fprintf(fp, "#%s\n", m_list[i]->m_name);
			}
			else
			{
				char str[21];
				hex2str(m_list[i]->m_id, 10, str);
				fprintf(fp, "%s\t#%s\n", str, m_list[i]->m_name);
			}
		}
		fclose(fp);
	}
}
