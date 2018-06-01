#include "StdAfx.h"
#include <malloc.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#include <WinSock2.h>
#include <Mstcpip.h>

#define ioctl ioctlsocket
#define close closesocket


#define ERRNOGET                GetLastError()
#define ERRNOSET(a)             SetLastError(a)

#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE


#else
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>

#define ERRNOGET                errno
#define ERRNOSET(a)             errno = a
#ifndef min
#define min(a, b)               ((a)<(b)?(a):(b))
#endif

#endif

typedef struct sockaddr * SOCKADDRPTR;  /* AL - 09/17/98 */
typedef unsigned long *   ioctlOnPtrTypeCast;

#ifdef WIN32
typedef int               sockaddr_namelen;
#else
typedef socklen_t         sockaddr_namelen;
#define INVALID_SOCKET    -1
#endif


#include "Inner.h"
#include "LmnCommon.h"




#define seliEvRead    0x01
#define seliEvWrite   0x02
#define seliEvExept   0x04


#define SELI_INNER_TYPE_TCP       0x01
#define SELI_INNER_TYPE_UDP       0x02
#define SELI_INNER_TYPE_SOCK5     0x04


/* SELI list of nodes. */
typedef struct
{
    BOOL               valid;         /* 是否正在被使用 */ 
    BOOL               inuse;         
    int                liEvent;       /* on what events the application want to be notified. */    // 用于回调通知
    int                seliEvent;     /* on what events the application want to be notified. */    // 用于select 的socket集合
    LPLIEVENTHANDLER   callback;      /* callback function */
    void *             context;       /* of application */ 
    int                fd;
    int                liState;
    BOOL               bException;    
    DWORD              dwTick;        /* 用来计算连接超时 */
    DWORD              dwTimeOut;     /* 超时时间，单位：毫秒 */

    DWORD              dwInnerType;

    DWORD              dwId;          // for selix, 唯一编号
    LPLIXEVENTHANDLER  callback_ex;
} seliNode;






typedef struct {
    const char *pName;
    int   protNum;
    int   protType;
} rvprot_t;

static rvprot_t  s_liProtData [2] =
{
    {"UDP",IPPROTO_UDP,SOCK_DGRAM},
    {"TCP",IPPROTO_TCP,SOCK_STREAM}
};


static  BOOL           s_bInited    = FALSE;
static  int            s_nInitCnt   = 0;       // 记录有多少模块调用初始化过
static  LmnLockType    s_seliLock;
static  seliNode *     s_nodes      = 0;
static  int            s_max_fd_cnt = FD_SETSIZE;




static BOOL  _seliSelectAdd2fdSet( fd_set *rdSet, fd_set *wrSet, fd_set *exSet,
                                   int fd, int sEvent )
{
	BOOL  bRet = FALSE;

#ifdef  WIN32
	SOCKET fdNum = (SOCKET)fd;
#else
	int fdNum = fd;
#endif
	
	if (sEvent & seliEvRead) 
	{
		FD_SET(fdNum, rdSet);
		bRet = TRUE;
	}
	else
	{
		FD_CLR(fdNum, rdSet);
	}

	if (sEvent & seliEvWrite) 
	{
		FD_SET(fdNum, wrSet);
		bRet = TRUE;
	}
	else
	{
		FD_CLR(fdNum, wrSet);
	}

	if (sEvent & seliEvExept) 
	{
		FD_SET(fdNum, exSet);
		bRet = TRUE;
	}
	else
	{
		FD_CLR(fdNum, exSet);
	}
	
	return bRet;
}



static int _GetIdleNodeIndex( )
{
    for( int i = 0; i < s_max_fd_cnt; i++ )
    {
        if( !s_nodes[i].valid )
            return i;
    }
    return -1;
}




static void _liUnblock (int sockId)
{
    int  rc = 0;
    BOOL on = TRUE;

    if ( (rc = ioctl(sockId,FIONBIO,(ioctlOnPtrTypeCast)&on)) < 0 )
    {
        // error
    }
}




static int _liBytesAvailable ( int sockId,int *bytesAvailable )
{
    int return_value;
    return_value = ioctl(sockId,FIONREAD,(ioctlOnPtrTypeCast)bytesAvailable);
    if( return_value < 0 )
        *bytesAvailable = return_value;
    return(return_value);
}

static void _SetAllUnUse ( )
{
    for ( int nIndex = 0; nIndex < s_max_fd_cnt; nIndex++ )
    {
        if ( s_nodes[nIndex].valid )
        {
            s_nodes[nIndex].inuse = FALSE;
        }
    }
}




/****************************************  seli  ***************************************************/



COMMONLMN_API  int  seliInit( DWORD dwMaxSocketCnt )
{
    if ( 0 == dwMaxSocketCnt || dwMaxSocketCnt > FD_SETSIZE )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( s_bInited )
    {
        // return LMN_ERR_ALREADY_INITED;
        s_nInitCnt++;
        return LMN_ERR_OK;
    }

#if WIN32
    WSADATA  wsaData;
    if( WSAStartup( 0x0202, &wsaData ) != 0 )
    {
        return LMN_ERR_UNKNOWN;
    }
#endif

    LmnInitLock( &s_seliLock );
    
    s_nodes = (seliNode *)g_pfnAllocMem( sizeof(seliNode) * dwMaxSocketCnt );
    if ( 0 == s_nodes )
    {
        return LMN_ERR_NO_MEMORY;
    }
    memset( s_nodes, 0, sizeof(seliNode) * dwMaxSocketCnt );
    s_max_fd_cnt = dwMaxSocketCnt;

    for ( int i = 0; i < s_max_fd_cnt; i++ )
    {
        s_nodes[i].fd = INVALID_SOCKET;
    }

    s_bInited = TRUE;
    s_nInitCnt++;

    return LMN_ERR_OK;
}


COMMONLMN_API  int  seliEnd( )
{
    if ( !s_bInited )
    {
        return LMN_ERR_OK;
    }
    
    assert( s_nodes );
    assert( s_nInitCnt > 0 );

    s_nInitCnt--;


    // 如果引用的模块个数到0
    if ( 0 == s_nInitCnt )
    {
        // 关闭所有的socket
        for( int i = 0; i < s_max_fd_cnt; i++ )
        {
            if ( s_nodes[i].valid )
            {
                close( s_nodes[i].fd );
            }
        }

        g_pfnFreeMem( s_nodes );
        s_nodes = 0;
        LmnDeinitLock( &s_seliLock );

        s_bInited = FALSE;
    }

    return LMN_ERR_OK;
}


// select多路复用
// nMs单位， 毫秒 
COMMONLMN_API  int  seliSelect( DWORD * pdwMs )
{
    struct timeval  tv;
    struct timeval * pTv = 0;

    int nMaxfd = -1;
    int nRet   = LMN_ERR_OK;
    int nIndex = 0;
    int nNum   = 0;

    fd_set tmprdSet;
    fd_set tmpwrSet;
    fd_set tmpexSet;


    if ( !s_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    // 20 ~ 1000 毫秒
    if ( 0 == pdwMs || *pdwMs > 1000 || *pdwMs < 20 )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    DWORD dwBeginTick  =  LmnGetTickCount();
    DWORD dwEndTick = 0;


    FD_ZERO(&tmprdSet);
    FD_ZERO(&tmpwrSet);
    FD_ZERO(&tmpexSet);


    LmnLock( &s_seliLock );
    for ( nIndex = 0; nIndex < s_max_fd_cnt; nIndex++ )
    {
        if ( s_nodes[nIndex].valid && !s_nodes[nIndex].bException )
        {
            // 防止回调函数中用li系列函数造成混淆
            s_nodes[nIndex].inuse = TRUE;

            assert( s_nodes[nIndex].fd != INVALID_SOCKET );

            if ( _seliSelectAdd2fdSet( &tmprdSet, &tmpwrSet, &tmpexSet, s_nodes[nIndex].fd, s_nodes[nIndex].seliEvent ) )
            {
                if ( nMaxfd < s_nodes[nIndex].fd )
                {
                    nMaxfd = s_nodes[nIndex].fd;
                }
            }
        }
    }

    // socket set都是空的
    if ( nMaxfd < 0 )
    {
        // 设置inuse
        _SetAllUnUse();

        LmnUnlock( &s_seliLock );
        // MYTRACE("=================\n");
        dwEndTick =  LmnGetTickCount();
        DWORD dwElapsed = dwEndTick - dwBeginTick;
        if ( *pdwMs > dwElapsed )
        {
            *pdwMs -= dwElapsed;
        }
        else
        {
            *pdwMs = 0;
        }
        return LMN_ERR_OK;
    }


    nMaxfd++;


    tv.tv_sec  =  *pdwMs / 1000;
    tv.tv_usec = ( *pdwMs % 1000 ) * 1000;
    pTv        = &tv;

    nNum = select( nMaxfd, &tmprdSet, &tmpwrSet, &tmpexSet, pTv );
    if( nNum < 0 )
    {
        // error
        nRet = LMN_ERR_SELECT;
    }
    else if( nNum > 0 )
    {
        // MYTRACE("===================\n");
        // MYTRACE(" nNum = %d \n", nNum );

        int fd = -1;

        for ( nIndex = 0; nIndex < s_max_fd_cnt; nIndex++ )
        {
            // ready用来防止在后面的回调函数中调用li函数产生新的socket进入该循环
            if ( !s_nodes[nIndex].valid || !s_nodes[nIndex].inuse )
            {
                continue;
            }

            fd = s_nodes[nIndex].fd;
            assert( fd != INVALID_SOCKET );

            int liEvent = 0;
            int nError = 0;

#ifdef  DEBUG
            // 检查出现在哪些集合里
            int nBitMask = 0;
#endif

            if ( FD_ISSET(fd, &tmpexSet) )
            {
#ifdef  DEBUG
                nBitMask |= 0x04;
#endif
                // MYTRACE("====== exception! ( will close)  \n");
                liEvent = liEvClose;
                nError  = -1;

                s_nodes[nIndex].bException = TRUE;
            }
            // 读集合
            else if ( FD_ISSET(fd, &tmprdSet) )
            {
#ifdef  DEBUG
                nBitMask |= 0x01;
#endif
                int nReadBytes  = 0;
                _liBytesAvailable( fd, &nReadBytes );
                // MYTRACE("====== on read event ! nReadBytes = %d  \n", nReadBytes );

                if ( (s_nodes[nIndex].liEvent & liEvRead) && (nReadBytes > 0) )
                {
                    liEvent = liEvRead;
                }
                else if ( (s_nodes[nIndex].liEvent & liEvClose) && (nReadBytes <= 0) ) 
                {
                    // MYTRACE("====== close on read event ! nReadBytes = %d  \n", nReadBytes );
                    liEvent = liEvClose;

                    // 对端关闭，下次select时候不再关注读数据
                    s_nodes[nIndex].seliEvent = 0;
                }
                else if ( s_nodes[nIndex].liEvent & liEvAccept )             
                {
                    // MYTRACE("====== accept ! nReadBytes = %d  \n", nReadBytes );
                    liEvent = liEvAccept;
                }
            }
            else if ( FD_ISSET(fd, &tmpwrSet) )
            {
#ifdef  DEBUG
                nBitMask |= 0x02;
#endif
                if ( s_nodes[nIndex].liEvent & liEvConnect )
                {
                    liEvent = liEvConnect;

                    // 下次select时候不再关注连接
                    s_nodes[nIndex].seliEvent = seliEvRead;
                    s_nodes[nIndex].liEvent   = liEvRead | liEvClose;
                    
                    assert( s_nodes[nIndex].liState  == liStConnecting );

                    s_nodes[nIndex].liState   = liStAvailable;
                }
                else if ( s_nodes[nIndex].liEvent & liEvWrite )            
                {
                    liEvent = liEvWrite;
                }
            }

            if ( liEvent != 0 && s_nodes[nIndex].callback != 0  ) 
            {
#ifdef  DEBUG
                // MYTRACE("event = %d, nBitMask = 0x%08X \n", liEvent, (DWORD)nBitMask );
#endif
                s_nodes[nIndex].callback( nIndex, liEvent, nError, s_nodes[nIndex].context );
            }


            if ( FD_ISSET(fd, &tmpexSet) )
            {
                nNum--;
            }

            if ( FD_ISSET(fd, &tmprdSet) )
            {
                nNum--;
            }

            if ( FD_ISSET(fd, &tmpwrSet) )
            {
                nNum--;
            }

            if ( 0 == nNum )
            {
                break;
            }
        }

        assert( 0 == nNum );
    }

    DWORD dwCurTick =  LmnGetTickCount();
    for ( nIndex = 0; nIndex < s_max_fd_cnt; nIndex++ )
    {
        if ( s_nodes[nIndex].valid && s_nodes[nIndex].inuse )
        {
            // 设置inuse
            s_nodes[nIndex].inuse = FALSE;

            // 如果在连接中，检查是否超时
            if ( !s_nodes[nIndex].bException && s_nodes[nIndex].liState == liStConnecting )
            {
                if ( dwCurTick - s_nodes[nIndex].dwTick > s_nodes[nIndex].dwTimeOut )
                {
                    // 连接超时也认为异常
                    s_nodes[nIndex].bException = TRUE;

                    if ( s_nodes[nIndex].callback != 0  ) 
                    {
                        s_nodes[nIndex].callback( nIndex, liEvClose, 0, s_nodes[nIndex].context );
                    }
                }
            }
        }
    }

    LmnUnlock( &s_seliLock );

    dwEndTick =  LmnGetTickCount();
    DWORD dwElapsed = dwEndTick - dwBeginTick;
    if ( *pdwMs > dwElapsed )
    {
        *pdwMs -= dwElapsed;
    }
    else
    {
        *pdwMs = 0;
    }
    return nRet;
}




/****************************************  END seli ****************************************************/





#define  DEFAULT_TCP_KEEPALIVE_IDLE_TIME       60                 // 单位：秒
#ifdef WIN32
#define  DEFAULT_TCP_KEEPALIVE_INTERVAL        30                 // 单位：秒
#else
#define  DEFAULT_TCP_KEEPALIVE_INTERVAL        60                 // 单位：秒
#endif
#define  DEFAULT_TCP_KEEPALIVE_PROBE_COUNT     3


/**************************************** li *******************************************************/


COMMONLMN_API  FdType liOpen ( liProtocol_t protocol, DWORD ipAddr, WORD wPort, 
							   LPLIEVENTHANDLER eventHandler, void* context,
                               const TKeepAliveParam * pKeepAliveParam )
{
    int s,nIndex;
    struct sockaddr_in sin; 
    BOOL on = TRUE;
    struct linger Linger;
    int yes = TRUE;


    if ( protocol > LI_PROTOTYPE_TCP || 0 == eventHandler )
    {
        return INVALID_SOCKET;
    }


    if ( !s_bInited )
    {
        PERROR("not inited! \n");
        return INVALID_SOCKET;
    }


    Linger.l_onoff  = TRUE; 
    Linger.l_linger = 0;   


    /* allocate a socket */
    if ( (s = socket (AF_INET, s_liProtData [protocol].protType, s_liProtData [protocol].protNum)) <  0 )  
    {
        PERROR("failed to socket! \n");
        return INVALID_SOCKET;
    }

    // set option
    if ( LI_PROTOTYPE_TCP == protocol )
    {
        // setsockopt(s, SOL_SOCKET,  SO_LINGER,    (char *)&Linger, sizeof(Linger));
        // setsockopt(s, IPPROTO_TCP, TCP_NODELAY,  (char *)&yes,    sizeof(yes));

        if ( pKeepAliveParam && pKeepAliveParam->bTcpKeepAlive )
        {
            if ( setsockopt(s, SOL_SOCKET,  SO_KEEPALIVE, (char *)&yes, sizeof(yes)) < 0  )
            {
                PERROR( "fail to set SO_KEEPALIVE! \n" );
            }

			TKeepAliveParam  tTmp;
			memcpy( &tTmp, pKeepAliveParam, sizeof(TKeepAliveParam) );

#ifndef WIN32
			if ( 0 == tTmp.dwCount )
			{
				tTmp.dwCount = DEFAULT_TCP_KEEPALIVE_PROBE_COUNT;
			}
#endif

			if ( 0 == tTmp.dwIdleTime )
			{
				tTmp.dwIdleTime = DEFAULT_TCP_KEEPALIVE_IDLE_TIME;
			}

			if ( 0 == tTmp.dwIntervalTimve )
			{
				tTmp.dwIntervalTimve = DEFAULT_TCP_KEEPALIVE_INTERVAL;
			}
			

#ifdef WIN32
            tcp_keepalive live, liveout;     
            live.keepaliveinterval = tTmp.dwIntervalTimve * 1000;      
            live.keepalivetime     = tTmp.dwIdleTime * 1000;
            live.onoff             = TRUE;     

            DWORD dwTmp;

            if ( 0 != WSAIoctl( s, SIO_KEEPALIVE_VALS, &live, sizeof(live), &liveout, sizeof(liveout), &dwTmp, NULL,NULL ) )
            {
                PERROR("failed to WASIOCTL! \n");
            }
#else
            int keepidle  = tTmp.dwIdleTime;                       // 开始探测前的空闲等待时间
            int keepintvl = tTmp.dwIntervalTimve;                  // 发送探测分节的时间间隔
            int keepcnt   = tTmp.dwCount;                          // 发送探测分节的次数

            if ( setsockopt( s, SOL_TCP, TCP_KEEPIDLE, (void *)&keepidle, sizeof(keepidle) ) < 0 )
            {
                PERROR( "fail to set TCP_KEEPIDLE! %d \n", ERRNOGET );
            }

            if ( setsockopt( s, SOL_TCP, TCP_KEEPINTVL, (void *)&keepintvl, sizeof (keepintvl) ) < 0 )
            {
                PERROR( "fail to set TCP_KEEPINTVL! %d \n", ERRNOGET );
            }

            if ( setsockopt( s, SOL_TCP, TCP_KEEPCNT, (void *)&keepcnt, sizeof (keepcnt) ) < 0 )
            {
                PERROR( "fail to set TCP_KEEPCNT! %d \n", ERRNOGET );
            }
#endif   
        } // keep alive

    }

    // NON-BLOCK socket
    if ( ioctl(s,FIONBIO,(ioctlOnPtrTypeCast)&on) < 0 ) 
    {
        PERROR("failed to unblock the socket \n");
        close(s);
        return INVALID_SOCKET;
    }

    // reuse address
    int nCmd = 1;
    setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (char *)&nCmd, sizeof(int));

    // bind
    if ( wPort > 0 )
    {
        memset(&sin, 0, sizeof(sin));
        sin.sin_family      = AF_INET;
        sin.sin_port        = htons(wPort);
        sin.sin_addr.s_addr = ipAddr;

        if ( bind (s, (SOCKADDRPTR)&sin ,sizeof (sin)) < 0 ) 
        {
            PERROR("failed to bind the socket(), port = %lu! \n", (DWORD)wPort);
            close(s);
            return INVALID_SOCKET;
        }
    }
    

    LmnLock( &s_seliLock );
    nIndex = _GetIdleNodeIndex(  );

    // 如果没有空间了
    if ( nIndex < 0 )
    {
        PERROR("NO socket array space! \n");
        close( s );
        LmnUnlock( &s_seliLock );
        return INVALID_SOCKET;
    }

    s_nodes[nIndex].valid      = TRUE;
    s_nodes[nIndex].inuse      = FALSE;
    s_nodes[nIndex].fd         = s;
    s_nodes[nIndex].callback   = eventHandler;
    s_nodes[nIndex].context    = context;
    s_nodes[nIndex].seliEvent  = 0;
    s_nodes[nIndex].liEvent    = 0;
    s_nodes[nIndex].liState    = liStInited;
    s_nodes[nIndex].bException = FALSE;
    s_nodes[nIndex].dwTick     = 0;
    s_nodes[nIndex].dwTimeOut  = 0;

    if ( LI_PROTOTYPE_UDP == protocol )
    {
        s_nodes[nIndex].seliEvent = seliEvRead | seliEvWrite;
        s_nodes[nIndex].liEvent   =  liEvRead | liEvWrite ;
        s_nodes[nIndex].liState   = liStAvailable;

        s_nodes[nIndex].dwInnerType = SELI_INNER_TYPE_UDP;
    }
    else
    {
        s_nodes[nIndex].dwInnerType = SELI_INNER_TYPE_TCP;
    }
    
    LmnUnlock( &s_seliLock );

    // 返回索引值
    return nIndex;
}

// 参数fd实际是静态数据的索引
COMMONLMN_API  int liClose ( FdType fd )
{
    BOOL on = FALSE;
    int rc;

    if ( !s_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }

    LmnLock( &s_seliLock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_NOT_SELI_SOCKET;
    }


    rc = ioctl( s_nodes[fd].fd, FIONBIO, (ioctlOnPtrTypeCast)&on );
    if ( 0 != rc )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_IOCTL;
    }

    rc = close( s_nodes[fd].fd );
    if ( 0 != rc )
    {
        PERROR("failed to close socket! \n");
        LmnUnlock( &s_seliLock );
        return LMN_ERR_CLOSE;
    }

    s_nodes[fd].valid      = FALSE;
    s_nodes[fd].inuse      = FALSE;
    s_nodes[fd].callback   = 0;
    s_nodes[fd].liEvent    = 0;
    s_nodes[fd].seliEvent  = 0;
    s_nodes[fd].fd         = INVALID_SOCKET;
    s_nodes[fd].liState    = liStInited;
    s_nodes[fd].bException = FALSE;
    s_nodes[fd].dwTick     = 0;
    s_nodes[fd].dwTimeOut  = 0;

    if ( s_nodes[fd].context )
    {
        if ( s_nodes[fd].dwInnerType & SELI_INNER_TYPE_SOCK5 )
        {
            g_pfnFreeMem( s_nodes[fd].context );
        }
    }
    s_nodes[fd].context       = 0;
    s_nodes[fd].dwInnerType   = 0;

    LmnUnlock( &s_seliLock );
    return LMN_ERR_OK;
}

// dwTimeOut 单位:秒
// 参数fd实际是静态数据的索引
COMMONLMN_API  int  liConnect ( int fd, DWORD ipAddr, WORD wPort, DWORD dwTimeOut )
{
    struct sockaddr_in sin;
    int status;

    if ( !s_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }

    
    // 最短timeout 5秒
    if ( dwTimeOut < 5  )
    {
        dwTimeOut = 5;
    }
    // 最长timeout 20秒
    else if ( dwTimeOut > 20 )
    {
        dwTimeOut = 20;
    }


    LmnLock( &s_seliLock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_NOT_SELI_SOCKET;
    }
    
    memset(&sin, 0, sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons( wPort );
    sin.sin_addr.s_addr = ipAddr;

    if ( ( status = connect ( s_nodes[fd].fd, (SOCKADDRPTR) &sin ,sizeof(sin)) ) < 0 )
    {
#ifdef WIN32
        if ( (ERRNOGET == EINPROGRESS) || (ERRNOGET == WSAEWOULDBLOCK) )  /* connection in progress */
#else
        if (ERRNOGET == EINPROGRESS)                                      /* connection in progress */
#endif
        {
            // OK
        }
        else
        {
            LmnUnlock( &s_seliLock );
            return LMN_ERR_CONNECT;
        }
    }

#ifdef WIN32
    s_nodes[fd].seliEvent = seliEvWrite| seliEvExept;
#else
    s_nodes[fd].seliEvent = seliEvRead | seliEvWrite;
#endif

    s_nodes[fd].liEvent   = liEvConnect | liEvClose ;

    s_nodes[fd].liState   = liStConnecting;
    s_nodes[fd].dwTick    = LmnGetTickCount();
    s_nodes[fd].dwTimeOut = dwTimeOut * 1000;

    LmnUnlock( &s_seliLock );
    return LMN_ERR_OK;
}



COMMONLMN_API  int liListen(int fd, int queueLen )
{
    if ( !s_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }


    LmnLock( &s_seliLock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_NOT_SELI_SOCKET;
    }

    if ( (listen ( s_nodes[fd].fd, queueLen) ) < 0 ) 
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_LISTEN;
    }

    s_nodes[fd].seliEvent = seliEvRead ;
    s_nodes[fd].liEvent   = liEvAccept;

    s_nodes[fd].liState   = liStListening;

    LmnUnlock( &s_seliLock );
    return LMN_ERR_OK;
}


COMMONLMN_API  FdType liAccept(int fd, LPLIEVENTHANDLER fnCallBack, void * context)
{
    struct sockaddr_in fsin;  /* the request from addr. */
    int socketId, nAcceptIndex = -1;

    if ( 0 == fnCallBack )
    {
        return -1;
    }

    if ( !s_bInited )
    {
        return -1;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return -1;
    }

    LmnLock( &s_seliLock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &s_seliLock );
        return -1;
    }


    sockaddr_namelen alen = (sockaddr_namelen)sizeof(fsin);

    socketId = accept( s_nodes[fd].fd,(SOCKADDRPTR) &fsin,&alen);

    if ( socketId >= 0 )
    {
        struct linger Linger;

        Linger.l_onoff = TRUE;
        Linger.l_linger = 0;
        setsockopt(socketId,SOL_SOCKET, SO_LINGER, (char *)&Linger, sizeof(Linger));


        nAcceptIndex = _GetIdleNodeIndex( );
        if ( nAcceptIndex < 0 )
        {
            close( socketId );
            LmnUnlock( &s_seliLock );
            return -1;
        }

        s_nodes[nAcceptIndex].valid      = TRUE;
        s_nodes[nAcceptIndex].inuse      = FALSE;
        s_nodes[nAcceptIndex].fd         = socketId;
        s_nodes[nAcceptIndex].callback   = fnCallBack;
        s_nodes[nAcceptIndex].context    = context;
        s_nodes[nAcceptIndex].seliEvent  = seliEvRead;
        s_nodes[nAcceptIndex].liEvent    =  liEvRead | liEvClose ;
        s_nodes[nAcceptIndex].liState    = liStAvailable;
        s_nodes[nAcceptIndex].bException = FALSE;
        s_nodes[nAcceptIndex].dwTick     = 0;
        s_nodes[nAcceptIndex].dwTimeOut  = 0;

        _liUnblock(socketId);
    }

    LmnUnlock( &s_seliLock );
    return nAcceptIndex;
}


COMMONLMN_API  int liShutdown( int fd )
{
    int rc;

    if ( !s_bInited )
    {
        return LMN_ERR_UNKNOWN;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }

    LmnLock( &s_seliLock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_NOT_SELI_SOCKET;
    }

    rc = shutdown( s_nodes[fd].fd, 1);
    if ( 0 != rc )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_SHUTDOWN;
    }

    s_nodes[fd].seliEvent &= ~seliEvRead;

    s_nodes[fd].liState = liStShutdowned;

    LmnUnlock( &s_seliLock );
    return LMN_ERR_OK;
}


COMMONLMN_API  int liTcpSend ( int fd, const char *buff, int * plen )
{
    int ret;

    if ( (0 == plen) || (*plen < 0) || (*plen > 0 && buff == 0) )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !s_bInited )
    {
        return LMN_ERR_UNKNOWN;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }

    LmnLock( &s_seliLock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_NOT_SELI_SOCKET;
    }
    
    ret = send( s_nodes[fd].fd, buff, *plen, 0 );
    if ( ret < 0 && ERRNOGET == EWOULDBLOCK )
    {
        ret = 0;
    }

    if ( ret >= 0 )
    {
        *plen = ret;
        LmnUnlock( &s_seliLock );
        return LMN_ERR_OK;
    }
    else
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_SEND;
    }
}


COMMONLMN_API  int liTcpRecv (int fd, char *buff,int * plen)
{
    if ( !s_bInited )
    {
        return LMN_ERR_UNKNOWN;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt || 0 == plen || *plen <= 0 || 0 == buff )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    LmnLock( &s_seliLock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_NOT_SELI_SOCKET;
    }
    
    int ret, bytes;

    _liBytesAvailable ( s_nodes[fd].fd, &bytes );
    if ( bytes < 0 )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_UNKNOWN;
    }
    else if ( bytes == 0 )
    {
        *plen = 0;
        LmnUnlock( &s_seliLock );
        return 0;
    }

    ret = recv ( s_nodes[fd].fd, buff, *plen, 0 );
    if ((ret < 0 ) && (ERRNOGET == EWOULDBLOCK))
    {
        ret=0;
    }

    *plen = ret;

    if ( ret >= 0 )
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_OK;
    }
    else
    {
        LmnUnlock( &s_seliLock );
        return LMN_ERR_RECV;
    }
}


// 返回值：
//          -1 ERROR
COMMONLMN_API  int liGetState (int fd)
{
    if ( !s_bInited )
    {
        return -1;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return -1;
    }

    LmnLock( &s_seliLock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &s_seliLock );
        return -1;
    }

    int nTmpState = s_nodes[fd].liState;

    LmnUnlock( &s_seliLock );
    return nTmpState;
}

COMMONLMN_API  int liGetSockName( int fd, DWORD * pdwIp, WORD * pwPort, BOOL bPeer /*= FALSE*/ )
{
    if ( !s_bInited )
    {
        return -1;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return -1;
    }

    LmnLock( &s_seliLock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &s_seliLock );
        return -1;
    }

    struct sockaddr_in addr;
#ifdef WIN32
    int len = sizeof(struct sockaddr_in);
#else
    socklen_t len = sizeof(struct sockaddr_in);
#endif // WIN32
    

    if ( bPeer )
    {
        if ( 0 != getpeername( s_nodes[fd].fd, (struct sockaddr *)&addr, &len ) )
        {
            LmnUnlock( &s_seliLock );
            return -1;
        }
    }
    else
    {
        if ( 0 != getsockname( s_nodes[fd].fd, (struct sockaddr *)&addr, &len ) )
        {
            LmnUnlock( &s_seliLock );
            return -1;
        }
    }
    
    
    if ( pwPort )
    {
        *pwPort = ntohs( addr.sin_port );
    }

    if ( pdwIp )
    {
        *pdwIp = addr.sin_addr.s_addr;
    }

    LmnUnlock( &s_seliLock );
    return 0;
}


#define  SS5_PROXY_VERSION                                '\x05'
#define  SS5_PROXY_AUTH_METHOD_NO_AUTH                    '\x00'
#define  SS5_PROXY_AUTH_METHOD_USER_PWD                   '\x02'


#define  SS5_PROXY_CONNECT_CMD                            '\x01'
#define  SS5_PROXY_RSV                                    '\x00'
#define  SS5_PROXY_IPV4_TYPE                              '\x01'


enum Sock5Status
{
    SOCK5_STATUS_METHOD_NEGOTIATING = 0,
    SOCK5_STATUS_CONNECTING_DEST,
    SOCK5_STATUS_USER_PWD_AUTHENING,
    SOCK5_STATUS_OK
};

static void  _GetSock5CnnDestQueryBuf( LmnCommon::CDataBuf & data, const TCnnSock5Param * pContext )
{
    BYTE  byTmp;

    byTmp = SS5_PROXY_VERSION;
    data.Append( &byTmp, sizeof(byTmp) );

    byTmp = SS5_PROXY_CONNECT_CMD;
    data.Append( &byTmp, sizeof(byTmp) );

    byTmp = SS5_PROXY_RSV;
    data.Append( &byTmp, sizeof(byTmp) );

    byTmp = SS5_PROXY_IPV4_TYPE;
    data.Append( &byTmp, sizeof(byTmp) );

    // ip
    data.Append( (void *)&pContext->dwDestAddr, sizeof(pContext->dwDestAddr) );

    WORD wDestPort = htons( pContext->wDestPort );
    data.Append( &wDestPort,  sizeof(wDestPort) );
}

static void _GetSock5UserPwdQueryBuf( LmnCommon::CDataBuf & data, const TCnnSock5Param * pContext )
{
    BYTE  byTmp;

    byTmp = '\x01';
    data.Append( (unsigned char *)&byTmp, sizeof(byTmp) );

    DWORD dwTmp;
    dwTmp = strlen( pContext->szUserName );

    byTmp = (BYTE)dwTmp;
    data.Append( &byTmp, sizeof(byTmp) );
    data.Append( (void *)pContext->szUserName, dwTmp );


    dwTmp = strlen( pContext->szPassword );

    byTmp = (BYTE)dwTmp;
    data.Append( &byTmp, sizeof(byTmp) );
    data.Append( (void *)pContext->szPassword, dwTmp );
}

static void _OnSock5Connected( TCnnSock5Param * pContext, FdType fd )
{
    LmnCommon::CDataBuf  data;

    BYTE sock5_version = SS5_PROXY_VERSION;
    data.Append( &sock5_version, sizeof(sock5_version) );           // VERSION

    BYTE auth_method_cnt = 0;
    if ( pContext->eMethod == SOCK5_METHOD_NONE )
    {
        auth_method_cnt = 1;
    }
    else
    {
        auth_method_cnt = 2;
    }
    data.Append( &auth_method_cnt, sizeof(auth_method_cnt) );       // AUTH METHOD count


    BYTE auth_method = SS5_PROXY_AUTH_METHOD_NO_AUTH;
    data.Append( &auth_method, sizeof(auth_method) );               // NO AUTH method

    auth_method = SS5_PROXY_AUTH_METHOD_USER_PWD;
    data.Append( &auth_method, sizeof(auth_method) );               // USER PASSWORD method

    int len = data.GetDataLength();
    liTcpSend( fd, (char *)data.GetData(), &len );

    pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_METHOD_NEGOTIATING;
}


static void _OnSock5Read( TCnnSock5Param * pContext, FdType fd )
{
    LmnCommon::CDataBuf data;
    char buf[NORMAL_BUF_SIZE];
    int  len = NORMAL_BUF_SIZE;

    Sock5Status state_ = (Sock5Status)pContext->reserverd0[0];

    if ( state_ == SOCK5_STATUS_OK )
    {
        pContext->eventHandler( fd, liEvRead, 0, pContext->context );
        return;
    }

    liTcpRecv( fd, buf, &len );
    
    //
    //  假定每次获取的数据都是一段完整的，非严格意义上的字节流解析
    //

    if ( state_ == SOCK5_STATUS_METHOD_NEGOTIATING )
    {
        if ( 2 != len && (BYTE)buf[0] != SS5_PROXY_VERSION )
        {
            s_nodes[fd].bException = TRUE;
            pContext->eventHandler( fd, liEvClose, 0, pContext->context );
        }
        else
        {
            // 如果是不用验证
            if ( SS5_PROXY_AUTH_METHOD_NO_AUTH == (BYTE)buf[1] )
            {
                _GetSock5CnnDestQueryBuf( data, pContext );
                len = data.GetDataLength();
                liTcpSend( fd, (char *)data.GetData(), &len );
                pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_CONNECTING_DEST;
            }
            // 如果是用户名/密码验证
            else if ( SS5_PROXY_AUTH_METHOD_USER_PWD == (BYTE)buf[1] )
            {
                _GetSock5UserPwdQueryBuf( data, pContext );
                len = data.GetDataLength();
                liTcpSend( fd, (char *)data.GetData(), &len );
                pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_USER_PWD_AUTHENING;

            }
            // 不支持的验证方法
            else
            {
                s_nodes[fd].bException = TRUE;
                pContext->eventHandler( fd, liEvClose, 0, pContext->context );
            }
        }
    }
    else if ( state_ == SOCK5_STATUS_USER_PWD_AUTHENING )
    {
        if ( 2 != len && (BYTE)buf[0] != '\x01' )
        {
            s_nodes[fd].bException = TRUE;
            pContext->eventHandler( fd, liEvClose, 0, pContext->context );
        }
        else
        {
            // 验证失败
            if ( buf[1] != '\x00' )
            {
                s_nodes[fd].bException = TRUE;
                pContext->eventHandler( fd, liEvClose, 0, pContext->context );
            }
            else
            {
                _GetSock5CnnDestQueryBuf( data, pContext );
                len = data.GetDataLength();
                liTcpSend( fd, (char *)data.GetData(), &len );
                pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_CONNECTING_DEST;
            }
        }
    }
    else if ( state_ == SOCK5_STATUS_CONNECTING_DEST )
    {
        if ( 10 != len && (BYTE)buf[0] != SS5_PROXY_VERSION && '\x00' != (BYTE)buf[1] )
        {
            s_nodes[fd].bException = TRUE;
            pContext->eventHandler( fd, liEvClose, 0, pContext->context );
        }
        // proxy 连接成功
        else
        {
            pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_OK;
            s_nodes[fd].liState = liStAvailable;

            pContext->eventHandler( fd, liEvConnect, 0, pContext->context );
        }
    }
}



static void _Sock5CallBack( FdType fd, int liEvent, int error, void * context )
{
    PTCnnSock5Param  pContext = (PTCnnSock5Param)context;

    switch ( liEvent )
    {
    case liEvClose:
        pContext->eventHandler( fd, liEvClose, 0, pContext->context );
        break;

    case liEvConnect:
		s_nodes[fd].liState = liStConnecting;
        _OnSock5Connected( pContext, fd );
    	break;

    case liEvRead:
        _OnSock5Read( pContext, fd );
        break;

    default:
        break;
    }
}



// 返回值： 0  OK
COMMONLMN_API  FdType  liConnectSock5 ( const TCnnSock5Param *  ptParam )
{
    if ( 0 == ptParam )
    {
        return INVALID_SOCKET;
    }

    if ( !s_bInited )
    {
        PERROR("not inited! \n");
        return INVALID_SOCKET;
    }

    LmnCommon::CFuncLock  cLock( &s_seliLock );

    // 如果使用sock5
    if ( ptParam->bUseSock5 )
    {
        if ( 0 == ptParam->dwSock5Addr || 0 == ptParam->wSock5Port || 0 == ptParam->eventHandler )
        {
            return INVALID_SOCKET;
        }

        if ( SOCK5_METHOD_USER == ptParam->eMethod && '\0' == ptParam->szUserName[0] )
        {
            return INVALID_SOCKET;
        }

        PTCnnSock5Param  pContext = (PTCnnSock5Param)g_pfnAllocMem( sizeof(TCnnSock5Param) );
        if ( 0 == pContext )
        {
            return INVALID_SOCKET;
        }

        memcpy( pContext, ptParam, sizeof(TCnnSock5Param) );

        FdType fd = liOpen ( LI_PROTOTYPE_TCP, 0, 0, _Sock5CallBack, pContext, 0 );
        if ( fd == INVALID_SOCKET )
        {
            g_pfnFreeMem( pContext );
            return INVALID_SOCKET;
        }

        s_nodes[fd].dwInnerType |= SELI_INNER_TYPE_SOCK5;

        if ( 0 != liConnect( fd, ptParam->dwSock5Addr, ptParam->wSock5Port, ptParam->dwTimeOut ) )
        {
            liClose( fd );
            g_pfnFreeMem( pContext );
            return INVALID_SOCKET;
        }

        return fd;   
    }
    // 如果不使用sock5
    else
    {
        FdType fd = liOpen ( LI_PROTOTYPE_TCP, 0, 0, ptParam->eventHandler, ptParam->context, 0 );
        if ( fd == INVALID_SOCKET )
        {
            return INVALID_SOCKET;
        }

        if ( 0 != liConnect( fd, ptParam->dwDestAddr, ptParam->wDestPort, ptParam->dwTimeOut ) )
        {
            liClose( fd );
            return INVALID_SOCKET;
        }

        return fd;   
    }
}


/**************************************** END li ***********************************************/



















typedef  struct  tagSelixManager_
{
    BOOL            m_bInited;                          // 全局是否初始化
    LmnLockType     m_Lock;                             // 全局锁
    DWORD           m_dwTotalLimit;                     // 全局socket个数限制
    PList           m_lstSelixNodes;                    // 成员是 PTSelixNode
    DWORD           m_dwSocketMaxId;                    // 记录全局最大socket编号
    DWORD           m_dwSelixNodeMaxId;                 // 记录全局最大的SelixNode编号
    int             m_nInitCnt;

    tagSelixManager_()
    {
        memset( this, 0, sizeof(*this) );
    }
}TSelixManager_, *PTSelixManager_;


typedef  struct  tagSelixNode_
{
    seliNode *         m_nodes;
    DWORD              m_dwNodesSize;
    DWORD              m_dwId;
}TSelixNode_, *PTSelixNode_;


// 静态变量 SeliManager
static  TSelixManager_   s_SelixManager;







static DWORD  _GetManagerAllocatedSocketCount()
{
    assert ( s_SelixManager.m_bInited );
    assert ( s_SelixManager.m_lstSelixNodes );

    // 把SeliNode的所有分配的socket个数加起来
    DWORD  dwCount = 0;
    PListNode pNode = GetListHead( s_SelixManager.m_lstSelixNodes );
    while ( pNode )
    {
        PTSelixNode_  pSelixNode = (PTSelixNode_)pNode->pData;
        assert( pSelixNode );
        dwCount += pSelixNode->m_dwNodesSize;
        pNode = GetNextListNode( pNode );
    }

    return dwCount;
}

static BOOL  _IfExistSeliNode( SelixNode pSeli )
{
    assert ( pSeli );
    assert ( s_SelixManager.m_bInited );
    assert ( s_SelixManager.m_lstSelixNodes );

    PListNode pNode = GetListHead( s_SelixManager.m_lstSelixNodes );
    while ( pNode )
    {
        if ( pNode == pSeli )
        {
            return TRUE;
        }
        pNode = GetNextListNode( pNode );
    }
    return FALSE;
}

static int _GetIdleNodeIndex( PTSelixNode_  pSelixNode_ )
{
    for( int i = 0; i < (int)pSelixNode_->m_dwNodesSize; i++ )
    {
        if( !pSelixNode_->m_nodes[i].valid )
            return i;
    }
    return -1;
}

static void _CalculateTimeElapsed( DWORD *pdwMs, DWORD dwBeginTick )
{
    DWORD dwEndTick =  LmnGetTickCount();
    DWORD dwElapsed = dwEndTick - dwBeginTick;
    if ( *pdwMs > dwElapsed )
    {
        *pdwMs -= dwElapsed;
    }
    else
    {
        *pdwMs = 0;
    }
}

static void _OnSock5ConnectedX( TCnnSock5ParamX * pContext, SelixNode pSeli, FdType fd )
{
    LmnCommon::CDataBuf  data;

    BYTE sock5_version = SS5_PROXY_VERSION;
    data.Append( &sock5_version, sizeof(sock5_version) );           // VERSION

    BYTE auth_method_cnt = 0;
    if ( pContext->eMethod == SOCK5_METHOD_NONE )
    {
        auth_method_cnt = 1;
    }
    else
    {
        auth_method_cnt = 2;
    }
    data.Append( &auth_method_cnt, sizeof(auth_method_cnt) );       // AUTH METHOD count


    BYTE auth_method = SS5_PROXY_AUTH_METHOD_NO_AUTH;
    data.Append( &auth_method, sizeof(auth_method) );               // NO AUTH method

    auth_method = SS5_PROXY_AUTH_METHOD_USER_PWD;
    data.Append( &auth_method, sizeof(auth_method) );               // USER PASSWORD method

    int len = data.GetDataLength();
    lixTcpSend( pSeli, fd, (char *)data.GetData(), &len );

    pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_METHOD_NEGOTIATING;
}

static void  _GetSock5CnnDestQueryBuf( LmnCommon::CDataBuf & data, const TCnnSock5ParamX * pContext )
{
    BYTE  byTmp;

    byTmp = SS5_PROXY_VERSION;
    data.Append( &byTmp, sizeof(byTmp) );

    byTmp = SS5_PROXY_CONNECT_CMD;
    data.Append( &byTmp, sizeof(byTmp) );

    byTmp = SS5_PROXY_RSV;
    data.Append( &byTmp, sizeof(byTmp) );

    byTmp = SS5_PROXY_IPV4_TYPE;
    data.Append( &byTmp, sizeof(byTmp) );

    // ip
    data.Append( (void *)&pContext->dwDestAddr, sizeof(pContext->dwDestAddr) );

    WORD wDestPort = htons( pContext->wDestPort );
    data.Append( &wDestPort,  sizeof(wDestPort) );
}

static void _GetSock5UserPwdQueryBuf( LmnCommon::CDataBuf & data, const TCnnSock5ParamX * pContext )
{
    BYTE  byTmp;

    byTmp = '\x01';
    data.Append( (unsigned char *)&byTmp, sizeof(byTmp) );

    DWORD dwTmp;
    dwTmp = strlen( pContext->szUserName );

    byTmp = (BYTE)dwTmp;
    data.Append( &byTmp, sizeof(byTmp) );
    data.Append( (void *)pContext->szUserName, dwTmp );


    dwTmp = strlen( pContext->szPassword );

    byTmp = (BYTE)dwTmp;
    data.Append( &byTmp, sizeof(byTmp) );
    data.Append( (void *)pContext->szPassword, dwTmp );
}

static void _OnSock5Read( TCnnSock5ParamX * pContext, SelixNode pSeli, FdType fd )
{
    LmnCommon::CDataBuf data;
    char buf[NORMAL_BUF_SIZE];
    int  len = NORMAL_BUF_SIZE;

    Sock5Status state_ = (Sock5Status)pContext->reserverd0[0];

    if ( state_ == SOCK5_STATUS_OK )
    {
        pContext->eventHandler( pSeli, fd, liEvRead, 0, pContext->context );
        return;
    }

    PListNode pNode            = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_  = (PTSelixNode_)pNode->pData;

    lixTcpRecv( pSeli, fd, buf, &len );

    //
    //  假定每次获取的数据都是一段完整的，非严格意义上的字节流解析
    //

    if ( state_ == SOCK5_STATUS_METHOD_NEGOTIATING )
    {
        if ( 2 != len && (BYTE)buf[0] != SS5_PROXY_VERSION )
        {
            pSelixNode_->m_nodes[fd].bException = TRUE;
            pContext->eventHandler( pSeli, fd, liEvClose, 0, pContext->context );
        }
        else
        {
            // 如果是不用验证
            if ( SS5_PROXY_AUTH_METHOD_NO_AUTH == (BYTE)buf[1] )
            {
                _GetSock5CnnDestQueryBuf( data, pContext );
                len = data.GetDataLength();
                lixTcpSend( pSeli, fd, (char *)data.GetData(), &len );
                pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_CONNECTING_DEST;
            }
            // 如果是用户名/密码验证
            else if ( SS5_PROXY_AUTH_METHOD_USER_PWD == (BYTE)buf[1] )
            {
                _GetSock5UserPwdQueryBuf( data, pContext );
                len = data.GetDataLength();
                lixTcpSend( pSeli, fd, (char *)data.GetData(), &len );
                pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_USER_PWD_AUTHENING;

            }
            // 不支持的验证方法
            else
            {
                pSelixNode_->m_nodes[fd].bException = TRUE;
                pContext->eventHandler( pSeli, fd, liEvClose, 0, pContext->context );
            }
        }
    }
    else if ( state_ == SOCK5_STATUS_USER_PWD_AUTHENING )
    {
        if ( 2 != len && (BYTE)buf[0] != '\x01' )
        {
            pSelixNode_->m_nodes[fd].bException = TRUE;
            pContext->eventHandler( pSeli, fd, liEvClose, 0, pContext->context );
        }
        else
        {
            // 验证失败
            if ( buf[1] != '\x00' )
            {
                pSelixNode_->m_nodes[fd].bException = TRUE;
                pContext->eventHandler( pSeli, fd, liEvClose, 0, pContext->context );
            }
            else
            {
                _GetSock5CnnDestQueryBuf( data, pContext );
                len = data.GetDataLength();
                lixTcpSend( pSeli, fd, (char *)data.GetData(), &len );
                pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_CONNECTING_DEST;
            }
        }
    }
    else if ( state_ == SOCK5_STATUS_CONNECTING_DEST )
    {
        if ( 10 != len && (BYTE)buf[0] != SS5_PROXY_VERSION && '\x00' != (BYTE)buf[1] )
        {
            pSelixNode_->m_nodes[fd].bException = TRUE;
            pContext->eventHandler( pSeli, fd, liEvClose, 0, pContext->context );
        }
        // proxy 连接成功
        else
        {
            pContext->reserverd0[0] = (BYTE)SOCK5_STATUS_OK;
            pSelixNode_->m_nodes[fd].liState = liStAvailable;

            pContext->eventHandler( pSeli, fd, liEvConnect, 0, pContext->context );
        }
    }
}

static void _Sock5CallBackX( SelixNode pSeli, FdType fd, int liEvent, int error, void * context )
{
    PTCnnSock5ParamX  pContext = (PTCnnSock5ParamX)context;
    PListNode pNode            = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_  = (PTSelixNode_)pNode->pData;

    switch ( liEvent )
    {
    case liEvClose:
        pContext->eventHandler( pSeli, fd, liEvClose, 0, pContext->context );
        break;

    case liEvConnect:
        pSelixNode_->m_nodes[fd].liState = liStConnecting;
        _OnSock5ConnectedX( pContext, pSeli, fd );
        break;

    case liEvRead:
        _OnSock5Read( pContext, pSeli, fd );
        break;

    default:
        break;
    }
}







COMMONLMN_API  int  selixInit( DWORD dwTotalMaxSocketCnt )
{
    if ( 0 == dwTotalMaxSocketCnt )
    {
        return  LMN_ERR_INVALID_ARGUMENT;
    }

    // 如果已经初始化了
    if ( s_SelixManager.m_bInited )
    {
        s_SelixManager.m_nInitCnt++;
        return LMN_ERR_OK;
    }

#if WIN32
    WSADATA  wsaData;
    if( WSAStartup( 0x0202, &wsaData ) != 0 )
    {
        return LMN_ERR_UNKNOWN;
    }
#endif

    // 初始化锁
    LmnInitLock( &s_SelixManager.m_Lock );

    // 初始化node列表
    s_SelixManager.m_lstSelixNodes = InitList( );

    // 如果内存不够
    if ( 0 == s_SelixManager.m_lstSelixNodes )
    {
        LmnDeinitLock( &s_SelixManager.m_Lock );
        return  LMN_ERR_NO_MEMORY;
    }

    // 记录manager里所有最大的socket记录
    s_SelixManager.m_dwTotalLimit = dwTotalMaxSocketCnt;

    // 记录已经初始化
    s_SelixManager.m_bInited = TRUE;

    return LMN_ERR_OK;
}


COMMONLMN_API  int  selixEnd( )
{
    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_OK;
    }

    s_SelixManager.m_nInitCnt--;
    if ( s_SelixManager.m_nInitCnt > 0 )
    {
        return LMN_ERR_OK;
    }

    LmnCommon::CFuncLock cManagerLock( &s_SelixManager.m_Lock );

    assert( s_SelixManager.m_lstSelixNodes );

    PListNode pNode_ = GetListHead( s_SelixManager.m_lstSelixNodes );
    while( pNode_ )
    {
        PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode_->pData;
        assert( pSelixNode_ && pSelixNode_->m_nodes );
        DWORD i;
        for ( i = 0; i < pSelixNode_->m_dwNodesSize; i++ )
        {
            if ( pSelixNode_->m_nodes[i].valid )
            {
                lixClose( (SelixNode)pNode_, (FdType)i );
            }
        }
        g_pfnFreeMem( pSelixNode_->m_nodes );
        g_pfnFreeMem( pSelixNode_ );

        pNode_ = GetNextListNode( pNode_ );
    }

    DeinitList( s_SelixManager.m_lstSelixNodes );
    s_SelixManager.m_lstSelixNodes = 0;

    s_SelixManager.m_dwSocketMaxId    = 0;
    s_SelixManager.m_dwSelixNodeMaxId = 0;
    s_SelixManager.m_dwTotalLimit     = 0;
    LmnDeinitLock( &s_SelixManager.m_Lock );

    s_SelixManager.m_bInited = FALSE;

    return LMN_ERR_OK;
}

COMMONLMN_API  BOOL  isSelixInited( )
{
	return s_SelixManager.m_bInited;
}

COMMONLMN_API  SelixNode  selixCreateNode( INOUT DWORD * pdwMaxSocketCnt /*= 0*/ )
{
    DWORD  dwMaxSocketCnt = 0;
    if ( 0 != pdwMaxSocketCnt )
    {
        dwMaxSocketCnt = *pdwMaxSocketCnt;
    }

    // 如果dwMaxSocketcnt为0，设置为最大值
    if ( 0 == dwMaxSocketCnt )
    {
        dwMaxSocketCnt = (DWORD)-1;
    }
    

    // 如果没有初始化
    if ( !s_SelixManager.m_bInited )
    {
        return 0;
    }

    LmnCommon::CFuncLock cManagerLock( &s_SelixManager.m_Lock );

    DWORD dwAllocatedCnt = _GetManagerAllocatedSocketCount();
    // 如果达到最大数，无可用资源
    if ( dwAllocatedCnt >= s_SelixManager.m_dwTotalLimit )
    {
        return 0;
    }

    // 取的可用的socket个数
    dwMaxSocketCnt = MIN( dwMaxSocketCnt, s_SelixManager.m_dwTotalLimit - dwAllocatedCnt );

    PTSelixNode_  pSelixNode = (PTSelixNode_)g_pfnAllocMem( sizeof(TSelixNode_) );
    // 没有内存了
    if ( 0 == pSelixNode )
    {
        return 0;
    }
    memset( pSelixNode, 0, sizeof(TSelixNode_) );


    pSelixNode->m_dwNodesSize = dwMaxSocketCnt;
    pSelixNode->m_nodes       = (seliNode *)g_pfnAllocMem( sizeof(seliNode) * dwMaxSocketCnt );
    // 没有内存
    if ( 0 == pSelixNode->m_nodes )
    {
        g_pfnFreeMem( pSelixNode );
        return 0;
    }
    memset( pSelixNode->m_nodes, 0, sizeof(seliNode) * dwMaxSocketCnt );
   

    assert( s_SelixManager.m_lstSelixNodes );
    PListNode pNode = Insert2ListTail( s_SelixManager.m_lstSelixNodes, (void *)pSelixNode );
    // 内存分配失败
    if ( 0 == pNode )
    {
        g_pfnFreeMem( pSelixNode->m_nodes );
        g_pfnFreeMem( pSelixNode );
        return 0;
    }

    // 分配编号
    s_SelixManager.m_dwSelixNodeMaxId++;
    pSelixNode->m_dwId = s_SelixManager.m_dwSelixNodeMaxId;

    return  pNode;
}


COMMONLMN_API  int  selixDestroyNode( SelixNode pSeli )
{
    if ( 0 == pSeli )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return LMN_ERR_INVALID_HANDLE;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;

    assert( pSelixNode_ && pSelixNode_->m_nodes );
    DWORD i;
    for ( i = 0; i < pSelixNode_->m_dwNodesSize; i++ )
    {
        if ( pSelixNode_->m_nodes[i].valid )
        {
            lixClose( pSeli, (FdType)i );
        }
    }
    g_pfnFreeMem( pSelixNode_->m_nodes );
    g_pfnFreeMem( pSelixNode_ );

    EraseList( s_SelixManager.m_lstSelixNodes,  pNode );

    return LMN_ERR_OK;
}


COMMONLMN_API  int  selixSelect( SelixNode pSeli, DWORD * pdwMs )
{
    struct timeval   tv;
    struct timeval * pTv = 0;

    int nMaxfd = -1;
    int nRet   = LMN_ERR_OK;
    int nIndex = 0;
    int nNum   = 0;

    fd_set tmprdSet;
    fd_set tmpwrSet;
    fd_set tmpexSet;

    
    if ( 0 == pSeli || 0 == pdwMs )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    // 没有初始化
    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    LmnCommon::CFuncLock   cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return LMN_ERR_INVALID_HANDLE;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;
    assert( pSelixNode_ && pSelixNode_->m_dwNodesSize > 0 );

    // 复制pSelixNode_
    TSelixNode_   tCopySelixNode;
    tCopySelixNode.m_dwNodesSize = pSelixNode_->m_dwNodesSize;
    tCopySelixNode.m_dwId        = pSelixNode_->m_dwId;
    tCopySelixNode.m_nodes       = (seliNode *)g_pfnAllocMem( sizeof(seliNode) * pSelixNode_->m_dwNodesSize );
    if ( 0 == tCopySelixNode.m_nodes )
    {
        return LMN_ERR_NO_MEMORY;
    }
    memcpy( tCopySelixNode.m_nodes,  pSelixNode_->m_nodes, sizeof(seliNode) * pSelixNode_->m_dwNodesSize );

    cManagerLock.UnLock();



    DWORD dwBeginTick  =  LmnGetTickCount();


    FD_ZERO(&tmprdSet);
    FD_ZERO(&tmpwrSet);
    FD_ZERO(&tmpexSet);


    for ( nIndex = 0; nIndex < (int)tCopySelixNode.m_dwNodesSize; nIndex++ )
    {
        // 有效且无异常
        if ( tCopySelixNode.m_nodes[nIndex].valid && !tCopySelixNode.m_nodes[nIndex].bException )
        {
            if ( _seliSelectAdd2fdSet( &tmprdSet, &tmpwrSet, &tmpexSet, tCopySelixNode.m_nodes[nIndex].fd, tCopySelixNode.m_nodes[nIndex].seliEvent ) )
            {
                if ( nMaxfd < tCopySelixNode.m_nodes[nIndex].fd )
                {
                    nMaxfd = tCopySelixNode.m_nodes[nIndex].fd;
                }
            }
        }
    }

    // socket set都是空的
    if ( nMaxfd < 0 )
    {        
        _CalculateTimeElapsed( pdwMs, dwBeginTick );
        g_pfnFreeMem( tCopySelixNode.m_nodes );
        return LMN_ERR_OK;
    }

    nMaxfd++;
    // printf("nMaxfd = %d\n", nMaxfd );


    tv.tv_sec  =  *pdwMs / 1000;
    tv.tv_usec = ( *pdwMs % 1000 ) * 1000;
    pTv        = &tv;

    nNum = select( nMaxfd, &tmprdSet, &tmpwrSet, &tmpexSet, pTv );
    if( nNum < 0 )
    {
        // error
        _CalculateTimeElapsed( pdwMs, dwBeginTick );
        nRet = LMN_ERR_SELECT;
        g_pfnFreeMem( tCopySelixNode.m_nodes );

        printf("== error!\n");
        return nRet;
    }
    else if ( 0 == nNum )
    {
        // ok
        //_CalculateTimeElapsed( pdwMs, dwBeginTick );
        //nRet = LMN_ERR_OK;
        //g_pfnFreeMem( tCopySelixNode.m_nodes );

        //printf("== return! \n");
        // return nRet;
    }


    // nNum > 0 

    cManagerLock.Lock();

    // 如果环境已经改变
    if ( !(s_SelixManager.m_bInited && _IfExistSeliNode(pSeli) && pSelixNode_->m_dwId == tCopySelixNode.m_dwId) )
    {
        _CalculateTimeElapsed( pdwMs, dwBeginTick );
        g_pfnFreeMem( tCopySelixNode.m_nodes );
        return LMN_ERR_OK;
    }



    int   fd        = INVALID_SOCKET;
    DWORD dwCurTick = LmnGetTickCount();

    for ( nIndex = 0; nIndex < (int)tCopySelixNode.m_dwNodesSize; nIndex++ )
    {
        // 肯定没有数据
        if ( !tCopySelixNode.m_nodes[nIndex].valid || tCopySelixNode.m_nodes[nIndex].bException )
        {
            continue;
        }

        // 如果原socket改变
        if (   !pSelixNode_->m_nodes[nIndex].valid || pSelixNode_->m_nodes[nIndex].bException 
             || pSelixNode_->m_nodes[nIndex].dwId != tCopySelixNode.m_nodes[nIndex].dwId )
        {
            continue;
        }

        fd = tCopySelixNode.m_nodes[nIndex].fd;
        assert( fd != INVALID_SOCKET );

        int liEvent = 0;
        int nError = 0;

        if ( 0 == nNum )
        {
            if ( tCopySelixNode.m_nodes[nIndex].liState == liStConnecting )
            {
                if ( dwCurTick - tCopySelixNode.m_nodes[nIndex].dwTick > tCopySelixNode.m_nodes[nIndex].dwTimeOut )
                {
                    // printf("tCopySelixNode.m_nodes[nIndex].dwTimeOut = %u \n", tCopySelixNode.m_nodes[nIndex].dwTimeOut );

                    // 连接超时也认为异常
                    tCopySelixNode.m_nodes[nIndex].bException = TRUE;
                    if ( tCopySelixNode.m_nodes[nIndex].callback_ex != 0  ) 
                    {
                        tCopySelixNode.m_nodes[nIndex].callback_ex( pSeli, nIndex, liEvClose, 0, tCopySelixNode.m_nodes[nIndex].context );
                    }
                }
            }
        }
        // 会出现在读set和写set有相同的socket的情况吗？
        else if ( FD_ISSET(fd, &tmprdSet) )
        {
            int nReadBytes  = 0;
            _liBytesAvailable( fd, &nReadBytes );

            if ( ( tCopySelixNode.m_nodes[nIndex].liEvent & liEvRead ) && (nReadBytes > 0) )
            {
                liEvent = liEvRead;
            }
            else if ( ( tCopySelixNode.m_nodes[nIndex].liEvent & liEvClose) && (nReadBytes <= 0) ) 
            {
                liEvent = liEvClose;
                tCopySelixNode.m_nodes[nIndex].seliEvent &= ~seliEvRead;

                // linux下可能出现read和write集合同时存在
                if ( FD_ISSET(fd, &tmpwrSet) )
                {
                    nNum--;
                }
            }
            else if ( tCopySelixNode.m_nodes[nIndex].liEvent & liEvAccept )             
            {
                liEvent = liEvAccept;
            }

            nNum--;
        }
        else if ( FD_ISSET(fd, &tmpwrSet) )
        {
            if ( tCopySelixNode.m_nodes[nIndex].liEvent & liEvConnect )
            {
                liEvent = liEvConnect;

                // 下次select时候不再关注连接
                tCopySelixNode.m_nodes[nIndex].seliEvent = seliEvRead;
                tCopySelixNode.m_nodes[nIndex].liEvent   = liEvRead | liEvClose;
                assert( tCopySelixNode.m_nodes[nIndex].liState  == liStConnecting );
                tCopySelixNode.m_nodes[nIndex].liState   = liStAvailable;
            }
            else if ( tCopySelixNode.m_nodes[nIndex].liEvent & liEvWrite )            
            {
                liEvent = liEvWrite;
            }

            nNum--;
        }
        else if ( FD_ISSET(fd, &tmpexSet) )
        {
            liEvent = liEvClose;
            nError  = -1;
            tCopySelixNode.m_nodes[nIndex].bException = TRUE;
            nNum--;
        }
        //else
        //{
        //    if ( tCopySelixNode.m_nodes[nIndex].liState == liStConnecting )
        //    {
        //        if ( dwCurTick - tCopySelixNode.m_nodes[nIndex].dwTick > tCopySelixNode.m_nodes[nIndex].dwTimeOut )
        //        {
        //            // 连接超时也认为异常
        //            tCopySelixNode.m_nodes[nIndex].bException = TRUE;
        //            if ( tCopySelixNode.m_nodes[nIndex].callback_ex != 0  ) 
        //            {
        //                tCopySelixNode.m_nodes[nIndex].callback_ex( pSeli, nIndex, liEvClose, 0, tCopySelixNode.m_nodes[nIndex].context );
        //            }
        //        }
        //    }
        //}

        if ( liEvent != 0 && tCopySelixNode.m_nodes[nIndex].callback_ex != 0  ) 
        {
            tCopySelixNode.m_nodes[nIndex].callback_ex( pSeli, nIndex, liEvent, nError, tCopySelixNode.m_nodes[nIndex].context );
        }

        pSelixNode_->m_nodes[nIndex].bException = tCopySelixNode.m_nodes[nIndex].bException;
        pSelixNode_->m_nodes[nIndex].liEvent    = tCopySelixNode.m_nodes[nIndex].liEvent;
        pSelixNode_->m_nodes[nIndex].liState    = tCopySelixNode.m_nodes[nIndex].liState;
        pSelixNode_->m_nodes[nIndex].seliEvent  = tCopySelixNode.m_nodes[nIndex].seliEvent;

        if ( 0 == nNum )
        {
            break;
        }
    }

    assert ( 0 == nNum );

    _CalculateTimeElapsed( pdwMs, dwBeginTick );

    g_pfnFreeMem( tCopySelixNode.m_nodes );
    return nRet;
}



// 返回值： socket
COMMONLMN_API  FdType lixOpen ( SelixNode pSeli, liProtocol_t protocol, DWORD ipAddr, WORD wPort, 
                                LPLIXEVENTHANDLER eventHandler, void* context, 
                                const TKeepAliveParam * pKeepAliveParam )
{
    int s,nIndex;
    struct sockaddr_in sin; 
    BOOL on = TRUE;
    struct linger Linger;
    int yes = TRUE;


    if ( 0 == pSeli || protocol > LI_PROTOTYPE_TCP || 0 == eventHandler )
    {
        return INVALID_SOCKET;
    }


    
    if ( !s_SelixManager.m_bInited )
    {
        PERROR("not inited! \n");
        return INVALID_SOCKET;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return INVALID_SOCKET;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;



    Linger.l_onoff  = TRUE; 
    Linger.l_linger = 0;   


    /* allocate a socket */
    if ( (s = socket (AF_INET, s_liProtData [protocol].protType, s_liProtData [protocol].protNum)) <  0 )  
    {
        PERROR("failed to socket! \n");
        return INVALID_SOCKET;
    }

    // set option
    if ( LI_PROTOTYPE_TCP == protocol )
    {
        // setsockopt(s, SOL_SOCKET,  SO_LINGER,    (char *)&Linger, sizeof(Linger));
        // setsockopt(s, IPPROTO_TCP, TCP_NODELAY,  (char *)&yes,    sizeof(yes));

        if ( pKeepAliveParam && pKeepAliveParam->bTcpKeepAlive )
        {
            if ( setsockopt(s, SOL_SOCKET,  SO_KEEPALIVE, (char *)&yes, sizeof(yes)) < 0  )
            {
                PERROR( "fail to set SO_KEEPALIVE! \n" );
            }

            TKeepAliveParam  tTmp;
            memcpy( &tTmp, pKeepAliveParam, sizeof(TKeepAliveParam) );

#ifndef WIN32
            if ( 0 == tTmp.dwCount )
            {
                tTmp.dwCount = DEFAULT_TCP_KEEPALIVE_PROBE_COUNT;
            }
#endif

            if ( 0 == tTmp.dwIdleTime )
            {
                tTmp.dwIdleTime = DEFAULT_TCP_KEEPALIVE_IDLE_TIME;
            }

            if ( 0 == tTmp.dwIntervalTimve )
            {
                tTmp.dwIntervalTimve = DEFAULT_TCP_KEEPALIVE_INTERVAL;
            }


#ifdef WIN32
            tcp_keepalive live, liveout;     
            live.keepaliveinterval = tTmp.dwIntervalTimve * 1000;      
            live.keepalivetime     = tTmp.dwIdleTime * 1000;
            live.onoff             = TRUE;     

            DWORD dwTmp;

            if ( 0 != WSAIoctl( s, SIO_KEEPALIVE_VALS, &live, sizeof(live), &liveout, sizeof(liveout), &dwTmp, NULL,NULL ) )
            {
                PERROR("failed to WASIOCTL! \n");
            }
#else
            int keepidle  = tTmp.dwIdleTime;                       // 开始探测前的空闲等待时间
            int keepintvl = tTmp.dwIntervalTimve;                  // 发送探测分节的时间间隔
            int keepcnt   = tTmp.dwCount;                          // 发送探测分节的次数

            if ( setsockopt( s, SOL_TCP, TCP_KEEPIDLE, (void *)&keepidle, sizeof(keepidle) ) < 0 )
            {
                PERROR( "fail to set TCP_KEEPIDLE! %d \n", ERRNOGET );
            }

            if ( setsockopt( s, SOL_TCP, TCP_KEEPINTVL, (void *)&keepintvl, sizeof (keepintvl) ) < 0 )
            {
                PERROR( "fail to set TCP_KEEPINTVL! %d \n", ERRNOGET );
            }

            if ( setsockopt( s, SOL_TCP, TCP_KEEPCNT, (void *)&keepcnt, sizeof (keepcnt) ) < 0 )
            {
                PERROR( "fail to set TCP_KEEPCNT! %d \n", ERRNOGET );
            }
#endif   
        } // keep alive

    }

    // NON-BLOCK socket
    if ( ioctl(s,FIONBIO,(ioctlOnPtrTypeCast)&on) < 0 ) 
    {
        PERROR("failed to unblock the socket \n");
        close(s);
        return INVALID_SOCKET;
    }

    // reuse address
    //int nCmd = 1;
    //setsockopt (s, SOL_SOCKET, SO_REUSEADDR, (char *)&nCmd, sizeof(int));

    // bind
    if ( wPort > 0 )
    {
        memset(&sin, 0, sizeof(sin));
        sin.sin_family      = AF_INET;
        sin.sin_port        = htons(wPort);
        sin.sin_addr.s_addr = ipAddr;

        if ( bind (s, (SOCKADDRPTR)&sin ,sizeof (sin)) < 0 ) 
        {
            PERROR("failed to bind the socket(), port = %lu! \n", (DWORD)wPort);
            close(s);
            return INVALID_SOCKET;
        }
    }


    

    nIndex = _GetIdleNodeIndex( pSelixNode_ );

    // 如果没有空间了
    if ( nIndex < 0 )
    {
        PERROR("NO socket array space! \n");
        close( s );
        return INVALID_SOCKET;
    }

    pSelixNode_->m_nodes[nIndex].valid          = TRUE;
    pSelixNode_->m_nodes[nIndex].inuse          = FALSE;
    pSelixNode_->m_nodes[nIndex].fd             = s;
    pSelixNode_->m_nodes[nIndex].callback       = 0;
    pSelixNode_->m_nodes[nIndex].callback_ex    = eventHandler;
    pSelixNode_->m_nodes[nIndex].context        = context;
    pSelixNode_->m_nodes[nIndex].seliEvent      = 0;
    pSelixNode_->m_nodes[nIndex].liEvent        = 0;
    pSelixNode_->m_nodes[nIndex].liState        = liStInited;
    pSelixNode_->m_nodes[nIndex].bException     = FALSE;
    pSelixNode_->m_nodes[nIndex].dwTick         = 0;
    pSelixNode_->m_nodes[nIndex].dwTimeOut      = 0;

    // 分配编号
    s_SelixManager.m_dwSelixNodeMaxId++;
    pSelixNode_->m_nodes[nIndex].dwId = s_SelixManager.m_dwSelixNodeMaxId;


    if ( LI_PROTOTYPE_UDP == protocol )
    {
        pSelixNode_->m_nodes[nIndex].seliEvent = seliEvRead | seliEvWrite;
        pSelixNode_->m_nodes[nIndex].liEvent   =  liEvRead | liEvWrite ;
        pSelixNode_->m_nodes[nIndex].liState   = liStAvailable;

        pSelixNode_->m_nodes[nIndex].dwInnerType = SELI_INNER_TYPE_UDP;
    }
    else
    {
        pSelixNode_->m_nodes[nIndex].dwInnerType = SELI_INNER_TYPE_TCP;
    }

    // 返回索引值
    return nIndex;
}





COMMONLMN_API  int  lixClose ( SelixNode pSeli, FdType fd )
{
    // BOOL on = FALSE;
    int rc;

    if ( 0 == pSeli )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return LMN_ERR_INVALID_HANDLE;
    }
    
    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;


    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }

    
    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return LMN_ERR_NOT_SELI_SOCKET;
    }


    //rc = ioctl( pSelixNode_->m_nodes[fd].fd, FIONBIO, (ioctlOnPtrTypeCast)&on );
    //if ( 0 != rc )
    //{
    //    return LMN_ERR_IOCTL;
    //}

    rc = close( pSelixNode_->m_nodes[fd].fd );
    if ( 0 != rc )
    {
        PERROR("failed to close socket! \n");
        return LMN_ERR_CLOSE;
    }

    if ( pSelixNode_->m_nodes[fd].callback_ex )
    {
        pSelixNode_->m_nodes[fd].callback_ex( pSeli, fd, liEvDestroy, 0, pSelixNode_->m_nodes[fd].context );
    }


    pSelixNode_->m_nodes[fd].valid      = FALSE;
    pSelixNode_->m_nodes[fd].inuse      = FALSE;
    pSelixNode_->m_nodes[fd].callback   = 0;
    pSelixNode_->m_nodes[fd].callback_ex = 0;
    pSelixNode_->m_nodes[fd].liEvent    = 0;
    pSelixNode_->m_nodes[fd].seliEvent  = 0;
    pSelixNode_->m_nodes[fd].fd         = INVALID_SOCKET;
    pSelixNode_->m_nodes[fd].liState    = liStInited;
    pSelixNode_->m_nodes[fd].bException = FALSE;
    pSelixNode_->m_nodes[fd].dwTick     = 0;
    pSelixNode_->m_nodes[fd].dwTimeOut  = 0;

    if ( pSelixNode_->m_nodes[fd].context )
    {
        if ( pSelixNode_->m_nodes[fd].dwInnerType & SELI_INNER_TYPE_SOCK5 )
        {
            g_pfnFreeMem( pSelixNode_->m_nodes[fd].context );
        }
    }
    pSelixNode_->m_nodes[fd].context       = 0;
    pSelixNode_->m_nodes[fd].dwInnerType   = 0;
    pSelixNode_->m_nodes[fd].dwId          = 0;

    return LMN_ERR_OK;
}

COMMONLMN_API  int  lixConnect ( SelixNode pSeli,  int fd, DWORD ipAddr, WORD wPort, DWORD dwTimeOut )
{
    struct sockaddr_in sin;
    int status;

    if ( 0 == pSeli )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return LMN_ERR_INVALID_HANDLE;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;



    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }


    // 最短timeout 1秒
    if ( dwTimeOut < 1  )
    {
        dwTimeOut = 1;
    }
    // 最长timeout 20秒
    else if ( dwTimeOut > 20 )
    {
        dwTimeOut = 20;
    }


    
    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return LMN_ERR_NOT_SELI_SOCKET;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons( wPort );
    sin.sin_addr.s_addr = ipAddr;

    if ( ( status = connect ( pSelixNode_->m_nodes[fd].fd, (SOCKADDRPTR) &sin ,sizeof(sin)) ) < 0 )
    {
#ifdef WIN32
        if ( (ERRNOGET == EINPROGRESS) || (ERRNOGET == WSAEWOULDBLOCK) )  /* connection in progress */
#else
        if (ERRNOGET == EINPROGRESS)                                      /* connection in progress */
#endif
        {
            // OK
        }
        else
        {
            return LMN_ERR_CONNECT;
        }
    }

#ifdef WIN32
    pSelixNode_->m_nodes[fd].seliEvent = seliEvWrite| seliEvExept;
#else
    pSelixNode_->m_nodes[fd].seliEvent = seliEvRead | seliEvWrite ;
#endif

    pSelixNode_->m_nodes[fd].liEvent   = liEvConnect | liEvClose ;
    pSelixNode_->m_nodes[fd].liState   = liStConnecting;
    pSelixNode_->m_nodes[fd].dwTick    = LmnGetTickCount();
    pSelixNode_->m_nodes[fd].dwTimeOut = dwTimeOut * 1000;

    return LMN_ERR_OK;
}

COMMONLMN_API  int lixListen( SelixNode pSeli, int fd, int queueLen )
{
    if ( 0 == pSeli )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return LMN_ERR_INVALID_HANDLE;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;

    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }


    
    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return LMN_ERR_NOT_SELI_SOCKET;
    }

    if ( (listen ( pSelixNode_->m_nodes[fd].fd, queueLen) ) < 0 ) 
    {
        return LMN_ERR_LISTEN;
    }

    pSelixNode_->m_nodes[fd].seliEvent = seliEvRead ;
    pSelixNode_->m_nodes[fd].liEvent   = liEvAccept;
    pSelixNode_->m_nodes[fd].liState   = liStListening;

    return LMN_ERR_OK;
}


COMMONLMN_API  FdType lixAccept( SelixNode pSeli, int fd, LPLIXEVENTHANDLER fnCallBack, void * context)
{
    struct sockaddr_in fsin;  /* the request from addr. */
    int socketId, nAcceptIndex = -1;

    if ( 0 == fnCallBack )
    {
        return -1;
    }

    if ( 0 == pSeli )
    {
        return -1;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return -1;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return -1;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;

    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return -1;
    }

    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return -1;
    }

    nAcceptIndex = _GetIdleNodeIndex( pSelixNode_ );
    //if ( nAcceptIndex < 0 )
    //{
    //    return -1;
    //}

    sockaddr_namelen alen = (sockaddr_namelen)sizeof(fsin);
    socketId = accept( pSelixNode_->m_nodes[fd].fd,(SOCKADDRPTR) &fsin,&alen);

    if ( socketId >= 0 )
    {
        if ( nAcceptIndex < 0 )
        {
            close( socketId );
            return -1;
        }
        else
        {
            //struct linger Linger;

            //Linger.l_onoff = TRUE;
            //Linger.l_linger = 0;
            //setsockopt(socketId,SOL_SOCKET, SO_LINGER, (char *)&Linger, sizeof(Linger));

            pSelixNode_->m_nodes[nAcceptIndex].valid      = TRUE;
            pSelixNode_->m_nodes[nAcceptIndex].inuse      = FALSE;
            pSelixNode_->m_nodes[nAcceptIndex].fd         = socketId;
            pSelixNode_->m_nodes[nAcceptIndex].callback   = 0;
            pSelixNode_->m_nodes[nAcceptIndex].callback_ex  = fnCallBack;
            pSelixNode_->m_nodes[nAcceptIndex].context    = context;
            pSelixNode_->m_nodes[nAcceptIndex].seliEvent  = seliEvRead;
            pSelixNode_->m_nodes[nAcceptIndex].liEvent    =  liEvRead | liEvClose ;
            pSelixNode_->m_nodes[nAcceptIndex].liState    = liStAvailable;
            pSelixNode_->m_nodes[nAcceptIndex].bException = FALSE;
            pSelixNode_->m_nodes[nAcceptIndex].dwTick     = 0;
            pSelixNode_->m_nodes[nAcceptIndex].dwTimeOut  = 0;

            _liUnblock(socketId);

            return nAcceptIndex;
        }
    }
    else
    {
        return -1;
    }
}

COMMONLMN_API  int lixShutdown( SelixNode pSeli, int fd )
{
    int rc;

    if ( 0 == pSeli )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return LMN_ERR_INVALID_HANDLE;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;

    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }

    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return LMN_ERR_NOT_SELI_SOCKET;
    }

    rc = shutdown( pSelixNode_->m_nodes[fd].fd, 1);
    if ( 0 != rc )
    {
        return LMN_ERR_SHUTDOWN;
    }

    pSelixNode_->m_nodes[fd].seliEvent &= ~seliEvRead;

    pSelixNode_->m_nodes[fd].liState = liStShutdowned;

    return LMN_ERR_OK;
}

COMMONLMN_API  int lixTcpSend ( SelixNode pSeli, int fd, const char *buff, int * plen )
{
    int ret;

    if ( (0 == plen) || (*plen < 0) || (*plen > 0 && buff == 0) )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return LMN_ERR_INVALID_HANDLE;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;


    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }

    
    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return LMN_ERR_NOT_SELI_SOCKET;
    }

    ret = send( pSelixNode_->m_nodes[fd].fd, buff, *plen, 0 );
    if ( ret < 0 && ERRNOGET == EWOULDBLOCK )
    {
        ret = 0;
    }

    if ( ret >= 0 )
    {
        *plen = ret;
        return LMN_ERR_OK;
    }
    else
    {
        return LMN_ERR_SEND;
    }
}

COMMONLMN_API  int lixTcpCompleteSend ( SelixNode pSeli, int fd, const char *buff, int * plen, DWORD dwSleepTime )
{
    int ret;

    if ( (0 == plen) || (*plen < 0) || (*plen > 0 && buff == 0) )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return LMN_ERR_INVALID_HANDLE;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;


    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }


    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return LMN_ERR_NOT_SELI_SOCKET;
    }

    int  nLeft        = *plen;
    const char * pBuf = buff;

    // ret = send( pSelixNode_->m_nodes[fd].fd, buff, *plen, 0 );
    ret = send( pSelixNode_->m_nodes[fd].fd, pBuf, nLeft, 0 );
    if ( ret < 0 )
    {
        if ( ERRNOGET == EWOULDBLOCK )
        {
            ret = 0;
        }
        else
        {
            *plen -= nLeft;
            return LMN_ERR_SEND;
        }
    }
    else
    {
        pBuf  += ret;
        nLeft -= ret;
    }

    // 如果没有发送完，继续发送
    while( ret >=0 && nLeft > 0 )
    {
        if ( dwSleepTime > 0 )
        {
            LmnSleep( dwSleepTime );
        }

        ret = send( pSelixNode_->m_nodes[fd].fd, pBuf, nLeft, 0 );

        if ( ret < 0 )
        {
            if ( ERRNOGET == EWOULDBLOCK )
            {
                ret = 0;
            }
            else
            {
                *plen -= nLeft;
                return LMN_ERR_SEND;
            }
        }
        else
        {
            pBuf  += ret;
            nLeft -= ret;
        }
    }

    return LMN_ERR_OK;
}


COMMONLMN_API  int lixTcpRecv ( SelixNode pSeli, int fd, char *buff,int * plen)
{
    if ( 0 == plen || *plen <= 0 || 0 == buff )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return LMN_ERR_NOT_INITED;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return LMN_ERR_INVALID_HANDLE;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;


    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return LMN_ERR_INVALID_FILE_DESCRIPTION;
    }

    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return LMN_ERR_NOT_SELI_SOCKET;
    }

    int ret, bytes;

    _liBytesAvailable ( pSelixNode_->m_nodes[fd].fd, &bytes );
    if ( bytes < 0 )
    {
        return LMN_ERR_UNKNOWN;
    }
    else if ( bytes == 0 )
    {
        *plen = 0;
        return 0;
    }

    ret = recv ( pSelixNode_->m_nodes[fd].fd, buff, *plen, 0 );
    if ((ret < 0 ) && (ERRNOGET == EWOULDBLOCK))
    {
        ret=0;
    }

    *plen = ret;

    if ( ret >= 0 )
    {
        return LMN_ERR_OK;
    }
    else
    {
        return LMN_ERR_RECV;
    }
}

COMMONLMN_API  int lixGetState ( SelixNode pSeli, int fd)
{
    if ( !s_SelixManager.m_bInited )
    {
        return -1;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return -1;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;


    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return -1;
    }

    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return -1;
    }

    return pSelixNode_->m_nodes[fd].liState;
}

COMMONLMN_API  int lixGetSockName( SelixNode pSeli, int fd, DWORD * pdwIp, WORD * pwPort )
{
    if ( !s_SelixManager.m_bInited )
    {
        return -1;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return -1;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;

    if ( fd < 0 || fd >= (int)pSelixNode_->m_dwNodesSize )
    {
        return -1;
    }

    if ( !pSelixNode_->m_nodes[fd].valid )
    {
        return -1;
    }

    struct sockaddr_in addr;
#ifdef WIN32
    int len = sizeof(struct sockaddr_in);
#else
    socklen_t len = sizeof(struct sockaddr_in);
#endif // WIN32


    if ( 0 != getsockname( pSelixNode_->m_nodes[fd].fd, (struct sockaddr *)&addr, &len ) )
    {
        return -1;
    }

    if ( pwPort )
    {
        *pwPort = addr.sin_port;
    }

    if ( pdwIp )
    {
        *pdwIp = addr.sin_addr.s_addr;
    }

    return 0;
}

COMMONLMN_API  FdType  lixConnectSock5 ( SelixNode pSeli, const TCnnSock5ParamX *  ptParam )
{
    if ( 0 == ptParam )
    {
        return INVALID_SOCKET;
    }

    if ( !s_SelixManager.m_bInited )
    {
        return INVALID_SOCKET;
    }

    LmnCommon::CFuncLock  cManagerLock( &s_SelixManager.m_Lock );

    // 句柄无效
    if ( !_IfExistSeliNode(pSeli) )
    {
        return INVALID_SOCKET;
    }

    PListNode pNode           = (PListNode)pSeli;
    PTSelixNode_  pSelixNode_ = (PTSelixNode_)pNode->pData;


    // 如果使用sock5
    if ( ptParam->bUseSock5 )
    {
        if ( 0 == ptParam->dwSock5Addr || 0 == ptParam->wSock5Port || 0 == ptParam->eventHandler )
        {
            return INVALID_SOCKET;
        }

        if ( SOCK5_METHOD_USER == ptParam->eMethod && '\0' == ptParam->szUserName[0] )
        {
            return INVALID_SOCKET;
        }

        PTCnnSock5ParamX  pContext = (PTCnnSock5ParamX)g_pfnAllocMem( sizeof(TCnnSock5ParamX) );
        if ( 0 == pContext )
        {
            return INVALID_SOCKET;
        }

        memcpy( pContext, ptParam, sizeof(TCnnSock5ParamX) );

        FdType fd = lixOpen ( pSeli, LI_PROTOTYPE_TCP, 0, 0, _Sock5CallBackX, pContext, 0 );
        if ( fd == INVALID_SOCKET )
        {
            g_pfnFreeMem( pContext );
            return INVALID_SOCKET;
        }

        pSelixNode_->m_nodes[fd].dwInnerType |= SELI_INNER_TYPE_SOCK5;

        if ( 0 != lixConnect( pSeli, fd, ptParam->dwSock5Addr, ptParam->wSock5Port, ptParam->dwTimeOut ) )
        {
            lixClose( pSeli, fd );
            g_pfnFreeMem( pContext );
            return INVALID_SOCKET;
        }

        return fd;   
    }
    // 如果不使用sock5
    else
    {
        FdType fd = lixOpen ( pSeli, LI_PROTOTYPE_TCP, 0, 0, ptParam->eventHandler, ptParam->context, 0 );
        if ( fd == INVALID_SOCKET )
        {
            return INVALID_SOCKET;
        }

        if ( 0 != lixConnect( pSeli, fd, ptParam->dwDestAddr, ptParam->wDestPort, ptParam->dwTimeOut ) )
        {
            liClose( fd );
            return INVALID_SOCKET;
        }

        return fd;   
    }
}

