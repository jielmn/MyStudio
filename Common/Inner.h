#ifndef _INNER_HEADER_2012_02_19_
#define _INNER_HEADER_2012_02_19_

#include <stdio.h>
#include "LmnCommon.h"

// 用来强制宏后加引号
#define DO_ONCE( statement )      do{ statement }while(0)

#define  SAFE_FREE( p )           DO_ONCE( if (p ) { g_pfnFreeMem( p );    p = 0; } )  

#ifdef WIN32
#define SNPRINTF(s,n, f,...)      _snprintf(s, n, f, __VA_ARGS__)
#define STRNCPY(d, s, n)          DO_ONCE( strncpy( d, s, (n)-1 ); d[(n)-1] = '\0'; )
#else
#define SNPRINTF(s,n, f...)       snprintf(s, n, f)
#define STRNCPY(d, s, n)          strncpy( d, s, n )
#endif

#define MIN( a, b )               ( (a) < (b) ? (a) : (b) )
#define MAX( a, b )               ( (a) > (b) ? (a) : (b) )


extern AllocMemCb  g_pfnAllocMem;
extern FreeMemCb   g_pfnFreeMem;


void          ClearListWithPointerValue( PList  pList );
PListNode     FindFirstListNodeByValue( PList  pList, void * pValue );


#define DEBUG_FLAG

#ifdef DEBUG_FLAG

#ifdef WIN32
#define  PERROR(f,...)                    fprintf( stderr, f, __VA_ARGS__ )
#define  MYTRACE(f,...)                   printf( f, __VA_ARGS__ )
#else
#define  PERROR(f...)                     fprintf( stderr, f )
#define  MYTRACE(f...)                    printf( f )
#endif

#else

#ifdef WIN32
#define  PERROR(f,...)             
#define  MYTRACE(f,...)             
#else
#define  PERROR(f...)                     
#define  MYTRACE(f...)                     
#endif

#endif // DEBUG_FLAG


#ifdef WIN32
#define  SLEEP(ms)                         Sleep(ms)
#else
#define  SLEEP(ms)                         usleep((ms)*1000)
#endif  // WIN32


#define  NORMAL_BUF_SIZE                   8192


// 得到某个字节的前几位
#define GET_FRONT_BITS( b, n )     ((BYTE)((b)>>(8-(n))))
// 得到某个字节的后几位
#define GET_BACK_BITS( b, n )      (((BYTE)((b)<<(8-(n))))>>(8-(n)))

// 从右至左（低位到高位顺序）
#define GET_MID_BITS( b, start, n )  (((BYTE)((b)<<(8-(n)-(start))))>>(8-(n)))









void * dfCreate();
BOOL   dfDestroy( void * h );
BOOL   dfAppend( void * h,const void * pData, DWORD dwDataLen );
BOOL   dfClear(void * h );
BOOL   dfRead(void * h, void * pData, DWORD dwDataSize = -1 );
BOOL   dfResetReadPos(void * h);
DWORD  dfGetDataLength(void * h);
const void * dfGetData(void * h);
DWORD  dfGetReadPos(void * h);
BOOL   dfSetReadPos( void * h, DWORD dwPos );




#endif




