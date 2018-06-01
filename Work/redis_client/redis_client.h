#ifndef  _REDIS_CLIENT_2013_6_17_
#define  _REDIS_CLIENT_2013_6_17_

#ifdef WIN32

#ifdef REDIS_CLIENT_EXPORTS
#define REDIS_CLIENT_API __declspec(dllexport)
#else
#define REDIS_CLIENT_API __declspec(dllimport)
#endif

#else // NOT WIN32

#define REDIS_CLIENT_API 

#endif


#ifdef WIN32

#include <Windows.h>

#else

typedef int  BOOL;
typedef unsigned long   DWORD;
typedef unsigned short  WORD;
typedef unsigned char   BYTE;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif



typedef  void *  HRedisClient; // RedisClientHandle;


// 同步链接
REDIS_CLIENT_API  int  ConnectRedis( HRedisClient * ph, const char * szAddr, WORD wPort, DWORD dwTimeout = 5 );

// 同步发送和接收
REDIS_CLIENT_API  int  SendRedisData( HRedisClient h, const void * pSendData, DWORD dwSendSize, 
                                      void * pRecvData, DWORD * pdwReceiveSize, DWORD dwTimeout = 5 );
// 关闭
REDIS_CLIENT_API  int  CloseRedis( HRedisClient h );




typedef void (*POnRedisDataCallback)( void * pData, DWORD dwDataLen, DWORD dwSendId );

typedef void (*POnRedisConnectCallback)( HRedisClient h, BOOL bConnected );

// 异步链接
REDIS_CLIENT_API  int  ConnectRedisAsyn( HRedisClient * ph, const char * szAddr, WORD wPort, void * pContext, POnRedisConnectCallback pCnnCallbk, POnRedisDataCallback pCallback );

// 异步发送
REDIS_CLIENT_API  int  SendRedisDataAsyn( HRedisClient h, const void * pSendData, DWORD dwSendSize, DWORD dwSendId );










typedef  void *  HRedisRequest;


// 统一请求协议
REDIS_CLIENT_API  HRedisRequest  CreateRedisRequest();

// 销毁请求
REDIS_CLIENT_API  int  DestroyRedisRequest( HRedisRequest h );

// 清空请求
REDIS_CLIENT_API  int  ClearRedisRequest( HRedisRequest h );

// 添加请求参数
// 以NULL结束
REDIS_CLIENT_API  int  AddRedisRequestParam( HRedisRequest h, const char * szParam, ... );


typedef  struct  tagRedisRequestParam
{
    char *      m_pParam;
    DWORD       m_dwParamLen;
}TRedisRequestParam, *PTRedisRequestParam;

REDIS_CLIENT_API  int  AddRedisRequestParamEx( HRedisRequest h, PTRedisRequestParam pParams, DWORD dwParamsCnt );



REDIS_CLIENT_API  int    ConvertRedisRequest2Buf( HRedisRequest h, char ** ppResultBuf, DWORD * pdwBufSize );
REDIS_CLIENT_API  int    FreeRedisRequestBuf( char * pBuf );
























// 不设置命名空间的xml
typedef  struct  tagTRedisXmlAttr
{
    char *   m_szAttrName;
    char *   m_szAttrValue;

    struct  tagTRedisXmlAttr *  m_pNextSibling;
}TRedisXmlAttr, *PTRedisXmlAttr;


typedef  struct tagTRedisXmlNode
{
    char *                     m_szName;
    char *                     m_szBody;
    PTRedisXmlAttr             m_pFirstAttr;
    struct tagTRedisXmlNode *  m_pNextSibling;
    struct tagTRedisXmlNode *  m_pFirstChild;
    struct tagTRedisXmlNode *  m_pParent;
}TRedisXmlNode, *PTRedisXmlNode;


REDIS_CLIENT_API  PTRedisXmlNode  CreateRXmlNode( const char * szNodeName );
REDIS_CLIENT_API  BOOL            DestroyRXmlNode( PTRedisXmlNode  pRootNode );

REDIS_CLIENT_API  BOOL            AppendRXmlNode( PTRedisXmlNode pParent, PTRedisXmlNode pChild );
// szAttrValue为NULL，就是销毁
REDIS_CLIENT_API  BOOL            SetRXmlAttr( PTRedisXmlNode  pNode, const char * szAttrName, const char * szAttrValue );
REDIS_CLIENT_API  BOOL            SetRXmlBody( PTRedisXmlNode  pNode, const char * szBody );



REDIS_CLIENT_API  char *   ConvertRXmlNode2Buf( PTRedisXmlNode  pRootNode );
REDIS_CLIENT_API  BOOL     FreeRXmlBuf( char * pBuf );

// 用DestroyRXmlNode销毁
REDIS_CLIENT_API  PTRedisXmlNode   ParseRXmlBuf( const char * pBuf, DWORD dwBufSize );







typedef  enum  ERedisRespType
{
    REDIS_RESP_TYPE_SINGLE_LINE = 0,
    REDIS_RESP_TYPE_NUMBER,
    REDIS_RESP_TYPE_BULK,
    REDIS_RESP_TYPE_MULTI_BULK
};


typedef  struct  tagRedisRespParam
{
    char *     m_pParam;
    DWORD      m_dwParamLen;

    tagRedisRespParam *   m_pNext;
}TRedisRespParam, *PTRedisRespParam;


typedef struct tagTRedisResp
{
    BOOL              m_bError;
    char *            m_szErrorDescription;

    ERedisRespType    m_eType;

    union
    {
        int                m_nNumber;
        PTRedisRespParam   m_pFirstParam;
    }data;
}TRedisResp, *PTRedisResp;


REDIS_CLIENT_API  int  ParseRedisResp( const char * pBuf, DWORD dwBufSize, PTRedisResp * ppResp );

REDIS_CLIENT_API  int  FreeRedisResp( PTRedisResp pResp );









#endif