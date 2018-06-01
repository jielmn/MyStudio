#pragma once

#include <string>
#ifdef _ANDROID_
namespace std {         typedef basic_string<wchar_t> wstring; }
#endif
class CloURLCode
{
public:
	static int	UrlDecode(std::wstring& strOut,const char* lpBuff,size_t len);
	static int  UrlDecodeA(std::string& strOut,const char* lpBuff,size_t len);

	static int	UrlEncode(std::wstring& strOut, const char* lpBuff,size_t len);	
	static int  UrlEncodeA(std::string& strOut, const char* lpBuff,size_t len);
};
