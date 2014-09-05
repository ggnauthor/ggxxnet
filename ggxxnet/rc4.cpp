/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

//******************************************************************
// include
//******************************************************************
#include "rc4.h"

//******************************************************************
// function
//******************************************************************
void getRC4(char* p_dst, int p_dsize, char* p_src, int p_ssize, char* p_key, int p_ksize)
{
	char s1[256], s2[256];
	for (int i = 0; i < 256; i++)
	{
		s1[i] = i;
		s2[i] = p_key[i % p_ksize];
	}

	int n = 0;
	for (int i = 0; i < 256; i++)
	{
		n = (n + s1[i] + s2[i]) & 0xFF;
		char tmp = s1[i];
		s1[i] = s1[n];
		s1[n] = tmp;
	}
	
	int a = 0, b = 0;
	for (int i = 0; i < p_dsize && i < p_ssize; i++)
	{
		a = (a + 1) & 0xFF;
		b = (b + s1[a]) & 0xFF;

		char tmp = s1[a];
		s1[a] = s1[b];
		s1[b] = tmp;

		int t = (s1[a] + s1[b]) & 0xFF;
		p_dst[i] = p_src[i] ^ s1[t];
	}
}
