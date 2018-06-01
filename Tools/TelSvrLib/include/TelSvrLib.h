#ifndef __TELNET_SERVER_2010_01_15__
#define __TELNET_SERVER_2010_01_15__


// 注册函数的参数类型
enum JTelArgType
{
    JTEL_ARG_TYPE_INT  = 1,
    JTEL_ARG_TYPE_TEXT = 2
};

// integer, text arguments
typedef int (* LPREG_CMD_FUNC)(int nCnt, void * args[]);

enum JTelError
{
    JTEL_ERROR_OK           = 0,
    JTEL_ERROR_FAILED_TO_WSASTARTUP,
    JTEL_ERROR_ALREADY_STARTED,
    JTEL_ERROR_FAILED_TO_SOCKET,
    JTEL_ERROR_FAILED_TO_BIND,
    JTEL_ERROR_FAILED_TO_LISTEN,
    JTEL_ERROR_NOT_STARTED,
};



/**************************************************************/
// 功能: 启动telnet服务
/**************************************************************/
int JTelSvrStart( unsigned short swPort );

/**************************************************************/
// 功能: 关闭telnet服务
/**************************************************************/
int JTelSvrStop ( );

/**************************************************************/
// 功能: 注册自定义的函数
// 注意: 在JTelSvrStart之前注册
/**************************************************************/
int JTelSvrRegCommand( const char * szCmd, const char * szHelpMsg, 
                       LPREG_CMD_FUNC pfunc, ... );

/**************************************************************/
// 功能: 在telnet客户端打印
/**************************************************************/
int JTelSvrPrint( const char * szFormat, ... );



#endif
