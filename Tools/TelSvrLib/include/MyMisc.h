#ifndef __MY_MISC_2010_08_10__
#define __MY_MISC_2010_08_10__

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#define DO_ONCE( statement )      do{ statement }while(0)
#define REMOVE_MEM_ARR(pData)     DO_ONCE( if(NULL != pData){delete[] pData;pData=NULL;} )
#define REMOVE_MEM(pData)         DO_ONCE( if(NULL != pData){delete   pData;pData=NULL;} )

typedef struct tagPoint
{
	float x;
	float y;
}Point, * PPoint;

int IsPointInPolygon( const PPoint pt, const Point polygon[], int nNum );

//////////////////////////////////////////////////////////////////////
// 查看字节流，左边显示字节值，右边显示字符
//////////////////////////////////////////////////////////////////////
char * DebugStream( char* out_buff, unsigned int out_bufLen, 
				    const char * in_stream, unsigned int in_streamLen );

#ifdef WIN32
//////////////////////////////////////////////////////////////////////
// 根据文件路径名获取文件标题(包含扩展名)
// 例子:
//     c:\test\t\a\a.txt 返回 a.txt
//     \test\t\a\b.txt   返回 b.txt
//     c.txt             返回 c.txt
//     ""(空)            返回 ""(空)
const char * GetFileTitle( const char* szFileName );

/////////////////获取文件的路径///////////////////////////////////////
// 例子:
//     "c:\test\t\a\a.txt" 得到 "c:\test\t\a\"
//     "\test\t\a\b.txt"   得到 "\test\t\a\"
//     "\"                 得到 "\"
//     c.txt               得到 NULL
//     ""(空)              得到 NULL
//
//  return value: -2, invalid parameters
//                -1, exceeds buff size
//                >=0, characters stored in the buff
int GetFileDirectory( const char* szFileName, 
					  char * szFileDirectory, unsigned int nDirLen );

#endif


int GetRand( int nMin, int nMax );





/************************************************************************/
/* 1. 该位是否设置(索引从0开始)                                         */
/* 例如7的二进制表示为0111，前三位为1，第四位为0                        */
/* IfHasBit(7, 0)返回1, IfHasBit(7,3)返回0。                            */
/* 2. 可用于64位的ULONGLONG类型, 32位的int, 16位的short int, 8位的char  */
/************************************************************************/
template <class T>
int IfHasBit(T num, unsigned char index)
{
	if ( index >= sizeof(T) * 8 )
	{
		return -1;
	}

	if ( 0 == ( num & ((T)1<<index) ) )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/************************************************************************/
/* 设置某位的值                                                         */
/************************************************************************/
template <class T>
void SetBit(T& num, unsigned char index, bool bSet = true)
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

//DWORD (WINAPI *PTHREAD_START_ROUTINE)( LPVOID lpThreadParameter);
#ifdef WIN32
typedef HANDLE    JThrdHandle;
#else
typedef pthread_t JThrdHandle;
#endif

typedef void * JThrdRet;
typedef void * (*JThreadFunc)(void *);

JThrdHandle JCreateThread( JThreadFunc pFunc, void * pParam, 
                           unsigned int nStackSize );
JThrdRet    JWait4Thrd( JThrdHandle hThrd );

#ifdef WIN32
typedef CRITICAL_SECTION JLockType;
#else
typedef pthread_mutex_t  JLockType;
#endif

void JInitLock  ( JLockType * pLock );
void JDeinitLock( JLockType * pLock );
void JLock( JLockType * pLock );
void JUnlock( JLockType * pLock );

class CJFuncLock
{
public:
    CJFuncLock( JLockType & lock );
    ~CJFuncLock();

private:
    JLockType & m_lock;
};


typedef unsigned int JSocketType;

#define JSOCKET_USER_MSG            0x400

int  JSocketInit();
void JSocketDeinit();

typedef void (*JSocketOnReceive)( JSocketType s, unsigned int nMsgId,
                                  void * pData, unsigned int nDataLen );
typedef void (*JSocketOnAccept)( JSocketType s );

enum eJDisconnectReason
{
    JDCNN_GRACEFULLY_CLOSE,
    JDCNN_UNGRACEFULLY_CLOSE,
    JDCNN_NO_HEART_BEAT,
    JDCNN_ERROR_PACK
};

typedef void (*JSocketOnDisconnect)( JSocketType s, unsigned int nReason );

int JSocketListen ( JSocketType * pSocket, unsigned short nPort,
                    unsigned int nMaxAccepted, JSocketOnAccept onAcceptCb,
                    JSocketOnReceive onReceiveCb, JSocketOnDisconnect onDisconnect );

int JSocketConnect( JSocketType * pSocket, unsigned int  nHostIp, 
                    unsigned short nPort, JSocketOnReceive onReceiveCb, 
                    JSocketOnDisconnect onDisconnect );

int JSocketClose( JSocketType s );
int JSocketSend( JSocketType s, unsigned int nMsgId,
                 void * pData, unsigned int nDataLen );

unsigned int JGetTickCount();  //milliseconds
void JSleep(unsigned int nMiliSeconds);


typedef void * JSemHandle;

int JSemCreate(JSemHandle * pHandle);
int JSemTake(JSemHandle h);
int JSemTakeByTime(JSemHandle h, unsigned int dwtime);
int JSemGive(JSemHandle h);
int JSemDelete(JSemHandle h);


#endif
