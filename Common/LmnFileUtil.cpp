#include "StdAfx.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#ifdef WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#include <iconv.h>
#include <sys/times.h>
#include <semaphore.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <error.h>
#include <errno.h>
#include <arpa/inet.h>
#endif

#include "Inner.h"
#include "LmnCommon.h"

#ifdef WIN32
typedef  wchar_t   F_CHAR;
#else
typedef  char      F_CHAR;
#endif





#ifdef WIN32
#define   NORMAL_SLASH_CHAR          '\\'
#define   WIDE_SLASH_CHAR           L'\\'
#define   F_STRNCPY( d, s, n )                   wcsncpy( d, s, n )
#define   F_STRCPY( d, s )                       wcscpy( d, s )
#define   F_STRRCHR( s, ch )                     wcsrchr( s, ch )
#define   F_STRCHR( s, ch )                      wcschr( s, ch )
#define   F_STRCMP( s1, s2 )                     wcscmp( s1, s2 )
#define   F_STRLEN( s )                          wcslen( s )
#define   READ_MODE                              L"rb"
#define   WRITE_MODE                             L"wb"
#define   READ_WRITE_MODE                        L"rb+"
#define   FILE_OPEN( f, m )                      _wfopen( f, m )
#define   WSNPRINTF                              _snwprintf
#define   FORMAT_1                               L"%s\\%s"
#define   END_CHAR                               L'\0'
#else
#define   NORMAL_SLASH_CHAR          '/'
#define   WIDE_SLASH_CHAR            '/'
#define   F_STRNCPY( d, s, n )                   strncpy( d, s, n )
#define   F_STRCPY( d, s )                       strcpy( d, s )
#define   F_STRRCHR( s, ch )                     strrchr( s, ch )
#define   F_STRCHR( s, ch )                      strchr( s, ch )
#define   F_STRCMP( s1, s2 )                     strcmp( s1, s2 )
#define   F_STRLEN( s )                          strlen( s )
#define   READ_MODE                              "rb"
#define   WRITE_MODE                             "wb"
#define   READ_WRITE_MODE                        "rb+"
#define   FILE_OPEN( f, m )                      fopen( f, m )
#define   WSNPRINTF                              snprintf
#define   FORMAT_1                               "%s/%s"
#define   END_CHAR                               '\0'
#endif



static BOOL ConvertPath_( PList lstPath, const char * szAbsolutePath )
{
    char * szDirItem = 0;
    DWORD dwLen      = 0;
    PListNode pNode  = 0;
    const char * p   = 0;
    const char * pSlash = 0;

    ClearList( lstPath );

#ifdef WIN32
    p      = szAbsolutePath;
#else
    assert( '/' == szAbsolutePath[0] );

    dwLen = 1;
    szDirItem = (char *)g_pfnAllocMem( dwLen + 1 );
    if ( 0 == szDirItem )
    {
        ClearListWithPointerValue( lstPath );
        return FALSE;
    }

    szDirItem[0] = '/';
    szDirItem[1] = '\0';

    pNode = Insert2ListTail( lstPath, szDirItem );
    if ( 0 == pNode )
    {
        g_pfnFreeMem( szDirItem );
        ClearListWithPointerValue( lstPath );
        return FALSE;
    }

    p = szAbsolutePath + 1;
#endif

    pSlash = strchr( p, NORMAL_SLASH_CHAR );
    while( pSlash || p )
    {
        if ( pSlash )
        {
            dwLen = pSlash - p;
        }
        else
        {
            dwLen = strlen( p );
        }

        if ( 0 == dwLen )
        {
            ClearListWithPointerValue( lstPath );
            return FALSE;
        }

        szDirItem = (char *)g_pfnAllocMem( dwLen + 1 );
        if ( 0 == szDirItem )
        {
            ClearListWithPointerValue( lstPath );
            return FALSE;
        }

        memcpy( szDirItem, p, dwLen );
        szDirItem[dwLen] = '\0';

        // 本目录，忽略不计
        if ( 0 == strcmp(szDirItem, ".") )
        {
            g_pfnFreeMem( szDirItem );
        }
        // 父目录，
        else if ( 0 == strcmp(szDirItem, "..") )
        {
            DWORD dwItemCnt = GetListSize( lstPath );
            // 错误的absolute路径格式
            if ( dwItemCnt <= 1 )
            {
                g_pfnFreeMem( szDirItem );
                ClearListWithPointerValue( lstPath );
                return FALSE;
            }

            // 抵消最后一个目录item
            pNode = GetListTail( lstPath );
            assert( pNode );

            g_pfnFreeMem( pNode->pData );
            EraseListTail( lstPath );
        }
        // 其他
        else
        {
            pNode = Insert2ListTail( lstPath, szDirItem );
            if ( 0 == pNode )
            {
                g_pfnFreeMem( szDirItem );
                ClearListWithPointerValue( lstPath );
                return FALSE;
            }
        }
        

        if ( pSlash )
        {
            p = pSlash + 1;
            pSlash = strchr( p, NORMAL_SLASH_CHAR );
        }
        else
        {
            // p += dwLen;
            return TRUE;
        }
    }

    return TRUE;
}

static BOOL IsAbsolutePath_( const char * szPath )
{
    DWORD  dwLen = strlen( szPath );

#ifdef WIN32
    if ( dwLen >= 2 && ':' == szPath[1] )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#else
    if ( dwLen >= 1 && '/' == szPath[0] )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#endif
}

static  char *  Convert2AbsolutePath_( char * szPath, DWORD dwPathSize )
{
    char  buf[LMN_MAX_FILE_PATH] = { 0 };
    // char  tmp[LMN_MAX_FILE_PATH] = { 0 };

    if ( IsAbsolutePath_( szPath ) )
    {
        return szPath;
    }

#ifdef WIN32
    ::GetCurrentDirectoryA( sizeof(buf) - 1, buf );
#else
    getcwd( buf, sizeof(buf) - 1 );
#endif

    DWORD  dwLen = strlen( buf );
    if ( buf[dwLen-1] != NORMAL_SLASH_CHAR )
    {
        buf[dwLen] = NORMAL_SLASH_CHAR;
        dwLen++;
    }

    strncat( buf, szPath, sizeof(buf) - dwLen - 1 );

    strncpy( szPath, buf, dwPathSize - 1 );

    return szPath;
}

static  BOOL  IsEqualPath_( PList lstPath1, PList lstPath2 )
{
    DWORD  dwSize1 = GetListSize( lstPath1 );
    DWORD  dwSize2 = GetListSize( lstPath2 );

    if ( dwSize1 != dwSize2 )
    {
        return FALSE;
    }

    PListNode pNode1 = GetListHead(lstPath1);
    PListNode pNode2 = GetListHead(lstPath2);

    while ( pNode1 )
    {
        assert( pNode2 );

        if ( 0 != StrICmp( (char *)pNode1->pData, (char *)pNode2->pData) )
        {
            return FALSE;
        }

        pNode1 = GetNextListNode(pNode1);
        pNode2 = GetNextListNode(pNode2);
    }

    return TRUE;
}


// lstPath2是否为lstPath1的父目录或更父目录
static  BOOL  IsParentPath_( PList lstPath1, PList lstPath2 )
{
    DWORD  dwSize1 = GetListSize( lstPath1 );
    DWORD  dwSize2 = GetListSize( lstPath2 );

    if ( dwSize2 >= dwSize1 )
    {
        return FALSE;
    }

    PListNode pNode1 = GetListHead(lstPath1);
    PListNode pNode2 = GetListHead(lstPath2);

    while ( pNode2 )
    {
        assert( pNode1 );

        if ( 0 != StrICmp( (char *)pNode1->pData, (char *)pNode2->pData) )
        {
            return FALSE;
        }

        pNode1 = GetNextListNode(pNode1);
        pNode2 = GetNextListNode(pNode2);
    }

    return TRUE;
}





static  BOOL  IsSlashChar_( char ch )
{
    if ( '/' == ch || '\\' == ch )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



static  void  NormalizePath_( char * szFilePath )
{
    char * p = szFilePath;
    while( *p != '\0' )
    {
        if ( IsSlashChar_(*p) )
        {
            *p = NORMAL_SLASH_CHAR;
        }
        p++;
    }
}

static  void  RemoveLastSlash( char * szFilePath )
{
    DWORD  dwLen = strlen( szFilePath );
    if ( dwLen > 0 )
    {
        if ( IsSlashChar_(szFilePath[dwLen-1]) )
        {
            szFilePath[dwLen-1] = '\0';
        }
    }
}







static  ELmnFileType  GetFileType_( const F_CHAR * path )
{
#ifdef WIN32 
    DWORD dwRet = GetFileAttributesW( path );
    // 目录存在
    if ( dwRet != INVALID_FILE_ATTRIBUTES )
    {
        if ( (dwRet & FILE_ATTRIBUTE_DIRECTORY) != 0 )
        {
            return  E_FILE_TYPE_FOLDER;
        }
        else
        {
            return E_FILE_TYPE_FILE;
        }
    }
    else
    {
        return E_FILE_TYPE_NONE;
    }
#else
    struct stat st;
    int nRet = ::stat( path, &st );
    // 目录/文件存在
    if ( 0 == nRet )
    {
        if ( S_ISDIR(st.st_mode) != 0 )
        {
            return E_FILE_TYPE_FOLDER;
        }
        else
        {
            return E_FILE_TYPE_FILE;
        }
    }
    else
    {
        return E_FILE_TYPE_NONE;
    }
#endif
}


static  BOOL  IsFolder_( const F_CHAR * path )
{
    ELmnFileType eFileType = GetFileType_( path );
    if ( E_FILE_TYPE_FOLDER == eFileType )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



static BOOL CreateFolder_( const F_CHAR *  path )
{
    const F_CHAR * pLastSlash = F_STRRCHR( path, WIDE_SLASH_CHAR );

    // 如果有父目录，确保父目录存在
    if ( pLastSlash )
    {
        F_CHAR  pFatherFolderPath[LMN_MAX_FILE_PATH] = { 0 };
        DWORD   dwFatherFolderPathLen = pLastSlash - path;
        F_STRNCPY( pFatherFolderPath, path, dwFatherFolderPathLen );

        ELmnFileType eFileType = GetFileType_( pFatherFolderPath );
        // 如果父目录不存在
        if ( E_FILE_TYPE_NONE == eFileType )
        {
            if ( !CreateFolder_( pFatherFolderPath ) )
            {
                return FALSE;
            }
        }
        // 如果父目录是文件
        else if ( E_FILE_TYPE_FILE == eFileType )
        {
            return FALSE;
        }
    }

#ifdef WIN32
    return ( ::CreateDirectoryW( path, 0 ) != 0 );
#else
    return ( ::mkdir(path, 0755 ) == 0 );
#endif

}


typedef BOOL (*TraverseDirectoryCallback)( const F_CHAR * full_path, BOOL bFolder, const F_CHAR * file_name, void * pParam0, void * pParam1 );


static  int  TraverseDirectory( const F_CHAR * folder, TraverseDirectoryCallback pfnCb, void * pParam0, void * pParam1 )
{
#ifdef WIN32
    WIN32_FIND_DATAW  data;

    // 构造通配符
    wchar_t  wszPath[LMN_MAX_FILE_PATH] = { 0 };
    _snwprintf( wszPath, LMN_MAX_FILE_PATH, L"%s\\*", folder );

    HANDLE handle = ::FindFirstFileW( wszPath, &data );
    if ( INVALID_HANDLE_VALUE != handle )
    {
        while( FindNextFileW( handle, &data ) )
        {
            // 忽略"."和".."
            if ( 0 == wcscmp( data.cFileName, L".") || 0 == wcscmp( data.cFileName, L".." ) )
            {
                continue;
            }

            wchar_t  wszSubPath[LMN_MAX_FILE_PATH] = { 0 };
            _snwprintf( wszSubPath, LMN_MAX_FILE_PATH, L"%s\\%s", folder, data.cFileName );

            BOOL  bFolder = ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? TRUE : FALSE ;

            // 如果不继续
            if ( !pfnCb( wszSubPath, bFolder, data.cFileName, pParam0, pParam1 ) )
            {
                break;
            }
        }

        ::FindClose( handle );
        return 0;
    }
    else
    {
        return -1;
    }
#else

    DIR * dir_ = ::opendir( folder );
    if ( 0 == dir_ )
    {
        return -1;
    }

    struct dirent * dirent_ = readdir(dir_);
    while ( dirent_ )
    {
        // 忽略"."和".."
        if ( !(0 == strcmp( dirent_->d_name, "." ) || 0 == strcmp( dirent_->d_name, ".." )) )
        {
            char  szSubPath[LMN_MAX_FILE_PATH] = { 0 };
            snprintf( szSubPath, LMN_MAX_FILE_PATH, "%s/%s", folder, dirent_->d_name );

            BOOL  bFolder = IsFolder_(szSubPath);
            // 如果不继续
            if ( !pfnCb( szSubPath, bFolder, dirent_->d_name, pParam0, pParam1 ) )
            {
                break;
            }
        }

        dirent_ = readdir(dir_);
    }

    closedir(dir_);
    return 0;

#endif
}




static  BOOL   DeleteFile_ ( const F_CHAR * path, BOOL bFolder );

static BOOL TraverseDeleteFile_( const F_CHAR * full_path, BOOL bFolder, const F_CHAR * file_name, void * pParam0, void * pParam1 )
{
    if ( !DeleteFile_( full_path, bFolder ) )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}




static  BOOL   DeleteFile_ ( const F_CHAR * path, BOOL bFolder )
{
    // 如果是目录
    if ( bFolder )
    {
        if ( 0 != TraverseDirectory( path, TraverseDeleteFile_, 0, 0 ) )
        {
            return FALSE;
        }

#ifdef WIN32
        return (::RemoveDirectoryW( path ) != 0);
#else
        return ( ::rmdir( path ) == 0 );
#endif
    }
    // 如果是文件
    else
    {
#ifdef WIN32
        return (::DeleteFileW( path ) != 0 );
#else
        return (::unlink( path ) == 0) ;
#endif
    }
}



#ifndef WIN32
static  BOOL  CopySingleFile_( const F_CHAR * old_path, const F_CHAR * new_path, BOOL bOverWrite )
{
    FILE * fpSrc = 0;
    FILE * fpDst = 0;

    if ( !bOverWrite )
    {
        ELmnFileType  eDestFileType = GetFileType_( new_path );
        if ( E_FILE_TYPE_NONE != eDestFileType )
        {
            return FALSE;
        }
    }   

    fpSrc = fopen( old_path, "rb" );
    if ( 0 == fpSrc )
    {
        return FALSE;
    }

    fpDst = fopen( new_path, "wb" );
    if ( 0 == fpDst )
    {
        fclose( fpSrc );
        return FALSE;
    }

    const DWORD  FILE_BUFF_SIZE = 0x8000;
    char buf[FILE_BUFF_SIZE];
    BOOL  bFail = FALSE;

    DWORD dwRead = fread( buf, 1, FILE_BUFF_SIZE, fpSrc );
    while( dwRead > 0 )
    {
        DWORD dwWrite = fwrite( buf, 1, dwRead, fpDst );
        if ( 0 == dwWrite )
        {
            bFail = TRUE;
            break;
        }

        if ( dwWrite != dwRead )
        {
            bFail = TRUE;
            break;
        }

        dwRead = fread( buf, 1, FILE_BUFF_SIZE, fpSrc );
    }

    fclose( fpSrc );
    fclose( fpDst );

    if ( bFail )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
#endif



static  BOOL   CopyFile_ ( const F_CHAR * old_path, const F_CHAR * new_path, BOOL bFolder, BOOL bOverWrite );

static BOOL TraverseCopyFile_( const F_CHAR * full_path, BOOL bFolder, const F_CHAR * file_name, void * pParam0, void * pParam1 )
{
    const F_CHAR * new_path   = (const F_CHAR *)pParam0;
    BOOL           bOverWrite = (BOOL)pParam1;

    F_CHAR  new_sub_path[LMN_MAX_FILE_PATH];
#ifdef WIN32
    _snwprintf( new_sub_path, LMN_MAX_FILE_PATH, L"%s\\%s", new_path, file_name );
#else
    snprintf( new_sub_path, LMN_MAX_FILE_PATH, "%s/%s", new_path, file_name );
#endif

    if ( bFolder )
    {
        if ( !CreateFolder_(new_sub_path) )
        {
            return FALSE;
        }
    }

    if ( !CopyFile_( full_path, new_sub_path, bFolder, bOverWrite ) )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static  BOOL   CopyFile_ ( const F_CHAR * old_path, const F_CHAR * new_path, BOOL bFolder, BOOL bOverWrite )
{
    // 如果是目录
    if ( bFolder )
    {
        if ( 0 != TraverseDirectory( old_path, TraverseCopyFile_, (void *)new_path, (void *)bOverWrite ) )
        {
            return FALSE;
        }
        return TRUE;
    }
    // 如果是文件
    else
    {
#ifdef WIN32
        return ::CopyFileW( old_path, new_path, !bOverWrite ) ? TRUE : FALSE;
#else
        return CopySingleFile_( old_path, new_path, TRUE ) ? TRUE : FALSE;
#endif
    }
}


#ifndef WIN32
static  BOOL  MoveSingleFile_( const F_CHAR * old_path, const F_CHAR * new_path, BOOL bOverWrite )
{
    if ( !bOverWrite )
    {
        ELmnFileType  eDestFileType = GetFileType_( new_path );
        if ( E_FILE_TYPE_NONE != eDestFileType )
        {
            return FALSE;
        }
    }

    if ( rename( old_path, new_path ) != 0 ) 
    {
        if ( errno != EXDEV )
            return FALSE;

        if ( !CopySingleFile_( old_path, new_path, bOverWrite ) )
            return FALSE;

        if ( !DeleteFile_( old_path, FALSE) )
            return FALSE;
    }
    
    return TRUE;
}
#endif


static  BOOL   MoveFile_ ( const F_CHAR * old_path, const F_CHAR * new_path, BOOL bFolder, BOOL bOverWrite );

static BOOL TraverseMoveFile_( const F_CHAR * full_path, BOOL bFolder, const F_CHAR * file_name, void * pParam0, void * pParam1 )
{
    const F_CHAR * new_path   = (const F_CHAR *)pParam0;
    BOOL           bOverWrite = (BOOL)pParam1;

    F_CHAR  new_sub_path[LMN_MAX_FILE_PATH];
#ifdef WIN32
    _snwprintf( new_sub_path, LMN_MAX_FILE_PATH, L"%s\\%s", new_path, file_name );
#else
    snprintf( new_sub_path, LMN_MAX_FILE_PATH, "%s/%s", new_path, file_name );
#endif

    if ( bFolder )
    {
        if ( !CreateFolder_(new_sub_path) )
        {
            return FALSE;
        }
    }

    if ( !MoveFile_( full_path, new_sub_path, bFolder, bOverWrite ) )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

static  BOOL   MoveFile_ ( const F_CHAR * old_path, const F_CHAR * new_path, BOOL bFolder, BOOL bOverWrite )
{
    // 如果是目录
    if ( bFolder )
    {
        if ( 0 != TraverseDirectory( old_path, TraverseMoveFile_, (void *)new_path, (void *)bOverWrite ) )
        {
            return FALSE;
        }
        return TRUE;
    }
    // 如果是文件
    else
    {
#ifdef WIN32
        return (::MoveFileW( old_path, new_path ) != 0 ? TRUE : FALSE );
#else
        return MoveSingleFile_( old_path, new_path, TRUE ) ? TRUE : FALSE;
#endif
    }
}

static  FILE *  OpenFile_ ( const F_CHAR * szFile, const F_CHAR * szMode )
{
#ifdef WIN32
    return _wfopen( szFile, szMode );
#else
    return fopen( szFile, szMode );
#endif
}

static  BOOL  GetFileSize_( const F_CHAR * path, BOOL bFolder, DWORD * pdwSize );

static BOOL TraverseGetFileSize_( const F_CHAR * full_path, BOOL bFolder, const F_CHAR * file_name, void * pParam0, void * pParam1 )
{
    DWORD * pdwSize = ( DWORD * )pParam0;

    return GetFileSize_( full_path, bFolder, pdwSize );
}

static  BOOL  GetFileSize_( const F_CHAR * path, BOOL bFolder, DWORD * pdwSize )
{
    if ( bFolder )
    {
        if ( 0 != TraverseDirectory( path, TraverseGetFileSize_, (void *)pdwSize, 0 ) )
        {
            return FALSE;
        }
        return TRUE;
    }
    else
    {
#ifdef WIN32
        FILE * fp = OpenFile_( path, L"rb" );
#else
        FILE * fp = OpenFile_( path, "rb" );
#endif
        if ( 0 == fp )
        {
            return FALSE;
        }

        fseek( fp, 0, SEEK_END );
        long nSize = ftell( fp );
        *pdwSize += nSize;

        fclose( fp );

        return TRUE;
    }
}




COMMONLMN_API  int  LmnCreateFolder( IN const char * szPathName, BOOL bIgnoreExisted /*= TRUE*/ )
{
    if ( 0 == szPathName )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    char  szPath[LMN_MAX_FILE_PATH] = { 0 };
    strncpy( szPath, szPathName, LMN_MAX_FILE_PATH - 1 );

    // 去掉空格
    StrTrim( szPath );
    // 标准化斜杠
    NormalizePath_( szPath );
    // 去掉最后一个斜杠
    RemoveLastSlash( szPath );

    // 计算长度
    DWORD  dwLen = strlen(szPath);
    if ( 0 == dwLen )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    F_CHAR * pPath = 0;

#ifdef WIN32
    wchar_t  wszPath[LMN_MAX_FILE_PATH] = { L'\0' };
    int nCount = MultiByteToWideChar( CP_ACP, 0, szPath, dwLen, wszPath, LMN_MAX_FILE_PATH - 1 );
    if ( 0 == nCount )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    pPath = wszPath;
#else
    pPath = szPath;
#endif


    ELmnFileType eFileType = GetFileType_( pPath );
    // 同名文件存在
    if ( E_FILE_TYPE_FILE == eFileType )
    {
        return LMN_ERR_FILE_EXISTS;
    }
    else if ( E_FILE_TYPE_FOLDER == eFileType )
    {
        if ( !bIgnoreExisted )
        {
            return LMN_ERR_FOLDER_EXISTS;
        }
        else
        {
            return LMN_ERR_OK;
        }
    }

    if ( CreateFolder_( pPath ) )
    {
        return LMN_ERR_OK;
    }
    else
    {
        return LMN_ERR_UNKNOWN;
    }
}


COMMONLMN_API  FILE * LmnOpenFile ( const char * szFile, const char * szMode )
{
    FILE * fp = 0;

    // 错误参数
    if ( 0 == szFile || 0 == szMode )
    {
        return 0;
    }

#ifdef WIN32
    WCHAR   wszFile[LMN_MAX_FILE_PATH] = { 0 };
    WCHAR   wszMode[64]  = { 0 };

    MultiByteToWideChar( CP_ACP,  0, szFile, strlen(szFile),  wszFile, LMN_MAX_FILE_PATH - 1 );
    MultiByteToWideChar( CP_ACP,  0, szMode, strlen(szMode),  wszMode, (sizeof(wszMode)/sizeof(WCHAR)) - 1 );

    fp = _wfopen( wszFile, wszMode );
#else
    fp = fopen( szFile, szMode );
#endif

    return fp;
}

COMMONLMN_API  DWORD LmnReadFile ( void * buffer, DWORD size, DWORD count, FILE * stream )
{
    return fread( buffer, size, count, stream );
}

COMMONLMN_API  DWORD LmnWriteFile ( const void * buffer, DWORD size, DWORD count, FILE * stream )
{
    return fwrite( buffer, size, count, stream );
}

COMMONLMN_API  void LmnCloseFile( FILE * fp )
{
    fclose( fp );
}

COMMONLMN_API  ELmnFileType  LmnGetFileType( IN const char * szPathName )
{
    if ( 0 == szPathName )
    {
        return E_FILE_TYPE_NONE;
    }

    char  szPath[LMN_MAX_FILE_PATH] = { 0 };
    strncpy( szPath, szPathName, LMN_MAX_FILE_PATH - 1 );

    // 去掉空格
    StrTrim( szPath );
    // 标准化斜杠
    NormalizePath_( szPath );
    // 去掉最后一个斜杠
    RemoveLastSlash( szPath );

    // 计算长度
    DWORD  dwLen = strlen(szPath);
    if ( 0 == dwLen )
    {
        return E_FILE_TYPE_NONE;
    }

    F_CHAR * pPath = 0;
#ifdef WIN32
    wchar_t  wszPath[LMN_MAX_FILE_PATH] = { L'\0' };
    int nCount = MultiByteToWideChar( CP_ACP, 0, szPath, dwLen, wszPath, LMN_MAX_FILE_PATH - 1 );
    if ( 0 == nCount )
    {
        return E_FILE_TYPE_NONE;
    }

    pPath = wszPath;
#else
    pPath = szPath;
#endif

    return GetFileType_( pPath );
}




COMMONLMN_API  int   LmnDeleteFile( IN const char * szPathName )
{
    if ( 0 == szPathName )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    char  szPath[LMN_MAX_FILE_PATH] = { 0 };
    strncpy( szPath, szPathName, LMN_MAX_FILE_PATH - 1 );

    // 去掉空格
    StrTrim( szPath );
    // 标准化斜杠
    NormalizePath_( szPath );
    // 去掉最后一个斜杠
    RemoveLastSlash( szPath );

    // 计算长度
    DWORD  dwLen = strlen(szPath);
    if ( 0 == dwLen )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    F_CHAR * pPath = 0;
#ifdef WIN32
    wchar_t  wszPath[LMN_MAX_FILE_PATH] = { L'\0' };
    int nCount = MultiByteToWideChar( CP_ACP, 0, szPath, dwLen, wszPath, LMN_MAX_FILE_PATH - 1 );
    if ( 0 == nCount )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    pPath = wszPath;
#else
    pPath = szPath;
#endif

    ELmnFileType  eFileType = GetFileType_( pPath );
    if ( E_FILE_TYPE_NONE == eFileType )
    {
        return LMN_ERR_FILE_DOES_NOT_EXIST;
    }

    if ( !DeleteFile_( pPath, ( E_FILE_TYPE_FOLDER == eFileType?TRUE:FALSE ) ) )
    {
        return LMN_ERR_UNKNOWN;
    }

    return LMN_ERR_OK;
}


COMMONLMN_API  int   LmnCopyFile( IN const char * szOldPathName, IN const char * szNewPathName, IN BOOL bOverWrite /*= TRUE*/ )
{
    if ( 0 == szOldPathName || 0 == szNewPathName )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    char  szOldPath[LMN_MAX_FILE_PATH] = { 0 };
    strncpy( szOldPath, szOldPathName, LMN_MAX_FILE_PATH - 1 );

    // 去掉空格
    StrTrim( szOldPath );
    // 标准化斜杠
    NormalizePath_( szOldPath );
    // 去掉最后一个斜杠
    RemoveLastSlash( szOldPath );

    // 计算长度
    DWORD  dwOldLen = strlen(szOldPath);
    if ( 0 == dwOldLen )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }


    char  szNewPath[LMN_MAX_FILE_PATH] = { 0 };
    strncpy( szNewPath, szNewPathName, LMN_MAX_FILE_PATH - 1 );

    // 去掉空格
    StrTrim( szNewPath );
    // 标准化斜杠
    NormalizePath_( szNewPath );
    // 去掉最后一个斜杠
    RemoveLastSlash( szNewPath );

    // 计算长度
    DWORD  dwNewLen = strlen(szNewPath);
    if ( 0 == dwNewLen )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( 0 == StrICmp(szOldPath, szNewPath) )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }


    Convert2AbsolutePath_( szOldPath, sizeof(szOldPath) );
    Convert2AbsolutePath_( szNewPath, sizeof(szNewPath) );

    PList lstOld = InitList();
    PList lstNew = InitList();

    if ( 0 == lstOld || 0 == lstNew )
    {
        return LMN_ERR_NO_MEMORY;
    }


    if ( !ConvertPath_( lstOld, szOldPath ) )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_UNKNOWN;
    }

    if ( !ConvertPath_( lstNew, szNewPath ) )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_UNKNOWN;
    }

    // 路径相等
    if ( IsEqualPath_(lstOld, lstNew) )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_INVALID_ARGUMENT;
    }


    F_CHAR * pOldPath = 0;
    F_CHAR * pNewPath = 0;

#ifdef WIN32
    wchar_t  wszOldPath[LMN_MAX_FILE_PATH] = { L'\0' };
    int nCount = 0;
    nCount = MultiByteToWideChar( CP_ACP, 0, szOldPath, dwOldLen, wszOldPath, LMN_MAX_FILE_PATH - 1 );
    if ( 0 == nCount )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_INVALID_ARGUMENT;
    }

    wchar_t  wszNewPath[LMN_MAX_FILE_PATH] = { L'\0' };
    nCount = MultiByteToWideChar( CP_ACP, 0, szNewPath, dwNewLen, wszNewPath, LMN_MAX_FILE_PATH - 1 );
    if ( 0 == nCount )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_INVALID_ARGUMENT;
    }

    pOldPath = wszOldPath;
    pNewPath = wszNewPath;
#else
    pOldPath = szOldPath;
    pNewPath = szNewPath;
#endif

    ELmnFileType  eFileType = GetFileType_( pOldPath );
    // 如果不是文件/文件夹
    if ( E_FILE_TYPE_NONE == eFileType )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_FILE_DOES_NOT_EXIST;
    }

    // 检查是否为父子目录
    if ( E_FILE_TYPE_FOLDER == eFileType )
    {
        if ( IsParentPath_(lstOld, lstNew) || IsParentPath_(lstNew, lstOld) )
        {
            DeinitList( lstOld );
            DeinitList( lstNew );
            return LMN_ERR_INVALID_ARGUMENT;
        }
    }

    DeinitList( lstOld );
    DeinitList( lstNew );


    // 如果非覆盖方式
    if ( !bOverWrite )
    {
        ELmnFileType  eNewFileType = GetFileType_( pNewPath );
        if ( E_FILE_TYPE_NONE != eNewFileType )
        {
            return LMN_ERR_FILE_EXISTS;
        }
    }
    else
    {
        ELmnFileType  eNewFileType = GetFileType_( pNewPath );
        if ( E_FILE_TYPE_NONE != eNewFileType )
        {
            if ( !DeleteFile_( pNewPath, (E_FILE_TYPE_FOLDER == eNewFileType?TRUE:FALSE) ) )
            {
                return LMN_ERR_UNKNOWN;
            }
        }
    }



    if ( E_FILE_TYPE_FILE == eFileType )
    {
        // 确保父目录存在
        const F_CHAR * pLastSlash = F_STRRCHR( pNewPath, WIDE_SLASH_CHAR );
        if ( pLastSlash )
        {
            F_CHAR  NewPathParent[LMN_MAX_FILE_PATH] = { 0 };
            DWORD  dwNewPathParentLen = pLastSlash - pNewPath;
            F_STRNCPY( NewPathParent, pNewPath, dwNewPathParentLen );

            ELmnFileType  eNewParentFileType = GetFileType_( NewPathParent );
            if ( E_FILE_TYPE_NONE == eNewParentFileType )
            {
                if ( !CreateFolder_( NewPathParent ) )
                {
                    return LMN_ERR_UNKNOWN;
                }
            }
            else if ( E_FILE_TYPE_FILE == eNewParentFileType )
            {
                return LMN_ERR_UNKNOWN;
            }
        }

#ifdef WIN32
        return ::CopyFileW( pOldPath, pNewPath, !bOverWrite ) ? LMN_ERR_OK : LMN_ERR_UNKNOWN;
#else
        return CopySingleFile_( pOldPath, pNewPath, TRUE ) ? LMN_ERR_OK : LMN_ERR_UNKNOWN;
#endif
    }
    else
    {
        if ( !CreateFolder_( pNewPath ) )
        {
            return LMN_ERR_UNKNOWN;
        }

        if ( !CopyFile_( pOldPath, pNewPath, TRUE, TRUE ) )
        {
            return LMN_ERR_UNKNOWN;
        }

        return LMN_ERR_OK;
    }
}



// 移动文件/目录
COMMONLMN_API  int   LmnMoveFile( IN const char * szOldPathName, IN const char * szNewPathName, IN BOOL bOverWrite /*= TRUE*/ )
{
    if ( 0 == szOldPathName || 0 == szNewPathName )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    char  szOldPath[LMN_MAX_FILE_PATH] = { 0 };
    strncpy( szOldPath, szOldPathName, LMN_MAX_FILE_PATH - 1 );

    // 去掉空格
    StrTrim( szOldPath );
    // 标准化斜杠
    NormalizePath_( szOldPath );
    // 去掉最后一个斜杠
    RemoveLastSlash( szOldPath );

    // 计算长度
    DWORD  dwOldLen = strlen(szOldPath);
    if ( 0 == dwOldLen )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }


    char  szNewPath[LMN_MAX_FILE_PATH] = { 0 };
    strncpy( szNewPath, szNewPathName, LMN_MAX_FILE_PATH - 1 );

    // 去掉空格
    StrTrim( szNewPath );
    // 标准化斜杠
    NormalizePath_( szNewPath );
    // 去掉最后一个斜杠
    RemoveLastSlash( szNewPath );

    // 计算长度
    DWORD  dwNewLen = strlen(szNewPath);
    if ( 0 == dwNewLen )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( 0 == StrICmp(szOldPath, szNewPath) )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    Convert2AbsolutePath_( szOldPath, sizeof(szOldPath) );
    Convert2AbsolutePath_( szNewPath, sizeof(szNewPath) );

    PList lstOld = InitList();
    PList lstNew = InitList();

    if ( 0 == lstOld || 0 == lstNew )
    {
        return LMN_ERR_NO_MEMORY;
    }


    if ( !ConvertPath_( lstOld, szOldPath ) )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_UNKNOWN;
    }

    if ( !ConvertPath_( lstNew, szNewPath ) )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_UNKNOWN;
    }

    // 路径相等
    if ( IsEqualPath_(lstOld, lstNew) )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_INVALID_ARGUMENT;
    }

    //PListNode p = GetListHead(lstOld);
    //while( p )
    //{
    //    printf("%s\n", (char*)p->pData);
    //    p = GetNextListNode( p );
    //}

    //p = GetListHead(lstNew);
    //while( p )
    //{
    //    printf("%s\n", (char*)p->pData);
    //    p = GetNextListNode( p );
    //}


    F_CHAR * pOldPath = 0;
    F_CHAR * pNewPath = 0;

#ifdef WIN32
    wchar_t  wszOldPath[LMN_MAX_FILE_PATH] = { L'\0' };
    int nCount = 0;
    nCount = MultiByteToWideChar( CP_ACP, 0, szOldPath, dwOldLen, wszOldPath, LMN_MAX_FILE_PATH - 1 );
    if ( 0 == nCount )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_INVALID_ARGUMENT;
    }

    wchar_t  wszNewPath[LMN_MAX_FILE_PATH] = { L'\0' };
    nCount = MultiByteToWideChar( CP_ACP, 0, szNewPath, dwNewLen, wszNewPath, LMN_MAX_FILE_PATH - 1 );
    if ( 0 == nCount )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_INVALID_ARGUMENT;
    }

    pOldPath = wszOldPath;
    pNewPath = wszNewPath;
#else
    pOldPath = szOldPath;
    pNewPath = szNewPath;
#endif

    ELmnFileType  eFileType = GetFileType_( pOldPath );
    // 如果不是文件/文件夹
    if ( E_FILE_TYPE_NONE == eFileType )
    {
        DeinitList( lstOld );
        DeinitList( lstNew );
        return LMN_ERR_FILE_DOES_NOT_EXIST;
    }

    // 检查是否为父子目录
    if ( E_FILE_TYPE_FOLDER == eFileType )
    {
        if ( IsParentPath_(lstOld, lstNew) || IsParentPath_(lstNew, lstOld) )
        {
            DeinitList( lstOld );
            DeinitList( lstNew );
            return LMN_ERR_INVALID_ARGUMENT;
        }
    }

    DeinitList( lstOld );
    DeinitList( lstNew );

    // 如果非覆盖方式
    if ( !bOverWrite )
    {
        ELmnFileType  eNewFileType = GetFileType_( pNewPath );
        if ( E_FILE_TYPE_NONE != eNewFileType )
        {
            return LMN_ERR_FILE_EXISTS;
        }
    }
    else
    {
        ELmnFileType  eNewFileType = GetFileType_( pNewPath );
        if ( E_FILE_TYPE_NONE != eNewFileType )
        {
            if ( !DeleteFile_( pNewPath, (E_FILE_TYPE_FOLDER == eNewFileType?TRUE:FALSE) ) )
            {
                return LMN_ERR_UNKNOWN;
            }
        }
    }



    if ( E_FILE_TYPE_FILE == eFileType )
    {
        // 确保父目录存在
        const F_CHAR * pLastSlash = F_STRRCHR( pNewPath, WIDE_SLASH_CHAR );
        if ( pLastSlash )
        {
            F_CHAR  NewPathParent[LMN_MAX_FILE_PATH] = { 0 };
            DWORD  dwNewPathParentLen = pLastSlash - pNewPath;
            F_STRNCPY( NewPathParent, pNewPath, dwNewPathParentLen );

            ELmnFileType  eNewParentFileType = GetFileType_( NewPathParent );
            if ( E_FILE_TYPE_NONE == eNewParentFileType )
            {
                if ( !CreateFolder_( NewPathParent ) )
                {
                    return LMN_ERR_UNKNOWN;
                }
            }
            else if ( E_FILE_TYPE_FILE == eNewParentFileType )
            {
                return LMN_ERR_UNKNOWN;
            }
        }

#ifdef WIN32
        return (::MoveFileW( pOldPath, pNewPath ) != 0 ? LMN_ERR_OK : LMN_ERR_UNKNOWN );
#else
        return MoveSingleFile_( pOldPath, pNewPath, TRUE ) ? LMN_ERR_OK : LMN_ERR_UNKNOWN;
#endif
    }
    else
    {
        if ( !CreateFolder_( pNewPath ) )
        {
            return LMN_ERR_UNKNOWN;
        }

        if ( !MoveFile_( pOldPath, pNewPath, TRUE, TRUE ) )
        {
            return LMN_ERR_UNKNOWN;
        }

        DeleteFile_( pOldPath, TRUE );

        return LMN_ERR_OK;
    }



}


COMMONLMN_API  DWORD  LmnGetFileSize( IN const char * szPathName )
{
    DWORD  dwTotalSize = 0;

    if ( 0 == szPathName )
    {
        return dwTotalSize;
    }

    char  szPath[LMN_MAX_FILE_PATH] = { 0 };
    strncpy( szPath, szPathName, LMN_MAX_FILE_PATH - 1 );

    // 去掉空格
    StrTrim( szPath );
    // 标准化斜杠
    NormalizePath_( szPath );
    // 去掉最后一个斜杠
    RemoveLastSlash( szPath );

    // 计算长度
    DWORD  dwLen = strlen(szPath);
    if ( 0 == dwLen )
    {
        return dwTotalSize;
    }

    F_CHAR * pPath = 0;
#ifdef WIN32
    wchar_t  wszPath[LMN_MAX_FILE_PATH] = { L'\0' };
    int nCount = MultiByteToWideChar( CP_ACP, 0, szPath, dwLen, wszPath, LMN_MAX_FILE_PATH - 1 );
    if ( 0 == nCount )
    {
        return dwTotalSize;
    }

    pPath = wszPath;
#else
    pPath = szPath;
#endif

    ELmnFileType  eFileType = GetFileType_( pPath );
    if ( E_FILE_TYPE_NONE == eFileType )
    {
        return dwTotalSize;
    }
    else if ( E_FILE_TYPE_FILE == eFileType )
    {
        if ( !GetFileSize_( pPath, FALSE, &dwTotalSize ) )
        {
            return 0;
        }
    }
    else
    {
        if ( !GetFileSize_( pPath, TRUE, &dwTotalSize ) )
        {
            return 0;
        }
    }


    return dwTotalSize;
}



















enum SubFileTransferState
{
    SUB_FILE_TRANSFER_STATE_NONE,
    SUB_FILE_TRANSFER_STATE_FILE_NAME_LEN_END,
    SUB_FILE_TRANSFER_STATE_FILE_NAME_STRING_END,
    SUB_FILE_TRANSFER_STATE_FILE_TYPE_END,
    SUB_FILE_TRANSFER_STATE_FILE_CONTENT_LEN_END,
    SUB_FILE_TRANSFER_STATE_FILE_CONTENT_END,

    SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_NAME_LEN,
    SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_NAME_STRING,
    SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_TYPE,
    SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_CONTENT_LEN,
    SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_CONTENT
};



typedef struct tagSubFileNode
{
    ELmnFileType    eType;                // 文件/文件夹的类型
    F_CHAR *        pFileName;            // 文件/文件夹名(短名称)

    // 和传输进度相关
    FILE *          fp;                   // 文件指针(文件类型有效)
    DWORD           dwPos;                // 已读进度(长度，文件名，文件数据内容等，视eState而定)

    DWORD           dwTransferSize;       // 要传输的大小
    DWORD           dwTransferedSize;

    SubFileTransferState  eState;
}TSubFileNode, *PTSubFileNode;



typedef  struct  tagStreamHandle
{
    ELmnFileType    eType;                       // 文件/文件夹类型
    BOOL            bRead;                       // 读或者写
    F_CHAR *        pFilePath;                   // 根路径
    BOOL            bOverWrite;                  // 是否覆盖
    
    PTreeNode       pFiles;                      // 所有的子文件/子目录(用于文件夹类型)
    FILE *          fp;                          // 父文件指针(用于文件类型)
    PTreeNode       pCurNode;
    PTreeNode       pLastNode;

    void *          pDataBuf;                    // 缓冲区，用于写文件夹
    DWORD           dwExpectBufLen;              // 期望接收的缓冲值
    SubFileTransferState  eBufState;             // 写文件时的状态机

    TFileStreamPos  tPos;                        // 打开文件时的位置
}TStreamHandle, *PTStreamHandle;



static void  FreeStreamHandle_ ( TStreamHandle * pHandle )
{
    assert ( pHandle );

    if ( pHandle->pFilePath )
    {
        g_pfnFreeMem( pHandle->pFilePath );
        pHandle->pFilePath = 0;
    }

    if ( pHandle->fp )
    {
        fclose( pHandle->fp );
        pHandle->fp = 0;
    }

    if ( pHandle->pDataBuf )
    {
        dfDestroy( pHandle->pDataBuf );
        g_pfnFreeMem( pHandle->pDataBuf );
        pHandle->pDataBuf = 0;
    }

    if ( pHandle->pFiles )
    {
        PTreeNode pNode = GetNextTreeNode( pHandle->pFiles );
        while( pNode )
        {
            PTSubFileNode pNodeData = (PTSubFileNode)pNode->pData;

            if ( pNodeData )
            {
                if ( pNodeData->fp )
                {
                    fclose( pNodeData->fp );
                    pNodeData->fp = 0;
                }

                if ( pNodeData->pFileName )
                {
                    g_pfnFreeMem( pNodeData->pFileName );
                    pNodeData->pFileName = 0;
                }

                g_pfnFreeMem( pNodeData );
            }

            pNode = GetNextTreeNode( pNode );
        }
        
        DeInitTree( pHandle->pFiles );
        pHandle->pFiles = 0;
    }

    g_pfnFreeMem( pHandle );
}


static char * Unicode2Utf8_( F_CHAR * pSrc, DWORD dwSrcLen, char * szDest, DWORD dwDestSize )
{
#ifdef WIN32
    int nLen = WideCharToMultiByte( CP_UTF8, 0, pSrc, dwSrcLen, szDest, dwDestSize - 1, 0, 0 );
    // 忽略错误
    //if ( 0 == nLen )
    //{
    //    return 0;
    //}
    szDest[nLen] = '\0';
#else
    DWORD dwMin = MIN( dwDestSize - 1, dwSrcLen );
    memcpy( szDest, pSrc, dwMin );
    szDest[dwMin] = '\0';
#endif

    return szDest;
}

static F_CHAR * Utf82Unicde_( const char * szUtf8, DWORD dwSrcLen, F_CHAR * pDest, DWORD dwDestSize )
{
#ifdef WIN32
    int nRet = MultiByteToWideChar( CP_UTF8, 0, szUtf8, dwSrcLen, pDest, dwDestSize - 1 );
    // 忽略错误
    //if ( 0 == nRet )
    //{
    //    return 0;
    //}
    pDest[nRet] = L'\0';
#else
    DWORD dwMin = MIN( dwDestSize - 1, dwSrcLen );
    memcpy( pDest, szUtf8, dwMin );
    pDest[dwMin] = '\0';
#endif
    return pDest;
}



// 从文件夹到文件
// 按名称升序
static  int  TraverseDirectoryOrderly( const F_CHAR * folder, TraverseDirectoryCallback pfnCb, void * pParam0, void * pParam1 )
{
    PList  lstDirs  = 0;
    PList  lstFiles = 0;

    lstDirs  = InitList();
    lstFiles = InitList();

    if ( 0 == lstDirs || 0 == lstFiles )
    {
        return -1;
    }

#ifdef WIN32
    WIN32_FIND_DATAW  data;

    // 构造通配符
    wchar_t  wszPath[LMN_MAX_FILE_PATH] = { 0 };
    _snwprintf( wszPath, LMN_MAX_FILE_PATH, L"%s\\*", folder );

    HANDLE handle = ::FindFirstFileW( wszPath, &data );
    if ( INVALID_HANDLE_VALUE != handle )
    {
        while( FindNextFileW( handle, &data ) )
        {
            // 忽略"."和".."
            if ( 0 == wcscmp( data.cFileName, L".") || 0 == wcscmp( data.cFileName, L".." ) )
            {
                continue;
            }

            BOOL  bFolder = ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? TRUE : FALSE ;

            wchar_t * pFileName = (wchar_t *)g_pfnAllocMem( sizeof(wchar_t) * 260 );
            if ( 0 == pFileName )
            {
                ClearListWithPointerValue(lstDirs);
                ClearListWithPointerValue(lstFiles);
                DeinitList( lstDirs );
                DeinitList( lstFiles );
                return -1;
            }
            wcscpy( pFileName, data.cFileName );


            PList  list_ = 0;
            if ( bFolder )
            {
                list_ = lstDirs;
            }
            else
            {
                list_ = lstFiles;
            }

            PListNode pNode = GetListHead(list_);
            PListNode lastNode = 0;
            while( pNode )
            {
                wchar_t * pName = (wchar_t *)pNode->pData;
                if ( wcscmp( pFileName, pName ) < 0 )
                {
                    break;
                }

                lastNode = pNode;
                pNode = GetNextListNode( pNode );
            }

            pNode = Insert2List( list_, pFileName, lastNode );
            if ( 0 == pNode )
            {
                g_pfnFreeMem(pFileName);
                ClearListWithPointerValue(lstDirs);
                ClearListWithPointerValue(lstFiles);
                DeinitList( lstDirs );
                DeinitList( lstFiles );
                return -1;
            }
        }

        ::FindClose( handle );



        // 遍历文件
        PListNode pNode = GetListHead(lstFiles);
        while( pNode )
        {
            wchar_t * pName = (wchar_t *)pNode->pData;

            wchar_t  wszSubPath[LMN_MAX_FILE_PATH] = { 0 };
            _snwprintf( wszSubPath, LMN_MAX_FILE_PATH, L"%s\\%s", folder, pName );

            // 如果不继续
            if ( !pfnCb( wszSubPath, FALSE, pName, pParam0, pParam1 ) )
            {
                break;
            }
            
            pNode = GetNextListNode( pNode );
        }

        // 遍历目录
        pNode = GetListHead(lstDirs);
        while( pNode )
        {
            wchar_t * pName = (wchar_t *)pNode->pData;

            wchar_t  wszSubPath[LMN_MAX_FILE_PATH] = { 0 };
            _snwprintf( wszSubPath, LMN_MAX_FILE_PATH, L"%s\\%s", folder, pName );

            // 如果不继续
            if ( !pfnCb( wszSubPath, TRUE, pName, pParam0, pParam1 ) )
            {
                break;
            }

            pNode = GetNextListNode( pNode );
        }

        ClearListWithPointerValue(lstDirs);
        ClearListWithPointerValue(lstFiles);
        DeinitList( lstDirs );
        DeinitList( lstFiles );
        return 0;
    }
    else
    {
        ClearListWithPointerValue(lstDirs);
        ClearListWithPointerValue(lstFiles);
        DeinitList( lstDirs );
        DeinitList( lstFiles );
        return -1;
    }
#else

    DIR * dir_ = ::opendir( folder );
    if ( 0 == dir_ )
    {
        return -1;
    }

    struct dirent * dirent_ = readdir(dir_);
    while ( dirent_ )
    {
        // 忽略"."和".."
        if ( !(0 == strcmp( dirent_->d_name, "." ) || 0 == strcmp( dirent_->d_name, ".." )) )
        {
            char  szSubPath[LMN_MAX_FILE_PATH] = { 0 };
            snprintf( szSubPath, LMN_MAX_FILE_PATH, "%s/%s", folder, dirent_->d_name );

            BOOL  bFolder = IsFolder_(szSubPath);

            char * pFileName = (char *)g_pfnAllocMem( sizeof(char) * 260 );
            if ( 0 == pFileName )
            {
                ClearListWithPointerValue(lstDirs);
                ClearListWithPointerValue(lstFiles);
                DeinitList( lstDirs );
                DeinitList( lstFiles );
                return -1;
            }
            strncpy( pFileName, dirent_->d_name, 260 );

            PList  list_ = 0;
            if ( bFolder )
            {
                list_ = lstDirs;
            }
            else
            {
                list_ = lstFiles;
            }

            PListNode pNode = GetListHead(list_);
            PListNode lastNode = 0;
            while( pNode )
            {
                char * pName = (char *)pNode->pData;
                if ( strcmp( pFileName, pName ) < 0 )
                {
                    break;
                }

                lastNode = pNode;
                pNode = GetNextListNode( pNode );
            }

            pNode = Insert2List( list_, pFileName, lastNode );
            if ( 0 == pNode )
            {
                g_pfnFreeMem(pFileName);
                ClearListWithPointerValue(lstDirs);
                ClearListWithPointerValue(lstFiles);
                DeinitList( lstDirs );
                DeinitList( lstFiles );
                return -1;
            }
        }

        dirent_ = readdir(dir_);
    }

    closedir(dir_);


    // 遍历文件
    PListNode pNode = GetListHead(lstFiles);
    while( pNode )
    {
        char * pName = (char *)pNode->pData;

        char  szSubPath[LMN_MAX_FILE_PATH] = { 0 };
        snprintf( szSubPath, LMN_MAX_FILE_PATH, "%s/%s", folder, pName );

        // 如果不继续
        if ( !pfnCb( szSubPath, FALSE, pName, pParam0, pParam1 ) )
        {
            break;
        }

        pNode = GetNextListNode( pNode );
    }

    // 遍历目录
    pNode = GetListHead(lstDirs);
    while( pNode )
    {
        char * pName = (char *)pNode->pData;

        char  szSubPath[LMN_MAX_FILE_PATH] = { 0 };
        snprintf( szSubPath, LMN_MAX_FILE_PATH, "%s/%s", folder, pName );

        // 如果不继续
        if ( !pfnCb( szSubPath, TRUE, pName, pParam0, pParam1 ) )
        {
            break;
        }

        pNode = GetNextListNode( pNode );
    }

    ClearListWithPointerValue(lstDirs);
    ClearListWithPointerValue(lstFiles);
    DeinitList( lstDirs );
    DeinitList( lstFiles );
    return 0;

#endif
}




static BOOL  TraverseGetAllFiles_( const F_CHAR * full_path, BOOL bFolder, const F_CHAR * file_name, void * pParam0, void * pParam1 )
{
    PTreeNode   pParent   = (PTreeNode)pParam0;
    PTreeNode * ppLastNode = (PTreeNode *)pParam1;
    
    TSubFileNode * pFileNode = (TSubFileNode *)g_pfnAllocMem( sizeof(TSubFileNode) );
    if ( 0 == pFileNode )
    {
        return FALSE;
    }
    memset( pFileNode, 0, sizeof(TSubFileNode) );

    pFileNode->eType = ( bFolder ? E_FILE_TYPE_FOLDER : E_FILE_TYPE_FILE );

    DWORD  dwFileNameLen = F_STRLEN(file_name);
    pFileNode->pFileName = (F_CHAR *)g_pfnAllocMem( sizeof(F_CHAR) * (dwFileNameLen + 1) );
    if ( 0 == pFileNode->pFileName )
    {
        g_pfnFreeMem(pFileNode);
        return FALSE;
    }
    F_STRCPY( pFileNode->pFileName, file_name );

    PTreeNode pNode = Insert2Tree( pParent, pFileNode );
    if ( 0 == pNode )
    {
        return FALSE;
    }

    if ( ppLastNode )
    {
        *ppLastNode = pNode;
    }

    if ( bFolder )
    {
        if ( 0 != TraverseDirectoryOrderly( full_path, TraverseGetAllFiles_, (void *)pNode, (void *)ppLastNode ) )
        {
            return FALSE;
        }
    }

    return TRUE;
}














static BOOL GetFilePos_( const F_CHAR * path, BOOL bFolder, TFileStreamPos * pPosition, DWORD dwRootPathLen  );

static BOOL TraverseGetFilePos_( const F_CHAR * full_path, BOOL bFolder, const F_CHAR * file_name, void * pParam0, void * pParam1 )
{
    TFileStreamPos * pPosition = (TFileStreamPos *)pParam0;
    DWORD dwRootPathLen = (DWORD)pParam1;            

    if ( !GetFilePos_(full_path, bFolder, pPosition, dwRootPathLen) )
    {
        return FALSE;
    }
    return TRUE;
}

static BOOL GetFilePos_( const F_CHAR * path, BOOL bFolder, TFileStreamPos * pPosition, DWORD dwRootPathLen  )
{
    DWORD dwLen = F_STRLEN( path );

    if ( bFolder )
    {
        if ( dwLen > dwRootPathLen )
        {
            pPosition->nPos     = 0;
            pPosition->eType    = E_FILE_TYPE_FOLDER;
#ifdef WIN32
            int nLen = WideCharToMultiByte( CP_UTF8, 0, path + dwRootPathLen + 1, dwLen - (dwRootPathLen + 1), 
                                            pPosition->szSubPath, LMN_MAX_FILE_PATH - 1, 0, 0 );
            // 错误
            if ( 0 == nLen )
            {
                return FALSE;
            }
            pPosition->szSubPath[nLen] = '\0';
#else
            F_STRNCPY( pPosition->szSubPath, path + dwRootPathLen + 1, LMN_MAX_FILE_PATH );
#endif
        }

        if ( 0 != TraverseDirectoryOrderly(path, TraverseGetFilePos_, (void *)pPosition, (void *)dwRootPathLen) )
        {
            return FALSE;
        }
        return TRUE;
    }
    else
    {
        FILE * fp = FILE_OPEN( path, READ_MODE );
        if ( 0 == fp )
        {
            return FALSE;
        }

        fseek( fp, 0, SEEK_END );
        long nSize = ftell( fp );
        fclose( fp );

        pPosition->nPos     = nSize;
        pPosition->eType    = E_FILE_TYPE_FILE;

        if ( dwLen > dwRootPathLen )
        {
#ifdef WIN32
            int nLen = WideCharToMultiByte( CP_UTF8, 0, path + dwRootPathLen + 1, dwLen - (dwRootPathLen + 1), 
                                            pPosition->szSubPath, LMN_MAX_FILE_PATH - 1, 0, 0 );
            // 错误
            if ( 0 == nLen )
            {
                return FALSE;
            }
            pPosition->szSubPath[nLen] = '\0';
#else
            F_STRNCPY( pPosition->szSubPath, path + dwRootPathLen + 1, LMN_MAX_FILE_PATH );
#endif
        }

        return TRUE;
    }
}



static F_CHAR * GetSubPath( PTreeNode pNode, F_CHAR * pSubPath, DWORD dwSubPathSize )
{
    TSubFileNode * pNodeData = (TSubFileNode *)pNode->pData;

    F_CHAR   tmpPath0[260], tmpPath1[260];
    F_CHAR * p0, * p1, * p2;
    p0 = tmpPath0;
    p1 = tmpPath1;

    F_STRNCPY( p0, pNodeData->pFileName, 260 );

    PTreeNode pParentNode = GetParentTreeNode( pNode );
    while( pParentNode )
    {
        TSubFileNode * pParentData = (TSubFileNode *)pParentNode->pData;

        if ( pParentData )
        {
            WSNPRINTF( p1, 260, FORMAT_1, pParentData->pFileName, p0 );
            p2 = p1;
            p1 = p0;
            p0 = p2;
        }

        pParentNode = GetParentTreeNode( pParentNode );
    }

    // DWORD dwPathLen = F_STRLEN( p0 );

    F_STRNCPY( pSubPath, p0, dwSubPathSize );

    return pSubPath;
}


static BOOL ReadFile_( PTreeNode pNode, void * buf, DWORD * pdwLeftSize, TStreamHandle * pStreamHandle, DWORD * pdwReadFileSize )
{
    assert( pNode && *pdwLeftSize > 0 );

    char * pBuf = (char *)buf;
    DWORD & dwLeftSize = *pdwLeftSize;
    DWORD & dwReadFileSize = *pdwReadFileSize;

    TSubFileNode * pData = (TSubFileNode *)pNode->pData;
    assert( pData );

    F_CHAR pSubPath[260];
    char szSubPath[260] = { 0 };
    DWORD  dwFileNameLen = 0;
    BOOL bGetSubPath = FALSE;

    if ( SUB_FILE_TRANSFER_STATE_NONE == pData->eState )
    {
        GetSubPath( pNode, pSubPath, sizeof(pSubPath) / sizeof(F_CHAR) );
        Unicode2Utf8_( pSubPath, F_STRLEN(pSubPath), szSubPath, sizeof(szSubPath) );

        dwFileNameLen = strlen( szSubPath );
        bGetSubPath = TRUE;

        char chFileNameLen = (char)dwFileNameLen;

        memcpy( pBuf, &chFileNameLen, 1 );
        pBuf++;
        dwLeftSize--;
        pData->dwPos = 0;
        pData->eState = SUB_FILE_TRANSFER_STATE_FILE_NAME_LEN_END;

        if ( 0 == dwLeftSize )
        {
            return TRUE;
        }
    }

    if ( SUB_FILE_TRANSFER_STATE_FILE_NAME_LEN_END == pData->eState )
    {
        if ( !bGetSubPath )
        {
            GetSubPath( pNode, pSubPath, sizeof(pSubPath) / sizeof(F_CHAR) );
            Unicode2Utf8_( pSubPath, F_STRLEN(pSubPath), szSubPath, sizeof(szSubPath) );

            dwFileNameLen = strlen( szSubPath );
            bGetSubPath = TRUE;
        }

        DWORD  dwCopySize = dwFileNameLen - pData->dwPos;

        if ( dwLeftSize >= dwCopySize )
        {
            memcpy( pBuf, szSubPath + pData->dwPos, dwCopySize );
            pBuf += dwCopySize;
            dwLeftSize -= dwCopySize;
            pData->dwPos = 0;
            pData->eState = SUB_FILE_TRANSFER_STATE_FILE_NAME_STRING_END;
        }
        else
        {
            memcpy( pBuf, szSubPath + pData->dwPos, dwLeftSize );
            pData->dwPos += dwLeftSize;
            pBuf += dwLeftSize;
            dwLeftSize -= dwLeftSize;
        }

        if ( 0 == dwLeftSize )
        {
            return TRUE;
        }
    }

    if ( SUB_FILE_TRANSFER_STATE_FILE_NAME_STRING_END == pData->eState )
    {
        char chType = (char)pData->eType;
        memcpy( pBuf, &chType, 1 );
        pBuf++;
        dwLeftSize--;
        pData->dwPos = 0;
        pData->eState = SUB_FILE_TRANSFER_STATE_FILE_TYPE_END;

        if ( 0 == dwLeftSize )
        {
            return TRUE;
        }
    }


    if ( SUB_FILE_TRANSFER_STATE_FILE_TYPE_END == pData->eState )
    {
        if (  E_FILE_TYPE_FOLDER == pData->eType )
        {
            PTreeNode pNextNode = GetNextTreeNode( pNode );
            pStreamHandle->pCurNode = pNextNode;

            if ( pNextNode )
            {
                return ReadFile_( pNextNode, pBuf, pdwLeftSize, pStreamHandle, pdwReadFileSize );
            }
            else
            {
                return TRUE;
            }
        }
        else
        {
            assert( E_FILE_TYPE_FILE == pData->eType );

            if ( !bGetSubPath )
            {
                GetSubPath( pNode, pSubPath, sizeof(pSubPath) / sizeof(F_CHAR) );
                //Unicode2Utf8_( pSubPath, F_STRLEN(pSubPath), szSubPath, sizeof(szSubPath) );
                //dwFileNameLen = strlen( szSubPath );
                bGetSubPath = TRUE;
            }

            if( 0 == pData->fp )
            {
                // 打开文件
                F_CHAR  tmpFilePath[260];
                WSNPRINTF( tmpFilePath, 260, FORMAT_1, pStreamHandle->pFilePath, pSubPath );
                pData->fp = FILE_OPEN( tmpFilePath, READ_MODE );
                if ( 0 == pData->fp )
                {
                    return FALSE;
                }
            }

            if ( 0 == pData->dwPos )
            {
                long nCurPos = ftell( pData->fp );

                if ( 0 != fseek( pData->fp, 0, SEEK_END ) )
                {
                    return FALSE;
                }

                long nEndPos = ftell( pData->fp );

                if ( 0 != fseek( pData->fp, nCurPos, SEEK_SET ) )
                {
                    return FALSE;
                }

                pData->dwTransferSize = (DWORD)(nEndPos - nCurPos);
            }


            DWORD dwFileSize_n     = htonl(pData->dwTransferSize);

            DWORD  dwCopySize = sizeof(DWORD) - pData->dwPos;

            if ( dwLeftSize >= dwCopySize )
            {
                memcpy( pBuf, (char *)&dwFileSize_n + pData->dwPos, dwCopySize );
                pBuf += dwCopySize;
                dwLeftSize -= dwCopySize;
                pData->dwPos = 0;
                pData->eState = SUB_FILE_TRANSFER_STATE_FILE_CONTENT_LEN_END;
            }
            else
            {
                memcpy( pBuf, (char *)&dwFileSize_n + pData->dwPos, dwLeftSize );
                pData->dwPos += dwLeftSize;

                pBuf += dwLeftSize;
                dwLeftSize -= dwLeftSize;
            }

            if ( 0 == dwLeftSize )
            {
                return TRUE;
            }
        }
    }

    if ( SUB_FILE_TRANSFER_STATE_FILE_CONTENT_LEN_END == pData->eState )
    {
        assert( pData->fp );

        DWORD dwMinSize = MIN( dwLeftSize, pData->dwTransferSize - pData->dwPos );

        DWORD dwReadCnt = fread( pBuf, 1, dwMinSize, pData->fp );
        while( dwReadCnt > 0 )
        {
            pBuf += dwReadCnt;
            dwLeftSize -= dwReadCnt;
            pData->dwPos += dwReadCnt;
            dwMinSize -= dwReadCnt;
            dwReadFileSize += dwReadCnt;

            if ( 0 == dwLeftSize )
            {
                return TRUE;
            }

            dwReadCnt = fread( pBuf, 1, dwMinSize, pData->fp );
        }


        fclose( pData->fp );
        pData->fp = 0;

        PTreeNode pNextNode = GetNextTreeNode( pNode );
        pStreamHandle->pCurNode = pNextNode;

        if ( pNextNode )
        {
            return ReadFile_( pNextNode, pBuf, pdwLeftSize, pStreamHandle, pdwReadFileSize );
        }
        else
        {
            return TRUE;
        }

    }
    return TRUE;
}


//static const F_CHAR * FindSlash_( const F_CHAR * src )
//{
//    const F_CHAR * pRet = F_STRCHR( src, WIDE_SLASH_CHAR );
//    if ( 0 == pRet )
//    {
//#ifdef WIN32
//        pRet = F_STRCHR( src, L'/' );
//#else
//        pRet = F_STRCHR( src, '\\' );
//#endif
//    }
//    return pRet;
//}


static BOOL WriteFile_( PTreeNode pNode, const void * buf, DWORD * pdwLeftSize, TStreamHandle * pStreamHandle, DWORD * pdwWriteFileSize )
{
    assert( buf && *pdwLeftSize > 0 );

    char * pBuf = (char *)buf;
    DWORD & dwLeftSize = *pdwLeftSize;
    DWORD & dwWriteFileSize= *pdwWriteFileSize;
    DWORD dwTmp;


    if ( SUB_FILE_TRANSFER_STATE_NONE == pStreamHandle->eBufState )
    {
        char chFileNameLen = 0;

        memcpy( &chFileNameLen, buf, 1 );
        pStreamHandle->dwExpectBufLen = (DWORD)chFileNameLen;

        dwLeftSize--;
        pBuf++;

        dfClear( pStreamHandle->pDataBuf );
        pStreamHandle->eBufState = SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_NAME_LEN;

        if ( 0 == dwLeftSize )
        {
            return TRUE;
        }
    }

    if ( SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_NAME_LEN == pStreamHandle->eBufState )
    {
        DWORD dwDataBufLen = dfGetDataLength( pStreamHandle->pDataBuf );

        DWORD dwExpectSize = pStreamHandle->dwExpectBufLen - dwDataBufLen;
        if ( dwLeftSize >= dwExpectSize )
        {
            if ( !dfAppend( pStreamHandle->pDataBuf, pBuf, dwExpectSize ) )
            {
                return FALSE;
            }

            pBuf += dwExpectSize;
            dwLeftSize -= dwExpectSize;
            pStreamHandle->eBufState = SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_NAME_STRING;
        }
        else
        {
            if ( !dfAppend( pStreamHandle->pDataBuf, pBuf, dwLeftSize ) )
            {
                return FALSE;
            }

            pBuf += dwLeftSize;
            dwLeftSize -= dwLeftSize;
        }

        if ( 0 == dwLeftSize )
        {
            return TRUE;
        }
    }

    if ( SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_NAME_STRING == pStreamHandle->eBufState )
    {
        char chType;
        memcpy( &chType, pBuf, 1 );

        if ( !(chType == (char)E_FILE_TYPE_FILE || chType == (char)E_FILE_TYPE_FOLDER) )
        {
            return FALSE;
        }

        char tmpPath[LMN_MAX_FILE_PATH] = { 0 };
        memcpy( tmpPath, (char *)dfGetData(pStreamHandle->pDataBuf), dfGetDataLength(pStreamHandle->pDataBuf) );
        NormalizePath_( tmpPath );
        dwTmp = strlen( tmpPath );


        F_CHAR  pSubPath[260] = { 0 };
        Utf82Unicde_( tmpPath, dwTmp, pSubPath, 260 );


        // 寻找子文件
        F_CHAR     tmpFileName[260];
        PTreeNode  pNode = pStreamHandle->pFiles;
        const F_CHAR *   p0 = pSubPath;
        const F_CHAR *   p1;
        PTreeNode  pParentNode = pNode;

        p1 = F_STRCHR( p0, WIDE_SLASH_CHAR );
        while( p1 )
        {
            DWORD  dwFileNameLen = p1 - p0;
            memcpy( tmpFileName, p0, sizeof(F_CHAR) * dwFileNameLen );
            tmpFileName[dwFileNameLen] = END_CHAR;

            BOOL bFind = FALSE;
            PTreeNode pChildNode = GetChildTreeNode( pNode );
            while( pChildNode )
            {
                TSubFileNode * pData = (TSubFileNode *)pChildNode->pData;
                if ( (0 == F_STRCMP( pData->pFileName, tmpFileName )) && (pData->eType == E_FILE_TYPE_FOLDER) )
                {
                    bFind = TRUE;
                    pParentNode = pChildNode;
                    break;
                }
                pChildNode = GetNextSiblingTreeNode( pChildNode );
            }

            // 从root到此所有的路径都匹配
            if ( bFind )
            {
                pNode = pChildNode;
            }
            // 当前子路径没有找到
            else
            {
                return FALSE;
            }

            p0 = p1 + 1;
            p1 = F_STRCHR( p0, WIDE_SLASH_CHAR );
        }


        DWORD  dwFileNameLen = F_STRLEN( p0 );

        TSubFileNode * pNewFile = (TSubFileNode *)g_pfnAllocMem( sizeof(TSubFileNode) );
        if ( 0 == pNewFile )
        {
            return FALSE;
        }
        memset( pNewFile, 0,sizeof(TSubFileNode) );


        pNewFile->eType = (ELmnFileType)chType;
        pNewFile->pFileName = (F_CHAR *)g_pfnAllocMem( sizeof(F_CHAR) * (dwFileNameLen + 1) );
        if ( 0 == pNewFile->pFileName )
        {
            g_pfnFreeMem( pNewFile );
            return FALSE;
        }
        F_STRCPY( pNewFile->pFileName, p0 );


        WSNPRINTF( tmpFileName, 260, FORMAT_1, pStreamHandle->pFilePath, pSubPath );
        if ( E_FILE_TYPE_FILE == pNewFile->eType )
        {
            pNewFile->fp = FILE_OPEN( tmpFileName, WRITE_MODE );
            if ( 0 == pNewFile->fp )
            {
                g_pfnFreeMem(pNewFile->pFileName);
                g_pfnFreeMem( pNewFile );
                return FALSE;
            }
        }
        else
        {
            assert( E_FILE_TYPE_FOLDER == pNewFile->eType );
            if ( !CreateFolder_( tmpFileName ) )
            {
                g_pfnFreeMem(pNewFile->pFileName);
                g_pfnFreeMem( pNewFile );
                return FALSE;
            }
        }


        PTreeNode pNewNode = Insert2Tree( pParentNode, pNewFile );
        if ( 0 == pNewNode )
        {
            if ( pNewFile->fp )
            {
                fclose( pNewFile->fp );
            }
            g_pfnFreeMem(pNewFile->pFileName);
            g_pfnFreeMem( pNewFile );
            return FALSE;
        }

        pStreamHandle->pCurNode = pNewNode;

        dfClear( pStreamHandle->pDataBuf );
        if ( chType == (char)E_FILE_TYPE_FOLDER )
        {
            pStreamHandle->eBufState = SUB_FILE_TRANSFER_STATE_NONE;
        }
        else
        {
            pStreamHandle->eBufState = SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_TYPE;
        }
        

        pBuf++;
        dwLeftSize--;

        if ( 0 == dwLeftSize )
        {
            return TRUE;
        }
        else
        {
            return WriteFile_( pStreamHandle->pCurNode, pBuf, pdwLeftSize, pStreamHandle, pdwWriteFileSize );
        }
    }


    assert( pNode );
    TSubFileNode * pData = (TSubFileNode *)pNode->pData;
    assert( pData );
    assert( E_FILE_TYPE_FILE == pData->eType );


    if ( SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_TYPE == pStreamHandle->eBufState )
    {
        DWORD dwDataBufLen = dfGetDataLength( pStreamHandle->pDataBuf );

        DWORD dwExpectSize = sizeof(DWORD) - dwDataBufLen;
        if ( dwLeftSize >= dwExpectSize )
        {
            if ( !dfAppend( pStreamHandle->pDataBuf, pBuf, dwExpectSize ) )
            {
                return FALSE;
            }

            DWORD dwTransferSize_n;
            memcpy( &dwTransferSize_n, dfGetData( pStreamHandle->pDataBuf ), sizeof(DWORD) );
            pData->dwTransferSize = ntohl( dwTransferSize_n );

            pBuf += dwExpectSize;
            dwLeftSize -= dwExpectSize;

            dfClear( pStreamHandle->pDataBuf );
            pStreamHandle->eBufState = SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_CONTENT_LEN;
        }
        else
        {
            if ( !dfAppend( pStreamHandle->pDataBuf, pBuf, dwLeftSize ) )
            {
                return FALSE;
            }

            pBuf += dwLeftSize;
            dwLeftSize -= dwLeftSize;
        }

        if ( 0 == dwLeftSize )
        {
            return TRUE;
        }
    }

    if ( SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_CONTENT_LEN == pStreamHandle->eBufState )
    {
        DWORD  dwExpectSize = pData->dwTransferSize - pData->dwTransferedSize;

        if ( dwLeftSize >= dwExpectSize )
        {
            // 写文件
            fwrite( pBuf, 1, dwExpectSize, pData->fp );

            pData->dwTransferedSize += dwExpectSize;

            pBuf += dwExpectSize;
            dwLeftSize -= dwExpectSize;
            dwWriteFileSize += dwExpectSize;
            

            fclose( pData->fp );
            pData->fp = 0;

            pStreamHandle->eBufState = SUB_FILE_TRANSFER_STATE_NONE;
        }
        else
        {
            // 写文件
            fwrite( pBuf, 1, dwLeftSize, pData->fp );

            pData->dwTransferedSize += dwLeftSize;
            dwWriteFileSize += dwLeftSize;
            dwLeftSize -= dwLeftSize;
            pBuf += dwLeftSize;
        }

        if ( 0 == dwLeftSize )
        {
            return TRUE;
        }
        else
        {
            return WriteFile_( pStreamHandle->pCurNode, pBuf, pdwLeftSize, pStreamHandle, pdwWriteFileSize );
        }
    }


    return FALSE;
}








// 打开文件/文件夹流
// 如果是读操作，忽略eType和bOverWrite
COMMONLMN_API  HFileStream  OpenFileStream( const char * szPathName, BOOL bRead, ELmnFileType eType /*= E_FILE_TYPE_FILE*/, 
                                            BOOL bOverWrite /*= FALSE*/, TFileStreamPos * ptPos /*= 0*/ )
{
    // 参数检查
    if ( 0 == szPathName )
    {
        return 0;
    }

    // 如果是写操作，ptPos不能为NULL
    //if ( !bRead && 0 == ptPos )
    //{
    //    return 0;
    //}


    char  szPath[LMN_MAX_FILE_PATH] = { 0 };
    strncpy( szPath, szPathName, LMN_MAX_FILE_PATH - 1 );

    // 去掉空格
    StrTrim( szPath );
    // 标准化斜杠
    NormalizePath_( szPath );
    // 去掉最后一个斜杠
    RemoveLastSlash( szPath );

    // 计算长度
    DWORD  dwLen = strlen(szPath);
    if ( 0 == dwLen )
    {
        return 0;
    }

    F_CHAR * pPath = 0;
#ifdef WIN32
    // 转换为Unicode，用于读写文件。 如果设置环境为中文环境也可以
    wchar_t  wszPath[LMN_MAX_FILE_PATH] = { L'\0' };
    int nCount = MultiByteToWideChar( CP_ACP, 0, szPath, dwLen, wszPath, LMN_MAX_FILE_PATH - 1 );
    if ( 0 == nCount )
    {
        return 0;
    }

    pPath = wszPath;
#else
    pPath = szPath;
#endif

    DWORD  dwPathLen = F_STRLEN(pPath);

    // 如果是只读操作
    if ( bRead )
    {
        eType = GetFileType_(pPath);
        // 如果文件不存在
        if ( E_FILE_TYPE_NONE == eType )
        {
            return 0;
        }

        // 分配句柄内存
        TStreamHandle * pStreamHandle = (TStreamHandle *)g_pfnAllocMem( sizeof(TStreamHandle) );
        if ( 0 == pStreamHandle )
        {
            return 0;
        }
        memset( pStreamHandle, 0, sizeof(TStreamHandle) );


        // 设置基本信息
        pStreamHandle->eType = eType;
        pStreamHandle->bRead = TRUE;

        pStreamHandle->pFilePath = (F_CHAR *)g_pfnAllocMem( sizeof(F_CHAR) * (dwPathLen + 1) );
        if ( 0 == pStreamHandle->pFilePath )
        {
            FreeStreamHandle_(pStreamHandle);
            return 0;
        }
        F_STRCPY( pStreamHandle->pFilePath, pPath );

        pStreamHandle->bOverWrite = FALSE;

        // 文件类型时候不需要tree结构
        if ( E_FILE_TYPE_FILE == eType )
        {
            pStreamHandle->fp = FILE_OPEN( pPath, READ_MODE );
            if ( 0 == pStreamHandle->fp )
            {
                FreeStreamHandle_(pStreamHandle);
                return 0;
            }
        }
        // 文件夹类型，fp设置为0
        else
        {
            pStreamHandle->fp = 0;

            // 初始化tree节点
            pStreamHandle->pFiles = InitTree( 0 );
            if ( 0 == pStreamHandle->pFiles )
            {
                FreeStreamHandle_(pStreamHandle);
                return 0;
            }

            if ( 0 != TraverseDirectoryOrderly( pPath, TraverseGetAllFiles_, (void *)pStreamHandle->pFiles, (void *)&pStreamHandle->pLastNode ) )
            {
                FreeStreamHandle_(pStreamHandle);
                return 0;
            }

            // 设置当前位置，如果是空文件夹，pStreamHandle->pCurNode为NULL
            pStreamHandle->pCurNode = GetChildTreeNode( pStreamHandle->pFiles );
        }
        
        return pStreamHandle;
    }
    else
    {
        if ( !(E_FILE_TYPE_FILE == eType || E_FILE_TYPE_FOLDER == eType) )
        {
            return 0;
        }

        ELmnFileType eExistType = GetFileType_(pPath);

        // 如果是覆盖，删除掉已存在的文件
        if ( bOverWrite )
        {
            if ( E_FILE_TYPE_NONE != eExistType )
            {
                if ( !DeleteFile_( pPath, (E_FILE_TYPE_FOLDER == eExistType ? TRUE: FALSE) ) )
                {
                    return 0;
                }

                eExistType = E_FILE_TYPE_NONE;
            }

            // 如果是目录需要创建目录
            if ( E_FILE_TYPE_FOLDER == eType )
            {
                if ( !CreateFolder_(pPath) )
                {
                    return 0;
                }
            }
        }
        // 非覆盖
        else
        {
            // 有文件存在
            if ( E_FILE_TYPE_NONE != eExistType )
            {
                // 如果已存在文件类型和期望的类型不一样
                if ( eType != eExistType )
                {
                    return 0;
                }
            }
            // 没有文件存在
            else
            {
                // 如果是目录需要创建目录
                if ( E_FILE_TYPE_FOLDER == eType )
                {
                    if ( !CreateFolder_(pPath) )
                    {
                        return 0;
                    }
                }
            }
        }

        // 创建handle
        TStreamHandle * pStreamHandle = (TStreamHandle *)g_pfnAllocMem( sizeof(TStreamHandle) );
        if ( 0 == pStreamHandle )
        {
            return 0;
        }
        memset( pStreamHandle, 0, sizeof(TStreamHandle) );

        // 设置handle基本信息
        pStreamHandle->eType = eType;
        pStreamHandle->bRead = FALSE;
        
        pStreamHandle->pFilePath = (F_CHAR *)g_pfnAllocMem( sizeof(F_CHAR) * (dwPathLen + 1) );
        if ( 0 == pStreamHandle->pFilePath )
        {
            FreeStreamHandle_(pStreamHandle);
            return 0;
        }
        F_STRCPY( pStreamHandle->pFilePath, pPath );

        pStreamHandle->bOverWrite = bOverWrite;


        // 如果是文件类型，设置fp和相关位置
        if ( E_FILE_TYPE_FILE == eType )
        {
            if ( E_FILE_TYPE_NONE == eExistType )
            {
                pStreamHandle->fp = FILE_OPEN( pPath, WRITE_MODE );
                if ( 0 == pStreamHandle->fp )
                {
                    FreeStreamHandle_(pStreamHandle);
                    return 0;
                }
            }
            else
            {
                pStreamHandle->fp = FILE_OPEN( pPath, READ_WRITE_MODE );

                if ( 0 == pStreamHandle->fp )
                {
                    FreeStreamHandle_(pStreamHandle);
                    return 0;
                }

                // 位置
                if ( 0 != fseek( pStreamHandle->fp, 0, SEEK_END ) )
                {
                    FreeStreamHandle_(pStreamHandle);
                    return 0;
                }
            }

            pStreamHandle->tPos.eType        = E_FILE_TYPE_FILE;
            pStreamHandle->tPos.nPos         = ftell( pStreamHandle->fp );
            pStreamHandle->tPos.szSubPath[0] = '\0';

            if ( ptPos )
            {
                memcpy( ptPos, &pStreamHandle->tPos, sizeof(TFileStreamPos) );
            }

            return pStreamHandle;
        }
        // 如果是文件夹类型，设置tree节点
        else
        {
            pStreamHandle->fp = 0;

            pStreamHandle->pDataBuf = dfCreate();
            if ( 0 == pStreamHandle->pDataBuf )
            {
                FreeStreamHandle_(pStreamHandle);
                return 0;
            }

            pStreamHandle->pFiles = InitTree( 0 );
            if ( 0 == pStreamHandle->pFiles )
            {
                FreeStreamHandle_(pStreamHandle);
                return 0;
            }

            if ( 0 != TraverseDirectoryOrderly( pPath, TraverseGetAllFiles_, (void *)pStreamHandle->pFiles, (void *)&pStreamHandle->pLastNode ) )
            {
                FreeStreamHandle_(pStreamHandle);
                return 0;
            }

            // 如果是空目录，pStreamHandle->pCurNode为NULL
            pStreamHandle->pCurNode = pStreamHandle->pLastNode;

            // 如果非空目录
            if ( pStreamHandle->pCurNode )
            {
                // 需要取出写的位置

                TSubFileNode * pNodeData = (TSubFileNode *)pStreamHandle->pCurNode->pData;


                // 子文件类型
                // ptPos->eType = pNodeData->eType;
                pStreamHandle->tPos.eType        = pNodeData->eType;

                F_CHAR pSubPath[260];
                GetSubPath( pStreamHandle->pCurNode, pSubPath, sizeof(pSubPath) / sizeof(F_CHAR) );
                Unicode2Utf8_( pSubPath, F_STRLEN(pSubPath), pStreamHandle->tPos.szSubPath, sizeof(pStreamHandle->tPos.szSubPath) );


                // 子文件进度(内容进度)
                if (  E_FILE_TYPE_FOLDER == pNodeData->eType )
                {
                    pStreamHandle->tPos.nPos = 0;
                }
                else
                {
                    F_CHAR  tmpFullPath[260];

                    WSNPRINTF( tmpFullPath, 260, FORMAT_1, pStreamHandle->pFilePath, pSubPath );
                    pNodeData->fp = FILE_OPEN( tmpFullPath, READ_WRITE_MODE );
                    if ( 0 == pNodeData->fp )
                    {
                        FreeStreamHandle_(pStreamHandle);
                        return 0;
                    }
                    fseek( pNodeData->fp, 0, SEEK_END );
                    pStreamHandle->tPos.nPos = ftell( pNodeData->fp );

                    pStreamHandle->eBufState = SUB_FILE_TRANSFER_STATE_RECEIVED_FILE_TYPE;
                }
                
            }
            // 空目录
            else
            {
                pStreamHandle->tPos.eType        = E_FILE_TYPE_FOLDER;
                pStreamHandle->tPos.nPos         = 0;
                pStreamHandle->tPos.szSubPath[0] = '\0';
            }

            if ( ptPos )
            {
                memcpy( ptPos, &pStreamHandle->tPos, sizeof(TFileStreamPos) );
            }

            return pStreamHandle;
        }
    }
}

COMMONLMN_API int CloseFileStream( IN HFileStream h )
{
    if ( 0 == h )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    TStreamHandle * pStreamHandle = (TStreamHandle *)h;
    FreeStreamHandle_(pStreamHandle);

    return LMN_ERR_OK;
}

COMMONLMN_API  int  GetFileStreamPos( IN HFileStream h, IN TFileStreamPos * pPosition )
{
    if ( 0 == h || 0 == pPosition )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    TStreamHandle * pStreamHandle = (TStreamHandle *)h;
    memcpy( pPosition, &pStreamHandle->tPos, sizeof(TFileStreamPos) );

    return LMN_ERR_OK;
}


// 设置当前位置(对于读操作)
COMMONLMN_API  int  SetFileStreamPos( IN HFileStream h, IN const TFileStreamPos * pPosition )
{
    if ( 0 == h || 0 == pPosition )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !(E_FILE_TYPE_FILE == pPosition->eType || E_FILE_TYPE_FOLDER == pPosition->eType) )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }


    TStreamHandle * pStreamHandle = (TStreamHandle *)h;

    // 必须为读
    if ( !pStreamHandle->bRead )
    {
        return LMN_ERR_UNKNOWN;
    }

    // 如果是文件类型
    if ( E_FILE_TYPE_FILE == pStreamHandle->eType )
    {
        // 如果目标类型不是文件类型
        if ( pPosition->eType != E_FILE_TYPE_FILE )
        {
            return LMN_ERR_UNKNOWN;
        }
        
        // 如果目标类型的子路径不是空白字符串
        if ( 0 != strlen( pPosition->szSubPath) )
        {
            return LMN_ERR_UNKNOWN;
        }

        // 如果文件没有打开
        if ( 0 == pStreamHandle->fp )
        {
            return LMN_ERR_UNKNOWN;
        }

        // 如果fseek失败
        if ( 0 != fseek( pStreamHandle->fp, pPosition->nPos, SEEK_SET ) )
        {
            return LMN_ERR_UNKNOWN;
        }

        // 如果设定的位置不等于目标位置
        if ( ftell( pStreamHandle->fp ) != (long)pPosition->nPos )
        {
            return LMN_ERR_UNKNOWN;
        }

        return LMN_ERR_OK;
    }
    // 文件夹
    else
    {
        // 如果目标的子路径是空文件夹（写文件的那一端没有写入任何数据），设置下一个子文件为读(恰好和tree的中序吻合)
        if ( '\0' == pPosition->szSubPath[0] )
        {
            pStreamHandle->pCurNode = GetChildTreeNode( pStreamHandle->pFiles );
            return LMN_ERR_OK;
        }

        // 目标的子路径不是空字符串
        F_CHAR pDestPath[260];

        char tmpPath[LMN_MAX_FILE_PATH] = { 0 };
        memcpy( tmpPath, pPosition->szSubPath, LMN_MAX_FILE_PATH );
        NormalizePath_( tmpPath );

        DWORD dwSubPathLen = strlen( tmpPath );

        // 转化字符串(win32下，到Unicode)
        Utf82Unicde_( tmpPath, dwSubPathLen, pDestPath, 260 );


        // 寻找子文件
        F_CHAR     tmpFileName[260];
        PTreeNode  pNode = pStreamHandle->pFiles;
        F_CHAR *   p0 = pDestPath;
        F_CHAR *   p1;
        PTreeNode  pCurNode = 0;

        p1 = F_STRCHR( p0, WIDE_SLASH_CHAR );
        while( TRUE )
        {
            ELmnFileType eExpectType;

            if ( p1 )
            {
                DWORD  dwFileNameLen = p1 - p0;
                memcpy( tmpFileName, p0, sizeof(F_CHAR) * dwFileNameLen );
                tmpFileName[dwFileNameLen] = END_CHAR;
                eExpectType = E_FILE_TYPE_FOLDER;
            }
            else
            {
                F_STRNCPY( tmpFileName, p0, 260 );
                eExpectType =  pPosition->eType;
            }

            BOOL bFind = FALSE;
            PTreeNode pChildNode = GetChildTreeNode( pNode );
            while( pChildNode )
            {
                TSubFileNode * pData = (TSubFileNode *)pChildNode->pData;
                if ( (0 == F_STRCMP( pData->pFileName, tmpFileName )) && (pData->eType == eExpectType) )
                {
                    bFind = TRUE;
                    pCurNode = pChildNode;
                    break;
                }
                pChildNode = GetNextSiblingTreeNode( pChildNode );
            }

            // 从root到此所有的路径都匹配
            if ( bFind )
            {
                pNode = pChildNode;
            }
            // 当前子路径没有找到
            else
            {
                return LMN_ERR_UNKNOWN;
            }

            if ( 0 == p1 )
            {
                break;
            }

            p0 = p1 + 1;
            p1 = F_STRCHR( p0, WIDE_SLASH_CHAR );
        }

        if ( pStreamHandle->pCurNode )
        {
            TSubFileNode * pLastCurData = (TSubFileNode *)pStreamHandle->pCurNode->pData;

            if ( pLastCurData->fp )
            {
                fclose( pLastCurData->fp );
                pLastCurData->fp = 0;
            }

            pLastCurData->eState = SUB_FILE_TRANSFER_STATE_NONE;
            pLastCurData->dwPos  = 0;
            pLastCurData->dwTransferSize = 0;

            pStreamHandle->pCurNode = 0;
        }
       
        assert( pCurNode );
        TSubFileNode * pCurData = (TSubFileNode *)pCurNode->pData;

        // 如果是文件类型，需要打开
        if ( E_FILE_TYPE_FILE == pCurData->eType )
        {
            WSNPRINTF( tmpFileName, 260, FORMAT_1, pStreamHandle->pFilePath, pDestPath );

            pCurData->fp = FILE_OPEN( tmpFileName, READ_MODE );
            if ( 0 == pCurData->fp )
            {
                return LMN_ERR_UNKNOWN;
            }

            if ( 0 != fseek( pCurData->fp, pPosition->nPos, SEEK_SET ) )
            {
                fclose( pCurData->fp );
                pCurData->fp = 0;
                return LMN_ERR_UNKNOWN;
            }

            // 如果文件大小和预期不一致
            if ( (long)ftell(pCurData->fp) != pPosition->nPos )
            {
                fclose( pCurData->fp );
                pCurData->fp = 0;
                return LMN_ERR_UNKNOWN;
            }

            pCurData->dwPos = 0;
            pCurData->dwTransferSize = 0;
            pCurData->eState = SUB_FILE_TRANSFER_STATE_FILE_TYPE_END;

            pStreamHandle->pCurNode = pCurNode;
        }
        // 文件夹类型
        else
        {
            // 设置路径为下一个子文件
            pStreamHandle->pCurNode = GetNextTreeNode( pCurNode );
        }

        return LMN_ERR_OK;
    }

    return LMN_ERR_OK;
}

// 读取(对于读操作)
// 文件类型： 纯数据
// 文件夹：   subpath + 类型(文件/文件夹) + 数据(文件类型才有数据)
//            subpath: string类型， 长度(1字节) + string
//            类型：   E_FILE_TYPE_FILE, E_FILE_TYPE_FOLDER(1字节)
//            数据：   长度(4字节，网络序)+纯数据
COMMONLMN_API  int ReadFileStream( HFileStream h, void * buf, DWORD * pdwBufSize, OUT DWORD * pdwReadFileSize /*= 0*/ )
{
    if ( 0 == h || 0 == buf || 0 == pdwBufSize || 0 == *pdwBufSize )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    TStreamHandle * pStreamHandle = (TStreamHandle *)h;

    // 必须为读
    if ( !pStreamHandle->bRead )
    {
        return LMN_ERR_UNKNOWN;
    }

    if ( E_FILE_TYPE_FILE == pStreamHandle->eType )
    {
        if ( 0 == pStreamHandle->fp )
        {
            return LMN_ERR_UNKNOWN;
        }

        DWORD dwReadCnt = fread( buf, 1, *pdwBufSize, pStreamHandle->fp );
        *pdwBufSize = dwReadCnt;

        return LMN_ERR_OK;
    }
    else
    {
        // 读的位置已经到最末
        if ( 0 == pStreamHandle->pCurNode )
        {
            *pdwBufSize = 0;
            return LMN_ERR_OK;
        }

        DWORD dwLeftSize = *pdwBufSize;
        DWORD dwReadFileSize = 0;

        if ( !ReadFile_( pStreamHandle->pCurNode, buf, &dwLeftSize, pStreamHandle, &dwReadFileSize ) )
        {
            return LMN_ERR_UNKNOWN;
        }

        *pdwBufSize = *pdwBufSize - dwLeftSize;

        if ( pdwReadFileSize )
        {
            *pdwReadFileSize = dwReadFileSize;
        }

        return LMN_ERR_OK;
    }
}

// 写入(对于写操作)
COMMONLMN_API  int WriteFileStream( IN HFileStream h, IN const void * buf, INOUT DWORD * pdwBufSize, OUT DWORD * pdwWriteFileSize /*= 0*/ )
{
    if ( 0 == h || 0 == buf || 0 == pdwBufSize )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    TStreamHandle * pStreamHandle = (TStreamHandle *)h;

    // 必须为写
    if ( pStreamHandle->bRead )
    {
        return LMN_ERR_UNKNOWN;
    }

    // 如果写入大小为0
    if ( 0 == *pdwBufSize )
    {
        return LMN_ERR_OK;
    }

    if ( E_FILE_TYPE_FILE == pStreamHandle->eType )
    {
        if ( 0 == pStreamHandle->fp )
        {
            return LMN_ERR_UNKNOWN;
        }

        DWORD dwWriteCnt = fwrite( buf, 1, *pdwBufSize, pStreamHandle->fp );
        *pdwBufSize = dwWriteCnt;

        if ( pdwWriteFileSize )
        {
            *pdwWriteFileSize = dwWriteCnt;
        }

        return LMN_ERR_OK;
    }
    else
    {
        DWORD dwLeftSize = *pdwBufSize;

        DWORD dwWriteFileSize = 0;

        if ( !WriteFile_( pStreamHandle->pCurNode, buf, &dwLeftSize, pStreamHandle, &dwWriteFileSize ) )
        {
            return LMN_ERR_UNKNOWN;
        }

        *pdwBufSize = *pdwBufSize - dwLeftSize;

        assert( 0 != *pdwBufSize );

        if ( pdwWriteFileSize )
        {
            *pdwWriteFileSize = dwWriteFileSize;
        }

        return LMN_ERR_OK;
    }

    return LMN_ERR_OK;
}




















