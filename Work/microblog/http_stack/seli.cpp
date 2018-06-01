#ifdef _IOS_PLATFORM_
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif
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


#include "seli.h"


#ifdef WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif


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
// static  LmnLockType    g_Lock;
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



  int  seliInit( DWORD dwMaxSocketCnt )
{
    if ( 0 == dwMaxSocketCnt || dwMaxSocketCnt > FD_SETSIZE )
    {
        return -1;
    }

    if ( s_bInited )
    {
        return -1;
    }

#if WIN32
    WSADATA  wsaData;
    if( WSAStartup( 0x0202, &wsaData ) != 0 )
    {
        return -1;
    }
#endif

    // LmnInitLock( &g_Lock );

    s_nodes = (seliNode *)g_pfnAllocMem( sizeof(seliNode) * dwMaxSocketCnt );
    if ( 0 == s_nodes )
    {
        return -1;
    }
    memset( s_nodes, 0, sizeof(seliNode) * dwMaxSocketCnt );
    s_max_fd_cnt = dwMaxSocketCnt;

    for ( int i = 0; i < s_max_fd_cnt; i++ )
    {
        s_nodes[i].fd = INVALID_SOCKET;
    }

    s_bInited = TRUE;

    return 0;
}

  BOOL IsSeliInited()
  {
      return s_bInited;
  }


  int  seliEnd( )
{
    if ( !s_bInited )
    {
        return 0;
    }

    assert( s_nodes );

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
    // LmnDeinitLock( &g_Lock );

    s_bInited = FALSE;

    return 0;
}


// select多路复用
// nMs单位， 毫秒 
int  seliSelect( DWORD * pdwMs )
{
    struct timeval  tv;
    struct timeval * pTv = 0;

    int nMaxfd = -1;
    int nRet   = 0;
    int nIndex = 0;
    int nNum   = 0;

    fd_set tmprdSet;
    fd_set tmpwrSet;
    fd_set tmpexSet;


    if ( !s_bInited )
    {
        return -1;
    }

    // 20 ~ 1000 毫秒
    if ( 0 == pdwMs || *pdwMs > 1000 || *pdwMs < 20 )
    {
        return -1;
    }

    DWORD dwBeginTick  =  LmnGetTickCount();
    DWORD dwEndTick = 0;


    FD_ZERO(&tmprdSet);
    FD_ZERO(&tmpwrSet);
    FD_ZERO(&tmpexSet);

    LmnLock( &g_Lock );

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

        LmnUnlock( &g_Lock );

        MYTRACE("=================\n");
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
        return 0;
    }

    LmnUnlock( &g_Lock );

    nMaxfd++;



    tv.tv_sec  =  *pdwMs / 1000;
    tv.tv_usec = ( *pdwMs % 1000 ) * 1000;
    pTv        = &tv;

    nNum = select( nMaxfd, &tmprdSet, &tmpwrSet, &tmpexSet, pTv );

    LmnLock( &g_Lock );

    if( nNum < 0 )
    {
        // error
        nRet = -1;
    }
    else if( nNum > 0 )
    {
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

            // 会出现在读set和写set有相同的socket的情况吗？
            if ( FD_ISSET(fd, &tmprdSet) )
            {
                int nReadBytes  = 0;
                _liBytesAvailable( fd, &nReadBytes );

                if ( (s_nodes[nIndex].liEvent & liEvRead) && (nReadBytes > 0) )
                {
                    liEvent = liEvRead;
                }
                else if ( (s_nodes[nIndex].liEvent & liEvClose) && (nReadBytes <= 0) ) 
                {
                    MYTRACE("====== close on read event ! nReadBytes = %d  \n", nReadBytes );
                    liEvent = liEvClose;

                    // 对端关闭，下次select时候不再关注读数据
                    s_nodes[nIndex].seliEvent &= ~seliEvRead;

                    // linux下可能出现read和write集合同时存在
                    if ( FD_ISSET(fd, &tmpwrSet) )
                    {
                        nNum--;
                    }
                }
                else if ( s_nodes[nIndex].liEvent & liEvAccept )             
                {
                    MYTRACE("====== accept ! nReadBytes = %d  \n", nReadBytes );
                    liEvent = liEvAccept;
                }

                nNum--;
            }
            else if ( FD_ISSET(fd, &tmpwrSet) )
            {
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

                nNum--;
            }
            else if ( FD_ISSET(fd, &tmpexSet) )
            {
                MYTRACE("====== exception! ( will close)  \n");
                liEvent = liEvClose;
                nError  = -1;

                s_nodes[nIndex].bException = TRUE;

                nNum--;
            }

            if ( liEvent != 0 && s_nodes[nIndex].callback != 0  ) 
            {
                s_nodes[nIndex].callback( nIndex, liEvent, nError, s_nodes[nIndex].context );
            }

            if ( 0 == nNum )
            {
                break;
            }
        }

        assert ( 0 == nNum );
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

    LmnUnlock( &g_Lock );

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





#define  TCP_KEEPALIVE_IDLE_TIME       60                 // 单位：秒
#define  TCP_KEEPALIVE_INTERVAL        60                 // 单位：秒
#define  TCP_KEEPALIVE_PROBE_COUNT     3



/**************************************** li *******************************************************/


  FdType liOpen ( liProtocol_t protocol, DWORD ipAddr, WORD wPort, LPLIEVENTHANDLER eventHandler, void* context,
                              BOOL bTcpKeepAlive )
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

        if ( bTcpKeepAlive )
        {
            if ( setsockopt(s, SOL_SOCKET,  SO_KEEPALIVE, (char *)&yes, sizeof(yes)) < 0  )
            {
                PERROR( "fail to set SO_KEEPALIVE! \n" );
            }

#ifdef WIN32
            tcp_keepalive live, liveout;     
            live.keepaliveinterval = TCP_KEEPALIVE_INTERVAL * 1000;      
            live.keepalivetime     = TCP_KEEPALIVE_IDLE_TIME * 1000;
            live.onoff             = TRUE;     

            DWORD dwTmp;

            if ( 0 != WSAIoctl( s, SIO_KEEPALIVE_VALS, &live, sizeof(live), &liveout, sizeof(liveout), &dwTmp, NULL,NULL ) )
            {
                PERROR("failed to WASIOCTL! \n");
            }
#else
#ifndef _IOS_PLATFORM_
            int keepidle  = TCP_KEEPALIVE_IDLE_TIME;               // 开始探测前的空闲等待时间
            int keepintvl = TCP_KEEPALIVE_INTERVAL;                // 发送探测分节的时间间隔
            int keepcnt   = TCP_KEEPALIVE_PROBE_COUNT;             // 发送探测分节的次数

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
            PERROR("failed to bind the socket()! \n");
            close(s);
            return INVALID_SOCKET;
        }
    }


    LmnLock( &g_Lock );
    nIndex = _GetIdleNodeIndex(  );

    // 如果没有空间了
    if ( nIndex < 0 )
    {
        PERROR("NO socket array space! \n");
        close( s );
        LmnUnlock( &g_Lock );
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

    LmnUnlock( &g_Lock );

    // 返回索引值
    return nIndex;
}

// 参数fd实际是静态数据的索引
  int liClose ( FdType fd )
{
    BOOL on = FALSE;
    int rc;

    if ( !s_bInited )
    {
        return -1;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return -1;
    }

    LmnLock( &g_Lock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &g_Lock );
        return -1;
    }


    rc = ioctl( s_nodes[fd].fd, FIONBIO, (ioctlOnPtrTypeCast)&on );
    if ( 0 != rc )
    {
        LmnUnlock( &g_Lock );
        return -1;
    }

    rc = close( s_nodes[fd].fd );
    if ( 0 != rc )
    {
        PERROR("failed to close socket! \n");
        LmnUnlock( &g_Lock );
        return -1;
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

    LmnUnlock( &g_Lock );
    return 0;
}

// dwTimeOut 单位:秒
// 参数fd实际是静态数据的索引
  int  liConnect ( int fd, DWORD ipAddr, WORD wPort, DWORD dwTimeOut )
{
    struct sockaddr_in sin;
    int status;

    if ( !s_bInited )
    {
        return -1;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return -1;
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


    LmnLock( &g_Lock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &g_Lock );
        return -1;
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
            LmnUnlock( &g_Lock );
            return -1;
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

    LmnUnlock( &g_Lock );
    return 0;
}



  int liListen(int fd, int queueLen )
{
    if ( !s_bInited )
    {
        return -1;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return -1;
    }


    LmnLock( &g_Lock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &g_Lock );
        return -1;
    }

    if ( (listen ( s_nodes[fd].fd, queueLen) ) < 0 ) 
    {
        LmnUnlock( &g_Lock );
        return -1;
    }

    s_nodes[fd].seliEvent = seliEvRead ;
    s_nodes[fd].liEvent   = liEvAccept;

    s_nodes[fd].liState   = liStListening;

    LmnUnlock( &g_Lock );
    return 0;
}


  FdType liAccept(int fd, LPLIEVENTHANDLER fnCallBack, void * context)
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

    LmnLock( &g_Lock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &g_Lock );
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
            LmnUnlock( &g_Lock );
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

    LmnUnlock( &g_Lock );
    return nAcceptIndex;
}


  int liShutdown( int fd )
{
    int rc;

    if ( !s_bInited )
    {
        return -1;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return -1;
    }

    LmnLock( &g_Lock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &g_Lock );
        return -1;
    }

    rc = shutdown( s_nodes[fd].fd, 1);
    if ( 0 != rc )
    {
        LmnUnlock( &g_Lock );
        return -1;
    }

    s_nodes[fd].seliEvent &= ~seliEvRead;

    s_nodes[fd].liState = liStShutdowned;

    LmnUnlock( &g_Lock );
    return 0;
}


  int liTcpSend ( int fd, const char *buff, int * plen )
{
    int ret;

    if ( (0 == plen) || (*plen < 0) || (*plen > 0 && buff == 0) )
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

    LmnLock( &g_Lock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &g_Lock );
        return -1;
    }

    int nSendCnt = 0;
    int nLeft    = *plen;


    ret = send( s_nodes[fd].fd, buff + nSendCnt, nLeft, 0 );
    if ( ret < 0 && ERRNOGET == EWOULDBLOCK )
    {
        ret = 0;
    }

    if ( ret > 0 )
    {
        nSendCnt += ret;
        nLeft    -= ret;
    }

    while( ret >= 0 && nLeft > 0 )
    {
        if ( 0 == ret )
        {
            SLEEP( 100 );
        }

        ret = send( s_nodes[fd].fd, buff + nSendCnt, nLeft, 0 );
        if ( ret < 0 && ERRNOGET == EWOULDBLOCK )
        {
            ret = 0;
        }

        if ( ret > 0 )
        {
            nSendCnt += ret;
            nLeft    -= ret;
        }
    }
    
    if ( ret >= 0 )
    {
        *plen = nSendCnt;
        LmnUnlock( &g_Lock );
        return 0;
    }
    else
    {
        LmnUnlock( &g_Lock );
        return -1;
    }
}


  int liTcpRecv (int fd, char *buff,int * plen)
{
    if ( !s_bInited )
    {
        return -1;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt || 0 == plen || *plen <= 0 || 0 == buff )
    {
        return -1;
    }

    LmnLock( &g_Lock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &g_Lock );
        return -1;
    }

    int ret, bytes;

    _liBytesAvailable ( s_nodes[fd].fd, &bytes );
    if ( bytes < 0 )
    {
        LmnUnlock( &g_Lock );
        return -1;
    }
    else if ( bytes == 0 )
    {
        *plen = 0;
        LmnUnlock( &g_Lock );
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
        LmnUnlock( &g_Lock );
        return 0;
    }
    else
    {
        LmnUnlock( &g_Lock );
        return -1;
    }
}


// 返回值：
//          -1 ERROR
  int liGetState (int fd)
{
    if ( !s_bInited )
    {
        return -1;
    }

    if ( fd < 0 || fd >= s_max_fd_cnt )
    {
        return -1;
    }

    LmnLock( &g_Lock );
    if ( !s_nodes[fd].valid )
    {
        LmnUnlock( &g_Lock );
        return -1;
    }

    int nTmpState = s_nodes[fd].liState;

    LmnUnlock( &g_Lock );
    return nTmpState;
}

/**************************************** END li ***********************************************/





/*********************** li extension *************************/

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

static void  _GetSock5CnnDestQueryBuf( CDataBuf & data, const TCnnSock5Param * pContext )
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

static void _GetSock5UserPwdQueryBuf( CDataBuf & data, const TCnnSock5Param * pContext )
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
    CDataBuf  data;

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
    CDataBuf data;
    char buf[NORMAL_BUF_SIZE];
    int  len = NORMAL_BUF_SIZE;

    Sock5Status state_ = (Sock5Status)pContext->reserverd0[0];

    if ( state_ == SOCK5_STATUS_OK )
    {
        pContext->eventHandler( fd, liEvRead, 0, pContext->context );
        return;
    }

    liTcpRecv( fd, buf, &len );

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
        _OnSock5Connected( pContext, fd );
        s_nodes[fd].liState = liStConnecting;
        break;

    case liEvRead:
        _OnSock5Read( pContext, fd );
        break;

    default:
        break;
    }
}



// 返回值： 0  OK
  FdType  lxConnectSock5 ( const TCnnSock5Param *  ptParam )
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

    CFuncLock  cLock( &g_Lock );

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

        FdType fd = liOpen ( LI_PROTOTYPE_TCP, 0, 0, _Sock5CallBack, pContext, FALSE );
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
        FdType fd = liOpen ( LI_PROTOTYPE_TCP, 0, 0, ptParam->eventHandler, ptParam->context, FALSE );
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


/*********************** end li extension *************************/
















