/*********************************************************************************/
// 说明：
//     如果不是整体工程都用该内存跟踪函数代替系统的内存函数
//     可能出现如下情况：
//        1. 用跟踪函数如JDEBUG_MALLOC分配内存，用free释放
//           进程的堆内存会出错
//        2. 用系统内存函数如malloc分配内存，用JDEBUG_FREE释放
//           因为跟踪内存有相同的GUID标志。一个被跟踪的实际分配
//           的内存如下
//             | GUID block | head  block | 用户使用的内存 | tail block |
//           JDEBUG_FREE会检查前面的guid block。通常系统内存函数分配的内存
//           前不会碰巧有内存跟踪函数设定的GUID(暂定为16字节的长度)，JDEBUG_FREE
//           可以正确判断该内存块而不错误回收内存。如果上述小概率事件发生，同样
//           的系统的堆内存会出错。
//        3. 用跟踪函数如JDEBUG_MALLOC分配内存，用JDEBUG_FREE释放，但是JDEBUG_FREE
//           之前用JDebugClear清除了跟踪记录。和2情况类似了。
//           如上述2所述的，JDEBUG_FREE可以识别该
//           内存块而正确释放(除非小概率事件发生，内存的GUID已经被篡改)
/*********************************************************************************/

#ifndef _JTRACK_MEMORY_2011_3_11_
#define _JTRACK_MEMORY_2011_3_11_

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#ifdef __cplusplus
#include <iostream>
#endif



/***************************************************/
// 以下函数用以跟踪内存块，不要直接调用
/***************************************************/

// 替代malloc
void * JDEBUG_MALLOC(  unsigned int nSize, const char * szFileName, 
                       unsigned int nFileLine );
// 替代realloc
void * JDEBUG_REALLOC( void *memblock, unsigned int nSize, 
                       const char * szFileName, unsigned int nFileLine );
// 替代calloc
void * JDEBUG_CALLOC( unsigned int nNum, unsigned int nSize, 
                      const char * szFileName, unsigned int nFileLine );

// 替代free
void   JDEBUG_FREE( void *memblock );







#define MAX_FILENAME      64

// 内存泄露结构体
typedef struct tagLeakItem
{
    char         szFileName[MAX_FILENAME];     // 文件名
    unsigned int nFileLen;                     // 行号
    unsigned int nMemSize;                     // 内存块大小
}LeakItem, *PLeakItem;

#define GUID_LEN          16
#define MARGIN_HEAD       16
#define MARGIN_TAIL       16

// 内存被篡改结构体
typedef struct tagHackItem
{
    char         szFileName[MAX_FILENAME];      // 文件名
    unsigned int nFileLen;                      // 行号
    int          bGuidHacked;                   // 内存标志块被篡改
    int          bHeadHacked;                   // 内存头被篡改
    int          bTailHacked;                   // 内存尾被篡改
    char         pGuid[GUID_LEN];               // 篡改后的标志块
    char         pHead[MARGIN_HEAD];            // 篡改后的头
    char         pTail[MARGIN_TAIL];            // 篡改后的尾
}HackItem, *PHackItem;


/***********************************************/
// 功能：获取内存块跟踪报告
// 返回值：0 成功
//        -1 失败
/***********************************************/
int JDebugGetReport( PLeakItem * ppLeak,   unsigned int * pLeakNum, 
                      PHackItem * ppHacked, unsigned int * pHackedNum );

/************************************************/
// 释放JDebugGetReport中分配的内存
/************************************************/
void JDebugFree( void * memblock );

/************************************************/
// 清空当前对内存的所有跟踪情况
// 适合跟踪某一段情况下的内存，例如：
//    1. JDebugGetReport
//    2. JDebugClear
//    3. 再运行一段时间
//    4. JDebugGetReport
//    5. ......
// 说明：使用该函数会带来释放内存风险。
//       因为所有的内存跟踪信息丢失
/************************************************/
void JDebugClear();


#ifndef NJDEBUG

#define malloc(nSize)              JDEBUG_MALLOC(nSize, __FILE__, __LINE__ )
#define realloc(memblock, nSize)   JDEBUG_REALLOC( memblock, nSize, __FILE__, __LINE__ )
#define calloc(nNum, nSize)        JDEBUG_CALLOC( nNum, nSize, __FILE__, __LINE__ )
#define free(memblock)             JDEBUG_FREE(memblock )

#endif


#ifdef __cplusplus

/*=================  不要直接调用以下函数         =================*/
void * operator new( unsigned int nSize, 
                     const char * szFileName, unsigned int nLine );
// 为什么不会被调用？
void   operator delete( void * memblock, 
                     const char * szFileName, unsigned int nLine );
void * operator new[]( unsigned int nSize, 
                     const char * szFileName, unsigned int nLine );
// 为什么不会被调用？
void   operator delete[]( void * memblock, 
                     const char * szFileName, unsigned int nLine );

// 可以被调用 delete p 模式
void   operator delete  ( void * memblock );
// 可以被调用 delete[] p模式
void   operator delete[]( void * memblock );
/********************************************************************/

#ifndef NJDEBUG
#define new new(__FILE__,__LINE__)
#endif // end of NJDEBUG

#endif // end of __cplusplus

#endif
