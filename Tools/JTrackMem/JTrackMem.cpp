#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#include "MyContainer.h"

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

/***************  ������    **********************/
#ifdef WIN32
typedef CRITICAL_SECTION JLockType_;
#else
typedef pthread_mutex_t  JLockType_;
#endif

static void JInitLock  ( JLockType_ * pLock )
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

static void JDeinitLock( JLockType_ * pLock )
{
    assert( pLock );
#ifdef WIN32
    DeleteCriticalSection( pLock );
#else
    pthread_mutex_destroy( pLock );
#endif
}

void JLock( JLockType_ * pLock )
{
    assert( pLock );
#ifdef WIN32
    EnterCriticalSection( pLock );
#else
    pthread_mutex_lock( pLock );
#endif
}

static void JUnlock( JLockType_ * pLock )
{
    assert( pLock );
#ifdef WIN32
    LeaveCriticalSection( pLock );
#else
    pthread_mutex_unlock( pLock );
#endif
}
/*************  ������ ����  ********************/


// ָʾ�Ա��ļ�����ڴ����ͻ�����غ�����������
#define NJDEBUG
#include "JTrackMem.h"

#define MARGIN_GUID       GUID_LEN
#define MARGIN            ( MARGIN_GUID + MARGIN_HEAD + MARGIN_TAIL)

static const char s_guid[GUID_LEN] = { (char)0x09, (char)0xE1, (char)0x36, 
               (char)0xD4, (char)0x7D, (char)0x0B, (char)0x9B, (char)0xA0, 
               (char)0xC2, (char)0x8C, (char)0x50, (char)0x22, (char)0x9D, 
               (char)0x5A, (char)0x34, (char)0x3F };

#define HEAD_CHAR         0xBB
#define BODY_CHAR         0xFF
#define TAIL_CHAR         0xDD

// ��ʼ����־
static int   s_nInitFlag = 0;

// item Ϊ _MemItem �ṹ��
static PList s_pList    = 0;
static JLockType_ s_lock;

// �������ڴ��ͷ�Ƚϵ�
static char  s_StdHead[MARGIN_HEAD];
// �������ڴ��β�Ƚϵ�
static char  s_StdTail[MARGIN_TAIL];

// s_pList��item
typedef struct _tagMemItem
{
    char         szFileName[MAX_FILENAME];
    unsigned int nFileLen;
    unsigned int nMemBodyLen;

    void *       guid;           //16�ֽڹ̶�GUID
    void *       memhead;        //�û��ڴ�ǰ��ͷ�ڴ��
    void *       membody;        //�û��������ڴ�
    void *       memtail;        //�û��ڴ��β�ڴ��
}_MemItem, *_PMemItem;

// ��ʼ���ڴ����ģ��(ÿ�ε����ڴ溯�������ã���Ч������)
static void _JMemTrackInit()
{
    if ( 0 == s_nInitFlag )
    {
        JInitLock( &s_lock );
        s_pList = InitList();
        memset( s_StdHead, HEAD_CHAR, MARGIN_HEAD );
        memset( s_StdTail, TAIL_CHAR, MARGIN_TAIL );
        s_nInitFlag = 1;
    }
}

// ���һ���ڴ������Ϣ
// ���غ��newҲ����malloc�����ڴ��
static _PMemItem _AddMemTrack( void * memblock, unsigned int nSize, 
                               const char * szFileName, unsigned int nFileLine )
{
    assert( memblock );

    _PMemItem pItem = (_PMemItem)malloc( sizeof(_MemItem) );
    memset( pItem, 0, sizeof(_MemItem) );

    strncpy( pItem->szFileName, szFileName, MAX_FILENAME - 1 );
    pItem->szFileName[MAX_FILENAME - 1] = '\0';
    pItem->nFileLen = nFileLine;
    pItem->nMemBodyLen = nSize;

    pItem->guid    = memblock;
    pItem->memhead = (void *)((char *)pItem->guid + MARGIN_GUID);
    pItem->membody = (void *)((char *)pItem->memhead + MARGIN_HEAD);
    pItem->memtail = (void *)((char *)pItem->membody + nSize);

    memcpy( pItem->guid, s_guid, MARGIN_GUID );
    memset( pItem->memhead, HEAD_CHAR, MARGIN_HEAD );
    memset( pItem->membody, BODY_CHAR, nSize );
    memset( pItem->memtail, TAIL_CHAR, MARGIN_TAIL );

    Add2ListTail( s_pList, (void *)pItem );

    return pItem;
}

// ɾ��һ���ڴ������Ϣ
static void _EraseListByBlock( void * memblock )
{
    PListNode pNode = GetListHead( s_pList );
    while( pNode )
    {
        _PMemItem pItem = (_PMemItem)pNode->pData;
        if ( pItem->membody == memblock )
        {
            EraseList( s_pList, pNode );
            break;
        }
        pNode = GetNextListNode( pNode );
    }
}

// ɾ��һ���ڴ������Ϣ
static int _EraseListByItem( _PMemItem pDel )
{
    PListNode pNode = GetListHead( s_pList );
    while( pNode )
    {
        _PMemItem pItem = (_PMemItem)pNode->pData;
        if ( pItem == pDel )
        {
            EraseList( s_pList, pNode );
            return 1;
        }
        pNode = GetNextListNode( pNode );
    }
    return 0;
}

// �����ٱ����Ƿ��и��ڴ��
static int _IfListContainBlock( void * memblock )
{
    PListNode pNode = GetListHead( s_pList );
    while( pNode )
    {
        _PMemItem pItem = (_PMemItem)pNode->pData;
        if ( pItem->membody == memblock )
        {
            return 1;
        }
        pNode = GetNextListNode( pNode );
    }
    return 0;
}

// �����û��õ��ķ����ڴ淵�ؼ�¼���ڴ���ټ�¼��
static _PMemItem GetItemByBlock( void * memblock )
{
    PListNode pNode = GetListHead( s_pList );
    while( pNode )
    {
        _PMemItem pItem = (_PMemItem)pNode->pData;
        if ( pItem->membody == memblock )
        {
            return pItem;
        }
        pNode = GetNextListNode( pNode );
    }
    return NULL;
}

//used by realloc
static void _ChangeMemTrack( _PMemItem pItem, void * newblock, unsigned int nSize, 
                            const char * szFileName, unsigned int nFileLine )
{
    assert( newblock );

    strncpy( pItem->szFileName, szFileName, MAX_FILENAME - 1 );
    pItem->szFileName[MAX_FILENAME - 1] = '\0';
    pItem->nFileLen = nFileLine;
    pItem->nMemBodyLen = nSize;

    pItem->guid    = newblock;
    pItem->memhead = (void *)((char *)pItem->guid + MARGIN_GUID);
    pItem->membody = (void *)((char *)pItem->memhead + MARGIN_HEAD);
    pItem->memtail = (void *)((char *)pItem->membody + nSize);

    memcpy( pItem->guid, s_guid, MARGIN_GUID );
    memset( pItem->memhead, HEAD_CHAR, MARGIN_HEAD );
    memset( pItem->membody, BODY_CHAR, nSize );
    memset( pItem->memtail, TAIL_CHAR, MARGIN_TAIL );
}

// ����ڴ��ͷ�Ƿ�۸�
static int _CheckIfHeadHacked( void * memhead )
{
    assert( memhead );
    
    if ( 0 == memcmp(memhead, s_StdHead, MARGIN_HEAD) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// ����ڴ��β�Ƿ�۸�
static int _CheckIfTailHacked( void * memtail )
{
    assert( memtail );
    
    if ( 0 == memcmp(memtail, s_StdTail, MARGIN_TAIL) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// ���guid���Ƿ�۸�
static int _CheckIfGuidHacked( void * memguid )
{
    assert( memguid );
    
    if ( 0 == memcmp(memguid, s_guid, MARGIN_GUID) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void * JDEBUG_MALLOC( unsigned int nSize, const char * szFileName, unsigned int nFileLine )
{
    _JMemTrackInit();

    void * memblock = malloc(nSize + MARGIN);
    // ϵͳ�ڴ����ʧ��
    if ( NULL == memblock )
    {
        return NULL;
    }

    //�̰߳�ȫ
    JLock( &s_lock );
    // ���һ���ڴ��¼
    _PMemItem pItem = _AddMemTrack( memblock, nSize, szFileName, nFileLine );
    JUnlock( &s_lock );
    return pItem->membody;
}

void * JDEBUG_REALLOC( void *memblock, unsigned int nSize, const char * szFileName, unsigned int nFileLine )
{
    _JMemTrackInit();

    if ( 0 == memblock )
    {
        return JDEBUG_MALLOC( nSize, szFileName, nFileLine );
    }
    else
    {
        //��ͬ��free
        if ( 0 == nSize )
        {
            JDEBUG_FREE(memblock );
            return 0;
        }
        else
        {
            JLock( &s_lock );
            
            _PMemItem pItem = GetItemByBlock( memblock );
            
            // ������ٱ���û�иø��ټ�¼
            if ( 0 == pItem )
            {
                if ( MARGIN_HEAD + MARGIN_GUID >= (unsigned long)memblock )
                {
                    JUnlock( &s_lock );
                    return realloc( memblock, nSize );
                }

                char * pGuid = (char *)memblock - MARGIN_HEAD - MARGIN_GUID;
                //���ܹ�ϣ�������(����JDebugClear)������֮ǰ������ڴ滹��
                if ( 0 == memcmp(pGuid, s_guid, MARGIN_GUID) )
                {
                    void * newblock = realloc( pGuid, nSize + MARGIN );
                    if ( NULL == newblock )
                    {
                        JUnlock( &s_lock );
                        return NULL;
                    }

                    _AddMemTrack( newblock, nSize, szFileName, nFileLine );
                    JUnlock( &s_lock );
                    return (char*)newblock + MARGIN_GUID + MARGIN_HEAD;
                }
                //���ܲ�����JDEBUG������ڴ�
                //(�������JDEBUG����ģ�����guid�ֱ��۸ģ�����Ĳ������ƻ���)
                else
                {
                    JUnlock( &s_lock );
                    return realloc(memblock, nSize);
                }
            }
            else
            {
                void * newblock = realloc( pItem->guid, nSize + MARGIN );
                if ( NULL == newblock )
                {
                    _EraseListByItem( pItem );
                    free(pItem);
                    JUnlock( &s_lock );
                    return NULL;
                }
                
                _ChangeMemTrack( pItem, newblock, nSize, szFileName, nFileLine );

                JUnlock( &s_lock );
                return pItem->membody;
            }
            
            JUnlock( &s_lock );
        }
    }
}

void * JDEBUG_CALLOC( unsigned int nNum, unsigned int nSize, const char * szFileName, unsigned int nFileLine)
{
    return JDEBUG_MALLOC( nNum * nSize, szFileName, nFileLine );
}

void JDEBUG_FREE( void *memblock  )
{
    _JMemTrackInit();

    JLock( &s_lock );

    _PMemItem pItem = GetItemByBlock( memblock );

    // �����¼����û�иø��ټ�¼
    if ( NULL == pItem )
    {
        // �����κδ���
        JUnlock( &s_lock );
        return;
    }

    _EraseListByItem( pItem );

    free( pItem->guid );
    free( pItem );

    JUnlock( &s_lock );
}

int JDebugGetReport( PLeakItem * ppLeak,   unsigned int * pLeakNum, 
                      PHackItem * ppHacked, unsigned int * pHackedNum )
{
    if ( 0 == ppLeak || 0 == pLeakNum || 0 == ppHacked || 0 == pHackedNum )
    {
        return -1;
    }

    _JMemTrackInit();

    unsigned int i;

    JLock( &s_lock );

    *pLeakNum = GetListSize(s_pList);

    *ppLeak = (PLeakItem)malloc( (*pLeakNum) * sizeof(LeakItem) );
    
    i = 0;
    PListNode pNode = GetListHead( s_pList );
    while( pNode )
    {
        PLeakItem pLeak = *ppLeak + i;
        _PMemItem pItem = (_PMemItem)pNode->pData;

        strcpy( pLeak->szFileName, pItem->szFileName );
        pLeak->nFileLen = pItem->nFileLen;
        pLeak->nMemSize = pItem->nMemBodyLen;

        pNode = GetNextListNode( pNode );
        i++;
    }

    PList pTmpList = InitList();

    pNode = GetListHead( s_pList );
    while( pNode )
    {
        _PMemItem pItem = (_PMemItem)pNode->pData;
        PHackItem p = NULL;

        if ( _CheckIfGuidHacked(pItem->guid) )
        {
            p = (PHackItem)malloc(sizeof(HackItem));
            memset( p, 0, sizeof(HackItem) );

            p->bGuidHacked = 1;
            memcpy( p->pGuid, pItem->guid, GUID_LEN );
        }

        if ( _CheckIfHeadHacked(pItem->memhead) )
        {
            if ( NULL == p )
            {
                p = (PHackItem)malloc(sizeof(HackItem));
                memset( p, 0, sizeof(HackItem) );
            }
            p->bHeadHacked = 1;
            memcpy( p->pHead, pItem->memhead, MARGIN_HEAD );
        }

        if ( _CheckIfTailHacked(pItem->memtail) )
        {
            if ( NULL == p )
            {
                p = (PHackItem)malloc(sizeof(HackItem));
                memset( p, 0, sizeof(HackItem) );
            }
            p->bTailHacked = 1;
            memcpy( p->pTail, pItem->memtail, MARGIN_TAIL );
        }

        if ( NULL != p )
        {
            strcpy( p->szFileName, pItem->szFileName );
            p->nFileLen = pItem->nFileLen;

            Add2ListTail( pTmpList, (void *)p );
        }

        pNode = GetNextListNode( pNode );
    }

    *pHackedNum = GetListSize(pTmpList);

    *ppHacked = (PHackItem)malloc( (*pHackedNum) * sizeof(HackItem) );

    i = 0;
    pNode = GetListHead( pTmpList );
    while( pNode )
    {
        PHackItem pHack = *ppHacked + i;
        PHackItem p     = (PHackItem)pNode->pData;

        memcpy( pHack, p, sizeof(HackItem) );
        free( p );
    
        pNode = GetNextListNode( pNode );
        i++;
    }

    DeinitList( pTmpList );

    JUnlock( &s_lock );

    return 0;
}

//����JDebugGetReport�з�����ڴ�
void JDebugFree( void * memblock )
{
    free( memblock );
}

void JDebugClear()
{
    if ( 0 == s_nInitFlag )
    {
        return;
    }

    JLock( &s_lock );

    PListNode pNode = GetListHead( s_pList );
    while( pNode )
    {
        _PMemItem pItem = (_PMemItem)pNode->pData;
        free(pItem);
        pNode = GetNextListNode( pNode );
    }

    ClearList( s_pList );

    JUnlock( &s_lock );
}

#ifdef __cplusplus

void * operator new( unsigned int nSize, 
                     const char * szFileName, unsigned int nLine )
{
    return JDEBUG_MALLOC( nSize, szFileName, nLine );
}

void  operator delete( void * memblock, 
                     const char * szFileName, unsigned int nLine )
{
    operator delete( memblock );
}

void * operator new[]( unsigned int nSize, 
                     const char * szFileName, unsigned int nLine )
{
    return JDEBUG_MALLOC( nSize, szFileName, nLine );
}

void   operator delete[]( void * memblock, 
                     const char * szFileName, unsigned int nLine )
{
    operator delete[]( memblock );
}

void   operator delete( void * memblock )
{
    JDEBUG_FREE( memblock );
}

void   operator delete[]( void * memblock )
{
    JDEBUG_FREE( memblock );
}

#endif // end of __cplusplus
