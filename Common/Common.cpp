// Common.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include <malloc.h>
#include <string.h>
#include "Inner.h"

#if VLD_ON
#ifdef WIN32
#include "vld.h"
#endif
#endif

AllocMemCb  g_pfnAllocMem = malloc;
FreeMemCb   g_pfnFreeMem  = free;



#ifdef WIN32

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


#endif


/****************************************************************************
 * 函数名：  SetMemoryCallbackFunc                                          *
 * 功  能：  设置自定义的内存回调函数                                       *
 * 说  明：  可选，默认的是系统的malloc和free                               *
 ****************************************************************************/
COMMONLMN_API int SetMemoryCallbackFunc( AllocMemCb pfnAlloc, FreeMemCb pfnFree)
{
    if ( 0 == pfnAlloc || 0 == pfnFree )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }
    
    g_pfnAllocMem = pfnAlloc;
    g_pfnFreeMem  = pfnFree;

    return LMN_ERR_OK;
}














COMMONLMN_API  LmnCommon::CDataBuf::CDataBuf( const void * pData /*= 0*/, DWORD dwDataLen /*= 0*/ )
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

COMMONLMN_API  LmnCommon::CDataBuf::~CDataBuf()
{
    if ( m_pData )
    {
        g_pfnFreeMem( m_pData );
    }
}


#define  MIN_BLOCK_SIZE    64

// 用自定义的方法计算需要块的实际分配内存大小
COMMONLMN_API  DWORD  LmnCommon::CDataBuf::CalcReqBufLen( DWORD dwReqLen )
{
    DWORD  dwMin = MIN_BLOCK_SIZE;

    while( dwMin <= dwReqLen )
    {
        dwMin *= 2;
    }

    return dwMin;
}


COMMONLMN_API BOOL LmnCommon::CDataBuf::Append( const void * pData, DWORD dwDataLen )
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

        if ( m_pData )
        {
            // 释放旧数据
            g_pfnFreeMem( m_pData );
        }

        m_pData = pTmpData;
    }

    memcpy( m_pData + m_dwDataLen, pData, dwDataLen );
    m_dwDataLen += dwDataLen;

    return TRUE;
}

COMMONLMN_API void LmnCommon::CDataBuf::Clear()
{
    m_dwDataLen = 0;
    m_dwRPos    = 0;
}

COMMONLMN_API BOOL LmnCommon::CDataBuf::Read( void * pData, DWORD dwDataSize /*= -1*/ )
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


COMMONLMN_API void LmnCommon::CDataBuf::ResetReadPos()
{
    m_dwRPos = 0;
}

COMMONLMN_API BOOL LmnCommon::CDataBuf::SetReadPos( DWORD dwPos )
{
    // 移到末尾
    if ( (DWORD)-1 == dwPos )
    {
        m_dwRPos = m_dwDataLen;
        return TRUE;
    }
    else
    {
        if ( dwPos > m_dwDataLen )
        {
            return FALSE;
        }

        m_dwRPos = dwPos;
        return TRUE;
    }
}



COMMONLMN_API DWORD  LmnCommon::CDataBuf::GetDataLength() const
{
    return m_dwDataLen - m_dwRPos;
}

COMMONLMN_API const void *  LmnCommon::CDataBuf::GetData() const
{
    return (m_pData + m_dwRPos);
}

COMMONLMN_API DWORD   LmnCommon::CDataBuf::GetReadPos() const
{
    return m_dwRPos;
}

COMMONLMN_API  void   LmnCommon::CDataBuf::Reform()
{
    if ( m_dwRPos > 0 && GetDataLength() > 0 )
    {
        memmove( m_pData, m_pData + m_dwRPos, GetDataLength() );

        m_dwDataLen -= m_dwRPos;
        m_dwRPos = 0;
    }
}




COMMONLMN_API  LmnCommon::CFuncLock::CFuncLock( LmnLockType * pLock )
{
    m_pLock   = pLock;
    m_bLocked = FALSE;

    Lock();
}

COMMONLMN_API  LmnCommon::CFuncLock::~CFuncLock()
{
    UnLock();
}

COMMONLMN_API  void LmnCommon::CFuncLock::UnLock()
{
    if ( m_pLock && m_bLocked )
    {
        LmnUnlock( m_pLock );
        m_bLocked = FALSE;
    }
}

COMMONLMN_API  void LmnCommon::CFuncLock::Lock()
{
    if ( m_pLock && !m_bLocked )
    {
        LmnLock( m_pLock );
        m_bLocked = TRUE;
    }
}







typedef struct tagDataBuf
{
    BYTE *   m_pData;                                       // 数据
    DWORD    m_dwDataBufLen;                                // 数据buf长度
    DWORD    m_dwDataLen;                                   // 有效数据长度
    DWORD    m_dwRPos;                                      // 当前读的位置
}TDataBuf, *PTDataBuf;


static DWORD CalcReqBufLen_( DWORD dwReqLen )
{
    DWORD  dwMin = MIN_BLOCK_SIZE;

    while( dwMin <= dwReqLen )
    {
        dwMin *= 2;
    }

    return dwMin;
}

void * dfCreate()
{
    TDataBuf * pDataBuf = (TDataBuf *)g_pfnAllocMem( sizeof(TDataBuf) );
    if ( pDataBuf )
    {
        memset( pDataBuf, 0, sizeof(TDataBuf) );
    }

    return pDataBuf;
}

BOOL   dfDestroy( void * h )
{
    if ( 0 == h )
    {
        return FALSE;
    }

    TDataBuf * pDataBuf = (TDataBuf *)h;

    if ( pDataBuf->m_pData )
    {
        g_pfnFreeMem( pDataBuf->m_pData );
    }

    return TRUE;
}


BOOL   dfAppend( void * h,const void * pData, DWORD dwDataLen )
{
    if ( 0 == h )
    {
        return FALSE;
    }

    if ( 0 == pData || 0 == dwDataLen )
    {
        return TRUE;
    }


    TDataBuf * pDataBuf = (TDataBuf *)h;

    // 如果缓存不够用
    if ( pDataBuf->m_dwDataLen + dwDataLen > pDataBuf->m_dwDataBufLen )
    {
        DWORD  dwNextBlockSize = CalcReqBufLen_( pDataBuf->m_dwDataLen + dwDataLen );
        BYTE * pTmpData = (BYTE*)g_pfnAllocMem( dwNextBlockSize );
        // 分配内存失败
        if ( 0 == pTmpData )
        {
            return FALSE;
        }
        pDataBuf->m_dwDataBufLen = dwNextBlockSize;

        // 复制数据
        memcpy( pTmpData, pDataBuf->m_pData, pDataBuf->m_dwDataLen );

        if ( pDataBuf->m_pData )
        {
            // 释放旧数据
            g_pfnFreeMem( pDataBuf->m_pData );
        }

        pDataBuf->m_pData = pTmpData;
    }

    memcpy( pDataBuf->m_pData + pDataBuf->m_dwDataLen, pData, dwDataLen );
    pDataBuf->m_dwDataLen += dwDataLen;

    return TRUE;
}


BOOL   dfClear(void * h )
{
    if ( 0 == h )
    {
        return FALSE;
    }

    TDataBuf * pDataBuf = (TDataBuf *)h;

    pDataBuf->m_dwDataLen = 0;
    pDataBuf->m_dwRPos    = 0;

    return TRUE;
}


BOOL   dfRead(void * h, void * pData, DWORD dwDataSize /*= -1*/ )
{
    if ( 0 == h )
    {
        return FALSE;
    }

    TDataBuf * pDataBuf = (TDataBuf *)h;

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
        if ( pDataBuf->m_dwRPos == pDataBuf->m_dwDataLen )
        {
            return TRUE;
        }

        memcpy( pData, pDataBuf->m_pData + pDataBuf->m_dwRPos, pDataBuf->m_dwDataLen - pDataBuf->m_dwRPos );
        pDataBuf->m_dwRPos = pDataBuf->m_dwDataLen;
        return TRUE;
    }
    else
    {
        if ( pDataBuf->m_dwRPos + dwDataSize > pDataBuf->m_dwDataLen )
        {
            return FALSE;
        }

        memcpy( pData, pDataBuf->m_pData + pDataBuf->m_dwRPos, dwDataSize );
        pDataBuf->m_dwRPos += dwDataSize;
        return TRUE;
    }
}

BOOL   dfResetReadPos(void * h)
{
    if ( 0 == h )
    {
        return FALSE;
    }

    TDataBuf * pDataBuf = (TDataBuf *)h;

    pDataBuf->m_dwRPos = 0;

    return TRUE;
}

DWORD  dfGetDataLength(void * h)
{
    if ( 0 == h )
    {
        return 0;
    }

    TDataBuf * pDataBuf = (TDataBuf *)h;

    return pDataBuf->m_dwDataLen;
}

const void * dfGetData(void * h)
{
    if ( 0 == h )
    {
        return 0;
    }

    TDataBuf * pDataBuf = (TDataBuf *)h;

    return pDataBuf->m_pData;
}

DWORD  dfGetReadPos(void * h)
{
    if ( 0 == h )
    {
        return 0;
    }

    TDataBuf * pDataBuf = (TDataBuf *)h;

    return pDataBuf->m_dwRPos;
}

BOOL  dfSetReadPos( void * h, DWORD dwPos )
{
    if ( 0 == h )
    {
        return FALSE;
    }

    TDataBuf * pDataBuf = (TDataBuf *)h;

    // 移到末尾
    if ( (DWORD)-1 == dwPos )
    {
        pDataBuf->m_dwRPos = pDataBuf->m_dwDataLen;
        return TRUE;
    }
    else
    {
        if ( pDataBuf->m_dwRPos + dwPos > pDataBuf->m_dwDataLen )
        {
            return FALSE;
        }

        pDataBuf->m_dwRPos += dwPos;
        return TRUE;
    }
}


