#define VLD_SWITCH 1

#if defined(WIN32) && VLD_SWITCH
#include "vld.h"
#endif

#ifdef _IOS_PLATFORM_
#include <malloc/malloc.h>
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include <time.h>
#include <assert.h>
#include <stdarg.h>

#ifndef WIN32
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>
#include <sys/time.h>
#endif


#include "inner.h"

AllocMemCb  g_pfnAllocMem = malloc;
FreeMemCb   g_pfnFreeMem  = free;

// fun_wb_header_callback  g_pfnHeaderCallback = 0;

// CGlobalDataWeibo *  CGlobalDataWeibo::s_pInstance = 0;


LmnLockType   g_Lock;




/*************************       互斥锁   **************************************/
int  LmnInitLock  ( LmnLockType * pLock )
{
    if ( 0 == pLock )
    {
        return -1;
    }

#ifdef WIN32
    InitializeCriticalSection( pLock );
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init( pLock, &attr );
    pthread_mutexattr_destroy( &attr );
#endif

    return 0;
}

int  LmnDeinitLock( LmnLockType * pLock )
{
    if ( 0 == pLock )
    {
        return -1;
    }

#ifdef WIN32
    DeleteCriticalSection( pLock );
#else
    pthread_mutex_destroy( pLock );
#endif

    return 0;
}

int  LmnLock( LmnLockType * pLock )
{
    if ( 0 == pLock )
    {
        return -1;
    }

#ifdef WIN32
    EnterCriticalSection( pLock );
#else
    pthread_mutex_lock( pLock );
#endif

    return 0;
}

int  LmnUnlock( LmnLockType * pLock )
{
    if ( 0 == pLock )
    {
        return -1;
    }

#ifdef WIN32
    LeaveCriticalSection( pLock );
#else
    pthread_mutex_unlock( pLock );
#endif

    return 0;
}
/*************************    END 互斥锁   **************************************/



DWORD  LmnGetTickCount()                       //单位：毫秒
{
#ifdef WIN32
    return GetTickCount();
#else
    struct tms t;
    memset( &t, 0, sizeof(t) );

    unsigned long ticks = (unsigned long) times( &t );
    unsigned long HZ    = sysconf( _SC_CLK_TCK );
    return ticks * ( 1000 / HZ  );
#endif
}


CDataBuf::CDataBuf( void * pData /*= 0*/, DWORD dwDataLen /*= 0*/ )
{
    m_pData         = 0;
    m_dwDataLen     = 0;
    m_dwRPos        = 0;
    m_dwDataBufLen  = 0;

    if ( 0 == dwDataLen )
    {
        return;
    }

    DWORD dwNextBlockSize = CalcReqBufLen( dwDataLen );
    m_pData = (BYTE*)g_pfnAllocMem( dwNextBlockSize );
    // 分配内存失败
    if ( 0 == m_pData )
    {
        return;
    }
    m_dwDataBufLen = dwNextBlockSize;

    if ( pData )
    {
        // 复制数据
        memcpy( m_pData, pData, dwDataLen );
        m_dwDataLen = dwDataLen;
    }
}

CDataBuf::~CDataBuf()
{
    if ( m_pData )
    {
        g_pfnFreeMem( m_pData );
    }
}


#define  MIN_BLOCK_SIZE    64

// 用自定义的方法计算需要块的实际分配内存大小
DWORD  CDataBuf::CalcReqBufLen( DWORD dwReqLen )
{
    DWORD  dwMin = MIN_BLOCK_SIZE;

    while( dwMin <= dwReqLen )
    {
        dwMin *= 2;
    }

    return dwMin;
}


BOOL CDataBuf::Append( const void * pData, DWORD dwDataLen )
{
    if ( 0 == pData || 0 == dwDataLen )
    {
        return TRUE;
    }

    // 如果缓存不够用
    if ( m_dwDataLen + dwDataLen > m_dwDataBufLen )
    {
        DWORD  dwNextBlockSize = CalcReqBufLen( m_dwDataLen + dwDataLen );
        BYTE * pTmpData = (BYTE*)g_pfnAllocMem( dwNextBlockSize );
        // 分配内存失败
        if ( 0 == pTmpData )
        {
            return FALSE;
        }
        m_dwDataBufLen = dwNextBlockSize;

        // 复制数据
        memcpy( pTmpData, m_pData, m_dwDataLen );
        // 释放旧数据
        g_pfnFreeMem( m_pData );

        m_pData = pTmpData;
    }

    memcpy( m_pData + m_dwDataLen, pData, dwDataLen );
    m_dwDataLen += dwDataLen;

    return TRUE;
}

void CDataBuf::Clear()
{
    m_dwDataLen = 0;
    m_dwRPos    = 0;
}

BOOL CDataBuf::Read( void * pData, DWORD dwDataSize /*= -1*/ )
{
    if ( 0 == dwDataSize )
    {
        return TRUE;
    }

    if ( 0 == pData )
    {
        return FALSE;
    }

    // 读取剩余的所有数据
    if ( (DWORD)-1 == dwDataSize )
    {
        // 已经是最末
        if ( m_dwRPos == m_dwDataLen )
        {
            return TRUE;
        }

        memcpy( pData, m_pData + m_dwRPos, m_dwDataLen - m_dwRPos );
        m_dwRPos = m_dwDataLen;
        return TRUE;
    }
    else
    {
        if ( m_dwRPos + dwDataSize > m_dwDataLen )
        {
            return FALSE;
        }

        memcpy( pData, m_pData + m_dwRPos, dwDataSize );
        m_dwRPos += dwDataSize;
        return TRUE;
    }
}

BOOL CDataBuf::ReadAndShift( void * pData, DWORD dwDataSize /*= -1*/ )
{
    if ( !Read( pData, dwDataSize) )
    {
        return FALSE;
    }

    if ( (DWORD)-1 == dwDataSize )
    {
        m_dwDataLen = m_dwRPos;
    }
    else
    {
        m_dwDataLen -= dwDataSize;
    }

    return  TRUE;
}


void CDataBuf::ResetReadPos()
{
    m_dwRPos = 0;
}

DWORD  CDataBuf::GetDataLength() const
{
    return m_dwDataLen;
}

const BYTE *  CDataBuf::GetData() const
{
    return m_pData;
}

DWORD   CDataBuf::GetReadPos() const
{
    return m_dwRPos;
}


CFuncLock::CFuncLock( LmnLockType * pLock )
{
    m_pLock = pLock;
    if ( m_pLock )
    {
        LmnLock( m_pLock );
    }
}

CFuncLock::~CFuncLock()
{
    if ( m_pLock )
    {
        LmnUnlock( m_pLock );
    }
}

char  TOLOWER( char ch )
{
    if ( ch >= 'A' && ch <= 'Z')
    {
        return 'a' + (ch - 'A');
    }
    else
    {
        return ch;
    }
}


int  STRICMP( const char * s1, const char * s2 )
{
    while( TOLOWER(*s1) == TOLOWER(*s2) && '\0' != *s1 )
    {
        s1++;
        s2++;
    }

    if ( TOLOWER(*s1) < TOLOWER(*s2) )
    {
        return -1;
    }
    else if ( TOLOWER(*s1) > TOLOWER(*s2) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int  STRNICMP( const char * s1, const char * s2, unsigned long dwCnt )
{
    unsigned long  dwCmpCnt = 0;

    while( dwCmpCnt < dwCnt && TOLOWER(*s1) == TOLOWER(*s2) && '\0' != *s1 )
    {
        s1++;
        s2++;
        dwCmpCnt++;
    }

    if ( dwCmpCnt == dwCnt )
    {
        return 0;
    }

    if ( TOLOWER(*s1) < TOLOWER(*s2) )
    {
        return -1;
    }
    else if ( TOLOWER(*s1) > TOLOWER(*s2) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}




static BOOL _IsBlankChar( char ch )
{
    if ( ' ' == ch || '\t' == ch || '\r' == ch || '\n' == ch )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/****************************************************************************
 * 函数名：  strtrim                                                        *
 * 功  能：  除去字符串前后的空白字符                                       *
 * 返回值：  0          失败                                                *
 *           非0        裁剪后的字符串                                      *
 ****************************************************************************/
char * StrTrim( char * szStr )
{
    // 如果参数为0
    if ( 0 == szStr )
	{
		return 0;
	}

    // 求得字符串长度
	DWORD dwLen = strlen( szStr );

    // 如果字符串长度为0
    if ( 0 == dwLen )
    {
        return szStr;
    }

	DWORD dwFirstNbPos = 0;             // 第一个不为空白字符的位置
    DWORD dwLastNbPos  = 0;             // 最后一个不为空白字符的位置

    // 计算出第一个不为空白字符的位置
	for ( dwFirstNbPos = 0; dwFirstNbPos < dwLen; dwFirstNbPos++ )
	{
		if ( !_IsBlankChar( szStr[dwFirstNbPos] ) )
		{
			break;
		}
	}

    // 计算出最后一个不为空白字符的位置
	for ( dwLastNbPos = dwLen - 1; dwLastNbPos >= dwFirstNbPos; dwLastNbPos-- )
	{
		if ( !_IsBlankChar( szStr[dwLastNbPos] ) )
		{
			break;
		}
	}

    // 计算前后剪裁掉空白字符后的应有的长度
	DWORD dwNewLen = dwLastNbPos - dwFirstNbPos + 1;

    // 如果长度无变化(无需剪裁)
	if ( dwNewLen == dwLen )
	{
		return szStr;
	}

    // 如果新字符串的长度为0
	if ( 0 == dwNewLen )
	{
		szStr[0] = '\0';
	}
    // 如果新字符串的长度不为0
	else
	{
        // 移动字符串
		memmove( szStr, szStr + dwFirstNbPos, dwNewLen );
		szStr[dwNewLen] = '\0';
	}

	return szStr;
}


/****************************************************************************
 * 函数名：  Str2Lower                                                      *
 * 功  能：  转化为小写字符串                                               *
 * 返回值：  0          失败                                                *
 *           非0        转化后的字符串                                      *
 ****************************************************************************/
char *   Str2Lower( char * szStr )
{
    // 判断参数
    if ( 0 == szStr )
    {
        return 0;
    }

    // 获取字符串长度
    DWORD dwLen = strlen( szStr );

    DWORD i;
    for ( i = 0; i < dwLen; i++ )
    {
        if ( szStr[i] >= 'A' && szStr[i] <= 'Z' )
        {
            szStr[i] = szStr[i] - 'A' + 'a';
        }
    }

    return szStr;
}












#ifdef WIN32
static DWORD WINAPI _WinThrdRuntine( LPVOID lpThreadParameter )
{
    void ** ppArgs    = (void **)lpThreadParameter;

    LmnThreadFunc pFunc = (LmnThreadFunc)ppArgs[0];
    void * pParam     = ppArgs[1];

    void * pRet = pFunc( pParam );

    g_pfnFreeMem( ppArgs );
    ppArgs = 0;

    return (DWORD)pRet;
}
#endif


LmnThrdType LmnCreateThread( LmnThreadFunc pfnThrd, void * pParam, DWORD dwStackSize )
{
    if ( 0 == pfnThrd )
    {
        return 0;
    }

#ifdef WIN32

    void ** ppArgs = (void **)g_pfnAllocMem( sizeof(void *) * 2 );
    if ( 0 == ppArgs )
    {
        return 0;
    }

    ppArgs[0] = pfnThrd;
    ppArgs[1] = pParam;

    LmnThrdType hThrd = ::CreateThread( 0, dwStackSize, _WinThrdRuntine, ppArgs, 0, 0 );
    if ( 0 == hThrd )
    {
        g_pfnFreeMem( ppArgs );
        ppArgs = 0;
        return 0;
    }

    return hThrd;

#else

    LmnThrdType  hThrd;
    int nRet = 0;

    if ( dwStackSize > 0 )
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize( &attr, dwStackSize );

        nRet = pthread_create( &hThrd, &attr, pfnThrd, pParam );

        pthread_attr_destroy(&attr);
    }
    else
    {
        nRet = pthread_create( &hThrd, 0, pfnThrd, pParam );
    }

    if ( 0 == nRet )
    {
        return hThrd;
    }
    else
    {
        return 0;
    }
#endif
}


void *   LmnWait4Thrd( LmnThrdType hThrd )
{
    if ( 0 == hThrd )
    {
        return 0;
    }

#ifdef WIN32

    WaitForSingleObject( hThrd, INFINITE );

    DWORD dwExit;
    if ( GetExitCodeThread( hThrd, &dwExit ) )
    {
        return (void *)dwExit;
    }
    else
    {
        return 0;
    }
#else

    void * pRet;

    if ( 0 == pthread_join( hThrd, &pRet ) )
    {
        return pRet;
    }
    else
    {
        return 0;
    }

#endif
}



















static const char  s_base64_alphabet[] = 
{ 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P', 
'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f', 
'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v', 
'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/', '=' };


// 得到某个字节的前几位
#define GET_FRONT_BITS( b, n )     ((BYTE)((b)>>(8-(n))))
// 得到某个字节的后几位
#define GET_BACK_BITS( b, n )      (((BYTE)((b)<<(8-(n))))>>(8-(n)))







/****************************************************************************
 * 函数名：  _GetBase64Index                                                *
 * 功  能：  根据base4字符获取字母表中的索引                                *
 ****************************************************************************/
static BOOL _GetBase64Index( char chBase64, BYTE * pbyIndex )
{
    if ( chBase64 >= 'A' && chBase64 <= 'Z' )
    {
        *pbyIndex = chBase64 - 'A';
        return TRUE;
    }
    else if ( chBase64 >= 'a' && chBase64 <= 'z' )
    {
        *pbyIndex = chBase64 - 'a' + 26;
        return TRUE;
    }
    else if ( chBase64 >= '0' && chBase64 <= '9' )
    {
        *pbyIndex = chBase64 - '0' + 52;
        return TRUE;
    }
    else if ( chBase64 == '+' )
    {
        *pbyIndex = 62;
        return TRUE;
    }
    else if ( chBase64 == '/' )
    {
        *pbyIndex = 63;
        return TRUE;
    }
    else if ( chBase64 == '=' )
    {
        *pbyIndex = 64;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/****************************************************************************
 * 函数名：  EncodeBase64                                                   *
 * 功  能：  把字节流转换成base64格式                                       *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 ****************************************************************************/
int EncodeBase64( void  *  pBase64, DWORD *  pdwSize, void  *  pSrc, DWORD    dwSrcLen )
{
    // 检查参数
    if ( 0 == pBase64 || 0 == pdwSize || 0 == pSrc )
    {
        return -1;
    }

    BYTE * pbyBase64    = (BYTE *)pBase64;
    const BYTE * pbySrc = (BYTE*)pSrc;

    // 计算被3除后的倍数以及余数
    DWORD dwMultiple  = dwSrcLen / 3;
    DWORD dwResidue   = dwSrcLen % 3;

    // 计算最小需要的缓冲大小
    DWORD dwMinSize = 4 * dwMultiple;
    if ( 0 != dwResidue )
    {
        dwMinSize += 4;
    }

    // 如果缓冲不够大
    if ( *pdwSize < dwMinSize )
    {
        return -1;
    }

    // Base64编码
    // 逻辑：以3个字节为一段，逐个检查每段。如果余下1到2个字节，那就做特别处理
    DWORD i;
    for ( i = 0; i < dwMultiple; i++ )
    {
        const BYTE * pbySegment = pbySrc + 3 * i;
    
        BYTE b1 = GET_FRONT_BITS( pbySegment[0], 6 ) ;
        BYTE b2 =   ( GET_BACK_BITS( pbySegment[0], 2 ) << 4 ) 
                  | GET_FRONT_BITS( pbySegment[1], 4 );
        BYTE b3 = ( GET_BACK_BITS(pbySegment[1],4)<<2 ) 
                  | GET_FRONT_BITS(pbySegment[2], 2);
        BYTE b4 = GET_BACK_BITS( pbySegment[2], 6 );

        BYTE * pbyDest = pbyBase64 + 4 * i;

        pbyDest[0] = s_base64_alphabet[b1];
        pbyDest[1] = s_base64_alphabet[b2];
        pbyDest[2] = s_base64_alphabet[b3];
        pbyDest[3] = s_base64_alphabet[b4];
    }

    const BYTE * pbySegment = pbySrc + 3 * i;
    BYTE *       pbyDest    = pbyBase64 + 4 * i;

    // 如果余1个字节
    if ( 1 == dwResidue )
    {
        BYTE b1 = GET_FRONT_BITS( pbySegment[0], 6 ) ;
        BYTE b2 = ( GET_BACK_BITS( pbySegment[0], 2 )<<4 );

        pbyDest[0] = s_base64_alphabet[b1];
        pbyDest[1] = s_base64_alphabet[b2];
        pbyDest[2] = s_base64_alphabet[64];
        pbyDest[3] = s_base64_alphabet[64];
    }
    // 如果余2个字节
    else if ( 2 == dwResidue )
    {
        BYTE b1 = GET_FRONT_BITS( pbySegment[0], 6 ) ;
        BYTE b2 = ( GET_BACK_BITS( pbySegment[0], 2 )<<4) | GET_FRONT_BITS(pbySegment[1], 4);
        BYTE b3 = GET_BACK_BITS(pbySegment[1],4)<<2;

        pbyDest[0] = s_base64_alphabet[b1];
        pbyDest[1] = s_base64_alphabet[b2];
        pbyDest[2] = s_base64_alphabet[b3];
        pbyDest[3] = s_base64_alphabet[64];
    }

    *pdwSize = dwMinSize;
    return 0;
}


/****************************************************************************
 * 函数名：  DecodeBase64                                                   *
 * 功  能：  把base64格式转换成字节流                                       *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 ****************************************************************************/
 int DecodeBase64(    void  * pDest, DWORD * pdwSize, void  * pBase64, DWORD   dwBase64Len  )
{
    // 检查参数
    if ( 0 == pDest || 0 == pdwSize || 0 == pBase64 )
    {
        return -1;
    }

    BYTE * pbyDest = (BYTE *)pDest;
    const BYTE * pbyBase64 = (BYTE *)pBase64;

    // base64数据格式不对
    if ( 0 != dwBase64Len % 4 )
    {
        return -1;
    }

    // 按4个字节为一段进行分段
    DWORD dwMultiple =  dwBase64Len / 4;

    // 检查缓冲区是否足够
    DWORD dwMinSize = 0;
    const BYTE * pbyLastSegment = 0;

    if ( dwMultiple > 0 )
    {
        dwMinSize = (dwMultiple - 1) * 3;
        pbyLastSegment = pbyBase64 + 4 * (dwMultiple - 1);
        // 如果最后一段倒数第2个字节为'='，还需要1个字节缓冲区
        if ( s_base64_alphabet[64] == pbyLastSegment[2] )
        {
            dwMinSize += 1;
        }
        // 如果最后一段倒数第12个字节为'='，还需要2个字节缓冲区
        else if ( s_base64_alphabet[64] == pbyLastSegment[3] )
        {
            dwMinSize += 2;
        }
        else
        {
            dwMinSize += 3;
        }
    }

    if ( *pdwSize < dwMinSize )
    {
        return -1;
    }
    
    
    DWORD i;
    // 遍历每一段
    for ( i = 0; i < dwMultiple; i++ )
    {
        BYTE * pbySegment   = pbyDest   + 3 * i;
        const BYTE * pbySrc = pbyBase64 + 4 * i;

        BYTE  abyIndex[4];
        DWORD j;
        for ( j = 0; j < 4; j++ )
        {
            // 获取索引失败
            if ( !_GetBase64Index( pbySrc[j], &abyIndex[j] ) )
            {
                return -1;
            }
        }

        if ( 64 == abyIndex[0] || 64 == abyIndex[1] )
        {
            return -1;
        }

        // 如果不是最后一段
        if ( i < dwMultiple - 1 )
        {
            if ( 64 == abyIndex[2] || 64 == abyIndex[3] )
            {
                return -1;
            }

            pbySegment[0] = (abyIndex[0]<<2) | GET_FRONT_BITS(abyIndex[1],4);
            pbySegment[1] = (GET_BACK_BITS(abyIndex[1],4)<<4) | GET_FRONT_BITS(abyIndex[2],6);
            pbySegment[2] = (GET_BACK_BITS(abyIndex[2],2)<<6) | abyIndex[3];
        }
        else
        {
            if ( 64 == abyIndex[2] )
            {
                if ( 64 != abyIndex[3] )
                {
                    return -1;
                }
                
                pbySegment[0] = (abyIndex[0]<<2) | GET_FRONT_BITS(abyIndex[1],4);
            }
            else if ( 64 == abyIndex[3] )
            {   
                pbySegment[0] = (abyIndex[0]<<2) | GET_FRONT_BITS(abyIndex[1],4);
                pbySegment[1] = (GET_BACK_BITS(abyIndex[1],4)<<4) | GET_FRONT_BITS(abyIndex[2],6);
            }
            else
            {
                pbySegment[0] = (abyIndex[0]<<2) | GET_FRONT_BITS(abyIndex[1],4);
                pbySegment[1] = (GET_BACK_BITS(abyIndex[1],4)<<4) | GET_FRONT_BITS(abyIndex[2],6);
                pbySegment[2] = (GET_BACK_BITS(abyIndex[2],2)<<6) | abyIndex[3];
            }
        }
    }

    *pdwSize = dwMinSize;
    return 0;
}


// win32下使用wchar
FILE * LmnOpenFile( const char * szFile, const char * szMode )
{
    FILE * fp = 0;

#ifdef WIN32
    WCHAR   wszFile[256] = { 0 };
    WCHAR   wszMode[256] = { 0 };

    MultiByteToWideChar( CP_ACP,  0, szFile, strlen(szFile),  wszFile, 256 - 1 );
    MultiByteToWideChar( CP_ACP,  0, szMode, strlen(szMode),  wszMode, 256 - 1 );

    fp = _wfopen( wszFile, wszMode );
#else
    fp = fopen( szFile, szMode );
#endif

    return fp;
}

long  LmnGetFileSize( const char * szFile )
{
    FILE * fp = LmnOpenFile( szFile, "rb" );

    if ( 0 == fp )
    {
        return -1;
    }

    fseek( fp, 0, SEEK_END );
    long nSize = ftell( fp );
    fclose( fp );

    return nSize;
}

int  GetDigitalFromChar( char ch )
{
    if ( ch >= 'a' && ch <= 'f' )
    {
        return (ch - 'a') + 10;
    }
    else if ( ch >= 'A' && ch <= 'F' )
    {
        return (ch - 'A') + 10;
    }
    else if ( ch >= '0' && ch <= '9' )
    {
        return (ch - '0');
    }
    else
    {
        return -1;
    }
}

char   GetCharFromUrl( char ch1, char ch2 )
{
    int n1 = GetDigitalFromChar(ch1);
    int n2 = GetDigitalFromChar(ch2);

    assert( n1 >= 0 && n2 >= 0 );

    return (char)(n1 * 16 + n2);
}

char *  GetStringFromUrl( char * szDest, DWORD dwDestSize, const char * szUrl )
{
    const char * p = szUrl;
    char *       q = szDest;

    char  tmp[2] = {0};


    while( '\0' != *p )
    {
        // url 转义字符
        if ( '%' == *p )
        {
            p++;
            assert( *p );
            tmp[0] = *p;

            p++;
            assert( *p );
            tmp[1] = *p;

            *q = GetCharFromUrl( tmp[0], tmp[1] );
            q++;
        }
        else
        {
            *q = *p;
            q++;
        }

        p++;
    }

    *q = '\0';

    return szDest;
}



static POnMicroblogCallback s_pfnLog = 0;

void  SetLogCallback( POnMicroblogCallback pfnLog )
{
    s_pfnLog = pfnLog;
}

void  MicroblgLog( const char * szFormat, ... )
{
    if ( s_pfnLog )
    {
        //时间戳
        time_t now;
        struct tm t;

        char buf[NORMAL_BUF_SIZE] = { 0 };

        time(&now);
        if ( now < 0 )
        {
            now = 0;
        }

        struct tm * pTime = localtime(&now);
        if ( 0 == pTime )
        {
            memset ( &t, 0, sizeof(t) );
        }
        else
        {
            t = *pTime;
        }

        sprintf( buf, "[%04u-%02u-%02u %02u:%02u:%02u] ", 
            t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
            t.tm_hour, t.tm_min, t.tm_sec );

        // 时间戳占用22个字节

        va_list ap;
        va_start( ap, szFormat );
        vsnprintf( buf + 22, NORMAL_BUF_SIZE - 1 - 22, szFormat, ap );
        va_end(ap);

        s_pfnLog( buf );
    }
}

static BOOL  s_bGLockInited = FALSE;

void    InitGLock()
{
    if ( !s_bGLockInited )
    {
        LmnInitLock( &g_Lock );

        s_bGLockInited = TRUE;
    }
}













char * GetIpAddr( char * szBuf, unsigned int dwSize, unsigned int dwIp )
{
    const char * p = (const char *)&dwIp;
    SNPRINTF( szBuf, dwSize, "%d.%d.%d.%d", (int)p[0], (int)p[1], (int)p[2], (int)p[3] );
    return szBuf;
}











class CStaticObj
{
public:
    CStaticObj()
    {
        LmnInitLock( &g_Lock );
    }

    ~CStaticObj()
    {
        LmnDeinitLock( &g_Lock );
    }
};

static CStaticObj s_InitObj;








/****************************************************************************
 * 函数名：  StrReplace                                                     *
 * 功  能：  用字符串szReplaceWith替换字符串szSource中的所有                *
 *           为szToReplace的地方                                            *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 ****************************************************************************/
int StrReplace(  char * szDest, DWORD dwSize, const char * szSource,  const char * szToReplace, const char * szReplaceWith  )
{
    // 检查参数
    if (    0 == szDest        || 0 == szSource || 0 == szToReplace
		 || 0 == szReplaceWith || 0 == dwSize )
	{
		return -1;
	}

    // 定义分别指向szDest和szSource的指针，便于书写
    char *       pchDest       = szDest;
    const char * pchSrc        = szSource;

    // 定义szDest剩余的长度(留一个字符用于结束符)
	DWORD  dwLeftSize          = dwSize - 1;

    // 求出被替代和替代字符串长度
	DWORD  dwToReplaceLen   = strlen( szToReplace );
    DWORD  dwRelpaceWithLen = strlen( szReplaceWith );

    // 查找第一个被替换的位置
	const char * pchFind = strstr( pchSrc, szToReplace );
	while( pchFind )
	{
        // 从pchSrc到pchFind之间的需要保留的字符串的长度
		DWORD dwLen = pchFind - pchSrc;
        
        // 如果缓冲区不够
		if ( dwLen > dwLeftSize )
		{
			memcpy( pchDest, pchSrc, dwLeftSize );
			pchDest[dwLeftSize] = '\0';
			return -1;
		}
		else
		{
			memcpy( pchDest, pchSrc, dwLen );
			pchDest    += dwLen;
			dwLeftSize -= dwLen;

            // 没有足够缓冲区来复制szReplaceWith
			if ( dwRelpaceWithLen > dwLeftSize )
			{
				memcpy( pchDest, szReplaceWith, dwLeftSize );
				pchDest[dwLeftSize] = '\0';
				return -1;
			}
			else
			{
				memcpy( pchDest, szReplaceWith, dwRelpaceWithLen );
				pchDest    += dwRelpaceWithLen;
				dwLeftSize -= dwRelpaceWithLen;

				pchSrc  = pchFind + dwToReplaceLen;
				pchFind = strstr( pchSrc, szToReplace );
			}
		}
	}

    // 没有找到szToReplace剩下的最后一段
    DWORD  dwLastLen  =  strlen( pchSrc );
	if ( dwLastLen > dwLeftSize )
	{
		memcpy( pchDest, pchSrc, dwLeftSize );
		pchDest[dwLeftSize] = '\0';
		return -1;
	}

	memcpy( pchDest, pchSrc, dwLastLen );
    pchDest[dwLastLen] = '\0';
	dwLeftSize -= dwLastLen;
    
    return -1;
}

