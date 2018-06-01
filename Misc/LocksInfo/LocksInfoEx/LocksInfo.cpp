
#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <assert.h>

#include "LocksInfo.h"


#define DO_ONCE( statement )      do{ statement }while(0)
#define  SAFE_FREE( p )           DO_ONCE( if (p ) { free( p );    p = 0; } )  





/****************************************************************************/
/*                        TREE(树/森林)                                     */
/****************************************************************************/

typedef struct tagTreeNode
{
	void *    pData;
}TreeNode, *PTreeNode;


const PTreeNode LMN_FIRST_TREE_NODE = 0;
const PTreeNode LMN_LAST_TREE_NODE  = (PTreeNode)(-1);


typedef struct tagTreeNode_
{
	void *                   pData;
	struct tagTreeNode_ *    pParent;
	struct tagTreeNode_ *    pNextSibling;
	struct tagTreeNode_ *    pPrevSibling;
	struct tagTreeNode_ *    pFirstChild;
}TreeNode_, *PTreeNode_;



/****************************************************************************
 * 函数名：  _Insert2Tree                                                   *
 * 功  能：  往树里插入节点                                                 *
 * 返回值：  成功，新的节点；失败，0                                        *
 ****************************************************************************/
static  PTreeNode_ _Insert2Tree( PTreeNode_  pParent_, PTreeNode_ pAfter_, 
                                 void * pData )
{
    assert( pParent_ );

    // PTreeNode_  pCurrent_   = new TreeNode_;
    PTreeNode_  pCurrent_   = (PTreeNode_)malloc( sizeof(TreeNode_) );
    if ( 0 == pCurrent_ )
    {
        return 0;
    }
    memset( pCurrent_, 0, sizeof(TreeNode_) );

    pCurrent_->pData        = pData;
	pCurrent_->pParent      = pParent_;


    PTreeNode_  pPreviousSibling_ = 0;
    PTreeNode_  pNextSibling_     = 0;

    if ( pAfter_ == (PTreeNode_)LMN_FIRST_TREE_NODE )
    {
         pPreviousSibling_ = 0;
         pNextSibling_     = pParent_->pFirstChild;
    }
    else if ( pAfter_ == (PTreeNode_)LMN_LAST_TREE_NODE )
    {
        PTreeNode_  pTemp_ = 0;

        pTemp_ = pParent_->pFirstChild;
        while( pTemp_ )
        {
            pPreviousSibling_ = pTemp_;
            pTemp_            = pTemp_->pNextSibling;
        }

        pNextSibling_     = 0;
    }
    else
    {
        PTreeNode_  pTemp_ = 0;

        pTemp_ = pParent_->pFirstChild;
        while( pTemp_ )
        {
            if ( pTemp_ == pAfter_ )
            {
                pPreviousSibling_ = pTemp_;
                pNextSibling_     = pTemp_->pNextSibling;
                break;
            }
            pTemp_            = pTemp_->pNextSibling;
        }

        // 没有找到这个node
        if ( 0 == pPreviousSibling_ )
        {
            // delete pCurrent_;
            SAFE_FREE(pCurrent_);
            return 0;
        }
    }

    if ( pPreviousSibling_ )
    {
        pPreviousSibling_->pNextSibling = pCurrent_;
        pCurrent_->pPrevSibling         = pPreviousSibling_;
    }

    if ( pNextSibling_ )
    {
        pNextSibling_->pPrevSibling     = pCurrent_;
        pCurrent_->pNextSibling         = pNextSibling_;
    }

    if ( 0 == pPreviousSibling_ )
    {
        pParent_->pFirstChild = pCurrent_;
    }

    return pCurrent_;
}

// 注意修改父节点，兄弟节点对应的属性
static  void  _EraseTreeNode( PTreeNode_  pTreeNode_ )
{
	assert( pTreeNode_ );

	PTreeNode_  pChild = pTreeNode_->pFirstChild;
	while( pChild )
	{
		PTreeNode_  pTemp = pChild;
		pChild = pChild->pNextSibling;

		_EraseTreeNode( pTemp );
	}

	SAFE_FREE( pTreeNode_ );
}



/****************************************************************************
 * 函数名：  InitTree                                                       *
 * 功  能：  初始化树                                                       *
 * 返回值：  成功，非0；失败，0                                             *
 ****************************************************************************/
static  PTreeNode InitTree( void * pData )
{
    // PTreeNode_  pTreeNode_  =  new TreeNode_;
    PTreeNode_  pTreeNode_  =  (PTreeNode_)malloc( sizeof(TreeNode_) );
    if ( 0 == pTreeNode_ )
    {
        return 0;
    }
    memset( pTreeNode_, 0, sizeof(TreeNode_) );

	return (PTreeNode)pTreeNode_;
}


/****************************************************************************
 * 函数名：  Insert2Tree                                                    *
 * 功  能：  往树里插入节点                                                 *
 * 返回值：  成功，新的节点；失败，0                                        *
 ****************************************************************************/
static  PTreeNode Insert2Tree(  PTreeNode pParent,  void * pData, 
									   PTreeNode pAfter = LMN_LAST_TREE_NODE )
{
    if ( 0 == pParent )
	{
		return 0;
	}

	return (PTreeNode)_Insert2Tree( (PTreeNode_)pParent,  (PTreeNode_)pAfter,  pData );
}


// 获取父节点
static  PTreeNode  GetParentTreeNode(  PTreeNode pTreeNode )
{
	if ( 0 == pTreeNode )
	{
		return 0;
	}

	PTreeNode_ pTreeNode_ = (PTreeNode_)pTreeNode;

	return (PTreeNode)pTreeNode_->pParent;
}


// 获取后一个兄弟节点
static  PTreeNode  GetNextSiblingTreeNode(  PTreeNode pTreeNode )
{
	if ( 0 == pTreeNode )
	{
		return 0;
	}

	PTreeNode_ pTreeNode_ = (PTreeNode_)pTreeNode;

	return (PTreeNode)pTreeNode_->pNextSibling;
}

// 获取前一个兄弟节点
static  PTreeNode  GetPrevSiblingTreeNode(  PTreeNode pTreeNode )
{
	if ( 0 == pTreeNode )
	{
		return 0;
	}

	PTreeNode_ pTreeNode_ = (PTreeNode_)pTreeNode;

	return (PTreeNode)pTreeNode_->pPrevSibling;
}

// 获取第一孩子节点
static  PTreeNode  GetChildTreeNode(  PTreeNode pTreeNode )
{
	if ( 0 == pTreeNode )
	{
		return 0;
	}

	PTreeNode_ pTreeNode_ = (PTreeNode_)pTreeNode;

	return (PTreeNode)pTreeNode_->pFirstChild;
}

// 获取第N个孩子节点
static  PTreeNode  GetChildTreeNodeByIndex( PTreeNode pTreeNode, DWORD dwIndex )
{
	if ( 0 == pTreeNode )
	{
		return 0;
	}

	PTreeNode_ pTreeNode_ = (PTreeNode_)pTreeNode;

	DWORD  i = 0;
	PTreeNode_  pChild = pTreeNode_->pFirstChild;
	while( pChild )
	{
		if ( i == dwIndex )
		{
			return (PTreeNode)pChild;
		}

		pChild = pChild->pNextSibling;
		i++;
	}

	return 0;
}

// 删除节点
// 如果有右节点，返回右节点。如果父节点有右节点，返回右节点，以此类推
// 用于树的遍历(采用中序遍历)
static  PTreeNode  EraseTreeNode(  PTreeNode pTreeNode )
{
	if ( 0 == pTreeNode )
	{
		return 0;
	}

	PTreeNode_ pTreeNode_ = (PTreeNode_)pTreeNode;

	PTreeNode_ pParent      = pTreeNode_->pParent;
	PTreeNode_ pNextSibling = pTreeNode_->pNextSibling;
	PTreeNode_ pPrevSibling = pTreeNode_->pPrevSibling;

	_EraseTreeNode( pTreeNode_ );

	// 如果有左节点
	if ( pPrevSibling )
	{
		pPrevSibling->pNextSibling = pNextSibling;
	}

	// 如果有右节点
	if ( pNextSibling )
	{
		pNextSibling->pPrevSibling = pPrevSibling;
	}

	// 如果有父节点而且是父节点的首节点
	if ( pParent && 0 == pPrevSibling )
	{
		pParent->pFirstChild = pNextSibling;
	}

	// 如果有右节点
	if ( pNextSibling )
	{
		return (PTreeNode)pNextSibling;
	}
	else
	{
		while( pParent )
		{
			if ( pParent->pNextSibling )
			{
				return (PTreeNode)pParent->pNextSibling;
			}
			
			pParent = pParent->pParent;
		}

		return 0;
	}
}


// 销毁树
static  void   DeInitTree( PTreeNode pTreeRoot )
{
	if ( 0 == pTreeRoot )
	{
		return;
	}

	PTreeNode_ pTreeNode_ = ( PTreeNode_ )pTreeRoot;

	_EraseTreeNode( pTreeNode_ );
}


// 遍历树的时候使用
static  PTreeNode  GetNextTreeNode( PTreeNode pTreeNode )
{
	if ( 0 == pTreeNode )
	{
		return 0;
	}

	PTreeNode_ pTreeNode_ = ( PTreeNode_ )pTreeNode;

	if ( pTreeNode_->pFirstChild )
	{
		return (PTreeNode)pTreeNode_->pFirstChild;
	}
	else if ( pTreeNode_->pNextSibling )
	{
		return (PTreeNode)pTreeNode_->pNextSibling;
	}
	else
	{
		PTreeNode_ pParent = pTreeNode_->pParent;
		while( pParent )
		{
			if ( pParent->pNextSibling )
			{
				return (PTreeNode)pParent->pNextSibling;
			}
			
			pParent = pParent->pParent;
		}

		return 0;
	}
}












































/*************************    2. 互斥锁   **************************************/
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


/*************************    END 互斥锁   **************************************/


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

#define  MAX_FILE_NAME_LEN               254
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



typedef struct tagLockNodeSubInfo
{
    char              szLockFileName[MAX_FILE_NAME_LEN];
    char              szLockFunction[MAX_FUNC_NAME_LEN];
    unsigned long     dwLockLine;

    tagLockNodeSubInfo(){ memset(this, 0, sizeof(*this)); }
    tagLockNodeSubInfo( const char * szFile, const char * szFunc, unsigned long dwLine)
    {
        memset(this, 0, sizeof(*this));

        if ( szFile )
            strncpy( szLockFileName, szFile, MAX_FILE_NAME_LEN );
        if ( szFunc )
            strncpy( szLockFunction, szFunc, MAX_FUNC_NAME_LEN );
        dwLockLine = dwLine;
    }
}TLockNodeSubInfo, *PTLockNodeSubInfo;



typedef struct tagLockNodeInfo
{
    void *            pLock;
    
    std::vector<PTLockNodeSubInfo>  vSubInfos;

    tagLockNodeInfo(){ pLock = 0; }
    ~tagLockNodeInfo()
    {
        std::vector<PTLockNodeSubInfo>::iterator it;
        for ( it = vSubInfos.begin(); it != vSubInfos.end(); it++ )
        {
            PTLockNodeSubInfo  pSubInfo = *it;
            delete pSubInfo;
        }
        vSubInfos.clear();
    }
}TLockNodeInfo, *PTLockNodeInfo;


class  FindSubLockInfoObj
{
public:
    FindSubLockInfoObj( const char * szFile, const char * szFunc, unsigned long dwLine )
    {
        m_pFile  = szFile;
        m_pFunc  = szFunc;
        m_dwLine = dwLine;
    }

    bool operator()( const TLockNodeSubInfo * pSubInfo )
    {
        if ( 0 == m_pFile || 0 == m_pFunc )
        {
            return false;
        }

        if ( 0 != strcmp(m_pFile, pSubInfo->szLockFileName) )
        {
            return false;
        }

        if ( 0 != strcmp(m_pFunc, pSubInfo->szLockFunction) )
        {
            return false;
        }

        if ( m_dwLine != pSubInfo->dwLockLine )
        {
            return false;
        }

        return true;
    }
private:
    const char *  m_pFile;
    const char *  m_pFunc;
    unsigned long m_dwLine;
};










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

    std::vector<void *>          m_vCurLocked;
    PTreeNode                    m_pTreeLocks;

private:
    void  ClearTreeLocks( PTreeNode  pNode, unsigned long dwDepth );
    void  UpdateTreeLocks( void * pLock,  const char * szFileName, const char * szFunction, unsigned long dwLine );

public:
    void  GetTreeLocksInfo( std::string & sInfo, const std::string & sDecent, PTreeNode pNode, unsigned long dwDepth );
};


CThreadLocksInfo::CThreadLocksInfo( unsigned long dwThreadId )
{
    m_dwThreadId = dwThreadId;

    m_pTreeLocks = InitTree( 0 );
    if ( 0 == m_pTreeLocks )
    {
        PERROR("CThreadLocksInfo::CThreadLocksInfo: failed to InitTree()! \n");
    }
}

void  CThreadLocksInfo::ClearTreeLocks( PTreeNode  pNode, unsigned long dwDepth )
{
    PTreeNode pChild = GetChildTreeNode( pNode );
    while ( pChild )
    {
        ClearTreeLocks( pChild, dwDepth + 1 );
        pChild = GetNextSiblingTreeNode( pChild );
    }

    // 根节点的数据为0
    if ( pNode->pData )
    {
        PTLockNodeInfo  pLockNodeInfo = (PTLockNodeInfo)pNode->pData;
        delete  pLockNodeInfo;
    }
}


void CThreadLocksInfo::UpdateTreeLocks( void * pLock,  const char * szFileName, const char * szFunction, unsigned long dwLine )
{
    if ( 0 == m_pTreeLocks )
    {
        return;
    }

    PTreeNode  pCurNode = m_pTreeLocks;
    std::vector<void *>::iterator  it_lock;

    for ( it_lock = m_vCurLocked.begin(); it_lock != m_vCurLocked.end(); it_lock++ )
    {
        PTreeNode  pFindNode = 0;
        void * pLock = *it_lock;


        PTreeNode pChild = GetChildTreeNode( pCurNode );
        while ( pChild )
        {
            PTLockNodeInfo  pLockInfo = (PTLockNodeInfo)pChild->pData;
            if ( pLockInfo->pLock == pLock )
            {
                pFindNode = pChild;
                break;
            }
            pChild = GetNextSiblingTreeNode( pChild );
        }

        // 没找到
        if ( 0 == pFindNode )
        {
            PERROR("CThreadLocksInfo::UpdateTreeLocks: failed to find a tree path! \n");
            return;
        }

        pCurNode = pFindNode;
    }



    PTreeNode  pUpdateNode = 0;
    PTreeNode pChild = GetChildTreeNode( pCurNode );
    while ( pChild )
    {
        PTLockNodeInfo  pLockInfo = (PTLockNodeInfo)pChild->pData;
        if ( pLockInfo->pLock == pLock )
        {
            pUpdateNode = pChild;
            break;
        }
        pChild = GetNextSiblingTreeNode( pChild );
    }


    // 如果tree里没有
    if ( 0 == pUpdateNode )
    {
        PTLockNodeInfo  pInfo = new TLockNodeInfo;

        pInfo->pLock = pLock;
        pInfo->vSubInfos.push_back( new TLockNodeSubInfo(szFileName, szFunction, dwLine) );

        pUpdateNode = Insert2Tree( pCurNode, pInfo );
        if ( 0 == pUpdateNode )
        {
            PERROR("CThreadLocksInfo::UpdateTreeLocks: failed to Insert2Tree()! \n");
            delete pInfo;
            return;
        }
    }
    else
    {
        PTLockNodeInfo  pInfo = (PTLockNodeInfo)pUpdateNode->pData;

        std::vector<PTLockNodeSubInfo>::iterator it_sub = find_if( 
            pInfo->vSubInfos.begin(), pInfo->vSubInfos.end(), FindSubLockInfoObj(szFileName, szFunction, dwLine) );
        if ( it_sub == pInfo->vSubInfos.end() )
        {
            pInfo->vSubInfos.push_back( new TLockNodeSubInfo(szFileName, szFunction, dwLine) );
        }
    }
}

void  CThreadLocksInfo::GetTreeLocksInfo( std::string & sInfo, const std::string & sDecent, PTreeNode pNode, unsigned long dwDepth )
{
    if ( 0 == pNode )
    {
        return;
    }

    char buf[1024];
    std::string   sTotalDecent;

    unsigned long i;
    for ( i = 0; i < dwDepth; i++ )
    {
        sTotalDecent += sDecent;
    }
    sInfo += sTotalDecent;


    PTLockNodeInfo pLockInfo = (PTLockNodeInfo)pNode->pData;
    if ( 0 != pLockInfo )
    {
        sprintf( buf, "lock: 0x%p\n", pLockInfo->pLock );
        sInfo += buf;

        std::vector<PTLockNodeSubInfo>::iterator  it_sub;
        for ( it_sub = pLockInfo->vSubInfos.begin(); it_sub != pLockInfo->vSubInfos.end(); it_sub++ )
        {
            PTLockNodeSubInfo  pSubInfo =  *it_sub;

            sInfo += sTotalDecent;
            sprintf( buf, "file=%s, func=%s, line=%lu \n", pSubInfo->szLockFileName, pSubInfo->szLockFunction, pSubInfo->dwLockLine );
            sInfo += buf;
        }
    }
    else
    {
        sprintf( buf, "thread: 0x%08X:\n", m_dwThreadId );
        sInfo += buf;
    }

    PTreeNode pChild = GetChildTreeNode( pNode );
    while ( pChild )
    {
        GetTreeLocksInfo( sInfo, sDecent, pChild, dwDepth + 1 );
        pChild = GetNextSiblingTreeNode( pChild );
    }
}


CThreadLocksInfo::~CThreadLocksInfo()
{
    std::vector<CLockInfo *>::iterator it;
    for ( it = m_vLocksInfo.begin(); it != m_vLocksInfo.end(); it++ )
    {
        delete  *it;
    }
    m_vLocksInfo.clear();


    if ( 0 == m_pTreeLocks )
    {
        return;
    }

    ClearTreeLocks( m_pTreeLocks, 0 );
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




    // for tree locks info
    if ( LOCK_STATE_LOCKED == eState )
    {
        UpdateTreeLocks( pLock, szFileName, szFunction, dwLine );

        m_vCurLocked.push_back( pLock );
    }
    else if ( LOCK_STATE_UNLOCKED == eState )
    {
        assert( m_vCurLocked.size() > 0 );
        std::vector<void *>::iterator it_last = m_vCurLocked.end() - 1;
        m_vCurLocked.erase( it_last );
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

static BOOL  CanFinalDeadLock_( PTreeNode pParent_A, PTreeNode pNodeB, void * pNotTheLock )
{
    PTreeNode pChild_B = GetChildTreeNode( pNodeB );
    while ( pChild_B )
    {
        if ( CanFinalDeadLock_( pParent_A, pChild_B, pNotTheLock ) )
        {
            return TRUE;
        }

        pChild_B = GetNextSiblingTreeNode( pChild_B );
    }


    PTLockNodeInfo  pLockNodeInfoB = (PTLockNodeInfo)pNodeB->pData;
    if ( pLockNodeInfoB->pLock == pNotTheLock )
    {
        return FALSE;
    }


    PTreeNode  pNodeA = pParent_A;
    while( pNodeA )
    {
        if ( 0 == pNodeA->pData )
        {
            return FALSE;
        }

        PTLockNodeInfo  pLockNodeInfoA = (PTLockNodeInfo)pNodeA->pData;
        if ( pLockNodeInfoA->pLock == pLockNodeInfoB->pLock )
        {
            return TRUE;
        }

        pNodeA = GetParentTreeNode( pNodeA );
    }

    return FALSE;
}


static BOOL  CanFinalDeadLock_( PTreeNode pNodeA, PTreeNode pNodeB )
{
    // 遍历A中的tree nodes
    PTreeNode pChild_A = GetChildTreeNode( pNodeA );
    while ( pChild_A )
    {
        if ( CanFinalDeadLock_( pChild_A, pNodeB ) )
        {
            return TRUE;
        }

        pChild_A = GetNextSiblingTreeNode( pChild_A );
    }

    // root node
    if ( 0 == pNodeA->pData )
    {
        return FALSE;
    }


    PTLockNodeInfo  pLockNodeInfoA = (PTLockNodeInfo)pNodeA->pData;
    PTLockNodeInfo  pLockNodeInfoB = (PTLockNodeInfo)pNodeB->pData;

    // lock address不一样
    if ( pLockNodeInfoA->pLock != pLockNodeInfoB->pLock )
    {
        return FALSE;
    }




    // 遍历B的child，是否在A之前出现
    PTreeNode  pParent_A = GetParentTreeNode( pNodeA );
    if ( 0 == pParent_A->pData )
    {
        return FALSE;
    }


    PTreeNode pChild_B = GetChildTreeNode( pNodeB );
    while ( pChild_B )
    {
        if ( CanFinalDeadLock_( pParent_A, pChild_B, pLockNodeInfoB->pLock ) )
        {
            return TRUE;
        }

        pChild_B = GetNextSiblingTreeNode( pChild_B );
    }

    return FALSE;
}


static BOOL  CanDeadLock_( CThreadLocksInfo * pA, PTreeNode pNodeB )
{
    // 遍历B中的tree node
    PTreeNode pChild_B = GetChildTreeNode( pNodeB );
    while ( pChild_B )
    {
        if ( CanDeadLock_( pA, pChild_B ) )
        {
            return TRUE;
        }

        pChild_B = GetNextSiblingTreeNode( pChild_B );
    }

    // root 节点
    if ( 0 == pNodeB->pData )
    {
        return FALSE;
    }

    if ( CanFinalDeadLock_( pA->m_pTreeLocks, pNodeB ) )
    {
        return TRUE;
    }

    return FALSE;
}







static void print_node( PTreeNode pNode, DWORD dwDepth )
{
    DWORD i;
    for ( i = 0; i < dwDepth; i++ )
    {
        fprintf( stderr, "  " );
    }

    fprintf( stderr, "%p\n", pNode );

    PTreeNode pChild = GetChildTreeNode( pNode );
    while ( pChild )
    {
        print_node(pChild, dwDepth+1);
        pChild = GetNextSiblingTreeNode( pChild );
    }    
}

static BOOL  CanDeadLock_( CThreadLocksInfo * pA, CThreadLocksInfo * pB )
{
    //print_node( pA->m_pTreeLocks, 0 );
    //print_node( pB->m_pTreeLocks, 0 );

    // 遍历B中的tree node
    return CanDeadLock_( pA, pB->m_pTreeLocks );
}



void   PRINT_TREE_LOCKS_INFO()
{
    char               buf[1024];
    std::string        s;
    const std::string  sDecent = "  ";
    CJFuncLock    theLock( s_AllLockInfo.m_lock );

    std::vector<CThreadLocksInfo *>::iterator it;
    for ( it = s_AllLockInfo.m_vAllInfo.begin(); it != s_AllLockInfo.m_vAllInfo.end(); it++ )
    {
        CThreadLocksInfo * pThreadInfo = *it;

        if ( 0 == pThreadInfo->m_pTreeLocks )
        {
            continue;
        }

        pThreadInfo->GetTreeLocksInfo( s, sDecent, pThreadInfo->m_pTreeLocks, 0 );

        s += "\n";
    }





    std::vector<CThreadLocksInfo *>::iterator it_a;
    std::vector<CThreadLocksInfo *>::iterator it_b;
    BOOL  bDetectDeadLock = FALSE;

    for ( it_a = s_AllLockInfo.m_vAllInfo.begin(); it_a != s_AllLockInfo.m_vAllInfo.end(); it_a++ )
    {
        CThreadLocksInfo * pThreadInfoA = *it_a;

        if ( 0 == pThreadInfoA->m_pTreeLocks )
        {
            continue;
        }


        for ( it_b = it_a + 1; it_b != s_AllLockInfo.m_vAllInfo.end(); it_b++ )
        {
            CThreadLocksInfo * pThreadInfoB = *it_b;

            if ( 0 == pThreadInfoB->m_pTreeLocks )
            {
                continue;
            }

            // 检查A和B有无死锁可能
            if ( CanDeadLock_(pThreadInfoA, pThreadInfoB) )
            {
                bDetectDeadLock = TRUE;

                sprintf( buf, "thread(0x%08X) and thread(0x%08X) have dead lock conflict! \n", pThreadInfoA->m_dwThreadId, pThreadInfoB->m_dwThreadId );
                s += buf;
                break;
            }
        }

        if ( bDetectDeadLock )
        {
            break;
        }
    }

    if ( !bDetectDeadLock )
    {
        s += "does not detect dead lock. \n";
    }



    if ( s.length() > 0 )
    {
        fprintf( stderr, s.c_str() );
    }



    
}