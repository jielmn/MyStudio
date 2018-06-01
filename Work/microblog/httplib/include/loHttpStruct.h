#ifndef __lo_HTTPSTRUCT_H__
#define __lo_HTTPSTRUCT_H__

#include "setup.h"

#include "libhttp/loHttpDef.h"

namespace lohttp
{

#define Http_Char_New(p,size) \
	(p) = new HTTPChar[size]; 

#define Http_Char_Free(p) \
	{ if(p) delete[] (p); (p) = NULL ;}

//#define SAFE_DELETE_MEMORYBUFFER(mbuff)\
//	{\
//	   if( (mbuff)->buffer && (mbuff)->size > 0 ){\
//	   delete[] (mbuff)->buffer;\
//	   (mbuff)->buffer = NULL;\
//	   (mbuff)->size = 0;}\
//    }

#define SAFE_DELETE_MEMORYBUFFER(mbuff)\
    {\
    if( (mbuff)->buffer ){\
    delete[] (mbuff)->buffer;\
    (mbuff)->buffer = NULL;\
    (mbuff)->size = 0;}\
    }

#define SAFE_NEW_MEMORYBUFFER(mbuff,buffsize,len)\
	{\
	   SAFE_DELETE_MEMORYBUFFER(mbuff);\
	   (mbuff)->buffer = new char[buffsize];\
	   (mbuff)->size = len;\
	}

#define COPY_MEMORYBUFFER(msrbuff,mdestbuff)\
	{\
	   if( (msrbuff)->buffer && (msrbuff)->size >= 0 && (mdestbuff)->buffer && (mdestbuff)->size >= (msrbuff)->size )\
	   memcpy( (mdestbuff)->buffer, (msrbuff)->buffer, (msrbuff)->size);\
	}


#ifndef SAFE_FREE_BUFFER
#define SAFE_FREE_BUFFER(p)    if ( (p) != NULL ) { free( (p) ); (p) = NULL; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)         if ( (p) != NULL ) { delete( (p) ); (p) = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)   if ( (p) != NULL ) { delete[] ( (p) ); (p) = NULL; }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)        if ( (p) != NULL ) { (p)->Release(); (p) = NULL; } 
#endif


#if !(defined(WIN32)) && !(defined(_WIN32))

	#define strcpy_s(dst,count,src) strcpy(dst,src)

#endif


	/**
	* 任务类型定义
	*/
	enum EHttpTransferType
	{
		E_HTTPTYPE_GET,            // GET任务
		E_HTTPTYPE_DOWNLOAD,       // DOWNLOAD任务，下载到文件的函数。
		E_HTTPTYPE_POST,           // 简单POST任务，POST参数需要指定为表单和非表单。
		E_HTTPTYPE_POST_FORMDATA,  // POST本地文件。
		E_HTTPTYPE_POST_TEXT_FILE, //post二进制与文本混排
//		E_HTTPTYPE_UPLOAD,         // UPLOAD本地文件，HTTP1.1的method
		E_HTTPTYPE_DELETE,         // DELETE method,HTTP 1.1
		E_HTTPTYPE_PUT,            // PUT method,HTTP 1.1
	};

#pragma pack(push)
#pragma pack(4)

	/**
	* @说明：自定的内存Buffer结构体。
	*/
	typedef struct _tagHttpMemoryBuffer
	{
		char *	buffer;				// Buffer内存地址
		size_t	size;				// Buffer大小
		_tagHttpMemoryBuffer()
		{
			memset( this, 0, sizeof(*this) );
		}
	}THttpMemoryBuffer;

	/**
	* @下载文件时用到的参数结构体，外部程序不用关心。
	*/
	typedef struct _tagHttpFileBuffer
	{
		char	fileName[1024];		// 从远程获取的文件的名称
		size_t	fileSize;			// 从远程获取的文件大小
		FILE *	fp;					// File point
		size_t	offset;				// 保存文件的偏移量
		char *	buffer;				// 数据缓冲，不一定要用到
		size_t	size;				// 数据缓冲大小
		_tagHttpFileBuffer()
		{
			memset( this, 0, sizeof(*this) );
		}
	}THttpFileBuffer;

	const int ConstProxyServerLength = 256;
	const int ConstProxyUserLength = 256;
	const int ConstProxyPwdLength = 128;

	typedef struct _tagHttpProxyInfo	// 代理信息结构体
	{
		int		nProxyType;
		unsigned short	wPort;						// 代理服务器端口
		char	cServer[ConstProxyServerLength];	// 代理服务器地址
		char	cUser[ConstProxyUserLength];		// 代理服务器用户
		char	cPassword[ConstProxyPwdLength];		// 代理服务器用户密码

		_tagHttpProxyInfo()
		{
			memset( this, 0, sizeof(*this) );
			wPort = 808;
		}
	}THttpProxyInfo;


	const int ConstLocalFileLength = 1024;
	const int ConstFormNameLength = 256;
	const int ConstFormContentLength = 1024;
	const int ConstHeadLength = 4096;
	const int ConstURLLength = 2048;
	const int ConstUploadParamLength = 1024;


	/**
	* @说明：HttpClient设置任务属性的结构体。
	* @备注：TTaskParam结构体有自身的构造函数和析构函数外部程序不需要再手动delete pData值
	*        并且外部程序最好是不要使用memset，这样会使里面的初始参数无效，TTaskParam具有”operator =”功能。
	*/
	typedef struct _tagTaskParam
	{
		char	cURL[ConstURLLength];		// 访问地址
		char	cHead[ConstHeadLength];		// Http头

		// 上传文件的参数
//		char	cLocalFile[ConstLocalFileLength];	// 本地文件绝对地址，POST文件和UPLOAD文件用
//		char    cFormName[ConstFormNameLength];
//		char    cFormContents[ConstFormContentLength];   //用于文件二进制数据与文本混排的post方法

		// 下载任务用的参数		
		char	cSavePath[ConstLocalFileLength];		// 本地保存文件路径
		char	cNewFileName[ConstLocalFileLength];		// 本地保存的文件名
		BOOL	bRename;				// 是否使用指定的新文件名
        BOOL	bToBuffer;				// 只下载到Buffer中

		// POST,DELETE,PUT用
		unsigned int uDataLength;	// POST数据长度
		char*	pData;				// POST数据
		BOOL	bFormData;			// 使用表单格式POST

		THttpForm form;

		_tagTaskParam()
		{
			memset( this, 0, sizeof(*this) );
			bFormData = TRUE;
		}

		~_tagTaskParam()
		{
			if ( pData != NULL )
			{
				delete[] pData;
				pData = NULL;
			}
		}

		void operator = (const _tagTaskParam& value) throw()
		{
			strcpy_s(cURL, ConstURLLength, value.cURL);
			strcpy_s(cHead,ConstHeadLength,value.cHead);

//			strcpy_s(cFormContents ,value.cFormContents);
//			strcpy_s(cFormName,value.cFormName);
//			strcpy_s(cLocalFile, value.cLocalFile);

			// POST,DELETE,PUT用
			if( value.uDataLength > 0 && value.pData )
			{
                if ( pData != NULL )
                {
                    delete[] pData;
                    pData = NULL;
                    uDataLength = 0;
                }

				uDataLength = value.uDataLength;
				pData = new char[value.uDataLength+1];
				memcpy(pData, value.pData, value.uDataLength);
				pData[uDataLength] = '\0';
			}
			bFormData = value.bFormData;

			// 下载任务用的参数
			bToBuffer = value.bToBuffer;

			strcpy_s(cSavePath ,  ConstLocalFileLength, value.cSavePath);
			strcpy_s(cNewFileName,ConstLocalFileLength, value.cNewFileName);
			bRename = value.bRename;

		    memcpy(&form , &value.form, sizeof(THttpForm) );
		};
	}TTaskParam;

	// http 参数
	typedef struct _tagHttpURLParam
	{
		HTTPChar*  pszName;
		HTTPChar*  pszValue;
		unsigned long	dwAttrib;
		_tagHttpURLParam()
		{
			pszName = NULL;
			pszValue = NULL;
			dwAttrib = 0;
		};
		~_tagHttpURLParam()
		{
			Http_Char_Free( pszName );
			Http_Char_Free( pszValue );
		};
	}THttpURLParam;

	typedef struct _tagHttpHeader
	{
		HTTPChar* pszName;
		HTTPChar* pszValue;

		_tagHttpHeader()
		{
			pszName = NULL;
			pszValue = NULL;
		};
		~_tagHttpHeader()
		{
			Http_Char_Free( pszName );
			Http_Char_Free( pszValue );
		};
	}THttpHeader;
    
#pragma pack(pop)

} // namespace lohttp


#ifndef BUILD_CHARACTER_SET

	#define HTTP_TCSCPY_S       strcpy_s
	#define HTTP_TCSLEN         strlen
	#define HTTP_TTOL           atol
	#define HTTP_TCSCHR         strchr
	#define HTTP_CT2A(s)        s 
	#define HTTP_CA2T(s)        s
	#define HTTP_STR_TO_UTF8(s) MbcsToUtf8(s)
	#define HTTP_HTString       std::string

#else

	#define HTTP_TCSCPY_S       _tcscpy_s
	#define HTTP_TCSLEN         _tcslen
	#define HTTP_TTOL           _ttol
	#define HTTP_TCSCHR         _tcschr
	#define HTTP_CT2A(s)        CT2A(s) 
	#define HTTP_CA2T(s)        CA2T(s)
	#define HTTP_STR_TO_UTF8(s) STR_TO_UTF8(s)
	#define HTTP_HTString       HTString

#endif

#endif // __lo_HTTPSTRUCT_H__
