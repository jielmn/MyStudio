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



// 防止和windows的头文件定义冲突
#ifndef _WINDEF_ 
typedef int                 BOOL;
typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef unsigned char       BYTE;
#endif // _WINDEF_ 



// 比较容易冲突的宏定义
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




//  定义常用的错误宏，用于一些函数的返回值(最大数值不超过100)
#define  LMN_ERR_OK                    0      // 正确
#define  LMN_ERR_INVALID_ARGUMENT      1      // 无效参数
#define  LMN_ERR_NOT_ENOUGH_BUFF       2      // 没有足够缓冲区
#define  LMN_ERR_NO_MEMORY             3      // 分配内存失败
#define  LMN_ERR_FAILED_TO_OPEN_FILE   4      // 打开文件失败
#define  LMN_ERR_NOT_INITED            5      // 没有初始化
#define  LMN_ERR_ALREADY_INITED        6      // 已经初始化
#define  LMN_ERR_INVALID_HANDLE        7      // 无效句柄
#define  LMN_ERR_NO_RESOURCE           8      // 没有资源
#define  LMN_ERR_ALREADY_STARTED       9      // 已经启动
#define  LMN_ERR_NOT_STARTED           10     // 没有启动
#define  LMN_ERR_INVALID_FORMAT        11     // 错误的格式



#define  LMN_ERR_INVALID_FILE_DESCRIPTION  100     // 无效的fd
#define  LMN_ERR_IOCTL                     101     // ioctl错误
#define  LMN_ERR_CLOSE                     102     // close错误
#define  LMN_ERR_SHUTDOWN                  103     // shutdown错误
#define  LMN_ERR_SEND                      104     // send错误
#define  LMN_ERR_RECV                      105     // recv错误
#define  LMN_ERR_CONNECT                   106     // connect错误
#define  LMN_ERR_LISTEN                    107     // listen错误
#define  LMN_ERR_SELECT                    108     // select错误



#define  LMN_ERR_UNKNOWN                (-1)       // 未知错误(通用)


// 定义String模块的起始错误码
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
 * 函数名：  SetMemoryCallbackFunc                                          *
 * 功  能：  设置自定义的内存回调函数                                       *
 * 说  明：  可选，默认的是系统的malloc和free                               *
 ****************************************************************************/
COMMONLMN_API int SetMemoryCallbackFunc( AllocMemCb pfnAlloc, FreeMemCb pfnFree);





/****************************************************************************
 * 函数名：  strtrim                                                        *
 * 功  能：  除去字符串前后的空白字符(' ', '\t', '\r'和'\n')                *
 * 返回值：  0          失败                                                *
 *           非0        裁剪后的字符串                                      *
 ****************************************************************************/
COMMONLMN_API char * StrTrim( INOUT char * szStr );


/****************************************************************************
 * 函数名：  Str2Lower                                                      *
 * 功  能：  转化为小写字符串                                               *
 * 返回值：  0          失败                                                *
 *           非0        转化后的字符串                                      *
 ****************************************************************************/
COMMONLMN_API char *   Str2Lower( INOUT char * szStr );


/****************************************************************************
 * 函数名：  Str2Upper                                                      *
 * 功  能：  转化为大写字符串                                               *
 * 返回值：  0          失败                                                *
 *           非0        转化后的字符串                                      *
 ****************************************************************************/
COMMONLMN_API char *   Str2Upper( INOUT char * szStr );


/****************************************************************************
 * 函数名：  StrReplace                                                     *
 * 功  能：  用字符串szReplaceWith替换字符串szSource中的所有                *
 *           为szToReplace的地方                                            *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 ****************************************************************************/
COMMONLMN_API int StrReplace( OUT char * szDest, IN DWORD dwSize, 
				              IN const char * szSource,  
                              IN const char * szToReplace, 
				              IN const char * szReplaceWith );


/****************************************************************************
 * 函数名：  Str2Int                                                        *
 * 功  能：  转换字符串(10进制或16进制)为数字(有符号的)                     *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 * 说明：    转换范围 -2147483648 ~ 2147483647                              *
 *           或者16进制 -0x80000000~0x7FFFFFFF                              *
 ****************************************************************************/
COMMONLMN_API int Str2Int( IN const char * szNum, OUT int * pnNum );


/****************************************************************************
 * 函数名：  EncodeBase64                                                   *
 * 功  能：  把字节流转换成base64格式                                       *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 ****************************************************************************/
COMMONLMN_API int EncodeBase64( OUT   void  *  pBase64, 
                                INOUT DWORD *  pdwSize, 
                                IN    void  *  pSrc, 
                                IN    DWORD    dwSrcLen );


/****************************************************************************
 * 函数名：  DecodeBase64                                                   *
 * 功  能：  把base64格式转换成字节流                                       *
 * 返回值：  0          成功                                                *
 *           非0        失败                                                *
 ****************************************************************************/
COMMONLMN_API int DecodeBase64( OUT   void  * pDest, 
                                INOUT DWORD * pdwSize, 
                                IN    void  * pBase4, 
                                IN    DWORD   dwBase64Len  );




// 转化为小写字母
COMMONLMN_API  char  Char2Lower( IN   char ch );
// 转化为大写字母
COMMONLMN_API  char  Char2Upper( IN   char ch );
// 字符串比较(忽视大小写)
COMMONLMN_API  int   StrICmp ( IN const char * s1, IN const char * s2 );
// 前N个字符串比较(忽视大小写)
COMMONLMN_API  int   StrNiCmp( IN const char * s1, IN const char * s2, IN DWORD dwCnt );














/****************************************************************************/
/*                        ARRAY(数组)                                       */
/****************************************************************************/



typedef void * PArray;

/****************************************************************************
 * 函数名：  InitArray                                                      *
 * 功  能：  初始化数组                                                     *
 * 返回值：  非0        成功                                                *
 *           0          失败                                                *
 ****************************************************************************/
COMMONLMN_API  PArray  InitArray  ( IN DWORD  dwMaxSize );


/****************************************************************************
 * 函数名：  GetArraySize                                                   *
 * 功  能：  获取数组大小                                                   *
 * 返回值：  数组大小                                                       *
 ****************************************************************************/
COMMONLMN_API  DWORD   GetArraySize  ( IN PArray pArray );


/****************************************************************************
 * 函数名：  Append2Array                                                   *
 * 功  能：  把数组追加到数组末尾                                           *
 * 返回值：  成功， 添加数据的索引；失败，-1                                *
 ****************************************************************************/
COMMONLMN_API  DWORD  Append2Array  ( IN PArray pArray,  IN void * pData );


/****************************************************************************
 * 函数名：  Insert2Array                                                   *
 * 功  能：  把数组插入到数组任意位置                                       *
 * 返回值：  成功， 添加数据的索引；失败，-1                                *
 ****************************************************************************/
COMMONLMN_API  DWORD  Insert2Array( IN PArray pArray, IN DWORD dwIndex, 
                                    IN void * pData );


/****************************************************************************
 * 函数名：  GetFromArra                                                    *
 * 功  能：  取数组任意位置的数据                                           *
 * 返回值：  成功， 数据；失败，0                                           *
 ****************************************************************************/
COMMONLMN_API  void * GetFromArray( IN PArray pArray,  IN DWORD dwIndex );


/****************************************************************************
 * 函数名：  SetArray                                                       *
 * 功  能：  设置数组任意位置的数据                                         *
 * 返回值：  成功， TRUE；失败，FALSE                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL SetArray( IN PArray pArray, IN DWORD dwIndex, 
                              IN void * pData );


/****************************************************************************
 * 函数名：  EraseArray                                                     *
 * 功  能：  擦除数组任意位置的数据                                         *
 * 返回值：  成功， TRUE；失败，FALSE                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL EraseArray( IN PArray pArray, IN DWORD dwIndex );


/****************************************************************************
 * 函数名：  ClearArray                                                     *
 * 功  能：  擦除数组所有的数据                                             *
 * 返回值：  成功， TRUE；失败，FALSE                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL  ClearArray( IN PArray pArray );


/****************************************************************************
 * 函数名：  DeinitArray                                                    *
 * 功  能：  销毁数组                                                       *
 * 返回值：  成功， TRUE；失败，FALSE                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL  DeinitArray( IN PArray pArray );












/****************************************************************************/
/*                        LIST(列表)                                        */
/****************************************************************************/


typedef void * PList;


typedef struct tagListNode
{
	void *               pData;
}ListNode, *PListNode;



/****************************************************************************
 * 函数名：  InitList                                                       *
 * 功  能：  初始化列表                                                     *
 * 返回值：  非0        成功                                                *
 *           0          失败                                                *
 ****************************************************************************/
COMMONLMN_API  PList  InitList( );



/****************************************************************************
 * 函数名：  GetListSize                                                    *
 * 功  能：  获取列表项个数                                                 *
 ****************************************************************************/
COMMONLMN_API  DWORD  GetListSize( IN PList pList );



/****************************************************************************
 * 函数名：  Insert2ListHead                                                *
 * 功  能：  把数据添加到列表头                                             *
 * 返回值：  成功，新增加的node；失败，0                                    *
 ****************************************************************************/
COMMONLMN_API  PListNode  Insert2ListHead( IN PList pList, IN void * pData );


/****************************************************************************
 * 函数名：  Insert2ListTail                                                *
 * 功  能：  把数据添加到列表尾                                             *
 * 返回值：  成功，新增加的node；失败，0                                    *
 ****************************************************************************/
COMMONLMN_API  PListNode  Insert2ListTail( IN PList pList, IN void * pData );


/****************************************************************************
 * 函数名：  Insert2List                                                    *
 * 功  能：  把数据添加到列表任意位置                                       *
 * 返回值：  成功，新增加的node；失败，0                                    *
 ****************************************************************************/
const PListNode LMN_FIRST_LIST_NODE = 0;
const PListNode LMN_LAST_LIST_NODE  = (PListNode)(-1);
COMMONLMN_API  PListNode  Insert2List( IN PList pList, IN void * pData, IN PListNode pAfterNode );

COMMONLMN_API  PListNode  Insert2ListEx( IN PList pList, IN void * pData, IN PListNode pBefoereNode );



/****************************************************************************
 * 函数名：  GetListHead                                                    *
 * 功  能：  获取列表表头                                                   *
 * 返回值：  成功，Node；失败，0                                            *
 ****************************************************************************/
COMMONLMN_API  PListNode GetListHead( IN PList pList );


/****************************************************************************
 * 函数名：  GetListTail                                                    *
 * 功  能：  获取列表表尾                                                   *
 * 返回值：  成功，Node；失败，0                                            *
 ****************************************************************************/
COMMONLMN_API  PListNode GetListTail( IN PList pList );


/****************************************************************************
 * 函数名：  GetNextListNode                                                *
 * 功  能：  获取列表的下一个Node                                           *
 * 返回值：  成功，Node；失败，0                                            *
 ****************************************************************************/
COMMONLMN_API  PListNode GetNextListNode( IN const PListNode pNode );


/****************************************************************************
 * 函数名：  GetPrevListNode                                                *
 * 功  能：  获取列表的上一个Node                                           *
 * 返回值：  成功，Node；失败，0                                            *
 ****************************************************************************/
COMMONLMN_API  PListNode GetPrevListNode( IN const PListNode pNode );


/****************************************************************************
 * 函数名：  EraseListHead                                                  *
 * 功  能：  擦除列表头                                                     *
 * 返回值：  成功，下一个Node；失败，0                                      *
 ****************************************************************************/
COMMONLMN_API  PListNode EraseListHead( IN PList pList );


/****************************************************************************
 * 函数名：  EraseListTail                                                  *
 * 功  能：  擦除列表尾                                                     *
 * 返回值：  0                                                              *
 ****************************************************************************/
COMMONLMN_API  PListNode EraseListTail( IN PList pList );



/****************************************************************************
 * 函数名：  EraseList                                                      *
 * 功  能：  擦除一个Node                                                   *
 * 返回值：  成功，下一个Node；失败，0                                      *
 ****************************************************************************/
COMMONLMN_API  PListNode EraseList( IN PList pList, IN PListNode pNode );



/****************************************************************************
 * 函数名：  ClearList                                                      *
 * 功  能：  清除列表所有的Node                                             *
 ****************************************************************************/
COMMONLMN_API  BOOL  ClearList( IN PList pList );


/****************************************************************************
 * 函数名：  DeinitList                                                     *
 * 功  能：  销毁列表                                                       *
 ****************************************************************************/
COMMONLMN_API  BOOL  DeinitList( IN PList pList );














#ifdef __cplusplus
} // extern "C" { 
#endif

























/****************************************************************************/
/*                        TREE(树/森林)                                     */
/****************************************************************************/

typedef struct tagTreeNode
{
	void *    pData;
}TreeNode, *PTreeNode;


const PTreeNode LMN_FIRST_TREE_NODE = 0;
const PTreeNode LMN_LAST_TREE_NODE  = (PTreeNode)(-1);



/****************************************************************************
 * 函数名：  InitTree                                                       *
 * 功  能：  初始化树                                                       *
 * 返回值：  成功，非0；失败，0                                             *
 ****************************************************************************/
COMMONLMN_API  PTreeNode InitTree( void * pData );


/****************************************************************************
 * 函数名：  Insert2Tree                                                    *
 * 功  能：  往树里插入节点                                                 *
 * 返回值：  成功，新的节点；失败，0                                        *
 ****************************************************************************/
COMMONLMN_API  PTreeNode Insert2Tree( IN PTreeNode pParent, IN void * pData, 
									  IN PTreeNode pAfter = LMN_LAST_TREE_NODE );

COMMONLMN_API  PTreeNode Insert2TreeEx( IN PTreeNode pParent, IN void * pData, 
                                        IN PTreeNode pBefore = LMN_LAST_TREE_NODE );


/****************************************************************************
 * 函数名：  GetParentTreeNode                                              *
 * 功  能：  获取父节点                                                     *
 * 返回值：  成功，非0；失败，0                                             *
 ****************************************************************************/
COMMONLMN_API  PTreeNode  GetParentTreeNode( IN PTreeNode pTreeNode );


// 获取后一个兄弟节点
// 返回值：成功，非0
COMMONLMN_API  PTreeNode  GetNextSiblingTreeNode( IN PTreeNode pTreeNode );

// 获取前一个兄弟节点
// 返回值：成功，非0
COMMONLMN_API  PTreeNode  GetPrevSiblingTreeNode( IN PTreeNode pTreeNode );

// 获取第一孩子节点
// 返回值：成功，非0
COMMONLMN_API  PTreeNode  GetChildTreeNode( IN PTreeNode pTreeNode );

// 获取第N个孩子节点, index从0开始
// 返回值：成功，非0
COMMONLMN_API  PTreeNode  GetChildTreeNodeByIndex( IN PTreeNode pTreeNode, DWORD dwIndex );

// 删除节点
// 如果有右节点，返回右节点。如果父节点有右节点，返回右节点，以此类推
// 用于树的遍历(采用中序遍历)
COMMONLMN_API  PTreeNode  EraseTreeNode( IN PTreeNode pTreeNode );

// 销毁树
COMMONLMN_API  void   DeInitTree( PTreeNode pTreeRoot );

// 遍历树的时候使用
// 从root开始，但是不包括root
// 中序遍历(父，左子，右子)
COMMONLMN_API  PTreeNode  GetNextTreeNode( PTreeNode pTreeNode );


















/****************************************************************************/
/*                        Hashtable                                         */
/****************************************************************************/


typedef void * PHashtable;


typedef struct tagHashNode
{
	void * pData;
}HashNode, *PHashNode;


// 自定义hash函数
typedef DWORD (*HashFunc)( void * pKey );

// 自定义两个key值比较结果
// 返回值 
//     0：相等
//     1：ke1  > key2
//    -1：key1 < key2
typedef int  (*CompFunc)( void * pKey1,void * pKey2 );

// 初始化一个Hashtable
// CAUSION：如果pfnCmp不为0，则pfnHash也必须不为0
COMMONLMN_API  PHashtable   InitHashtable( DWORD dwSize = 0, 
										   HashFunc pfnHash = 0, 
										   CompFunc pfnCmp = 0 );

// 获取Hashtable当前有多少个节点
COMMONLMN_API  DWORD        GetHashtableSize( PHashtable pHt );

// 添加节点到Hashtable
// 如果存在同样key的节点，则添加失败
COMMONLMN_API  PHashNode    Add2Hashtable( PHashtable pHt, const void * pKey, const void * pData );

// 添加节点到Hashtable
// 如果存在同样key的节点，则修改该节点Value
// 如果参数ppOldData不为0，则在修改的情况下返回旧值
COMMONLMN_API  PHashNode    SetHashtable( PHashtable pHt, const void * pKey, const void * pData, void ** ppOldData = 0 );

// Hashtable是否存在key
COMMONLMN_API  BOOL         IfHashtableContainKey( PHashtable pHt, const void * pKey );

// 根据key获取hashtable的某项值
COMMONLMN_API  BOOL         GetHashtableValue( PHashtable pHt, const void * pKey, void ** ppValue );

// 获取hashtable的第一项值(可用于遍历)
COMMONLMN_API  PHashNode    GetHashtableFirstNode( PHashtable pHt );

// 获取hastbable的下一项值(可用于遍历)
COMMONLMN_API  PHashNode    GetNextHashtableNode( const HashNode * pNode );

// 根据node获取key(可用于遍历)
COMMONLMN_API  void *       GetHashNodeKey( const HashNode * pNode );

// 删除hashtable的某项(可用户遍历)
// 返回值：删除项的下一项
COMMONLMN_API  PHashNode    EraseHashtable( PHashNode pNode );

// 根据key删除hashtable的某项
COMMONLMN_API  int          EraseHashtableByKey( PHashtable pHt, const void * pKey );

// 清空hashtable
COMMONLMN_API  int          ClearHashtable( PHashtable pHt );

// 销毁hashtable
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
// 功能: 判断一个点是否在一个多边形中
/************************************************************************/
COMMONLMN_API BOOL IsPointInPolygon( const Point *  ptPoint,  
									 const Point atPolygon[],  
									 DWORD dwVertexCount );



/***********************************************************************/
// 功能: 查看字节流，左边显示字节值，右边显示字符
// 参数: bDosCarriageReturn,  回车符号是\n还是\r\n
//       bShowAnsi,  文字部分是否以ansi方式显示(显示汉字)
/***********************************************************************/
COMMONLMN_API int DebugStream(  char *       pchDebugBuf,  DWORD  dwDebugBufLen, 
				                const BYTE * pbyStream,    DWORD  dwStreamLen,
				                BOOL         bDosCarriageReturn = FALSE,
                                BOOL         bShowAnsi = FALSE );



/***********************************************************************/
// 功能: 获取一个随机数
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
    char                  szName[MAX_CONSOLE_MENU_ITEM_NAME_SIZE];     // 菜单item名称
    ConsoleMenuHandle     hMenu;                                       // 不为0时，表示跳转到另一个菜单
}TConsoleMenuItem, *PTConsoleMenuItem;

// 用户自定义的选择某项菜单后的回调函数
typedef void  (*HandleChoiceCb)( ConsoleMenuHandle hMenu,  DWORD dwIndex );


// 选择当前菜单的哪个选项
// 返回值: QUIT_CONSOLE_MENU, 退出ConsoleMenu系统
//         0 ~ N对应当前菜单的索引
#define QUIT_CONSOLE_MENU                        ((DWORD)-1)
typedef DWORD (*SelectChoiceCb)( ConsoleMenuHandle hMenu, const char * szChoice );



// 初始化菜单系统
COMMONLMN_API int InitConsole( SelectChoiceCb pfnSelect, HandleChoiceCb  pfnHandleChoice );

// 创建子菜单
COMMONLMN_API ConsoleMenuHandle  CreateConsoleMenu( const char * szTitle );

// 添加菜单项(到末尾)
COMMONLMN_API int AddConsoleMenuItem( ConsoleMenuHandle hConsoleMenu,  const TConsoleMenuItem * pMenuItem );

// 显示某个子菜单
// 注意：在一次InitConsole和DeinitConsole之间只准调用一次
COMMONLMN_API int  DisplayConsoleMenu( ConsoleMenuHandle hConsoleMenu );

// 去初始化菜单系统
COMMONLMN_API int  DeinitConsole();











/***********************************************************************/
// 功能: 根据文件路径名获取文件所在目录，文件名和扩展名  
/***********************************************************************/
COMMONLMN_API int  ParseFileName( const char * szFilePath, 
					char * szFileDirectory = 0,  DWORD  dwDirLen = 0,
                    char * szFileName = 0,       DWORD dwNameLen = 0,
                    char * szExtention = 0,      DWORD dwExtLen = 0 );









/*************************       互斥锁   **************************************/
#ifdef WIN32
typedef   CRITICAL_SECTION    LmnLockType;
#else
typedef   pthread_mutex_t     LmnLockType;
#endif

COMMONLMN_API int  LmnInitLock  ( LmnLockType * pLock );
COMMONLMN_API int  LmnDeinitLock( LmnLockType * pLock );
COMMONLMN_API int  LmnLock( LmnLockType * pLock );
COMMONLMN_API int  LmnUnlock( LmnLockType * pLock );
/*************************    END 互斥锁   **************************************/




/*************************    信号量      ************************************/
typedef void * LmnSemphoreType;

// 进程内semphore
COMMONLMN_API  int  LmnSemCreate(LmnSemphoreType * pSemphore, DWORD dwInitCnt, DWORD dwMaxCnt);
COMMONLMN_API  int  LmnSemTake(LmnSemphoreType semphore);
COMMONLMN_API  int  LmnSemTakeByTime(LmnSemphoreType semphore, DWORD dwWaitMiniSeconds);
COMMONLMN_API  int  LmnSemGive(LmnSemphoreType semphore);
COMMONLMN_API  int  LmnSemDestroy(LmnSemphoreType semphore);
/*************************   END 信号量    ************************************/



/*************************    线程      *************************************/
COMMONLMN_API  DWORD  LmnGetCurrentThrdId();

// 删除的时候
typedef void (*OnThrdDataEraseCb)( void * pKey,  void * pValue );
// Set的时候数据改变(key不变)
typedef void (*OnThrdDataChangeCb)( void * pKey,  void * pValue );

// init thread local storage module
// 内部用hashtable实现，所以可能需要hash函数，key比较函数
COMMONLMN_API  int   LmnInitThrdTls( HashFunc           pfnHash = 0,     // hash(key)
                                     CompFunc           pfnComp = 0,     // compare(key)
                                     OnThrdDataEraseCb  pfnOnErase = 0,  // hash item 被删除
                                     OnThrdDataChangeCb pfnOnChange = 0 ); // hash item 被修改

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

// 返回值： 成功，非0
COMMONLMN_API  LmnThrdType LmnCreateThread( LmnThreadFunc pfnThrd, void * pParam, DWORD dwStackSize, BOOL bDetached = FALSE );
COMMONLMN_API  void *      LmnWait4Thrd( LmnThrdType hThrd );

/*************************   END 线程    ************************************/










/*************************    RADVISION形式 协议栈   *************************************/

#define liEvRead    0x01
#define liEvWrite   0x02
#define liEvExcept  0x04
#define liEvAccept  0x08
#define liEvConnect 0x10
#define liEvClose   0x20       // 对端关闭
#define liEvDestroy 0x40       // 调用close关闭

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
    DWORD    dwIdleTime;                    // 单位：秒
    DWORD    dwIntervalTimve;               // 单位：秒

#ifndef WIN32 
    DWORD    dwCount;                       // WIN32情况下，keepalive的次数不能修改，默认5次
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

// select多路复用
// nMs单位， 毫秒 (20~1000)
COMMONLMN_API  int  seliSelect( DWORD * pdwMs );

// 返回值： socket
// -1：失败
COMMONLMN_API  FdType  liOpen ( liProtocol_t protocol, DWORD ipAddr, WORD wPort, 
                                LPLIEVENTHANDLER eventHandler, void* context, 
                                const TKeepAliveParam * pKeepAliveParam );

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int  liClose ( FdType fd );

// dwTimeOut 单位:秒
// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int  liConnect ( int fd, DWORD ipAddr, WORD wPort, DWORD dwTimeOut );

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int liListen(int fd, int queueLen );

// 返回值： socket
COMMONLMN_API  FdType liAccept(int fd, LPLIEVENTHANDLER fnCallBack, void * context);

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int liShutdown( int fd );

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int liTcpSend (int fd, const char *buff, int * plen );

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int liTcpRecv (int fd, char *buff,int * plen);

// 返回值：
//          -1 ERROR
COMMONLMN_API  int liGetState (int fd);

// 注意：当收到关闭事件的时候再调用时在linux环境下无效
COMMONLMN_API  int liGetSockName(int fd, DWORD * pdwIp, WORD * pwPort, BOOL bPeer = FALSE );

// 连接sock5服务
// 如果参数bUseSock5为True，则使用sock5 proxy
// 否者不使用sock5 proxy
// 
// 说明： lxConnectSock5 调用了liOpen，所以不需要再调用liOpen
COMMONLMN_API  FdType  liConnectSock5 ( const TCnnSock5Param *  ptParam );


/*************************    END RADVISION形式 协议栈   *************************************/








/************************     RADVISION形式 协议栈 扩展 *************************************/
typedef  void *     SelixNode;

typedef void(*LPLIXEVENTHANDLER)( SelixNode pSeli, FdType fd, int liEvent, int error, void * context );

COMMONLMN_API  int          selixInit( DWORD dwTotalMaxSocketCnt );
COMMONLMN_API  int          selixEnd( );
COMMONLMN_API  BOOL         isSelixInited( );

// 如果pdwMaxSocketCnt为0，则认为创建使用所有剩下的MaxSocketCount
COMMONLMN_API  SelixNode    selixCreateNode( INOUT DWORD * pdwMaxSocketCnt = 0 );

COMMONLMN_API  int          selixDestroyNode( SelixNode pSeli );


// select多路复用
// nMs单位， 毫秒 (20~1000)
COMMONLMN_API  int  selixSelect( SelixNode pSeli, DWORD * pdwMs );

// 返回值： socket
COMMONLMN_API  FdType  lixOpen ( SelixNode pSeli, liProtocol_t protocol, DWORD ipAddr, WORD wPort, 
                                    LPLIXEVENTHANDLER eventHandler, void* context, 
                                    const TKeepAliveParam * pKeepAliveParam );

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int  lixClose ( SelixNode pSeli, FdType fd );

// dwTimeOut 单位:秒
// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int  lixConnect ( SelixNode pSeli,  int fd, DWORD ipAddr, WORD wPort, DWORD dwTimeOut );

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int lixListen( SelixNode pSeli, int fd, int queueLen );

// 返回值： socket
COMMONLMN_API  FdType lixAccept( SelixNode pSeli, int fd, LPLIXEVENTHANDLER fnCallBack, void * context);

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int lixShutdown( SelixNode pSeli, int fd );

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int lixTcpSend ( SelixNode pSeli, int fd, const char *buff, int * plen );

// 如果只发送一部分，sleep一段时间( 0==dwSleeptime，则不sleep )再发送，直达发送完或失败
COMMONLMN_API  int lixTcpCompleteSend ( SelixNode pSeli, int fd, const char *buff, int * plen, DWORD dwSleepTime );

// 返回值： 0  OK
//          -1 ERROR
COMMONLMN_API  int lixTcpRecv ( SelixNode pSeli, int fd, char *buff,int * plen);

// 返回值：
//          -1 ERROR
COMMONLMN_API  int lixGetState ( SelixNode pSeli, int fd);


COMMONLMN_API  int lixGetSockName( SelixNode pSeli, int fd, DWORD * pdwIp, WORD * pwPort );

// 连接sock5服务
// 如果参数bUseSock5为True，则使用sock5 proxy
// 否者不使用sock5 proxy
// 
// 说明： lxConnectSock5 调用了liOpen，所以不需要再调用liOpen


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


/************************  END RADVISION形式 协议栈 扩展 *************************************/
















/****************************************************************************
 * 函数名：  Utf8ToAnsi                                                     *
 * 功  能：  utf8编码变为Ansi编码                                           *
 * 返回值：  非0        成功                                                *
 *           0          失败                                                *
 ****************************************************************************/
COMMONLMN_API char * Utf8ToAnsi( char * szAnsi, unsigned int dwAnsiSize, const char * szUtf8 );




/****************************************************************************
 * 函数名：  AnsiToUtf8                                                     *
 * 功  能：  Ansi编码转为utf8编码                                           *
 * 返回值：  非0        成功                                                *
 *           0          失败                                                *
 ****************************************************************************/
COMMONLMN_API char * AnsiToUtf8( char * szUtf8, unsigned int dwUtf8Size, const char * szAnsi );








/*************************    时间      ********************************/
COMMONLMN_API DWORD   LmnGetTickCount();                       //单位：毫秒
COMMONLMN_API void    LmnSleep( DWORD dwMiliSeconds );         //单位：毫秒
/*************************  END 时间   **********************************/



/*************************    Post Message Thread    ********************************/
typedef struct tagLmxMsgData
{
    DWORD   dwMsgId;
    void *  pMsg;
    DWORD   dwMsgLen;
}LmxMsgData, *PLmxMsgData;


typedef  void *  LmnThrdHandle;
// 返回值： TRUE, 继续执行消息循环； FALSE，退出消息循环
typedef  BOOL (*POnThreadMessage)( LmnThrdHandle h, const LmxMsgData * pMsgData, void * pContext );


// 对于某些线程(例如主线程)，不需要创建线程了。第二个参数设置为FALSE。在主线程里用，相当于为主线程创建一个消息队列，但是不创建新线程
COMMONLMN_API   LmnThrdHandle   LmxCreateThread( POnThreadMessage pfnOnMessage, BOOL bCreateThrd = TRUE, void * pContext = 0, BOOL bDetach = FALSE );
COMMONLMN_API   int             LmxDestroyThread( LmnThrdHandle h );

// 对于调用LmxCreateThread时第二个参数为FALSE，主动阻塞，不停的获取消息
COMMONLMN_API   int             LmxThreadRun( LmnThrdHandle h );

COMMONLMN_API   int             LmxPostMessage( LmnThrdHandle h, const LmxMsgData * pMsgData );
// 延时时间最长
COMMONLMN_API   int             LmxPostDelayMessage( LmnThrdHandle h, const LmxMsgData * pMsgData, DWORD dwDelayedMs );
/*************************  END Post Message Thread    ******************************/













//版本号
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

// 获取字符串形式的version
// *.*.*.* 或 *.*.* 或 *.*
COMMONLMN_API  char *  LmnGetVersion( INOUT char * szVersion, IN DWORD dwVersionSize, IN DWORD dwVersion, IN EVersionType eVer = VERSION_TYPE4 );
// 获取整形格式的version
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


// 先占先得，还是后占后得， 快速关闭telnet窗口，有可能回调函数的Printf会输出到下一个telnet窗口
//enum ETenetdMethod
//{
//    TELNETD_METHOD_FORMMER_PRIORITY,
//    TELNETD_METHOD_LATTER_PRIORITY
//};


// 功能: 启动telnet服务
// 参数： 端口， 注册的命令，命令个数， 命令的回调， 同时可连接上的用户个数
// 说明： 依赖于selixInit
//        全局只能起一个telnet服务
COMMONLMN_API  int  LmnStartTelnetd( WORD wPort, const TRegTelnetdCmd * atCmds, DWORD dwCmdCnt, 
									 LPOnTelnetCmd pfnOnCommand, DWORD dwMaxCoUsersCnt );


COMMONLMN_API  int  LmnStopTelnetd( );


COMMONLMN_API  int  LmnTelPrintf( DWORD dwClientId, const char * szFormat, ... );

COMMONLMN_API  int  LmnTelPrintf0( DWORD dwClientId, const char * szMsg );



/************************************ end telnetd *****************************************/







COMMONLMN_API  int  Unicode2Utf8( IN DWORD dwUnicode, INOUT BYTE * pOutput, INOUT DWORD * pdwOutputSize, BOOL  bLittleEndian = TRUE );

COMMONLMN_API  int  UnicodeStream2Utf8Stream( IN const WORD * pUnicode, IN DWORD dwUnicodeSize, 
                                              INOUT BYTE * pOutput, INOUT DWORD * pdwOutputSize, BOOL  bLittleEndian = TRUE );

// 转换第一个首Utf8字符
COMMONLMN_API  int  Utf8ToUnicode( IN const BYTE * pUtf8, INOUT DWORD * pdwUtf8Size, OUT DWORD * pdwUnicode,  BOOL bLittleEndian = TRUE );

COMMONLMN_API  int  Utf8Stream2UnicodeStream( IN const BYTE * pUtf8, IN DWORD dwUtf8Size, 
                                              OUT WORD * pwUnicode, INOUT DWORD * pdwUnicodeSize, BOOL bLittleEndian = TRUE );





enum ELmnFileType
{
    E_FILE_TYPE_NONE,
    E_FILE_TYPE_FILE,
    E_FILE_TYPE_FOLDER
};


// 创建目录(bIgnoreExisted, 如果存在目录是否忽略该错误)
COMMONLMN_API  int  LmnCreateFolder( IN const char * szPathName, BOOL bIgnoreExisted = TRUE );

// 打开文件(主要用于win32下的中文名/路径文件)
COMMONLMN_API  FILE * LmnOpenFile ( IN const char * szFile, IN const char * szMode );

COMMONLMN_API  DWORD LmnReadFile ( void * buffer, DWORD size, DWORD count, FILE * stream );

COMMONLMN_API  DWORD LmnWriteFile ( const void * buffer, DWORD size, DWORD count, FILE * stream );

COMMONLMN_API  void LmnCloseFile( FILE * fp );


// 获取文件类型(目录/文件)
COMMONLMN_API  ELmnFileType  LmnGetFileType( IN const char * szPathName );

// 删除文件/目录
COMMONLMN_API  int   LmnDeleteFile( IN const char * szPathName );

// 复制文件/目录
// szNewPathName, 复制后的全路径名，不是父路径名
COMMONLMN_API  int   LmnCopyFile( IN const char * szOldPathName, IN const char * szNewPathName, IN BOOL bOverWrite = TRUE );

// 移动文件/目录
// szNewPathName, 移动后的全路径名，不是父路径名
COMMONLMN_API  int   LmnMoveFile( IN const char * szOldPathName, IN const char * szNewPathName, IN BOOL bOverWrite = TRUE );

// 获取文件/目录大小
COMMONLMN_API  DWORD  LmnGetFileSize( IN const char * szPathName );












//////////////////////      文件/文件夹转化为流，用于文件/文件夹传输(基于TCP)     ///////////////////////////


#define  LMN_MAX_FILE_PATH                        340

typedef  void *  HFileStream;


typedef  struct  tagFileStreamPos
{
    ELmnFileType   eType;
    // 以utf8格式保存路径名
    char           szSubPath[LMN_MAX_FILE_PATH];
    // 如果是文件的话，当前文件的大小
    long           nPos;
}TFileStreamPos, *PTFileStreamPos;


// 打开文件/文件夹流
// 如果是读操作，忽略eType和bOverWrite参数
COMMONLMN_API  HFileStream  OpenFileStream( const char * szPathName, BOOL bRead, ELmnFileType eType = E_FILE_TYPE_FILE, 
                                            BOOL bOverWrite = FALSE, TFileStreamPos * ptPos = 0 );

// 设置当前位置(对于读操作)
COMMONLMN_API  int  SetFileStreamPos( IN HFileStream h, IN const TFileStreamPos * pPosition );

// 读取(对于读操作)
// pdwBufSize，输入输出参数，buf的size，以及读后buf的大小
// pdwReadFileSize读取文件(不包含文件夹)的内容的大小
COMMONLMN_API  int ReadFileStream( IN HFileStream h, IN void * buf, INOUT DWORD * pdwBufSize, OUT DWORD * pdwReadFileSize = 0 );

// 写入(对于写操作)
// pdwBufSize，输入输出参数，buf的大小，以及有效使用的字节
// pdwWriteFileSize，输入输出参数，实际写入的文件大小(不包含文件夹)
COMMONLMN_API  int WriteFileStream( IN HFileStream h, IN const void * buf, INOUT DWORD * pdwBufSize, OUT DWORD * pdwWriteFileSize = 0 );


COMMONLMN_API int CloseFileStream( IN HFileStream h );



COMMONLMN_API  int  GetFileStreamPos( IN HFileStream h, IN TFileStreamPos * pPosition );

























namespace LmnCommon
{
    /*******************************************************************************/
    // 功能: 查看数字(char|unsigned char|short|unsigned short|int|unsigned int等
    //       是否设置了某位。位的索引从0开始。
    // 返回值: 
    //          TRUE,  设置了
    //          FALSE, 没有设置(或参数错误)
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
    // 设置某位的值                                                        
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
        COMMONLMN_API  BOOL Append( const void * pData, DWORD dwDataLen );                        // 写入数据到末尾
        COMMONLMN_API  void Clear();

        COMMONLMN_API  BOOL Read( void * pData, DWORD dwDataSize = -1 );                    // 从当前读位置开始读取数据(可以连续读取数据)
        COMMONLMN_API  void ResetReadPos();                                                 // 把读重置为0
        COMMONLMN_API  BOOL SetReadPos( DWORD dwPos );

        COMMONLMN_API  DWORD         GetDataLength() const;
        COMMONLMN_API  const void *  GetData() const;
        COMMONLMN_API  DWORD         GetReadPos() const;


        // 把前面的已读的内存去掉（未读的内存往前移动），减少以后可能再分配空间的可能性
        COMMONLMN_API  void         Reform();

    private:
        // 用自定义的方法计算需要块的实际分配内存大小
        COMMONLMN_API  static DWORD CalcReqBufLen( DWORD dwReqLen );

        BYTE *   m_pData;                                       // 数据
        DWORD    m_dwDataBufLen;                                // 数据buf长度
        DWORD    m_dwDataLen;                                   // 有效数据长度
        DWORD    m_dwRPos;                                      // 当前读的位置
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



    // 循环存储items，超过上限的item覆盖最早的item
    // M的值不要设置太大(超出栈内存)
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

            // 如果超出数组上限，设置end pos为0
            if ( m_dwEndPos > M )
            {
                m_dwEndPos = 0;
            }

            // 如果begin == end，移动begin pos
            if ( m_dwEndPos == m_dwBeginPos )
            {
                m_dwBeginPos++;
                // 如果begin pos超出上限，设置begin pos为0
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
        DWORD    m_dwBeginPos;                                 // 起始位置
        DWORD    m_dwEndPos;                                   // 结束位置
    };




    // 固定数组上限的items
    // M的值不要设置太大(超出栈内存)
    template <class T, const DWORD M>
    class  CFixedSizeItems
    {
    public:
        CFixedSizeItems()
        {
            memset( m_bUsed, 0, sizeof(m_bUsed) );
        }

        ~CFixedSizeItems(){}


        // 找到一个空位存储一个item
        DWORD  AddItem( const T & t )
        {
            DWORD i;
            for ( i = 0; i < M; i++ )
            {
                // 如果有空位
                if ( !m_bUsed[i] )
                {
                    break;
                }
            }
            
            // 全部的没有空位
            if ( i >= M )
            {
                return (DWORD)-1;
            }

            m_items[i] = t;
            m_bUsed[i] = TRUE;

            return i;
        }

        // 在某个位置设置一个item
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
        BOOL     m_bUsed[M];                // 对应的items是否被使用(未被使用的，可以作为空间存储新的item)
    };

} // namespace LmnCommon













#endif // _LEMON_COMMON_2012_01_20_




