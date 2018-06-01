#include <assert.h>
#include <vector>

#include "inner.h"
#include "seli.h"
#include "redis_client.h"

#define  MAX_REDIS_CLIENT_SOCKET_COUNT   8
#define  DEFAULT_REDIS_PORT              6379

static void SynCallback_connect( FdType fd, int liEvent, int error, void * context )
{
    BOOL  * pBool = (BOOL *)context;

    switch ( liEvent )
    {
    case liEvConnect:
        {
            *pBool = TRUE;
        }
        break;

    case liEvClose:
        {
            *pBool = FALSE;
        }
        break;

    default:
        break;
    }
}

REDIS_CLIENT_API  int  ConnectRedis( HRedisClient * ph, const char * szAddr, WORD wPort, DWORD dwTimeout /*= 5*/ )
{
    if ( 0 == ph || 0 == szAddr )
    {
        return -1;
    }

    if ( 0 == wPort )
    {
        wPort = DEFAULT_REDIS_PORT;
    }

    if ( 0 == dwTimeout )
    {
        dwTimeout = 5;
    }

    if ( !IsSeliInited() )
    {
        seliInit(MAX_REDIS_CLIENT_SOCKET_COUNT);
    }

    BOOL  bConnect = FALSE;
    FdType fd = liOpen( LI_PROTOTYPE_TCP, 0, 0, SynCallback_connect, (void *)&bConnect, FALSE, FALSE );
    if ( INVALID_SOCKET == fd )
    {
        return -1;
    }

    struct hostent * pHost = gethostbyname( szAddr );
    if ( 0 == pHost )
    {
        liClose( fd );
        return -1;
    }

    DWORD dwRedisAddr = *reinterpret_cast<DWORD *>(pHost->h_addr_list[0]);

    if ( 0 != liConnect( fd, dwRedisAddr, wPort, dwTimeout ) )
    {
        liClose( fd );
        return -1;
    }
    
    DWORD dwSeliTimeOut = dwTimeout * 1000;
    if ( 0 != seliSelect( &dwSeliTimeOut, fd ) )
    {
        liClose( fd );
        return -1;
    }

    if ( bConnect )
    {
        *ph = (void *)fd;
        return 0;
    }
    else
    {
        liClose( fd );
        return -1;
    }
}

REDIS_CLIENT_API  int  SendRedisData( HRedisClient h,    const void * pSendData,  DWORD dwSendSize, 
                                      void * pRecvData,  DWORD * pdwReceiveSize,  DWORD dwTimeout /*= 5*/ )
{
    if ( 0 == h || 0 == pSendData || 0 == dwSendSize || 0 == pRecvData || 0 == pdwReceiveSize || 0 == *pdwReceiveSize )
    {
        return -1;
    }

    FdType fd = (FdType)h;

    int len = dwSendSize;
    if ( 0 != liTcpSend( fd, (char *)pSendData, &len ) )
    {
        return -1;
    }

    int nRecvLen = *pdwReceiveSize;
    char * p     = (char *)pRecvData;
    int  ret     = 0;
    int  nError  = -1;

    if ( 0 == dwTimeout )
    {
        dwTimeout = 5;
    }

    DWORD dwRecvLeftTime = dwTimeout * 1000;


    len = nRecvLen;
    ret = liTcpRecv( fd, p, &len );
    while ( 0 == ret )
    {
        if ( len > 0 )
        {
            p += len;
            nRecvLen -= len;

            // 如果还有剩余缓冲剩余
            if ( nRecvLen > 0 )
            {
                len = nRecvLen;
                ret = liTcpRecv( fd, p, &len );
            }
            // 没有缓冲剩余
            else
            {
                nError = 1;
                break;
            }
        }
        else if ( 0 == len )
        {
            // 接收到数据过(认为数据全部接收完)
            if ( nRecvLen != (int)(*pdwReceiveSize) )
            {
                nError = 0;
                break;
            }
            // 还没有接收到数据，继续
            else
            {
                // 如果没有超时
                if ( dwRecvLeftTime > 0 )
                {
                    SLEEP(100);

                    if ( dwRecvLeftTime > 100 )
                    {
                        dwRecvLeftTime -= 100;
                    }
                    len = nRecvLen;
                    ret = liTcpRecv( fd, p, &len );
                }
                // 超时
                else
                {
                    nError = 2;
                    break;
                }
            }
        }
        else
        {
            assert( 0 );
            break;
        }
    }


    if ( 0 == nError )
    {
        *pdwReceiveSize -= nRecvLen;
        return 0;
    }
    else
    {
        return nError;
    }

    return 0;
}

REDIS_CLIENT_API  int  CloseRedis( HRedisClient h )
{
    if ( 0 == h )
    {
        return -1;
    }

    FdType fd = (FdType)h;
    int ret = liClose( fd );

    if ( 0 != ret )
    {
        return ret;
    }
    
    return 0;
}






typedef struct tagRedisReqAsyn
{
    char *       m_pRequest;
    DWORD        m_dwRequestLen;
    DWORD        m_dwRequestId;
}TRedisReqAsyn, *PTRedisReqAsyn;


typedef struct tagRedisAsynParam
{
    FdType                            m_fd;
    POnRedisConnectCallback           m_pCnnCallbk;
    POnRedisDataCallback              m_pDataCallback;
    char                              m_szHost[64];
    WORD                              m_wPort;
    char                              m_reserved0[2];
    std::vector<PTRedisReqAsyn>       m_vRequests;
    std::vector<char>                 m_vBuf;
}TRedisAsynParam, *PTRedisAsynParam;


static void AsynCallback_connect( FdType fd, int liEvent, int error, void * context )
{
    PTRedisAsynParam pParam = (PTRedisAsynParam)context;

    switch ( liEvent )
    {
    case liEvConnect:
        {
            pParam->m_pCnnCallbk( pParam, TRUE );
        }
        break;

    case liEvClose:
        {
            pParam->m_pCnnCallbk( pParam, FALSE );
        }
        break;

    case liEvRead:
        {
            char  buf[4096];
            int len = 4096;
            int ret = 0;
            ret = liTcpRecv( fd, buf, &len );
            while( 0 == ret )
            {
                if ( len > 0 )
                {

                }
                else
                {
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

static void * SeliSelectThreadFunc( void * pParam )
{
    while( 1 )
    {
        DWORD  dwTimeout = 200;
        seliSelect( &dwTimeout );
    }
}


static  std::vector<PTRedisAsynParam>   s_vRedisAysnParams;


static void * DnsResolveThreadFunc( void * pParam )
{
    while( 1 )
    {
        LmnLock( &g_Lock );

        std::vector<PTRedisAsynParam>::iterator  it;
        for ( it = s_vRedisAysnParams.begin(); it != s_vRedisAysnParams.end(); it++ )
        {
            PTRedisAsynParam  pParam = *it;

            struct hostent * pHost = gethostbyname( pParam->m_szHost );
            if ( 0 == pHost )
            {
                pParam->m_pCnnCallbk( pParam, FALSE );
                continue;
            }

            DWORD dwRedisAddr = *reinterpret_cast<DWORD *>(pHost->h_addr_list[0]);
            DWORD dwTimeout = 5;

            if ( 0 != liConnect( pParam->m_fd, dwRedisAddr, pParam->m_wPort, dwTimeout ) )
            {
                pParam->m_pCnnCallbk( pParam, FALSE );
                continue;
            }
        }

        s_vRedisAysnParams.clear();
     
        LmnUnlock( &g_Lock );

        SLEEP(200);
    }
}

static LmnThrdType  s_SeliThrd = 0;
static LmnThrdType  s_DnsThrd  = 0;
static BOOL         s_bInited  = FALSE;




REDIS_CLIENT_API  int  ConnectRedisAsyn( HRedisClient * ph, const char * szAddr, WORD wPort, void * pContext, POnRedisConnectCallback pCnnCallbk, POnRedisDataCallback pCallback )
{
    if ( 0 == ph || 0 == szAddr || 0 == pCnnCallbk || 0 == pCallback )
    {
        return -1;
    }

    if ( 0 == wPort )
    {
        wPort = DEFAULT_REDIS_PORT;
    }


    if ( !IsSeliInited() )
    {
        seliInit(MAX_REDIS_CLIENT_SOCKET_COUNT);
    }

    if ( !s_bInited )
    {
        s_SeliThrd = LmnCreateThread( SeliSelectThreadFunc, 0, 0 );
        if ( 0 == s_SeliThrd )
        {
            return -1;
        }

        s_DnsThrd = LmnCreateThread( DnsResolveThreadFunc, 0, 0 );
        if ( 0 == s_DnsThrd )
        {
            return -1;
        }

        s_bInited = TRUE;
    }


    PTRedisAsynParam pParam = new TRedisAsynParam;
    if ( 0 == pParam )
    {
        return -1;
    }

    pParam->m_fd = -1;
    pParam->m_pCnnCallbk = pCnnCallbk;
    pParam->m_pDataCallback = pCallback;
    strncpy( pParam->m_szHost, szAddr, sizeof(pParam->m_szHost) );
    pParam->m_wPort = wPort;


    FdType fd = liOpen( LI_PROTOTYPE_TCP, 0, 0, AsynCallback_connect, (void *)pParam, FALSE, TRUE );
    if ( INVALID_SOCKET == fd )
    {
        delete pParam;
        return -1;
    }
    pParam->m_fd = fd;


    CFuncLock theLock( &g_Lock );
    s_vRedisAysnParams.push_back( pParam );

    *ph = (void *)pParam;
    return 0;
}

REDIS_CLIENT_API  int  SendRedisDataAsyn( HRedisClient h, const void * pSendData, DWORD dwSendSize, DWORD dwSendId )
{
    if ( 0 == h || 0 == pSendData || 0 == dwSendSize )
    {
        return -1;
    }

    PTRedisAsynParam pParam = (PTRedisAsynParam)h;

    PTRedisReqAsyn pRequest = new TRedisReqAsyn;
    if ( 0 == pRequest )
    {
        return -1;
    }

    pRequest->m_pRequest = new char[dwSendSize];
    if ( 0 == pRequest )
    {
        delete pRequest;
        return -1;
    }

    memcpy( pRequest->m_pRequest, pSendData, dwSendSize );
    pRequest->m_dwRequestLen = dwSendSize;
    pRequest->m_dwRequestId = dwSendId;

    
    CFuncLock theLock( &g_Lock );

    int len = dwSendSize;
    if ( 0 != liTcpSend( pParam->m_fd, (const char *)pSendData, &len ) )
    {
        delete[] pRequest->m_pRequest;
        delete pRequest;
        return -1;
    }
    
    pParam->m_vRequests.push_back( pRequest );


    return 0;
}



static  DWORD  CountXmlValueLen( const char * szValue )
{
    DWORD  dwLen = strlen( szValue );

    for ( DWORD i = 0; i < dwLen; i++ )
    {
        // &amp;
        if ( '&' == szValue[i] )
        {
            dwLen += 4;
        }
        // &lt;
        else if ( '<' == szValue[i] )
        {
            dwLen += 3;
        }
        // &gt;
        else if ( '>' == szValue[i] )
        {
            dwLen += 3;
        }
        // &apos;
        else if ( '\'' == szValue[i] )
        {
            dwLen += 5;
        }
        // &quot;
        else if ( '"' == szValue[i] )
        {
            dwLen += 5;
        }
    }

    return dwLen;
}

static  char *  RXmlValueCopy( char * szDst, const char * szSrc )
{
    const char * p = szSrc;
    char *       q = szDst;

    char ch = *p;
    while( ch )
    {
        // &amp;
        if ( '&' == *p )
        {
            memcpy( q, "&amp;", 5 );
            q += 5;
        }
        // &lt;
        else if ( '<' == ch )
        {
            memcpy( q, "&lt;", 4 );
            q += 4;
        }
        // &gt;
        else if ( '>' == ch )
        {
            memcpy( q, "&gt;", 4 );
            q += 4;
        }
        // &apos;
        else if ( '\'' == ch )
        {
            memcpy( q, "&apos;", 6 );
            q += 6;
        }
        // &quot;
        else if ( '"' == ch )
        {
            memcpy( q, "&quot;", 6 );
            q += 6;
        }
        else
        {
            *q = ch;
            q++;
        }

        p++;
        ch = *p;
    }

    
    return q;
}




REDIS_CLIENT_API  PTRedisXmlNode  CreateRXmlNode( const char * szNodeName )
{
    if ( 0 == szNodeName )
    {
        return 0;
    }


    TRedisXmlNode * pNode = (TRedisXmlNode *)g_pfnAllocMem( sizeof(TRedisXmlNode) );
    if ( 0 == pNode )
    {
        return 0;
    }
    memset( pNode, 0, sizeof(TRedisXmlNode) );


    DWORD dwNameLen = strlen( szNodeName );
    pNode->m_szName = (char *)g_pfnAllocMem( sizeof(char) * (dwNameLen + 1) );
    if ( 0 == pNode->m_szName )
    {
        g_pfnFreeMem( pNode );
        return 0;
    }
    strcpy( pNode->m_szName, szNodeName );

    return pNode;
}

static void DestroyRXmlNode_( PTRedisXmlNode  pNode )
{
    PTRedisXmlNode  pTmpNode = 0;
    PTRedisXmlNode  pChild = pNode->m_pFirstChild;
    while( pChild )
    {
        pTmpNode = pChild;
        pChild   = pChild->m_pNextSibling;

        DestroyRXmlNode_( pTmpNode );
    }

    PTRedisXmlAttr  pTmpAttr = 0;
    PTRedisXmlAttr  pAttr = pNode->m_pFirstAttr;
    while( pAttr )
    {
        pTmpAttr = pAttr;
        pAttr = pAttr->m_pNextSibling;

        if ( pTmpAttr->m_szAttrName )
        {
            g_pfnFreeMem( pTmpAttr->m_szAttrName );
        }

        if ( pTmpAttr->m_szAttrValue )
        {
            g_pfnFreeMem( pTmpAttr->m_szAttrValue );
        }

        g_pfnFreeMem( pTmpAttr );
    }

    if ( pNode->m_szBody )
    {
        g_pfnFreeMem( pNode->m_szBody );
    }

    if ( pNode->m_szName )
    {
        g_pfnFreeMem( pNode->m_szName );
    }

    g_pfnFreeMem( pNode );
}

REDIS_CLIENT_API  BOOL  DestroyRXmlNode( PTRedisXmlNode  pRootNode )
{
    if ( 0 == pRootNode )
    {
        return FALSE;
    }

    DestroyRXmlNode_( pRootNode );

    return TRUE;
}


REDIS_CLIENT_API  BOOL            AppendRXmlNode( PTRedisXmlNode pParent, PTRedisXmlNode pChild )
{
    if ( 0 == pParent || 0 == pChild )
    {
        return FALSE;
    }

    if ( pChild->m_pParent || pChild->m_pNextSibling )
    {
        return FALSE;
    }

    if ( pParent->m_szBody )
    {
        return FALSE;
    }


    PTRedisXmlNode  pLastChild = pParent->m_pFirstChild;
    while( pLastChild )
    {
        if ( pLastChild->m_pNextSibling )
        {
            pLastChild = pLastChild->m_pNextSibling;
        }
        else
        {
            break;
        }
    }


    if ( pLastChild )
    {
        pLastChild->m_pNextSibling = pChild;
    }
    else
    {
        pParent->m_pFirstChild = pChild;
    }

    pChild->m_pParent = pParent;

    return TRUE;
}


REDIS_CLIENT_API  BOOL  SetRXmlAttr( PTRedisXmlNode  pNode, const char * szAttrName, const char * szAttrValue )
{
    if ( 0 == pNode || 0 == szAttrName )
    {
        return FALSE;
    }

    PTRedisXmlAttr pPrevAttr = 0;
    PTRedisXmlAttr pAttr     = pNode->m_pFirstAttr;

    while ( pAttr )
    {
        // 找到
        if ( 0 == strcmp( pAttr->m_szAttrName, szAttrName ) )
        {
            // 销毁该属性
            if ( 0 == szAttrValue )
            {
                // 没有前一个Attr节点
                if ( 0 == pPrevAttr )
                {
                    pNode->m_pFirstAttr = pAttr->m_pNextSibling;
                }
                else
                {
                    pPrevAttr->m_pNextSibling = pAttr->m_pNextSibling;
                }

                assert( pAttr->m_szAttrName );
                g_pfnFreeMem( pAttr->m_szAttrName );

                if ( pAttr->m_szAttrValue )
                {
                    g_pfnFreeMem( pAttr->m_szAttrValue );
                }
                g_pfnFreeMem( pAttr );

                return TRUE;
            }
            // 重设置该属性
            else
            {
                if ( pAttr->m_szAttrValue )
                {
                    g_pfnFreeMem( pAttr->m_szAttrValue );
                    pAttr->m_szAttrValue = 0;
                }
                
                DWORD  dwAttrValueLen = strlen( szAttrValue );
                pAttr->m_szAttrValue = (char *)g_pfnAllocMem( sizeof(char) * (dwAttrValueLen + 1) );
                if ( 0 == pAttr->m_szAttrValue )
                {
                    return FALSE;
                }
                strcpy( pAttr->m_szAttrValue, szAttrValue );

                return TRUE;
            }
        }

        pPrevAttr = pAttr;
        pAttr     = pAttr->m_pNextSibling;
    }


    PTRedisXmlAttr  pNewAttr = (PTRedisXmlAttr)g_pfnAllocMem( sizeof(TRedisXmlAttr) );
    if ( 0 == pNewAttr )
    {
        return FALSE;
    }
    memset(pNewAttr, 0, sizeof(TRedisXmlAttr));


    DWORD  dwNameLen = strlen(szAttrName);
    pNewAttr->m_szAttrName = (char *)g_pfnAllocMem( sizeof(char) * (dwNameLen+1) );
    if ( 0 == pNewAttr->m_szAttrName )
    {
        g_pfnFreeMem( pNewAttr );
        return FALSE;
    }
    strcpy( pNewAttr->m_szAttrName, szAttrName );


    DWORD dwValueLen = strlen(szAttrValue);
    pNewAttr->m_szAttrValue = (char *)g_pfnAllocMem( sizeof(char) * (dwValueLen+1) );
    if ( 0 == pNewAttr->m_szAttrValue )
    {
        g_pfnFreeMem( pNewAttr->m_szAttrName );
        g_pfnFreeMem( pNewAttr );
        return FALSE;
    }
    strcpy( pNewAttr->m_szAttrValue, szAttrValue );

    if ( pPrevAttr )
    {
        pPrevAttr->m_pNextSibling = pNewAttr;
    }
    else
    {
        pNode->m_pFirstAttr = pNewAttr;
    }

    return TRUE;
}

REDIS_CLIENT_API  BOOL  SetRXmlBody( PTRedisXmlNode  pNode, const char * szBody )
{
    if ( 0 == pNode )
    {
        return FALSE;
    }

    if ( 0 == szBody )
    {
        if ( pNode->m_szBody )
        {
            g_pfnFreeMem( pNode->m_szBody );
            pNode->m_szBody = 0;
        }

        return TRUE;
    }
    else
    {
        if ( pNode->m_pFirstChild )
        {
            return FALSE;
        }

        if ( pNode->m_szBody )
        {
            g_pfnFreeMem( pNode->m_szBody );
            pNode->m_szBody = 0;
        }

        DWORD dwBodyLen = strlen(szBody);
        pNode->m_szBody = (char *)g_pfnAllocMem( sizeof(char) * (dwBodyLen+1) );
        if ( 0 == pNode->m_szBody )
        {
            return FALSE;
        }
        strcpy( pNode->m_szBody, szBody );

        return TRUE;
    }
}


static  DWORD  CountRXmlNodeLen( PTRedisXmlNode pNode )
{
    DWORD  dwLen = 0;

    // <name/>
    if ( 0 == pNode->m_pFirstChild && 0 == pNode->m_szBody )
    {
        dwLen = CountXmlValueLen( pNode->m_szName ) + 3;

        // attr="..."
        PTRedisXmlAttr pAttr = pNode->m_pFirstAttr;
        while( pAttr )
        {
            if ( pAttr->m_szAttrValue )
            {
                dwLen += CountXmlValueLen(pAttr->m_szAttrName);  // attr name
                dwLen += 4;  // 空格 + "=" + 2个引号
                dwLen += CountXmlValueLen(pAttr->m_szAttrValue);
            }
            pAttr = pAttr->m_pNextSibling;
        }

        return dwLen;
    }
    // <name>...</name>
    else
    {
        // 不考虑既有body又有child的情况

        dwLen += CountXmlValueLen(pNode->m_szName) + 2;

        // attr="..."
        PTRedisXmlAttr pAttr = pNode->m_pFirstAttr;
        while( pAttr )
        {
            if ( pAttr->m_szAttrValue )
            {
                dwLen += CountXmlValueLen(pAttr->m_szAttrName);  // attr name
                dwLen += 4;  // 空格 + "=" + 2个引号
                dwLen += CountXmlValueLen(pAttr->m_szAttrValue);
            }
            pAttr = pAttr->m_pNextSibling;
        }

        if ( pNode->m_szBody )
        {
            dwLen += CountXmlValueLen( pNode->m_szBody );
        }
        else
        {
            PTRedisXmlNode  pChild = pNode->m_pFirstChild;
            while( pChild )
            {
                dwLen += CountRXmlNodeLen( pChild );
                pChild = pChild->m_pNextSibling;
            }
        }        

        dwLen += CountXmlValueLen(pNode->m_szName) + 3;
    }
    
    return dwLen;
}

static  char *  ConvertRXmlNode2Buf_( PTRedisXmlNode pNode, char * pDst )
{
    DWORD  dwLen = 0;
    char * p     = pDst;

    // <name/>
    if ( 0 == pNode->m_pFirstChild && 0 == pNode->m_szBody )
    {
        memcpy( p, "<", 1 );
        p++;

        p = RXmlValueCopy( p, pNode->m_szName );


        // attr="..."
        PTRedisXmlAttr pAttr = pNode->m_pFirstAttr;
        while( pAttr )
        {
            if ( pAttr->m_szAttrValue )
            {
                memcpy( p, " ", 1 );
                p++;

                p = RXmlValueCopy( p, pAttr->m_szAttrName );

                memcpy( p, "=\"", 2 );
                p += 2;

                p = RXmlValueCopy( p, pAttr->m_szAttrValue );

                memcpy( p, "\"", 1 );
                p++;
            }
            pAttr = pAttr->m_pNextSibling;
        }


        memcpy( p, "/>", 2 );
        p += 2;

        return p;
    }
    // <name>...</name>
    else
    {
        // 不考虑既有body又有child的情况

        memcpy( p, "<", 1 );
        p++;

        p = RXmlValueCopy( p, pNode->m_szName );


        // attr="..."
        PTRedisXmlAttr pAttr = pNode->m_pFirstAttr;
        while( pAttr )
        {
            if ( pAttr->m_szAttrValue )
            {
                memcpy( p, " ", 1 );
                p++;

                p = RXmlValueCopy( p, pAttr->m_szAttrName );

                memcpy( p, "=\"", 2 );
                p += 2;

                p = RXmlValueCopy( p, pAttr->m_szAttrValue );

                memcpy( p, "\"", 1 );
                p++;
            }
            pAttr = pAttr->m_pNextSibling;
        }

        memcpy( p, ">", 1 );
        p++;



        if ( pNode->m_szBody )
        {
            p = RXmlValueCopy( p, pNode->m_szBody );
        }
        else
        {
            PTRedisXmlNode  pChild = pNode->m_pFirstChild;
            while( pChild )
            {
                p = ConvertRXmlNode2Buf_( pChild, p );
                pChild = pChild->m_pNextSibling;
            }
        }        


        memcpy( p, "</", 2 );
        p += 2;

        p = RXmlValueCopy( p, pNode->m_szName );

        memcpy( p, ">", 1 );
        p++;

        return p;
    }
}



REDIS_CLIENT_API  char *   ConvertRXmlNode2Buf( PTRedisXmlNode  pRootNode )
{
    if ( 0 == pRootNode )
    {
        return 0;
    }

    DWORD  dwTotalLen = CountRXmlNodeLen( pRootNode );
    char * pBuf = (char *)g_pfnAllocMem( sizeof(char) * (dwTotalLen + 1) );
    if ( 0 == pBuf )
    {
        return 0;
    }

    char * p = ConvertRXmlNode2Buf_( pRootNode, pBuf );
    *p = '\0';

    return pBuf;
}


REDIS_CLIENT_API  BOOL     FreeRXmlBuf( char * pBuf )
{
    if ( 0 == pBuf )
    {
        return FALSE;
    }

    g_pfnFreeMem( pBuf );

    return TRUE;
}







static BOOL  IsBlankChar_( char  ch )
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





// 因为从转义到实义字符，所需字符数是减少的，所以临时buff的最大大小可以确定
static char *  DecodeRXmlValue( const char * szSrc, DWORD dwSrcSize )
{
    char * pValue1 = (char *)g_pfnAllocMem( sizeof(char) * (dwSrcSize + 1) );
    if ( 0 == pValue1 )
    {
        return 0;
    }
    memcpy( pValue1, szSrc, dwSrcSize );
    pValue1[dwSrcSize] = '\0';


    char * pValue2 = (char *)g_pfnAllocMem( sizeof(char) * (dwSrcSize + 1) );
    if ( 0 == pValue2 )
    {
        g_pfnFreeMem( pValue1 );
        return 0;
    }

    StrReplace( pValue2,  dwSrcSize + 1,  pValue1, "&amp;",  "&" );
    StrReplace( pValue1,  dwSrcSize + 1,  pValue2, "&lt;",   "<" );
    StrReplace( pValue2,  dwSrcSize + 1,  pValue1, "&gt;",   ">" );
    StrReplace( pValue1 , dwSrcSize + 1,  pValue2, "&apos;", "'" );
    StrReplace( pValue2,  dwSrcSize + 1,  pValue1, "&quot;", "\"" );

    g_pfnFreeMem( pValue1 );

    return pValue2;
}

static  BOOL  ParseRXmlAttr( PTRedisXmlNode pNode, const char * pBuf, DWORD dwBufSize )
{
    DWORD i;
    for ( i = 0; i < dwBufSize; i++ )
    {
        if ( !IsBlankChar_(pBuf[i]) )
        {
            break;
        }
    }

    if ( 0 == i )
    {
        return FALSE;
    }

    if ( i >= dwBufSize )
    {
        return TRUE;
    }

    DWORD  dwAttrNameStartPos = i;
    DWORD  dwAttrNameEndPos   = i;

    for ( i++; i < dwBufSize; i++ )
    {
        if ( '=' == pBuf[i] )
        {
            break;
        }

        dwAttrNameEndPos++;
    }

    if ( i >= dwBufSize )
    {
        return FALSE;
    }

    DWORD  dwAttrNameLen = dwAttrNameEndPos - dwAttrNameStartPos + 1;

    PTRedisXmlAttr  pLastAttr = 0;
    PTRedisXmlAttr  pTmpAttr  = pNode->m_pFirstAttr;
    while( pTmpAttr )
    {
        pLastAttr = pTmpAttr;
        pTmpAttr  = pTmpAttr->m_pNextSibling;
    }

    PTRedisXmlAttr  pCurAttr = (PTRedisXmlAttr)g_pfnAllocMem( sizeof(TRedisXmlAttr) );
    if ( 0 == pCurAttr )
    {
        return FALSE;
    }
    memset( pCurAttr, 0, sizeof(TRedisXmlAttr) );

    if ( 0 == pLastAttr )
    {
        pNode->m_pFirstAttr = pCurAttr;
    }
    else
    {
        pLastAttr->m_pNextSibling = pCurAttr;
    }

    pCurAttr->m_szAttrName = (char *)g_pfnAllocMem( sizeof(char) * (dwAttrNameLen+1) );
    if ( 0 == pCurAttr->m_szAttrName )
    {
        return FALSE;
    }
    memcpy( pCurAttr->m_szAttrName, pBuf + dwAttrNameStartPos, dwAttrNameLen );
    pCurAttr->m_szAttrName[dwAttrNameLen] = '\0';
    StrTrim( pCurAttr->m_szAttrName);


    BOOL  bQuote = FALSE;
    for ( i = dwAttrNameEndPos + 2; i < dwBufSize; i++ )
    {
        if ( '"' == pBuf[i] )
        {
            bQuote = TRUE;
            break;
        }
        else if ( '\'' == pBuf[i] )
        {
            bQuote = FALSE;
            break;
        }
    }

    if ( i >= dwBufSize )
    {
        return FALSE;
    }


    DWORD  dwAttrValueStartPos = i + 1;

    for ( i = dwAttrValueStartPos; i < dwBufSize; i++ )
    {
        if ( bQuote )
        {
            if ( '"' == pBuf[i] )
            {
                break;
            }
        }
        else
        {
            if ( '\'' == pBuf[i] )
            {
                break;
            }
        }
    }

    if ( i >= dwBufSize )
    {
        return FALSE;
    }

    DWORD  dwAttrValueLen = i - dwAttrValueStartPos;

    pCurAttr->m_szAttrValue = DecodeRXmlValue( pBuf + dwAttrValueStartPos, dwAttrValueLen );
    if ( 0 == pCurAttr->m_szAttrValue )
    {
        return FALSE;
    }
    
    DWORD dwLeft = dwBufSize - (i + 1);

    if ( dwLeft > 0 )
    {
        return ParseRXmlAttr( pNode, pBuf + i + 1, dwBufSize - (i + 1) );
    }
    else
    {
        return TRUE;
    }
}

// <tagName>或<tagName/> (忽略尖括号)
// <tagName attr="">或<tagName attr="" /> (忽略尖括号)
static  PTRedisXmlNode   ParseRXmlTag( const char * pBuf, DWORD dwBufSize )
{
    DWORD i;
    for ( i = 0; i < dwBufSize; i++ )
    {
        if ( !IsBlankChar_(pBuf[i]) )
        {
            break;
        }
    }

    if ( i >= dwBufSize )
    {
        return 0;
    }

    DWORD  dwTagNameStartPos = i;
    DWORD  dwTagNameEndPos   = i;

    for ( i++; i < dwBufSize; i++ )
    {
        if ( IsBlankChar_(pBuf[i]) )
        {
            break;
        }

        dwTagNameEndPos++;
    }

    PTRedisXmlNode  pNode = (PTRedisXmlNode)g_pfnAllocMem( sizeof(TRedisXmlNode) );
    if ( 0 == pNode )
    {
        return 0;
    }
    memset( pNode, 0, sizeof(TRedisXmlNode) );

    DWORD dwTagNameLen = 0;


    // "name"形式(忽略掉尖括号)
    if ( i >= dwBufSize )
    {
        dwTagNameLen = dwTagNameEndPos - dwTagNameStartPos + 1;

        pNode->m_szName = DecodeRXmlValue( pBuf + dwTagNameStartPos, dwTagNameLen );
        if ( 0 == pNode->m_szName )
        {
            g_pfnFreeMem( pNode );
            return 0;
        }

        return pNode;
    }
    // "name attr=.."形式
    else
    {
        dwTagNameLen = dwTagNameEndPos - dwTagNameStartPos + 1;

        pNode->m_szName = DecodeRXmlValue( pBuf + dwTagNameStartPos, dwTagNameLen );
        if ( 0 == pNode->m_szName )
        {
            g_pfnFreeMem( pNode );
            return 0;
        }

        if ( !ParseRXmlAttr(pNode, pBuf + (dwTagNameEndPos + 1), dwBufSize - (dwTagNameEndPos + 1)  ) )
        {
            DestroyRXmlNode_( pNode );
            return 0;
        }

        return pNode;
    }
}

// 从"<"开始
static  PTRedisXmlNode  ParseRXmlNode_( const char * pBuf, DWORD dwBufSize, PTRedisXmlNode pParent, DWORD * pdwParseEndPos )
{
    DWORD  i;
    char   chSlash = 0;

    for ( i = 1; i < dwBufSize; i++ )
    {
        if ( '>' == pBuf[i] )
        {
            break;
        }

        chSlash = pBuf[i];
    }

    // 没有找到">"
    if ( i >= dwBufSize )
    {
        return 0;
    }

    // 找到了，但是不可能是<>形式
    if ( i == 1 )
    {
        return 0;
    }

    DWORD  dwGtPos  = i;
    DWORD  dwTagLen = dwGtPos - 1;

    if ( '/' == chSlash )
    {
        dwTagLen--;
        if ( 0 == dwTagLen )
        {
            return 0;
        }

        // 没有body和child
        PTRedisXmlNode pNode = ParseRXmlTag( pBuf + 1, dwTagLen );
        if ( 0 == pNode )
        {
            return 0;
        }

        if ( pParent )
        {
            AppendRXmlNode( pParent, pNode );
        }

        *pdwParseEndPos = dwGtPos + 1;

        return pNode;
    }
    else
    {
        PTRedisXmlNode pNode = ParseRXmlTag( pBuf + 1, dwTagLen );
        if ( 0 == pNode )
        {
            return 0;
        }

        // body(如果有子node，忽略掉body)
        DWORD  dwNextStartPos = dwGtPos + 1;


        do 
        {
            for ( i = dwNextStartPos; i < dwBufSize; i++ )
            {
                if ( '<' == pBuf[i] )
                {
                    break;
                }
            }

            if ( i >= dwBufSize )
            {
                DestroyRXmlNode_( pNode );
                return 0;
            }

            if ( dwBufSize - i <= 4 )
            {
                DestroyRXmlNode_( pNode );
                return 0;
            }


            DWORD  dwNextLtPos = i;

            if ( '/' == pBuf[dwNextLtPos+1] )
            {
                for ( i = dwNextLtPos + 2; i < dwBufSize; i++ )
                {
                    if ( '>' == pBuf[i] )
                    {
                        break;
                    }
                }

                if ( i >= dwBufSize )
                {
                    DestroyRXmlNode_( pNode );
                    return 0;
                }

                DWORD  dwTagNameLen = i - ( dwNextLtPos + 1 ) - 1;
                char * pTmp = (char *)g_pfnAllocMem( sizeof(char) * (dwTagNameLen + 1) );
                if ( 0 == pTmp )
                {
                    DestroyRXmlNode_( pNode );
                    return 0;
                }
                memcpy( pTmp, pBuf + dwNextLtPos + 2,dwTagNameLen );
                pTmp[dwTagNameLen] = '\0';
                StrTrim(pTmp);

                if ( 0 != strcmp( pNode->m_szName, pTmp ) )
                {
                    DestroyRXmlNode_( pNode );
                    g_pfnFreeMem( pTmp );
                    return 0;
                }
                g_pfnFreeMem( pTmp );

                if ( 0 == pNode->m_pFirstChild )
                {
                    DWORD dwBodyLen = dwNextLtPos - dwGtPos - 1;
                    if ( dwBodyLen > 0 )
                    {
                        pNode->m_szBody = DecodeRXmlValue( pBuf + dwGtPos + 1, dwBodyLen );
                        if ( 0 == pNode->m_szBody )
                        {
                            DestroyRXmlNode_( pNode );
                            return 0;
                        }
                    }
                }

                if ( pParent )
                {
                    AppendRXmlNode( pParent, pNode );
                }

                *pdwParseEndPos = i + 1;
                return pNode;
            }
            else
            {
                DWORD  dwParseEndPos = 0;
                PTRedisXmlNode pChild = ParseRXmlNode_( pBuf + dwNextLtPos, dwBufSize - dwNextLtPos, pNode, &dwParseEndPos );
                if ( 0 == pChild )
                {
                    DestroyRXmlNode_( pNode );
                    return 0;
                }

                dwNextStartPos = dwParseEndPos + dwNextLtPos;
            }

        } while ( 1 );

    }


    return 0;
}

static  PTRedisXmlNode   ParseRXmlRoot_( const char * pBuf, DWORD dwBufSize )
{
    DWORD i;
    BOOL  bError = FALSE;
    for ( i = 0; i < dwBufSize; i++ )
    {
        if ( '<' == pBuf[i] )
        {
            break;
        }
        else if ( !IsBlankChar_(pBuf[i]) )
        {
            bError = TRUE;
            break;
        }
    }

    // 没有找到"<"
    if ( i >= dwBufSize )
    {
        return 0;
    }

    if ( bError )
    {
        return 0;
    }

    DWORD dwNextStartPos = 0;

    PTRedisXmlNode pRoot = ParseRXmlNode_( pBuf + i, dwBufSize - i, 0, &dwNextStartPos );

    if ( 0 == pRoot )
    {
        return 0;
    }

    for ( i += dwNextStartPos; i < dwBufSize; i++ )
    {
        if ( !IsBlankChar_(pBuf[i]) )
        {
            break;
        }
    }

    if ( i < dwBufSize )
    {
        DestroyRXmlNode_( pRoot );
        return 0;
    }

    return pRoot;
}


REDIS_CLIENT_API  PTRedisXmlNode   ParseRXmlBuf( const char * pBuf, DWORD dwBufSize )
{
    if ( 0 == pBuf || 0 == dwBufSize )
    {
        return 0;
    }

    return ParseRXmlRoot_( pBuf, dwBufSize );
}













typedef struct tagRedisRequestParam_
{
    char   *                  m_szParam;
    DWORD                     m_dwParamLen;

    tagRedisRequestParam_ *   m_pNext;
}TRedisRequestParam_, *PTRedisRequestParam_;


typedef struct  tagRedisRequest_
{
    TRedisRequestParam_ *     m_pFirstParam;
}TRedisRequest_, *PTRedisRequest_;



// 统一请求协议
REDIS_CLIENT_API  HRedisRequest  CreateRedisRequest()
{
    PTRedisRequest_  pRequest = (PTRedisRequest_)g_pfnAllocMem(sizeof(TRedisRequest_));
    if ( 0 == pRequest )
    {
        return 0;
    }

    memset( pRequest, 0, sizeof(TRedisRequest_) );
    return (HRedisRequest)pRequest;
}

// 销毁请求
REDIS_CLIENT_API  int  DestroyRedisRequest( HRedisRequest h )
{
    if ( 0 == h )
    {
        return -1;
    }

    PTRedisRequest_  pRequest = (PTRedisRequest_)h;

    PTRedisRequestParam_ pParam = pRequest->m_pFirstParam;
    while( pParam )
    {
        PTRedisRequestParam_  pTmp = pParam;
        pParam = pParam->m_pNext;

        if ( pTmp->m_szParam )
        {
            g_pfnFreeMem( pTmp->m_szParam );
        }
        g_pfnFreeMem( pTmp );
    }

    g_pfnFreeMem( pRequest );

    return 0;
}

// 清空请求
REDIS_CLIENT_API  int  ClearRedisRequest( HRedisRequest h )
{
    if ( 0 == h )
    {
        return -1;
    }

    PTRedisRequest_  pRequest = (PTRedisRequest_)h;

    PTRedisRequestParam_ pParam = pRequest->m_pFirstParam;
    while( pParam )
    {
        PTRedisRequestParam_  pTmp = pParam;
        pParam = pParam->m_pNext;

        if ( pTmp->m_szParam )
        {
            g_pfnFreeMem( pTmp->m_szParam );
        }
        g_pfnFreeMem( pTmp );
    }

    pRequest->m_pFirstParam = 0;

    return 0;
}


static void  DestroyRedisRequestParam( PTRedisRequestParam_  pFirstParam )
{
    PTRedisRequestParam_ pCurParam  = pFirstParam;
    while( pCurParam )
    {
        PTRedisRequestParam_ pTmp = pCurParam;
        pCurParam = pCurParam->m_pNext;

        if ( pTmp->m_szParam )
        {
            g_pfnFreeMem( pTmp->m_szParam );
        }
        g_pfnFreeMem( pTmp );
    }
}

// 添加请求参数
// 以NULL结束
REDIS_CLIENT_API  int  AddRedisRequestParam( HRedisRequest h, const char * szParam, ... )
{
    if ( 0 == h || 0 == szParam )
    {
        return -1;
    }

    PTRedisRequest_  pRequest = (PTRedisRequest_)h;

    PTRedisRequestParam_ pLastParam = 0;
    PTRedisRequestParam_ pCurParam  = pRequest->m_pFirstParam;
    while( pCurParam )
    {
        pLastParam = pCurParam;
        pCurParam  = pCurParam->m_pNext;
    }


    pCurParam = (PTRedisRequestParam_)g_pfnAllocMem(sizeof(TRedisRequestParam_));
    if ( 0 == pCurParam )
    {
        return -1;
    }
    memset( pCurParam, 0, sizeof(TRedisRequestParam_) );


    DWORD dwParamLen = strlen(szParam);
    pCurParam->m_szParam = (char *)g_pfnAllocMem( sizeof(char) * (dwParamLen + 1) );
    if ( 0 == pCurParam->m_szParam )
    {
        return -1;
    }
    strcpy( pCurParam->m_szParam, szParam );
    pCurParam->m_dwParamLen = dwParamLen;



    PTRedisRequestParam_ pFirstParam = pCurParam;
    PTRedisRequestParam_ pPrevParam  = pFirstParam;


    va_list  ap;
    va_start( ap, szParam );
    const char * szNextParam = va_arg( ap, const char * );
    while ( szNextParam )
    {
        pCurParam = (PTRedisRequestParam_)g_pfnAllocMem(sizeof(TRedisRequestParam_));
        if ( 0 == pCurParam )
        {
            DestroyRedisRequestParam( pFirstParam );
            return -1;
        }
        memset( pCurParam, 0, sizeof(TRedisRequestParam_) );


        DWORD dwParamLen = strlen(szNextParam);
        pCurParam->m_szParam = (char *)g_pfnAllocMem( sizeof(char) * (dwParamLen + 1) );
        if ( 0 == pCurParam->m_szParam )
        {
            DestroyRedisRequestParam( pFirstParam );
            return -1;
        }
        strcpy( pCurParam->m_szParam, szNextParam );
        pCurParam->m_dwParamLen = dwParamLen;


        pPrevParam->m_pNext = pCurParam;
        pPrevParam = pCurParam;

        szNextParam = va_arg( ap, const char * );
    }
    va_end( ap );


    if ( 0 == pLastParam )
    {
        pRequest->m_pFirstParam = pFirstParam;
    }
    else
    {
        pLastParam->m_pNext = pFirstParam;
    }

    return 0;
}

REDIS_CLIENT_API  int  AddRedisRequestParamEx( HRedisRequest h, PTRedisRequestParam pParams, DWORD dwParamsCnt )
{
    if ( 0 == h || 0 == pParams || 0 == dwParamsCnt )
    {
        return -1;
    }

    PTRedisRequest_  pRequest = (PTRedisRequest_)h;

    PTRedisRequestParam_ pLastParam = 0;
    PTRedisRequestParam_ pCurParam  = pRequest->m_pFirstParam;
    while( pCurParam )
    {
        pLastParam = pCurParam;
        pCurParam  = pCurParam->m_pNext;
    }



    PTRedisRequestParam_ pFirstParam = 0;
    PTRedisRequestParam_ pPrevParam  = 0;

    DWORD i;
    for ( i = 0; i < dwParamsCnt; i++ )
    {
        pCurParam = (PTRedisRequestParam_)g_pfnAllocMem(sizeof(TRedisRequestParam_));
        if ( 0 == pCurParam )
        {
            if ( pFirstParam )
            {
                DestroyRedisRequestParam( pFirstParam );
            }
            return -1;
        }
        memset( pCurParam, 0, sizeof(TRedisRequestParam_) );


        DWORD dwParamLen = pParams[i].m_dwParamLen;
        pCurParam->m_szParam = (char *)g_pfnAllocMem( sizeof(char) * (dwParamLen + 1) );
        if ( 0 == pCurParam->m_szParam )
        {
            if ( pFirstParam )
            {
                DestroyRedisRequestParam( pFirstParam );
            }
            return -1;
        }
        memcpy( pCurParam->m_szParam, pParams[i].m_pParam, dwParamLen );
        pCurParam->m_dwParamLen = dwParamLen;


        if ( 0 == pFirstParam )
        {
            pFirstParam = pCurParam;
            pPrevParam  = pCurParam;
        }
        else
        {
            pPrevParam->m_pNext = pCurParam;
            pPrevParam = pCurParam;
        }
    }


    if ( 0 == pLastParam )
    {
        pRequest->m_pFirstParam = pFirstParam;
    }
    else
    {
        pLastParam->m_pNext = pFirstParam;
    }

    return 0;
}


REDIS_CLIENT_API  int ConvertRedisRequest2Buf( HRedisRequest h, char ** ppResultBuf, DWORD * pdwBufSize )
{
    if ( 0 == h || 0 == ppResultBuf || 0 == pdwBufSize )
    {
        return -1;
    }

    PTRedisRequest_  pRequest = (PTRedisRequest_)h;

    if ( 0 == pRequest->m_pFirstParam )
    {
        return -1;
    }


    // *number\r\n (number最多10位数字)
    DWORD  dwBuffSize  = 13;
    DWORD  dwParamsCnt = 0;

    PTRedisRequestParam_  pParam = pRequest->m_pFirstParam;
    while( pParam )
    {
        dwParamsCnt++;

        // $size\r\n param len \r\n
        // DWORD  dwParamLen = strlen( pParam->m_szParam );
        DWORD  dwParamLen = pParam->m_dwParamLen;
        dwBuffSize += 15 + dwParamLen; 

        pParam = pParam->m_pNext;
    }

    // 字符串结束符号
    dwBuffSize++;

    char * pBuf = (char *)g_pfnAllocMem( sizeof(char) * (dwBuffSize+1) );
    if ( 0 == pBuf )
    {
        return -1;
    }


    char * p = pBuf;
    sprintf( p, "*%lu\r\n", dwParamsCnt );
    DWORD dwLen = strlen(p);
    p += dwLen;
    

    pParam = pRequest->m_pFirstParam;
    while( pParam )
    {
        // $size\r\n param len \r\n
        DWORD  dwParamLen = pParam->m_dwParamLen;
        sprintf( p, "$%lu\r\n", dwParamLen );
        dwLen = strlen(p);
        p += dwLen;

        memcpy( p, pParam->m_szParam, dwParamLen );
        p += dwParamLen;

        memcpy( p, "\r\n", 2 );
        p += 2;

        pParam = pParam->m_pNext;
    }

    *p = '\0';

    *ppResultBuf = pBuf;
    *pdwBufSize  = (DWORD)(p - pBuf);

    return 0;
}

REDIS_CLIENT_API  int    FreeRedisRequestBuf( char * pBuf )
{
    if ( 0 == pBuf )
    {
        return -1;
    }

    g_pfnFreeMem( pBuf );

    return 0;
}


REDIS_CLIENT_API  int  ParseRedisResp( const char * pBuf, DWORD dwBufSize, PTRedisResp * ppResp )
{
    if ( 0 == pBuf || 0 == dwBufSize || 0 == ppResp )
    {
        return -1;
    }

    PTRedisResp pResp = (PTRedisResp)g_pfnAllocMem( sizeof(TRedisResp) );
    if ( 0 == pResp )
    {
        return -1;
    }
    memset( pResp, 0, sizeof(TRedisResp) );

    // +OK\r\n
    if ( '+' == pBuf[0] )
    {
        if ( dwBufSize <= 3 )
        {
            g_pfnFreeMem( pResp );
            return -1;
        }

        if ( '\r' != pBuf[dwBufSize-2] || '\n' != pBuf[dwBufSize-1] )
        {
            g_pfnFreeMem( pResp );
            return -1;
        }

        pResp->m_bError = FALSE;
        pResp->m_eType  = REDIS_RESP_TYPE_SINGLE_LINE;

        pResp->m_szErrorDescription = (char *)g_pfnAllocMem( sizeof(char) * (dwBufSize) );
        if ( 0 == pResp->m_szErrorDescription )
        {
            g_pfnFreeMem(pResp);
            return -1;
        }

        DWORD  dwDescriptionLen = dwBufSize - 3;

        memcpy( pResp->m_szErrorDescription, pBuf + 1, dwDescriptionLen );
        pResp->m_szErrorDescription[dwDescriptionLen] = '\0';
    }
    // -ERR ...\r\n
    else if ( '-' == pBuf[0] )
    {
        if ( dwBufSize <= 3 )
        {
            g_pfnFreeMem( pResp );
            return -1;
        }

        if ( '\r' != pBuf[dwBufSize-2] || '\n' != pBuf[dwBufSize-1] )
        {
            g_pfnFreeMem( pResp );
            return -1;
        }

        pResp->m_bError = TRUE;
        pResp->m_eType  = REDIS_RESP_TYPE_SINGLE_LINE;

        pResp->m_szErrorDescription = (char *)g_pfnAllocMem( sizeof(char) * (dwBufSize) );
        if ( 0 == pResp->m_szErrorDescription )
        {
            g_pfnFreeMem(pResp);
            return -1;
        }

        DWORD  dwDescriptionLen = dwBufSize - 3;

        memcpy( pResp->m_szErrorDescription, pBuf + 1, dwDescriptionLen );
        pResp->m_szErrorDescription[dwDescriptionLen] = '\0';
    }
    // :NUMBER\r\n
    else if ( ':' == pBuf[0] )
    {
        if ( dwBufSize <= 3 )
        {
            g_pfnFreeMem( pResp );
            return -1;
        }

        if ( '\r' != pBuf[dwBufSize-2] || '\n' != pBuf[dwBufSize-1] )
        {
            g_pfnFreeMem( pResp );
            return -1;
        }

        pResp->m_bError = FALSE;
        pResp->m_eType  = REDIS_RESP_TYPE_NUMBER;

        sscanf( pBuf + 1, "%d", &pResp->data.m_nNumber );
    }
    // $number\r\ndata\r\n
    else if ( '$' == pBuf[0] )
    {
        if ( dwBufSize <= 5 )
        {
            g_pfnFreeMem( pResp );
            return -1;
        }

        if ( '\r' != pBuf[dwBufSize-2] || '\n' != pBuf[dwBufSize-1] )
        {
            g_pfnFreeMem( pResp );
            return -1;
        }



        pResp->m_bError = FALSE;
        pResp->m_eType  = REDIS_RESP_TYPE_BULK;

        
        int nParamLen = 0;
        sscanf( pBuf + 1, "%d", &nParamLen );


        if ( nParamLen >= 0 )
        {
            DWORD i;
            for ( i = 1; i < dwBufSize - 2; i++ )
            {
                if ( '\r' == pBuf[i] )
                {
                    break;
                }
            }

            if ( i >= dwBufSize - 2 )
            {
                g_pfnFreeMem( pResp );
                return -1;
            }

            if ( '\n' != pBuf[i+1] )
            {
                g_pfnFreeMem( pResp );
                return -1;
            }

            DWORD dwParamRealLen = dwBufSize - (i + 2);
            if ( dwParamRealLen < 2 )
            {
                g_pfnFreeMem( pResp );
                return -1;
            }

            dwParamRealLen -= 2;

            // 实际长度和描述长度不符合
            if (  (int)dwParamRealLen != nParamLen )
            {
                g_pfnFreeMem( pResp );
                return -1;
            }


            pResp->data.m_pFirstParam = (PTRedisRespParam)g_pfnAllocMem( sizeof(TRedisRespParam) );
            if ( 0 == pResp->data.m_pFirstParam )
            {
                g_pfnFreeMem( pResp );
                return -1;
            }
            memset( pResp->data.m_pFirstParam, 0, sizeof(TRedisRespParam) );

            pResp->data.m_pFirstParam->m_pParam = (char *)g_pfnAllocMem( nParamLen + 1 );
            if ( 0 == pResp->data.m_pFirstParam )
            {
                g_pfnFreeMem( pResp->data.m_pFirstParam );
                g_pfnFreeMem( pResp );
                return -1;
            }   

            if ( nParamLen > 0 )
            {
                memcpy( pResp->data.m_pFirstParam->m_pParam, pBuf + i + 2, nParamLen );
            }

            pResp->data.m_pFirstParam->m_pParam[nParamLen] = '\0';
            pResp->data.m_pFirstParam->m_dwParamLen = nParamLen;
        }
    }

    *ppResp = pResp;

    return 0;
}


REDIS_CLIENT_API  int  FreeRedisResp( PTRedisResp pResp )
{
    if ( 0 == pResp )
    {
        return -1;
    }

    if ( pResp->m_szErrorDescription )
    {
        g_pfnFreeMem( pResp->m_szErrorDescription );
    }

    if ( REDIS_RESP_TYPE_BULK == pResp->m_eType || REDIS_RESP_TYPE_MULTI_BULK == pResp->m_eType )
    {
        PTRedisRespParam pParam = pResp->data.m_pFirstParam;
        while( pParam )
        {
            PTRedisRespParam pTmp = pParam;
            pParam = pParam->m_pNext;

            if ( pTmp->m_pParam )
            {
                g_pfnFreeMem( pTmp->m_pParam );
            }
            g_pfnFreeMem( pTmp );
        }
    }

    g_pfnFreeMem( pResp );

    return 0;
}