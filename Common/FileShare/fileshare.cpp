
#include <stdio.h>
#include <assert.h>
#include <string.h>
#ifndef WIN32
#include <arpa/inet.h>
#endif

#include <stdlib.h>
#include <time.h>

#include "LmnCommon.h"
#include "fileshare.h"


#ifdef DEBUG

#ifdef WIN32
#define DEBUG_PRINT( f, ... )          printf( f, __VA_ARGS__ )
#else
#define DEBUG_PRINT( f... )                    printf( f )
#endif

#else

#ifdef WIN32
#define DEBUG_PRINT( f, ... )  
#else
#define DEBUG_PRINT( f... )                     
#endif


#endif


#ifdef WIN32
#define SNPRINTF                  _snprintf
#define STRNCPY(d, s, n)          DO_ONCE( strncpy( d, s, (n)-1 ); d[(n)-1] = '\0'; )
#else
#define SNPRINTF                  snprintf
#define STRNCPY(d, s, n)          strncpy( d, s, n )
#endif



static LmnLockType s_fss_lock;
static BOOL        s_thrd_flag = FALSE;                       // 控制工作线程运行的变量
static LmnThrdType s_fss_thrd = 0;                            // 工作线程
static BOOL        s_inited   = FALSE;                        // 记录初始化的变量
static PList       s_fss = 0;                                 // 保存对象的列表
static SelixNode   s_seli_node = 0;


// 连接超时时间，单位：秒
#define  FSS_CONNECTION_TIMEOUT                                  2
// 传输数据超时时间，单位：毫秒
#define  FSS_TRANSFER_TIMEOUT                                    5000

#define  RAND_MIN_PORT                                           1025
#define  RAND_MAX_PORT                                           50000
#define  TRY_RAND_PORT_COUNT                                     20

#define  PROGRESS_TYPE                                           '\x01'
#define  COMPLETE_ACK_TYPE                                       '\x02'
#define  COMPLETE_TYPE                                           '\x03'
#define  DATA_TYPE                                               '\x04'
#define  VALIDATION_TYPE                                         '\x05'
#define  VALIDATION_ACK_TYPE                                     '\x06'
#define  POSITION_TYPE                                           '\x07'


#define  RESULT_FORMAT_ERROR                                     -1
#define  RESULT_CONTINUE                                         0
#define  RESULT_PROGRESS_OK                                      1
#define  RESULT_COMPLETE_ACK_OK                                  2




enum FileShareSessionState
{
    FILE_SHARE_SESSION_NONE = 0,
    FILE_SHARE_SESSION_LISTEN,                 // 监听
    FILE_SHARE_SESSION_CONNECTING,             // 正在连接
    FILE_SHARE_SESSION_CONNECTED,              // 监听端被连上
    FILE_SHARE_SESSION_CONNECT,                // 连接端连上
    FILE_SHARE_SESSION_ESTABLISHED,
};

enum FileShareSessionSubState
{
    SUB_STATE_NONE = 0,
    SUB_STATE_DATA_LEN,                        // 正在获取数据长度
    SUB_STATE_DATA,                            // 正在获取数据
    SUB_STATE_DATA1,                           // 扩展，第二部分数据
    SUB_STATE_END
};


typedef struct tagFileShareSesion
{
    TCreateFileShareSesionParam   tCreateParam;                // 创建时的参数
    FileShareSessionState         eState;                      // 状态
    FileShareSessionSubState      eSubState;                   // 子状态

    WORD                          wListenPort;                 // 监听端口
    char                          reserverd0[2];
    FdType                        fdListen;                    // 对应的fd

    DWORD                         dwConnectingCnt;             // 连接对端时，创建的fd的个数
    FdType                        ConnectingFds[LMN_MAX_IP_ADDRESS_COUNT]; // 连接对端的fd数组
    DWORD                         dwConnectingLeftCnt;         // 连接中的剩余个数
    FdType                        fdConnected;                 // 连接上的fd
    
    LmnCommon::CDataBuf *         pDataBuf;                    // 临时缓存数据的
    DWORD                         dwExpectSize;                // 协议一般先指定数据长度，再传数据，用于保存后面的数据长度

    LmnCommon::CDataBuf *         pSendBuf;                    // 发送缓冲区

    BOOL                          bSendPos;
    BOOL                          bReceivedPos;

    DWORD                         dwTimeTick;
    DWORD                         dwProgress;                  // 发送或者接收的字节数字

    TFileStreamPos                tFilePos;                    // 保存文件位置数据
    BOOL                          bSendComplete;               // 发送方是否发送complete字节


    tagFileShareSesion()
    {
        memset( this, 0, sizeof(*this) );

        DWORD i;
        for ( i = 0; i < LMN_MAX_IP_ADDRESS_COUNT; i++ )
        {
            ConnectingFds[i] = -1;
        }

        fdListen = -1;
        fdConnected = -1;

        pDataBuf = new LmnCommon::CDataBuf;
        pSendBuf = new LmnCommon::CDataBuf;
    }

    ~tagFileShareSesion()
    {
        if ( -1 != fdListen )
        {
            lixClose( s_seli_node, fdListen );
            fdListen = -1;
        }

        DWORD i;
        for ( i = 0; i < LMN_MAX_IP_ADDRESS_COUNT; i++ )
        {
            if ( -1 != ConnectingFds[i] )
            {
                lixClose( s_seli_node, ConnectingFds[i] );
                ConnectingFds[i] = -1;
            }
        }

        if ( -1 != fdConnected )
        {
            lixClose( s_seli_node, fdConnected );
            fdConnected = -1;
        }

        delete pDataBuf;
        delete pSendBuf;
    }

    void Clear()
    {
        dwExpectSize = 0;
        pDataBuf->Clear();
        pSendBuf->Clear();

        if ( -1 != fdListen )
        {
            lixClose( s_seli_node, fdListen );
            fdListen = -1;
        }
        wListenPort = 0;

        if ( -1 != fdConnected )
        {
            lixClose( s_seli_node, fdConnected );
            fdConnected = -1;
        }

        DWORD i;
        for ( i = 0; i < LMN_MAX_IP_ADDRESS_COUNT; i++ )
        {
            if ( -1 != ConnectingFds[i] )
            {
                lixClose(  s_seli_node, ConnectingFds[i] );
                ConnectingFds[i] = -1;
            }
        }
        dwConnectingCnt = 0;
        dwConnectingLeftCnt = 0;

        bSendPos = FALSE;
        bReceivedPos = FALSE;

        dwTimeTick = 0;
        dwProgress = 0;

        eState = FILE_SHARE_SESSION_NONE;
        eSubState = SUB_STATE_NONE;
    }
}TFileShareSesion, *PTFileShareSesion;






static int SureSend_( FdType fd, char * pData, DWORD dwSendSize )
{
    if ( 0 == dwSendSize )
    {
        return 0;
    }

    DWORD dwLeft = dwSendSize;

    int nLen = (int)dwLeft;
    int ret = lixTcpSend( s_seli_node, fd, pData, &nLen );

    while(  0 == ret )
    {
        if ( 0 == nLen )
        {
            LmnSleep( 100 );
            nLen = (int)dwLeft;
            ret = lixTcpSend( s_seli_node, fd, pData, &nLen );
        }
        else
        {
            dwLeft -= nLen;
            if ( dwLeft > 0 )
            {
                pData += nLen;
                nLen = (int)dwLeft;
                ret = lixTcpSend( s_seli_node, fd, pData, &nLen );
            }
            else
            {
                return 0;
            }
        }
    }

    return -1;
}

static char * GetValidationStr_( char * szBuf, DWORD dwBufSize, TFileShareSesion * pSesseion )
{
    if ( pSesseion->tCreateParam.bSender )
    {
        SNPRINTF( szBuf, dwBufSize, "%s%s%s", pSesseion->tCreateParam.szSessionId, pSesseion->tCreateParam.szUserId, 
            pSesseion->tCreateParam.szPeerId );
    }
    else
    {
        SNPRINTF( szBuf, dwBufSize, "%s%s%s", pSesseion->tCreateParam.szSessionId, pSesseion->tCreateParam.szPeerId, 
            pSesseion->tCreateParam.szUserId );
    }
    return szBuf;
}

// 验证字符串 session id + send id + receive id
// 字符串( 4字节 + 字符串 )
static int  SendValidation_( TFileShareSesion * pSesseion, FdType fd )
{
    char buf[4096] = { 0 };

    buf[0] = VALIDATION_TYPE;
    char * pBuf = buf + sizeof(char) + sizeof(WORD);

    GetValidationStr_( pBuf, sizeof(buf) - sizeof(char) - sizeof(WORD), pSesseion );

    DWORD  dwLen  = strlen( pBuf );
    WORD   wLen_n = htons( (WORD)dwLen );

    memcpy( buf + sizeof(char), &wLen_n, sizeof(WORD) );

    return SureSend_( fd, buf, sizeof(char) + sizeof(WORD) + dwLen );
}


// dwSize为0时，全部取出来
// dwSize不为0时，取出最大指定字节数据
static void  ReadData2Buf_( FdType fd, LmnCommon::CDataBuf * pDataBuf, DWORD dwSize = 0 )
{
    char buf[8192];
    int len, ret;

    if ( 0 == dwSize )
    {
        len = sizeof(buf);

        ret = lixTcpRecv( s_seli_node, fd, buf, &len );
        while ( 0 == ret )
        {
            if ( len > 0 )
            {
                pDataBuf->Append( buf, len );
            }
            else
            {
                break;
            }

            len = sizeof(buf);
            ret = lixTcpRecv( s_seli_node, fd, buf, &len );
        }
    }
    else
    {
        assert( dwSize <= sizeof(buf) );
        len = (int)dwSize;

        ret = lixTcpRecv( s_seli_node, fd, buf, &len );
        if ( 0 == ret && len > 0 )
        {
            pDataBuf->Append( buf, len );
        }
    }
    
}



static int  SendValidationOk_( FdType fd )
{
    char  chOk = VALIDATION_ACK_TYPE;

    return SureSend_( fd, &chOk, sizeof(char) );
}


static int SendPos_( TFileShareSesion * pSesseion, FdType fd, TFileStreamPos * pPos )
{
    DWORD dwTmp = 0;
    WORD  wTmp = 0;
    char buf[4096];
    buf[0] = POSITION_TYPE;
    char * pBuf = buf + sizeof(char);

    dwTmp = htonl( pPos->eType );
    memcpy( pBuf, &dwTmp, sizeof(DWORD) );
    pBuf += sizeof(DWORD);

    dwTmp = htonl( pPos->nPos );
    memcpy( pBuf, &dwTmp, sizeof(DWORD) );
    pBuf += sizeof(DWORD);

    DWORD dwPathLen = strlen( pPos->szSubPath );
    wTmp = htons( (WORD)dwPathLen );
    memcpy( pBuf, &wTmp, sizeof(WORD) );
    pBuf += sizeof(WORD);

    if ( dwPathLen > 0 )
    {
        memcpy( pBuf, pPos->szSubPath, dwPathLen );
    }
    
    return SureSend_( fd, buf, sizeof(char) + sizeof(DWORD) * 2 + sizeof(WORD) + dwPathLen );
}

// 0: continue;
// -1: send error
// -2: file stream error

// 数据大小(2字节) + 数据
static int  SendData_( TFileShareSesion * pSesseion, FdType fd )
{
    int nLen = 0;
    int nTmp = 0;
    int ret = 0;
    DWORD  dwBufSize;

    dwBufSize = pSesseion->pSendBuf->GetDataLength();
    if ( 0 != dwBufSize )
    {
        DEBUG_PRINT("== pSesseion->pSendBuf length = %u \n", dwBufSize );

        nLen = dwBufSize;
        ret = lixTcpSend( s_seli_node, fd, (char *)pSesseion->pSendBuf->GetData(), &nLen );
        if ( 0 == ret )
        {
            if ( 0 == nLen )
            {
                return 0;
            }
            // 还没有写完
            else if ( nLen < (int)dwBufSize )
            {
                DEBUG_PRINT("== sended length = %d \n", nLen);

                DWORD dwPos = pSesseion->pSendBuf->GetReadPos();
                pSesseion->pSendBuf->SetReadPos(dwPos + nLen );
                return 0;
            }
            else
            {
                DEBUG_PRINT("== sended length = %d \n", nLen);

                pSesseion->pSendBuf->Clear();
            }
        }
        // error
        else
        {
            return -1;
        }
    }

    if ( pSesseion->bSendComplete )
    {
        return 0;
    }


    // 从文件流里读取数据并发送，发送不完的，保存在缓冲区里
    char buf[1024 * 64];
    buf[0] = DATA_TYPE;

    dwBufSize = sizeof( buf ) - sizeof(WORD) - sizeof(char);
    ret = ReadFileStream( pSesseion->tCreateParam.hFileStream, buf + sizeof(WORD) + sizeof(char), &dwBufSize );

    int ret_send = 0;
    WORD wDataSize = 0, wDataSize_net = 0;
    char * pBuf = 0;

    while ( 0 == ret )
    {
        BOOL bContinue = TRUE;

        DEBUG_PRINT("read file data size = %u \n", dwBufSize );

        // 读取完毕了
        if ( 0 == dwBufSize )
        {
            char chType = COMPLETE_TYPE;
            pBuf = &chType;
            nLen = sizeof(char);
            bContinue = FALSE;

            pSesseion->bSendComplete = TRUE;

            DEBUG_PRINT("send complete! \n");
        }
        else
        {
            wDataSize = (WORD)dwBufSize;
            wDataSize_net = htons( wDataSize );
            memcpy( buf + sizeof(char), &wDataSize_net, sizeof(WORD) );

            pBuf = buf;
            nLen = dwBufSize + sizeof(char) + sizeof(WORD);

            DEBUG_PRINT( "data size = %u \n", dwBufSize );
        }

        nTmp = nLen;
        ret_send = lixTcpSend( s_seli_node, fd, pBuf, &nLen );

        if ( 0 == ret_send )
        {
            // 一点都没有发送，socket缓冲区满了，缓存数据到缓冲区
            if ( 0 == nLen )
            {
                pSesseion->pSendBuf->Append( pBuf, nTmp );
                return 0;
            }
            // 发送了一部分
            else if ( nLen < nTmp )
            {
                DEBUG_PRINT("sended length = %d \n", nLen);
                DEBUG_PRINT("pSesseion->pSendBuf length = %u \n", pSesseion->pSendBuf->GetDataLength() );
                pSesseion->pSendBuf->Append( pBuf + nLen, nTmp - nLen );
                DEBUG_PRINT("## pSesseion->pSendBuf length = %u \n", pSesseion->pSendBuf->GetDataLength() );
                return 0;
            }
            // buf发送完全，接着发送
            else
            {
                DEBUG_PRINT("sended length = %d \n", nLen);

                if ( !bContinue )
                {
                    return 0;
                }

                dwBufSize = sizeof( buf ) - sizeof(WORD) - sizeof(char);
                ret = ReadFileStream( pSesseion->tCreateParam.hFileStream, buf + sizeof(WORD) + sizeof(char), &dwBufSize );
            }
        }
        else
        {
            return -1;
        }
    }

    return -2;
}



// 0: ok
// -1: error to recv
// 1: complete
// -2: format error
// -3: failed to write file
static int  RecvData_( TFileShareSesion * pSesseion, FdType fd, DWORD * pdwWrite )
{
    char buf[8192];
    int nLen, nTmp;
    DWORD & dwWriteCnt = *pdwWrite;
    DWORD dwBufSize = 0;
    int ret_write = 0;
    DWORD i;
    DWORD dwTemp;
    int ret = 0;




    if ( SUB_STATE_NONE == pSesseion->eSubState )
    {
        nLen = sizeof(char);
        int ret = lixTcpRecv( s_seli_node, fd, buf, &nLen );

        if ( 0 != ret )
        {
            return -1;
        }

        if ( 0 == nLen )
        {
            return 0;
        }

        if ( COMPLETE_TYPE == buf[0] )
        {
            return 1;
        }

        if ( DATA_TYPE != buf[0] )
        {
            return -2;
        }

        pSesseion->eSubState = SUB_STATE_DATA_LEN;
    }


    if ( SUB_STATE_DATA_LEN == pSesseion->eSubState )
    {
        assert( pSesseion->pDataBuf->GetDataLength() < sizeof(WORD) );

        nLen = sizeof(WORD) - pSesseion->pDataBuf->GetDataLength();
        int ret = lixTcpRecv( s_seli_node, fd, buf, &nLen );

        if ( 0 != ret )
        {
            return -1;
        }

        pSesseion->pDataBuf->Append( buf, nLen );

        if ( pSesseion->pDataBuf->GetDataLength() < sizeof(WORD) )
        {
            return 0;
        }

        WORD wTemp = 0;
        pSesseion->pDataBuf->Read( &wTemp, sizeof(WORD) );
        pSesseion->dwExpectSize = ntohs(wTemp);

        pSesseion->pDataBuf->Clear();
        pSesseion->eSubState = SUB_STATE_DATA;
    }


    if ( SUB_STATE_DATA == pSesseion->eSubState )
    {
        assert( pSesseion->dwExpectSize > 0 );
        if ( 0 == pSesseion->dwExpectSize )
        {
            pSesseion->eSubState = SUB_STATE_NONE;
            return RecvData_( pSesseion, fd, pdwWrite );
        }

        DEBUG_PRINT("pSesseion->dwExpectSize = %u \n", pSesseion->dwExpectSize);

        DWORD dwCnt     = ( pSesseion->dwExpectSize - 1 )  / sizeof(buf) + 1;
        DWORD dwResidue = pSesseion->dwExpectSize % sizeof(buf);

        for ( i = 0; i < dwCnt; i++ )
        {
            if ( i < dwCnt - 1 )
            {
                nLen = (int)sizeof(buf);
            }
            else
            {
                nLen = (int)dwResidue;
            }

            nTmp = nLen;

            ret = lixTcpRecv( s_seli_node, fd, buf, &nLen );
            if ( 0 != ret )
            {
                return -1;
            }

            if ( 0 == nLen )
            {
                return 0;
            }

            DEBUG_PRINT("recv length = %d \n", nLen);

            pSesseion->dwExpectSize -= (DWORD)nLen;

            // 写文件
            dwBufSize = (DWORD)nLen;

            //DEBUG_PRINT("WriteFileStream, dwBufSize = %u \n", dwBufSize );
            //for ( i = 0; i < dwBufSize; i++ )
            //{
            //    DEBUG_PRINT("\\x%02x", (unsigned char)buf[i] );
            //}
            //DEBUG_PRINT("\n");

            ret_write = WriteFileStream( pSesseion->tCreateParam.hFileStream, buf, &dwBufSize, &dwTemp );
            if ( 0 == ret_write )
            {
                dwWriteCnt += dwTemp;
            }
            else
            {
                return -3;
            }

            if ( nLen < nTmp )
            {
                return 0;
            }
        }


        
        pSesseion->eSubState = SUB_STATE_NONE;
        return RecvData_(pSesseion, fd, pdwWrite);
    }

    assert( 0 );
    return 0;
}

static int  SendProgress_( FdType fd, DWORD dwWriteLen )
{
    char buf[4096];
    buf[0] = PROGRESS_TYPE;

    DWORD  dwLen_n = htonl( dwWriteLen );
    memcpy( buf + sizeof(char), &dwLen_n, sizeof(DWORD) );

    return SureSend_( fd, buf, sizeof(char) + sizeof(DWORD) );
}

static int  SendCompleteAck_( FdType fd )
{
    char buf[4096];
    buf[0] = COMPLETE_ACK_TYPE;

    return SureSend_( fd, buf, sizeof(char) );
}

static void OnSocketEvent_( SelixNode pSeli, FdType fd, int liEvent, int error, void * context )
{
    LmnCommon::CFuncLock theLock( &s_fss_lock );
    TFileShareSesion * pSesseion = (TFileShareSesion *)context;

    switch( liEvent )
    {
    case liEvAccept:
        {
            if ( FILE_SHARE_SESSION_LISTEN == pSesseion->eState )
            {
                FdType fdAccept = lixAccept( pSeli, fd, OnSocketEvent_, pSesseion );

                // 失败了
                if ( -1 == fdAccept )
                {
                    // 关闭监听端口
                    pSesseion->Clear();

                    // accept失败
                    pSesseion->tCreateParam.pfnOnEvent( FSS_ON_FAILED_TO_ESTABLISH_CONNECTION, pSesseion, FSS_ERROR_SOCKET_LIMIT, pSesseion->tCreateParam.pArgument );
                    break;
                }

                pSesseion->eState = FILE_SHARE_SESSION_CONNECTED;
                pSesseion->fdConnected = fdAccept;

                // 关闭监听端口
                lixClose( pSeli, fd );
                pSesseion->fdListen = -1;

                // 设置检查时间点
                pSesseion->dwTimeTick = LmnGetTickCount();
            }
        }
        break;

    case liEvConnect:
        {
            assert( FILE_SHARE_SESSION_CONNECTING == pSesseion->eState );

            if ( FILE_SHARE_SESSION_CONNECTING == pSesseion->eState )
            {
                // 设置状态为“主动连接上”
                pSesseion->eState = FILE_SHARE_SESSION_CONNECT;
                pSesseion->fdConnected = fd;


                // 关闭其他的fd集合
                DWORD i;
                for ( i = 0; i < pSesseion->dwConnectingCnt; i++ )
                {
                    if ( (pSesseion->ConnectingFds[i] != fd) && (-1 != pSesseion->ConnectingFds[i]) )
                    {
                        lixClose( pSeli, pSesseion->ConnectingFds[i] );
                    }

                     pSesseion->ConnectingFds[i] = -1;
                }
                pSesseion->dwConnectingCnt = 0;
                pSesseion->dwConnectingLeftCnt = 0;

                // 验证消息类型: VALIDATION_TYPE
                // 发送验证字符串
                // session id + sender id + receiver id
                if ( 0 != SendValidation_( pSesseion, fd ) )
                {
                    pSesseion->Clear();
                    pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_FAILED_TO_SEND, pSesseion->tCreateParam.pArgument );
                    break;
                }


                // 设置检查时间点
                pSesseion->dwTimeTick = LmnGetTickCount();
            }

        }
        break;

    case liEvRead:
        {
            int ret = 0;
            const char * pData = 0;
            DWORD dwTmp = 0;
            WORD  wTmp = 0;


            // 如果是被连接上(监听方)
            if ( FILE_SHARE_SESSION_CONNECTED == pSesseion->eState )
            {
                // 设置检查时间点
                pSesseion->dwTimeTick = LmnGetTickCount();

                if ( SUB_STATE_NONE == pSesseion->eSubState )
                {
                    // 取数据到缓冲区
                    ReadData2Buf_( fd, pSesseion->pDataBuf, 1 );
                    pData = (const char *)pSesseion->pDataBuf->GetData();

                    // 继续等数据
                    if ( 0 == pSesseion->pDataBuf->GetDataLength() )
                    {
                        break;
                    }

                    // 验证类型不对
                    if ( VALIDATION_TYPE != *pData )
                    {
                        assert( fd == pSesseion->fdConnected );
                        pSesseion->Clear();

                        pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_WRONG_FORMAT, pSesseion->tCreateParam.pArgument );
                        break;
                    }

                    pSesseion->pDataBuf->Clear();
                    pSesseion->eSubState = SUB_STATE_DATA_LEN;
                }

                // 自己的验证字符串
                char szMyValidationStr[4096];
                GetValidationStr_( szMyValidationStr, sizeof(szMyValidationStr), pSesseion );
                DWORD dwMyValidationStrLen = strlen(szMyValidationStr);


                // 继续读取验证字符串长度
                if ( SUB_STATE_DATA_LEN == pSesseion->eSubState )
                {
                    DWORD dwLen = pSesseion->pDataBuf->GetDataLength();

                    // 取数据到缓冲区
                    ReadData2Buf_( fd, pSesseion->pDataBuf, sizeof(WORD) - dwLen );
                    pData = (const char *)pSesseion->pDataBuf->GetData();

                    // 数据还未接收完全
                    if ( pSesseion->pDataBuf->GetDataLength() < sizeof(WORD) )
                    {
                        break;
                    }
                    
                    pSesseion->pDataBuf->Read( &wTmp, sizeof(WORD) );
                    pSesseion->dwExpectSize = (DWORD)ntohs(wTmp);

                    // 长度不对
                    if ( pSesseion->dwExpectSize != dwMyValidationStrLen )
                    {
                        assert( fd == pSesseion->fdConnected );
                        pSesseion->Clear();

                        pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_WRONG_MATCH, pSesseion->tCreateParam.pArgument );
                        break;
                    }

                    pSesseion->pDataBuf->Clear();
                    pSesseion->eSubState = SUB_STATE_DATA;
                }

                
                // 验证字符串
                if ( SUB_STATE_DATA == pSesseion->eSubState )
                {
                    DWORD dwLen = pSesseion->pDataBuf->GetDataLength();

                    // 取数据到缓冲区
                    ReadData2Buf_( fd, pSesseion->pDataBuf, pSesseion->dwExpectSize - dwLen );
                    pData = (const char *)pSesseion->pDataBuf->GetData();

                    // 数据还未接收完全
                    if ( pSesseion->pDataBuf->GetDataLength() < pSesseion->dwExpectSize )
                    {
                        break;
                    }

                    // 数据不对
                    if ( 0 != memcmp( pData, szMyValidationStr, dwMyValidationStrLen ) )
                    {
                        assert( fd == pSesseion->fdConnected );
                        pSesseion->Clear();

                        pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_WRONG_MATCH, pSesseion->tCreateParam.pArgument );
                        break;
                    }

                    pSesseion->dwExpectSize = 0;
                    pSesseion->pDataBuf->Clear();

                    // 发送验证成功
                    if ( 0 != SendValidationOk_(fd) )
                    {
                        assert( fd == pSesseion->fdConnected );
                        pSesseion->Clear();

                        pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_FAILED_TO_SEND, pSesseion->tCreateParam.pArgument );
                        break;
                    }

                    // 更改状态
                    pSesseion->eState = FILE_SHARE_SESSION_ESTABLISHED;
                    pSesseion->eSubState = SUB_STATE_NONE;

                    // on  start
                    pSesseion->tCreateParam.pfnOnEvent( FSS_ON_START, pSesseion, 0, pSesseion->tCreateParam.pArgument );
                }
            }
            // 如果是主动连接上
            else if ( FILE_SHARE_SESSION_CONNECT == pSesseion->eState )
            {
                // 设置检查时间点
                pSesseion->dwTimeTick = LmnGetTickCount();


                // 读取一个字节
                ReadData2Buf_( fd, pSesseion->pDataBuf, 1 );
                pData = (const char *)pSesseion->pDataBuf->GetData();

                // 没有数据，继续等
                if ( 0 == pSesseion->pDataBuf->GetDataLength() )
                {
                    break;
                }

                // 类型不对
                if ( VALIDATION_ACK_TYPE != *pData )
                {
                    assert( fd == pSesseion->fdConnected );
                    pSesseion->Clear();

                    pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_WRONG_FORMAT, pSesseion->tCreateParam.pArgument );
                    break;
                }

                // 更改状态
                pSesseion->eState = FILE_SHARE_SESSION_ESTABLISHED;
                assert( pSesseion->eSubState == SUB_STATE_NONE );
                pSesseion->pDataBuf->Clear();

                // on  start
                pSesseion->tCreateParam.pfnOnEvent( FSS_ON_START, pSesseion, 0, pSesseion->tCreateParam.pArgument );
            }


            if ( FILE_SHARE_SESSION_ESTABLISHED == pSesseion->eState )
            {
                // 设置检查时间点
                pSesseion->dwTimeTick = LmnGetTickCount();


                // 如果是文件共享的发送方，准备发送数据
                // 发送方也接收接收方的进度通知
                // 发送方也接收文件位置通知
                if ( pSesseion->tCreateParam.bSender )
                {
                    if ( !pSesseion->bReceivedPos )
                    {
                        if ( SUB_STATE_NONE == pSesseion->eSubState )
                        {
                            // 取数据到缓冲区
                            ReadData2Buf_( fd, pSesseion->pDataBuf, 1 );
                            pData = (const char *)pSesseion->pDataBuf->GetData();

                            // 没有数据，继续等
                            if ( 0 == pSesseion->pDataBuf->GetDataLength() )
                            {
                                break;
                            }

                            if ( POSITION_TYPE != *pData )
                            {
                                assert( fd == pSesseion->fdConnected );
                                pSesseion->Clear();

                                pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_WRONG_FORMAT, pSesseion->tCreateParam.pArgument );
                                break;
                            }

                            pSesseion->pDataBuf->Clear();
                            pSesseion->eSubState = SUB_STATE_DATA;
                        }
                
                        if ( SUB_STATE_DATA == pSesseion->eSubState )
                        {
                            DWORD dwLen = pSesseion->pDataBuf->GetDataLength();
                            pSesseion->dwExpectSize = sizeof(DWORD) * 2 + sizeof(WORD);

                            // 取数据到缓冲区
                            ReadData2Buf_( fd, pSesseion->pDataBuf, pSesseion->dwExpectSize - dwLen );
                            pData = (const char *)pSesseion->pDataBuf->GetData();

                            // 数据还未接收完全
                            if ( pSesseion->pDataBuf->GetDataLength() < pSesseion->dwExpectSize )
                            {
                                break;
                            }

                            pSesseion->pDataBuf->Read( (unsigned char *)&dwTmp, sizeof(DWORD) );
                            pSesseion->tFilePos.eType = (ELmnFileType)ntohl( dwTmp );

                            pSesseion->pDataBuf->Read( (unsigned char *)&dwTmp, sizeof(DWORD) );
                            pSesseion->tFilePos.nPos = ntohl( dwTmp );

                            pSesseion->pDataBuf->Read( (unsigned char *)&wTmp, sizeof(WORD) );
                            pSesseion->dwExpectSize = ntohs( wTmp );
                            pSesseion->pDataBuf->Clear();
                            pSesseion->eSubState = SUB_STATE_DATA1;

                        }

                        if ( SUB_STATE_DATA1 == pSesseion->eSubState )
                        {
                            DWORD dwLen = pSesseion->pDataBuf->GetDataLength();

                            // 取数据到缓冲区
                            ReadData2Buf_( fd, pSesseion->pDataBuf, pSesseion->dwExpectSize - dwLen );
                            pData = (const char *)pSesseion->pDataBuf->GetData();

                            // 数据还未接收完全
                            if ( pSesseion->pDataBuf->GetDataLength() < pSesseion->dwExpectSize )
                            {
                                break;
                            }

                            pSesseion->pDataBuf->Read( (unsigned char *)pSesseion->tFilePos.szSubPath, pSesseion->dwExpectSize );
                            assert( pSesseion->dwExpectSize < LMN_MAX_FILE_PATH );
                            pSesseion->tFilePos.szSubPath[pSesseion->dwExpectSize] = '\0';


                            if ( 0 != SetFileStreamPos( pSesseion->tCreateParam.hFileStream, &pSesseion->tFilePos ) )
                            {
                                assert( fd == pSesseion->fdConnected );
                                pSesseion->Clear();

                                pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_FAILED_TO_SET_FILE_POS, pSesseion->tCreateParam.pArgument );
                                break;
                            }

                            pSesseion->dwExpectSize = 0;
                            pSesseion->pDataBuf->Clear();
                            pSesseion->eSubState = SUB_STATE_NONE;
                            pSesseion->bReceivedPos = TRUE;
                        }
                    }
                    else
                    {
                        // 接收进度数据
                        do 
                        {
                            if ( SUB_STATE_NONE == pSesseion->eSubState )
                            {
                                // 取数据到缓冲区
                                ReadData2Buf_( fd, pSesseion->pDataBuf, 1 );
                                pData = (const char *)pSesseion->pDataBuf->GetData();

                                // 没有数据
                                if ( 0 == pSesseion->pDataBuf->GetDataLength() )
                                {
                                    break;
                                }

                                if ( COMPLETE_ACK_TYPE == *pData )
                                {
                                    // on complete
                                    pSesseion->tCreateParam.pfnOnEvent( FSS_ON_COMPLETE, pSesseion, 0, pSesseion->tCreateParam.pArgument );

                                    assert( fd == pSesseion->fdConnected );
                                    pSesseion->Clear();
                                    return;
                                }

                                // 类型不对
                                if ( PROGRESS_TYPE != *pData )
                                {
                                    assert( fd == pSesseion->fdConnected );
                                    pSesseion->Clear();

                                    pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_WRONG_FORMAT, pSesseion->tCreateParam.pArgument );
                                    return;
                                }

                                pSesseion->pDataBuf->Clear();
                                pSesseion->eSubState = SUB_STATE_DATA;
                            }

                            if ( SUB_STATE_DATA == pSesseion->eSubState )
                            {
                                DWORD dwLen = pSesseion->pDataBuf->GetDataLength();
                                pSesseion->dwExpectSize = sizeof(DWORD);

                                // 取数据到缓冲区
                                ReadData2Buf_( fd, pSesseion->pDataBuf, pSesseion->dwExpectSize - dwLen );
                                pData = (const char *)pSesseion->pDataBuf->GetData();

                                // 数据还未接收完全
                                if ( pSesseion->pDataBuf->GetDataLength() < pSesseion->dwExpectSize )
                                {
                                    break;
                                }

                                pSesseion->pDataBuf->Read( (unsigned char *)&dwTmp, sizeof(DWORD) );
                                DWORD dwProgress = ntohl(dwTmp);

                                pSesseion->dwProgress += dwProgress;

                                // on progress
                                pSesseion->tCreateParam.pfnOnEvent( FSS_ON_PROGRESS, pSesseion, pSesseion->dwProgress, pSesseion->tCreateParam.pArgument );

                                pSesseion->dwExpectSize = 0;
                                pSesseion->pDataBuf->Clear();
                                pSesseion->eSubState = SUB_STATE_NONE;
                            }

                        } while ( TRUE );
                    }
                    

                    // 尽可能发送多的数据
                    ret = SendData_( pSesseion, fd );
                    if ( 0 != ret )
                    {
                        assert( fd == pSesseion->fdConnected );
                        pSesseion->Clear();

                        int nError = ( -1 == ret ? FSS_ERROR_FAILED_TO_SEND : FSS_ERROR_FAILED_TO_READ_FILE );
                        pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, nError, pSesseion->tCreateParam.pArgument );
                        break;
                    }
                }
                // 接收方
                else
                {
                    // 如果没有发送pos，发送pos
                    if ( !pSesseion->bSendPos )
                    {
                        TFileStreamPos tPos;
                        ret = GetFileStreamPos( pSesseion->tCreateParam.hFileStream, &tPos );
                        if ( 0 != ret )
                        {
                            assert( fd == pSesseion->fdConnected );
                            pSesseion->Clear();

                            pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_FAILED_TO_GET_FILE_POS, pSesseion->tCreateParam.pArgument );
                            break;
                        }

                        if ( 0 != SendPos_( pSesseion, fd, &tPos ) )
                        {
                            assert( fd == pSesseion->fdConnected );
                            pSesseion->Clear();

                            pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_FAILED_TO_SEND, pSesseion->tCreateParam.pArgument );
                            break;
                        }

                        pSesseion->bSendPos = TRUE;
                        break;
                    }



                    dwTmp = 0;
                    ret = RecvData_( pSesseion, fd, &dwTmp );
                    if ( ret < 0 )
                    {
                        assert( fd == pSesseion->fdConnected );
                        pSesseion->Clear();

                        int nError = FSS_ERROR_FAILED_TO_RECV;
                        if ( -2 == ret )
                        {
                            nError = FSS_ERROR_WRONG_FORMAT;
                        }
                        else if ( -3 == ret )
                        {
                            nError = FSS_ERROR_FAILED_TO_WRITE_FILE;
                        }

                        pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, nError, pSesseion->tCreateParam.pArgument );
                        break;
                    }


                    // 回复进度
                    if ( dwTmp > 0 )
                    {
                        if ( 0 != SendProgress_( fd, dwTmp ) )
                        {
                            assert( fd == pSesseion->fdConnected );
                            pSesseion->Clear();

                            pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_FAILED_TO_SEND, pSesseion->tCreateParam.pArgument );
                            break; 
                        }
                    }


                    if ( 1 == ret )
                    {
                        SendCompleteAck_( fd );

                        // on complete
                        pSesseion->tCreateParam.pfnOnEvent( FSS_ON_COMPLETE, pSesseion, 0, pSesseion->tCreateParam.pArgument );
                        pSesseion->Clear();
                        break;
                    }

                    pSesseion->dwProgress += dwTmp;
                    // on progress
                    pSesseion->tCreateParam.pfnOnEvent( FSS_ON_PROGRESS, pSesseion, pSesseion->dwProgress, pSesseion->tCreateParam.pArgument );
                }
            }
        }
        break;

    case liEvClose:
        {
            DEBUG_PRINT("== liEvClose! fd = %d \n", fd );

            assert( FILE_SHARE_SESSION_NONE   != pSesseion->eState );
            assert( FILE_SHARE_SESSION_LISTEN != pSesseion->eState );

            lixClose( pSeli, fd );
            pSesseion->fdConnected = -1;

            if ( FILE_SHARE_SESSION_CONNECTING == pSesseion->eState )
            {
                assert( pSesseion->dwConnectingLeftCnt );
                pSesseion->dwConnectingLeftCnt--;

                DEBUG_PRINT("pSesseion->dwConnectingLeftCnt = %u \n", pSesseion->dwConnectingLeftCnt);


                // 前面已经关闭socket，现在直接设置为-1好了
                DWORD i;
                for ( i = 0; i < pSesseion->dwConnectingCnt; i++ )
                {
                    if ( pSesseion->ConnectingFds[i] == fd )
                    {
                        pSesseion->ConnectingFds[i] = -1;
                    }
                }

                // 所有的远程连接都没有连接上
                if ( 0 == pSesseion->dwConnectingLeftCnt )
                {
                    pSesseion->tCreateParam.pfnOnEvent( FSS_ON_FAILED_TO_ESTABLISH_CONNECTION, pSesseion, 0, pSesseion->tCreateParam.pArgument );
                    pSesseion->eState = FILE_SHARE_SESSION_NONE;
                }
            }
            else
            {
                pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_CONNECTION_CLOSED, pSesseion->tCreateParam.pArgument );
                pSesseion->eState = FILE_SHARE_SESSION_NONE;
            }
        }
        break;
    }
}

static void  CheckTimeout_()
{
    DWORD dwTick = LmnGetTickCount();
    LmnCommon::CFuncLock theLock( &s_fss_lock );

    PListNode pNode =  GetListHead( s_fss );
    while( pNode )
    {
        TFileShareSesion * pSesseion = (TFileShareSesion *)pNode->pData;
        if (     FILE_SHARE_SESSION_CONNECT == pSesseion->eState || FILE_SHARE_SESSION_CONNECTED == pSesseion->eState 
              || FILE_SHARE_SESSION_ESTABLISHED == pSesseion->eState )
        {
            if ( dwTick - pSesseion->dwTimeTick >= FSS_TRANSFER_TIMEOUT )
            {
                pSesseion->tCreateParam.pfnOnEvent( FSS_ON_ERROR, pSesseion, FSS_ERROR_CONNECTION_CLOSED, pSesseion->tCreateParam.pArgument );
                pSesseion->Clear();
            }
        }
        //else if ( FILE_SHARE_SESSION_LISTEN == pSesseion->eState )
        //{
        //    if ( dwTick - pSesseion->dwTimeTick >= FSS_CONNECTION_TIMEOUT * 1000 )
        //    {
        //        pSesseion->tCreateParam.pfnOnEvent( FSS_ON_FAILED_TO_ESTABLISH_CONNECTION, pSesseion, 0, pSesseion->tCreateParam.pArgument );
        //        pSesseion->Clear();
        //    }
        //}

        pNode = GetNextListNode( pNode );
    }
}

static void * fssThread_(void * pArg)
{
    while( s_thrd_flag )
    {
        DWORD dwTime = 5;

        selixSelect( s_seli_node, &dwTime );
        CheckTimeout_();

        if ( dwTime > 0 )
        {
            LmnSleep( dwTime );
        }
    }
    return 0;
}








// 文件传输初始化
int fssInit()
{
    // 如果已经初始化了
    if ( s_inited )
    {
        return -1;
    }

    // 初始化简单的select模式
    const DWORD dwSocketsCnt = 64;
    int ret = selixInit( dwSocketsCnt );
    if ( LMN_ERR_OK != ret )
    {
        return -1;
    }

    DWORD dwTmp = 32;
    s_seli_node = selixCreateNode( &dwTmp );
    if ( 0 == s_seli_node )
    {
        return -1;
    }

    s_thrd_flag = TRUE;
    LmnInitLock( &s_fss_lock );

    s_fss = InitList();

    s_fss_thrd = LmnCreateThread( fssThread_, 0, FALSE );

    s_inited = TRUE;
    return 0;
}


// 文件传输去初始化
int fssDeinit()
{
    if ( !s_inited )
    {
        return -1;
    }

    // 等待工作线程结束
    s_thrd_flag = FALSE;
    LmnWait4Thrd( s_fss_thrd );
    s_fss_thrd = 0;
    

    // 后面完善释放
    PListNode pNode = GetListHead( s_fss );
    while ( pNode )
    {
        TFileShareSesion * pSession = (TFileShareSesion *)pNode->pData;
        delete pSession;

        pNode = GetNextListNode( pNode );
    }
    DeinitList( s_fss );
    s_fss = 0;

    LmnDeinitLock( &s_fss_lock );

    selixDestroyNode( s_seli_node );
    s_seli_node = 0;

    s_inited = FALSE;

    selixEnd();

    return 0;
}


// 创建传输句柄
 HFileShareSession  fssCreateSession( INOUT TCreateFileShareSesionParam * ptParam )
{
    // 参数检查
    if ( 0 == ptParam )
    {
        return 0;
    }

    if ( '\0' == ptParam->szSessionId[0] || '\0' == ptParam->szUserId[0] || '\0' == ptParam->szPeerId[0] )
    {
        return 0;
    }

    if ( 0 == ptParam->pfnOnEvent )
    {
        return 0;
    }

    if ( 0 == ptParam->hFileStream )
    {
        return 0;
    }

    if ( !s_inited )
    {
        return 0;
    }



    // 创建句柄
    TFileShareSesion * pSession = new TFileShareSesion;

    // 保存参数
    memcpy( &pSession->tCreateParam, ptParam, sizeof(TCreateFileShareSesionParam) );

    LmnCommon::CFuncLock theLock( &s_fss_lock );
    Insert2ListTail( s_fss, pSession );

    return pSession;
}



// 创建监听端口(绑定的tcp地址为0)
 int  fssListen( IN HFileShareSession hSession, OUT WORD * pwPort )
{
    // 参数检查
    if ( 0 == hSession || 0 == pwPort )
    {
        return -1;
    }

    if ( !s_inited )
    {
        return -1;
    }

    TFileShareSesion * pSession = (TFileShareSesion *)hSession;

    if ( FILE_SHARE_SESSION_NONE != pSession->eState )
    {
        return -1;
    }

    InitRand();

    LmnCommon::CFuncLock theLock( &s_fss_lock );

    // 获取随机端口，开始监听
    int nCnt = 0;
    pSession->wListenPort = (WORD)GetRand( RAND_MIN_PORT, RAND_MAX_PORT );
    pSession->fdListen = lixOpen( s_seli_node, LI_PROTOTYPE_TCP, 0, pSession->wListenPort, OnSocketEvent_, pSession, 0 );
    // 因为没有设置错误原因，所以暂时是尝试20次
    while( TRUE )
    {
        // socket创建成功
        if ( -1 != pSession->fdListen )
        {
            // 监听成功
            if ( 0 == lixListen( s_seli_node, pSession->fdListen, 10 ) )
            {
                break;
            }
        }
        nCnt++;

        if ( -1 != pSession->fdListen )
        {
            lixClose( s_seli_node, pSession->fdListen );
            pSession->fdListen = -1;
        }

        // 超出尝试次数
        if ( nCnt >= TRY_RAND_PORT_COUNT )
        {
            break;
        }

        pSession->wListenPort = (WORD)GetRand( RAND_MIN_PORT, RAND_MAX_PORT );
        pSession->fdListen = lixOpen( s_seli_node, LI_PROTOTYPE_TCP, 0, pSession->wListenPort, OnSocketEvent_, pSession, 0 );
    }

    // 最终没有创建成功
    if ( -1 == pSession->fdListen )
    {
        return -1;
    }

    // 设置状态为listen状态
    pSession->eState = FILE_SHARE_SESSION_LISTEN;

    *pwPort = pSession->wListenPort;

    return 0;
}

// 关闭监听端口
 int  fssCloseListen( IN HFileShareSession hSession )
{
    if ( 0 == hSession )
    {
        return -1;
    }

    if ( !s_inited )
    {
        return -1;
    }

    TFileShareSesion * pSession = (TFileShareSesion *)hSession;


    LmnCommon::CFuncLock theLock( &s_fss_lock );

    // 如果不是无效的socket，就关闭它
    if ( -1 != pSession->fdListen )
    {
        liClose( pSession->fdListen );
    }

    pSession->fdListen = -1;
    pSession->wListenPort = 0;

    return LMN_ERR_OK;
}



// 试图和对方互相建立tcp通道
 int  fssConnect( IN HFileShareSession hSession, IN const TSocketInfo * ptPeerSocketsInfo )
{
    if ( 0 == hSession || 0 == ptPeerSocketsInfo )
    {
        return -1;
    }

    if ( 0 == ptPeerSocketsInfo->dwCount || 0 == ptPeerSocketsInfo->wPort )
    {
        return -1;
    }

    if ( !s_inited )
    {
        return -1;
    }


    TFileShareSesion * pSession = (TFileShareSesion *)hSession;

    if ( FILE_SHARE_SESSION_NONE != pSession->eState )
    {
        return -1;
    }

    LmnCommon::CFuncLock theLock( &s_fss_lock );

    pSession->eState = FILE_SHARE_SESSION_CONNECTING;
    pSession->dwConnectingCnt = ( ptPeerSocketsInfo->dwCount < LMN_MAX_IP_ADDRESS_COUNT ? ptPeerSocketsInfo->dwCount : LMN_MAX_IP_ADDRESS_COUNT ) ;

    DWORD i, j;
    for ( i = 0; i < ptPeerSocketsInfo->dwCount; i++ )
    {
        
        FdType fd = lixOpen( s_seli_node, LI_PROTOTYPE_TCP, 0, 0, OnSocketEvent_, pSession, 0 );
        if ( -1 == fd )
        {
            // 关闭前面的端口
            for ( j = 0; j < i; j++ )
            {
                lixClose( s_seli_node, pSession->ConnectingFds[j] );
                pSession->ConnectingFds[j] = -1;
            }
            pSession->dwConnectingCnt = 0;
            return -1;
        }

        // DEBUG_PRINT("================== fd = %d \n", fd );
        if ( 0 != lixConnect( s_seli_node, fd, ptPeerSocketsInfo->dwIpAddrs[i], ptPeerSocketsInfo->wPort, FSS_CONNECTION_TIMEOUT ) )
        {
            lixClose( s_seli_node, fd );

            // 关闭前面的端口
            for ( j = 0; j < i; j++ )
            {
                lixClose( s_seli_node, pSession->ConnectingFds[j] );
                pSession->ConnectingFds[j] = -1;
            }
            pSession->dwConnectingCnt = 0;

            return -1;
        }

        pSession->ConnectingFds[i] = fd;
    }

    // 保存活跃的socket个数
    pSession->dwConnectingLeftCnt = pSession->dwConnectingCnt;

    return 0;
}

// 销毁传输句柄
 int  fssDestroySession( IN HFileShareSession hSession )
 {
     // 参数检查
     if ( 0 == hSession )
     {
         return -1;
     }

     if ( !s_inited )
     {
         return -1;
     }

     TFileShareSesion * pSession = (TFileShareSesion *)hSession;

     LmnCommon::CFuncLock theLock( &s_fss_lock );

     PListNode pNode =  GetListHead( s_fss );
     while( pNode )
     {
         TFileShareSesion * pItem = (TFileShareSesion *)pNode->pData;

         // 找到
         if ( pItem == pSession )
         {
            delete pItem;
            EraseList( s_fss, pNode );
            return 0;
         }

         pNode = GetNextListNode( pNode );
     }

     // 没有找到
     return -1;
 }