#ifndef _FILE_SHARE_
#define _FILE_SHARE_


// 1. 假设双方都能知道自己的ip地址，并且能通过某种方式（例如第三方）互相交换各自的信息（例如告知对方自己的ip地址和监听的端口，文件大小，文件名等）
// 2. 尝试连接有2步：一方试图connect另一方，如果成功，文件传输进行。如果失败，再反过来由另一方试图连接这一方。如果都失败，才算完全失败
// 3. 没有用STUN等其他帮助内网穿越的协议








typedef  void *  HFileShareSession;

#define LMN_MAX_SESSION_ID_LEN                   64
#define LMN_MAX_USER_ID_LEN                      64
#define LMN_MAX_IP_ADDRESS_COUNT                 16




#define FSS_ON_FAILED_TO_ESTABLISH_CONNECTION      0
#define FSS_ON_START                               1                  // 文件传输开始
#define FSS_ON_PROGRESS                            2                  // 文件传输进度
#define FSS_ON_COMPLETE                            3                  // 文件传输结束
#define FSS_ON_ERROR                               4                  // 文件传输错误



// nEvent：FSS_ON_FAILED_TO_ESTABLISH_CONNECTION到FSS_ON_ERROR
// FSS_ON_PROGRESS时，参数dwParam表示传输的字节数字
// FSS_ON_ERROR时，   参数dwParam表示错误码
typedef void (* LmnOnFssEvent )( int nEvent, HFileShareSession hSession, DWORD dwParam, void * pArgument );


#define  FSS_ERROR_SOCKET_LIMIT                    1
#define  FSS_ERROR_FAILED_TO_SEND                  2
#define  FSS_ERROR_FAILED_TO_RECV                  3
#define  FSS_ERROR_WRONG_FORMAT                    4
#define  FSS_ERROR_WRONG_MATCH                     5
#define  FSS_ERROR_FAILED_TO_SET_FILE_POS          6
#define  FSS_ERROR_FAILED_TO_GET_FILE_POS          7
#define  FSS_ERROR_FAILED_TO_READ_FILE             8
#define  FSS_ERROR_FAILED_TO_WRITE_FILE            9
#define  FSS_ERROR_CONNECTION_CLOSED               10



typedef struct tagCreateFileShareSesionParam
{
    char      szSessionId[LMN_MAX_SESSION_ID_LEN+1];                 // session id
    char      szUserId[LMN_MAX_USER_ID_LEN+1];                       // 发送者或接收者id
    char      szPeerId[LMN_MAX_USER_ID_LEN+1];                       // 发送者或接收者id，对端id
    char      reserverd0[1];
    BOOL      bSender;                                               // 是否发送者
    HFileStream  hFileStream;                                        // 文件句柄
    void *    pArgument;                                             // 用户自定义参数
    LmnOnFssEvent   pfnOnEvent;                                      // 用户的回调
}TCreateFileShareSesionParam, *PTCreateFileShareSesionParam;



typedef struct tagSocketInfo
{
    DWORD     dwIpAddrs[LMN_MAX_IP_ADDRESS_COUNT];                // 创建成功的listen socket的ip部分
    DWORD     dwCount;
    WORD      wPort;                                              // 创建成功的listen socket的port部分，因为只创建一个socket
    char      reserverd0[2];
}TSocketInfo, *PTSocketInfo;








// 文件传输初始化
int fssInit();

// 文件传输去初始化
int fssDeinit();

// 创建传输句柄
 HFileShareSession  fssCreateSession( INOUT TCreateFileShareSesionParam * ptParam );

// 创建监听端口(绑定的tcp地址为0)
 int  fssListen( IN HFileShareSession hSession, OUT WORD * pwPort );

// 关闭监听端口
 int  fssCloseListen( IN HFileShareSession hSession );

// 试图和对方互相建立tcp通道
 int  fssConnect( IN HFileShareSession hSession, IN const TSocketInfo * ptPeerSocketsInfo );

// 销毁传输句柄
 int  fssDestroySession( IN HFileShareSession hSession );








#endif