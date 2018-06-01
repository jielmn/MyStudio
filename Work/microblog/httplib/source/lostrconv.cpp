#include "lostrconv.h"

#if (defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE) )

#include <windows.h>


/*
** Convert an ansi string to microsoft unicode, based on the
** current codepage settings for file apis.
** 
** Space to hold the returned string is obtained
** from sqliteMalloc.
*/
std::wstring MbcsToUnicode(const char *zinname)
{
	std::wstring strout;
	if( zinname )
	{
		wchar_t *zMbcsname;
#if defined(_WIN32_WCE)
		int codepage = CP_ACP;
#else
		int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
#endif
		int nByte = MultiByteToWideChar(codepage, 0, zinname, -1, NULL,0);
		zMbcsname = new wchar_t[nByte+1];
		if( zMbcsname==0 ){
			return strout;
		}
		nByte = MultiByteToWideChar(codepage, 0, zinname, -1, zMbcsname, nByte);
		if( nByte > 0 )
		{
			zMbcsname[nByte] = 0;
			strout = zMbcsname;
		}
		delete[] zMbcsname;
	}
	return strout;

}

/*
** Convert microsoft unicode to multibyte character string, based on the
** user's Ansi codepage.
**
** Space to hold the returned string is obtained from
** sqliteMalloc().
*/
std::string UnicodeToMbcs(const wchar_t *zWide)
{
	std::string strout;
	if( zWide )
	{
		int nByte;
		char *zname = 0;
#if defined(_WIN32_WCE)
		int codepage = CP_ACP;
#else
		int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
#endif

		nByte = WideCharToMultiByte(codepage, 0, zWide, -1, 0, 0, 0, 0);
		zname = new char[ nByte+1];
		if( zname==0 ){
			return strout;
		}
		nByte = WideCharToMultiByte(codepage, 0, zWide, -1, zname, nByte+1, 0, 0);
		if( nByte > 0 ){
			zname[nByte]='\0';
			strout = zname;
		}
		delete []zname;
	}
	return strout;
}

std::string UnicodeToUtf8(const wchar_t *zWide)
{
	int nByte;
	char *zname;
	std::string strout;
	if( !zWide )
		return strout;

	nByte = WideCharToMultiByte(CP_UTF8, 0, zWide, -1, 0, 0, 0, 0);
	zname = new char[nByte +1];
	if( zname == 0 ){
		return strout;
	}
	nByte = WideCharToMultiByte(CP_UTF8, 0, zWide, -1, zname, nByte+1, 	0, 0);
	if( nByte > 0 )
	{
		zname[nByte] = '\0';
	    strout = zname;
	}
	delete []zname;
	return strout;
}

/*
** Convert multibyte character string to UTF-8.  Space to hold the
** returned string is obtained from sqliteMalloc().
*/
std::string MbcsToUtf8(const char *zinname)
{
  std::wstring zTmpWide = MbcsToUnicode(zinname);
  if( zTmpWide.empty() ){
	  return std::string("");
  }
  return UnicodeToUtf8(zTmpWide.c_str());
}



#else


#include "bbiconv.h"
#include <string.h>

/*
** Convert an ansi string to microsoft unicode, based on the
** current codepage settings for file apis.
** 
** Space to hold the returned string is obtained
** from sqliteMalloc.
*/
std::wstring MbcsToUnicode(const char *zinname)
{
	std::wstring strout;
	bbiconv_gbk_2_wchar_s(zinname,strlen(zinname),strout);
	return strout;

}

/*
** Convert microsoft unicode to multibyte character string, based on the
** user's Ansi codepage.
**
** Space to hold the returned string is obtained from
** sqliteMalloc().
*/
std::string UnicodeToMbcs(const wchar_t *zWide)
{
	std::string strout;
	bbiconv_wchar_2_gbk_s(zWide,wcslen(zWide),strout);
	return strout;
}

std::string UnicodeToUtf8(const wchar_t *zWide)
{
	std::string strout ;
	bbiconv_wchar_2_utf8_s(zWide,wcslen(zWide),strout);
	return strout;
}

/*
** Convert multibyte character string to UTF-8.  Space to hold the
** returned string is obtained from sqliteMalloc().
*/
std::string MbcsToUtf8(const char *zinname)
{
	std::string strout ;
	bbiconv_gbk_2_utf8_s(zinname,strlen(zinname),strout);
	return strout;
}

// for other platform
char* _strlwr(char *a)
{
	char *ret = a;
	while (*a != '\0')
	{
		if (isupper (*a))
		*a = tolower (*a);
		++a;
	}
	return ret;
}

#endif
