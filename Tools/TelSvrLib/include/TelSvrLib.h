#ifndef __TELNET_SERVER_2010_01_15__
#define __TELNET_SERVER_2010_01_15__


// ע�ắ���Ĳ�������
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
// ����: ����telnet����
/**************************************************************/
int JTelSvrStart( unsigned short swPort );

/**************************************************************/
// ����: �ر�telnet����
/**************************************************************/
int JTelSvrStop ( );

/**************************************************************/
// ����: ע���Զ���ĺ���
// ע��: ��JTelSvrStart֮ǰע��
/**************************************************************/
int JTelSvrRegCommand( const char * szCmd, const char * szHelpMsg, 
                       LPREG_CMD_FUNC pfunc, ... );

/**************************************************************/
// ����: ��telnet�ͻ��˴�ӡ
/**************************************************************/
int JTelSvrPrint( const char * szFormat, ... );



#endif
