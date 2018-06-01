#ifndef __CLASS_HTTP_2012_10_24__
#define __CLASS_HTTP_2012_10_24__

#include <assert.h>
#include <string>
#include <vector>
#include <algorithm>
#include "inner.h"
#include "seli.h"

#include "ssl_adapter.h"


// key必须是小写
class  CHttpOption
{
public:
    CHttpOption( const std::string & sKey = "", const std::string & sValue = "" )
    {
        m_sKey   = sKey;
        m_sValue = sValue;
    }

    std::string        m_sKey;
    std::string        m_sValue;
};
void ClearVector( std::vector<CHttpOption*> & v );



class FindHttpOptionObj
{
public:
    FindHttpOptionObj( const std::string & sKey )
    {
        m_sKey = sKey;
    }

    bool operator() ( CHttpOption * p )
    {
        if ( 0 == STRICMP( p->m_sKey.c_str(), m_sKey.c_str() ) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    std::string  m_sKey;
};





 //------------------------------a4220a5863e7
 //Content-Disposition: form-data; name="height"

 //375
 //------------------------------a4220a5863e7
 //Content-Disposition: form-data; name="file"; filename="a.jpg"
 //Content-Type: image/jpeg

 // xxxxxxxx

class CHttpFormData
{
public:
    enum EFormatDataType
    {
        FORMAT_DATA_TYPE_NORMAL = 0,
        FORMAT_DATA_TYPE_UPLOAD
    };

    CHttpFormData( const CHttpFormData & c )
    {
        m_eType        = c.m_eType;
        m_sName        = c.m_sName;
        m_sValue       = c.m_sValue;
        m_sFileName    = c.m_sFileName;
        m_sContentType = c.m_sContentType;
    }

    // 参数bUtf8表示sValue是否为utf8格式
    // 一般name都是英文字符，属于utf8格式，自然不用转换
    CHttpFormData( const std::string & sName = "",  const std::string & sValue = "", 
                   EFormatDataType eType =  FORMAT_DATA_TYPE_NORMAL, BOOL bUtf8 = FALSE )
    {
        m_eType  = eType;
        m_sName  = sName;

        // 如果是上传文件
        if ( FORMAT_DATA_TYPE_UPLOAD == m_eType )
        {
            m_sValue = sValue;


            std::string::size_type pos;
#ifdef WIN32
            pos = m_sValue.find_last_of( '\\' );
#else
            pos = m_sValue.find_last_of( '/' );
#endif
            // 保存文件名(短形式的)
            if ( std::string::npos == pos )
            {
                m_sFileName = m_sValue;
            }
            else
            {
                m_sFileName = m_sValue.substr( pos + 1 );
            }

            std::string sExt;

            pos = m_sFileName.find( '.' );
            if ( std::string::npos != pos )
            {
                sExt = m_sFileName.substr( pos + 1 );
            }

            if ( sExt == "jpg" )
            {
                m_sContentType = "image/jpeg";
            }
            else if ( sExt == "gif" )
            {
                m_sContentType = "image/gif";
            }
            else if ( sExt == "png" )
            {
                m_sContentType = "image/png";
            }
            else if ( sExt == "bmp" )
            {
                m_sContentType = "image/bmp";
            }
            else if ( sExt == "txt" )
            {
                m_sContentType = "text/plain";
            }
            else
            {
                m_sContentType = "unknown";
            }


            if ( !bUtf8 )
            {
                char  tmpBuf[8192];
                AnsiToUtf8( m_sFileName.c_str(), tmpBuf, 8192 );
                m_sFileName = tmpBuf;
            }
        }
        else
        {
            if ( !bUtf8 )
            {
                char  tmpBuf[8192];
                AnsiToUtf8( sValue.c_str(),  tmpBuf, 8192 );
                m_sValue = tmpBuf;
            }
            else
            {
                m_sValue = sValue;
            }
        }
    }

    DWORD  GetTotalLength( DWORD dwBoundaryLen );

public:
    const std::string & GetName() const { return m_sName; }
    void  SetName( const std::string & sName ) { m_sName = sName; }

    const std::string & GetValue() const { return m_sValue; }
    void  SetValue( const std::string & sValue ) 
    {
        m_sValue = sValue;
    }

    EFormatDataType GetType() const { return m_eType; }
    void  SetType( EFormatDataType e ) { m_eType = e; }


    const std::string & GetFileName()    const { return m_sFileName; }
    const std::string & GetContentType() const { return m_sContentType; }

private:

    EFormatDataType     m_eType;
    std::string         m_sName;
    std::string         m_sValue;              // 如果是上传文件类型，则m_sValue是文件路径名

private:
    std::string         m_sFileName;           // 对上传类型有效
    std::string         m_sContentType;

};
void ClearVector( std::vector<CHttpFormData*> & v );

//
// POST /api/login HTTP/1.1
// User-Agent: Movision SDK/1.0
// Host: 10.23.46.26
// Accept: 
//         Content-Length: 93
//         Content-Type: application/x-www-form-urlencoded
// 
//         account_token=0233ad4f2c015aac578cd8e23fa83e7d&username=shijie%40kedacom.com&password=1231234
//
//  method POST
//  User-Agent, Host, Accept: options
//  ccount_token=0233...:   data
//

class CHttpRequest
{
public:
    enum EHttpMethod
    {
        HTTP_METHOD_GET        = 0,
        HTTP_METHOD_POST
    };

    enum EContentType
    {
        HTTP_CONTENT_TYPE_NORMAL = 0,
        HTTP_CONTENT_TYPE_FORM
    };

    enum EProxyType
    {
        PROXY_TYPE_NONE,
        PROXY_TYPE_HTTP,
        PROXY_TYPE_SOCK5
    };

    CHttpRequest( const std::string & sUrl = "", EHttpMethod eMethod = HTTP_METHOD_GET, const std::string & sData = "", 
                  EContentType eContentType = HTTP_CONTENT_TYPE_NORMAL )
    {
        m_sUrl = sUrl;
        assert( eMethod <= HTTP_METHOD_POST && eMethod >= HTTP_METHOD_GET );
        m_eMethod        = eMethod;
        m_sData          = sData;
        m_wProxyport     = 0;
        m_eProxyType     = PROXY_TYPE_NONE;

        m_eContentType = eContentType;
    }

    ~CHttpRequest()
    {
        ClearVector( m_vOptions );
        ClearVector( m_vFormatData );
    }
    
    std::string & operator [] ( const std::string & sOptionKey )
    {
        std::vector<CHttpOption*>::iterator it = std::find_if( m_vOptions.begin(), m_vOptions.end(), FindHttpOptionObj(sOptionKey) );
        if ( m_vOptions.end() == it )
        {
            CHttpOption * pOption = new CHttpOption( sOptionKey );
            m_vOptions.push_back( pOption );
            return pOption->m_sValue;
        }
        else
        {
            return (*it)->m_sValue;
        }
    }

    BOOL  IfKeyExist( const std::string & sOptionKey, std::string & sOptionValue ) const
    {
        std::vector<CHttpOption*>::const_iterator it = std::find_if( m_vOptions.begin(), m_vOptions.end(), FindHttpOptionObj(sOptionKey) );
        if ( m_vOptions.end() == it )
        {
            return FALSE;
        }
        else
        {
            sOptionValue = (*it)->m_sValue;
            return TRUE;
        }
    }

    void SetData( const std::string & sData )
    {
        m_sData = sData;
    }

    EHttpMethod GetMethod() const { return m_eMethod; }
    const std::string & GetData() const { return m_sData; }
    const DWORD         GetOptionsCnt() const { return m_vOptions.size(); }
    CHttpOption *       GetOption( DWORD dwIndex )
    { 
        if ( dwIndex < m_vOptions.size() ) 
        {
            return m_vOptions[dwIndex];
        }
        else
        {
            return 0;
        }
    }

    CHttpRequest & operator = ( const CHttpRequest & c )
    {
        m_eMethod = c.m_eMethod;
        m_sData   = c.m_sData;
        m_sUrl    = c.m_sUrl;
        m_eProxyType = c.m_eProxyType;
        m_sProxy     = c.m_sProxy;
        m_wProxyport = c.m_wProxyport;
        m_sProxyUser     = c.m_sProxyUser;
        m_sProxyPwd      = c.m_sProxyPwd;
        m_eContentType   = c.m_eContentType;
        

        ClearVector( m_vOptions );
        std::vector<CHttpOption*>::const_iterator it;

        for ( it = c.m_vOptions.begin();  it != c.m_vOptions.end(); it++ )
        {
            CHttpOption* pOption = *it;
            CHttpOption* pNew    = new CHttpOption;

            pNew->m_sKey   = pOption->m_sKey;
            pNew->m_sValue = pOption->m_sValue;

            m_vOptions.push_back( pNew );
        }

        ClearVector( m_vFormatData );
        std::vector<CHttpFormData*>::const_iterator it_f;

        for ( it_f = c.m_vFormatData.begin();  it_f != c.m_vFormatData.end();  it_f++ )
        {
            CHttpFormData * pItem = *it_f;
            CHttpFormData * pNewItem = new CHttpFormData( *pItem );

            m_vFormatData.push_back( pNewItem );
        }

        return *this;
    }

    const std::string & GetUrl() const { return m_sUrl; }
    void  SetUrl( const std::string & sUrl ) { m_sUrl = sUrl; }

    const std::string & GetProxy() const { return m_sProxy; }
    WORD                GetProxyPort() const { return m_wProxyport; }
    EProxyType          GetProxyType() const { return m_eProxyType; }
    void SetProxy( const std::string sProxy, WORD wPort = 8080, EProxyType proxyType = PROXY_TYPE_HTTP )
    {
        if ( proxyType > PROXY_TYPE_SOCK5 )
        {
            proxyType = PROXY_TYPE_NONE;
        }

        m_sProxy     = sProxy;
        m_wProxyport = wPort;
        m_eProxyType = proxyType;
    }

    const std::string & GetProxyUser() const { return m_sProxyUser; }
    const std::string & GetProxyPwd()  const { return m_sProxyPwd; }
    void SetProxyUser( const std::string & sProxyUser ) { m_sProxyUser = sProxyUser; }
    void SetProxyPwd ( const std::string & sProxyPwd  ) { m_sProxyPwd  = sProxyPwd; }

    void  SetOptions( const std::string & sOptions )
    {
        std::string::size_type  last_pos = 0;
        std::string::size_type  cur_pos  = 0;

        cur_pos = sOptions.find( "\r\n", last_pos );
        while( TRUE )
        {
            std::string  sOption;

            if ( cur_pos != std::string::npos )
            {
                sOption = sOptions.substr( last_pos, cur_pos - last_pos );
            }
            else
            {
                sOption = sOptions.substr( last_pos );
            }

            
            std::string::size_type pos = sOption.find( ':' );
            if ( std::string::npos != pos )
            {
                std::string sKey   = sOption.substr( 0, pos );
                std::string sValue = sOption.substr( pos + 1 );

                char szKey[4096] = { 0};
                char szValue[4096] = { 0 };

                strncpy( szKey,   sKey.c_str(),   4096 -1 );
                strncpy( szValue, sValue.c_str(), 4096 -1 );

				MyStrTrim( szKey );
				MyStrTrim( szValue );

                // option key和value均不能为0
                if ( szKey[0] != '\0' && szValue[0] != '\0' )
                {
                    CHttpOption * pOption = new CHttpOption;
                    pOption->m_sKey   = szKey;
                    pOption->m_sValue = szValue;

                    m_vOptions.push_back( pOption );                
                }
            }

            if ( cur_pos == std::string::npos )
            {
                break;
            }

            last_pos = cur_pos + 2;
            cur_pos = sOptions.find( "\r\n", last_pos );
        }
    }

    EContentType GetContentType() const { return m_eContentType; }
    void         SetContentType( EContentType e ) { m_eContentType = e; }

    // 参数bUtf8表示传入参数sName， sValue是否已经为utf8格式
    void AppendFormatData( const std::string & sName,  const std::string & sValue, 
                           CHttpFormData::EFormatDataType eType =  CHttpFormData::FORMAT_DATA_TYPE_NORMAL, BOOL bUtf8 = FALSE )
    {
        if ( sName.length() > 0 )
        {
            CHttpFormData * pNewFormat = new CHttpFormData( sName, sValue, eType, bUtf8 );
            m_vFormatData.push_back( pNewFormat );
        }
    }

    // key=value url格式
    void AppendFormatData( const std::string & sLine,  CHttpFormData::EFormatDataType eType =  CHttpFormData::FORMAT_DATA_TYPE_NORMAL )
    {
        std::string::size_type pos = sLine.find( '=' );
        // 无效
        if ( pos == std::string::npos )
        {
            return;
        }

        std::string sName  = sLine.substr( 0, pos );
        std::string sValue = sLine.substr( pos + 1 );

        char buf[8192] = { 0 };
        GetStringFromUrl( buf, 8192, sValue.c_str() );
        
        // 转换后的字符串即为utf-8

        CHttpFormData * pNewFormat = new CHttpFormData( sName, buf, eType, TRUE );
        m_vFormatData.push_back( pNewFormat );
    }

    DWORD  GetTotalFormDataLen( DWORD dwBoundaryLen )
    {
        DWORD dwTotalLen = 0;

        std::vector<CHttpFormData*>::iterator it;
        for ( it = m_vFormatData.begin(); it != m_vFormatData.end(); it++ )
        {
            CHttpFormData * pItem = *it;
            
            dwTotalLen += pItem->GetTotalLength(dwBoundaryLen);
        }

        dwTotalLen += 2 + dwBoundaryLen + 2 + 2;

        return dwTotalLen;
    }

private:
    EHttpMethod                 m_eMethod;
    std::vector<CHttpOption*>   m_vOptions;
    std::string                 m_sData;
    std::string                 m_sUrl;

    EProxyType                  m_eProxyType;
    std::string                 m_sProxy;
    WORD                        m_wProxyport;
    std::string                 m_sProxyUser;
    std::string                 m_sProxyPwd;

    EContentType                   m_eContentType;

public:
    std::vector<CHttpFormData*>    m_vFormatData;
};



//HTTP/1.1 200 OK
//Server: nginx
//Date: Wed, 24 Oct 2012 03:24:09 GMT
//      Content-Type: text/xml;charset=UTF-8
//      Transfer-Encoding: chunked
//Connection: keep-alive
//Pragma: No-cache
//Expires: Thu, 01 Jan 1970 00:00:00 GMT
//         Cache-Control: no-cache
//
//
//
//         <Login>
//         <success>true</success>
//         <description>...............</description>
//         <username>shijie@kedacom.com</username>
//         </Login>
// 

class CHttpResponse
{
public:
    enum EProgressState
    {
        PROGRESS_VERSION,
		PROGRESS_VERSION_CARRIAGE_RETURN,       // 寻找'\xa'
        PROGRESS_OPTION,
		PROGRESS_OPTION_CARRIAGE_RETURN,        // 寻找'\xa'
        PROGRESS_EMPTY_LINE_CARRIAGE_RETURN,    // 寻找'\xa'
        PROGRESS_CHUNK_SIZE,
        PROGRESS_CHUNK_SIZE_CARRIAGE_RETURN,
        PROGRESS_DATA,
        PROGRESS_CHUNK_DATA_EMPTY_LINE,
        PROGRESS_OK
    };

    CHttpResponse( DWORD dwState = 0 )
    {
        Clear();
        m_dwHttpState   = dwState;
    }

    ~CHttpResponse()
    {
        ClearVector( m_vOptions );
    }

    // 只读
    const char * operator [] ( const std::string & sOptionKey ) const 
    {
        std::vector<CHttpOption*>::const_iterator it = std::find_if( m_vOptions.begin(), m_vOptions.end(), FindHttpOptionObj(sOptionKey) );
        if ( m_vOptions.end() == it )
        {
            return 0;
        }
        else
        {
            return (*it)->m_sValue.c_str();
        }
    }


    DWORD GetHttpState() const { return m_dwHttpState; }
    BOOL  IfChucked() const { return m_bChucked; }
    EProgressState GetProgressState() const { return m_eState; }
    void  SetProgressState( EProgressState state ) { m_eState = state; }
    void  SetHttpCode( DWORD dwCode ) { m_dwHttpState = dwCode; }

    void  AppendOption( const char * key )
    {
        CHttpOption * pOption = new CHttpOption( key );
        m_vOptions.push_back( pOption );
    }

    void  SetLastOptionValue( const char * value )
    {
        if ( m_vOptions.size() > 0 )
        {
            CHttpOption * pOption = m_vOptions[m_vOptions.size() - 1];
            pOption->m_sValue = value;
        }        
    }

    const CHttpOption * GetLastOption()
    {
        if ( m_vOptions.size() > 0 )
        {
            CHttpOption * pOption = m_vOptions[m_vOptions.size() - 1];
            return pOption;
        }
        else
        {
            return 0;
        }
    }
    void  SetChunked( BOOL b ){ m_bChucked = b; }
    void  SetChunkSize( DWORD dwSize ) { m_dwChunkSize = dwSize; }
    DWORD GetChunkSize() const { return m_dwChunkSize; }

    static DWORD HexStr2ULong( const char * szHex )
    {
        DWORD i;
        DWORD n = strlen( szHex );
        DWORD dwCnt = 0;

        for ( i = 0; i < n; i++ )
        {
            DWORD t = 0;

            if ( szHex[i] >= '0' && szHex[i] <='9' )
            {
                t = szHex[i] - '0';
            }
            else if ( szHex[i] >= 'a' && szHex[i] <='f' )
            {
                t = szHex[i] - 'a' + 10;
            }

            dwCnt = dwCnt * 16 + t;
        }

        return dwCnt;
    }

    void AppendData( const char * buf, int nSize )
    {
        m_cData.Append( (char*)buf, nSize );
    }

    DWORD GetDataSize() const { return m_cData.GetDataLength(); }

    const char * GetData() const 
    {
        return (const char *)m_cData.GetData();
    }

    DWORD GetContentLen() const { return m_dwContentLen; }

    void  SetContentLen( DWORD n ) { m_dwContentLen = n; }

    BOOL  HasLocation() const 
    {
        if ( 0 == (*this)["Location"] )
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    void  Clear()
    {
        m_eState       = PROGRESS_VERSION;
        m_dwHttpState  = 0;
        m_bChucked     = FALSE;
        m_dwChunkSize  = 0;
        m_dwContentLen = 0;
        
        m_cData.Clear();
        m_cTempBuf.Clear();

        ClearVector( m_vOptions );
    }

    const char * GetLocation()
    {
        return (*this)["Location"];
    }

    BOOL  HasContentLen() const 
    {
        if ( 0 == (*this)["Content-Length"] )
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    BOOL  ServerConnectionClose() const 
    {
        std::vector<CHttpOption*>::const_iterator it = std::find_if( m_vOptions.begin(), m_vOptions.end(), FindHttpOptionObj("Connection") );
        if ( m_vOptions.end() == it )
        {
            return FALSE;
        }

        if ( 0 == STRICMP( (*it)->m_sValue.c_str(), "close" ) )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    const std::vector<CHttpOption*> & GetOptions() const { return m_vOptions; }

private:
    EProgressState              m_eState;
    DWORD                       m_dwHttpState;
    std::vector<CHttpOption*>   m_vOptions;
    BOOL                        m_bChucked;
    DWORD                       m_dwChunkSize;
    DWORD                       m_dwContentLen;
public:
    CDataBuf                    m_cData;

public:
    CDataBuf                    m_cTempBuf;
};









class CHttpTransaction
{
public:
    enum EHttpTransacState
    {
        TRANSACTION_STATE_NONE,
        TRANSACTION_STATE_REQUESTING,
        TRANSACTION_STATE_OK
    };

    CHttpTransaction( void * context ): m_eState(TRANSACTION_STATE_NONE){ m_pContext = context; }

    CHttpRequest       m_request;
    CHttpResponse      m_response;
    EHttpTransacState  m_eState;
    void *             m_pContext;

    CDataBuf           m_TotalHeader;
    std::string        m_sLastUrl;

    void PrintDebugInfo( std::string & sInfo );
};
void ClearVector( std::vector<CHttpTransaction*> & v );







class CSockAddr
{
public:
    CSockAddr( DWORD dwIp = (DWORD)-1, WORD wPort = 0, const std::string & domain = "") : m_dwServerIp(dwIp), m_wPort(wPort), m_sDomain(domain){}

    DWORD          m_dwServerIp;
    WORD           m_wPort;
    std::string    m_sDomain;

    CSockAddr & operator = ( const CSockAddr & c )
    {
        m_dwServerIp = c.m_dwServerIp;
        m_wPort      = c.m_wPort;
        m_sDomain    = c.m_sDomain;
        return *this;
    }
};







class CHttpTransfer
{
public:
    enum EHttpTransferState
    {
        TRANSFER_STATE_IDLE,
        TRANSFER_STATE_GETTING_HOST_NAME,                                       // 正在等域名解析
        TRANSFER_STATE_HOST_NAME_RESOLVED,                                      // 域名解析完毕

        TRANSFER_STATE_GETTING_DEST_NAME,                                       // 正在等Dest域名解析
        TRANSFER_STATE_DEST_NAME_RESOLVED,                                      // Dest域名解析完毕

        TRANSFER_STATE_CONNECTING,                                              // 正在连接http server
        TRANSFER_STATE_OK,                                                      // 连接上http server
        TRANSFER_STATE_WAITING_SERVER_CLOSE,                                    // 等待服务器关闭
        TRANSFER_STATE_SSL_CONNECTING,
    };

    CHttpTransfer( LmnLockType * pLock )
    {
        m_eState = TRANSFER_STATE_IDLE; 
        m_pLock = pLock; 
        m_fd = -1; 
        m_dwId = ++s_dwId; 
        m_bUseSock5 = FALSE;
        m_bUseSsl   = FALSE;
        m_pSslAdap  = 0;
    }

    void Clear()
    {
        ClearVector( m_vTransactions );

        if ( m_pSslAdap )
        {
            lmn_base::CSslAdapter::Destroy( m_pSslAdap );
            m_pSslAdap = 0;
        }
    }
    ~CHttpTransfer()
    {
        Clear();
    }

    void PrintDebugInfo( std::string & sInfo );
    

    EHttpTransferState  GetState() const { return m_eState; }
    void                SetState( EHttpTransferState e ) { m_eState = e; }
    CSockAddr &         GetSockAddr() { return m_cPeerAddr; }
    void                AppendTransaction( const CHttpRequest & request, void * context );   
    void                Step();

    BOOL                IfUseSock5() const { return m_bUseSock5; }
    void                SetUseSock5( BOOL b ) { m_bUseSock5 = b; }
    CSockAddr &         GetDestAddr() { return m_cDestAddr; }

    BOOL                IfUseSsl() const { return m_bUseSsl; }
    void                SetUseSsl( BOOL b ) 
    {
        m_bUseSsl = b; 
        if ( m_bUseSsl && 0 == m_pSslAdap )
        {
            // 创建一个ssl adapter对象
            m_pSslAdap = lmn_base::CSslAdapter::Create();
        }
    }

    void                Reconnect();

    std::string &       GetSock5User() { return m_sSock5User; }
    std::string &       GetSock5Pwd()  { return m_sSock5Pwd; }

    DWORD               GetLastTick() const { return m_dwLastActiveTick; }
    void                SetLastTick( DWORD d ) { m_dwLastActiveTick = d; }


private:
    CSockAddr                        m_cPeerAddr;
    std::vector<CHttpTransaction*>   m_vTransactions;
    EHttpTransferState               m_eState;
    DWORD                            m_dwId;
    LmnLockType *                    m_pLock;
    FdType                           m_fd;

    CSockAddr                        m_cDestAddr;
    BOOL                             m_bUseSock5;

    BOOL                             m_bUseSsl;
    lmn_base::CSslAdapter *          m_pSslAdap;

    std::string                      m_sSock5User;
    std::string                      m_sSock5Pwd;
    DWORD                            m_dwLastActiveTick;


    static DWORD                     s_dwId;
    static void OnLiEvent( FdType fd, int liEvent, int error, void * context );

    static std::string GenerateBoundary();

public:
    void    OnEventClose( int nErr = 0 );
private:
    void    OnEventRead( );
    void    OnEventConnected( );

    // 发送request
    void    BeginTransaction( CHttpTransaction * pTransaction);

    BOOL    ParseReceived( const char * buf, int nBufSize, CHttpResponse * pResponse, int & nConsumedSize, CDataBuf & cTotalHeader );
	BOOL    ParseLine( CHttpResponse * pResponse );
};
void ClearVector( std::vector<CHttpTransfer*> & v );







const DWORD MAX_HTTP_CONNECTION_COUNT = 5;
const DWORD MAX_SELE_SOCKETS_CNT      = 8;
const DWORD HTTP_CONNECTION_TIMEOUT   = 5;       //单位:秒
const DWORD HTTP_RECEIVE_BUFFER_SIZE  = 8192;
#ifdef _DEBUG
const DWORD HTTP_IDLE_TIMEOUT         = 5;      //单位:秒
#else
const DWORD HTTP_IDLE_TIMEOUT         = 60;    //单位:秒
#endif

#define  MAX_DNS_DOMAIN_NAME_LEN      32

typedef  struct  tagDnsResolve
{
    char    szDomain[MAX_DNS_DOMAIN_NAME_LEN];
    DWORD   dwIp;
    tagDnsResolve()
    {
        memset( this, 0, sizeof(*this) );
    }
}TDnsResolve, *PTDnsResolve;






class CHttp
{
public:
    static CHttp * GetInstance( BOOL bCreate = TRUE )
    {
        if ( bCreate )
        {
            if ( 0 == s_pInstance )
            {
                s_pInstance = new CHttp();
            }
            return s_pInstance;
        }
        else
        {
            return s_pInstance;
        }
    }

    ~CHttp()
    {
        ClearVector( m_vTransfers );
        s_pInstance = 0;
    }

private:
    CHttp(){ InitGLock();  }

public:
    // 简单的传递Url
    bool Get ( const std::string & strUrl, void * context = 0 );

    // 简单的Post Url
    // bool Post( const std::string & strUrl, const std::string & strData = "", void * context = 0 );

    // 详细设置request参数
    bool AddRequest( CHttpRequest & request, void * context = 0 );

    
    void CheckDnsResolved(  );

    void CheckIdleOut( );

    void PrintDebugInfo();

private:
    std::vector<CHttpTransfer *>       m_vTransfers;

    static CHttp * s_pInstance;
   
    // 从url获取host, port和path
    static  bool  ParseUrl( const std::string & strUrl, std::string & strHost, WORD & wPort, std::string & strPath , BOOL * pHasSlash = 0, BOOL * pbHttps = 0 );

    // 找到一个用于传输http request的transfer object
    CHttpTransfer *  GetTransfer( const std::string & strHost, WORD wPort, BOOL bUseSsl );

    CHttpTransfer *  GetTransfer( const std::string & strSock5Host, WORD wSock5Port,  const std::string & strDestHost, WORD wDestPort,
        const std::string & strUser, const std::string & strPwd, BOOL bUseSsl  );

    friend class CHttpTransfer;
};






typedef void (*HttpCallback)( int nError, DWORD dwCode, const char * szData, DWORD dwDataLen, 
                              const char * szHeader, DWORD dwHeaderLen,  void * context );
void   InitHttpStack( HttpCallback pfnCallback );
void   DeinitHttpStack();



#endif


