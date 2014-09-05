/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

//******************************************************************
// include
//******************************************************************
#include "md5.h"

//******************************************************************
// function
//******************************************************************
bool calcMD5(BYTE* p_in, int p_len, int p_size, DWORD* p_wk)
{
	#define rol(x, n) ((x) << n | (x) >> (32-n))

	#define F(x,y,z)				(x & y | ~x & z)
	#define G(x,y,z)				(x & z | y & ~z)
	#define H(x,y,z)				(x ^ y ^ z)
	#define I(x,y,z)				(y ^ (x | ~z))
	#define func(a,b,c,d,k,s,t,f)	(a = b + rol(a + f(b,c,d) + x[k] + t, s))

	bool result = false;

	if (p_len < 56)
	{
		// padding
		memset((void*)&p_in[p_len], 0, 56 - p_len);
		if (p_size % 64 < 56) p_in[p_len] = 0x80;

		// size
		int bitlen = p_size * 8;
		memcpy((void*)&p_in[56], &bitlen, 4);
		memset((void*)&p_in[60], 0, 4);

		result = true;
	}
	else if (p_len < 64)
	{
		// padding
		p_in[p_len] = 0x80;
		memset((void*)&p_in[p_len+1], 0, 63 - p_len);
	}
	
	DWORD a = p_wk[0];
	DWORD b = p_wk[1];
	DWORD c = p_wk[2];
	DWORD d = p_wk[3];

	DWORD*	x = (DWORD*)p_in;

	// round1
	func(a,b,c,d, 0, 7,0xd76aa478,F);
	func(d,a,b,c, 1,12,0xe8c7b756,F);
	func(c,d,a,b, 2,17,0x242070db,F);
	func(b,c,d,a, 3,22,0xc1bdceee,F);
	func(a,b,c,d, 4, 7,0xf57c0faf,F);
	func(d,a,b,c, 5,12,0x4787c62a,F);
	func(c,d,a,b, 6,17,0xa8304613,F);
	func(b,c,d,a, 7,22,0xfd469501,F);
	func(a,b,c,d, 8, 7,0x698098d8,F);
	func(d,a,b,c, 9,12,0x8b44f7af,F);
	func(c,d,a,b,10,17,0xffff5bb1,F);
	func(b,c,d,a,11,22,0x895cd7be,F);
	func(a,b,c,d,12, 7,0x6b901122,F);
	func(d,a,b,c,13,12,0xfd987193,F);
	func(c,d,a,b,14,17,0xa679438e,F);
	func(b,c,d,a,15,22,0x49b40821,F);

	// round2
	func(a,b,c,d, 1, 5,0xf61e2562,G);
	func(d,a,b,c, 6, 9,0xc040b340,G);
	func(c,d,a,b,11,14,0x265e5a51,G);
	func(b,c,d,a, 0,20,0xe9b6c7aa,G);
	func(a,b,c,d, 5, 5,0xd62f105d,G);
	func(d,a,b,c,10, 9,0x02441453,G);
	func(c,d,a,b,15,14,0xd8a1e681,G);
	func(b,c,d,a, 4,20,0xe7d3fbc8,G);
	func(a,b,c,d, 9, 5,0x21e1cde6,G);
	func(d,a,b,c,14, 9,0xc33707d6,G);
	func(c,d,a,b, 3,14,0xf4d50d87,G);
	func(b,c,d,a, 8,20,0x455a14ed,G);
	func(a,b,c,d,13, 5,0xa9e3e905,G);
	func(d,a,b,c, 2, 9,0xfcefa3f8,G);
	func(c,d,a,b, 7,14,0x676f02d9,G);
	func(b,c,d,a,12,20,0x8d2a4c8a,G);

	// round3
	func(a,b,c,d, 5, 4,0xfffa3942,H);
	func(d,a,b,c, 8,11,0x8771f681,H);
	func(c,d,a,b,11,16,0x6d9d6122,H);
	func(b,c,d,a,14,23,0xfde5380c,H);
	func(a,b,c,d, 1, 4,0xa4beea44,H);
	func(d,a,b,c, 4,11,0x4bdecfa9,H);
	func(c,d,a,b, 7,16,0xf6bb4b60,H);
	func(b,c,d,a,10,23,0xbebfbc70,H);
	func(a,b,c,d,13, 4,0x289b7ec6,H);
	func(d,a,b,c, 0,11,0xeaa127fa,H);
	func(c,d,a,b, 3,16,0xd4ef3085,H);
	func(b,c,d,a, 6,23,0x04881d05,H);
	func(a,b,c,d, 9, 4,0xd9d4d039,H);
	func(d,a,b,c,12,11,0xe6db99e5,H);
	func(c,d,a,b,15,16,0x1fa27cf8,H);
	func(b,c,d,a, 2,23,0xc4ac5665,H);

	// round4
	func(a,b,c,d, 0, 6,0xf4292244,I);
	func(d,a,b,c, 7,10,0x432aff97,I);
	func(c,d,a,b,14,15,0xab9423a7,I);
	func(b,c,d,a, 5,21,0xfc93a039,I);
	func(a,b,c,d,12, 6,0x655b59c3,I);
	func(d,a,b,c, 3,10,0x8f0ccc92,I);
	func(c,d,a,b,10,15,0xffeff47d,I);
	func(b,c,d,a, 1,21,0x85845dd1,I);
	func(a,b,c,d, 8, 6,0x6fa87e4f,I);
	func(d,a,b,c,15,10,0xfe2ce6e0,I);
	func(c,d,a,b, 6,15,0xa3014314,I);
	func(b,c,d,a,13,21,0x4e0811a1,I);
	func(a,b,c,d, 4, 6,0xf7537e82,I);
	func(d,a,b,c,11,10,0xbd3af235,I);
	func(c,d,a,b, 2,15,0x2ad7d2bb,I);
	func(b,c,d,a, 9,21,0xeb86d391,I);

	memset(p_in, 0, 64);

	p_wk[0] += a;
	p_wk[1] += b;
	p_wk[2] += c;
	p_wk[3] += d;
	
	return result;
}

void getMD5(BYTE* p_buf, int p_size, BYTE* p_out)
{
	DWORD	wk[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };
	BYTE	buf[64];

	int pos = 0;
	while (pos < p_size)
	{
		int size = p_size - pos;
		if (size > 64) size = 64;

		memcpy(buf, &p_buf[pos], size);

		if (calcMD5(buf, size, size + pos, wk))
		{
			break;
		}
		pos += size;
	}

	//sprintf_s((char*)p_out, 32,
	sprintf((char*)p_out,
		"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
		(BYTE)(wk[0]		& 0xff),
		(BYTE)(wk[0] >> 8	& 0xff),
		(BYTE)(wk[0] >> 16	& 0xff),
		(BYTE)(wk[0] >> 24	& 0xff),
		(BYTE)(wk[1]		& 0xff),
		(BYTE)(wk[1] >> 8	& 0xff),
		(BYTE)(wk[1] >> 16	& 0xff),
		(BYTE)(wk[1] >> 24	& 0xff),
		(BYTE)(wk[2]		& 0xff),
		(BYTE)(wk[2] >> 8	& 0xff),
		(BYTE)(wk[2] >> 16	& 0xff),
		(BYTE)(wk[2] >> 24	& 0xff),
		(BYTE)(wk[3]		& 0xff),
		(BYTE)(wk[3] >> 8	& 0xff),
		(BYTE)(wk[3] >> 16	& 0xff),
		(BYTE)(wk[3] >> 24	& 0xff));
}

void getFileMD5(char* p_fname, BYTE* p_out)
{
	DWORD	wk[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };
	BYTE	buf[64];
	size_t	size = 0;

	FILE*	fp = fopen(p_fname, "rb");
	while (fp)
	{
		size_t actsz = fread(buf, 1, 64, fp);
		
		if (calcMD5(buf, actsz, size += actsz, wk))
		{
			break;
		}
	}

	sprintf((char*)p_out,
		"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
		(BYTE)(wk[0]		& 0xff),
		(BYTE)(wk[0] >> 8	& 0xff),
		(BYTE)(wk[0] >> 16	& 0xff),
		(BYTE)(wk[0] >> 24	& 0xff),
		(BYTE)(wk[1]		& 0xff),
		(BYTE)(wk[1] >> 8	& 0xff),
		(BYTE)(wk[1] >> 16	& 0xff),
		(BYTE)(wk[1] >> 24	& 0xff),
		(BYTE)(wk[2]		& 0xff),
		(BYTE)(wk[2] >> 8	& 0xff),
		(BYTE)(wk[2] >> 16	& 0xff),
		(BYTE)(wk[2] >> 24	& 0xff),
		(BYTE)(wk[3]		& 0xff),
		(BYTE)(wk[3] >> 8	& 0xff),
		(BYTE)(wk[3] >> 16	& 0xff),
		(BYTE)(wk[3] >> 24	& 0xff));

	if (fp) fclose(fp);
}
