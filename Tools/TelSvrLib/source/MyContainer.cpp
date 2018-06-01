#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "MyContainer.h"

#define MAX_SIZE            0x100000

typedef struct tagArray_
{
	void **      ppData;
	unsigned int nCount;
	unsigned int nMaxCount;
}Array_, *PArray_;

int _Insert2Array( PArray_ pArr_, unsigned int nIndex, void * pData )
{
	if ( pArr_->nCount == pArr_->nMaxCount )
	{
		if ( MAX_SIZE == pArr_->nMaxCount )
		{
			return -1;
		}

		pArr_->nMaxCount *= 2;
		if ( pArr_->nMaxCount > MAX_SIZE )
		{
			pArr_->nMaxCount = MAX_SIZE;
		}

		pArr_->ppData = (void **)realloc( pArr_->ppData, sizeof(void *) * pArr_->nMaxCount );
	}

	if ( nIndex > pArr_->nCount )
	{
		nIndex = pArr_->nCount;
	}

	void ** q = pArr_->ppData;
	memcpy( q + nIndex, q + nIndex + 1, sizeof(void *) * ( pArr_->nCount - nIndex ) );
	q[nIndex] = pData;
	pArr_->nCount++;

	return nIndex;
}

PArray InitArray( unsigned int nMaxSize )
{
	if ( 0 == nMaxSize )
	{
		nMaxSize = 1;
	}

	if ( nMaxSize > MAX_SIZE )
	{
		return NULL;
	}

	PArray_ pArr = (PArray_)malloc( sizeof(Array_) );
	pArr->nCount = 0;

	pArr->ppData  = (void **)malloc( sizeof(void *) * nMaxSize );
	pArr->nMaxCount = nMaxSize;

	return pArr;
}

unsigned int GetArraySize( PArray pArr )
{
	if ( NULL == pArr )
	{
		return 0;
	}
	
	PArray_ p = (PArray_)pArr;

	return p->nCount;
}

int Add2Array( PArray pArr, void * pData )
{
	if ( NULL == pArr )
	{
		return -1;
	}

	PArray_ pArr_ = (PArray_)pArr;

	return _Insert2Array( pArr_, pArr_->nCount, pData );
}

int Insert2Array( PArray pArr, unsigned int nIndex, void * pData )
{
	if ( NULL == pArr )
	{
		return -1;
	}

	PArray_ pArr_ = (PArray_)pArr;

	return _Insert2Array( pArr_, nIndex, pData );
}

void * GetFromArray( PArray pArr, unsigned int nIndex )
{
	if ( NULL == pArr )
	{
		return NULL;
	}

	PArray_ pArr_ = (PArray_)pArr;
	
	if ( nIndex >= pArr_->nCount )
	{
		return NULL;
	}

	void ** q = pArr_->ppData;

	return q[nIndex];
}

int SetArray( PArray pArr, unsigned int nIndex, void * pData )
{
	if ( NULL == pArr )
	{
		return -1;
	}

	PArray_ pArr_ = (PArray_)pArr;

	if ( nIndex >= pArr_->nCount )
	{
		return -1;
	}

	void ** q = pArr_->ppData;
	q[nIndex] = pData;

	return 0;
}

int EraseArray( PArray pArr, unsigned int nIndex )
{
	if ( NULL == pArr )
	{
		return -1;
	}

	PArray_ pArr_ = (PArray_)pArr;

	if ( nIndex >= pArr_->nCount )
	{
		return -1;
	}

	void ** q = pArr_->ppData;
	memcpy( q + nIndex, q + nIndex + 1, sizeof(void *) * ( pArr_->nCount - nIndex - 1 ) );
	pArr_->nCount--;

	return 0;
}

int ClearArray( PArray pArr )
{
	if ( NULL == pArr )
	{
		return -1;
	}

	PArray_ pArr_ = (PArray_)pArr;
	pArr_->nCount = 0;

	return 0;
}

int DeinitArray( PArray pArr )
{
	if ( NULL == pArr )
	{
		return -1;
	}

	PArray_ pArr_ = (PArray_)pArr;

	free( pArr_->ppData );
	free( pArr_ );

	return 0;
}


typedef struct tagListNode_
{
	void *                pData;
	struct tagListNode_ * pPrevious;
	struct tagListNode_ * pNext;
}ListNode_, *PListNode_;

typedef struct tagList_
{
	PListNode_    pHead;
	PListNode_    pTail;
	unsigned int  nCount;
}List_, *PList_;

PList_ _InitList( )
{
	PList_ pList_ = (PList_)malloc( sizeof(List_) );
	memset( pList_, 0, sizeof(List_) );
	return pList_;
}

PList InitList( )
{
	return (PList)_InitList();
}

unsigned int GetListSize( PList pList )
{
	if ( NULL == pList )
	{
		return 0;
	}

	PList_ pList_ = (PList_)pList;

	return pList_->nCount;
}

PListNode_ _Add2ListHead( PList_ pList_, void * pData )
{
	PListNode_ pHead_ = pList_->pHead;
	PListNode_ pNew_  = (PListNode_)malloc( sizeof(ListNode_) );
	pList_->pHead = pNew_;

	pNew_->pPrevious = NULL;
	pNew_->pNext     = pHead_;
	pNew_->pData     = pData;

	if ( NULL != pHead_ )
	{
		pHead_->pPrevious = pNew_;
	}
	else if ( NULL == pList_->pTail )
	{
		pList_->pTail = pNew_;
	}

	pList_->nCount++;
	return pNew_;
}

PListNode Add2ListHead( PList pList, void * pData )
{
	if ( NULL == pList )
	{
		return NULL;
	}
	
	PList_ pList_ = (PList_)pList;

	return (PListNode)_Add2ListHead( pList_, pData );
}

PListNode_ _Add2ListTail( PList_ pList_, void * pData )
{
	PListNode_ pTail_ = pList_->pTail;
	PListNode_ pNew_ = (PListNode_)malloc( sizeof(ListNode_) );
	pList_->pTail = pNew_;

	pNew_->pPrevious = pTail_;
	pNew_->pNext     = NULL;
	pNew_->pData     = pData;

	if ( NULL != pTail_ )
	{
		pTail_->pNext = pNew_;
	}
	else if ( NULL == pList_->pHead )
	{
		pList_->pHead = pNew_;
	}

	pList_->nCount++;

	return pNew_;
}

PListNode Add2ListTail( PList pList, void * pData )
{
	if ( NULL == pList )
	{
		return NULL;
	}

	PList_  pList_  = (PList_)pList;
	return (PListNode)_Add2ListTail( pList_, pData );
}

PListNode GetListHead( PList pList )
{
	if ( NULL == pList )
	{
		return NULL;
	}

	PList_ pList_ = (PList_)pList;
	return (PListNode)pList_->pHead;
}

PListNode GetListTail( PList pList )
{
	if ( NULL == pList )
	{
		return NULL;
	}

	PList_ pList_ = (PList_)pList;
	return (PListNode)pList_->pTail;
}

PListNode GetNextListNode( const PListNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	PListNode_ pNode_ = (PListNode_)pNode;

	return (PListNode)pNode_->pNext;
}

PListNode GetPrevListNode( const PListNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	PListNode_ pNode_ = (PListNode_)pNode;

	return (PListNode)pNode_->pPrevious;
}

PListNode_ _EraseList( PList_ pList_, PListNode_ pNode_ )
{
	PListNode_ pPrev_   = pNode_->pPrevious;
	PListNode_ pNext_   = pNode_->pNext;

	if ( NULL == pPrev_ )
	{
		pList_->pHead = pNext_;
	}
	else
	{
		pPrev_->pNext = pNext_;
	}

	if ( NULL == pNext_ )
	{
		pList_->pTail = pPrev_;
	}
	else
	{
		pNext_->pPrevious = pPrev_;
	}

	free( pNode_ );
	pList_->nCount--;
	return pNext_;
}

PListNode EraseListHead( PList pList )
{
	if ( NULL == pList )
	{
		return NULL;
	}

	PList_  pList_ = (PList_)pList;
	if ( NULL == pList_->pHead )
	{
		return NULL;
	}

	return (PListNode)_EraseList( pList_, pList_->pHead );
}

PListNode EraseListTail( PList pList )
{
	if ( NULL == pList )
	{
		return NULL;
	}

	PList_  pList_ = (PList_)pList;
	if ( NULL == pList_->pTail )
	{
		return NULL;
	}

	return (PListNode)_EraseList( pList_, pList_->pTail );
}

PListNode EraseList( PList pList, PListNode pNode )
{
	if ( NULL == pList )
	{
		return NULL;
	}

	if ( NULL == pNode )
	{
		return NULL;
	}

	PList_ pList_ = (PList_)pList;
	PListNode_ pNode_  = (PListNode_)pNode;

	return (PListNode)_EraseList( pList_, pNode_ );
}

void _ClearList( PList_ pList_ )
{
	PListNode_ pNode_ = pList_->pHead;
	while( pNode_ )
	{
		PListNode_ pTmp_ = pNode_;
		pNode_ = pNode_->pNext;
		free( pTmp_ );
	}

	pList_->pHead = NULL;
	pList_->pTail = NULL;
	pList_->nCount = 0;
}

int ClearList( PList pList )
{
	if ( NULL == pList )
	{
		return -1;
	}
	
	PList_ pList_ = (PList_)pList;
	_ClearList( pList_ );

	return 0;
}

void _DeinitList( PList_ pList_ )
{
	_ClearList( pList_ );
	free( pList_ );
}

int DeinitList( PList pList )
{
	if ( NULL == pList )
	{
		return -1;
	}
	
	_DeinitList( (PList_)pList );

	return 0;
}


typedef struct tagHashtable_
{
	PList_ *        ppList_;
	unsigned int    nListSize;
	unsigned int    nUsedListCnt;
	unsigned int    nListLimit;
	unsigned int    nNodeCnt;
	HashFunc        pfnHash;
	CompFunc        pfnCmp;
}Hashtable_, *PHashtable_;

typedef struct tagHashNode_
{
	void *         pData;
	void *         pKey;
	unsigned int   nHash;
	unsigned int   nIndex;
	PListNode_     pListNode_;
	PHashtable_    pHt_;
}HashNode_, *PHashNode_;

#define MIN_HASH_SIZE         0x100
#define MAX_HASH_SIZE         0x10000

unsigned int _GetHashValue( const PHashtable_ pHt_, void * pKey )
{
	unsigned int nHash = 0;
	if ( pHt_->pfnHash )
	{
		nHash = pHt_->pfnHash( pKey );
	}
	else
	{
		nHash = (unsigned int)pKey;
	}
	return nHash;
}

unsigned int _GetHashIndex( unsigned int nHash, unsigned int nSize )
{
	return nHash % nSize;
	//return ( ( nHash * nHash ) / 2 ) % nSize;
}

int _HashKeyCmp( const PHashtable_ pHt_, void * pKey1, void * pKey2 )
{
	if ( pHt_->pfnCmp )
	{
		return pHt_->pfnCmp( pKey1, pKey2 );
	}
	else
	{
		if ( pKey1 == pKey2 )
		{
			return 0;
		}
		else if ( pKey1 < pKey2 )
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
}

void _ClearHashtable( PHashtable_ pHt_ )
{
	unsigned int i;
	for ( i = 0; i < pHt_->nListSize; i++ )
	{
		PList_ pList_ = pHt_->ppList_[i];
		
		if ( NULL != pList_ )
		{
			PListNode_ pListNode_ = pList_->pHead;
			while( pListNode_ )
			{
				free( pListNode_->pData );
				pListNode_ = pListNode_->pNext;
			}
			_ClearList( pList_ );
		}
	}
}

void _DestroyHashtable( PHashtable_ pHt_ )
{
	unsigned int i;
	for ( i = 0; i < pHt_->nListSize; i++ )
	{
		PList_ pList_ = pHt_->ppList_[i];

		if ( NULL != pList_ )
		{
			PListNode_ pListNode_ = pList_->pHead;
			while( pListNode_ )
			{
				free( pListNode_->pData );
				pListNode_ =pListNode_->pNext;
			}

			_DeinitList( pList_ );
		}
	}

	free( pHt_->ppList_ );
	free( pHt_ );
}

PHashtable_ _InitHashtable( unsigned int nSize, HashFunc pfnHash, CompFunc pfnCmp )
{
	PHashtable_ pHt_ = (PHashtable_)malloc( sizeof(Hashtable_) );
	memset( pHt_, 0, sizeof(Hashtable_) );

	pHt_->ppList_ = (PList_ *)malloc( sizeof(PList_) * nSize );
	memset( pHt_->ppList_, 0, sizeof(PList_) * nSize );

	pHt_->nListSize    = nSize;
	pHt_->nUsedListCnt = 0;
	pHt_->nListLimit   = (unsigned int)( nSize * 0.75f );
	pHt_->nNodeCnt     = 0;
	pHt_->pfnHash      = pfnHash;
	pHt_->pfnCmp       = pfnCmp;

	return pHt_;
}

PHashNode_ _Add2Hashtable( PHashtable_ pHt_, void * pKey, void * pData, unsigned int nHash )
{
	unsigned int nIndex = _GetHashIndex( nHash, pHt_->nListSize );

	if ( NULL == pHt_->ppList_[nIndex] )
	{
		pHt_->ppList_[nIndex] = _InitList();
	}

	PList_ pList_ = pHt_->ppList_[nIndex];

	PHashNode_ pNode_ = (PHashNode_)malloc( sizeof(HashNode_) );
	pNode_->pKey  = pKey;
	pNode_->pData = pData;
	pNode_->nHash = nHash;
	pNode_->nIndex = nIndex;
	pNode_->pHt_   = pHt_;
	
	_Add2ListTail( pList_, pNode_ );

	pNode_->pListNode_ = pList_->pTail;

	if ( 1 == pList_->nCount )
	{
		pHt_->nUsedListCnt++;
	}

	pHt_->nNodeCnt++;

	return pNode_;
}

int _IfHashtableContainKey( const PHashtable_ pHt_, void * pKey, const PList_ pList_ )
{
	if ( NULL == pList_ )
	{
		return 0;
	}

	PListNode_ pListNode_ = pList_->pHead;
	while( pListNode_ )
	{
		PHashNode_ pHashNode_ = (PHashNode_)pListNode_->pData;

		if ( 0 == _HashKeyCmp( pHt_, pKey, pHashNode_->pKey ) )
		{
			return 1;
		}

		pListNode_ = pListNode_->pNext;
	}

	return 0;
}

PHashNode_ _GetHashtableFirstNode( const PHashtable_ pHt_ )
{
	if ( 0 == pHt_->nNodeCnt )
	{
		return NULL;
	}

	unsigned int i;
	for ( i = 0; i < pHt_->nListSize; i++ )
	{
		PList_ pList_ = pHt_->ppList_[i];

		if ( pList_ )
		{
			PListNode_ pListNode_ = pList_->pHead;
			if ( pListNode_ )
			{
				return (PHashNode_)pListNode_->pData;
			}
		}
	}

	return NULL;
}

PHashNode_ _GetNextHashtableNode( const PHashNode_ pNode_ )
{
	PListNode_  pListNode_ = pNode_->pListNode_;
	assert( pListNode_ );

	PListNode_ pNextListNode_ = pListNode_->pNext;
	if ( pNextListNode_ )
	{
		return (PHashNode_)pNextListNode_->pData;
	}

	PHashtable_ pHt_ = pNode_->pHt_;
	assert ( pHt_ );

	unsigned int i;
	for ( i = pNode_->nIndex + 1; i < pHt_->nListSize; i++ )
	{
		PList_ pList_ = pHt_->ppList_[i];
		if ( pList_ )
		{
			pNextListNode_ = pList_->pHead;
			if ( pNextListNode_ )
			{
				return (PHashNode_)pNextListNode_->pData;
			}
		}
	}
	return NULL;
}



PHashtable InitHashtable( unsigned int nSize, HashFunc pfnHash, CompFunc pfnCmp )
{
	if ( nSize < MIN_HASH_SIZE )
	{
		nSize = MIN_HASH_SIZE;
	}

	if ( nSize > MAX_HASH_SIZE )
	{
		return NULL;
	}

	PHashtable_ pHt_ = _InitHashtable( nSize, pfnHash, pfnCmp );

	void ** pAddr = (void **)malloc( sizeof(void *) );
	*pAddr = (void *)pHt_;

	return pAddr;
}

unsigned int GetHashtableSize( PHashtable pHt )
{
	if ( NULL == pHt )
	{
		return 0;
	}

	void ** pAddr = (void **)pHt;
	PHashtable_ pHt_ = (PHashtable_)(*pAddr);

	if ( NULL == pHt_ )
	{
		return 0;
	}

	return pHt_->nNodeCnt;
}

PHashNode Add2Hashtable( PHashtable pHt, void * pKey, void * pData )
{
	if ( NULL == pHt )
	{
		return NULL;
	}

	void ** pAddr = (void **)pHt;
	PHashtable_ pHt_ = (PHashtable_)(*pAddr);
	if ( NULL == pHt_ )
	{
		return NULL;
	}

	unsigned int nHash  = _GetHashValue( pHt_, pKey );
	unsigned int nIndex = _GetHashIndex( nHash, pHt_->nListSize );
	PList_       pList_ = pHt_->ppList_[nIndex];

	if ( _IfHashtableContainKey( pHt_, pKey, pList_ ) )
	{
		return NULL;
	}

	if ( pHt_->nUsedListCnt == pHt_->nListLimit )
	{
		if ( NULL == pList_ || 0 == pList_->nCount )
		{
			if ( pHt_->nListSize < MAX_HASH_SIZE )
			{
				unsigned int nSize = pHt_->nListSize * 2;
				if ( nSize > MAX_HASH_SIZE )
				{
					nSize = MAX_HASH_SIZE;
				}

				PHashtable_ pNew_ = _InitHashtable( nSize, pHt_->pfnHash, pHt_->pfnCmp );

				PHashNode_ pNode_ = _GetHashtableFirstNode( pHt_ );
				while( pNode_ )
				{
					_Add2Hashtable( pNew_, pNode_->pKey, pNode_->pData, pNode_->nHash );
					pNode_ = _GetNextHashtableNode( pNode_ );
				}

				_DestroyHashtable( pHt_ );
				pHt_ = pNew_;
				*pAddr = (void *)pHt_;
			}
		}	
	}

	return (PHashNode)_Add2Hashtable( pHt_, pKey, pData, nHash );
}

int IfHashtableContainKey( PHashtable pHt, void * pKey )
{
	if ( NULL == pHt )
	{
		return 0;
	}

	void ** pAddr = (void **)pHt;
	PHashtable_ pHt_ = (PHashtable_)(*pAddr);
	if ( NULL == pHt_ )
	{
		return 0;
	}
	
	unsigned int nHash = _GetHashValue( pHt_, pKey );

	unsigned int nIndex = _GetHashIndex( nHash, pHt_->nListSize );

	PList_ pList_ = pHt_->ppList_[nIndex];

	return _IfHashtableContainKey( pHt_, pKey, pList_ );
}

int EraseHashtableByKey( PHashtable pHt, void * pKey )
{
    if ( NULL == pHt )
    {
        return 1;
    }

    void ** pAddr = (void **)pHt;
	PHashtable_ pHt_ = (PHashtable_)(*pAddr);

	if ( NULL == pHt_ )
	{
		return 2;
	}

	unsigned int nHash  = _GetHashValue( pHt_, pKey );
	unsigned int nIndex = _GetHashIndex( nHash, pHt_->nListSize ); 

	PList_ pList_ = pHt_->ppList_[nIndex];
	if ( NULL == pList_ )
	{
		return 3;
	}

	PListNode_ pListNode_ = pList_->pHead;
	while( pListNode_ )
	{
		PHashNode_ pNode_ = (PHashNode_)pListNode_->pData;

		if ( 0 == _HashKeyCmp( pHt_, pKey, pNode_->pKey ) )
		{
            EraseHashtable( (PHashNode)pNode_ );
            return 0;
		}

		pListNode_ = pListNode_->pNext;
	}

	return -1;
}

void * GetHashtableValue( PHashtable pHt, void * pKey )
{
	if ( NULL == pHt )
	{
		return NULL;
	}

	void ** pAddr = (void **)pHt;
	PHashtable_ pHt_ = (PHashtable_)(*pAddr);

	if ( NULL == pHt_ )
	{
		return NULL;
	}

	unsigned int nHash  = _GetHashValue( pHt_, pKey );
	unsigned int nIndex = _GetHashIndex( nHash, pHt_->nListSize ); 

	PList_ pList_ = pHt_->ppList_[nIndex];
	if ( NULL == pList_ )
	{
		return NULL;
	}

	PListNode_ pListNode_ = pList_->pHead;
	while( pListNode_ )
	{
		PHashNode_ pNode_ = (PHashNode_)pListNode_->pData;

		if ( 0 == _HashKeyCmp( pHt_, pKey, pNode_->pKey ) )
		{
			return pNode_->pData;
		}

		pListNode_ = pListNode_->pNext;
	}

	return NULL;
}

PHashNode GetHashtableFirstNode( PHashtable pHt )
{
	if ( NULL == pHt )
	{
		return NULL;
	}

	void ** pAddr = (void **)pHt;
	PHashtable_ pHt_ = (PHashtable_)(*pAddr);

	if ( NULL == pHt_ )
	{
		return NULL;
	}

	return (PHashNode)_GetHashtableFirstNode( pHt_ );
}

PHashNode GetNextHashtableNode( const PHashNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	PHashNode_ pNode_ = ( PHashNode_ )pNode;

	return (PHashNode)_GetNextHashtableNode( pNode_ );
}

void * GetHashNodeKey( const PHashNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	PHashNode_ pNode_ = (PHashNode_)pNode;

	return pNode_->pKey;
}

PHashNode EraseHashtable( PHashNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	PHashNode_ pNode_ = (PHashNode_)pNode;
	PListNode_ pListNode_ = pNode_->pListNode_;
	PHashtable_ pHt_ = pNode_->pHt_;
	unsigned int nIndex = pNode_->nIndex;
	PList_ pList_ = pHt_->ppList_[nIndex];

	PHashNode_ pNextNode_ = _GetNextHashtableNode( pNode_ );

	free( pNode_ );
	PListNode_ pNextListNode_ = _EraseList( pList_, pListNode_ );
	pHt_->nNodeCnt--;

	if ( NULL == pNextListNode_ )
	{
		pHt_->nUsedListCnt--;
	}

	return (PHashNode)pNextNode_;
}

int ClearHashtable( PHashtable pHt )
{
	if ( NULL == pHt )
	{
		return -1;
	}

	void ** pAddr = (void **)pHt;
	PHashtable_ pHt_ = (PHashtable_)(*pAddr);
	if ( NULL == pHt_ )
	{
		return -1;
	}

	_ClearHashtable( pHt_ );
	return 0;
}

int DeinitHashtable( PHashtable pHt )
{
	if ( NULL == pHt )
	{
		return -1;
	}

	void ** pAddr = (void **)pHt;
	PHashtable_ pHt_ = (PHashtable_)(*pAddr);
	if ( NULL == pHt_ )
	{
		return -1;
	}

	_DestroyHashtable( pHt_ );

	free( pHt );

	return 0;
}





typedef struct tagTreeNode_
{
	void *                pData;
	struct tagTreeNode_ * pParent;
	struct tagTreeNode_ * pNextSibling;
	struct tagTreeNode_ * pPrevSibling;
	struct tagTreeNode_ * pChild;
	unsigned int          nChildrenCount;
}TreeNode_, *PTreeNode_;

typedef struct tagTree_
{
	PTreeNode_              pRoot;
}Tree_, *PTree_;


void _FreeTreeNode( PTreeNode_ pNode_ )
{
	PTreeNode_ pChild_    = pNode_->pChild;
	PTreeNode_ pCurChild_ = NULL;

	while( pChild_ )
	{
		pCurChild_ = pChild_;
		pChild_    = pChild_->pNextSibling;

		_FreeTreeNode( pCurChild_ );
	}

	free( pNode_ );
}


PTree InitTree( )
{
	PTree_ pTree_  = (PTree_)malloc( sizeof(Tree_) );
	pTree_->pRoot  = NULL;

	return (PTree)pTree_;
}

PTreeNode Insert2TreeRoot( PTree pTree, void * pData )
{
	if ( NULL == pTree )
	{
		return NULL;
	}

	PTree_ pTree_ = (PTree_)pTree;
	if ( NULL != pTree_->pRoot )
	{
		return NULL;
	}

	PTreeNode_ pNode_ = (PTreeNode_)malloc( sizeof(TreeNode_) );
	memset( pNode_, 0, sizeof(TreeNode_) );
	pNode_->pData = pData;

	pTree_->pRoot = pNode_;

	return (PTreeNode)pNode_;
}

// if pAfter is NULL, insert to the 1st position
// if pAfter is LAST_TREE_NODE, insert to the last position
PTreeNode Insert2Tree( PTreeNode pParent, PTreeNode pAfter, void * pData )
{
	if ( NULL == pParent )
	{
		return NULL;
	}

	PTreeNode_ pParent_    = (PTreeNode_)pParent;
	PTreeNode_ pAfter_     = (PTreeNode_)pAfter;

	PTreeNode_ pNode_ = (PTreeNode_)malloc( sizeof(TreeNode_) );
	memset( pNode_, 0, sizeof(TreeNode_) );
	pNode_->pData = pData;
	pNode_->pParent = pParent_;


	if ( NULL == pAfter_ )
	{
		PTreeNode_ pChild_ = pParent_->pChild;

		pNode_->pNextSibling = pChild_;

		if ( pChild_ )
		{
			pChild_->pPrevSibling = pNode_;
		}
		else
		{
			pParent_->pChild = pNode_;
		}
	
		pParent_->nChildrenCount++;	
		return (PTreeNode)pNode_;
	}
	else if ( (PTreeNode_)LAST_TREE_NODE == pAfter_ )
	{
		PTreeNode_ pChild_ = pParent_->pChild;
		PTreeNode_ pLastChild_ = NULL;

		while( pChild_ )
		{
			pLastChild_ = pChild_;
			pChild_ = pChild_->pNextSibling;
		}

		pNode_->pPrevSibling = pLastChild_;
		
		if ( pLastChild_ )
		{
			pLastChild_->pNextSibling = pNode_;
		}
		else
		{
			pParent_->pChild = pNode_;
		}

		pParent_->nChildrenCount++;
		return (PTreeNode)pNode_;
	}
	else
	{
		PTreeNode_ pChild_ = pParent_->pChild;
		while( pChild_ )
		{
			if ( pChild_ == pAfter_ )
			{
				break;
			}

			pChild_ = pChild_->pNextSibling;
		}

		if ( pChild_ )
		{
			PTreeNode_ pNextChild_ = pChild_->pNextSibling;
			
			pChild_->pNextSibling = pNode_;
			pNode_->pPrevSibling  = pChild_;
			pNode_->pNextSibling  = pNextChild_;
			
			if ( pNextChild_ )
			{
				pNextChild_->pPrevSibling = pNode_;
			}

			pParent_->nChildrenCount++;
			return (PTreeNode)pNode_;
		}
		else
		{
			delete pNode_;
			return NULL;
		}
	}
}

PTreeNode GetTreeRoot( PTree pTree )
{
	if ( NULL == pTree )
	{
		return NULL;
	}

	PTree_ pTree_ = (PTree_)pTree;

	return (PTreeNode)pTree_->pRoot;
}

PTreeNode GetTreeNextSibling( const PTreeNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	const PTreeNode_ pNode_ = (const PTreeNode_)pNode;

	return (PTreeNode)pNode_->pNextSibling;
}

PTreeNode GetTreePrevSibling( const PTreeNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	const PTreeNode_ pNode_ = (const PTreeNode_)pNode;

	return (PTreeNode)pNode_->pPrevSibling;
}

PTreeNode GetTreeChild ( const PTreeNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	const PTreeNode_ pNode_ = (const PTreeNode_)pNode;

	return (PTreeNode)pNode_->pChild;
}

PTreeNode GetTreeParent( const PTreeNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	const PTreeNode_ pNode_ = (const PTreeNode_)pNode;

	return (PTreeNode)pNode_->pParent;
}

PTreeNode GetTreeNextNode( const PTreeNode pNode )
{
	if ( NULL == pNode )
	{
		return NULL;
	}

	const PTreeNode_ pNode_ = (const PTreeNode_)pNode;

	PTreeNode_ pNext_ = pNode_->pChild;
	if ( pNext_ )
	{
		return (PTreeNode)pNext_;
	}
	else
	{
		pNext_ = pNode_->pNextSibling;
		if ( pNext_ )
		{
			return (PTreeNode)pNext_;
		}
		else
		{
			PTreeNode_ pParent_ = pNode_->pParent;
			while( pParent_ )
			{
				pNext_ = pParent_->pNextSibling;
				if ( pNext_ )
				{
					return (PTreeNode)pNext_;
				}
				pParent_ = pParent_->pParent;
			}
			return NULL;
		}
	}
}

// return next tree node in the traversing method
PTreeNode EraseTreeNode( PTree pTree, PTreeNode pNode )
{
	if ( NULL == pTree )
	{
		return NULL;
	}

	if ( NULL == pNode )
	{
		return NULL;
	}
	
	PTree_     pTree_ = (PTree_)pTree;
	PTreeNode_ pNode_ = (PTreeNode_)pNode;

	if ( pNode_ == pTree_->pRoot )
	{
		pTree_->pRoot = NULL;

		_FreeTreeNode( pNode_ );

		return NULL;
	}
	else
	{
		PTreeNode_ pParent_      = pNode_->pParent;
		PTreeNode_ pNextSibling_ = pNode_->pNextSibling;
		PTreeNode_ pPrevSibling_ = pNode_->pPrevSibling;

		if ( pPrevSibling_ )
		{
			pPrevSibling_->pNextSibling = pNextSibling_;
		}
		else
		{
			pParent_->pChild = pNextSibling_;
		}

		if ( pNextSibling_ )
		{
			pNextSibling_->pPrevSibling = pPrevSibling_;
		}

		pParent_->nChildrenCount--;

		_FreeTreeNode( pNode_ );

		if ( pNextSibling_ )
		{
			return (PTreeNode)pNextSibling_;
		}
		else
		{
			while( pParent_ )
			{
				PTreeNode_ pNext_ = pParent_->pNextSibling;
				if ( pNext_ )
				{
					return (PTreeNode)pNext_;
				}
				pParent_ = pParent_->pParent;
			}
			return NULL;
		}
	}
}

void _ClearTree ( PTree_ pTree_ )
{
	if ( pTree_->pRoot )
	{
		_FreeTreeNode( pTree_->pRoot );
		pTree_->pRoot = NULL;
	}
}

int ClearTree ( PTree pTree )
{
	if ( NULL == pTree )
	{
		return -1;
	}

	PTree_ pTree_ = (PTree_)pTree;
	_ClearTree( pTree_ );
	
	return 0;
}

int DeinitTree( PTree pTree )
{
	if ( NULL == pTree )
	{
		return -1;
	}

	PTree_ pTree_ = (PTree_)pTree;
	_ClearTree( pTree_ );
	free( pTree_ );

	return 0;
}

int _TraverseTreeNode( PTreeNode_ pNode_, PFunTraverseTreeNode pfn, void * pParam )
{
	int iRet = pfn( (PTreeNode)pNode_, pParam );
	if ( 0 == iRet )
	{
		return 0 ;
	}

	PTreeNode_ pChild_ = pNode_->pChild;
	while( pChild_ )
	{
		iRet = pfn( (PTreeNode)pChild_, pParam );
		if ( 0 == iRet )
		{
			return 0;
		}

		pChild_ = pChild_->pNextSibling;
	}

	return 1;
}

void TraverseTreeNode( PTreeNode pNode, PFunTraverseTreeNode pfn, void * pParam )
{
	if ( NULL == pNode )
	{
		return;
	}

	if ( NULL == pfn )
	{
		return;
	}

	_TraverseTreeNode( (PTreeNode_)pNode, pfn, pParam );
}


