#include <stdio.h>
#include <windows.h>
#include "LocksInfo.h"








/*************************    2. »¥³âËø   **************************************/
#ifdef WIN32
typedef CRITICAL_SECTION JLockType;
#else
typedef pthread_mutex_t  JLockType;
#endif


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

static void JLock( JLockType * pLock, const char * szFile, const char * szFunction, unsigned long dwLine )
{
    SET_LOCK_STATE ( pLock,  szFile, szFunction, dwLine,  LOCK_STATE_TRY_TO_LOCK );
#ifdef WIN32
    EnterCriticalSection( pLock );
#else
    pthread_mutex_lock( pLock );
#endif
    SET_LOCK_STATE ( pLock,  szFile, szFunction, dwLine,  LOCK_STATE_LOCKED );
}

static void JUnlock( JLockType * pLock, const char * szFile, const char * szFunction, unsigned long dwLine )
{
    SET_LOCK_STATE ( pLock,  szFile, szFunction, dwLine,  LOCK_STATE_TRY_TO_UNLOCK );
#ifdef WIN32
    LeaveCriticalSection( pLock );
#else
    pthread_mutex_unlock( pLock );
#endif
    SET_LOCK_STATE ( pLock,  szFile, szFunction, dwLine,  LOCK_STATE_UNLOCKED );
}

/*************************    END »¥³âËø   **************************************/


#ifdef WIN32
typedef HANDLE          LmnThrdType;
#else
typedef pthread_t       LmnThrdType;
#endif

typedef  void * (*LmnThreadFunc)(void *);

#ifdef WIN32
static DWORD WINAPI _WinThrdRuntine( LPVOID lpThreadParameter )
{
    void ** ppArgs    = (void **)lpThreadParameter;

    LmnThreadFunc pFunc = (LmnThreadFunc)ppArgs[0];
    void * pParam     = ppArgs[1];

    void * pRet = pFunc( pParam );

    free( ppArgs );

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

    void ** ppArgs = (void **)malloc( sizeof(void *) * 2 );
    if ( 0 == ppArgs )
    {
        return 0;
    }

    ppArgs[0] = pfnThrd;
    ppArgs[1] = pParam;

    LmnThrdType hThrd = ::CreateThread( 0, dwStackSize, _WinThrdRuntine, ppArgs, 0, 0 );
    if ( 0 == hThrd )
    {
        free( ppArgs );
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

/*************************   END Ïß³Ì    ************************************/
#ifdef WIN32
#define SLEEP(n)           Sleep(n)
#else
#define SLEEP(n)           usleep( (n)*1000 )
#endif

#define  LOCK(s)               JLock( s, __FILE__, __FUNCTION__, __LINE__ )
#define  UNLOCK(s)             JUnlock( s, __FILE__, __FUNCTION__, __LINE__ )

static JLockType  s_lock1;
static JLockType  s_lock2;

void test2(unsigned long n);

void test1( unsigned long n )
{
    LOCK( &s_lock1 );
    test2( 100 );
    UNLOCK( &s_lock1 );
}

void test2(unsigned long n)
{
    LOCK( &s_lock2 );
    SLEEP( n );
    UNLOCK( &s_lock2 );
}

void * TestThrd1(void * pArg)
{
    while( 1 )
    {
        LOCK( &s_lock1 );
        test1( 100 );
        UNLOCK( &s_lock1 );

        SLEEP( 100 );
    }
    

    return 0;
}

void * TestThrd2(void * pArg)
{
    while( 1 )
    {
        test2( 2000 );
        SLEEP( 100 );
    }
    return 0;
}


int main()
{
    JInitLock( &s_lock1 );
    JInitLock( &s_lock2 );

    LmnCreateThread( TestThrd1, 0, 0 );
    LmnCreateThread( TestThrd2, 0, 0 );

    while( 1 )
    {
        PRINT_ALL_LOCK_INFO();
        SLEEP( 1000 );
    }
    return 0;
}