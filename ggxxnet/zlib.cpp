/* for Visual Studio 8.0 */
#ifdef _MSC_VER
	#if (_MSC_VER >= 1400)
		#define POINTER_64 __ptr64
	#endif
#endif

//******************************************************************
// include
//******************************************************************
#include "zlib.h"

//******************************************************************
// function
//******************************************************************
int zfsize(FILE* p_fp)
{
	/* 展開後のサイズ */
	int size = 0;
	if (p_fp)
	{
		fseek(p_fp, 4, SEEK_SET);
		fread(&size, 1, 4, p_fp);
	}
	return size;
}

int zfsig(FILE* p_fp)
{
	int sig = 0;
	if (p_fp)
	{
		fseek(p_fp, 0, SEEK_SET);
		fread(&sig, 1, 4, p_fp);
	}
	return sig;
}

void zfread(char* p_ptr, int p_size, FILE* p_fp)
{
	if (p_ptr && p_fp)
	{
		const int	bufsize = 1024 * 512; // 512kb
		char		buf[bufsize];

		fseek(p_fp, 8, SEEK_SET);

		z_stream z;
		
		z.zalloc = Z_NULL;
		z.zfree  = Z_NULL;
		z.opaque = Z_NULL;
		
		z.next_in	= NULL;
		z.avail_in	= 0;
		if (inflateInit(&z) != Z_OK && z.msg) OutputDebugString(z.msg);
		z.next_out	= (Bytef*)p_ptr;
		z.avail_out = p_size;
		
		int status = Z_OK;
		while (status != Z_STREAM_END)
		{
			if (z.avail_in == 0)
			{
				z.next_in  = (Bytef*)buf;
				z.avail_in = fread(buf, 1, bufsize, p_fp);
			}

			status = inflate(&z, Z_FINISH);
			if (status < Z_OK && z.msg) OutputDebugString(z.msg);
			if (status == Z_BUF_ERROR) break;
		}

		if (inflateEnd(&z) != Z_OK && z.msg) OutputDebugString(z.msg);
	}
}

void zfwrite(char* p_ptr, int p_size, FILE* p_fp, int p_sig)
{
	if (p_ptr && p_fp)
	{
		const int	bufsize = 1024 * 512; // 512kb
		char		buf[bufsize];

		z_stream z;
		
		z.zalloc = Z_NULL;
		z.zfree  = Z_NULL;
		z.opaque = Z_NULL;
		
		if (deflateInit(&z, Z_DEFAULT_COMPRESSION) != Z_OK && z.msg) OutputDebugString(z.msg);

		z.next_in	= (Bytef*)p_ptr;
		z.avail_in	= p_size;
		z.next_out	= (Bytef*)buf;
		z.avail_out = bufsize;
		
		/* 識別子 */
		int fsig = p_sig;
		fwrite(&fsig, 1, 4, p_fp);

		/* 元のサイズ */
		fwrite(&p_size, 1, 4, p_fp);
		
		int status = Z_OK;
		while (status != Z_STREAM_END)
		{
			status = deflate(&z, Z_FINISH);
			if (status < Z_OK && z.msg) OutputDebugString(z.msg);
			
			if (z.avail_out == 0 || status == Z_STREAM_END)
			{
				fwrite(buf, 1, bufsize - z.avail_out, p_fp);
				z.next_out = (Bytef*)buf;
				z.avail_out = bufsize;
			}
		}
		
		if (deflateEnd(&z) != Z_OK && z.msg) OutputDebugString(z.msg);
	}
}

int zmsize(char* p_inptr)
{
	/* 展開後のサイズ */
	int size = 0;
	if (p_inptr)
	{
		size = *((DWORD*)p_inptr);
	}
	return size;
}

int zmread(char* p_inptr, int p_insize, char* p_outptr, int p_outsize)
{
	if (p_inptr && p_outptr)
	{
		z_stream z;
		
		z.zalloc = Z_NULL;
		z.zfree  = Z_NULL;
		z.opaque = Z_NULL;
		
		if (inflateInit(&z) != Z_OK && z.msg) OutputDebugString(z.msg);

		z.next_in	= (Bytef*)(p_inptr + 4);
		z.avail_in	= p_insize - 4;
		z.next_out	= (Bytef*)p_outptr;
		z.avail_out = p_outsize;
		
		int status = Z_OK;
		while (status != Z_STREAM_END)
		{
			status = inflate(&z, Z_FINISH);
			if (status < Z_OK && z.msg) OutputDebugString(z.msg);
			if (status == Z_BUF_ERROR && z.avail_in == 0) break;
			if (status == Z_BUF_ERROR && z.avail_out == 0) break;
			if (status == Z_DATA_ERROR) break;
		}

		if (inflateEnd(&z) != Z_OK && z.msg) OutputDebugString(z.msg);

		return (int)p_outsize - (int)z.avail_out;
	}
	return 0;
}

int zmwrite(char* p_inptr, int p_insize, char* p_outptr, int p_outsize)
{
	char* dp = p_outptr;

	if (p_inptr && p_outptr)
	{
		z_stream z;
		
		z.zalloc = Z_NULL;
		z.zfree  = Z_NULL;
		z.opaque = Z_NULL;
		
		if (deflateInit(&z, Z_DEFAULT_COMPRESSION) != Z_OK && z.msg) OutputDebugString(z.msg);

		z.next_in	= (Bytef*)p_inptr;
		z.avail_in	= p_insize;
		z.next_out	= (Bytef*)(p_outptr + 4);
		z.avail_out = p_outsize - 4;

		/* 元のサイズを先頭に記録 */
		*((DWORD*)p_outptr) = p_insize;

		int status = Z_OK;
		while (status == Z_OK)
		{
			status = deflate(&z, Z_FINISH);
			if (status < Z_OK && z.msg) OutputDebugString(z.msg);
			if (status == Z_BUF_ERROR) break; // 出力バッファが足りない
		}
		
		if (deflateEnd(&z) != Z_OK && z.msg) OutputDebugString(z.msg);

		return (int)p_outsize - (int)z.avail_out + 4;
	}
	return 0;
}
