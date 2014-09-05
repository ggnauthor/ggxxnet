//******************************************************************
// include
//******************************************************************
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

//******************************************************************
// class
//******************************************************************
class CCpuID
{
public:
	CCpuID(void);
	~CCpuID(void) {}

	const char* getVendorName(void) { return m_vendorName; }
	const char* getCpuName(void) { return m_cpuName; }
	DWORD getEAX(void) { return m_cpu_eax; }
	DWORD getEDX(void) { return m_cpu_edx; }
	DWORD getECX(void) { return m_cpu_ecx; }

private:
	char	m_vendorName[13];
	char	m_cpuName[49];
	DWORD	m_cpu_eax;
	DWORD	m_cpu_edx;
	DWORD	m_cpu_ecx;
};
