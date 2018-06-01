

#include "DetectBlockedFunction.h"
#ifdef WIN32
#include <windows.h>
#else
#include <sys/times.h>
#endif

#include <assert.h>
#include <vector>
#include <algorithm>



#ifdef WIN32
#define SNPRINTF         _snprintf
#else
#define SNPRINTF         snprintf
#endif






/*************************    2. 互斥锁   **************************************/
#ifdef WIN32
typedef CRITICAL_SECTION JLockType;
#else
typedef pthread_mutex_t  JLockType;
#endif

class CJFuncLock
{
public:
    CJFuncLock( JLockType & lock );
    ~CJFuncLock();
    
private:
    JLockType & m_lock;
};


static void JInitLock  ( JLockType * pLock )
{
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

static void JDeinitLock( JLockType * pLock )
{
#ifdef WIN32
    DeleteCriticalSection( pLock );
#else
    pthread_mutex_destroy( pLock );
#endif
}

static void JLock( JLockType * pLock )
{
#ifdef WIN32
    EnterCriticalSection( pLock );
#else
    pthread_mutex_lock( pLock );
#endif
}

static void JUnlock( JLockType * pLock )
{
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

/*************************    END 互斥锁   **************************************/





/*************************    1. 线程   **************************************/

#ifdef WIN32
typedef HANDLE          JThrdHandle;
#else
typedef pthread_t       JThrdHandle;
#endif

typedef void * (*JThreadFunc)(void *);


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


static JThrdHandle JCreateThread( JThreadFunc pFunc, void * pParam,
                           unsigned int dwStackSize )
{
#ifdef WIN32
    if ( 0 == pFunc )
    {
        return 0;
    }
    
    void ** ppArgs = new void *[2];
    ppArgs[0] = pFunc;
    ppArgs[1] = pParam;
    JThrdHandle hThrd = CreateThread( 0, dwStackSize, 
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
    
    if ( dwStackSize > 0 )
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize( &attr, dwStackSize );
        
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


/*************************    END 线程   **************************************/





/**************************  其他 ***************************************/

static unsigned long  GetCurTick()
{
#ifdef WIN32
    return GetTickCount();
#else
    unsigned long ticks = (unsigned long) times( 0 );
    unsigned long HZ    = sysconf( _SC_CLK_TCK );
    return ticks * ( 1000 / HZ  );
#endif
}

static  unsigned long  GetCurrentThrdId_()
{
#ifdef WIN32
    return GetCurrentThreadId();
#else
    return (unsigned long)pthread_self();
#endif
}


typedef std::vector<cDetectBlockedFunction *>  vDetect;


class  FindBlockInfoObj
{
public:
    FindBlockInfoObj( cDetectBlockedFunction * p ) : m_p(p) {}
    bool operator () ( cDetectBlockedFunction * p )
    {
        if ( m_p == p)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    cDetectBlockedFunction *   m_p;
};


class  CThreadBlocksInfo
{
public:
    CThreadBlocksInfo( unsigned long dwThreadId ) : m_dwThreadId(dwThreadId){}
    ~CThreadBlocksInfo(){}

public:
    unsigned long                           m_dwThreadId;
    vDetect                                 m_vBlocks;
};


class FindThreadBlocksObj
{
public:
    FindThreadBlocksObj( unsigned long dwThreadId ) : m_dwThreadId ( dwThreadId ) { }
    bool  operator () ( CThreadBlocksInfo * p )
    {
        if ( p->m_dwThreadId == m_dwThreadId )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    unsigned long m_dwThreadId;
};


class  CAllBlocksInfo
{
public:
    CAllBlocksInfo()
    {
        ::JInitLock( &m_lock );
    }

    ~CAllBlocksInfo()
    {
        JLock( &m_lock );

        std::vector<CThreadBlocksInfo *>::iterator it;
        for ( it = m_vThreads.begin(); it != m_vThreads.end(); it++ )
        {
            delete *it;
        }
        m_vThreads.clear();

        JUnlock( &m_lock );
        JDeinitLock( &m_lock );
    }

    void  AddBlockInfo( cDetectBlockedFunction * pBlock )
    {
        unsigned  long  dwThreadId = GetCurrentThrdId_();
        
        CThreadBlocksInfo * pThreadInfo = 0;

        CJFuncLock theLock( m_lock );

        std::vector<CThreadBlocksInfo *>::iterator it = std::find_if( 
            m_vThreads.begin(), m_vThreads.end(), FindThreadBlocksObj(dwThreadId) );

        if ( it == m_vThreads.end() )
        {
            pThreadInfo = new CThreadBlocksInfo( dwThreadId );
            m_vThreads.push_back( pThreadInfo );
        }
        else
        {
            pThreadInfo = *it;
        }

        pThreadInfo->m_vBlocks.push_back( pBlock );
    }

    void  DelBlockInfo( cDetectBlockedFunction * pBlock )
    {
        unsigned  long  dwThreadId = GetCurrentThrdId_();

        CThreadBlocksInfo * pThreadInfo = 0;

        CJFuncLock theLock( m_lock );

        std::vector<CThreadBlocksInfo *>::iterator it = std::find_if( 
            m_vThreads.begin(), m_vThreads.end(), FindThreadBlocksObj(dwThreadId) );

        assert( it != m_vThreads.end() );
        pThreadInfo = *it;


        vDetect::iterator it_b = std::find_if ( 
            pThreadInfo->m_vBlocks.begin(), pThreadInfo->m_vBlocks.end(), FindBlockInfoObj( pBlock ) );

        assert( it_b != pThreadInfo->m_vBlocks.end() );
        assert( it_b == ( pThreadInfo->m_vBlocks.end() - 1 ) );

        pThreadInfo->m_vBlocks.erase( it_b );
    }

public:
    std::vector<CThreadBlocksInfo *>        m_vThreads;
    JLockType                               m_lock;
};


static   CAllBlocksInfo  s_AllBlocksInfo;


static const char * GetFileName( const char * szFilePath )
{
#ifdef WIN32
    const char  chDelimiter  = '\\';
#else
    const char  chDelimiter  = '/';
#endif

    const char * p = (const char *)strrchr( szFilePath, (int)chDelimiter );
    if (  p )
    {
        return (p + 1);
    }
    else
    {
        return szFilePath;
    }
}

/************************** END 其他 *************************************/







cDetectBlockedFunction::cDetectBlockedFunction( const char * szFile,  const char * szFunction, unsigned long dwLine )
{
    strncpy( m_szFile, GetFileName(szFile), MAX_SOURCE_FILE_NAME_SIZE );
    strncpy( m_szFunction, szFunction, MAX_FUNCTION_NAME_SIZE );
    m_dwLine = dwLine;
    m_dwTick = GetCurTick();

    s_AllBlocksInfo.AddBlockInfo( this );
}


cDetectBlockedFunction::~cDetectBlockedFunction()
{
    s_AllBlocksInfo.DelBlockInfo( this );
}










static void * DetectThreadFunc(void * arg )
{
    while( 1 )
    {
        unsigned long dwCurTick = GetCurTick();

        JLock( &s_AllBlocksInfo.m_lock );

        std::vector<CThreadBlocksInfo *>::iterator it;

        for ( it = s_AllBlocksInfo.m_vThreads.begin(); it != s_AllBlocksInfo.m_vThreads.end(); it++ )
        {
            int  bFindBlocked = 0;

            CThreadBlocksInfo * pThreadInfo = *it;

            vDetect & v = pThreadInfo->m_vBlocks;

            vDetect::iterator it_b;
            vDetect::reverse_iterator it_r;


            for ( it_b = v.begin(); it_b != v.end(); it_b++ )
            {
                cDetectBlockedFunction * pDetectObj = *it_b;

                unsigned long  dwElapsed = dwCurTick- pDetectObj->m_dwTick;

                if ( dwElapsed >= MIN_BLOCKED_ELAPSED_TIME )
                {
                    bFindBlocked = 1;
                    break;
                }
            }

            char buf[1024];
            std::string  sTotal;

            if ( bFindBlocked )
            {
                SNPRINTF( buf, sizeof(buf), "thread(%08x) blocked: \n", pThreadInfo->m_dwThreadId );
                sTotal += buf;
            }

            vDetect::reverse_iterator it_e( it_b );

            // 以堆栈的形式打印
            for( it_r = v.rbegin(); it_r != it_e; it_r++ )
            {
                
                cDetectBlockedFunction * pDetectObj = *it_r;
                unsigned long           dwElapsed   = dwCurTick- pDetectObj->m_dwTick;
                

#if defined(WIN32) && WINVER <= 0x400   // vc6
                SNPRINTF( buf, sizeof(buf),  "    %s, %lu, %0.2f \n",      pDetectObj->m_szFile, pDetectObj->m_dwLine, dwElapsed / 1000.0f );
#else
                SNPRINTF( buf, sizeof(buf),  "    %s, %lu, %s, %0.2f \n",  pDetectObj->m_szFile, pDetectObj->m_dwLine, pDetectObj->m_szFunction, dwElapsed / 1000.0f );
#endif  // defined(WIN32) && WINVER <= 0x400

                sTotal += buf;
            }

            if ( bFindBlocked )
            {
                sTotal += "\n";
            }
            
            fprintf( stderr, sTotal.c_str() );
        }

        JUnlock( &s_AllBlocksInfo.m_lock );

        
#ifdef WIN32
        Sleep( DETECT_INTERVAL  * 1000 );
#else
        sleep( DETECT_INTERVAL  );
#endif
    }

    return 0;
}







class cForStatic
{
public:
    cForStatic()
    {
        JCreateThread( DetectThreadFunc, 0, 0 );
    }


};


// 如果开启了检测开关
#if ENABLE_BLOCKED_FUNCTION_DETECT

static cForStatic  s_StaticInitObj;

#endif  // ENABLE_BLOCKED_FUNCTION_DETECT

























