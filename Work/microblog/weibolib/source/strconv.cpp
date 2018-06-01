#include "strconv.h"


#if ( defined(WIN32) || defined(_WIN32) )

	#include <windows.h>

#else

	#include "bbiconv.h"
	#include <string.h>

	#if ( defined(__ARM_D6446__) || defined(_NETRA600_) )

		extern "C" int Unicode2Gb(unsigned short wUnicodeChar, char achGbcodeChar[]);

	#endif

#endif //(defined(WIN32) || defined(_WIN32) )

#ifdef __cplusplus
extern "C" {
#endif


	/*
	** Convert an ansi string to microsoft unicode, based on the
	** current codepage settings for file apis.
	** 
	** Space to hold the returned string is obtained
	** from malloc.
	*/
	int lo_C2W(wchar_t** pout ,const char *zinname)
	{
#if (defined(WIN32) || defined(_WIN32) )
		wchar_t *zMbcsname = 0;
		int codepage = 0;
		int nByte = 0;

		if( !zinname || * zinname == '\0' )
			return 0;

#if defined(_WIN32_WCE)
		codepage =  CP_ACP ;
#else
		codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
#endif
		nByte = MultiByteToWideChar(codepage, 0, zinname, -1, NULL,0);
		zMbcsname = (wchar_t*)malloc( sizeof(wchar_t)*(nByte+1) );
		if( zMbcsname == 0 )
			return 0;

		nByte = MultiByteToWideChar(codepage, 0, zinname, -1, zMbcsname, nByte);
		if( nByte > 0 )
		{
			zMbcsname[nByte] = 0;
		}
		*pout = zMbcsname;
		return nByte;
#else
		const char* in  = zinname;
		size_t in_len = strlen(in);
		size_t outlen = bbiconv_counts_gbk_2_wchar(in , in_len);
		if( outlen <= 0 )
			return -1;

		wchar_t* out_ptr = (wchar_t*)malloc( (sizeof(wchar_t)) * (outlen+outlen+sizeof(wchar_t)) );
		if( !out_ptr )
			return -1;

		bbiconv_gbk_2_wchar(in , in_len , out_ptr , outlen);
		*pout = out_ptr;
		return outlen;
#endif //

	}

	/*
	** Convert microsoft unicode to multibyte character string, based on the
	** user's Ansi codepage.
	**
	** Space to hold the returned string is obtained from
	** malloc().
	*/
	int lo_W2C(char** pout ,const wchar_t *zWide)
	{
#if (defined(WIN32) || defined(_WIN32) )
		char *zname = 0;
		int  codepage = 0;
		int  nByte = 0;

		if( !zWide || *zWide == '\0' )
			return 0;

#if defined(_WIN32_WCE)
		codepage =  CP_ACP ;
#else
		codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
#endif
		nByte = WideCharToMultiByte(codepage, 0, zWide, -1, 0, 0, 0, 0);
		zname = (char*)malloc( nByte + 1 );
		if( zname == 0 ) return 0;

		nByte = WideCharToMultiByte(codepage, 0, zWide, -1, zname, nByte+1, 0, 0);
		if( nByte > 0 )	zname[nByte] = '\0';

		*pout = zname;
		return nByte;
#else

		const wchar_t* in  = zWide;
		size_t in_len = wcslen(in);
		size_t outlen = bbiconv_counts_wchar_2_gbk(in , in_len);
		if( outlen <= 0 )
			return -1;

		char* out_ptr = (char*)malloc( (sizeof(char)) * (outlen+1) );
		if( !out_ptr )
			return -1;

		bbiconv_wchar_2_gbk(in , in_len , out_ptr , outlen);
		*pout = out_ptr;
		return outlen;

#endif //#if (defined(WIN32) || defined(_WIN32) )

	}

	int lo_W2Utf8(char** pout , const wchar_t *zWide)
	{
#if (defined(WIN32) || defined(_WIN32) )
		int nByte = 0;
		char *zname = 0;

		if( !zWide || *zWide == '\0' )
			return 0;

		nByte = WideCharToMultiByte(CP_UTF8, 0, zWide, -1, 0, 0, 0, 0);
		zname = (char*)malloc( nByte +1 );
		if( zname == 0 )
		{
			return 0;
		}
		nByte = WideCharToMultiByte(CP_UTF8, 0, zWide, -1, zname, nByte+1, 	0, 0);
		if( nByte > 0 )
		{
			zname[nByte] = '\0';
		}
		*pout = zname ;
		return nByte;
#else

		const wchar_t* in  = zWide;
		size_t in_len = wcslen(in);
		size_t outlen = bbiconv_counts_wchar_2_utf8(in , in_len);
		if( outlen <= 0 )
			return -1;

		char* out_ptr = (char*)malloc( (sizeof(char)) * (outlen+1) );
		if( !out_ptr )
			return -1;

		bbiconv_wchar_2_utf8(in , in_len , out_ptr , outlen);
		*pout = out_ptr;
		return outlen;

#endif //#if (defined(WIN32) || defined(_WIN32) )
	}
		
	int lo_Utf82W(wchar_t** pout ,const char* utf8 ) 
	{
#if (defined(WIN32) || defined(_WIN32) )

		int nByte = 0;
		wchar_t *zwstr = 0;

		if( !utf8 )
			return 0;

		nByte = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		zwstr = (wchar_t*)malloc( (nByte+1)*sizeof(wchar_t) );
		if( zwstr == 0 ){
			return 0;
		}
		nByte = MultiByteToWideChar(CP_UTF8, 0, utf8 , -1 , zwstr, nByte);
		if( nByte > 0 )
		{
			zwstr[nByte] = 0;
		}
		*pout = zwstr;
		return nByte;

#else
		const char* in  = utf8;
		size_t in_len = strlen(in);
		size_t outlen = bbiconv_counts_utf8_2_wchar(in , in_len);
		if( outlen <= 0 )
			return -1;

		wchar_t* out_ptr = (wchar_t*)malloc( outlen+sizeof(wchar_t) );
		if( !out_ptr )
			return -1;

		bbiconv_utf8_2_wchar(in , in_len , out_ptr , outlen);
		*pout = out_ptr;
		return outlen;

#endif //
	}

	/*
	** Convert multibyte character string to UTF-8.  Space to hold the
	** returned string is obtained from malloc().
	*/
	int lo_C2Utf8(char** pout , const char *zinname)
	{
#if (defined(WIN32) || defined(_WIN32) )
		wchar_t *zTmp = 0;
		int outlen = 0;
		
		outlen = lo_C2W(&zTmp , zinname);
		if( !zTmp || outlen <= 0 )
		{
			return 0;
		}

		outlen = lo_W2Utf8(pout , zTmp);
		free( zTmp );
		return outlen ;
#else
		const char* in  = zinname;
		size_t in_len = strlen(in);
		size_t outlen = bbiconv_counts_gbk_2_utf8(in , in_len);
		if( outlen <= 0 )
			return -1;

		char* out_ptr = (char*)malloc( outlen+sizeof(char) );
		if( !out_ptr )
			return -1;

		bbiconv_gbk_2_utf8(in , in_len , out_ptr , outlen);
		*pout = out_ptr;
		return outlen;

#endif //#if (defined(WIN32) || defined(_WIN32) )

	}

	/*
	** Convert UTF-8 to multibyte character string.  Space to hold the 
	** returned string is obtained from sqliteMalloc().
	*/
	int lo_Utf82C(char** pout ,const char *zstr)
	{

#if (defined(WIN32) || defined(_WIN32) )

		wchar_t* pwout = 0;
		int outlen = 0;
		if( 0 == lo_Utf82W(&pwout , zstr) )
			return 0;

		outlen = lo_W2C(pout , pwout );
		free( pwout );
		return outlen;

#else

	#if ( defined(__ARM_D6446__) || defined(_NETRA600_) )

		int outlen = 0;
		size_t in_len = strlen(zstr);          // UTF-8字符串长度
		size_t uni_len = in_len * 2 + 100;     // 预分配Unicode字符串长度
		size_t gb2312_len = in_len * 2 + 100;  // 预分配GB2312字符串长度

		// 分配存放Unicode编码字符串的bug
		char* uni_ptr = (char *)malloc( uni_len );
		if ( uni_ptr == NULL )
		{
			printf("lo_Utf82C ==>  malloc Unicode buf failed!\n");
			return -1;
		}
		memset(uni_ptr, 0, uni_len);

		// 分配存放GB2312编码字符串的bug
		char* gb2312_ptr = (char *)malloc( gb2312_len );
		if ( gb2312_ptr == NULL )
		{
			printf("lo_Utf82C ==>  malloc gb2312 buf failed!\n");
			return -1;
		}
		memset(gb2312_ptr, 0, gb2312_len);

		int nCurLen = 0;
		int nCharLen = 0;
		char* pchCurGb = NULL;
		unsigned short* pwChar = NULL;
		unsigned short wCurUnicode = 0;
		unsigned char abyGbChar[2] = {0};

		pwChar = (unsigned short *)uni_ptr;    
		pchCurGb = gb2312_ptr;


		// UTF-8转Unicode
		const char* src = zstr;
		unsigned short *des = (unsigned short *)uni_ptr;

// 		int UTF2Uni(const char* src, unsigned short *des)
		{
			int size_s = strlen(src);
			int s = 0, d = 0;

			while ( s < size_s )
			{
				unsigned char c = src[s];
				if ((c & 0x80) == 0) 
				{
					des[d++] += src[s++];
				} 
				else if((c & 0xE0) == 0xC0)  ///< 110x-xxxx 10xx-xxxx
				{
					unsigned short* wideChar = (des+d);
					*wideChar  = (src[s + 0] & 0x3F) << 6;
					*wideChar |= (src[s + 1] & 0x3F);

					s += 2;
					d++;
				}
				else if((c & 0xF0) == 0xE0)  ///< 1110-xxxx 10xx-xxxx 10xx-xxxx
				{
					unsigned short* wideChar = (des+d);

					*wideChar  = (src[s + 0] & 0x1F) << 12;
					*wideChar |= (src[s + 1] & 0x3F) << 6;
					*wideChar |= (src[s + 2] & 0x3F);

					s += 3;
					d++;
				} 
				else if((c & 0xF8) == 0xF0)  ///< 1111-0xxx 10xx-xxxx 10xx-xxxx 10xx-xxxx 
				{
					unsigned short* wideChar = (des+d);

					*wideChar  = (src[s + 0] & 0x0F) << 18;
					*wideChar  = (src[s + 1] & 0x3F) << 12;
					*wideChar |= (src[s + 2] & 0x3F) << 6;
					*wideChar |= (src[s + 3] & 0x3F);

					s += 4;
					d++;
				} 
				else 
				{
					unsigned short* wideChar = (des+d); ///< 1111-10xx 10xx-xxxx 10xx-xxxx 10xx-xxxx 10xx-xxxx 

					*wideChar  = (src[s + 0] & 0x07) << 24;
					*wideChar  = (src[s + 1] & 0x3F) << 18;
					*wideChar  = (src[s + 2] & 0x3F) << 12;
					*wideChar |= (src[s + 3] & 0x3F) << 6;
					*wideChar |= (src[s + 4] & 0x3F);

					s += 5;
					d++;
				}
			}

			in_len = d*2;
		}

		// Unicode转GB2312
		size_t i = 0;
		for (i=0; i<in_len; i=i+2)
		{
			wCurUnicode = *pwChar;
			nCharLen = Unicode2Gb(wCurUnicode, (char*)abyGbChar);
			if (nCharLen == 2)
			{
				*pchCurGb = abyGbChar[0];
				pchCurGb++;
				*pchCurGb = abyGbChar[1];
				pchCurGb++;
				nCurLen += 2;
			}
			else if (nCharLen == 1)
			{
				*pchCurGb = abyGbChar[0];
				pchCurGb++;
				nCurLen++;
			}
			else
			{
				*pchCurGb = '?';
				pchCurGb++;
				nCurLen++;
			}
			
			pwChar++;
		}

		if ( uni_ptr != NULL )
		{
			free(uni_ptr);
			uni_ptr = NULL;
		}

		*pchCurGb = '\0';
		outlen = nCurLen;

		*pout = gb2312_ptr;
		return outlen;

	#else   // 非ARM+NETRA和WIN的其他平台

		const char* in = zstr;
		size_t in_len = strlen(in);
		size_t outlen = bbiconv_counts_utf8_2_gbk(in, in_len);
		if ( outlen <= 0 )
		{
			printf("bbiconv_counts_utf8_2_gbk  outlen <= 0   Error!\n");
			return -1;
		}

		char* out_ptr = (char *)malloc( outlen + sizeof(char) );
		if ( out_ptr == NULL )
		{
			printf("malloc failed!\n");
			return -1;
		}

		bbiconv_utf8_2_gbk(in, in_len, out_ptr, outlen);
		*pout = out_ptr;
		return outlen;

	#endif

#endif
	}

#ifdef __cplusplus
}
#endif



