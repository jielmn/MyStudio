#ifndef __MY_CONTAINER_2010_08_10__
#define __MY_CONTAINER_2010_08_10__

///////////////////////////////////////////////////////////////
// array data container
///////////////////////////////////////////////////////////////
typedef void * PArray;

PArray InitArray( unsigned int nMaxSize );
unsigned int GetArraySize( PArray pArr );

// return value: index, -1 means error
int  Add2Array( PArray pArr, void * pData );
// return value: index, -1 means error
int  Insert2Array( PArray pArr, unsigned int nIndex, void * pData );

// if error occurs, return NULL
void * GetFromArray( PArray pArr, unsigned int nIndex );
int  SetArray( PArray pArr, unsigned int nIndex, void * pData );
int  EraseArray( PArray pArr, unsigned int nIndex );
int  ClearArray( PArray pArr );
int  DeinitArray( PArray pArr );


//////////////////////////////////////////////////////////////////
// list container
//////////////////////////////////////////////////////////////////
typedef void * PList;
typedef struct tagListNode
{
	void *               pData;
}ListNode, *PListNode;

PList InitList( );
unsigned int GetListSize( PList pList );
PListNode  Add2ListHead( PList pList, void * pData );
PListNode  Add2ListTail( PList pList, void * pData );
// if error occurs, return NULL
PListNode GetListHead( PList pList );
// if error occurs, return NULL
PListNode GetListTail( PList pList );
// if error occurs, return NULL
PListNode GetNextListNode( const PListNode pNode );
// if error occurs, return NULL
PListNode GetPrevListNode( const PListNode pNode );

// return value: next list node;
// if error occurs, return NULL
PListNode EraseListHead( PList pList );
// return value: next list node;
// if error occurs, return NULL
PListNode EraseListTail( PList pList );
// return value: next list node;
// if error occurs, return NULL
PListNode EraseList( PList pList, PListNode pNode );

int  ClearList( PList pList );
int  DeinitList( PList pList );


///////////////////////////////////////////////////////////////
// hashtable container
///////////////////////////////////////////////////////////////
typedef void * PHashtable;
typedef struct tagHashNode
{
	void * pData;
}HashNode, *PHashNode;

typedef unsigned int (*HashFunc)( void * pKey );
typedef int (*CompFunc)( void * pKey1, void * pKey2 );

PHashtable InitHashtable( unsigned int nSize, HashFunc pfnHash, CompFunc pfnCmp );
unsigned int GetHashtableSize( PHashtable pHt );
PHashNode Add2Hashtable( PHashtable pHt, void * pKey, void * pData );
int IfHashtableContainKey( PHashtable pHt, void * pKey );
void * GetHashtableValue( PHashtable pHt, void * pKey );
PHashNode GetHashtableFirstNode( PHashtable pHt );
PHashNode GetNextHashtableNode( const PHashNode pNode );
void * GetHashNodeKey( const PHashNode pNode );
PHashNode EraseHashtable( PHashNode pNode );
int EraseHashtableByKey( PHashtable pHt, void * pKey );
int ClearHashtable( PHashtable pHt );
int DeinitHashtable( PHashtable pHt );



//////////////////////////////////////////////////////////////////
// tree container
/////////////////////////////////////////////////////////////////
#define LAST_TREE_NODE    (PTreeNode)(0xFFFFFFFF)

typedef void * PTree;
typedef struct tagTreeNode
{
	void *    pData;
}TreeNode, *PTreeNode;


PTree InitTree( );

PTreeNode Insert2TreeRoot( PTree pTree, void * pData );
// if pAfter is NULL, insert to the 1st position
// if pAfter is LAST_TREE_NODE, insert to the last position
PTreeNode Insert2Tree( PTreeNode pParent, PTreeNode pAfter, void * pData );

PTreeNode GetTreeRoot( PTree pTree );
PTreeNode GetTreeNextSibling( const PTreeNode pNode );
PTreeNode GetTreePrevSibling( const PTreeNode pNode );
PTreeNode GetTreeChild ( const PTreeNode pNode );
PTreeNode GetTreeParent( const PTreeNode pNode );

// for traversing method
PTreeNode GetTreeNextNode( const PTreeNode pNode );

// return next tree node in the traversing method
PTreeNode EraseTreeNode( PTree pTree, PTreeNode pNode );

int  ClearTree ( PTree pTree );
int  DeinitTree( PTree pTree );

typedef int (*PFunTraverseTreeNode)( PTreeNode pNode, void * pParam );
// don't delete tree node in the callback function
void TraverseTreeNode( PTreeNode pNode, PFunTraverseTreeNode pfn, void * pParam );

#endif

