#include "loUrlcode.h"
#include "lostrconv.h"

int CloURLCode::UrlDecodeA(std::string& strOut,const char* lpBuff,size_t len)
{
	strOut = "";
	char pBuf[4];
	int  c;
	for( size_t i = 0; i < len ; )
	{
		if( *(lpBuff) == '%' )
		{
			if( i < len - 2 ) {				
				pBuf[0] = *(lpBuff+1);
				pBuf[1] = *(lpBuff+2);
				pBuf[2] = '\0';
				sscanf( pBuf, "%02X", &c );
				strOut += (char)c;
				i += 3;				
				lpBuff += 3;
			}
			else{				
				return -1;
			}
		}
		else{
			strOut += *lpBuff;
			i++;
			lpBuff++;
		}
	}
	return 0;
}

int CloURLCode::UrlDecode(std::wstring& strOut,const char* lpBuff,size_t len)
{
	strOut = L"";
	std::string outA;
	UrlDecodeA(outA, lpBuff , len);

	strOut = CA2W( outA.c_str() );
	return 0;
}
/*
strOut = _T("");
char pBuf[4];
int  c;
for( size_t i = 0; i < len ; )
{
if( *(lpBuff) == '%' )
{
if( i < len - 2 ) {				
pBuf[0] = *(lpBuff+1);
pBuf[1] = *(lpBuff+2);
pBuf[2] = '\0';
sscanf_s( pBuf, "%02X", &c );
strOut += (char)c;
i += 3;				
lpBuff += 3;
}
else{				
return -1;
}
}
else{
strOut += *lpBuff;
i++;
lpBuff++;
}
}
return 0;
}
*/

int CloURLCode::UrlEncodeA(std::string& strOut, const char* lpBuff,size_t len)
{
	strOut = "";
	//将保留字符转化为16进制格式.
	// according to RFC 2396
	char pBuf[4];
	unsigned char c;
	for( size_t i = 0; i < len; i++ ) {
		c = *(lpBuff+i);
		if( isalpha( c ) || isdigit( c ) ||
			c == '-' ||
			c == '_' ||
			c == '.' ||
			c == '!' ||
			c == '~' ||
			c == '*' ||
			c == '\'' ||
			c == '(' ||
			c == ')' )
		{
			// found an unreserved character
			strOut += c;
		}
		else if( c == ' ' )
			strOut += "%20";
		else
		{
			sprintf( pBuf, "%02X", c );
			strOut += "%";
			strOut += pBuf;
		}
	}
	return 0;
}

int CloURLCode::UrlEncode(std::wstring& strOut, const char* lpBuff,size_t len)
{
	strOut = L"";
	std::string outA;
	UrlEncodeA(outA, lpBuff , len);

	strOut = CA2W( outA.c_str() );
	return 0;
}
/*
//将保留字符转化为16进制格式.
// according to RFC 2396
char pBuf[4];
unsigned char c;
for( size_t i = 0; i < len; i++ ) {
c = *(lpBuff+i);
if( isalpha( c ) || isdigit( c ) ||
c == '-' ||
c == '_' ||
c == '.' ||
c == '!' ||
c == '~' ||
c == '*' ||
c == '\'' ||
c == '(' ||
c == ')' )
{
// found an unreserved character
strOut += c;
}
else if( c == ' ' )
strOut += _T("%20");
else
{
sprintf_s( pBuf, "%02X", c );
strOut += _T("%");
strOut += CA2W(pBuf);
}
}
return 0;
}
*/

