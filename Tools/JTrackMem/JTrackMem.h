/*********************************************************************************/
// ˵����
//     ����������幤�̶��ø��ڴ���ٺ�������ϵͳ���ڴ溯��
//     ���ܳ������������
//        1. �ø��ٺ�����JDEBUG_MALLOC�����ڴ棬��free�ͷ�
//           ���̵Ķ��ڴ�����
//        2. ��ϵͳ�ڴ溯����malloc�����ڴ棬��JDEBUG_FREE�ͷ�
//           ��Ϊ�����ڴ�����ͬ��GUID��־��һ�������ٵ�ʵ�ʷ���
//           ���ڴ�����
//             | GUID block | head  block | �û�ʹ�õ��ڴ� | tail block |
//           JDEBUG_FREE����ǰ���guid block��ͨ��ϵͳ�ڴ溯��������ڴ�
//           ǰ�����������ڴ���ٺ����趨��GUID(�ݶ�Ϊ16�ֽڵĳ���)��JDEBUG_FREE
//           ������ȷ�жϸ��ڴ�������������ڴ档�������С�����¼�������ͬ��
//           ��ϵͳ�Ķ��ڴ�����
//        3. �ø��ٺ�����JDEBUG_MALLOC�����ڴ棬��JDEBUG_FREE�ͷţ�����JDEBUG_FREE
//           ֮ǰ��JDebugClear����˸��ټ�¼����2��������ˡ�
//           ������2�����ģ�JDEBUG_FREE����ʶ���
//           �ڴ�����ȷ�ͷ�(����С�����¼��������ڴ��GUID�Ѿ����۸�)
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
// ���º������Ը����ڴ�飬��Ҫֱ�ӵ���
/***************************************************/

// ���malloc
void * JDEBUG_MALLOC(  unsigned int nSize, const char * szFileName, 
                       unsigned int nFileLine );
// ���realloc
void * JDEBUG_REALLOC( void *memblock, unsigned int nSize, 
                       const char * szFileName, unsigned int nFileLine );
// ���calloc
void * JDEBUG_CALLOC( unsigned int nNum, unsigned int nSize, 
                      const char * szFileName, unsigned int nFileLine );

// ���free
void   JDEBUG_FREE( void *memblock );







#define MAX_FILENAME      64

// �ڴ�й¶�ṹ��
typedef struct tagLeakItem
{
    char         szFileName[MAX_FILENAME];     // �ļ���
    unsigned int nFileLen;                     // �к�
    unsigned int nMemSize;                     // �ڴ���С
}LeakItem, *PLeakItem;

#define GUID_LEN          16
#define MARGIN_HEAD       16
#define MARGIN_TAIL       16

// �ڴ汻�۸Ľṹ��
typedef struct tagHackItem
{
    char         szFileName[MAX_FILENAME];      // �ļ���
    unsigned int nFileLen;                      // �к�
    int          bGuidHacked;                   // �ڴ��־�鱻�۸�
    int          bHeadHacked;                   // �ڴ�ͷ���۸�
    int          bTailHacked;                   // �ڴ�β���۸�
    char         pGuid[GUID_LEN];               // �۸ĺ�ı�־��
    char         pHead[MARGIN_HEAD];            // �۸ĺ��ͷ
    char         pTail[MARGIN_TAIL];            // �۸ĺ��β
}HackItem, *PHackItem;


/***********************************************/
// ���ܣ���ȡ�ڴ����ٱ���
// ����ֵ��0 �ɹ�
//        -1 ʧ��
/***********************************************/
int JDebugGetReport( PLeakItem * ppLeak,   unsigned int * pLeakNum, 
                      PHackItem * ppHacked, unsigned int * pHackedNum );

/************************************************/
// �ͷ�JDebugGetReport�з�����ڴ�
/************************************************/
void JDebugFree( void * memblock );

/************************************************/
// ��յ�ǰ���ڴ�����и������
// �ʺϸ���ĳһ������µ��ڴ棬���磺
//    1. JDebugGetReport
//    2. JDebugClear
//    3. ������һ��ʱ��
//    4. JDebugGetReport
//    5. ......
// ˵����ʹ�øú���������ͷ��ڴ���ա�
//       ��Ϊ���е��ڴ������Ϣ��ʧ
/************************************************/
void JDebugClear();


#ifndef NJDEBUG

#define malloc(nSize)              JDEBUG_MALLOC(nSize, __FILE__, __LINE__ )
#define realloc(memblock, nSize)   JDEBUG_REALLOC( memblock, nSize, __FILE__, __LINE__ )
#define calloc(nNum, nSize)        JDEBUG_CALLOC( nNum, nSize, __FILE__, __LINE__ )
#define free(memblock)             JDEBUG_FREE(memblock )

#endif


#ifdef __cplusplus

/*=================  ��Ҫֱ�ӵ������º���         =================*/
void * operator new( unsigned int nSize, 
                     const char * szFileName, unsigned int nLine );
// Ϊʲô���ᱻ���ã�
void   operator delete( void * memblock, 
                     const char * szFileName, unsigned int nLine );
void * operator new[]( unsigned int nSize, 
                     const char * szFileName, unsigned int nLine );
// Ϊʲô���ᱻ���ã�
void   operator delete[]( void * memblock, 
                     const char * szFileName, unsigned int nLine );

// ���Ա����� delete p ģʽ
void   operator delete  ( void * memblock );
// ���Ա����� delete[] pģʽ
void   operator delete[]( void * memblock );
/********************************************************************/

#ifndef NJDEBUG
#define new new(__FILE__,__LINE__)
#endif // end of NJDEBUG

#endif // end of __cplusplus

#endif
