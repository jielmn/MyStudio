
#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

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


class CJFuncLock
{
public:
    CJFuncLock( JLockType & lock ) : m_lock( lock )
    {
        JLock( &m_lock );
    }

    ~CJFuncLock()
    {
        JUnlock( &m_lock );
    }

private:
    JLockType & m_lock;
};


/*************************    END »¥³âËø   **************************************/


static unsigned long LmnGetCurrentThrdId()
{
#ifdef WIN32
    return GetCurrentThreadId();
#else
    return (unsigned long)pthread_self();
#endif
}


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





#ifdef WIN32
#define PERROR(f,...)           fprintf( stderr, f, __VA_ARGS__ )
#else
#define PERROR(f...)            fprintf( stderr, f )
#endif

#define  MAX_FILE_NAME_LEN               64
#define  MAX_FUNC_NAME_LEN               64


typedef struct tagLockItemInfo
{
    char              szLockFileName[MAX_FILE_NAME_LEN];
    char              szLockFunction[MAX_FUNC_NAME_LEN];
    unsigned long     dwLockLine;

    char              szUnlockFileName[MAX_FILE_NAME_LEN];
    char              szUnlockFunction[MAX_FUNC_NAME_LEN];
    unsigned long     dwUnlockLine;

    ELockState        eState;

    unsigned long     dwIndex;
    void *            pData;

    tagLockItemInfo(){ memset(this, 0, sizeof(*this)); }
}TLockItemInfo, *PTLockItemInfo;




class   CLockInfo
{
public:
    CLockInfo();
    ~CLockInfo();

public:
    void *                         m_pLock;
    std::vector<PTLockItemInfo>    m_vInfos;
};

CLockInfo::CLockInfo()
{
    m_pLock   = 0;
}

CLockInfo::~CLockInfo()
{
    std::vector<PTLockItemInfo>::iterator  it;
    for ( it = m_vInfos.begin(); it != m_vInfos.end(); it++ )
    {
        delete * it;
    }
    m_vInfos.clear();
}





class FindLockInfoObj
{
public:
    FindLockInfoObj( void * pLock );
    bool operator() ( CLockInfo * p );

private:
    void *    m_pLock;
};


FindLockInfoObj::FindLockInfoObj( void * pLock )
{
    m_pLock = pLock;
}

bool FindLockInfoObj::operator() ( CLockInfo * p )
{
    return p->m_pLock == m_pLock ? true : false;
}






class  CThreadLocksInfo
{
public:
    CThreadLocksInfo( unsigned long dwThreadId );
    ~CThreadLocksInfo();

    int  SetLockInfo( void * pLock,  const char * szFileName, const char * szFunction, unsigned long dwLine,  ELockState eState, unsigned long dwIndex );

public:
    unsigned  long               m_dwThreadId;
    std::vector<CLockInfo *>     m_vLocksInfo;
};


CThreadLocksInfo::CThreadLocksInfo( unsigned long dwThreadId )
{
    m_dwThreadId = dwThreadId;
}

CThreadLocksInfo::~CThreadLocksInfo()
{
    std::vector<CLockInfo *>::iterator it;
    for ( it = m_vLocksInfo.begin(); it != m_vLocksInfo.end(); it++ )
    {
        delete  *it;
    }
    m_vLocksInfo.clear();
}


int  CThreadLocksInfo::SetLockInfo( void * pLock,  const char * szFileName, const char * szFunction, unsigned long dwLine,  ELockState eState, unsigned long dwIndex )
{
    std::vector<CLockInfo *>::iterator  it;
    CLockInfo * pLockInfo = 0;

    it = std::find_if( m_vLocksInfo.begin(), m_vLocksInfo.end(), FindLockInfoObj(pLock) );
    if ( m_vLocksInfo.end() == it )
    {
        if ( !( LOCK_STATE_TRY_TO_LOCK == eState || LOCK_STATE_LOCKED == eState ) )
        {
            PERROR("CThreadLocksInfo::SetLockInfo: eState argument is wrong! \n");
            return -1;
        }

        pLockInfo = new CLockInfo;
        if ( 0 == pLockInfo )
        {
            PERROR("CThreadLocksInfo::SetLockInfo: failed to allocate memory! \n");
            return -1;
        }


        PTLockItemInfo  pItemInfo = new TLockItemInfo;
        if ( 0 == pItemInfo )
        {
            PERROR("CThreadLocksInfo::SetLockInfo: failed to allocate memory! \n");
            delete pLockInfo;
            return -1;
        }

        strncpy( pItemInfo->szLockFileName, GetFileName(szFileName), sizeof(pItemInfo->szLockFileName) - 1 );
        if ( szFunction )
        {
            strncpy( pItemInfo->szLockFunction, szFunction, sizeof(pItemInfo->szLockFunction) - 1 );
        }
        pItemInfo->dwLockLine = dwLine;
        pItemInfo->eState     = eState;
        pItemInfo->dwIndex    = dwIndex;
        pItemInfo->pData      = pLock;

        pLockInfo->m_pLock = pLock;
        pLockInfo->m_vInfos.push_back( pItemInfo );

        m_vLocksInfo.push_back( pLockInfo );
    }
    else
    {
        pLockInfo = *it;
        assert( pLockInfo->m_vInfos.size() > 0 );

        std::vector<PTLockItemInfo>::iterator it_item;
        it_item = pLockInfo->m_vInfos.end() - 1;
        
        PTLockItemInfo  pItemInfo = *it_item;

        if ( LOCK_STATE_TRY_TO_LOCK == pItemInfo->eState )
        {
            if ( LOCK_STATE_LOCKED != eState )
            {
                PERROR("CThreadLocksInfo::SetLockInfo: eState argument is wrong! \n");
                return -1;
            }
            pItemInfo->eState  = eState;
            pItemInfo->dwIndex = dwIndex;
        }
        else if ( LOCK_STATE_LOCKED == pItemInfo->eState )
        {
            if ( LOCK_STATE_LOCKED == eState )
            {
                PERROR("CThreadLocksInfo::SetLockInfo: eState argument is wrong! \n");
                return -1;
            }

            if ( LOCK_STATE_TRY_TO_LOCK == eState )
            {
                PTLockItemInfo  pItemInfo = new TLockItemInfo;
                if ( 0 == pItemInfo )
                {
                    PERROR("CThreadLocksInfo::SetLockInfo: failed to allocate memory! \n");
                    return -1;
                }

                strncpy( pItemInfo->szLockFileName, GetFileName(szFileName), sizeof(pItemInfo->szLockFileName) - 1 );
                if ( szFunction )
                {
                    strncpy( pItemInfo->szLockFunction, szFunction, sizeof(pItemInfo->szLockFunction) - 1 );
                }
                pItemInfo->dwLockLine = dwLine;
                pItemInfo->eState     = eState;
                pItemInfo->dwIndex    = dwIndex;
                pItemInfo->pData      = pLock;

                pLockInfo->m_vInfos.push_back( pItemInfo );
            }
            else if ( LOCK_STATE_TRY_TO_UNLOCK == eState )
            {
                strncpy( pItemInfo->szUnlockFileName, GetFileName(szFileName), sizeof(pItemInfo->szUnlockFileName) - 1 );
                if ( szFunction )
                {
                    strncpy( pItemInfo->szUnlockFunction, szFunction, sizeof(pItemInfo->szUnlockFunction) - 1 );
                }
                pItemInfo->dwUnlockLine = dwLine;
                pItemInfo->eState       = eState;
                pItemInfo->dwIndex      = dwIndex;
            }
            else if ( LOCK_STATE_UNLOCKED == eState )
            {
                delete pItemInfo;
                pLockInfo->m_vInfos.erase( it_item );
            }
        }
        else if ( LOCK_STATE_TRY_TO_UNLOCK == pItemInfo->eState )
        {
            if ( LOCK_STATE_UNLOCKED != eState )
            {
                PERROR("CThreadLocksInfo::SetLockInfo: eState argument is wrong! \n");
                return -1;
            }

            delete pItemInfo;
            pLockInfo->m_vInfos.erase( it_item );
        }
        else
        {
            assert( 0 );
        }

        if ( 0 == pLockInfo->m_vInfos.size() )
        {
            delete pLockInfo;
            m_vLocksInfo.erase( it );
        }
    }
    return 0;
}





class  FindThreadLockObj
{
public:
    FindThreadLockObj( unsigned long  dwThradId );
    bool operator () ( CThreadLocksInfo * p );

private:
    unsigned  long     m_dwThreadId;
};



FindThreadLockObj::FindThreadLockObj( unsigned long  dwThradId )
{
    m_dwThreadId = dwThradId;
}


bool FindThreadLockObj::operator () ( CThreadLocksInfo * p )
{
    return p->m_dwThreadId == m_dwThreadId ? true : false;
}








class  CAllLockInfo
{
public:
    CAllLockInfo();
    ~CAllLockInfo();

    int  SetLockInfo( void * pLock,  const char * szFileName, const char * szFunction, unsigned long dwLine,  ELockState eState );

public:
    std::vector<CThreadLocksInfo *>    m_vAllInfo;
    JLockType                          m_lock;
    unsigned long                      m_dwMaxIndex;
};



CAllLockInfo::CAllLockInfo()
{
    m_dwMaxIndex = 0;
    JInitLock( &m_lock );
}

CAllLockInfo::~CAllLockInfo()
{
    JLock( &m_lock );

    std::vector<CThreadLocksInfo *>::iterator  it;
    for ( it = m_vAllInfo.begin();  it != m_vAllInfo.end();  it++ )
    {
        delete *it;
    }
    m_vAllInfo.clear();

    JUnlock( &m_lock );

    JDeinitLock( &m_lock );
}


int  CAllLockInfo::SetLockInfo( void * pLock,  const char * szFileName, const char * szFunction, unsigned long dwLine,  ELockState eState )
{
    unsigned long dwThreadId = LmnGetCurrentThrdId();

    std::vector<CThreadLocksInfo *>::iterator  it;
    CThreadLocksInfo * pThreadInfo = 0;

    CJFuncLock   theLock( m_lock );

    it = std::find_if( m_vAllInfo.begin(), m_vAllInfo.end(), FindThreadLockObj(dwThreadId) );
    if ( m_vAllInfo.end() == it )
    {
        pThreadInfo = new CThreadLocksInfo(dwThreadId);
        if ( 0 == pThreadInfo )
        {
            PERROR("Failed to allocate memory! \n");
            return -1;
        }
        m_vAllInfo.push_back( pThreadInfo );
    }
    else
    {
        pThreadInfo = *it;
    }

    int nRet = pThreadInfo->SetLockInfo( pLock, szFileName, szFunction, dwLine, eState, m_dwMaxIndex );
    if ( 0 != nRet )
    {
        PERROR("Failed to SetLockInfo(%p, %s, %s, %lu, %u) \n", pLock, szFileName, szFunction, dwLine, eState );
    }
    m_dwMaxIndex++;
    return nRet;
}



static  CAllLockInfo  s_AllLockInfo;









void   SET_LOCK_STATE ( void * pLock,  const char * szFileName, const char * szFunction, unsigned long dwLine,  ELockState eState )
{
    if ( 0 == pLock ||  0 == szFileName || LOCK_STATE_NONE == eState || eState > LOCK_STATE_UNLOCKED )
    {
        PERROR("SET_LOCK_STATE argument error! \n");
        return;
    }

    s_AllLockInfo.SetLockInfo( pLock,  szFileName,  szFunction,  dwLine,  eState );
}

void  PRINT_ALL_LOCK_INFO()
{
    char buf[128];
    std::string   s;
    CJFuncLock    theLock( s_AllLockInfo.m_lock );

    const std::string  sDecent = "  ";

    std::vector<CThreadLocksInfo *>::iterator it;
    for ( it = s_AllLockInfo.m_vAllInfo.begin(); it != s_AllLockInfo.m_vAllInfo.end(); it++ )
    {
        CThreadLocksInfo * pThreadInfo = *it;

        if ( 0 == pThreadInfo->m_vLocksInfo.size() )
        {
            continue;
        }

        std::vector<PTLockItemInfo>  vTmp;


        std::vector<CLockInfo *>::iterator it_sub;
        for ( it_sub = pThreadInfo->m_vLocksInfo.begin(); it_sub != pThreadInfo->m_vLocksInfo.end(); it_sub++ )
        {
            CLockInfo * pLock = *it_sub;
            assert( pLock->m_vInfos.size() > 0 );

            std::vector<PTLockItemInfo>::iterator it_item;
            for ( it_item = pLock->m_vInfos.begin(); it_item != pLock->m_vInfos.end(); it_item++ )
            {
                PTLockItemInfo  pItem = *it_item;
                
                
                std::vector<PTLockItemInfo>::iterator  it_tmp;
                for ( it_tmp = vTmp.begin(); it_tmp != vTmp.end(); it_tmp++ )
                {
                    PTLockItemInfo  pTmpItem = *it_tmp;

                    assert( pItem->dwIndex != pTmpItem->dwIndex );

                    if ( pItem->dwIndex > pTmpItem->dwIndex )
                    {
                        break;
                    }
                }

                if ( it_tmp == vTmp.end() )
                {
                    vTmp.push_back( pItem );
                }
                else
                {
                    vTmp.insert( it_tmp, pItem );
                }
            }
        }

        s += "thread id: ";
        sprintf( buf, "0x%08x", pThreadInfo->m_dwThreadId );
        s += buf;
        s += "\n";
        
        std::vector<PTLockItemInfo>::iterator  it_print;
        for ( it_print = vTmp.begin(); it_print != vTmp.end(); it_print++ )
        {
            PTLockItemInfo  pPrintItem = *it_print;

            assert ( pPrintItem->eState != LOCK_STATE_NONE && pPrintItem->eState != LOCK_STATE_UNLOCKED );

            s += sDecent;

            if ( pPrintItem->eState == LOCK_STATE_TRY_TO_LOCK )
            {
                s += pPrintItem->szLockFileName;
                s += ", ";
                s += pPrintItem->szLockFunction;
                s += ", ";

                sprintf( buf, "%lu", pPrintItem->dwLockLine );
                s += buf;
                s += ": " ;
                s += "try to lock";
            }
            else if ( pPrintItem->eState == LOCK_STATE_LOCKED )
            {
                s += pPrintItem->szLockFileName;
                s += ", ";
                s += pPrintItem->szLockFunction;
                s += ", ";

                sprintf( buf, "%lu", pPrintItem->dwLockLine );
                s += buf;
                s += ": " ;
                s += "locked";
            }
            else
            {
                s += pPrintItem->szUnlockFileName;
                s += ", ";
                s += pPrintItem->szUnlockFunction;
                s += ", ";

                sprintf( buf, "%lu", pPrintItem->dwUnlockLine );
                s += buf;
                s += ": " ;
                s += "try to unlock";
            }

            s += "(";
            sprintf( buf, "0x%08x", pPrintItem->pData );
            s += buf;
            s += ")";
            s += "\n";
        }
        s += "\n";
    }

    if ( s.length() > 0 )
    {
        fprintf( stderr, s.c_str() );
    }
}
