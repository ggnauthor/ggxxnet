/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

//******************************************************************
// include
//******************************************************************
#include "cpuid.h"

//******************************************************************
// functions
//******************************************************************
void cpuid(DWORD p_in1, DWORD* p_eax, DWORD* p_ebx, DWORD* p_ecx, DWORD* p_edx)
{
	_asm
	{
		mov eax, p_in1
		cpuid
		mov edi, p_eax
		mov [edi], eax
		mov edi, p_ebx
		mov [edi], ebx
		mov edi, p_ecx
		mov [edi], ecx
		mov edi, p_edx
		mov [edi], edx
	}
}

bool idSupport(DWORD p_id)
{
	DWORD eax = 0, ebx = 0, ecx = 0, edx = 0;
	cpuid(0, &eax, &ebx, &ecx, &edx);
	return eax >= p_id;
}

CCpuID::CCpuID(void)
{
	DWORD supid, dummy;

	cpuid(0, &supid, &dummy, &dummy, &dummy);

	if (supid < 0) m_vendorName[0] = '\0';
	else
	{
		DWORD* dp = (DWORD*)m_vendorName;
		cpuid(0, &dummy, dp, dp+2, dp+1);
		*(m_vendorName+12) = '\0';
	}

	if (supid < 1)
	{
		m_cpu_eax = 0xffffffff;
		m_cpu_edx = 0xffffffff;
		m_cpu_ecx = 0xffffffff;
	}
	else
	{
		cpuid(1, &m_cpu_eax, &dummy, &m_cpu_ecx, &m_cpu_edx);
	}

	// Šg’£–½—ß
	cpuid(0x80000000, &supid, &dummy, &dummy, &dummy);

	if (supid < 0x80000004) m_cpuName[0] = '\0';
	else
	{
		DWORD* dp = (DWORD*)m_cpuName;
		cpuid(0x80000002, dp, dp+1, dp+2, dp+3);
		cpuid(0x80000003, dp+4, dp+5, dp+6, dp+7);
		cpuid(0x80000004, dp+8, dp+9, dp+10, dp+11);
		*(m_cpuName+48) = '\0';
	}
}
