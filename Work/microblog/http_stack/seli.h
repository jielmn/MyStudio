#ifndef  __SELI_2012_10_24__
#define  __SELI_2012_10_24__

#include "inner.h"

/*************************    RADVISION形式 协议栈   *************************************/

#define liEvRead    0x01
#define liEvWrite   0x02
#define liEvExcept  0x04
#define liEvAccept  0x08
#define liEvConnect 0x10
#define liEvClose   0x20

typedef int FdType;
typedef void(*LPLIEVENTHANDLER)( FdType fd, int liEvent, int error, void * context );




  int  seliInit( DWORD dwMaxSocketCnt );
  BOOL IsSeliInited();
  int  seliEnd( );


// select多路复用
// nMs单位， 毫秒 (20~1000)
  int  seliSelect( DWORD * pdwMs );


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



// 返回值： socket
  FdType  liOpen ( liProtocol_t protocol, DWORD ipAddr, WORD wPort, 
                               LPLIEVENTHANDLER eventHandler, void* context, 
                               BOOL bTcpKeepAlive );

// 返回值： 0  OK
//          -1 ERROR
  int  liClose ( FdType fd );

// dwTimeOut 单位:秒
// 返回值： 0  OK
//          -1 ERROR
  int  liConnect ( int fd, DWORD ipAddr, WORD wPort, DWORD dwTimeOut );

// 返回值： 0  OK
//          -1 ERROR
  int liListen(int fd, int queueLen );

// 返回值： socket
  FdType liAccept(int fd, LPLIEVENTHANDLER fnCallBack, void * context);

// 返回值： 0  OK
//          -1 ERROR
  int liShutdown( int fd );

// 返回值： 0  OK
//          -1 ERROR
  int liTcpSend (int fd, const char *buff, int * plen );

// 返回值： 0  OK
//          -1 ERROR
  int liTcpRecv (int fd, char *buff,int * plen);

// 返回值：
//          -1 ERROR
  int liGetState (int fd);




/*************************    END RADVISION形式 协议栈   *************************************/





/*********************** li extension *************************/

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


// 连接sock5服务
// 如果参数bUseSock5为True，则使用sock5 proxy
// 否者不使用sock5 proxy
// 
// 说明： lxConnectSock5 调用了liOpen，所以不需要再调用liOpen
  FdType  lxConnectSock5 ( const TCnnSock5Param *  ptParam );




/*********************** end li extension *************************/





#endif



