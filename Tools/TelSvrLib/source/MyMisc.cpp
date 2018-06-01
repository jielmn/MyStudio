#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#ifdef WIN32
#include <Winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/times.h>
#include <semaphore.h>
#endif
#include "MyMisc.h"
#include "MyContainer.h"

#define   _JDEBUG

#define IGNORED_DIMENSION                  (0.000001f)
#define IS_FLOAT_ZEOR( n )                 ( (n) >= -IGNORED_DIMENSION && (n) <= IGNORED_DIMENSION )
#define IS_FLOAT_EQUAL( a, b )             IS_FLOAT_ZEOR( a - b )

typedef struct tagLine
{
	Point p1;
	Point p2;
}Line, * PLine;

int IsPointInPoint( const PPoint p1, const PPoint p2 )
{
	float diff_x = p1->x - p2->x;
	if ( !(diff_x >= -IGNORED_DIMENSION && diff_x <= IGNORED_DIMENSION) )
	{
		return 0;
	}

	float diff_y = p1->y - p2->y;
	if ( !(diff_y >= -IGNORED_DIMENSION && diff_y <= IGNORED_DIMENSION) )
	{
		return 0;
	}

	return 1;
}

// return value:
// 1 : in the line section
// -1: int the line but not in the line section
// 0 : not in the line
int IsPointInLine( const PPoint pt, const PLine pLine )
{
	if ( IsPointInPoint( pt, &pLine->p1 ) || IsPointInPoint( pt, &pLine->p2 ) )
	{
		return 1;
	}

	float diff_x = pLine->p1.x - pLine->p2.x;

	// if is vertical line
	if ( diff_x >= -IGNORED_DIMENSION && diff_x <= IGNORED_DIMENSION )
	{
		diff_x = pt->x - pLine->p1.x;
		// not in the line
		if ( !(diff_x >= -IGNORED_DIMENSION && diff_x <= IGNORED_DIMENSION) )
		{
			return 0;
		}

		// in the line
		float min_y = 0.0f;
		float max_y = 0.0f;

		if ( pLine->p1.y < pLine->p2.y )
		{
			min_y = pLine->p1.y;
			max_y = pLine->p2.y;
		}
		else
		{
			min_y = pLine->p2.y;
			max_y = pLine->p1.y;
		}

		if ( pt->y >= min_y && pt->y <= max_y )
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		float lineSlope = ( pLine->p2.y - pLine->p1.y ) / ( pLine->p2.x - pLine->p1.x );
		float cmpSlope  = ( pt->y       - pLine->p1.y ) / ( pt->x       - pLine->p1.x );

		float diff = lineSlope - cmpSlope;
		if ( !IS_FLOAT_ZEOR( diff ) )
		{
			return -1;
		}

		// in the line
		float min_y = 0.0f;
		float max_y = 0.0f;

		if ( pLine->p1.y < pLine->p2.y )
		{
			min_y = pLine->p1.y;
			max_y = pLine->p2.y;
		}
		else
		{
			min_y = pLine->p2.y;
			max_y = pLine->p1.y;
		}

		if ( pt->y >= min_y && pt->y <= max_y )
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
}

int IsHorizontalLine( const PLine pLine )
{
	return IS_FLOAT_EQUAL( pLine->p1.y, pLine->p2.y );
}

int IsPointInPolygon( const PPoint pt, const Point polygon[], int nNum )
{
	int nCount = 0;

	int i = 0;
	for ( i = 0; i < nNum; i++ )
	{
		Line line;
		line.p1 = polygon[i];
		line.p2 = polygon[(i+1)%nNum];

		int nIsInLine = IsPointInLine( pt, &line);
		// in the line section
		if ( 1 == nIsInLine )
		{
			return 1;
		}

		// if is horizontal line
		if ( IS_FLOAT_EQUAL( line.p1.y, line.p2.y ) )
		{
			continue;
		}

		if ( IS_FLOAT_EQUAL ( line.p1.y, pt->y ) && line.p1.x < pt->x )
		{
			if ( line.p1.y > line.p2.y )
			{
				nCount++;
			}
		}
		else if ( IS_FLOAT_EQUAL ( line.p2.y, pt->y ) && line.p2.x < pt->x )
		{
			if ( line.p2.y > line.p1.y )
			{
				nCount++;
			}
		}
		else
		{
			float min_y = 0.0f;
			float max_y = 0.0f;

			if ( line.p1.y < line.p2.y )
			{
				min_y = line.p1.y;
				max_y = line.p2.y;
			}
			else
			{
				min_y = line.p2.y;
				max_y = line.p1.y;
			}

			if ( !(pt->y >= min_y && pt->y <= max_y) )
			{
				continue;
			}

			Point intersectPoint;
			intersectPoint.x = 0.0f;
			intersectPoint.y = pt->y;

			// if is vertical line
			if ( IS_FLOAT_EQUAL( line.p1.x, line.p2.x) )
			{
				intersectPoint.x = line.p1.x;
			}
			else
			{
				float k = ( line.p2.y - line.p1.y ) / ( line.p2.x - line.p1.x );
				intersectPoint.x = line.p1.x + ( intersectPoint.y - line.p1.y ) / k;
			}

			if ( intersectPoint.x <= pt->x )
			{
				nCount++;
			}
		}
	}

	return ( nCount % 2 == 1 ? 1 : 0 );
}


static size_t  _jsnprintf( char *buffer, size_t count, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
#ifdef WIN32
	int n = _vsnprintf(buffer, count, format, ap);
#else
    int n = vsnprintf(buffer, count, format, ap);
#endif
	va_end(ap);

	if ( n < 0 )
	{
		return count;
	}
	else
	{
		return n;
	}
}

/************************************************************************/
/*  调试字节流用的                                                      */
/*  最终buff结果：左边为二进制代码，右边为字符                          */
/************************************************************************/
char* DebugStream( char* buff, unsigned int bufLen, const char* stream,
				   unsigned int streamLen)
{
    if ( 0 == buff )
    {
        return 0;
    }

	memset(buff, 0, bufLen);

	if ( NULL == stream )
	{
		return buff;
	}
	
	unsigned int i, j, k;
	unsigned char s[17] = {0};

	//j: current position
	//buffLen - 1: keep a position for the terminate character(null)
	for ( i = 0, j = 0; i < streamLen && j < bufLen - 1; i++)
	{
		j += _jsnprintf( buff + j, (bufLen - 1) - j, "%02x ", (unsigned char)stream[i] );
		//ascii between 32 and 127 is visible
		if ( stream[i] >=32 )
		{
			s[i % 16] = stream[i];
		}
		else
		{
			s[i % 16] = '.';
		}
		
		if ( 0 == (i + 1) % 16 )
		{
			j += _jsnprintf( buff + j, (bufLen - 1) - j, " %s\r\n", s ); 
		}
	}

	if ( i < streamLen )
	{
		return buff;
	}
	else if ( j < bufLen - 1 )
	{
		if ( 0 != i % 16 )
		{		
			for ( k = 0; k < 16 - (i % 16); k++ )
			{
				j += _jsnprintf( buff + j, (bufLen - 1) - j, "   ");
				s[(i+k) % 16] = ' ';
			}

			j += _jsnprintf( buff + j, (bufLen - 1) - j, " %s\r\n", s ); 
		}
	}

	return buff;
}


#ifdef WIN32

////////////////////////找到文件名称/////////////////////
const char *  GetFileTitle( const char* szFileName )
{
	if ( NULL == szFileName )
	{
		return NULL;
	}
	
	const char * p = strrchr(szFileName, '\\');
	if ( NULL == p )
	{
		return szFileName;
	}
	else
	{
		return p + 1;
	}
}

/////////////////获取文件的路径///////////////////////////////////////
int GetFileDirectory( const char * szFileName, 
					  char * szFileDirectory, unsigned int nDirLen )
{
	if ( NULL == szFileName || NULL == szFileDirectory || 0 == nDirLen )
	{
		return -2;
	}
	
	const char * p = strrchr(szFileName, '\\');
	if ( NULL == p )
	{
		szFileDirectory[0] = '\0';
		return 0;
	}
	else
	{
		unsigned int nCopy = p - szFileName;
		if ( nDirLen < nCopy + 1 )
		{
			strncpy( szFileDirectory, szFileName, nDirLen - 1 );
			szFileDirectory[nDirLen - 1] = '\0';
			return -1;
		}
		else
		{
			strncpy( szFileDirectory, szFileName, nCopy );
			szFileDirectory[nCopy] = '\0';
			return nCopy;
		}
	}
}

#endif

int GetRand( int nMin, int nMax )
{
    if ( nMin > nMax )
    {
        int nTemp = nMax;
        nMax = nMin;
        nMin = nTemp;
    }

    int nLength = nMax - nMin + 1;

    int nRet = nMin + rand() % nLength;

    return nRet;
}

#ifdef WIN32
static DWORD WINAPI _WinThrdRuntine( LPVOID lpThreadParameter )
{
    void ** ppArgs    = (void **)lpThreadParameter;
    JThreadFunc pFunc = (JThreadFunc)ppArgs[0];
    void * pParam     = ppArgs[1];

    void * ret = pFunc( pParam );
    delete[] ppArgs;

    return (DWORD)ret;
}
#endif

JThrdHandle JCreateThread( JThreadFunc pFunc, void * pParam,
                           unsigned int nStackSize )
{
#ifdef WIN32
    if ( 0 == pFunc )
    {
        return 0;
    }

    void ** ppArgs = new void *[2];
    ppArgs[0] = pFunc;
    ppArgs[1] = pParam;
    JThrdHandle hThrd = CreateThread( 0, nStackSize, 
                        _WinThrdRuntine, ppArgs, 0, 0);
    if ( 0 == hThrd )
    {
        delete[] ppArgs;
    }
    return hThrd;

#else
    if ( 0 == pFunc )
    {
        return 0;
    }

    JThrdHandle hThrd;
    int nRet = 0;

    if ( nStackSize > 0 )
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize( &attr, nStackSize );

        nRet = pthread_create( &hThrd, &attr, pFunc, pParam );

        pthread_attr_destroy(&attr);
    }
    else
    {
        nRet = pthread_create( &hThrd, 0, pFunc, pParam );
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

JThrdRet JWait4Thrd( JThrdHandle hThrd )
{
#ifdef WIN32
    if ( 0 == hThrd )
    {
        return (void *)-1;
    }

    WaitForSingleObject( hThrd, INFINITE );
    DWORD dwExit;
    if ( GetExitCodeThread( hThrd, &dwExit ) )
    {
        return (void *)dwExit;
    }
    else
    {
        return (void *)-1;
    }
#else
    if ( 0 == hThrd )
    {
        return (void *)-1;
    }

    JThrdRet ret;
    if ( 0 == pthread_join( hThrd, &ret ) )
    {
        return ret;
    }
    else
    {
        return (void *)-1;
    }
#endif
}

void JInitLock  ( JLockType * pLock )
{
    assert( pLock );
#ifdef WIN32
    InitializeCriticalSection( pLock );
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );

    pthread_mutex_init( pLock, &attr );

    pthread_mutexattr_destroy( &attr );
#endif
}

void JDeinitLock( JLockType * pLock )
{
    assert( pLock );
#ifdef WIN32
    DeleteCriticalSection( pLock );
#else
    pthread_mutex_destroy( pLock );
#endif
}

void JLock( JLockType * pLock )
{
    assert( pLock );
#ifdef WIN32
    EnterCriticalSection( pLock );
#else
    pthread_mutex_lock( pLock );
#endif
}

void JUnlock( JLockType * pLock )
{
    assert( pLock );
#ifdef WIN32
    LeaveCriticalSection( pLock );
#else
    pthread_mutex_unlock( pLock );
#endif
}

CJFuncLock::CJFuncLock( JLockType & lock ) : m_lock( lock )
{
    JLock( &m_lock );
}

CJFuncLock::~CJFuncLock()
{
    JUnlock( &m_lock );
}


/***************************************************************/
/* JSocket                                                     */
/***************************************************************/

////////////////////// NOTE ////////////////////////////
//连接以后服务器先发送一个是否可以连接的消息
// 4字节: 0不可以，1可以

// 0: 尽快检查是否断链
// 1: 每隔_check_time时间检查是否断链
#define  CHECK_DISCNN_MODE            0

static JLockType * s_pJSocketGeneralLock   = 0;
static int         s_nDisconnectNotifyFlag = 0;

#define SEND_BUF_SIZE              4096
#define RECV_BUF_SIZE              (SEND_BUF_SIZE * 2)

#define JSOCKET_HEART_BEAT_REQ     1
#define JSOCKET_HEART_BEAT_ACK     2

#define SELECT_WAIT_TIME           1 //seconds
#define BUFFING_TIME               100 //miliseconds

static unsigned int _check_time       = SELECT_WAIT_TIME * 1000;
static unsigned int _check_heart_time = SELECT_WAIT_TIME * 2 * 1000 + BUFFING_TIME;

#ifdef WIN32
#define CLOSE_SOCKET( s )          \
    DO_ONCE( shutdown(s, SD_BOTH); closesocket(s); )
#else
#define CLOSE_SOCKET( s )          \
    DO_ONCE( shutdown(s, SHUT_RDWR); close(s); )
#endif

typedef struct _tagJSocketData
{
    JThrdHandle   hThrd;
    int           nThrdLoop;
    int           nState;
}_JSocketData;

// key: socket, value: _JSocketData * 
static PHashtable  s_Sockets = 0;

static int         s_nInitFlag = 0;
static int         s_nProcFlag = 0;
static JSemHandle  s_hSem = 0;
static PList       s_pList = 0;
static JLockType   s_listLock;
static JThrdHandle s_hThrd = 0;

enum _ProcType
{
    _PROCTYPE_ON_RECEIVE,
    _PROCTYPE_ON_ACCEPT,
    _PROCTYPE_ON_DISCONNECT
};

typedef struct _tagProcSockItem
{
    unsigned int     nType;
    void *           pFunc;
    JSocketType      nSock;
    unsigned int     nMsgId;
    char *           pData;
    unsigned int     nDataLen;
    unsigned int     nDiscReason;

    _tagProcSockItem()
    {
        nType       = 0;
        pFunc       = 0;
        nSock       = 0;
        nMsgId      = 0;
        pData       = 0;
        nDataLen    = 0;
        nDiscReason = 0;
    }
}_procSockItem, *_PprocSockItem;


typedef struct _tagJMsg
{
    unsigned int   nMsgId;
    unsigned int   nDataLen;
}_JMsg;


#ifndef WIN32

static void sigHandle( int sig )
{
    //printf("%d signal caught!\n", sig);
}

#endif

static void * _SocketProcThrd(void * lpParam)
{
    while( 1 )
    {
        JSemTake( s_hSem );
        if ( 0 == s_nProcFlag )
        {
            break;
        }

        JLock( &s_listLock );
        PListNode pNode = GetListHead( s_pList );
        _PprocSockItem pItem = (_PprocSockItem)pNode->pData;
        EraseListHead( s_pList );
        JUnlock( &s_listLock );

        assert( pItem && pItem->pFunc && pItem->nSock && pItem->nType <= _PROCTYPE_ON_DISCONNECT );

        if ( _PROCTYPE_ON_RECEIVE == pItem->nType )
        {
            JSocketOnReceive pfun = (JSocketOnReceive)pItem->pFunc;
            pfun( pItem->nSock, pItem->nMsgId, pItem->pData, pItem->nDataLen );
        }
        else if ( _PROCTYPE_ON_ACCEPT == pItem->nType )
        {
            JSocketOnAccept pfun = (JSocketOnAccept)pItem->pFunc;
            pfun( pItem->nSock );
        }
        else if ( _PROCTYPE_ON_DISCONNECT == pItem->nType )
        {
            JSocketOnDisconnect pfun = (JSocketOnDisconnect)pItem->pFunc;
            pfun( pItem->nSock, pItem->nDiscReason );
        }

        if ( pItem->pData )
        {
            assert( pItem->nDataLen > 0 && pItem->nType == _PROCTYPE_ON_RECEIVE );
            delete[] pItem->pData;
        }

        delete pItem;
    }
    return 0;
}

int JSocketInit()
{
    if ( 0 != s_nInitFlag )
    {
        return 1;
    }

    assert( 0 == s_Sockets );
    s_Sockets = InitHashtable( 128, 0, 0 );

    assert( 0 == s_pJSocketGeneralLock );    
    s_pJSocketGeneralLock = new JLockType;
    JInitLock( s_pJSocketGeneralLock );

    s_nDisconnectNotifyFlag = 1;

    s_nProcFlag = 1;
    s_pList = InitList();
    s_hThrd = JCreateThread( _SocketProcThrd, 0, 0);
    JSemCreate( &s_hSem );
    JInitLock( &s_listLock );

#ifdef WIN32
    WSADATA wsaData;
    WSAStartup( 0x0202, &wsaData );
#else
    struct sigaction act;

    act.sa_handler = sigHandle;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_flags |= SA_RESTART;

    sigaction(SIGUSR1, &act, NULL);
#endif

    s_nInitFlag = 1;
    return 0;
}

static void _sendSimpleMsg( JSocketType s, unsigned int nMsgId )
{
    _JMsg msg;
    msg.nMsgId   = nMsgId;
    msg.nDataLen = 0;

    send( s, (const char *)&msg, sizeof(_JMsg), 0 );
}

void JSocketDeinit()
{
    if ( 0 == s_nInitFlag )
    {
        return;
    }

	s_nProcFlag = 0;
    JSemGive( s_hSem );
    JWait4Thrd( s_hThrd );

    s_nDisconnectNotifyFlag = 0;

    assert( s_pJSocketGeneralLock );
    JLock( s_pJSocketGeneralLock );
    if ( 0 != s_Sockets )
    {
        PHashNode pNode = GetHashtableFirstNode( s_Sockets );
        while( pNode )
        {
            _JSocketData * pSocketData = (_JSocketData *)pNode->pData;
            JSocketType s = (JSocketType)GetHashNodeKey( pNode );

            CLOSE_SOCKET( s );

            pSocketData->nThrdLoop = 0;
#ifndef WIN32
            pthread_kill( pSocketData->hThrd, SIGUSR1 );
#endif
            JWait4Thrd( pSocketData->hThrd );
            delete pSocketData;

            pNode = GetNextHashtableNode( pNode );
        }

        DeinitHashtable( s_Sockets );
        s_Sockets = 0;
    }
    //JUnlock( s_pJSocketGeneralLock );
    JDeinitLock( s_pJSocketGeneralLock );
    delete s_pJSocketGeneralLock;
    s_pJSocketGeneralLock = 0;  

	JLock( &s_listLock );
	PListNode pNode = GetListHead( s_pList );
	while( pNode )
	{
		_PprocSockItem pItem = (_PprocSockItem)pNode->pData;
		delete pItem;
		pNode = GetNextListNode( pNode );
	}
	DeinitList( s_pList );
	s_pList = 0;
    JDeinitLock( &s_listLock );    
    JSemDelete( s_hSem );

#ifdef WIN32
    WSACleanup();
#endif

//     if ( 0 != s_pJSocketGeneralLock )
//     {
//         JDeinitLock( s_pJSocketGeneralLock );
//         delete s_pJSocketGeneralLock;
//         s_pJSocketGeneralLock = 0;
//     }
}

static void _JSocketOnReceiveSysMsg( JSocketType s, unsigned int nMsgId,
                                  void * pData, unsigned int nDataLen )
{
    if ( JSOCKET_HEART_BEAT_REQ == nMsgId )
    {
        _sendSimpleMsg( s, JSOCKET_HEART_BEAT_ACK );
    }
}

static void * _ListenThrd( void * pParams )
{
    int * pArgs = (int *)pParams;
    JSocketType s                = (JSocketType)pArgs[0];
    unsigned int nMaxAccepted    = (unsigned int)pArgs[1];
    JSocketOnAccept onAcceptCb   = (JSocketOnAccept)pArgs[2];
    JSocketOnReceive onReceiveCb = (JSocketOnReceive)pArgs[3];
    JSocketOnDisconnect onDisconnect = (JSocketOnDisconnect)pArgs[4];
    _JSocketData * pSocketData   = (_JSocketData *)pArgs[5];

    fd_set      fds;
    JSocketType maxSock = s;
    JSocketType * pClientSockets = new JSocketType[nMaxAccepted];
    memset( pClientSockets, 0, sizeof(JSocketType) * nMaxAccepted );
    unsigned int * pClientTicks = new unsigned int[nMaxAccepted];
    memset( pClientTicks, 0, sizeof(unsigned int) * nMaxAccepted );

    char ** ppBuff = new char*[nMaxAccepted];
    unsigned int * pBufSize = new unsigned int[nMaxAccepted];

    unsigned int i = 0;
    for ( i = 0; i < nMaxAccepted; i++ )
    {
        ppBuff[i] = new char[RECV_BUF_SIZE];
        pBufSize[i] = 0;
    }

    FD_ZERO( &fds );
    FD_SET( s, &fds );

    unsigned int nAcceptedCnt = 0;
    

    unsigned int last_tick = JGetTickCount();

    while( pSocketData->nThrdLoop )
    {
        fd_set f;
        memcpy( &f, &fds, sizeof(fd_set) );

        struct timeval tv;
        tv.tv_sec  = SELECT_WAIT_TIME;
        tv.tv_usec = 0;
    
        int ret = select( maxSock + 1, &f, NULL, NULL, &tv );
        if (ret < 0)
        {
            break;
        }
        else if ( 0 == ret )
        {
            unsigned int cur_tick = JGetTickCount();
            // 检查所有client socket是否掉线
            if ( cur_tick - last_tick >= _check_time )
            {
                for ( i = 0; i < nMaxAccepted; i++ )
                {
                    if ( 0 != pClientSockets[i] )
                    {
                        //掉线
                        if ( cur_tick - pClientTicks[i] >= _check_heart_time )
                        {
                            CLOSE_SOCKET( pClientSockets[i] );
                            nAcceptedCnt--;
                            
                            if ( onDisconnect )
                            {
                                _PprocSockItem pItem = new _procSockItem;
                                pItem->nType = _PROCTYPE_ON_DISCONNECT;
                                pItem->pFunc = (void *)onDisconnect;
                                pItem->nSock = pClientSockets[i];
                                pItem->nDiscReason = JDCNN_NO_HEART_BEAT;

                                JLock( &s_listLock );
                                Add2ListTail( s_pList, pItem );
                                JUnlock( &s_listLock );

                                JSemGive( s_hSem );
                                //onDisconnect( pClientSockets[i], JDCNN_NO_HEART_BEAT );
                            }

                            FD_CLR( pClientSockets[i], &fds );
                            pClientSockets[i] = 0;
                            pClientTicks[i] = 0;
                        }
                        else
                        {
                            _sendSimpleMsg( pClientSockets[i], JSOCKET_HEART_BEAT_REQ );
                        }
                    }
                }
                last_tick = cur_tick;
            }
            continue;
        }
       
        char buf[SEND_BUF_SIZE];

        // check every fd in the set
        for ( i = 0; i < nMaxAccepted; i++)
        {
            if ( FD_ISSET( pClientSockets[i], &f) )
            {
                ret = recv( pClientSockets[i], buf, SEND_BUF_SIZE, 0);
                // close
                if ( ret <= 0 )
                {
                    CLOSE_SOCKET( pClientSockets[i] );
                    pBufSize[i] = 0;
                    nAcceptedCnt--;

                    if ( s_nDisconnectNotifyFlag && onDisconnect )
                    {
                        _PprocSockItem pItem = new _procSockItem;
                        pItem->nType = _PROCTYPE_ON_DISCONNECT;
                        pItem->pFunc = (void *)onDisconnect;
                        pItem->nSock = pClientSockets[i];
                        pItem->nDiscReason = (0 == ret ? JDCNN_GRACEFULLY_CLOSE : JDCNN_UNGRACEFULLY_CLOSE);

                        JLock( &s_listLock );
                        Add2ListTail( s_pList, pItem );
                        JUnlock( &s_listLock );

                        JSemGive( s_hSem );

                        //onDisconnect( pClientSockets[i], 
                        //              0 == ret ? JDCNN_GRACEFULLY_CLOSE : JDCNN_UNGRACEFULLY_CLOSE );
                    }

                    FD_CLR( pClientSockets[i], &fds );
                    pClientSockets[i] = 0;
                    pClientTicks[i] = 0;
                }
                // receive data
                else
                {
                    _JMsg * pMsg = 0;
                    unsigned int n = 0;
                    int nError = 0;

                    memcpy( ppBuff[i] + pBufSize[i], buf, ret );
                    pBufSize[i] += ret;

                    do 
                    {
                        pMsg = (_JMsg *)(ppBuff[i] + n);

                        //不是一个完整的数据或错误的数据
                        if ( pBufSize[i] - n < sizeof( _JMsg ) )
                        {
                            memmove( ppBuff[i], ppBuff[i] + n, pBufSize[i] - n );
                            break;
                        }
                        //错误包
                        else if ( pMsg->nDataLen > SEND_BUF_SIZE - sizeof( _JMsg ) )
                        {
                            CLOSE_SOCKET( pClientSockets[i] );
                            pBufSize[i] = 0;
                            nAcceptedCnt--;

                            if ( s_nDisconnectNotifyFlag && onDisconnect )
                            {
                                _PprocSockItem pItem = new _procSockItem;
                                pItem->nType = _PROCTYPE_ON_DISCONNECT;
                                pItem->pFunc = (void *)onDisconnect;
                                pItem->nSock = pClientSockets[i];
                                pItem->nDiscReason = JDCNN_ERROR_PACK;

                                JLock( &s_listLock );
                                Add2ListTail( s_pList, pItem );
                                JUnlock( &s_listLock );

                                JSemGive( s_hSem );

                                //onDisconnect( pClientSockets[i], JDCNN_ERROR_PACK );
                            }

                            FD_CLR( pClientSockets[i], &fds );
                            pClientSockets[i] = 0;
                            pClientTicks[i] = 0;
                            nError = 1;
                            break;
                        }
                        //不是一个完整的数据包
                        else if ( pBufSize[i] - n < sizeof( _JMsg ) + pMsg->nDataLen )
                        {
                            memmove( ppBuff[i], ppBuff[i] + n, pBufSize[i] - n );
                            break;
                        }

                        n += sizeof( _JMsg ) + pMsg->nDataLen;
                        assert( n <= pBufSize[i] );

                        if ( pMsg->nMsgId < JSOCKET_USER_MSG )
                        {
                            if ( pMsg->nDataLen > 0 )
                            {
                                _JSocketOnReceiveSysMsg( pClientSockets[i], 
                                    pMsg->nMsgId, (void *)(pMsg + 1), 
                                    pMsg->nDataLen );
                            }
                            else
                            {
                                _JSocketOnReceiveSysMsg( pClientSockets[i], 
                                    pMsg->nMsgId, 0, 0 );
                            }
                        }
                        else
                        {
                            assert( onReceiveCb );

                            _PprocSockItem pItem = new _procSockItem;
                            pItem->nType = _PROCTYPE_ON_RECEIVE;
                            pItem->pFunc = (void *)onReceiveCb;
                            pItem->nSock = pClientSockets[i];
                            pItem->nMsgId = pMsg->nMsgId;

                            if ( pMsg->nDataLen > 0 )
                            {
                                char * pTmp = new char[pMsg->nDataLen];
                                memcpy( pTmp, pMsg + 1, pMsg->nDataLen );
                                pItem->pData = pTmp;
                            }
                            else
                            {
                                pItem->pData = 0;
                            }
                            pItem->nDataLen = pMsg->nDataLen;
               

                            JLock( &s_listLock );
                            Add2ListTail( s_pList, pItem );
                            JUnlock( &s_listLock );

                            JSemGive( s_hSem );

//                             if ( pMsg->nDataLen > 0 )
//                             {
//                                 onReceiveCb( pClientSockets[i], pMsg->nMsgId, 
//                                             (void *)(pMsg + 1), pMsg->nDataLen );
//                             }
//                             else
//                             {
//                                 onReceiveCb( pClientSockets[i], pMsg->nMsgId, 0, 0 );
//                             }
                        }

                    } while ( 1 );

                    if ( 0 == nError )
                    {
                        pBufSize[i] -= n;
                        pClientTicks[i] = JGetTickCount();
                    }
                }
            }
        }

        struct sockaddr_in client_addr;
#ifdef WIN32
        int sin_size;
#else
        socklen_t sin_size;
#endif
        sin_size = sizeof(client_addr);

        if ( FD_ISSET( s, &f ) )
        {
            JSocketType new_fd = accept( s, 
                          (struct sockaddr *)&client_addr, &sin_size );
            // accept error
            if ( -1 == (int)new_fd )
            {
                continue;
            }

            // add to fd queue
            if ( nAcceptedCnt < nMaxAccepted )
            {
                FD_SET( new_fd, &fds );

                for ( i = 0; i < nMaxAccepted; i++ )
                {
                    if ( 0 == pClientSockets[i] )
                    {
                        pClientSockets[i] = new_fd;
                        pClientTicks[i]   = JGetTickCount();
                        break;
                    }
                }
                assert( i < nMaxAccepted );

                if ( new_fd > maxSock )
                {
                    maxSock = new_fd;
                }

                nAcceptedCnt++;

                int nAllow = htonl(1);
                send( new_fd, (const char *)&nAllow, sizeof(int), 0 );

                if ( onAcceptCb )
                {
                    _PprocSockItem pItem = new _procSockItem;
                    pItem->nType = _PROCTYPE_ON_ACCEPT;
                    pItem->pFunc = (void *)onAcceptCb;
                    pItem->nSock = new_fd;
                   
                    JLock( &s_listLock );
                    Add2ListTail( s_pList, pItem );
                    JUnlock( &s_listLock );

                    JSemGive( s_hSem );
                    //onAcceptCb( new_fd );
                }

            }
            // max connections arrive
            else
            {
                int nAllow = htonl(0);
                send( new_fd, (const char *)&nAllow, sizeof(int), 0 );

                CLOSE_SOCKET( new_fd );
            }
        }

        // 检查所有client socket是否掉线
        unsigned int cur_tick = JGetTickCount();
        if ( cur_tick - last_tick >= _check_time )
        {
            for ( i = 0; i < nMaxAccepted; i++ )
            {
                if ( 0 != pClientSockets[i] )
                {
                    //掉线
                    if ( cur_tick - pClientTicks[i] >= _check_heart_time )
                    {
                        CLOSE_SOCKET( pClientSockets[i] );
                        pBufSize[i] = 0;
                        nAcceptedCnt--;

                        if ( onDisconnect )
                        {
                            _PprocSockItem pItem = new _procSockItem;
                            pItem->nType = _PROCTYPE_ON_DISCONNECT;
                            pItem->pFunc = (void *)onDisconnect;
                            pItem->nSock = pClientSockets[i];
                            pItem->nDiscReason = JDCNN_NO_HEART_BEAT;

                            JLock( &s_listLock );
                            Add2ListTail( s_pList, pItem );
                            JUnlock( &s_listLock );

                            JSemGive( s_hSem );
                            //onDisconnect( pClientSockets[i], JDCNN_NO_HEART_BEAT );
                        }

                        FD_CLR( pClientSockets[i], &fds );
                        pClientSockets[i] = 0;
                        pClientTicks[i] = 0;
                    }
                    //发送心跳请求
                    else
                    {
                        _sendSimpleMsg( pClientSockets[i], JSOCKET_HEART_BEAT_REQ );
                    }
                }
            }
            last_tick = cur_tick;
        }
    }

    for ( i = 0; i < nMaxAccepted; i++ )
    {
        if ( 0 != pClientSockets[i] )
        {
            CLOSE_SOCKET( pClientSockets[i] );
            pClientSockets[i] = 0;
            pClientTicks[i] = 0;
        }

        delete[] ppBuff[i];
        pBufSize[i] = 0;
    }

    delete[] ppBuff;
    delete[] pBufSize;
    delete[] pClientSockets;
    delete[] pClientTicks;
    delete[] pArgs;
    return 0;
}

int JSocketListen ( JSocketType * pSocket, unsigned short nPort,
                    unsigned int nMaxAccepted, JSocketOnAccept onAcceptCb,
                    JSocketOnReceive onReceiveCb, JSocketOnDisconnect onDisconnect )
{
    //没有初始化
    if ( 0 == s_nInitFlag )
    {
        return 1;
    }

    if ( 0 == pSocket )
    {
        return 2;
    }

    assert( 0 != s_Sockets );

    if ( 0 == nMaxAccepted )
    {
        nMaxAccepted = 2;
    }

    // sockaddr_in为TCP/IP套接字指定了套接字的地址。
    // 其它的协议都使用相似的结构。
    sockaddr_in local;

    // 现在我们来为sockaddr_in结构赋值。
    local.sin_family      = AF_INET;                 // 地址族
    local.sin_addr.s_addr = INADDR_ANY;              // 网际IP地址
    local.sin_port        = htons( nPort );  // 使用的端口

    // 由socket函数创建我们的SOCKET。
    *pSocket = socket( AF_INET,SOCK_STREAM, 0 );

    // 如果socket()函数失败，我们就退出。
    if( (unsigned int)-1 == *pSocket )
    {
        return 3;
    }

    int yes = 1;
    if ( 0 != setsockopt( *pSocket, SOL_SOCKET, SO_REUSEADDR, 
                          (const char *)&yes, sizeof(int) ) )
    {
        return 4;
    }

    // bind将我们刚创建的套接字和sockaddr_in结构联系起来。
    // 它主要使用本地地址及一个特定的端口来连接套接字。
    // 如果它返回非零值，就表示出现错误。
    if( 0 != bind( *pSocket, (sockaddr*)&local, sizeof(local) ) )
    {
        return 5;
    }

    // listen命令套接字监听来自客户端的连接。
    // 第二个参数是最大连接数。
    if( 0 != listen( *pSocket,10 ) )
    {
        return 6;
    }

    
    
    _JSocketData * pSocketData = new _JSocketData;
    pSocketData->hThrd     = 0;
    pSocketData->nThrdLoop = 1;
    pSocketData->nState    = 0;

    JLock( s_pJSocketGeneralLock );
    Add2Hashtable( s_Sockets, (void *)*pSocket, 
                                     pSocketData );
    JUnlock( s_pJSocketGeneralLock );

    int * pArgs = new int[6];
    pArgs[0] = *pSocket;
    pArgs[1] = nMaxAccepted;
    pArgs[2] = (int)onAcceptCb;
    pArgs[3] = (int)onReceiveCb;
    pArgs[4] = (int)onDisconnect;
    pArgs[5] = (int)pSocketData;
    JThrdHandle hThrd = JCreateThread( _ListenThrd, pArgs, 0 );

    pSocketData->hThrd = hThrd;

    return 0;
}

static void * _ConnectThrd( void * pParams )
{
    int * pArgs = (int *)pParams;

    JSocketType s                = (JSocketType)pArgs[0];
    JSocketOnReceive onReceiveCb = (JSocketOnReceive)pArgs[1];
    JSocketOnDisconnect onDisconnect = (JSocketOnDisconnect)pArgs[2];
    _JSocketData *   pSocketData = (_JSocketData *)pArgs[3];

    char buf[RECV_BUF_SIZE];
    unsigned int nBufSize = 0;

    unsigned int last_tick = JGetTickCount();
    unsigned int sockTick  = last_tick;

    while( pSocketData->nThrdLoop )
    {
        fd_set fds;
        FD_ZERO( &fds );
        FD_SET( s, &fds );

        struct timeval tv;
        tv.tv_sec  = SELECT_WAIT_TIME;
        tv.tv_usec = 0;

        int ret = select( s + 1, &fds, NULL, NULL, &tv );
        if (ret < 0)
        {
            CLOSE_SOCKET( s );
            break;
        }
        else if ( 0 == ret )
        {
            unsigned int cur_tick = JGetTickCount();
            // 检查所有client socket是否掉线
            if ( cur_tick - last_tick >= _check_time )
            {
                //掉线
                if ( cur_tick - sockTick >= _check_heart_time )
                {
                    CLOSE_SOCKET( s );
                
                    if ( onDisconnect )
                    {
                        _PprocSockItem pItem = new _procSockItem;
                        pItem->nType = _PROCTYPE_ON_DISCONNECT;
                        pItem->pFunc = (void *)onDisconnect;
                        pItem->nSock = s;
                        pItem->nDiscReason = JDCNN_NO_HEART_BEAT;

                        JLock( &s_listLock );
                        Add2ListTail( s_pList, pItem );
                        JUnlock( &s_listLock );

                        JSemGive( s_hSem );
                        //onDisconnect( s, JDCNN_NO_HEART_BEAT );
                    }

                    s = 0;
                    sockTick = 0;
                    break;
                }
                else
                {
                    //客户端不请求服务器发送
                    //_sendSimpleMsg( s, JSOCKET_HEART_BEAT_REQ );
                }

                last_tick = cur_tick;
            }
            continue;
        }

        assert( nBufSize <= SEND_BUF_SIZE );
        ret = recv( s, buf + nBufSize, SEND_BUF_SIZE, 0 );
        // close
        if ( ret <= 0 )
        {
            CLOSE_SOCKET( s );

            if ( s_nDisconnectNotifyFlag && onDisconnect )
            {
                _PprocSockItem pItem = new _procSockItem;
                pItem->nType = _PROCTYPE_ON_DISCONNECT;
                pItem->pFunc = (void *)onDisconnect;
                pItem->nSock = s;
                pItem->nDiscReason = (0 == ret ? JDCNN_GRACEFULLY_CLOSE : JDCNN_UNGRACEFULLY_CLOSE);

                JLock( &s_listLock );
                Add2ListTail( s_pList, pItem );
                JUnlock( &s_listLock );

                JSemGive( s_hSem );

                //onDisconnect( s, 0 == ret ? JDCNN_GRACEFULLY_CLOSE : JDCNN_UNGRACEFULLY_CLOSE );
            }

            break;
        }
        // receive data
        else
        {
            _JMsg * pMsg = 0;
            unsigned int n = 0;
            int nError = 0;

            nBufSize += ret;

            do 
            {
                pMsg = (_JMsg *)(buf + n);

                //不是一个完整的数据或错误的数据
                if ( nBufSize - n < sizeof( _JMsg ) )
                {
                    memmove( buf, buf + n, nBufSize - n );
                    break;
                }
                //错误包
                else if ( pMsg->nDataLen > SEND_BUF_SIZE - sizeof( _JMsg ) )
                {
                    CLOSE_SOCKET( s );
                    nBufSize = 0;
                
                    if ( onDisconnect )
                    {
                        _PprocSockItem pItem = new _procSockItem;
                        pItem->nType = _PROCTYPE_ON_DISCONNECT;
                        pItem->pFunc = (void *)onDisconnect;
                        pItem->nSock = s;
                        pItem->nDiscReason = JDCNN_ERROR_PACK ;

                        JLock( &s_listLock );
                        Add2ListTail( s_pList, pItem );
                        JUnlock( &s_listLock );

                        JSemGive( s_hSem );

                        //onDisconnect( s, JDCNN_ERROR_PACK );
                    }

                    s = 0;
                    sockTick = 0;
                    nError = 1;
                    break;
                }
                //不是一个完整的数据包
                else if ( nBufSize - n < sizeof( _JMsg ) + pMsg->nDataLen )
                {
                    memmove( buf, buf + n, nBufSize - n );
                    break;
                }

                n += sizeof( _JMsg ) + pMsg->nDataLen;
                assert( n <= nBufSize );

                if ( pMsg->nMsgId < JSOCKET_USER_MSG )
                {
                    if ( pMsg->nDataLen > 0 )
                    {
                        _JSocketOnReceiveSysMsg( s, pMsg->nMsgId, 
                              (void *)(pMsg + 1), pMsg->nDataLen );
                    }
                    else
                    {
                        _JSocketOnReceiveSysMsg( s, pMsg->nMsgId, 0, 0 );
                    }
                }
                else
                {
                    assert( onReceiveCb );

                    _PprocSockItem pItem = new _procSockItem;
                    pItem->nType = _PROCTYPE_ON_RECEIVE;
                    pItem->pFunc = (void *)onReceiveCb;
                    pItem->nSock = s;
                    pItem->nMsgId = pMsg->nMsgId;

                    if ( pMsg->nDataLen > 0 )
                    {
                        char * pTmp = new char[pMsg->nDataLen];
                        memcpy( pTmp, pMsg + 1, pMsg->nDataLen );
                        pItem->pData = pTmp;
                    }
                    else
                    {
                        pItem->pData = 0;
                    }
                    pItem->nDataLen = pMsg->nDataLen;
       

                    JLock( &s_listLock );
                    Add2ListTail( s_pList, pItem );
                    JUnlock( &s_listLock );

                    JSemGive( s_hSem );

//                     if ( pMsg->nDataLen > 0 )
//                     {
//                         onReceiveCb( s, pMsg->nMsgId, (void *)(pMsg + 1), 
//                                      pMsg->nDataLen );
//                     }
//                     else
//                     {
//                         onReceiveCb( s, pMsg->nMsgId, 0, 0 );
//                     }
                }
                
            } while ( 1 );

            if ( 0 == nError )
            {
                nBufSize -= n;
                sockTick = JGetTickCount();
            }
            else
            {
                break;
            }
        }
    }

    delete[] pArgs;

    return 0;
}

int JSocketConnect( JSocketType * pSocket, unsigned int  nHostIp, 
                    unsigned short nPort, JSocketOnReceive onReceiveCb,
                    JSocketOnDisconnect onDisconnect )
{
    if ( 0 == s_nInitFlag )
    {
        return 1;
    }

    if ( 0 == pSocket )
    {
        return 2;
    }

    assert( 0 != s_Sockets );

    if ( 0 == onReceiveCb )
    {
        return 3;
    }

    *pSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if( (unsigned int)-1 == *pSocket )
    { 
        return 4;
    }

    struct sockaddr_in serverAddress; 
    memset(&serverAddress,0,sizeof(sockaddr_in)); 

    serverAddress.sin_family           = AF_INET; 
#ifdef WIN32
    serverAddress.sin_addr.S_un.S_addr = nHostIp; 
#else
    serverAddress.sin_addr.s_addr = nHostIp;
#endif
    serverAddress.sin_port             = htons(nPort); 

    int ret = connect( *pSocket, (sockaddr*)&serverAddress,
                       sizeof(serverAddress) );
    if( -1 == ret )
    { 
        return 5; 
    }

    int nAllow;
    int nRecvCnt = recv( *pSocket, (char *)&nAllow, sizeof(int), 0 );

    // shutdown
    if ( nRecvCnt <= 0 )
    {
        CLOSE_SOCKET( *pSocket );
        return 6;
    }

    nAllow = ntohl( nAllow );
    // not allowed;
    if ( 0 == nAllow )
    {
        CLOSE_SOCKET( *pSocket );
        return 7;
    }


    _JSocketData * pSocketData = new _JSocketData;
    pSocketData->hThrd     = 0;
    pSocketData->nThrdLoop = 1;
    pSocketData->nState    = 0;

    JLock( s_pJSocketGeneralLock );
    Add2Hashtable( s_Sockets, (void *)*pSocket, 
                                     pSocketData );
    JUnlock( s_pJSocketGeneralLock );

    int * pArgs = new int[4];
    pArgs[0] = *pSocket;
    pArgs[1] = (int)onReceiveCb;
    pArgs[2] = (int)onDisconnect;
    pArgs[3] = (int)pSocketData;

    JThrdHandle hThrd = JCreateThread( _ConnectThrd, pArgs, 0 );

    pSocketData->hThrd = hThrd;
    return 0;
}

int JSocketClose( JSocketType s )
{
    if ( 0 == s_nInitFlag )
    {
        return 1;
    }

    CJFuncLock funcLock( *s_pJSocketGeneralLock );

    CLOSE_SOCKET( s );

    if ( IfHashtableContainKey( s_Sockets, (void *)s ) )
    {
        _JSocketData * pSocketData = (_JSocketData *)GetHashtableValue( s_Sockets, (void *)s );
        pSocketData->nThrdLoop = 0;
#ifndef WIN32
        pthread_kill( pSocketData->hThrd, SIGUSR1 );
#endif
        JWait4Thrd( pSocketData->hThrd );
        delete pSocketData;

        EraseHashtableByKey( s_Sockets, (void *)s ) ;
    }
    return 0;
}

int JSocketSend( JSocketType s, unsigned int nMsgId,
                 void * pData, unsigned int nDataLen )
{
    if ( 0 == s_nInitFlag )
    {
        return 1;
    }

    if ( 0 == s )
    {
        return 2;
    }

    if ( nMsgId < JSOCKET_USER_MSG )
    {
        return 3;
    }

    if ( 0 == pData )
    {
        nDataLen = 0;
    }

    if ( sizeof(_JMsg) + nDataLen > SEND_BUF_SIZE )
    {
        return 4;
    }

    char buf[SEND_BUF_SIZE];
    _JMsg * pMsg = (_JMsg *)buf;
    pMsg->nMsgId = nMsgId;
    pMsg->nDataLen = nDataLen;

    if ( nDataLen > 0 )
    {
        memcpy( buf + sizeof(_JMsg), pData, nDataLen );
    }

    if ( send( s, buf, sizeof(_JMsg) + nDataLen, 0 ) < 0 )
    {
        return 5;
    }

    return 0;
}

#ifndef WIN32
unsigned long _CLK_TCK = sysconf( _SC_CLK_TCK );
unsigned long _MILI_TIMES = 1000 / _CLK_TCK;
#endif

unsigned int JGetTickCount()
{
#ifdef WIN32
    return GetTickCount();
#else
    unsigned int ticks = (unsigned int) times( NULL );
    return ticks * _MILI_TIMES;
#endif
}

void JSleep(unsigned int nMiliSeconds)
{
#ifdef WIN32
    Sleep( nMiliSeconds );
#else
    usleep( nMiliSeconds * 1000 );
#endif
}


int JSemCreate(JSemHandle * pHandle)
{
    if ( 0 == pHandle )
    {
        return 1;
    }
#ifdef WIN32
    HANDLE h = CreateSemaphore( 0, 0, 0x7FFFFFFF, 0);
    if ( 0 == h )
    {
        return 2;
    }
    *pHandle = h;
#else
    sem_t * p = 0;
    p = new sem_t;

    if ( -1 == sem_init( p, 0, 0 ) )
    {
        delete p;
        return 2;
    }
    *pHandle = (JSemHandle)p;
#endif
    return 0;
}

int JSemTake(JSemHandle h)
{
#ifdef WIN32
    if ( 0 == h )
    {
        return 1;
    }
    
    if ( WAIT_FAILED == WaitForSingleObject( h, INFINITE ) )
    {
        return 2;
    }

#else
    if ( 0 == h )
    {
        return 1;
    }

    sem_t * p = (sem_t *)h;
    
    if ( -1 == sem_wait( p ) )
    {
        return 2;
    }
#endif
    return 0;
}

int JSemTakeByTime(JSemHandle h, unsigned int dwtime)
{
    if ( 0 == h )
    {
        return 1;
    }
    
#ifdef WIN32
    DWORD dwRet = WaitForSingleObject( h, dwtime );
    if ( WAIT_OBJECT_0 == dwRet )
    {
        return 0;
    }
    else
    {
        return 2;
    }
#else
    sem_t * p = (sem_t *)h;
    timespec ts;                         
    clock_gettime(CLOCK_REALTIME, &ts );       //获取当前时间
    ts.tv_sec  += ( dwtime / 1000 );           //加上等待时间的秒数
    ts.tv_nsec += ( dwtime % 1000 ) * 1000;    //加上等待时间纳秒数
    return sem_timedwait( p, &ts );
#endif   
}

int JSemGive(JSemHandle h)
{
    if ( 0 == h )
    {
        return 1;
    }
#ifdef WIN32
    if ( ReleaseSemaphore( h, 1, 0 ) )
    {
        return 0;
    }
    else
    {
        return 2;
    }
#else
    sem_t * p = (sem_t *)h;
    return sem_post( p );
#endif
}

int JSemDelete(JSemHandle h)
{
#ifdef WIN32
    if ( 0 == h )
    {
        return 1;
    }
    
    if ( !CloseHandle( h ) )
    {
        return 2;
    }

#else
    if ( 0 == h )
    {
        return 1;
    }

    sem_t * p = (sem_t *)h;

    if ( -1 == sem_destroy( p ) )
    {
        return 2;
    }

    delete p;    
#endif
    return 0;
}
