#ifndef _FILE_SHARE_
#define _FILE_SHARE_


// 1. ����˫������֪���Լ���ip��ַ��������ͨ��ĳ�ַ�ʽ����������������ཻ�����Ե���Ϣ�������֪�Է��Լ���ip��ַ�ͼ����Ķ˿ڣ��ļ���С���ļ����ȣ�
// 2. ����������2����һ����ͼconnect��һ��������ɹ����ļ�������С����ʧ�ܣ��ٷ���������һ����ͼ������һ���������ʧ�ܣ�������ȫʧ��
// 3. û����STUN����������������Խ��Э��








typedef  void *  HFileShareSession;

#define LMN_MAX_SESSION_ID_LEN                   64
#define LMN_MAX_USER_ID_LEN                      64
#define LMN_MAX_IP_ADDRESS_COUNT                 16




#define FSS_ON_FAILED_TO_ESTABLISH_CONNECTION      0
#define FSS_ON_START                               1                  // �ļ����俪ʼ
#define FSS_ON_PROGRESS                            2                  // �ļ��������
#define FSS_ON_COMPLETE                            3                  // �ļ��������
#define FSS_ON_ERROR                               4                  // �ļ��������



// nEvent��FSS_ON_FAILED_TO_ESTABLISH_CONNECTION��FSS_ON_ERROR
// FSS_ON_PROGRESSʱ������dwParam��ʾ������ֽ�����
// FSS_ON_ERRORʱ��   ����dwParam��ʾ������
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
    char      szUserId[LMN_MAX_USER_ID_LEN+1];                       // �����߻������id
    char      szPeerId[LMN_MAX_USER_ID_LEN+1];                       // �����߻������id���Զ�id
    char      reserverd0[1];
    BOOL      bSender;                                               // �Ƿ�����
    HFileStream  hFileStream;                                        // �ļ����
    void *    pArgument;                                             // �û��Զ������
    LmnOnFssEvent   pfnOnEvent;                                      // �û��Ļص�
}TCreateFileShareSesionParam, *PTCreateFileShareSesionParam;



typedef struct tagSocketInfo
{
    DWORD     dwIpAddrs[LMN_MAX_IP_ADDRESS_COUNT];                // �����ɹ���listen socket��ip����
    DWORD     dwCount;
    WORD      wPort;                                              // �����ɹ���listen socket��port���֣���Ϊֻ����һ��socket
    char      reserverd0[2];
}TSocketInfo, *PTSocketInfo;








// �ļ������ʼ��
int fssInit();

// �ļ�����ȥ��ʼ��
int fssDeinit();

// ����������
 HFileShareSession  fssCreateSession( INOUT TCreateFileShareSesionParam * ptParam );

// ���������˿�(�󶨵�tcp��ַΪ0)
 int  fssListen( IN HFileShareSession hSession, OUT WORD * pwPort );

// �رռ����˿�
 int  fssCloseListen( IN HFileShareSession hSession );

// ��ͼ�ͶԷ����ཨ��tcpͨ��
 int  fssConnect( IN HFileShareSession hSession, IN const TSocketInfo * ptPeerSocketsInfo );

// ���ٴ�����
 int  fssDestroySession( IN HFileShareSession hSession );








#endif