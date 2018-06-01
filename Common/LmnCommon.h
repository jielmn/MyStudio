#ifndef  _LEMON_COMMON_2012_01_20_
#define  _LEMON_COMMON_2012_01_20_


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the COMMON_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// COMMONLMN_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WIN32

#ifdef COMMON_EXPORTS
#define COMMONLMN_API __declspec(dllexport)
#else
#define COMMONLMN_API __declspec(dllimport)
#endif

#else

#define COMMONLMN_API 

#endif


#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif



// ��ֹ��windows��ͷ�ļ������ͻ
#ifndef _WINDEF_ 
typedef int                 BOOL;
typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef unsigned char       BYTE;
#endif // _WINDEF_ 



// �Ƚ����׳�ͻ�ĺ궨��
#ifdef  TRUE
#undef  TRUE
#endif
#define TRUE  1

#ifdef  FALSE
#undef  FALSE
#endif
#define FALSE  0

#ifdef  IN
#undef  IN
#endif
#define IN

#ifdef  OUT
#undef  OUT
#endif
#define OUT

#ifdef  INOUT
#undef  INOUT
#endif
#define INOUT




//  ���峣�õĴ���꣬����һЩ�����ķ���ֵ(�����ֵ������100)
#define  LMN_ERR_OK                    0      // ��ȷ
#define  LMN_ERR_INVALID_ARGUMENT      1      // ��Ч����
#define  LMN_ERR_NOT_ENOUGH_BUFF       2      // û���㹻������
#define  LMN_ERR_NO_MEMORY             3      // �����ڴ�ʧ��
#define  LMN_ERR_FAILED_TO_OPEN_FILE   4      // ���ļ�ʧ��
#define  LMN_ERR_NOT_INITED            5      // û�г�ʼ��
#define  LMN_ERR_ALREADY_INITED        6      // �Ѿ���ʼ��
#define  LMN_ERR_INVALID_HANDLE        7      // ��Ч���
#define  LMN_ERR_NO_RESOURCE           8      // û����Դ
#define  LMN_ERR_ALREADY_STARTED       9      // �Ѿ�����
#define  LMN_ERR_NOT_STARTED           10     // û������
#define  LMN_ERR_INVALID_FORMAT        11     // ����ĸ�ʽ



#define  LMN_ERR_INVALID_FILE_DESCRIPTION  100     // ��Ч��fd
#define  LMN_ERR_IOCTL                     101     // ioctl����
#define  LMN_ERR_CLOSE                     102     // close����
#define  LMN_ERR_SHUTDOWN                  103     // shutdown����
#define  LMN_ERR_SEND                      104     // send����
#define  LMN_ERR_RECV                      105     // recv����
#define  LMN_ERR_CONNECT                   106     // connect����
#define  LMN_ERR_LISTEN                    107     // listen����
#define  LMN_ERR_SELECT                    108     // select����



#define  LMN_ERR_UNKNOWN                (-1)       // δ֪����(ͨ��)


// ����Stringģ�����ʼ������
#define  LMN_ERR_STRING_BASE                    1000
#define  LMN_ERR_WRONG_FORMAT_FOR_NUMBER        LMN_ERR_STRING_BASE
#define  LMN_ERR_BASE64_LENGTH_INVALID          (LMN_ERR_STRING_BASE + 1)
#define  LMN_ERR_WRONG_BASE64_FORMAT            (LMN_ERR_STRING_BASE + 2)


#define  LMN_ERR_CONTAINER_BASE                 1200
#define  LMN_ERR_NO_SUCH_KEY                    LMN_ERR_CONTAINER_BASE


#define  LMN_ERR_MISC_BASE                      1400  
#define  LMN_ERR_CONSOLE_MENU_NOT_INITIALIZED   LMN_ERR_MISC_BASE    
#define  LMN_ERR_WRONG_FILE_PATH_FORMAT         (LMN_ERR_MISC_BASE + 1)
#define  LMN_ERR_CONSOLE_MENU_ALREADY_INITED    (LMN_ERR_MISC_BASE + 2)
#define  LMN_ERR_NO_SUCH_CONSOLE_MENU           (LMN_ERR_MISC_BASE + 3)
#define  LMN_ERR_NO_SUCH_CONSOLE_MENU_ITEM      (LMN_ERR_MISC_BASE + 4)
#define  LMN_ERR_FAILED_TO_CREATE_SEMPHORE      (LMN_ERR_MISC_BASE + 5)
#define  LMN_ERR_FAILED_TO_WAIT_SEMPHORE        (LMN_ERR_MISC_BASE + 6)
#define  LMN_ERR_FAILED_TO_RELEASE_SEMPHORE     (LMN_ERR_MISC_BASE + 7)
#define  LMN_ERR_FAILED_TO_DESTROY_SEMPHORE     (LMN_ERR_MISC_BASE + 8)
#define  LMN_ERR_INVALID_THREAD_HANDLE          (LMN_ERR_MISC_BASE + 9)
#define  LMN_ERR_TOO_MUCH_DELAY_TIME            (LMN_ERR_MISC_BASE + 10)


#define  LMN_ERR_SELI_BASE                      1600 
#define  LMN_ERR_NOT_SELI_SOCKET                LMN_ERR_SELI_BASE


#define  LMN_ERR_FILE_UTILS_BASE                2000
#define  LMN_ERR_FILE_EXISTS                    LMN_ERR_FILE_UTILS_BASE
#define  LMN_ERR_FILE_DOES_NOT_EXIST            (LMN_ERR_FILE_UTILS_BASE + 1)
#define  LMN_ERR_FOLDER_EXISTS                  (LMN_ERR_FILE_UTILS_BASE + 2)



#ifdef __cplusplus
extern "C" {
#endif



typedef void * (*AllocMemCb)( size_t dwSize );
typedef void   (*FreeMemCb)( void *  pMemblock );

/****************************************************************************
 * ��������  SetMemoryCallbackFunc                                          *
 * ��  �ܣ�  �����Զ�����ڴ�ص�����                                       *
 * ˵  ����  ��ѡ��Ĭ�ϵ���ϵͳ��malloc��free                               *
 ****************************************************************************/
COMMONLMN_API int SetMemoryCallbackFunc( AllocMemCb pfnAlloc, FreeMemCb pfnFree);





/****************************************************************************
 * ��������  strtrim                                                        *
 * ��  �ܣ�  ��ȥ�ַ���ǰ��Ŀհ��ַ�(' ', '\t', '\r'��'\n')                *
 * ����ֵ��  0          ʧ��                                                *
 *           ��0        �ü�����ַ���                                      *
 ****************************************************************************/
COMMONLMN_API char * StrTrim( INOUT char * szStr );


/****************************************************************************
 * ��������  Str2Lower                                                      *
 * ��  �ܣ�  ת��ΪСд�ַ���                                               *
 * ����ֵ��  0          ʧ��                                                *
 *           ��0        ת������ַ���                                      *
 ****************************************************************************/
COMMONLMN_API char *   Str2Lower( INOUT char * szStr );


/****************************************************************************
 * ��������  Str2Upper                                                      *
 * ��  �ܣ�  ת��Ϊ��д�ַ���                                               *
 * ����ֵ��  0          ʧ��                                                *
 *           ��0        ת������ַ���                                      *
 ****************************************************************************/
COMMONLMN_API char *   Str2Upper( INOUT char * szStr );


/****************************************************************************
 * ��������  StrReplace                                                     *
 * ��  �ܣ�  ���ַ���szReplaceWith�滻�ַ���szSource�е�����                *
 *           ΪszToReplace�ĵط�                                            *
 * ����ֵ��  0          �ɹ�                                                *
 *           ��0        ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API int StrReplace( OUT char * szDest, IN DWORD dwSize, 
				              IN const char * szSource,  
                              IN const char * szToReplace, 
				              IN const char * szReplaceWith );


/****************************************************************************
 * ��������  Str2Int                                                        *
 * ��  �ܣ�  ת���ַ���(10���ƻ�16����)Ϊ����(�з��ŵ�)                     *
 * ����ֵ��  0          �ɹ�                                                *
 *           ��0        ʧ��                                                *
 * ˵����    ת����Χ -2147483648 ~ 2147483647                              *
 *           ����16���� -0x80000000~0x7FFFFFFF                              *
 ****************************************************************************/
COMMONLMN_API int Str2Int( IN const char * szNum, OUT int * pnNum );


/****************************************************************************
 * ��������  EncodeBase64                                                   *
 * ��  �ܣ�  ���ֽ���ת����base64��ʽ                                       *
 * ����ֵ��  0          �ɹ�                                                *
 *           ��0        ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API int EncodeBase64( OUT   void  *  pBase64, 
                                INOUT DWORD *  pdwSize, 
                                IN    void  *  pSrc, 
                                IN    DWORD    dwSrcLen );


/****************************************************************************
 * ��������  DecodeBase64                                                   *
 * ��  �ܣ�  ��base64��ʽת�����ֽ���                                       *
 * ����ֵ��  0          �ɹ�                                                *
 *           ��0        ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API int DecodeBase64( OUT   void  * pDest, 
                                INOUT DWORD * pdwSize, 
                                IN    void  * pBase4, 
                                IN    DWORD   dwBase64Len  );




// ת��ΪСд��ĸ
COMMONLMN_API  char  Char2Lower( IN   char ch );
// ת��Ϊ��д��ĸ
COMMONLMN_API  char  Char2Upper( IN   char ch );
// �ַ����Ƚ�(���Ӵ�Сд)
COMMONLMN_API  int   StrICmp ( IN const char * s1, IN const char * s2 );
// ǰN���ַ����Ƚ�(���Ӵ�Сд)
COMMONLMN_API  int   StrNiCmp( IN const char * s1, IN const char * s2, IN DWORD dwCnt );














/****************************************************************************/
/*                        ARRAY(����)                                       */
/****************************************************************************/



typedef void * PArray;

/****************************************************************************
 * ��������  InitArray                                                      *
 * ��  �ܣ�  ��ʼ������                                                     *
 * ����ֵ��  ��0        �ɹ�                                                *
 *           0          ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API  PArray  InitArray  ( IN DWORD  dwMaxSize );


/****************************************************************************
 * ��������  GetArraySize                                                   *
 * ��  �ܣ�  ��ȡ�����С                                                   *
 * ����ֵ��  �����С                                                       *
 ****************************************************************************/
COMMONLMN_API  DWORD   GetArraySize  ( IN PArray pArray );


/****************************************************************************
 * ��������  Append2Array                                                   *
 * ��  �ܣ�  ������׷�ӵ�����ĩβ                                           *
 * ����ֵ��  �ɹ��� ������ݵ�������ʧ�ܣ�-1                                *
 ****************************************************************************/
COMMONLMN_API  DWORD  Append2Array  ( IN PArray pArray,  IN void * pData );


/****************************************************************************
 * ��������  Insert2Array                                                   *
 * ��  �ܣ�  ��������뵽��������λ��                                       *
 * ����ֵ��  �ɹ��� ������ݵ�������ʧ�ܣ�-1                                *
 ****************************************************************************/
COMMONLMN_API  DWORD  Insert2Array( IN PArray pArray, IN DWORD dwIndex, 
                                    IN void * pData );


/****************************************************************************
 * ��������  GetFromArra                                                    *
 * ��  �ܣ�  ȡ��������λ�õ�����                                           *
 * ����ֵ��  �ɹ��� ���ݣ�ʧ�ܣ�0                                           *
 ****************************************************************************/
COMMONLMN_API  void * GetFromArray( IN PArray pArray,  IN DWORD dwIndex );


/****************************************************************************
 * ��������  SetArray                                                       *
 * ��  �ܣ�  ������������λ�õ�����                                         *
 * ����ֵ��  �ɹ��� TRUE��ʧ�ܣ�FALSE                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL SetArray( IN PArray pArray, IN DWORD dwIndex, 
                              IN void * pData );


/****************************************************************************
 * ��������  EraseArray                                                     *
 * ��  �ܣ�  ������������λ�õ�����                                         *
 * ����ֵ��  �ɹ��� TRUE��ʧ�ܣ�FALSE                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL EraseArray( IN PArray pArray, IN DWORD dwIndex );


/****************************************************************************
 * ��������  ClearArray                                                     *
 * ��  �ܣ�  �����������е�����                                             *
 * ����ֵ��  �ɹ��� TRUE��ʧ�ܣ�FALSE                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL  ClearArray( IN PArray pArray );


/****************************************************************************
 * ��������  DeinitArray                                                    *
 * ��  �ܣ�  ��������                                                       *
 * ����ֵ��  �ɹ��� TRUE��ʧ�ܣ�FALSE                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL  DeinitArray( IN PArray pArray );












/****************************************************************************/
/*                        LIST(�б�)                                        */
/****************************************************************************/


typedef void * PList;


typedef struct tagListNode
{
	void *               pData;
}ListNode, *PListNode;



/****************************************************************************
 * ��������  InitList                                                       *
 * ��  �ܣ�  ��ʼ���б�                                                     *
 * ����ֵ��  ��0        �ɹ�                                                *
 *           0          ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API  PList  InitList( );



/****************************************************************************
 * ��������  GetListSize                                                    *
 * ��  �ܣ�  ��ȡ�б������                                                 *
 ****************************************************************************/
COMMONLMN_API  DWORD  GetListSize( IN PList pList );



/****************************************************************************
 * ��������  Insert2ListHead                                                *
 * ��  �ܣ�  ��������ӵ��б�ͷ                                             *
 * ����ֵ��  �ɹ��������ӵ�node��ʧ�ܣ�0                                    *
 ****************************************************************************/
COMMONLMN_API  PListNode  Insert2ListHead( IN PList pList, IN void * pData );


/****************************************************************************
 * ��������  Insert2ListTail                                                *
 * ��  �ܣ�  ��������ӵ��б�β                                             *
 * ����ֵ��  �ɹ��������ӵ�node��ʧ�ܣ�0                                    *
 ****************************************************************************/
COMMONLMN_API  PListNode  Insert2ListTail( IN PList pList, IN void * pData );


/****************************************************************************
 * ��������  Insert2List                                                    *
 * ��  �ܣ�  ��������ӵ��б�����λ��                                       *
 * ����ֵ��  �ɹ��������ӵ�node��ʧ�ܣ�0                                    *
 ****************************************************************************/
const PListNode LMN_FIRST_LIST_NODE = 0;
const PListNode LMN_LAST_LIST_NODE  = (PListNode)(-1);
COMMONLMN_API  PListNode  Insert2List( IN PList pList, IN void * pData, IN PListNode pAfterNode );

COMMONLMN_API  PListNode  Insert2ListEx( IN PList pList, IN void * pData, IN PListNode pBefoereNode );



/****************************************************************************
 * ��������  GetListHead                                                    *
 * ��  �ܣ�  ��ȡ�б��ͷ                                                   *
 * ����ֵ��  �ɹ���Node��ʧ�ܣ�0                                            *
 ****************************************************************************/
COMMONLMN_API  PListNode GetListHead( IN PList pList );


/****************************************************************************
 * ��������  GetListTail                                                    *
 * ��  �ܣ�  ��ȡ�б��β                                                   *
 * ����ֵ��  �ɹ���Node��ʧ�ܣ�0                                            *
 ****************************************************************************/
COMMONLMN_API  PListNode GetListTail( IN PList pList );


/****************************************************************************
 * ��������  GetNextListNode                                                *
 * ��  �ܣ�  ��ȡ�б����һ��Node                                           *
 * ����ֵ��  �ɹ���Node��ʧ�ܣ�0                                            *
 ****************************************************************************/
COMMONLMN_API  PListNode GetNextListNode( IN const PListNode pNode );


/****************************************************************************
 * ��������  GetPrevListNode                                                *
 * ��  �ܣ�  ��ȡ�б����һ��Node                                           *
 * ����ֵ��  �ɹ���Node��ʧ�ܣ�0                                            *
 ****************************************************************************/
COMMONLMN_API  PListNode GetPrevListNode( IN const PListNode pNode );


/****************************************************************************
 * ��������  EraseListHead                                                  *
 * ��  �ܣ�  �����б�ͷ                                                     *
 * ����ֵ��  �ɹ�����һ��Node��ʧ�ܣ�0                                      *
 ****************************************************************************/
COMMONLMN_API  PListNode EraseListHead( IN PList pList );


/****************************************************************************
 * ��������  EraseListTail                                                  *
 * ��  �ܣ�  �����б�β                                                     *
 * ����ֵ��  0                                                              *
 ****************************************************************************/
COMMONLMN_API  PListNode EraseListTail( IN PList pList );



/****************************************************************************
 * ��������  EraseList                                                      *
 * ��  �ܣ�  ����һ��Node                                                   *
 * ����ֵ��  �ɹ�����һ��Node��ʧ�ܣ�0                                      *
 ****************************************************************************/
COMMONLMN_API  PListNode EraseList( IN PList pList, IN PListNode pNode );



/****************************************************************************
 * ��������  ClearList                                                      *
 * ��  �ܣ�  ����б����е�Node                                             *
 ****************************************************************************/
COMMONLMN_API  BOOL  ClearList( IN PList pList );


/****************************************************************************
 * ��������  DeinitList                                                     *
 * ��  �ܣ�  �����б�                                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL  DeinitList( IN PList pList );














#ifdef __cplusplus
} // extern "C" { 
#endif

























/****************************************************************************/
/*                        TREE(��/ɭ��)                                     */
/****************************************************************************/

typedef struct tagTreeNode
{
	void *    pData;
}TreeNode, *PTreeNode;


const PTreeNode LMN_FIRST_TREE_NODE = 0;
const PTreeNode LMN_LAST_TREE_NODE  = (PTreeNode)(-1);



/****************************************************************************
 * ��������  InitTree                                                       *
 * ��  �ܣ�  ��ʼ����                                                       *
 * ����ֵ��  �ɹ�����0��ʧ�ܣ�0                                             *
 ****************************************************************************/
COMMONLMN_API  PTreeNode InitTree( void * pData );


/****************************************************************************
 * ��������  Insert2Tree                                                    *
 * ��  �ܣ�  ���������ڵ�                                                 *
 * ����ֵ��  �ɹ����µĽڵ㣻ʧ�ܣ�0                                        *
 ****************************************************************************/
COMMONLMN_API  PTreeNode Insert2Tree( IN PTreeNode pParent, IN void * pData, 
									  IN PTreeNode pAfter = LMN_LAST_TREE_NODE );

COMMONLMN_API  PTreeNode Insert2TreeEx( IN PTreeNode pParent, IN void * pData, 
                                        IN PTreeNode pBefore = LMN_LAST_TREE_NODE );


/****************************************************************************
 * ��������  GetParentTreeNode                                              *
 * ��  �ܣ�  ��ȡ���ڵ�                                                     *
 * ����ֵ��  �ɹ�����0��ʧ�ܣ�0                                             *
 ****************************************************************************/
COMMONLMN_API  PTreeNode  GetParentTreeNode( IN PTreeNode pTreeNode );


// ��ȡ��һ���ֵܽڵ�
// ����ֵ���ɹ�����0
COMMONLMN_API  PTreeNode  GetNextSiblingTreeNode( IN PTreeNode pTreeNode );

// ��ȡǰһ���ֵܽڵ�
// ����ֵ���ɹ�����0
COMMONLMN_API  PTreeNode  GetPrevSiblingTreeNode( IN PTreeNode pTreeNode );

// ��ȡ��һ���ӽڵ�
// ����ֵ���ɹ�����0
COMMONLMN_API  PTreeNode  GetChildTreeNode( IN PTreeNode pTreeNode );

// ��ȡ��N�����ӽڵ�, index��0��ʼ
// ����ֵ���ɹ�����0
COMMONLMN_API  PTreeNode  GetChildTreeNodeByIndex( IN PTreeNode pTreeNode, DWORD dwIndex );

// ɾ���ڵ�
// ������ҽڵ㣬�����ҽڵ㡣������ڵ����ҽڵ㣬�����ҽڵ㣬�Դ�����
// �������ı���(�����������)
COMMONLMN_API  PTreeNode  EraseTreeNode( IN PTreeNode pTreeNode );

// ������
COMMONLMN_API  void   DeInitTree( PTreeNode pTreeRoot );

// ��������ʱ��ʹ��
// ��root��ʼ�����ǲ�����root
// �������(�������ӣ�����)
COMMONLMN_API  PTreeNode  GetNextTreeNode( PTreeNode pTreeNode );


















/****************************************************************************/
/*                        Hashtable                                         */
/****************************************************************************/


typedef void * PHashtable;


typedef struct tagHashNode
{
	void * pData;
}HashNode, *PHashNode;


// �Զ���hash����
typedef DWORD (*HashFunc)( void * pKey );

// �Զ�������keyֵ�ȽϽ��
// ����ֵ 
//     0�����
//     1��ke1  > key2
//    -1��key1 < key2
typedef int  (*CompFunc)( void * pKey1,void * pKey2 );

// ��ʼ��һ��Hashtable
// CAUSION�����pfnCmp��Ϊ0����pfnHashҲ���벻Ϊ0
COMMONLMN_API  PHashtable   InitHashtable( DWORD dwSize = 0, 
										   HashFunc pfnHash = 0, 
										   CompFunc pfnCmp = 0 );

// ��ȡHashtable��ǰ�ж��ٸ��ڵ�
COMMONLMN_API  DWORD        GetHashtableSize( PHashtable pHt );

// ��ӽڵ㵽Hashtable
// �������ͬ��key�Ľڵ㣬�����ʧ��
COMMONLMN_API  PHashNode    Add2Hashtable( PHashtable pHt, const void * pKey, const void * pData );

// ��ӽڵ㵽Hashtable
// �������ͬ��key�Ľڵ㣬���޸ĸýڵ�Value
// �������ppOldData��Ϊ0�������޸ĵ�����·��ؾ�ֵ
COMMONLMN_API  PHashNode    SetHashtable( PHashtable pHt, const void * pKey, const void * pData, void ** ppOldData = 0 );

// Hashtable�Ƿ����key
COMMONLMN_API  BOOL         IfHashtableContainKey( PHashtable pHt, const void * pKey );

// ����key��ȡhashtable��ĳ��ֵ
COMMONLMN_API  BOOL         GetHashtableValue( PHashtable pHt, const void * pKey, void ** ppValue );

// ��ȡhashtable�ĵ�һ��ֵ(�����ڱ���)
COMMONLMN_API  PHashNode    GetHashtableFirstNode( PHashtable pHt );

// ��ȡhastbable����һ��ֵ(�����ڱ���)
COMMONLMN_API  PHashNode    GetNextHashtableNode( const HashNode * pNode );

// ����node��ȡkey(�����ڱ���)
COMMONLMN_API  void *       GetHashNodeKey( const HashNode * pNode );

// ɾ��hashtable��ĳ��(���û�����)
// ����ֵ��ɾ�������һ��
COMMONLMN_API  PHashNode    EraseHashtable( PHashNode pNode );

// ����keyɾ��hashtable��ĳ��
COMMONLMN_API  int          EraseHashtableByKey( PHashtable pHt, const void * pKey );

// ���hashtable
COMMONLMN_API  int          ClearHashtable( PHashtable pHt );

// ����hashtable
COMMONLMN_API  int          DeinitHashtable( PHashtable pHt );















/****************************************************************************/
/*                        Misc                                              */
/****************************************************************************/


typedef struct tagPoint
{
	float x;
	float y;
}Point, * PPoint;


/************************************************************************/
// ����: �ж�һ�����Ƿ���һ���������
/************************************************************************/
COMMONLMN_API BOOL IsPointInPolygon( const Point *  ptPoint,  
									 const Point atPolygon[],  
									 DWORD dwVertexCount );



/***********************************************************************/
// ����: �鿴�ֽ����������ʾ�ֽ�ֵ���ұ���ʾ�ַ�
// ����: bDosCarriageReturn,  �س�������\n����\r\n
//       bShowAnsi,  ���ֲ����Ƿ���ansi��ʽ��ʾ(��ʾ����)
/***********************************************************************/
COMMONLMN_API int DebugStream(  char *       pchDebugBuf,  DWORD  dwDebugBufLen, 
				                const BYTE * pbyStream,    DWORD  dwStreamLen,
				                BOOL         bDosCarriageReturn = FALSE,
                                BOOL         bShowAnsi = FALSE );



/***********************************************************************/
// ����: ��ȡһ�������
/***********************************************************************/
COMMONLMN_API DWORD GetRand( DWORD dwMin, DWORD dwMax );

COMMONLMN_API void  InitRand( BOOL bCustomSrand = FALSE, DWORD dwSrand = 0 );







// UI example:
//               TEST
// 1. add                           1. back
// 2. sub                           2. do something
// 3. multiply            ==>       3. quit
// 4. divide
// 5. next menu                     input your choice:
// 
// input your choice: 
//


typedef  void *  ConsoleMenuHandle;


#define  MAX_CONSOLE_MENU_ITEM_NAME_SIZE         80
typedef  struct  tagConsoleMenuItem
{
    char                  szName[MAX_CONSOLE_MENU_ITEM_NAME_SIZE];     // �˵�item����
    ConsoleMenuHandle     hMenu;                                       // ��Ϊ0ʱ����ʾ��ת����һ���˵�
}TConsoleMenuItem, *PTConsoleMenuItem;

// �û��Զ����ѡ��ĳ��˵���Ļص�����
typedef void  (*HandleChoiceCb)( ConsoleMenuHandle hMenu,  DWORD dwIndex );


// ѡ��ǰ�˵����ĸ�ѡ��
// ����ֵ: QUIT_CONSOLE_MENU, �˳�ConsoleMenuϵͳ
//         0 ~ N��Ӧ��ǰ�˵�������
#define QUIT_CONSOLE_MENU                        ((DWORD)-1)
typedef DWORD (*SelectChoiceCb)( ConsoleMenuHandle hMenu, const char * szChoice );



// ��ʼ���˵�ϵͳ
COMMONLMN_API int InitConsole( SelectChoiceCb pfnSelect, HandleChoiceCb  pfnHandleChoice );

// �����Ӳ˵�
COMMONLMN_API ConsoleMenuHandle  CreateConsoleMenu( const char * szTitle );

// ��Ӳ˵���(��ĩβ)
COMMONLMN_API int AddConsoleMenuItem( ConsoleMenuHandle hConsoleMenu,  const TConsoleMenuItem * pMenuItem );

// ��ʾĳ���Ӳ˵�
// ע�⣺��һ��InitConsole��DeinitConsole֮��ֻ׼����һ��
COMMONLMN_API int  DisplayConsoleMenu( ConsoleMenuHandle hConsoleMenu );

// ȥ��ʼ���˵�ϵͳ
COMMONLMN_API int  DeinitConsole();











/***********************************************************************/
// ����: �����ļ�·������ȡ�ļ�����Ŀ¼���ļ�������չ��  
/***********************************************************************/
COMMONLMN_API int  ParseFileName( const char * szFilePath, 
					char * szFileDirectory = 0,  DWORD  dwDirLen = 0,
                    char * szFileName = 0,       DWORD dwNameLen = 0,
                    char * szExtention = 0,      DWORD dwExtLen = 0 );









/*************************       ������   **************************************/
#ifdef WIN32
typedef   CRITICAL_SECTION    LmnLockType;
#else
typedef   pthread_mutex_t     LmnLockType;
#endif

COMMONLMN_API int  LmnInitLock  ( LmnLockType * pLock );
COMMONLMN_API int  LmnDeinitLock( LmnLockType * pLock );
COMMONLMN_API int  LmnLock( LmnLockType * pLock );
COMMONLMN_API int  LmnUnlock( LmnLockType * pLock );
/*************************    END ������   **************************************/




/*************************    �ź���      ************************************/
typedef void * LmnSemphoreType;

// ������semphore
COMMONLMN_API  int  LmnSemCreate(LmnSemphoreType * pSemphore, DWORD dwInitCnt, DWORD dwMaxCnt);
COMMONLMN_API  int  LmnSemTake(LmnSemphoreType semphore);
COMMONLMN_API  int  LmnSemTakeByTime(LmnSemphoreType semphore, DWORD dwWaitMiniSeconds);
COMMONLMN_API  int  LmnSemGive(LmnSemphoreType semphore);
COMMONLMN_API  int  LmnSemDestroy(LmnSemphoreType semphore);
/*************************   END �ź���    ************************************/



/*************************    �߳�      *************************************/
COMMONLMN_API  DWORD  LmnGetCurrentThrdId();

// ɾ����ʱ��
typedef void (*OnThrdDataEraseCb)( void * pKey,  void * pValue );
// Set��ʱ�����ݸı�(key����)
typedef void (*OnThrdDataChangeCb)( void * pKey,  void * pValue );

// init thread local storage module
// �ڲ���hashtableʵ�֣����Կ�����Ҫhash������key�ȽϺ���
COMMONLMN_API  int   LmnInitThrdTls( HashFunc           pfnHash = 0,     // hash(key)
                                     CompFunc           pfnComp = 0,     // compare(key)
                                     OnThrdDataEraseCb  pfnOnErase = 0,  // hash item ��ɾ��
                                     OnThrdDataChangeCb pfnOnChange = 0 ); // hash item ���޸�

COMMONLMN_API  int   LmnSetThrdData( const void * pKey,  const void * pValue );
COMMONLMN_API  int   LmnGetThrdData( const void * pKey,  void ** ppValue );
COMMONLMN_API  int   LmnEraseThrdData( const void * pKey );

COMMONLMN_API  int   LmnDeinitThrdTls();




#ifdef WIN32
typedef HANDLE          LmnThrdType;
#else
typedef pthread_t       LmnThrdType;
#endif

typedef  void * (*LmnThreadFunc)(void *);

// ����ֵ�� �ɹ�����0
COMMONLMN_API  LmnThrdType LmnCreateThread( LmnThreadFunc pfnThrd, void * pParam, DWORD dwStackSize, BOOL bDetached = FALSE );
COMMONLMN_API  void *      LmnWait4Thrd( LmnThrdType hThrd );

/*************************   END �߳�    ************************************/










/*************************    RADVISION��ʽ Э��ջ   *************************************/

#define liEvRead    0x01
#define liEvWrite   0x02
#define liEvExcept  0x04
#define liEvAccept  0x08
#define liEvConnect 0x10
#define liEvClose   0x20       // �Զ˹ر�
#define liEvDestroy 0x40       // ����close�ر�

typedef int FdType;
typedef void(*LPLIEVENTHANDLER)( FdType fd, int liEvent, int error, void * context );

typedef enum
{
    LI_PROTOTYPE_UDP,
    LI_PROTOTYPE_TCP
}liProtocol_t;

#define liStInited          0
#define liStConnecting      1
#define liStListening       2
#define liStShutdowned      3
#define liStAvailable       4

typedef struct tagKeepAliveParam
{
    BOOL     bTcpKeepAlive;                       
    DWORD    dwIdleTime;                    // ��λ����
    DWORD    dwIntervalTimve;               // ��λ����

#ifndef WIN32 
    DWORD    dwCount;                       // WIN32����£�keepalive�Ĵ��������޸ģ�Ĭ��5��
#endif
}TKeepAliveParam, *PTKeepAliveParam;

enum  ESock5Method
{
    SOCK5_METHOD_NONE     = 0,
    SOCK5_METHOD_USER
};


#define  MAX_SOCK5_USERNAME_LEN                       32
#define  MAX_SOCK5_PASSWORD_LEN                       32

typedef struct tagCnnSock5Param
{
    BOOL            bUseSock5;

    DWORD           dwDestAddr;
    WORD            wDestPort;
    char            reserverd0[2];

    DWORD           dwSock5Addr;
    WORD            wSock5Port;
    char            reserverd1[2];
    DWORD           dwTimeOut;

    LPLIEVENTHANDLER eventHandler;
    void *           context;

    ESock5Method    eMethod;

    char            szUserName[MAX_SOCK5_USERNAME_LEN];
    char            szPassword[MAX_SOCK5_PASSWORD_LEN];

}TCnnSock5Param, *PTCnnSock5Param;



COMMONLMN_API  int  seliInit( DWORD dwMaxSocketCnt );
COMMONLMN_API  int  seliEnd( );

// select��·����
// nMs��λ�� ���� (20~1000)
COMMONLMN_API  int  seliSelect( DWORD * pdwMs );

// ����ֵ�� socket
// -1��ʧ��
COMMONLMN_API  FdType  liOpen ( liProtocol_t protocol, DWORD ipAddr, WORD wPort, 
                                LPLIEVENTHANDLER eventHandler, void* context, 
                                const TKeepAliveParam * pKeepAliveParam );

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int  liClose ( FdType fd );

// dwTimeOut ��λ:��
// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int  liConnect ( int fd, DWORD ipAddr, WORD wPort, DWORD dwTimeOut );

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int liListen(int fd, int queueLen );

// ����ֵ�� socket
COMMONLMN_API  FdType liAccept(int fd, LPLIEVENTHANDLER fnCallBack, void * context);

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int liShutdown( int fd );

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int liTcpSend (int fd, const char *buff, int * plen );

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int liTcpRecv (int fd, char *buff,int * plen);

// ����ֵ��
//          -1 ERROR
COMMONLMN_API  int liGetState (int fd);

// ע�⣺���յ��ر��¼���ʱ���ٵ���ʱ��linux��������Ч
COMMONLMN_API  int liGetSockName(int fd, DWORD * pdwIp, WORD * pwPort, BOOL bPeer = FALSE );

// ����sock5����
// �������bUseSock5ΪTrue����ʹ��sock5 proxy
// ���߲�ʹ��sock5 proxy
// 
// ˵���� lxConnectSock5 ������liOpen�����Բ���Ҫ�ٵ���liOpen
COMMONLMN_API  FdType  liConnectSock5 ( const TCnnSock5Param *  ptParam );


/*************************    END RADVISION��ʽ Э��ջ   *************************************/








/************************     RADVISION��ʽ Э��ջ ��չ *************************************/
typedef  void *     SelixNode;

typedef void(*LPLIXEVENTHANDLER)( SelixNode pSeli, FdType fd, int liEvent, int error, void * context );

COMMONLMN_API  int          selixInit( DWORD dwTotalMaxSocketCnt );
COMMONLMN_API  int          selixEnd( );
COMMONLMN_API  BOOL         isSelixInited( );

// ���pdwMaxSocketCntΪ0������Ϊ����ʹ������ʣ�µ�MaxSocketCount
COMMONLMN_API  SelixNode    selixCreateNode( INOUT DWORD * pdwMaxSocketCnt = 0 );

COMMONLMN_API  int          selixDestroyNode( SelixNode pSeli );


// select��·����
// nMs��λ�� ���� (20~1000)
COMMONLMN_API  int  selixSelect( SelixNode pSeli, DWORD * pdwMs );

// ����ֵ�� socket
COMMONLMN_API  FdType  lixOpen ( SelixNode pSeli, liProtocol_t protocol, DWORD ipAddr, WORD wPort, 
                                    LPLIXEVENTHANDLER eventHandler, void* context, 
                                    const TKeepAliveParam * pKeepAliveParam );

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int  lixClose ( SelixNode pSeli, FdType fd );

// dwTimeOut ��λ:��
// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int  lixConnect ( SelixNode pSeli,  int fd, DWORD ipAddr, WORD wPort, DWORD dwTimeOut );

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int lixListen( SelixNode pSeli, int fd, int queueLen );

// ����ֵ�� socket
COMMONLMN_API  FdType lixAccept( SelixNode pSeli, int fd, LPLIXEVENTHANDLER fnCallBack, void * context);

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int lixShutdown( SelixNode pSeli, int fd );

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int lixTcpSend ( SelixNode pSeli, int fd, const char *buff, int * plen );

// ���ֻ����һ���֣�sleepһ��ʱ��( 0==dwSleeptime����sleep )�ٷ��ͣ�ֱ�﷢�����ʧ��
COMMONLMN_API  int lixTcpCompleteSend ( SelixNode pSeli, int fd, const char *buff, int * plen, DWORD dwSleepTime );

// ����ֵ�� 0  OK
//          -1 ERROR
COMMONLMN_API  int lixTcpRecv ( SelixNode pSeli, int fd, char *buff,int * plen);

// ����ֵ��
//          -1 ERROR
COMMONLMN_API  int lixGetState ( SelixNode pSeli, int fd);


COMMONLMN_API  int lixGetSockName( SelixNode pSeli, int fd, DWORD * pdwIp, WORD * pwPort );

// ����sock5����
// �������bUseSock5ΪTrue����ʹ��sock5 proxy
// ���߲�ʹ��sock5 proxy
// 
// ˵���� lxConnectSock5 ������liOpen�����Բ���Ҫ�ٵ���liOpen


typedef struct tagCnnSock5ParamX
{
    BOOL            bUseSock5;

    DWORD           dwDestAddr;
    WORD            wDestPort;
    char            reserverd0[2];

    DWORD           dwSock5Addr;
    WORD            wSock5Port;
    char            reserverd1[2];
    DWORD           dwTimeOut;

    LPLIXEVENTHANDLER eventHandler;
    void *           context;

    ESock5Method    eMethod;

    char            szUserName[MAX_SOCK5_USERNAME_LEN];
    char            szPassword[MAX_SOCK5_PASSWORD_LEN];

}TCnnSock5ParamX, *PTCnnSock5ParamX;

COMMONLMN_API  FdType  lixConnectSock5 ( SelixNode pSeli, const TCnnSock5ParamX *  ptParam );


/************************  END RADVISION��ʽ Э��ջ ��չ *************************************/
















/****************************************************************************
 * ��������  Utf8ToAnsi                                                     *
 * ��  �ܣ�  utf8�����ΪAnsi����                                           *
 * ����ֵ��  ��0        �ɹ�                                                *
 *           0          ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API char * Utf8ToAnsi( char * szAnsi, unsigned int dwAnsiSize, const char * szUtf8 );




/****************************************************************************
 * ��������  AnsiToUtf8                                                     *
 * ��  �ܣ�  Ansi����תΪutf8����                                           *
 * ����ֵ��  ��0        �ɹ�                                                *
 *           0          ʧ��                                                *
 ****************************************************************************/
COMMONLMN_API char * AnsiToUtf8( char * szUtf8, unsigned int dwUtf8Size, const char * szAnsi );








/*************************    ʱ��      ********************************/
COMMONLMN_API DWORD   LmnGetTickCount();                       //��λ������
COMMONLMN_API void    LmnSleep( DWORD dwMiliSeconds );         //��λ������
/*************************  END ʱ��   **********************************/



/*************************    Post Message Thread    ********************************/
typedef struct tagLmxMsgData
{
    DWORD   dwMsgId;
    void *  pMsg;
    DWORD   dwMsgLen;
}LmxMsgData, *PLmxMsgData;


typedef  void *  LmnThrdHandle;
// ����ֵ�� TRUE, ����ִ����Ϣѭ���� FALSE���˳���Ϣѭ��
typedef  BOOL (*POnThreadMessage)( LmnThrdHandle h, const LmxMsgData * pMsgData, void * pContext );


// ����ĳЩ�߳�(�������߳�)������Ҫ�����߳��ˡ��ڶ�����������ΪFALSE�������߳����ã��൱��Ϊ���̴߳���һ����Ϣ���У����ǲ��������߳�
COMMONLMN_API   LmnThrdHandle   LmxCreateThread( POnThreadMessage pfnOnMessage, BOOL bCreateThrd = TRUE, void * pContext = 0, BOOL bDetach = FALSE );
COMMONLMN_API   int             LmxDestroyThread( LmnThrdHandle h );

// ���ڵ���LmxCreateThreadʱ�ڶ�������ΪFALSE��������������ͣ�Ļ�ȡ��Ϣ
COMMONLMN_API   int             LmxThreadRun( LmnThrdHandle h );

COMMONLMN_API   int             LmxPostMessage( LmnThrdHandle h, const LmxMsgData * pMsgData );
// ��ʱʱ���
COMMONLMN_API   int             LmxPostDelayMessage( LmnThrdHandle h, const LmxMsgData * pMsgData, DWORD dwDelayedMs );
/*************************  END Post Message Thread    ******************************/













//�汾��
#define  GENERATE_NUMERIC_VERSION4(a,b,c,d)         ((((char)(a))<<24)|(((char)(b))<<16)|(((char)(c))<<8)|((char)(d)))
#define  GENERATE_NUMERIC_VERSION3(b,c,d)           ((((char)(b))<<16)|(((char)(c))<<8)|((char)(d)))
#define  GENERATE_NUMERIC_VERSION2(c,d)             ((((char)(c))<<8)|((char)(d)))

COMMONLMN_API  DWORD GenerateNumericVersion( DWORD dwCnt, ... );

enum EVersionType
{
    VERSION_TYPE4 = 0,
    VERSION_TYPE3,
    VERSION_TYPE2
};

// ��ȡ�ַ�����ʽ��version
// *.*.*.* �� *.*.* �� *.*
COMMONLMN_API  char *  LmnGetVersion( INOUT char * szVersion, IN DWORD dwVersionSize, IN DWORD dwVersion, IN EVersionType eVer = VERSION_TYPE4 );
// ��ȡ���θ�ʽ��version
COMMONLMN_API  DWORD   LmnGetVersion( IN const char * szVersion );








/***************************************** telnetd *******************************************/

typedef void(* LPOnTelnetCmd )( const char * szCmd, const char * aszParams[], DWORD dwParamsCnt, DWORD dwClientId );

#define   MAX_TELNETD_COMMAND_NAME_LEN          32
#define   MAX_TELNETD_HELP_LANE                 128

typedef struct tagRegTelnetdCmd
{
	char      szCmdName[MAX_TELNETD_COMMAND_NAME_LEN];
	char      szHelp[MAX_TELNETD_HELP_LANE];
}TRegTelnetdCmd, *PTRegTelnetdCmd;

#define   MAX_TELNET_CO_USERS_COUNT             5


// ��ռ�ȵã����Ǻ�ռ��ã� ���ٹر�telnet���ڣ��п��ܻص�������Printf���������һ��telnet����
//enum ETenetdMethod
//{
//    TELNETD_METHOD_FORMMER_PRIORITY,
//    TELNETD_METHOD_LATTER_PRIORITY
//};


// ����: ����telnet����
// ������ �˿ڣ� ע��������������� ����Ļص��� ͬʱ�������ϵ��û�����
// ˵���� ������selixInit
//        ȫ��ֻ����һ��telnet����
COMMONLMN_API  int  LmnStartTelnetd( WORD wPort, const TRegTelnetdCmd * atCmds, DWORD dwCmdCnt, 
									 LPOnTelnetCmd pfnOnCommand, DWORD dwMaxCoUsersCnt );


COMMONLMN_API  int  LmnStopTelnetd( );


COMMONLMN_API  int  LmnTelPrintf( DWORD dwClientId, const char * szFormat, ... );

COMMONLMN_API  int  LmnTelPrintf0( DWORD dwClientId, const char * szMsg );



/************************************ end telnetd *****************************************/







COMMONLMN_API  int  Unicode2Utf8( IN DWORD dwUnicode, INOUT BYTE * pOutput, INOUT DWORD * pdwOutputSize, BOOL  bLittleEndian = TRUE );

COMMONLMN_API  int  UnicodeStream2Utf8Stream( IN const WORD * pUnicode, IN DWORD dwUnicodeSize, 
                                              INOUT BYTE * pOutput, INOUT DWORD * pdwOutputSize, BOOL  bLittleEndian = TRUE );

// ת����һ����Utf8�ַ�
COMMONLMN_API  int  Utf8ToUnicode( IN const BYTE * pUtf8, INOUT DWORD * pdwUtf8Size, OUT DWORD * pdwUnicode,  BOOL bLittleEndian = TRUE );

COMMONLMN_API  int  Utf8Stream2UnicodeStream( IN const BYTE * pUtf8, IN DWORD dwUtf8Size, 
                                              OUT WORD * pwUnicode, INOUT DWORD * pdwUnicodeSize, BOOL bLittleEndian = TRUE );





enum ELmnFileType
{
    E_FILE_TYPE_NONE,
    E_FILE_TYPE_FILE,
    E_FILE_TYPE_FOLDER
};


// ����Ŀ¼(bIgnoreExisted, �������Ŀ¼�Ƿ���Ըô���)
COMMONLMN_API  int  LmnCreateFolder( IN const char * szPathName, BOOL bIgnoreExisted = TRUE );

// ���ļ�(��Ҫ����win32�µ�������/·���ļ�)
COMMONLMN_API  FILE * LmnOpenFile ( IN const char * szFile, IN const char * szMode );

COMMONLMN_API  DWORD LmnReadFile ( void * buffer, DWORD size, DWORD count, FILE * stream );

COMMONLMN_API  DWORD LmnWriteFile ( const void * buffer, DWORD size, DWORD count, FILE * stream );

COMMONLMN_API  void LmnCloseFile( FILE * fp );


// ��ȡ�ļ�����(Ŀ¼/�ļ�)
COMMONLMN_API  ELmnFileType  LmnGetFileType( IN const char * szPathName );

// ɾ���ļ�/Ŀ¼
COMMONLMN_API  int   LmnDeleteFile( IN const char * szPathName );

// �����ļ�/Ŀ¼
// szNewPathName, ���ƺ��ȫ·���������Ǹ�·����
COMMONLMN_API  int   LmnCopyFile( IN const char * szOldPathName, IN const char * szNewPathName, IN BOOL bOverWrite = TRUE );

// �ƶ��ļ�/Ŀ¼
// szNewPathName, �ƶ����ȫ·���������Ǹ�·����
COMMONLMN_API  int   LmnMoveFile( IN const char * szOldPathName, IN const char * szNewPathName, IN BOOL bOverWrite = TRUE );

// ��ȡ�ļ�/Ŀ¼��С
COMMONLMN_API  DWORD  LmnGetFileSize( IN const char * szPathName );












//////////////////////      �ļ�/�ļ���ת��Ϊ���������ļ�/�ļ��д���(����TCP)     ///////////////////////////


#define  LMN_MAX_FILE_PATH                        340

typedef  void *  HFileStream;


typedef  struct  tagFileStreamPos
{
    ELmnFileType   eType;
    // ��utf8��ʽ����·����
    char           szSubPath[LMN_MAX_FILE_PATH];
    // ������ļ��Ļ�����ǰ�ļ��Ĵ�С
    long           nPos;
}TFileStreamPos, *PTFileStreamPos;


// ���ļ�/�ļ�����
// ����Ƕ�����������eType��bOverWrite����
COMMONLMN_API  HFileStream  OpenFileStream( const char * szPathName, BOOL bRead, ELmnFileType eType = E_FILE_TYPE_FILE, 
                                            BOOL bOverWrite = FALSE, TFileStreamPos * ptPos = 0 );

// ���õ�ǰλ��(���ڶ�����)
COMMONLMN_API  int  SetFileStreamPos( IN HFileStream h, IN const TFileStreamPos * pPosition );

// ��ȡ(���ڶ�����)
// pdwBufSize���������������buf��size���Լ�����buf�Ĵ�С
// pdwReadFileSize��ȡ�ļ�(�������ļ���)�����ݵĴ�С
COMMONLMN_API  int ReadFileStream( IN HFileStream h, IN void * buf, INOUT DWORD * pdwBufSize, OUT DWORD * pdwReadFileSize = 0 );

// д��(����д����)
// pdwBufSize���������������buf�Ĵ�С���Լ���Чʹ�õ��ֽ�
// pdwWriteFileSize���������������ʵ��д����ļ���С(�������ļ���)
COMMONLMN_API  int WriteFileStream( IN HFileStream h, IN const void * buf, INOUT DWORD * pdwBufSize, OUT DWORD * pdwWriteFileSize = 0 );


COMMONLMN_API int CloseFileStream( IN HFileStream h );



COMMONLMN_API  int  GetFileStreamPos( IN HFileStream h, IN TFileStreamPos * pPosition );

























namespace LmnCommon
{
    /*******************************************************************************/
    // ����: �鿴����(char|unsigned char|short|unsigned short|int|unsigned int��
    //       �Ƿ�������ĳλ��λ��������0��ʼ��
    // ����ֵ: 
    //          TRUE,  ������
    //          FALSE, û������(���������)
    /*******************************************************************************/
    template <class T>
    bool IfHasBit(T num, BYTE index)
    {
	    if ( index >= sizeof(T) * 8 )
	    {
		    return false;
	    }

	    if ( 0 == ( num & ((T)1<<index) ) )
	    {
		    return false;
	    }
	    else
	    {
		    return true;
	    }
    }

    /*******************************************************************************/
    // ����ĳλ��ֵ                                                        
    /*******************************************************************************/
    template <class T>
    void SetBit(T& num, BYTE index, bool bSet = true)
    {
	    if ( index >= sizeof(T) * 8 )
	    {
		    return;
	    }

	    if ( bSet )
	    {
		    num |= (((T)0x01)<<(index));
	    }
	    else
	    {
		    num &= (~(((T)0x01)<<(index)));
	    }
    }


    class  CDataBuf
    {
    public:
        COMMONLMN_API  CDataBuf( const void * pData = 0, DWORD dwDataLen = 0 );
        COMMONLMN_API  ~CDataBuf();
        COMMONLMN_API  BOOL Append( const void * pData, DWORD dwDataLen );                        // д�����ݵ�ĩβ
        COMMONLMN_API  void Clear();

        COMMONLMN_API  BOOL Read( void * pData, DWORD dwDataSize = -1 );                    // �ӵ�ǰ��λ�ÿ�ʼ��ȡ����(����������ȡ����)
        COMMONLMN_API  void ResetReadPos();                                                 // �Ѷ�����Ϊ0
        COMMONLMN_API  BOOL SetReadPos( DWORD dwPos );

        COMMONLMN_API  DWORD         GetDataLength() const;
        COMMONLMN_API  const void *  GetData() const;
        COMMONLMN_API  DWORD         GetReadPos() const;


        // ��ǰ����Ѷ����ڴ�ȥ����δ�����ڴ���ǰ�ƶ����������Ժ�����ٷ���ռ�Ŀ�����
        COMMONLMN_API  void         Reform();

    private:
        // ���Զ���ķ���������Ҫ���ʵ�ʷ����ڴ��С
        COMMONLMN_API  static DWORD CalcReqBufLen( DWORD dwReqLen );

        BYTE *   m_pData;                                       // ����
        DWORD    m_dwDataBufLen;                                // ����buf����
        DWORD    m_dwDataLen;                                   // ��Ч���ݳ���
        DWORD    m_dwRPos;                                      // ��ǰ����λ��
    };


    class  CFuncLock
    {
    public:
          COMMONLMN_API  CFuncLock( LmnLockType * pLock );
          COMMONLMN_API  ~CFuncLock();
          COMMONLMN_API  void UnLock();
          COMMONLMN_API  void Lock();

    private:
          LmnLockType *  m_pLock;
          BOOL           m_bLocked;
    };



    // ѭ���洢items���������޵�item���������item
    // M��ֵ��Ҫ����̫��(����ջ�ڴ�)
    template <class T, const DWORD M>
    class  CRecycledItems
    {
    public:
        CRecycledItems()
        {
            m_dwBeginPos = 0;
            m_dwEndPos   = 0;
        }

        ~CRecycledItems()
        {
            Clear();
        }

        DWORD GetCount() const
        {
            if ( m_dwEndPos < m_dwBeginPos )
            {
                return m_dwEndPos + (M+1) - m_dwBeginPos;
            }
            else
            {
                return m_dwEndPos - m_dwBeginPos;
            }
        }

        void Append( const T & t )
        {
            m_items[m_dwEndPos] = t;
            m_dwEndPos++;

            // ��������������ޣ�����end posΪ0
            if ( m_dwEndPos > M )
            {
                m_dwEndPos = 0;
            }

            // ���begin == end���ƶ�begin pos
            if ( m_dwEndPos == m_dwBeginPos )
            {
                m_dwBeginPos++;
                // ���begin pos�������ޣ�����begin posΪ0
                if ( m_dwBeginPos > M )
                {
                    m_dwBeginPos = 0;
                }
            }  
        }

        void Clear()
        {
            m_dwBeginPos = 0;
            m_dwEndPos   = 0;
        }

        BOOL IsFull() const
        {
            return GetCount() == M ? TRUE : FALSE; 
        }

        // example:
        // for ( int i = 0; i < items.GetCount(); i++ )
        // {
        //     items[i];
        // }
        const T & operator[]( DWORD dwIndex ) const
        {
            if ( dwIndex >= GetCount() )
            {
                throw -1;
            }

            dwIndex += m_dwBeginPos;
            dwIndex %= M + 1;

            return m_items[dwIndex];
        }


    private:
        T        m_items[M + 1];
        DWORD    m_dwBeginPos;                                 // ��ʼλ��
        DWORD    m_dwEndPos;                                   // ����λ��
    };




    // �̶��������޵�items
    // M��ֵ��Ҫ����̫��(����ջ�ڴ�)
    template <class T, const DWORD M>
    class  CFixedSizeItems
    {
    public:
        CFixedSizeItems()
        {
            memset( m_bUsed, 0, sizeof(m_bUsed) );
        }

        ~CFixedSizeItems(){}


        // �ҵ�һ����λ�洢һ��item
        DWORD  AddItem( const T & t )
        {
            DWORD i;
            for ( i = 0; i < M; i++ )
            {
                // ����п�λ
                if ( !m_bUsed[i] )
                {
                    break;
                }
            }
            
            // ȫ����û�п�λ
            if ( i >= M )
            {
                return (DWORD)-1;
            }

            m_items[i] = t;
            m_bUsed[i] = TRUE;

            return i;
        }

        // ��ĳ��λ������һ��item
        BOOL  SetItem( const T & t, DWORD dwIndex )
        {
            if ( dwIndex >= M )
            {
                return FALSE;
            }

            m_items[dwIndex] = t;
            m_bUsed[dwIndex] = TRUE;
        }

        BOOL  DeleteItem( DWORD dwIndex )
        {
            if ( dwIndex >= M )
            {
                return FALSE;
            }

            if ( !m_bUsed[dwIndex] )
            {
                return FALSE;
            }

            m_bUsed[dwIndex] = FALSE;
            return TRUE;
        }


        DWORD  GetFirstItem() const
        {
            DWORD i;
            for ( i = 0; i < M; i++ )
            {
                if ( m_bUsed[i] )
                {
                    return i;
                }
            }

            return (DWORD)-1;
        }

        DWORD  GetNextItem( DWORD dwPos ) const
        {
            DWORD i;
            for ( i = dwPos + 1; i < M; i++ )
            {
                if ( m_bUsed[i] )
                {
                    return i;
                }
            }

            return (DWORD)-1;
        }

        const T & operator[]( DWORD dwIndex ) const
        {
            if ( dwIndex >= M )
            {
                throw -1;
            }

            if ( !m_bUsed[dwIndex] )
            {
                throw -1;
            }

            return m_items[dwIndex];
        }

    private:
        T        m_items[M];
        BOOL     m_bUsed[M];                // ��Ӧ��items�Ƿ�ʹ��(δ��ʹ�õģ�������Ϊ�ռ�洢�µ�item)
    };

} // namespace LmnCommon













#endif // _LEMON_COMMON_2012_01_20_




