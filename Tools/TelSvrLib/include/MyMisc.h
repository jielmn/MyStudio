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
// �鿴�ֽ����������ʾ�ֽ�ֵ���ұ���ʾ�ַ�
//////////////////////////////////////////////////////////////////////
char * DebugStream( char* out_buff, unsigned int out_bufLen, 
				    const char * in_stream, unsigned int in_streamLen );

#ifdef WIN32
//////////////////////////////////////////////////////////////////////
// �����ļ�·������ȡ�ļ�����(������չ��)
// ����:
//     c:\test\t\a\a.txt ���� a.txt
//     \test\t\a\b.txt   ���� b.txt
//     c.txt             ���� c.txt
//     ""(��)            ���� ""(��)
const char * GetFileTitle( const char* szFileName );

/////////////////��ȡ�ļ���·��///////////////////////////////////////
// ����:
//     "c:\test\t\a\a.txt" �õ� "c:\test\t\a\"
//     "\test\t\a\b.txt"   �õ� "\test\t\a\"
//     "\"                 �õ� "\"
//     c.txt               �õ� NULL
//     ""(��)              �õ� NULL
//
//  return value: -2, invalid parameters
//                -1, exceeds buff size
//                >=0, characters stored in the buff
int GetFileDirectory( const char* szFileName, 
					  char * szFileDirectory, unsigned int nDirLen );

#endif


int GetRand( int nMin, int nMax );





/************************************************************************/
/* 1. ��λ�Ƿ�����(������0��ʼ)                                         */
/* ����7�Ķ����Ʊ�ʾΪ0111��ǰ��λΪ1������λΪ0                        */
/* IfHasBit(7, 0)����1, IfHasBit(7,3)����0��                            */
/* 2. ������64λ��ULONGLONG����, 32λ��int, 16λ��short int, 8λ��char  */
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
/* ����ĳλ��ֵ                                                         */
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
