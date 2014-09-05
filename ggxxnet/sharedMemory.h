#pragma once

//******************************************************************
// class
//******************************************************************
class CSharedMemory
{
public:
	CSharedMemory(char* p_name, int p_size);
	~CSharedMemory(void);

	void get(void* p_ptr, int p_offset, int p_size);
	void set(void* p_ptr, int p_offset, int p_size);

private:
	HANDLE	m_mapping;
	void*	m_mapview;
	HANDLE	m_mutex;
	char	m_mutexname[256];
};
