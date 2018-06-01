#include <assert.h>

#include "inner.h"
#include "seli.h"
#include "httpstack.h"
#include "ssl_adapter.h"


#ifndef WIN32
#include <arpa/inet.h>
#include <netdb.h>
#define  INVALID_SOCKET  -1
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1600
#include <iterator>
#endif

#define MIN( a, b )   ( (a) < (b) ? (a) : (b) )


CHttp * CHttp::s_pInstance = 0;
DWORD   CHttpTransfer::s_dwId = 0;

static HttpCallback  s_http_callback = 0;


static   std::vector<PTDnsResolve>    s_vDnsQuery;                         // 待解析的DNS

static   std::vector<PTDnsResolve>    s_vDnsResolved;                      // 解开的DNS



bool operator == ( const CSockAddr & a, const CSockAddr & b )
{
    if ( a.m_wPort != b.m_wPort )
        return false;

    if ( a.m_dwServerIp == b.m_dwServerIp && a.m_dwServerIp != (DWORD)-1 )
        return true;

    if ( a.m_sDomain == b.m_sDomain )
        return true;

    return false;
}


void ClearVector( std::vector<CHttpOption*> & v )
{
    std::vector<CHttpOption*>::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( std::vector<CHttpTransaction*> & v )
{
    std::vector<CHttpTransaction*>::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( std::vector<CHttpTransfer*> & v )
{
    std::vector<CHttpTransfer*>::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( std::vector<CHttpFormData*> & v )
{
    std::vector<CHttpFormData*>::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}


DWORD  CHttpFormData::GetTotalLength( DWORD dwBoundaryLen )
{
    // -- + boundary + "\r\n"
    DWORD  dwTotalLen = 2 + dwBoundaryLen + 2;

    // Content-Disposition: form-data; name="file"; filename="a.jpg"
    // Content-Type: image/jpeg
    std::string  sTmp = "Content-Disposition: form-data; name=\"";
    sTmp += m_sName + "\"";

    if ( m_eType == FORMAT_DATA_TYPE_UPLOAD )
    {
        sTmp += "; filename=\"" + m_sFileName + "\"";
    }
    sTmp += "\r\n";
    

    if ( m_sContentType.length() > 0 )
    {
        sTmp += "Content-Type: " + m_sContentType + "\r\n";
    }
    dwTotalLen += sTmp.length();


    // 空白行
    dwTotalLen += 2;


    if ( m_eType == FORMAT_DATA_TYPE_NORMAL )
    {
        dwTotalLen += m_sValue.length() + 2;
    }
    else
    {
        dwTotalLen += LmnGetFileSize( m_sValue.c_str() ) + 2;
    }

    return dwTotalLen;
}

// 缩进8个空格
void CHttpTransaction::PrintDebugInfo( std::string & sInfo )
{
    const char * pIndent = "        ";
    // char buf[8192];

    sInfo += pIndent;
    sInfo += "url: " + m_request.GetUrl();
    sInfo += "\n";

    const char * aszStates[] = 
        {
            "TRANSACTION_STATE_NONE",
            "TRANSACTION_STATE_REQUESTING",
            "TRANSACTION_STATE_OK"
        };

    assert( m_eState <= TRANSACTION_STATE_OK );

    sInfo += pIndent;
    sInfo += "state: ";
    sInfo += aszStates[m_eState];
    sInfo += "\n";
}






void  CHttpTransfer::Reconnect()
{
    std::vector<CHttpTransaction*>::iterator it;
    assert( (DWORD)-1 != m_cPeerAddr.m_dwServerIp  );

    // 如果不使用sock5连接
    if ( !m_bUseSock5 )
    {
        // 重连
        m_fd = liOpen( LI_PROTOTYPE_TCP, 0, 0, OnLiEvent, this, FALSE );
        if ( m_fd == INVALID_SOCKET )
        {
            for ( it = m_vTransactions.begin(); it != m_vTransactions.end(); it++ )
            {
                CHttpTransaction * pTransaction = *it;
                if ( s_http_callback )
                {
                    s_http_callback( 0, pTransaction->m_response.GetHttpState(), 
                                     pTransaction->m_response.GetData(), pTransaction->m_response.GetDataSize(), 
                                     (char *)pTransaction->m_TotalHeader.GetData(),  pTransaction->m_TotalHeader.GetDataLength(),
                                     pTransaction->m_pContext );
                }
            }

            ClearVector( m_vTransactions );
            m_eState = TRANSFER_STATE_IDLE;
            return;
        }

        if ( 0 != liConnect( m_fd, m_cPeerAddr.m_dwServerIp, m_cPeerAddr.m_wPort, HTTP_CONNECTION_TIMEOUT ) )
        {
            MICROBLOG_LOG("failed to liConnect()! \n");
            OnEventClose( -1 );
        }
        else
        {
            m_eState = TRANSFER_STATE_CONNECTING;
        }
        return;
    }
    // 如果使用sock5连接
    else
    {
        assert( (DWORD)-1 != m_cDestAddr.m_dwServerIp );

        // sock5连接
        TCnnSock5Param  t;
        memset( &t, 0, sizeof(t) );

        t.bUseSock5    = TRUE;
        t.dwDestAddr   = m_cDestAddr.m_dwServerIp;
        t.wDestPort    = m_cDestAddr.m_wPort;
        t.dwSock5Addr  = m_cPeerAddr.m_dwServerIp;
        t.wSock5Port   = m_cPeerAddr.m_wPort;
        t.dwTimeOut    = HTTP_CONNECTION_TIMEOUT;
        t.eventHandler = OnLiEvent;
        t.context      = this;
        t.eMethod      = SOCK5_METHOD_USER;
        strncpy( t.szUserName, m_sSock5User.c_str(), MAX_SOCK5_USERNAME_LEN );
        strncpy( t.szPassword, m_sSock5Pwd.c_str(),  MAX_SOCK5_PASSWORD_LEN );

        m_fd = lxConnectSock5 ( &t );

        if ( m_fd == INVALID_SOCKET )
        {
            for ( it = m_vTransactions.begin(); it != m_vTransactions.end(); it++ )
            {
                CHttpTransaction * pTransaction = *it;
                if ( s_http_callback )
                {
                    s_http_callback( 0, pTransaction->m_response.GetHttpState(), 
                        pTransaction->m_response.GetData(), pTransaction->m_response.GetDataSize(), 
                        (char *)pTransaction->m_TotalHeader.GetData(),  pTransaction->m_TotalHeader.GetDataLength(),
                        pTransaction->m_pContext );
                }
            }

            ClearVector( m_vTransactions );
            m_eState = TRANSFER_STATE_IDLE;
            return;
        }

        m_eState = TRANSFER_STATE_CONNECTING;
        return;
    }
}


void  CHttpTransfer::AppendTransaction( const CHttpRequest & request, void * context )
{
    CHttpTransaction * pTransaction = new CHttpTransaction( context );
    pTransaction->m_request = request;
    m_vTransactions.push_back( pTransaction );
}

void  CHttpTransfer::Step()
{
    // 准备连接
    if ( TRANSFER_STATE_IDLE == m_eState )
    {
        // 可以直接连接
        if ( (DWORD)-1 != m_cPeerAddr.m_dwServerIp )
        {
            // 如果不使用sock5代理
            if ( !m_bUseSock5 )
            {
                Reconnect();
            }
            else
            {
                if ( (DWORD)-1 != m_cDestAddr.m_dwServerIp )
                {
                    Reconnect();
                }
                // 解析DEST DNS
                else
                {
                    PTDnsResolve  pDnsQuery = new TDnsResolve;
                    strncpy( pDnsQuery->szDomain, m_cDestAddr.m_sDomain.c_str(), MAX_DNS_DOMAIN_NAME_LEN - 1 );

                    LmnLock( &g_Lock );
                    s_vDnsQuery.push_back( pDnsQuery );
                    LmnUnlock( &g_Lock );
                    m_eState = TRANSFER_STATE_GETTING_DEST_NAME;
                }
            }
        }
        // 需要DNS解析(暂时放在此处线程解析)
        // DNS解析可能会有几秒钟的阻塞
        else
        {
            PTDnsResolve  pDnsQuery = new TDnsResolve;
            strncpy( pDnsQuery->szDomain, m_cPeerAddr.m_sDomain.c_str(), MAX_DNS_DOMAIN_NAME_LEN - 1 );

            LmnLock( &g_Lock );
            s_vDnsQuery.push_back( pDnsQuery );
            LmnUnlock( &g_Lock );

            m_eState = TRANSFER_STATE_GETTING_HOST_NAME;
        }
    }
    else if ( TRANSFER_STATE_HOST_NAME_RESOLVED == m_eState )
    {
        // 解析失败
        if ( (DWORD)-1 == m_cPeerAddr.m_dwServerIp )
        {
            OnEventClose();
        }
        else
        {
            // 如果不使用sock5代理
            if ( !m_bUseSock5 )
            {
                Reconnect();
            }
            // 使用sock5代理
            else
            {
                // 不需要解析dest域名
                if ( (DWORD)-1 != m_cDestAddr.m_dwServerIp )
                {
                    Reconnect();
                }
                else
                {
                    PTDnsResolve  pDnsQuery = new TDnsResolve;
                    strncpy( pDnsQuery->szDomain, m_cDestAddr.m_sDomain.c_str(), MAX_DNS_DOMAIN_NAME_LEN - 1 );

                    LmnLock( &g_Lock );
                    s_vDnsQuery.push_back( pDnsQuery );
                    LmnUnlock( &g_Lock );
                    m_eState = TRANSFER_STATE_GETTING_DEST_NAME;
                }
            }
        }
    }
    else if ( TRANSFER_STATE_DEST_NAME_RESOLVED == m_eState )
    {
        assert( m_bUseSock5 );

        // 解析失败
        if ( (DWORD)-1 == m_cDestAddr.m_dwServerIp )
        {
            OnEventClose();
        }
        else
        {
            Reconnect();
        }
    }
    else if ( TRANSFER_STATE_OK == m_eState )
    {
        // 没有transaction
        if ( 0 == m_vTransactions.size() )
        {
            return;
        }

        std::vector<CHttpTransaction*>::iterator it = m_vTransactions.begin();
        CHttpTransaction * pTransaction = *it;

        // 正在处理某一个transaction
        if ( CHttpTransaction::TRANSACTION_STATE_REQUESTING == pTransaction->m_eState )
        {
            return;
        }

        assert(  CHttpTransaction::TRANSACTION_STATE_NONE  == pTransaction->m_eState  );

        // 如果还没有发出请求，则请求之(包括重定向)
        BeginTransaction( pTransaction );
    }
}

void  CHttpTransfer::OnEventClose( int nErr /*= 0*/ )
{
    if ( m_bUseSsl )
    {
        assert ( m_pSslAdap );

        if ( lmn_base::CSslAdapter::SSL_NONE != m_pSslAdap->GetState() )
        {
            m_pSslAdap->StopSsl();
        }
        else
        {
            liClose(m_fd);
        }
    }
    else
    {
        liClose( m_fd );
    }
    m_fd = INVALID_SOCKET;


    if (  0 != nErr ||  TRANSFER_STATE_CONNECTING == m_eState || TRANSFER_STATE_HOST_NAME_RESOLVED == m_eState
          ||  TRANSFER_STATE_DEST_NAME_RESOLVED == m_eState   )
    {
        std::vector<CHttpTransaction*>::iterator it;
        for ( it = m_vTransactions.begin(); it != m_vTransactions.end(); it++ )
        {
            CHttpTransaction * pTransaction = *it;
            assert( s_http_callback );

            if ( s_http_callback )
            {
                s_http_callback( nErr, pTransaction->m_response.GetHttpState(), 
                    pTransaction->m_response.GetData(), pTransaction->m_response.GetDataSize(), 
                    (char *)pTransaction->m_TotalHeader.GetData(),  pTransaction->m_TotalHeader.GetDataLength(),
                    pTransaction->m_pContext );
            }
        }

        ClearVector( m_vTransactions );
        m_eState = TRANSFER_STATE_IDLE;    
        return;
    }
    else 
    {
        assert( TRANSFER_STATE_OK == m_eState || TRANSFER_STATE_WAITING_SERVER_CLOSE == m_eState );
        
        if ( 0 == m_vTransactions.size() )
        {
            m_eState = TRANSFER_STATE_IDLE;
            return;
        }

        std::vector<CHttpTransaction*>::iterator it = m_vTransactions.begin();
        CHttpTransaction * pTransaction = *it;

        // 可能中途断链，上报残缺的response
        if ( pTransaction->m_eState == CHttpTransaction::TRANSACTION_STATE_REQUESTING )
        {
            if ( s_http_callback )
            {
                s_http_callback( 0, pTransaction->m_response.GetHttpState(), 
                    pTransaction->m_response.GetData(), pTransaction->m_response.GetDataSize(), 
                    (char *)pTransaction->m_TotalHeader.GetData(),  pTransaction->m_TotalHeader.GetDataLength(),
                    pTransaction->m_pContext );
            }

            delete pTransaction;
            pTransaction = 0;
            it = m_vTransactions.erase( it );
        }

        if ( m_vTransactions.end() != it )
        {
            pTransaction = *it;
            assert( pTransaction->m_eState == CHttpTransaction::TRANSACTION_STATE_NONE );

            Reconnect();
            return;
        }
        else
        {
            m_eState = TRANSFER_STATE_IDLE;
            return;
        }
    }
}

void  CHttpTransfer::OnEventRead( )
{
    std::vector<CHttpTransaction*>::iterator it = m_vTransactions.begin();
    // assert( it != m_vTransactions.end() );
    if ( it == m_vTransactions.end() )
    {
        OnEventClose(-1);
        return;
    }

    CHttpTransaction * pTransaction = *it;
    assert( CHttpTransaction::TRANSACTION_STATE_REQUESTING == pTransaction->m_eState );
    CHttpResponse *    pResponse = &pTransaction->m_response;


    char buf[HTTP_RECEIVE_BUFFER_SIZE];
    int len;

    memset( buf, 0, sizeof(buf) );
    len = HTTP_RECEIVE_BUFFER_SIZE;
    int ret = 0;
    CDataBuf  tmpData;

    if ( m_bUseSsl )
    {
        assert( m_pSslAdap );
        ret = m_pSslAdap->Recv( buf, len );
        if ( ret >= 0 )
        {
            len = ret;
            ret = 0;
        }
    }
    else
    {
        ret = liTcpRecv( m_fd, buf, &len );
    }
    

    // 读取了数据
    while ( 0 == ret && len > 0 )
    {
        tmpData.Append( buf, len );

        memset( buf, 0, sizeof(buf) );
        len = HTTP_RECEIVE_BUFFER_SIZE;

        if ( m_bUseSsl )
        {
            ret = m_pSslAdap->Recv( buf, len );
            if ( ret >= 0 )
            {
                len = ret;
                ret = 0;
            }
        }
        else
        {
            ret = liTcpRecv( m_fd, buf, &len );
        }
    }

    char * pBuf  = (char*)tmpData.GetData();
    len          = (int)tmpData.GetDataLength();
    int nComsued = 0;
    BOOL bConnectionClose = FALSE;

    // 解析字节流
    while ( len > 0 )
    {
        BOOL bRet = ParseReceived( pBuf, len, pResponse, nComsued, pTransaction->m_TotalHeader );
        pBuf += nComsued;
        len  -= nComsued;

        // 解析失败
        if ( !bRet )
        {
            pResponse->m_cData.Clear();
            pResponse->m_cData.Append( tmpData.GetData(), tmpData.GetDataLength() );
            OnEventClose( 1 );        // 解析失败
            return;
        }

        // 完成一个transaction
        if ( CHttpResponse::PROGRESS_OK == pResponse->GetProgressState() )
        {
            // 还有剩余数据
            if ( len > 0 )
            {
                pResponse->m_cData.Clear();
                pResponse->m_cData.Append( tmpData.GetData(), tmpData.GetDataLength() );
                OnEventClose( 2 );        // 数据长度不对
                return;
            }

            bConnectionClose = pResponse->ServerConnectionClose();

            // 如果不需要重定向
            if ( !pResponse->HasLocation() )
            {
                pTransaction->m_eState = CHttpTransaction::TRANSACTION_STATE_OK;

                
                // 通知上层
                s_http_callback( 0, pResponse->GetHttpState(), pResponse->GetData(), pResponse->GetDataSize(), 
                    (char *)pTransaction->m_TotalHeader.GetData(),  pTransaction->m_TotalHeader.GetDataLength(),
                    pTransaction->m_pContext );
                // END 通知上层

                // 回收内存
                delete  pTransaction;
                pTransaction = 0;
                pResponse    = 0;
                it = m_vTransactions.erase( it );

                if ( !bConnectionClose )
                {
                    Step();
                }
                else
                {
                    m_eState = TRANSFER_STATE_WAITING_SERVER_CLOSE;
                }
            }
            else
            {
                // 重置状态
                pTransaction->m_eState = CHttpTransaction::TRANSACTION_STATE_NONE;

                // 如果不使用proxy
                if ( CHttpRequest::PROXY_TYPE_NONE == pTransaction->m_request.GetProxyType() )
                {
                    CHttp * pHttp = CHttp::GetInstance();

                    std::string strHost;
                    WORD wPort;
                    std::string strPath;
                    BOOL bHasSlash = FALSE;
                    BOOL bHttps    = FALSE;
                    BOOL bUseSsl   = FALSE;
                    CHttp::ParseUrl( pResponse->GetLocation(), strHost, wPort, strPath, &bHasSlash, &bUseSsl );

                    CHttpTransfer * pTransfer = pHttp->GetTransfer(strHost, wPort, bUseSsl);
                    // 需改动transaction的位置
                    if ( pTransfer != this )
                    {
                        it = m_vTransactions.erase( it );
                        pTransfer->m_vTransactions.push_back( pTransaction );
                    }

                    if ( pTransfer == this )
                    {
                        if ( !bConnectionClose )
                        {
                            pTransfer->Step();
                        }
                        else
                        {
                            m_eState = TRANSFER_STATE_WAITING_SERVER_CLOSE;
                        }
                    }
                    else
                    {
                        pTransfer->Step();
                    }
                }
                else
                {
                    if ( !bConnectionClose )
                    {
                        Step();
                    }
                    else
                    {
                        m_eState = TRANSFER_STATE_WAITING_SERVER_CLOSE;
                    }
                }
            }  
        }
    }

    if ( 0 != ret )
    {
        OnEventClose( -1 );
        return;
    }
}

BOOL  CHttpTransfer::ParseLine( CHttpResponse * pResponse )
{
	DWORD  dwLen     = pResponse->m_cTempBuf.GetDataLength();
	char   buf[4096] = { 0 };
	memcpy( buf, pResponse->m_cTempBuf.GetData(), MIN(dwLen,4096-1) );
	// 转小写
	MyStrTrim( buf );

	switch ( pResponse->GetProgressState() )
	{
	case CHttpResponse::PROGRESS_VERSION:
		{
			char * szVersion = buf;
			char * szCode = 0;
			char * szDescription = 0;

			DWORD  i = 0;
			while( i < dwLen )
			{
				if ( buf[i] == ' ' )
				{
					buf[i] = '\0';
					if ( 0 == szCode )
					{
						szCode = buf + (i + 1);
					}
					else if ( 0 == szDescription )
					{
						szDescription = buf + (i + 1);
						break;
					}
				}
				i++;
			}

			// 简单检验格式
            if ( 0 == szCode || 0 == szDescription )
            {
                return FALSE;
            }

            if ( !(0 == STRICMP("HTTP/1.1", szVersion) || 0 == STRICMP("HTTP/1.0", szVersion)) )
            {
                return FALSE;
            }
			// END 简单检验格式

			DWORD  dwTemp = 0;
			int ret = sscanf( szCode, " %lu", &dwTemp );
            if ( 0 == ret )
            {
                return FALSE;
            }

			pResponse->SetHttpCode( dwTemp );

			return TRUE;
		}
		break;

	case CHttpResponse::PROGRESS_OPTION:
		{
			char * szKey   = buf;
			char * szValue = 0;

			DWORD  i = 0;
			while( i < dwLen )
			{
				if ( buf[i] == ':' )
				{
					buf[i] = '\0';
					if ( 0 == szValue )
					{
						szValue = buf + (i + 1);
						break;
					}
				}
				i++;
			}

            if ( 0 == szValue )
            {
                return FALSE;
            }

			MyStrTrim(szKey);
            Str2Lower( szKey );
            pResponse->AppendOption( szKey );

			MyStrTrim(szValue);
            pResponse->SetLastOptionValue( szValue );

            if (    0 == STRICMP( pResponse->GetLastOption()->m_sValue.c_str(), "chunked" ) 
                && 0 == STRICMP(  pResponse->GetLastOption()->m_sKey.c_str(), "Transfer-Encoding" )  )
            {
                pResponse->SetChunked( TRUE );
            }
            else if ( 0 == STRICMP( pResponse->GetLastOption()->m_sKey.c_str(), "Content-Length" )  )
            {
                DWORD dwTemp = 0;
                sscanf( szValue, " %lu", &dwTemp );
                pResponse->SetContentLen( dwTemp );
            }
            return TRUE;
		}
		break;

    case CHttpResponse::PROGRESS_CHUNK_SIZE:
        {
			MyStrTrim(buf);
            pResponse->SetChunkSize( CHttpResponse::HexStr2ULong( buf ) );
            return TRUE;
        }
        break;

	default:
		break;
	}

	return FALSE;
}


BOOL  CHttpTransfer::ParseReceived( const char * buf, int nBufSize, CHttpResponse * pResponse, int & nConsumedSize, CDataBuf & cTotalHeader )
{
    // 该response必须是未完成状态
    assert( CHttpResponse::PROGRESS_OK != pResponse->GetProgressState() );

    nConsumedSize = 0;

    // 简化情况下解析
    // 即假设http服务器是正常的

    switch ( pResponse->GetProgressState() )
    {
    case CHttpResponse::PROGRESS_VERSION:
        {
            const char * pCarriageReturn = (const char *)memchr( buf, '\xd', nBufSize );
            if ( 0 == pCarriageReturn )
            {
                nConsumedSize = nBufSize;
				pResponse->m_cTempBuf.Append( (char*)buf, nBufSize );

                cTotalHeader.Append( (char*)buf, nConsumedSize );
            }
            else
            {
                nConsumedSize = pCarriageReturn - buf;
				pResponse->m_cTempBuf.Append( buf, nConsumedSize );

				// 解析行
				BOOL bRet = ParseLine( pResponse );

                pResponse->m_cTempBuf.Clear();
				nConsumedSize++;                // '\xd'消耗了

                cTotalHeader.Append( (char*)buf, nConsumedSize );
				
                // 解析失败
                if ( !bRet )
                {
                    return FALSE;
                }

                pResponse->SetProgressState( CHttpResponse::PROGRESS_VERSION_CARRIAGE_RETURN );
				return TRUE;
            }
        }
        break;

    case CHttpResponse::PROGRESS_VERSION_CARRIAGE_RETURN:
        {
            if ( *buf == '\xa' )
            {
                nConsumedSize = 1;

                cTotalHeader.Append( (char*)buf, nConsumedSize );

                pResponse->SetProgressState( CHttpResponse::PROGRESS_OPTION );
                return TRUE;
            }
            else
            {
                nConsumedSize = 1;
                // 解析失败
                return FALSE;
            }
        }
        break;

    case CHttpResponse::PROGRESS_OPTION:
        {
            const char * pCarriageReturn = (const char *)memchr( buf, '\xd', nBufSize );
            if ( 0 == pCarriageReturn )
            {
                nConsumedSize = nBufSize;
                pResponse->m_cTempBuf.Append( buf, nBufSize );

                cTotalHeader.Append( (char*)buf, nConsumedSize );
            }
            else
            {
                nConsumedSize = pCarriageReturn - buf;

                // 可能是空行
                if ( 0 == pResponse->m_cTempBuf.GetDataLength() && 0 == nConsumedSize )
                {
                    nConsumedSize++;
                    pResponse->m_cTempBuf.Clear();

                    cTotalHeader.Append( (char*)buf, nConsumedSize );

                    pResponse->SetProgressState( CHttpResponse::PROGRESS_EMPTY_LINE_CARRIAGE_RETURN );
                    return TRUE;
                }
                else
                {
                    pResponse->m_cTempBuf.Append( buf, nConsumedSize );

                    // 解析行
                    BOOL bRet = ParseLine( pResponse );

                    pResponse->m_cTempBuf.Clear();
                    nConsumedSize++;                // '\xd'消耗了

                    cTotalHeader.Append( (char*)buf, nConsumedSize );

                    // 解析失败
                    if ( !bRet )
                    {
                        return FALSE;
                    }

                    pResponse->SetProgressState( CHttpResponse::PROGRESS_OPTION_CARRIAGE_RETURN );
                    return TRUE;
                }
            }
        }
        break;

    case CHttpResponse::PROGRESS_OPTION_CARRIAGE_RETURN:
        {
            if ( *buf == '\xa' )
            {
                nConsumedSize = 1;

                cTotalHeader.Append( (char*)buf, nConsumedSize );

                pResponse->SetProgressState( CHttpResponse::PROGRESS_OPTION );
                return TRUE;
            }
            else
            {
                nConsumedSize = 1;
                // 解析失败
                return FALSE;
            }
        }
        break;

    case CHttpResponse::PROGRESS_EMPTY_LINE_CARRIAGE_RETURN:
        {
            if ( *buf == '\xa' )
            {
                nConsumedSize = 1;
                cTotalHeader.Append( (char*)buf, nConsumedSize );

                if ( pResponse->IfChucked() )
                {
                    pResponse->SetProgressState( CHttpResponse::PROGRESS_CHUNK_SIZE );
                }
                else
                {
                    // 如果没有数据，认为OK了
                    if ( pResponse->HasContentLen() && 0 == pResponse->GetContentLen() )
                    {
                        pResponse->SetProgressState( CHttpResponse::PROGRESS_OK );
                    }
                    else
                    {
                        pResponse->SetProgressState( CHttpResponse::PROGRESS_DATA );
                    }
                }
                return TRUE;
            }
            else
            {
                nConsumedSize = 1;
                // 解析失败
                return FALSE;
            }
        }
        break;

    case CHttpResponse::PROGRESS_CHUNK_SIZE:
        {
            const char * pCarriageReturn = (const char *)memchr( buf, '\xd', nBufSize );
            if ( 0 == pCarriageReturn )
            {
                nConsumedSize = nBufSize;
                pResponse->m_cTempBuf.Append( buf, nBufSize );
            }
            else
            {
                nConsumedSize = pCarriageReturn - buf;
                pResponse->m_cTempBuf.Append( buf, nConsumedSize );

                // 解析行
                BOOL bRet = ParseLine( pResponse );

                pResponse->m_cTempBuf.Clear();
                nConsumedSize++;                // '\xd'消耗了

                // 解析失败
                if ( !bRet )
                {
                    return FALSE;
                }

                pResponse->SetProgressState( CHttpResponse::PROGRESS_CHUNK_SIZE_CARRIAGE_RETURN );
                return TRUE;
            }
        }
        break;

    case CHttpResponse::PROGRESS_CHUNK_SIZE_CARRIAGE_RETURN:
        {
            if ( *buf == '\xa' )
            {
                nConsumedSize = 1;
                pResponse->SetProgressState( CHttpResponse::PROGRESS_DATA );
                return TRUE;
            }
            else
            {
                nConsumedSize = 1;
                // 解析失败
                return FALSE;
            }
        }
        break;

    case CHttpResponse::PROGRESS_DATA:
        {
            if ( pResponse->IfChucked() )
            {
                // 数据还不够
                if ( pResponse->m_cTempBuf.GetDataLength() + (DWORD)nBufSize < pResponse->GetChunkSize() )
                {
                    // 缓存在temp buf里
                    pResponse->m_cTempBuf.Append( buf, nBufSize );
                    nConsumedSize = nBufSize;
                }
                // 数据够了
                else
                {
                    DWORD dwTempBufSize = pResponse->m_cTempBuf.GetDataLength();
                    pResponse->m_cTempBuf.Append( buf, pResponse->GetChunkSize() - dwTempBufSize );

                    nConsumedSize = pResponse->GetChunkSize() - dwTempBufSize;

                    pResponse->AppendData( (char *)pResponse->m_cTempBuf.GetData(), pResponse->m_cTempBuf.GetDataLength() );
                    pResponse->m_cTempBuf.Clear();

                    pResponse->SetProgressState( CHttpResponse::PROGRESS_CHUNK_DATA_EMPTY_LINE );
                }
            }
            else if ( pResponse->HasContentLen() )
            {
                // 数据还不够
                if ( pResponse->GetDataSize() + (DWORD)nBufSize < pResponse->GetContentLen() )
                {
                    pResponse->AppendData( buf, nBufSize );
                    nConsumedSize = nBufSize;
                }
                // 数据够了
                else
                {
                    DWORD dwTempBufSize = pResponse->GetDataSize();
                    pResponse->AppendData( buf, pResponse->GetContentLen() - dwTempBufSize );
                    nConsumedSize += pResponse->GetContentLen() - dwTempBufSize;
                    pResponse->SetProgressState( CHttpResponse::PROGRESS_OK );
                }
            }
            else
            {
                pResponse->AppendData( buf, nBufSize );
                nConsumedSize += nBufSize;
            }
            return TRUE;
        }
        break;

    case CHttpResponse::PROGRESS_CHUNK_DATA_EMPTY_LINE:
        {
            if ( 0 == pResponse->m_cTempBuf.GetDataLength() )
            {
                nConsumedSize = 1;
                if ( '\xd' == buf[0] )
                {
                    pResponse->m_cTempBuf.Append(buf, nConsumedSize);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
            else if ( 1 == pResponse->m_cTempBuf.GetDataLength() )
            {
                nConsumedSize = 1;
                if ( '\xa' == buf[0] )
                {
                    // 如果上一轮chunk size为0
                    if ( 0 == pResponse->GetChunkSize() )
                    {
                        pResponse->SetProgressState( CHttpResponse::PROGRESS_OK );
                    }
                    else
                    {
                        pResponse->SetProgressState( CHttpResponse::PROGRESS_CHUNK_SIZE );
                    }
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
            else
            {
                assert ( 0 );
                return FALSE;
            }
        }
        break;

    default:
        break;
    }

    return FALSE;
}


static int socket_recv( void * socket, void * data, size_t length, int * pbBlocked )
{
    if ( pbBlocked )
    {
        *pbBlocked = 0;
    }

    if ( 0 == liTcpRecv( (int)socket, (char *)data, (int *)&length ) )
    {
        if ( 0 == length && pbBlocked )
        {
            *pbBlocked = 1;
        }
        return length;
    }
    else
    {
        return -1;
    }
}

static int socket_send( void * socket, const void * data, size_t length, int * pbBlocked )
{
    if ( pbBlocked )
    {
        *pbBlocked = 0;
    }

    if ( 0 == liTcpSend( (int)socket, (const char *)data, (int *)&length ) )
    {
        if ( 0 == length && pbBlocked )
        {
            *pbBlocked = 1;
        }
        return length;
    }
    else
    {
        return -1;
    }
}

static int close_socket( void * socket )
{
    return liClose( (int)socket );
}


void  CHttpTransfer::OnEventConnected( )
{
    m_eState = TRANSFER_STATE_OK;
    Step();
}


void  CHttpTransfer::BeginTransaction( CHttpTransaction * pTransaction )
{
    CHttpRequest & request = pTransaction->m_request;


    std::string   strUrl;
    std::string   strHost, strPath;
    WORD          wPort; 

    if ( !pTransaction->m_response.HasLocation() )
    {
        strUrl = request.GetUrl();
    }
    else
    {
        strUrl = pTransaction->m_response.GetLocation();
    }

    CHttp::ParseUrl( strUrl, strHost, wPort, strPath );


    // 评出发送格式
    std::string strSendData;

    // 如果有location的，都用get方法
    if ( pTransaction->m_response.HasLocation() )
    {
        strSendData += "GET";
    }
    else
    {
        if ( CHttpRequest::HTTP_METHOD_GET == request.GetMethod() )
        {
            strSendData += "GET";
        }
        else if ( CHttpRequest::HTTP_METHOD_POST == request.GetMethod() )
        {
            strSendData += "POST";
        }
        else
        {
            assert( 0 );
        }
    }
    

    strSendData += " ";
    
    if ( CHttpRequest::PROXY_TYPE_NONE == request.GetProxyType() || CHttpRequest::PROXY_TYPE_SOCK5 == request.GetProxyType() )
    {
        strSendData += strPath;
    }
    else
    {
        strSendData += strUrl;
    }
    strSendData += " HTTP/1.1\r\n";

    request["Host"] = strHost;

    // 没有http proxy
    if ( CHttpRequest::PROXY_TYPE_NONE == request.GetProxyType() )
    {
        if ( 0 == request["Connection"].length() )
        {
            request["Connection"] = "keep-alive";
        }
    }
    // 有http proxy
    else
    {
        if ( 0 == request["Proxy-Connection"].length() )
        {
            request["Proxy-Connection"] = "keep-alive";
        }
    }

    std::string sBoundary;

    // 如果有location的，都用get方法，不设置data
    if ( pTransaction->m_response.HasLocation() )
    {
        request["Content-Length"] = "0";
    }
    else
    {
        // 如果非表单形式
        if ( CHttpRequest::HTTP_CONTENT_TYPE_NORMAL == request.GetContentType() )
        {
            char szContentLength[256] = { 0 };
            sprintf( szContentLength, "%u", request.GetData().length() );
            request["Content-Length"] = szContentLength;
        }
        // 表单形式
        else
        {
            sBoundary = CHttpTransfer::GenerateBoundary();

            DWORD dwContentLen = request.GetTotalFormDataLen( sBoundary.length() );

            // 计算所有表单的长度
            char szContentLength[256] = { 0 };
            sprintf( szContentLength, "%u", dwContentLen );
            request["Content-Length"] = szContentLength;
            //

            // 设置表单类型
            request["Content-Type"] = "multipart/form-data; boundary=" + sBoundary;
            // 
        }
    }


    DWORD i;
    for ( i = 0; i < request.GetOptionsCnt(); i++ )
    {
        CHttpOption * option = request.GetOption(i);
        strSendData += option->m_sKey;
        strSendData += ": ";
        strSendData += option->m_sValue;
        strSendData += "\r\n";
    }

    // 如果有location的，设置refer
    if ( pTransaction->m_response.HasLocation() )
    {
        assert( pTransaction->m_sLastUrl.length() > 0 );
        strSendData += "Referer: " + pTransaction->m_sLastUrl + "\r\n";
    }

    //Proxy-Authorization: Basic c2hpamllOnNoaWppZQ==
    // 有http proxy
    if ( CHttpRequest::PROXY_TYPE_HTTP == request.GetProxyType() )
    {
        strSendData += "Proxy-Authorization: Basic ";

        char buf[1024] = { 0 };
        DWORD dwLen = 1024;

        std::string tmp = request.GetProxyUser() + ":" + request.GetProxyPwd();
        EncodeBase64( buf, &dwLen, (void *)tmp.c_str(), tmp.length() );

        strSendData += buf;
        strSendData += "\r\n";
    }

    strSendData += "\r\n";






    // 如果没有location的，才设置Data
    if ( !pTransaction->m_response.HasLocation() && CHttpRequest::HTTP_CONTENT_TYPE_NORMAL == request.GetContentType() )
    {
        strSendData += request.GetData();
    }

    // 发送数据不要太大
    int len = strSendData.length();
    if ( m_bUseSsl )
    {
        if ( (len = m_pSslAdap->Send( strSendData.c_str(), len )) < 0 )
        {
            OnEventClose();
            return;
        }
    }
    else
    {
        if ( 0 != liTcpSend( m_fd, strSendData.c_str(), &len ) )
        {
            OnEventClose();
            return;
        }
    }
    

    // 如果有表单，继续发送
    if ( !pTransaction->m_response.HasLocation() && CHttpRequest::HTTP_CONTENT_TYPE_FORM == request.GetContentType() )
    {
        std::vector<CHttpFormData*>::iterator it;
        for ( it = request.m_vFormatData.begin(); it != request.m_vFormatData.end(); it++ )
        {
            CHttpFormData * pItem = *it;

            std::string sFormData = "--" + sBoundary + "\r\n";

            // Content-Disposition: form-data; name="file"; filename="a.jpg"
            // Content-Type: image/jpeg
            sFormData += "Content-Disposition: form-data; name=\"";
            sFormData += pItem->GetName() + "\"";

            if ( pItem->GetType() == CHttpFormData::FORMAT_DATA_TYPE_UPLOAD )
            {
                sFormData += "; filename=\"" + pItem->GetFileName() + "\"";
            }
            sFormData += "\r\n";

            if ( pItem->GetContentType().length() > 0 )
            {
                sFormData += "Content-Type: " +  pItem->GetContentType() + "\r\n";
            }

            // 空白行
            sFormData += "\r\n";

            
            // 发送
            len = sFormData.length();
            if ( m_bUseSsl )
            {
                if ( (len = m_pSslAdap->Send( sFormData.c_str(), len )) < 0 )
                {
                    OnEventClose();
                    return;
                }
            }
            else
            {
                if ( 0 != liTcpSend( m_fd, sFormData.c_str(), &len ) )
                {
                    OnEventClose();
                    return;
                }
            }


            if ( pItem->GetType() == CHttpFormData::FORMAT_DATA_TYPE_NORMAL )
            {
                sFormData = pItem->GetValue() + "\r\n";

                len = sFormData.length();

                if ( m_bUseSsl )
                {
                    if ( (len = m_pSslAdap->Send( sFormData.c_str(), len )) < 0 )
                    {
                        OnEventClose();
                        return;
                    }
                }
                else
                {
                    if ( 0 != liTcpSend( m_fd, sFormData.c_str(), &len ) )
                    {
                        OnEventClose();
                        return;
                    }
                }
            }
            else
            {
                // 传输文件
                FILE * fp = LmnOpenFile( pItem->GetValue().c_str(), "rb" );

                // 文件打开失败
                if ( 0 == fp )
                {
                    OnEventClose();
                    return;
                }

                char abyFileData[4096];
                size_t dwReadLen = fread( abyFileData, 1, 4096, fp );
                while( dwReadLen > 0 )
                {
                    len = dwReadLen;

                    if ( m_bUseSsl )
                    {
                        if ( (len = m_pSslAdap->Send( abyFileData, len )) < 0 )
                        {
                            OnEventClose();
                            return;
                        }
                    }
                    else
                    {
                        if ( 0 != liTcpSend( m_fd, abyFileData, &len ) )
                        {
                            OnEventClose();
                            return;
                        }
                    }
                    dwReadLen = fread( abyFileData, 1, 4096, fp );
                }

                fclose( fp );
                //

                sFormData = "\r\n";
                len = sFormData.length();

                if ( m_bUseSsl )
                {
                    if ( (len = m_pSslAdap->Send( sFormData.c_str(), len )) < 0 )
                    {
                        OnEventClose();
                        return;
                    }
                }
                else
                {
                    if ( 0 != liTcpSend( m_fd, sFormData.c_str(), &len ) )
                    {
                        OnEventClose();
                        return;
                    }
                }
            }
        }

        std::string sTail = "--" + sBoundary + "--\r\n";

        len = sTail.length();

        if ( m_bUseSsl )
        {
            if ( (len = m_pSslAdap->Send( sTail.c_str(), len )) < 0  )
            {
                OnEventClose();
                return;
            }
        }
        else
        {
            if ( 0 != liTcpSend( m_fd, sTail.c_str(), &len ) )
            {
                OnEventClose();
                return;
            }
        }
    }
    // 


    // 用于refer
    pTransaction->m_sLastUrl = strUrl;

    pTransaction->m_response.Clear();

    pTransaction->m_eState = CHttpTransaction::TRANSACTION_STATE_REQUESTING;
}


void CHttpTransfer::OnLiEvent( FdType fd, int liEvent, int error, void * context )
{
    CHttpTransfer * pTransfer = (CHttpTransfer *)context;
    CFuncLock cLock( pTransfer->m_pLock );

    assert( fd == pTransfer->m_fd );

    switch( liEvent )
    {
    case liEvClose:
        pTransfer->OnEventClose( -1 );
        break;

    case liEvRead:
        pTransfer->m_dwLastActiveTick = LmnGetTickCount();

        if ( pTransfer->IfUseSsl() )
        {
            assert ( pTransfer->m_pSslAdap );

            lmn_base::CSslAdapter::SslState eState = pTransfer->m_pSslAdap->GetState();
            if ( eState == lmn_base::CSslAdapter::SSL_CONNECTING )
            {
                eState = pTransfer->m_pSslAdap->OnReadEvent();
                if ( eState == lmn_base::CSslAdapter::SSL_CONNECTED )
                {
                    pTransfer->OnEventConnected();
                }
            }
            else if ( eState == lmn_base::CSslAdapter::SSL_CONNECTED )
            {
                pTransfer->OnEventRead();
            }
            else if ( eState == lmn_base::CSslAdapter::SSL_ERROR )
            {
                pTransfer->OnEventClose( -1 );
            }
        }
        else
        {
            pTransfer->OnEventRead();
        }
        break;

    case liEvConnect:
        pTransfer->m_dwLastActiveTick = LmnGetTickCount();

        if ( pTransfer->m_bUseSsl )
        {
            assert( pTransfer->m_pSslAdap );

            lmn_base::CSslAdapter::SockCb param;
            param.pfnClose = close_socket;
            param.pfnSend  = socket_send;
            param.pfnRecv  = socket_recv;

            std::string sPeerAddr = pTransfer->m_cPeerAddr.m_sDomain;
            if ( 0 == sPeerAddr.length() )
            {
                char buf[128];
                GetIpAddr( buf, 128, pTransfer->m_cPeerAddr.m_dwServerIp );
                sPeerAddr = buf;
            }

            pTransfer->m_pSslAdap->StartSsl( (void *)pTransfer->m_fd, sPeerAddr.c_str(), true, &param );
            pTransfer->m_eState = TRANSFER_STATE_SSL_CONNECTING;
        }
        else
        {
            pTransfer->OnEventConnected();
        }
        break;

    default:
        break;
    }
}

std::string CHttpTransfer::GenerateBoundary()
{
    // 先简处理
    return "----------------------------258d97ac2b79";
}

// 每行需要缩进4个空格
void CHttpTransfer::PrintDebugInfo( std::string & sInfo )
{
    char buf[8192];
    const char * pIndent = "    ";

    sInfo += pIndent;
    sInfo += "peer address: ";

    if ( 0 == m_cPeerAddr.m_sDomain.length() )
    {
        char szIpDotAddr[128] = { 0 };
        unsigned char * pIp = (unsigned char *)&m_cPeerAddr.m_dwServerIp;

        sprintf( szIpDotAddr, "%u.%u.%u.%u", (unsigned int)pIp[0], (unsigned int)pIp[1], 
                                             (unsigned int)pIp[2], (unsigned int)pIp[3] );

        sInfo += szIpDotAddr;
    }
    else
    {
        sInfo += m_cPeerAddr.m_sDomain;
    }

    sInfo += ":";
    SNPRINTF( buf, 8192, "%u", (unsigned int)m_cPeerAddr.m_wPort );
    sInfo += buf;
    sInfo += "\n";


    const char * aszStates[] = {
        "TRANSFER_STATE_IDLE",
        "TRANSFER_STATE_GETTING_HOST_NAME",
        "TRANSFER_STATE_HOST_NAME_RESOLVED",
        "TRANSFER_STATE_GETTING_DEST_NAME",
        "TRANSFER_STATE_DEST_NAME_RESOLVED",
        "TRANSFER_STATE_CONNECTING",
        "TRANSFER_STATE_OK",
        "TRANSFER_STATE_WAITING_SERVER_CLOSE"
    };

    assert( m_eState <= TRANSFER_STATE_WAITING_SERVER_CLOSE );

    sInfo += pIndent;
    sInfo += "state: ";
    sInfo += aszStates[m_eState];
    sInfo += "\n";


    std::vector<CHttpTransaction*>::iterator it;
    int i;
    for ( it = m_vTransactions.begin(), i = 0; it != m_vTransactions.end(); it++, i++ )
    {
        sInfo += pIndent;
        SNPRINTF( buf, 8192, "%d", i+1 );
        sInfo += buf;
        sInfo += "th transaction: \n";

        CHttpTransaction * pTrans = *it;
        pTrans->PrintDebugInfo( sInfo );
    }
}











// 从url获取host, port和path
bool  CHttp::ParseUrl( const std::string & strUrl, std::string & strHost, WORD & wPort, std::string & strPath, BOOL * pHasSlash /*= 0*/, BOOL * pbHttps /*=0*/ )
{
    std::string strUrlCopy;
    BOOL  bHttps = FALSE;

    // url有http protocol
    if ( strUrl.substr(0,7) == "http://" )
    {
        strUrlCopy = strUrl.substr(7);
    }
    else if ( strUrl.substr(0,8) == "https://" )
    {
        strUrlCopy = strUrl.substr(8);
        bHttps = TRUE;
    }
    // url没有http protocol
    else
    {
        strUrlCopy = strUrl;
    }


    if ( pbHttps )
    {
        *pbHttps = bHttps;
    }

    // https 缺省端口 443
    if ( bHttps )
    {
        wPort   = 443;
    }
    // http 缺省端口80
    else
    {
        wPort   = 80;
    }


    std::string::size_type dwSlashPos = strUrlCopy.find_first_of('/');
    // URL形如: [url]
    if ( std::string::npos == dwSlashPos )
    {
        strHost = strUrlCopy;
        strPath = "/";
        if ( pHasSlash )
        {
            *pHasSlash = FALSE;
        }
        return true;
    }

    if ( pHasSlash )
    {
        *pHasSlash = TRUE;
    }

    std::string strAddr = strUrlCopy.substr(0,dwSlashPos);

    strPath = strUrlCopy.substr(dwSlashPos);
    

    std::string::size_type dwColon = strAddr.find_first_of(":");
    if ( std::string::npos == dwColon )
    {
        strHost = strAddr;
        return true;
    }

    strHost = strAddr.substr( 0, dwColon );
    DWORD dwTmp = 0;
    sscanf(strAddr.substr(dwColon+1).c_str(), " %lu", &dwTmp);
    wPort = (WORD)dwTmp;

    return true;
}


// 找到一个用于传输http request的transfer object，不使用sock5代理
CHttpTransfer *  CHttp::GetTransfer( const std::string & strHost, WORD wPort, BOOL bUseSsl )
{
    DWORD dwIp = inet_addr( strHost.c_str() );
    std::vector<CHttpTransfer *>::iterator it;
    CSockAddr  cAddr( dwIp, wPort, strHost );

    // 第一个idle trasfer
    CHttpTransfer * pFirstIdle = 0;
    for ( it = m_vTransfers.begin();  it != m_vTransfers.end(); it++ )
    {
        CHttpTransfer * pTransfer = *it;
        if ( CHttpTransfer::TRANSFER_STATE_IDLE != pTransfer->GetState() )
        {
            // 如果没有使用sock5代理，并且使用https的条件一致
            if ( !pTransfer->IfUseSock5() && pTransfer->IfUseSsl() == bUseSsl )
            {
                if ( pTransfer->GetSockAddr() == cAddr )
                {
                    return pTransfer;
                }
            }
        }
        else if ( 0 == pFirstIdle )
        {
            pFirstIdle = pTransfer;
        }
    }

    if ( 0 == pFirstIdle )
    {
        pFirstIdle = new CHttpTransfer( &g_Lock );
        m_vTransfers.push_back( pFirstIdle );
    }
    
    pFirstIdle->GetSockAddr().m_dwServerIp = dwIp;
    pFirstIdle->GetSockAddr().m_sDomain    = strHost;
    pFirstIdle->GetSockAddr().m_wPort      = wPort;
    pFirstIdle->SetUseSock5( FALSE );
    pFirstIdle->SetUseSsl( bUseSsl );
    return pFirstIdle;
}

// 通过sock5的
CHttpTransfer *  CHttp::GetTransfer( const std::string & strSock5Host, WORD wSock5Port,  const std::string & strDestHost, WORD wDestPort,
                                    const std::string & strUser, const std::string & strPwd, BOOL bUseSsl )
{
    std::vector<CHttpTransfer *>::iterator it;

    DWORD dwSock5Ip = inet_addr( strSock5Host.c_str() );
    CSockAddr  cSock5Addr( dwSock5Ip, wSock5Port, strSock5Host );

    DWORD dwDestIp = inet_addr( strDestHost.c_str() );
    CSockAddr  cDestAddr( dwDestIp, wDestPort, strDestHost );


    // 第一个idle trasfer
    CHttpTransfer * pFirstIdle = 0;
    for ( it = m_vTransfers.begin();  it != m_vTransfers.end(); it++ )
    {
        CHttpTransfer * pTransfer = *it;
        if ( CHttpTransfer::TRANSFER_STATE_IDLE != pTransfer->GetState() )
        {
            // 如果使用sock5代理，并且使用https一致
            if ( pTransfer->IfUseSock5() && pTransfer->IfUseSsl() == bUseSsl )
            {
                // 代理地址和目的地址都相同，用户名和密码相同
                if (    pTransfer->GetSockAddr() == cSock5Addr && pTransfer->GetDestAddr() == cDestAddr 
                     && pTransfer->GetSock5User() == strUser   && pTransfer->GetSock5Pwd() == strPwd  )
                {
                    return pTransfer;
                }
            }
        }
        else if ( 0 == pFirstIdle )
        {
            pFirstIdle = pTransfer;
        }
    }

    if ( 0 == pFirstIdle )
    {
        pFirstIdle = new CHttpTransfer( &g_Lock );
        pFirstIdle->SetUseSock5( TRUE );
        m_vTransfers.push_back( pFirstIdle );
    }

    pFirstIdle->GetSockAddr().m_dwServerIp = dwSock5Ip;
    pFirstIdle->GetSockAddr().m_sDomain    = strSock5Host;
    pFirstIdle->GetSockAddr().m_wPort      = wSock5Port;

    pFirstIdle->GetDestAddr().m_dwServerIp = dwDestIp;
    pFirstIdle->GetDestAddr().m_sDomain    = strDestHost;
    pFirstIdle->GetDestAddr().m_wPort      = wDestPort;

    pFirstIdle->GetSock5User()             = strUser;
    pFirstIdle->GetSock5Pwd()              = strPwd;
    pFirstIdle->SetUseSock5( TRUE );
    pFirstIdle->SetUseSsl( bUseSsl );

    return pFirstIdle;
}




// 简单的传递url
bool  CHttp::Get( const std::string & strUrl, void * context )
{
    std::string   strHost, strPath;
    WORD          wPort;

    BOOL bHasSlash = FALSE;
    BOOL bHttps    = FALSE;

    // 解析URL
    ParseUrl( strUrl, strHost, wPort, strPath, &bHasSlash, &bHttps );

    std::string sTemp = strUrl;
    if ( !bHasSlash )
    {
        sTemp += + "/";
    }

    
    // 创建request对象
    CHttpRequest  request( sTemp, CHttpRequest::HTTP_METHOD_GET );

    // lock
    CFuncLock  cLock( &g_Lock );

    // 根据url获取一个恰当的transfer
    CHttpTransfer * pTransfer = GetTransfer( strHost, wPort, bHttps );

    // 把request放在该transfer任务之后
    pTransfer->AppendTransaction( request, context );

    // step
    pTransfer->Step();

    return true;
}

// 详细设置request参数
bool CHttp::AddRequest( CHttpRequest & request, void * context /*= 0*/ )
{
    // lock
    CFuncLock  cLock( &g_Lock );

    std::string   strHost, strPath;
    WORD          wPort;

    BOOL bHasSlash = FALSE;
    BOOL bHttps    = FALSE;

    // 解析URL
    ParseUrl( request.GetUrl(), strHost, wPort, strPath, &bHasSlash, &bHttps );

    if ( !bHasSlash )
    {
        request.SetUrl( request.GetUrl() + "/" );
    }


    // 根据url获取一个恰当的transfer
    CHttpTransfer * pTransfer = 0;

    // 不需要 http proxy
    if ( CHttpRequest::PROXY_TYPE_NONE == request.GetProxyType() )
    {
        pTransfer = GetTransfer( strHost, wPort, bHttps );
        
    }
    // 需要http proxy
    else if ( CHttpRequest::PROXY_TYPE_HTTP == request.GetProxyType() )
    {
        pTransfer = GetTransfer( request.GetProxy(),  request.GetProxyPort(), bHttps );
    }
    else if ( CHttpRequest::PROXY_TYPE_SOCK5 == request.GetProxyType() )
    {
        pTransfer = GetTransfer( request.GetProxy(),  request.GetProxyPort(), strHost, wPort, 
            request.GetProxyUser(), request.GetProxyPwd(), bHttps );
    }
    else
    {
        assert ( 0 );
    }

    // 把request放在该transfer任务之后
    pTransfer->AppendTransaction( request, context );

    // step
    pTransfer->Step();   

    return true;
}




void  CHttp::CheckDnsResolved(  )
{
    // lock
    CFuncLock  cLock( &g_Lock );

    std::vector<PTDnsResolve> & v = s_vDnsResolved;

    std::vector<CHttpTransfer *>::iterator it;
    for ( it = m_vTransfers.begin();  it != m_vTransfers.end(); it++ )
    {
        CHttpTransfer * pTransfer = *it;
        if ( CHttpTransfer::TRANSFER_STATE_GETTING_HOST_NAME == pTransfer->GetState() )
        {
            std::vector<PTDnsResolve>::iterator it_d;
            for ( it_d = v.begin(); it_d != v.end(); it_d++ )
            {
                PTDnsResolve p = *it_d;
                if ( pTransfer->GetSockAddr().m_sDomain == p->szDomain )
                {
                    break;
                }
            }

            // 找到
            if ( it_d != v.end() )
            {
                pTransfer->GetSockAddr().m_dwServerIp = (*it_d)->dwIp;
                pTransfer->SetState( CHttpTransfer::TRANSFER_STATE_HOST_NAME_RESOLVED );
                pTransfer->Step();

                delete *it_d;
                v.erase( it_d );
            }
        }
        else if ( CHttpTransfer::TRANSFER_STATE_GETTING_DEST_NAME == pTransfer->GetState() )
        {
            std::vector<PTDnsResolve>::iterator it_d;
            for ( it_d = v.begin(); it_d != v.end(); it_d++ )
            {
                PTDnsResolve p = *it_d;
                if ( pTransfer->GetDestAddr().m_sDomain == p->szDomain )
                {
                    break;
                }
            }

            // 找到
            if ( it_d != v.end() )
            {
                pTransfer->GetDestAddr().m_dwServerIp = (*it_d)->dwIp;
                pTransfer->SetState( CHttpTransfer::TRANSFER_STATE_DEST_NAME_RESOLVED );
                pTransfer->Step();

                delete *it_d;
                v.erase( it_d );
            }
        }
    }

    assert( 0 == v.size() );
}


void  CHttp::CheckIdleOut( )
{
    CFuncLock  cLock( &g_Lock );

    DWORD dwCurTick = LmnGetTickCount();
    DWORD dwMax     = HTTP_IDLE_TIMEOUT * 1000;

    std::vector<CHttpTransfer *>::iterator it;
    for ( it = m_vTransfers.begin();  it != m_vTransfers.end(); it++ )
    {
        CHttpTransfer * pTransfer = *it;

        if (   CHttpTransfer::TRANSFER_STATE_OK ==  pTransfer->GetState()
            || CHttpTransfer::TRANSFER_STATE_WAITING_SERVER_CLOSE == pTransfer->GetState()
            || CHttpTransfer::TRANSFER_STATE_SSL_CONNECTING == pTransfer->GetState() )
        {
            if ( dwCurTick - pTransfer->GetLastTick() >= dwMax )
            {
                pTransfer->OnEventClose( -1 );
            }
        }
    }
}

void CHttp::PrintDebugInfo()
{
    CFuncLock cLock( &g_Lock );

    if ( 0 == m_vTransfers.size() )
    {
        MICROBLOG_LOG("no debug information! \n");
        return;
    }

    std::string  sInfo;
    char buf[8192];

    std::vector<CHttpTransfer *>::iterator  it;
    int i = 0;

    sInfo += "\n";

    for ( it = m_vTransfers.begin(), i = 0;  it != m_vTransfers.end(); it++, i++ )
    {
        SNPRINTF( buf, 8192, "%d", i+1 );
        sInfo += buf;
        sInfo += "th transfer: \n";

        CHttpTransfer * pTransfer = *it;
        pTransfer->PrintDebugInfo( sInfo );
    }

    MICROBLOG_LOG( sInfo.c_str() );
}













BOOL  s_bHttpStack = FALSE;
LmnThrdType  s_thrdSeli = 0;
LmnThrdType  s_thrdDns  = 0;


void * HttpThread(void * pArg)
{
    while( s_bHttpStack )
    {
        DWORD  dwTime = 200;
        seliSelect( &dwTime );

        CHttp * pHttp = CHttp::GetInstance( FALSE );
        if ( pHttp )
        {
            pHttp->CheckDnsResolved( );
            pHttp->CheckIdleOut();
        }

        if ( dwTime > 0 )
        {
            SLEEP( dwTime );
        }
        else
        {
            SLEEP( 20 );
        }
    }

    return 0;
}

void * DnsResolveThread( void * pArg )
{
    while( s_bHttpStack )
    {
        LmnLock( &g_Lock );
        if ( 0 == s_vDnsQuery.size() )
        {
            LmnUnlock( &g_Lock );
            SLEEP( 1000 );
        }
        else
        {
            std::vector<PTDnsResolve> vCopy;
            std::vector<PTDnsResolve>::iterator it;

            std::copy( s_vDnsQuery.begin(), s_vDnsQuery.end(), std::back_inserter(vCopy) );
            s_vDnsQuery.clear();
            LmnUnlock( &g_Lock );

            for ( it = vCopy.begin(); it != vCopy.end(); it++ )
            {
                PTDnsResolve p  = *it;
                struct hostent * pHost = gethostbyname(p->szDomain);
                if ( 0 == pHost )
                {
                    p->dwIp       = (DWORD)-1;
                }
                else
                {
                    p->dwIp         = *reinterpret_cast<DWORD *>(pHost->h_addr_list[0]);
                }
            }

            LmnLock( &g_Lock );
            std::copy( vCopy.begin(), vCopy.end(), std::back_inserter(s_vDnsResolved) );
            LmnUnlock( &g_Lock );
        }
    }
    return 0;
}


void  InitHttpStack( HttpCallback pfnCallback )
{
    InitGLock();

    if ( !::IsSeliInited() )
    { 
        ::seliInit(MAX_SELE_SOCKETS_CNT);
    }

    s_http_callback  = pfnCallback;


    s_bHttpStack = TRUE;


    s_thrdSeli = LmnCreateThread( HttpThread, 0, 0 );
    s_thrdDns  = LmnCreateThread( DnsResolveThread, 0, 0 );

    
}


void   DeinitHttpStack()
{
    s_bHttpStack = FALSE;
    LmnWait4Thrd( s_thrdSeli );
    LmnWait4Thrd( s_thrdDns );

    CHttp * pHttp = CHttp::GetInstance( FALSE );
    if ( pHttp )
    {
        delete pHttp;
    }

    ::seliEnd();
}

















