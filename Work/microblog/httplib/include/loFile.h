#ifndef __lo_FILEIO_H__
#define __lo_FILEIO_H__


#include "lo/locommon.h"

// for platform win32
#if defined(WIN32) || defined(_WIN32)
#	include <tchar.h>
#else
#	include "lo/lodefine.h"

#define FILE_ATTRIBUTE_DIRECTORY   0x00000010  

//static
//unsigned int GetFileAttributesW(LPCWSTR lpszFileName)
//{
//	
//	return 0xFFFFFFFF;
//}

//static
//unsigned int GetFileAttributesA(const char* lpszFileName)
//{
//	return 0xFFFFFFFF;
//}

//static 
//unsigned int CreateDirectoryA(const char* lpszPath,void* pNil)
//{
//	return FALSE;
//}

//static 
//unsigned int CreateDirectoryW(const wchar_t* lpszPath,void* pNil)
//{
//	return FALSE;
//}

// for unicode
#if defined(UNICODE) || defined(_UNICODE)
#	define GetFileAttributes GetFileAttributesW 
#	define CreateDirectory   CreateDirectoryW
#else
#	define GetFileAttributes GetFileAttributesA
#	define CreateDirectory   CreateDirectoryA
#endif
/*
// for unicode
#if defined(UNICODE) || defined(_UNICODE)
typedef const wchar_t* LPCTSTR;
typedef wchar_t        TCHAR;
#else
typedef const char*    LPCTSTR;
typedef char           TCHAR;
#endif // !unicode

typedef const wchar_t* LPCWSTR;

//const unsigned int	FALSE = 0;
//const unsigned int	TRUE = 1;
 */

#endif // !win32


DEFINE_NAMESPACE(locom)



/**
 * @brief Œƒº˛/Œƒº˛º–≤Ÿ◊˜io
 *
 * @author loach
 *
 * @date 2007-09-10
*/
class loCOMMON_API CloFile
{
public:
	/**
	 * @brief Œƒº˛À‰∑Ò¥Ê‘⁄
	*/
	static BOOL FileExists(LPCWSTR lpszFileName);
	static BOOL IsFileExists(LPCTSTR lpszFileName);

	/**  «∑ÒŒƒº˛ƒø¬º¥Ê‘⁄ */
	static BOOL IsDirectoryExists(LPCTSTR strFileName);

	/**
	 * @brief  «Œƒº˛£¨ªπ «¬∑æ∂
	 * @return 0 Œƒº˛º–£¨-1 ≤ª¥Ê‘⁄,1 Œƒº˛
	*/
	static int  IsPathOrFile(LPCTSTR lpszFileName);

	/** ¥¥Ω®Œƒº˛º– */	
	static BOOL CreateFolder(LPCTSTR szFolder);
	static BOOL CreateDirectory(LPCTSTR lpPath);
};

END_NAMESPACE(locom)

#endif /* __lo_FILEIO_H__ */

