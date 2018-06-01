#ifndef __STRCONV_H__
#define __STRCONV_H__

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

	/*
	** Convert an ansi string to microsoft unicode, based on the
	** current codepage settings for file apis.
	** 
	** Space to hold the returned string is obtained
	** from malloc. please free
	*/
	int lo_C2W(wchar_t** pout , const char *zinname);
	int lo_W2C(char** pout ,const wchar_t *zWide);


	int lo_W2Utf8(char** pout ,const wchar_t *zWide);
	int lo_Utf82W(wchar_t** pout , const char* utf8 );
	/*
	** Convert multibyte character string to UTF-8.  Space to hold the
	** returned string is obtained from malloc().
	*/
	int lo_C2Utf8(char** pout , const char *zinname);
	int lo_Utf82C(char** pout , const char *zstr);


	class CC2W
	{
	public:
		CC2W(const char* z)
			:_out(0)
		{
			_length = lo_C2W(&_out , z);
		}
		~CC2W()
		{
			if( _out ) free(_out);
		}
		wchar_t* c_str() const
		{
			return _out;
		}
		int length()const{ return _length;}
	private:
		wchar_t* _out;
		int _length;
	};


	class CW2C
	{
	public:
		CW2C(const wchar_t* z)
			:_out(0)
		{
			_length=lo_W2C(&_out , z);
		}
		~CW2C()
		{
			if( _out ) free(_out);
		}
		char* c_str() const
		{
			return _out;
		}
		int length()const{ return _length;}
	private:
		char* _out;
		int _length;
	};


	class CW2UTF8
	{
	public:
		CW2UTF8(const wchar_t* z)
			:_out(0)
		{
			_length=lo_W2Utf8(&_out , z);
		}
		~CW2UTF8()
		{
			if( _out ) free(_out);
		}
		char* c_str() const
		{
			return _out;
		}
		int length()const{ return _length;}
	private:
		char* _out;
		int _length;
	};

	class CUTF82W
	{
	public:
		CUTF82W(const char* z)
			:_out(0)
		{
			_length=lo_Utf82W(&_out , z);
		}
		~CUTF82W()
		{
			if( _out ) free(_out);
		}
		wchar_t* c_str() const
		{
			return _out;
		}
		int length()const{ return _length;}
	private:
		wchar_t* _out;
		int _length;
	};


	class CC2UTF8
	{
	public:
		CC2UTF8(const char* z)
			:_out(0)
		{
			_length=lo_C2Utf8(&_out , z);
		}
		~CC2UTF8()
		{
			if( _out ) free(_out);
		}
		char* c_str() const
		{
			return _out;
		}
		int length()const{ return _length;}
	private:
		char* _out;
		int _length;
	};

	class CUTF82C
	{
	public:
		CUTF82C(const char* z)
			:_out(0)
		{
			_length=lo_Utf82C(&_out , z);
		}
		~CUTF82C()
		{
			if( _out ) free(_out);
		}
		char* c_str() const
		{
			return _out;
		}
		int length()const{ return _length;}
	private:
		char* _out;
		int _length;
	};

	/***/
	class CIntToStr
	{
	public:
		CIntToStr(int count)
		{
#ifdef WIN32
			_out[0] = '\0';
			_itoa(count , _out , 10 );
#else
			sprintf(_out, "%d", count);
#endif
		}
		const char* c_str() const{ return _out;}
	private:
		char _out[32]; 
	};

#ifdef __cplusplus
}
#endif

#endif // __STRCONV_H__
