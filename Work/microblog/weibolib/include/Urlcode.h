/**
*  @brief URL Encode
*  @file Urlcode.h
**/


#ifndef __urlcode_H__
#define __urlcode_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

	int	 lo_UrlDecode(wchar_t** out,const char* pin,  int len);
	int  lo_UrlDecodeA(char** out,const char* pin, int len);

	int	 lo_UrlEncode(wchar_t** out , const char* pin ,  int len);	
	int  lo_UrlEncodeA(char** out, const char* pin ,  int len);

	class CURLDecodeW
	{
	public:
		CURLDecodeW(const char* pin,  int len)
		{
			_out = 0;
			lo_UrlDecode(&_out,pin,len);
		}
		~CURLDecodeW()
		{
			free(_out);
		}
		wchar_t* c_str() const{ return _out;}
	private:
		wchar_t* _out;
	};

	class CURLDecodeA
	{
	public:
		CURLDecodeA(const char* pin,  int len)
		{
			_out = 0;
			lo_UrlDecodeA(&_out,pin,len);
		}
		~CURLDecodeA()
		{
			free(_out);
		}
		char* c_str() const{ return _out;}
	private:
		char* _out;
	};


	class CURLEncodeW
	{
	public:
		CURLEncodeW(const char* pin,  int len)
		{
			_out = 0;
			lo_UrlEncode(&_out,pin,len);
		}
		~CURLEncodeW()
		{
			free(_out);
		}
		wchar_t* c_str() const{ return _out;}
	private:
		wchar_t* _out;
	};

	class CURLEncodeA
	{
	public:
		CURLEncodeA(const char* pin,  int len)
		{
			_out = 0;
			lo_UrlEncodeA(&_out,pin,len);
		}
		~CURLEncodeA()
		{
			free(_out);
		}
		char* c_str() const{ return _out;}
	private:
		char* _out;
	};

#ifdef __cplusplus
}
#endif

#endif //__urlcode_H__
