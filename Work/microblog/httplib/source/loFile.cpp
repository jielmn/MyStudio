#include "loFile.h"


#if ( !defined(WIN32) && !defined(_WIN32) && !defined(_WIN32_WCE))


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>
#include <bbiconv.h>


#define _tcslen    strlen
#define lstrlen    strlen
#define _tcsrchr   strrchr
#define _tcscat_s  strcat
#define _tcscpy_s  strcpy
#define _tcscpy	   strcpy
#define _tcsncpy   strncpy
#define _tcsncpy_s strncpy
#define _tcschr    strchr
#define _tcscat    strcat


#endif //( !defined(WIN32) && !defined(_WIN32) && !defined(_WIN32_WCE))

DEFINE_NAMESPACE(locom)

//----------------------------------------------------------------------//
// CloFile

BOOL CloFile::IsFileExists(LPCTSTR lpszFileName)
{
	return ( 1 != IsPathOrFile(lpszFileName) ) ? FALSE : TRUE;
}

BOOL CloFile::IsDirectoryExists(LPCTSTR lpszFileName)
{
	return ( 0 != IsPathOrFile(lpszFileName) ) ? FALSE: TRUE;
}

BOOL CloFile::FileExists(LPCWSTR lpszFileName)
{
#if (defined(WIN32)) || (defined(_WIN32)) 

	DWORD  dwFlag = GetFileAttributesW(lpszFileName); 
	if ( 0xFFFFFFFF == dwFlag ) 
		return FALSE;//不存在
	if (  FILE_ATTRIBUTE_DIRECTORY & dwFlag ) 
		return FALSE;//文件夹
	return TRUE; //文件

#else
	std::string outfile;
	bbiconv_wchar_2_gbk_s(lpszFileName,wcslen(lpszFileName),outfile);
	return (access(outfile.c_str(),F_OK) < 0 ) ;

#endif //#if (defined(WIN32)) || (defined(_WIN32))
}

int CloFile::IsPathOrFile(LPCTSTR lpszFileName)
{
#if (defined(WIN32)) || (defined(_WIN32)) 

	DWORD  dwFlag = GetFileAttributes(lpszFileName); 
	if ( 0xFFFFFFFF == dwFlag ) 
		return -1;//不存在
	if (  FILE_ATTRIBUTE_DIRECTORY & dwFlag ) 
		return 0;//文件夹
	return 1; //文件
#else
	struct stat s;
	if (stat(lpszFileName, &s) < 0)
	    return -1;
	return S_ISDIR(s.st_mode) ? 0 : 1;

#endif //#if (defined(WIN32)) || (defined(_WIN32)) 

}

BOOL CloFile::CreateDirectory(LPCTSTR lpPath)
{
	const TCHAR* sz = 0 , *szOffset = 0;
	TCHAR  pszPath[MAX_PATH+1]  = { 0 };
	TCHAR  szDirect[MAX_PATH+1] = { 0 };

	if( !lpPath || *lpPath == '\0' )
		return FALSE;

	size_t srclen = _tcslen(lpPath);
	if( srclen > MAX_PATH )
	{
		return -1;
	}
	_tcscpy( pszPath , lpPath );

	// 反找
	sz = _tcsrchr(lpPath , _T('\\') );
	if( !sz )
	{//
		if(  !_tcsrchr(lpPath , _T('/') ) )
		{
			_tcscat( pszPath ,_T("\\"));
		}
	}

	szOffset = pszPath;
	do
	{
		if( *szOffset == '\0' || szOffset > (pszPath + srclen) )
			break;

		sz = _tcschr(szOffset , _T('\\') );
		if( !sz )
		{
			sz = _tcschr(szOffset , _T('/') );
			if( !sz )
				break;
		}

		size_t len = (size_t)(sz - pszPath) + 1;
		_tcsncpy( szDirect , pszPath , len ); 
		szDirect[ len ] ='\0';

		szOffset = (++sz) ;

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)

		if( !::CreateDirectory( szDirect , NULL) )
		{// 已存在，当成功处理
//			if( GetLastError() != ERROR_ALREADY_EXISTS )
//				return FALSE;
		}
#else
		mkdir(szDirect, 0777);

#endif //
	}while(1);

	return TRUE;
}


BOOL CloFile::CreateFolder(LPCTSTR szFolder)
{
	if (!szFolder || !lstrlen(szFolder))
		return FALSE;

#if (defined(WIN32)) || (defined(_WIN32))

	DWORD dwAttrib = GetFileAttributes(szFolder);

	// already exists ?
	if (dwAttrib != 0xffffffff)
		return ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);

	// if can't create parent
	if(::CreateDirectory(szFolder,NULL)){
		dwAttrib = 0;			
	}
	return (0==dwAttrib);

#else
	int flag = IsPathOrFile(szFolder);
	if (flag != -1)
	    return flag == 0;
	return mkdir(szFolder, 0777) == 0;

#endif //#if (defined(WIN32)) || (defined(_WIN32))

}

END_NAMESPACE(locom)
