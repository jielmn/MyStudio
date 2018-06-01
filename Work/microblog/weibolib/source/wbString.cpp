/**
*  @brief character encoding conversion.
*  @file  wbString.cpp
**/



#include "wbString.h"
#include "strconv.h"
#include "Urlcode.h"
#include <string.h>
#include <stdlib.h>


#if defined(WIN32) || defined(WINCE)

	#if defined(UNICODE) || defined(_UNICODE)

		const char* UnicodeToMbcs(const wchar_t *zWide , char** outstr)
		{
			char* pout = 0;

			if( lo_W2C(&pout , zWide ) <= 0 )
				return "";

			*outstr = pout;
			return pout;
		}

		const char* UnicodeToUTF8(const wchar_t *zWide , char** outstr )
		{
			char* pout = 0;
			if( lo_W2Utf8(&pout , zWide ) <= 0 )
				return "";

			*outstr = pout;
			return pout;
		}

	#endif // unicode

	const wchar_t* MbcsToUnicode(const char *zinname,wchar_t** outstr)
	{
		wchar_t* pout = 0;
		if( lo_C2W(&pout , zinname ) <= 0 )
			return L"";

		*outstr = pout;
		return pout;
	}

	const char* MbcsToUTF8(const char *zz , char** outstr)
	{
		char* pout = 0;
		if( lo_C2Utf8(&pout , zz ) <= 0 )
			return "";

		*outstr = pout ;
		return pout;
	}

#else // 其它平台

	#if (( defined(__ARM_D6446__) || defined(_NETRA600_)) && !defined(_ANDROID_) )

		#include "bbiconv.h"
		extern "C" int utlChr2UTF8( const char *str, unsigned char *bmpStr );

		const char* MbcsToUTF8(const char *zz, char** outstr)
		{
			size_t in_len  = strlen(zz);
			size_t pout_len = in_len * sizeof(char) * 2;

			char* pout = (char *)malloc( pout_len );
			if ( pout == NULL )
			{
				printf("MbcsToUTF8 ==>  malloc failed!\n");
				return NULL;
			}
			memset( pout, 0, pout_len );

			int UTF8Len = utlChr2UTF8(zz, (unsigned char *)pout);

			printf("【1】pout = %s   UTF8Len = %d\n", pout, UTF8Len);
			pout[UTF8Len] = '\0';
			printf("【2】pout = %s   UTF8Len = %d\n", pout, UTF8Len);

			*outstr = pout;

			return pout;
		}

	#else   // 非ARM+NETRA和WIN的其他平台

		#include "bbiconv.h"

		const char* MbcsToUTF8(const char *zz, char** outstr)
		{
			size_t in_len  = strlen(zz);
			size_t outlen = bbiconv_counts_gbk_2_utf8(zz, in_len);

			char* pout = (char *)malloc( (outlen*sizeof(char)) + sizeof(char) );
			bbiconv_gbk_2_utf8( zz, in_len, pout, outlen );

			*outstr = pout;
			return pout;
		}

	#endif

#endif
