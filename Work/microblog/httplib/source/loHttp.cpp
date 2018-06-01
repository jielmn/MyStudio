#include "setup.h"
#include "loHttpAssoc.h"
#include "loHttpCurl.h"
#include "loUrlcode.h"
#include "lostrconv.h"
#include "loHttpStruct.h"

#include "libhttp/loHttp.h"
#include "libhttp/thpool.h"

extern "C"
{
	#include "spthread.h"
};

using namespace lohttp;


void  CloHttp::OnHttpStackCb(int nError, unsigned long dwCode, const char * szData, unsigned long dwDataLen, 
                             const char * szHeader, unsigned long dwHeaderLen,  void * context)
{
#if SHOW_HTTP_DATA
    if ( szHeader )
    {
        const char * szTitle = "Received header:";
        unsigned long dwTitleLen = strlen( szTitle );

        char * p = new char[dwHeaderLen+1+3+dwTitleLen];
        memcpy( p, szTitle, dwTitleLen );
        p[dwTitleLen] = '\n';
        memcpy( p+1+dwTitleLen, szHeader, dwHeaderLen );
        p[1+dwTitleLen+dwHeaderLen] = '\n';
        p[1+dwTitleLen+dwHeaderLen+1] = '\n';
        p[1+dwTitleLen+dwHeaderLen+2] = '\0';
        MICROBLOG_LOG( "%s", p );
        delete[] p;
    }

    if ( szData )
    {
        const char * szTitle = "Received data:";
        unsigned long dwTitleLen = strlen( szTitle );

        char * p = new char[dwDataLen+1+3+dwTitleLen];
        memcpy( p, szTitle, dwTitleLen );
        p[dwTitleLen] = '\n';
        memcpy( p+1+dwTitleLen, szData, dwDataLen );
        p[1+dwTitleLen+dwDataLen] = '\n';
        p[1+dwTitleLen+dwDataLen+1] = '\n';
        p[1+dwTitleLen+dwDataLen+2] = '\0';
        MICROBLOG_LOG( "%s", p );
        delete[] p;
    }
#endif


    CloHttp * pHttp = (CloHttp *)context;

    CloHttpResponse * pResponse = pHttp->CreateHttpResponse( dwCode, szData, dwDataLen, szHeader, dwHeaderLen );
    pHttp->m_pHttpAssoc->OnFinished( pResponse );

    int nSleepCnt = 0;
    while ( !pHttp->m_bStartAsynFinished && nSleepCnt < 3 )
    {
        // sleep 100毫秒
        SLEEP( 100 );
        nSleepCnt++;
    }

    if ( pHttp->m_bStartAsynFinished )
    {
        pHttp->m_pHttpAssoc->OnTaskState( E_HTTPSTATUS_COMPLETE );
    }
}


CloHttp::CloHttp()
// : m_pHttpAssoc( new CloHttpAssoc() )
{
    m_bStartAsynFinished = 0;
	m_pHttpAssoc = new CloHttpAssoc();
	if ( m_pHttpAssoc == NULL )
	{
		printf("CloHttp() ==> new m_pHttpAssoc failed!\n");
	}
}

CloHttp::~CloHttp(void)
{
// 	Cancel();

	if ( m_pHttpAssoc != NULL )
	{
		delete m_pHttpAssoc;
		m_pHttpAssoc = NULL;
	}
}

void CloHttp::AddBinaryData( const char* szBinaryData, long lLen )
{
	if ( m_pHttpAssoc != NULL && szBinaryData != NULL && lLen > 0 )
	{
		m_pHttpAssoc->m_binData.WriteBytes( szBinaryData, lLen );
	}
	else
	{
		printf("AddBinaryData ==> Get something wrong! Return!\n");
	}
	return;
}

void CloHttp::ClearBinaryData(void)
{
	if ( m_pHttpAssoc == NULL )
	{
		printf("ClearBinaryData ==> m_pHttpAssoc is NULL! Return!\n");
		return;
	}
	m_pHttpAssoc->m_binData.Shift( 0 );
	return;
}

void  CloHttp::SetFileName(const HTTPChar* szPath, const HTTPChar* szFile)
{
	if ( m_pHttpAssoc == NULL )
	{
		printf("SetFileName ==> m_pHttpAssoc is NULL! Return!\n");
		return;
	}

	if ( szPath != NULL )
	{
		HTTP_TCSCPY_S( m_pHttpAssoc->m_szPath, HTTP_TCSLEN(szPath) + 1, szPath );
	}

	if ( szFile != NULL )
	{
		HTTP_TCSCPY_S( m_pHttpAssoc->m_szFile, HTTP_TCSLEN(szFile) + 1, szFile );
	}
	return;
}

void CloHttp::ClearParams()
{
	if ( m_pHttpAssoc == NULL )
	{
		printf("ClearParams ==> m_pHttpAssoc is NULL! Return!\n");
		return;
	}
	m_pHttpAssoc->ClearParams();
	return;
}

void CloHttp::ClearHeader()
{
	if ( m_pHttpAssoc == NULL )
	{
		printf("ClearHeader ==> m_pHttpAssoc is NULL! Return!\n");
		return;
	}
	m_pHttpAssoc->ClearHeader();
	return;
}

//int CloHttp::Split_url_param(const HTTPChar*& next , const HTTPChar*& key ,
//							 const HTTPChar*& qe , const HTTPChar*& split ,const char endflag )
//{
//	if( !next || *next == '\0' )
//		return -1;
//
//	key = next; 
//	// 前面的空格除掉
//	do
//	{
//		if( *key != 0x20 || *key == '\0' ) break;
//		key++;
//	}while(1);
//
//	qe = strchr(key , '=' );
//	if( !qe )
//		return -1;
//
//	split = strchr( (qe+1) , endflag);
//
//	if(split) next = (split + 1 );
//	else next = 0;
//
//	return 0;
//}
//
//int CloHttp::Split_url_copy_keyval(HTTPChar* val , const HTTPChar* start , const HTTPChar* end)
//{
//	if( !val || !start || *start == '\0' )
//		return -1;
//
//	if( end && end <= start )
//		return -1;
//
//	if( !end || *end == '\0' )
//	{
//		strcpy(val , start );
//	}
//	else
//	{
//		strncpy(val , start , end - start );		
//	}
//	return 0;
//}

void CloHttp::InitURL( const HTTPChar* szUrl, int method )
{
	HTTPChar szURI[ConstURLLength] = {0};
	HTTP_TCSCPY_S( szURI, ConstURLLength, szUrl );

	if ( E_HTTP_GET_METHOD != method &&	E_HTTP_DOWNLOAD_METHOD != method )
	{
		HTTPChar* szFind = HTTP_TCSCHR( szURI, HTTP_T('?') );
		if ( szFind != NULL )
		{
			HTTPChar* szFind1 = szFind;
			szFind1++;

			HTTPChar* key = 0;
			HTTPChar* param = HTTP_TCSCHR( szFind1, HTTP_T('&') );
			*szFind = HTTP_T('\0');
			do
			{
				if ( param != NULL )
				{
					*param = HTTP_T('\0');
					param++;
				}
				key = HTTP_TCSCHR( szFind1, HTTP_T('=') );
				if ( key )
				{
					*key = HTTP_T('\0');
					key++;
					AddParam( szFind1, key );
				}
				else
				{
					assert(0);
				}
				szFind1 = param;
				if ( !szFind1 || *szFind1 == HTTP_T('\0') )
				{
					break;
				}
				else
				{
					param = HTTP_TCSCHR( szFind1, HTTP_T('&') );
				}
			}while(1);				
		}		
	}
	SetURL(szURI);
}


CloHttpResponse* CloHttp::CreateHttpResponse(DWORD dwCode /*= 0*/, const char * pBody /*= 0*/, DWORD dwBody /*= 0*/, const char * pHeader /*= 0*/, DWORD dwHeader /*= 0*/ )
{
	THttpMemoryBuffer header;
	THttpMemoryBuffer body;

	//if ( m_pHttpAssoc == NULL || m_pHttpAssoc->m_pHttpCurl == NULL )
	//{
	//	printf("HttpMethodGet ==> m_pHttpAssoc or m_pHttpAssoc->m_pHttpCurl is NULL! Return!\n");
	//	return NULL;
	//}

	//m_pHttpAssoc->m_pHttpCurl->GetResponedHeader( &header );
	//m_pHttpAssoc->m_pHttpCurl->GetResponedBody( &body );

    SAFE_NEW_MEMORYBUFFER( &header, dwHeader+1, dwHeader );
    memcpy( header.buffer, pHeader, dwHeader );
    header.buffer[dwHeader] = '\0';

    SAFE_NEW_MEMORYBUFFER( &body, dwBody+1, dwBody );
    memcpy( body.buffer, pBody, dwBody );
    body.buffer[dwBody] = '\0';

	CloHttpResponse *pResponse = new CloHttpResponse();
	if ( pResponse == NULL || pResponse->m_pResponseAssoc == NULL )
	{
		printf("CreateHttpResponse ==> new failed! Return!\n");
		return NULL;
	}

	pResponse->m_pResponseAssoc->SetHttpResponse( &header, &body );
	// pResponse->m_pResponseAssoc->SetHttpCode( m_pHttpAssoc->m_pHttpCurl->GetHttpCode() );
    pResponse->m_pResponseAssoc->SetHttpCode( dwCode  );

	SAFE_DELETE_MEMORYBUFFER( &header );
	SAFE_DELETE_MEMORYBUFFER( &body );

	return pResponse;
}

CloHttpResponse* CloHttp::GetRespone(void) const
{
	if ( m_pHttpAssoc == NULL )
	{
		printf("GetRespone ==> m_pHttpAssoc is NULL! Return!\n");
		return NULL;
	}
	return m_pHttpAssoc->m_pRespone;
}

CloHttpResponse* CloHttp::DetachRespone(void)
{
	CloHttpResponse* pDetachRespone = NULL;
	if ( m_pHttpAssoc == NULL )
	{
		printf("DetachRespone ==> m_pHttpAssoc is NULL! Return!\n");
		return NULL;
	}
	pDetachRespone = m_pHttpAssoc->m_pRespone;
	m_pHttpAssoc->m_pRespone = NULL;

	return pDetachRespone;
}

THttpTime* CloHttp::GetHttpTime() const
{
	if ( m_pHttpAssoc == NULL )
	{
		printf("GetHttpTime ==> m_pHttpAssoc is NULL! Return!\n");
		return NULL;
	}
	return &m_pHttpAssoc->m_time;
}



int CloHttp::Request( const HTTPChar* szUrl, int method )
{
	int iret = -1;

	if ( m_pHttpAssoc == NULL )
	{
		printf("Request ==> m_pHttpAssoc is NULL! Return!\n");
		return -1;
	}

	{
		if ( szUrl != NULL && *szUrl != '\0' ) 
		{
			InitURL( szUrl, method );
		}
		else if ( *(m_pHttpAssoc->m_szURL) != '\0' )
		{
			InitURL( m_pHttpAssoc->m_szURL, method );
		}
		else
		{
			printf("Request ==> Find no CORRECT URL! Error!\n");
			return -1;
		}

		if ( m_pHttpAssoc->m_pHttpCurl == NULL ) 
		{
			printf("Request ==> m_pHttpAssoc->m_pHttpCurl is NULL!\n");
			return -1;
		}
	}

	m_pHttpAssoc->OnStart();
    m_pHttpAssoc->m_pHttpCurl->m_pContext = this;

	switch ( method )
	{
	case E_HTTP_GET_METHOD:
		iret = HttpMethodGet();
		break;

	case E_HTTP_POST_METHOD:
		iret = HttpMethodPost();
		break;

//	case E_HTTP_POST_BINARY_METHOD:
//		METHOD_CALLBACK( HttpMethodPostBinary );

//	case E_HTTP_UPLOAD_BINARY_TEXT_METHOD:
//		METHOD_CALLBACK( HttpMethodPostBinaryText );

	case E_HTTP_UPLOAD_METHOD:
		iret = HttpMethodUpload();
		break;

	case E_HTTP_DELETE_METHOD:
		iret = HttpMethodDelete();
		break;

	case E_HTTP_PUT_METHOD:
		iret = HttpMethodPut();
		break;

	case E_HTTP_DOWNLOAD_METHOD:
		iret = HttpMethodDownload();
		break;

	default:
		printf("Request ==〉no method match!\n");
		break;
	}

	//m_pHttpAssoc->OnFinished( CreateHttpResponse() );

	//m_pHttpAssoc->OnTaskState( E_HTTPSTATUS_COMPLETE );

	return iret;
}

// 异步请求（启动线程）
void CloHttp::RequestAsyn( const HTTPChar* szUrl, int method, void *p_threadpool )
{
	if ( m_pHttpAssoc == NULL )
	{
		printf("RequestAsyn ==> m_pHttpAssoc is NULL! Return!\n");
		return;
	}

	{
		if ( szUrl != NULL && *szUrl != '\0' ) 
		{
			InitURL( szUrl, method );
		}
		else if ( *(m_pHttpAssoc->m_szURL) != '\0' )
		{
			InitURL( m_pHttpAssoc->m_szURL, method );
		}
		else
		{
			printf("Request ==> Find no CORRECT URL! Error!\n");
			return;
		}

		if ( m_pHttpAssoc->m_pHttpCurl == NULL ) 
		{
			printf("Request ==> m_pHttpAssoc->m_pHttpCurl is NULL!\n");
			return;
		}
	}
	
	m_pHttpAssoc->m_nMethod = method;

//  	sp_thread_create(NULL, NULL, (sp_thread_func_t)CloHttpAssoc::HttpThread, this);

//#ifndef _IOS_PLATFORM_
//	thpool_add_work( p_threadpool, CloHttpAssoc::HttpThread, this );
//#else
//	sp_thread_t thread_handle;
//	sp_thread_create( &thread_handle, NULL, (sp_thread_func_t)CloHttpAssoc::HttpThread, this );
//	sp_thread_detach(thread_handle);
//#endif


    StartAsyn();
    m_bStartAsynFinished = 1;
}

void CloHttp::Wait()
{
	if ( m_pHttpAssoc == NULL || m_pHttpAssoc->m_hInetTh == NULL )
	{
		printf("Wait ==> m_pHttpAssoc or m_pHttpAssoc->m_hInetTh is NULL! return!\n");
		return ;
	}

// 	WaitForSingleObject( m_pHttpAssoc->m_hInetTh , INFINITE );
	printf("Wait ==>   no wait routine is implemented right now!\n");
	return;
}


unsigned long CloHttp::StartAsyn()
{
	if ( m_pHttpAssoc != NULL )
	{
		return Request( NULL, m_pHttpAssoc->m_nMethod );
	}
	else
	{
		printf("StartAsyn ==> odd! m_pHttpAssoc should not be NULL!\n");
		return 0;
	}
}

bool CloHttp::Cancel( unsigned long msTimeout )
{
	if ( m_pHttpAssoc == NULL || m_pHttpAssoc->m_pHttpCurl == NULL )
	{
		printf("Cancel ==> m_pHttpAssoc or m_pHttpAssoc->m_pHttpCurl is NULL! Return!\n");
		return false;
	}

	m_pHttpAssoc->m_pHttpCurl->CancelRequest();

// 2012-07-04 commented by sunfei 去掉重复的clear操作

	// 其他数据清掉
// 	ClearParams();
// 	ClearHeader();
// 	ClearBinaryData();  // 没有地方调用了 AddBinaryData

// commented end

	return true;
}


void CloHttp::SetCallbackOfBuffer( fun_httpbuffer_callback fbuffer_cb )
{
	if ( m_pHttpAssoc != NULL )
	{
		m_pHttpAssoc->m_cb.fbuffer_cb = fbuffer_cb;
	}
	return;
}

void CloHttp::SetCallbackOfProgress( fun_httpprogress_callback fprogress_cb )
{
	if ( m_pHttpAssoc != NULL )
	{
		m_pHttpAssoc->m_cb.fprogress_cb = fprogress_cb;
	}
	return;
}

void CloHttp::SetCallbackOfCode( fun_httpcode_callback fcode_cb )
{
	if ( m_pHttpAssoc != NULL )
	{
		m_pHttpAssoc->m_cb.fcode_cb = fcode_cb;
	}
	return;
}

void CloHttp::SetCallbackOfRespone( fun_httprespone_callback fresp_cb )
{
	if ( m_pHttpAssoc != NULL )
	{
		m_pHttpAssoc->m_cb.frespone_cb = fresp_cb;
	}
	return;
}

void CloHttp::SetCallbackOfRelease( fun_httprelease_callback frelease_cb )
{
	if ( m_pHttpAssoc != NULL )
	{
		m_pHttpAssoc->m_cb.frelease_cb = frelease_cb;
	}
	return;
}

void CloHttp::SetUserData( void* puserdata )
{
	if ( m_pHttpAssoc != NULL )
	{
		m_pHttpAssoc->m_pUserData = puserdata;
	}
	return;
}
	

void CloHttp::SetURL( const HTTPChar* szURL )
{
	if ( m_pHttpAssoc != NULL && szURL != NULL )
	{
		HTTP_TCSCPY_S( m_pHttpAssoc->m_szURL, ConstURLLength, szURL );
	}
	return;
}

void CloHttp::AddParam( const HTTPChar* szName, const HTTPChar* szValue, unsigned long dwParamAttr /* = lohttp::ParamNormal */)
{
	THttpURLParam *param = new THttpURLParam;
	if ( param == NULL )
	{
		printf("AddParam ==> new failed! Return!\n");
		return;
	}
	if ( m_pHttpAssoc == NULL )
	{
		printf("AddParam ==> m_pHttpAssoc is NULL! Return!\n");
		return;
	}

	if( szName != NULL )
	{
		size_t len = HTTP_TCSLEN(szName);
		Http_Char_New(param->pszName,len+1);
		HTTP_TCSCPY_S(param->pszName, len + 1, szName);
		param->pszName[len] = '\0';
	}
	if( szValue != NULL  )
	{
		size_t len = HTTP_TCSLEN(szValue);
		Http_Char_New(param->pszValue,len+1);
		HTTP_TCSCPY_S(param->pszValue, len + 1, szValue);
		param->pszValue[len] = '\0';
	}
	param->dwAttrib = dwParamAttr;
	m_pHttpAssoc->m_lstParam.push_back(param);
	return;
}

void CloHttp::AddHeader( const HTTPChar* szName, const HTTPChar* szValue/* = NULL*/)
{
	THttpHeader *header = new THttpHeader;
	if ( header == NULL )
	{
		printf("AddHeader ==> new failed! Return!\n");
		return;
	}
	if ( m_pHttpAssoc == NULL )
	{
		printf("AddHeader ==> m_pHttpAssoc is NULL! Return!\n");
		return;
	}

	if( szName != NULL )
	{
		size_t len = HTTP_TCSLEN(szName);
		Http_Char_New(header->pszName,len+1);
		HTTP_TCSCPY_S(header->pszName, len + 1, szName);
		header->pszName[len] = '\0';
	}
	if( szValue != NULL  )
	{
		size_t len = HTTP_TCSLEN(szValue);
		Http_Char_New(header->pszValue,len+1);
		HTTP_TCSCPY_S(header->pszValue, len + 1, szValue);
		header->pszValue[len] = '\0';
	}
	m_pHttpAssoc->m_lstHeader.push_back(header);
	return;
}

void CloHttp::SetProxySrvAddr( int proxyType,const HTTPChar* szProxySrv,unsigned short nProxySrvPort )
{
	if ( m_pHttpAssoc == NULL )
	{
		printf("SetProxySrvAddr ==> m_pHttpAssoc is NULL! Return!\n");
		return;
	}

	m_pHttpAssoc->m_nProxyType = proxyType;
	m_pHttpAssoc->m_nProxySrvPort = nProxySrvPort;

	HTTP_TCSCPY_S( m_pHttpAssoc->m_szProxySrvAddr, HTTP_TCSLEN(szProxySrv) + 1,szProxySrv );
	return;
}

void CloHttp::SetProxyAccount( const HTTPChar* szUserName /* = NULL */,
							   const HTTPChar* szPassword /* = NULL */ )
{
	if ( m_pHttpAssoc == NULL )
	{
		printf("SetProxyAccount ==> m_pHttpAssoc is NULL! Return!\n");
		return;
	}

	HTTP_TCSCPY_S( m_pHttpAssoc->m_szProxyUserName, HTTP_TCSLEN(szUserName) + 1, szUserName );
	HTTP_TCSCPY_S( m_pHttpAssoc->m_szProxyUserPwd, HTTP_TCSLEN(szPassword) + 1, szPassword );
	return;
}

void CloHttp::ClearProxy()
{
	if ( m_pHttpAssoc != NULL )
	{
		m_pHttpAssoc->ClearProxy();
	}
	return;
}

void CloHttp::EnableFormData( bool bForm )
{
	if ( m_pHttpAssoc != NULL )
	{
		m_pHttpAssoc->m_bFormData = bForm;
	}
	return;
}

void CloHttp::SetHttpForm( const struct THttpForm* pform )
{
	if ( m_pHttpAssoc != NULL && pform != NULL )
	{
		if ( m_pHttpAssoc->m_Httpform.ffromdata_free_cb != NULL && m_pHttpAssoc->m_Httpform.pformdata != NULL )
		{
			m_pHttpAssoc->m_Httpform.ffromdata_free_cb( m_pHttpAssoc->m_Httpform.pformdata );
			m_pHttpAssoc->m_Httpform.pformdata = NULL;
		}
		memcpy( &m_pHttpAssoc->m_Httpform, pform, sizeof( struct THttpForm ) );
	}
	return;
}

//void CloHttp::SetUploadParam(const HTTPChar* lpszParamName, const HTTPChar* lpszFile)
//{
//	if(lpszParamName)
//	{
//		_TCSCPY_S(m_pHttpAssoc->m_szUploadParam, ConstURLLength, lpszParamName);
//	}
//
//	if( lpszFile )
//	{
//		_TCSCPY_S(m_pHttpAssoc->m_szUploadFile, MAX_PATH+1, lpszFile);
//	}
//}

void CloHttp::SetUserAgent( const HTTPChar* szAgent )
{
	if ( m_pHttpAssoc != NULL && m_pHttpAssoc->m_pHttpCurl != NULL && szAgent != NULL )
	{
		m_pHttpAssoc->m_pHttpCurl->SetUserAgent( HTTP_CT2A(szAgent) );
	}
	return;
}

// 记录开始时间
#define START_HTTP_TIME() \
	THttpTime dTime;\
	time_t now;\
	HTTP_TCSCPY_S( dTime.szURL, ConstURLLength, m_pHttpAssoc->m_szURL );\
	time(&now);\
	dTime.tRequest = now;\

// 记录完成时间
#define END_HTTP_TIME() \
	time(&now);\
	dTime.tResponse = now;\
	m_pHttpAssoc->OnTaskTime(&dTime);


int	CloHttp::HttpMethodDownload()
{
	TTaskParam param;

	if ( m_pHttpAssoc == NULL || m_pHttpAssoc->m_pHttpCurl == NULL )
	{
		printf("HttpMethodDownload ==> m_pHttpAssoc or m_pHttpAssoc->m_pHttpCurl is NULL! Return!\n");
		return -1;
	}

	strcpy_s( param.cURL, ConstURLLength, m_pHttpAssoc->MakeGetURL().c_str() );
	strcpy_s( param.cHead, ConstHeadLength,  m_pHttpAssoc->GetHttpHeader().c_str() );

	if ( *(m_pHttpAssoc->m_szPath) == '\0' || *(m_pHttpAssoc->m_szFile) == '\0' )
	{
		param.bToBuffer = TRUE;
	}
	else
	{
		param.bToBuffer = FALSE;
		strcpy_s( param.cSavePath,   ConstLocalFileLength, HTTP_CT2A(m_pHttpAssoc->m_szPath) );
		strcpy_s( param.cNewFileName,ConstLocalFileLength, HTTP_CT2A(m_pHttpAssoc->m_szFile) );
		param.bRename = TRUE;
	}
	m_pHttpAssoc->m_pHttpCurl->SetTransferParam( E_HTTPTYPE_DOWNLOAD, (LPVOID)&param );

	SetHttpProxyCfg();

	START_HTTP_TIME();

	if ( m_pHttpAssoc->m_pHttpCurl->Request() != 0 )
	{
		printf("HttpMethodDownload ==> m_pHttpAssoc->m_pHttpCurl->Request() failed!\n");
		return -1;	
	}

	END_HTTP_TIME();

	return 0;
}

int CloHttp::HttpMethodGet()
{
	TTaskParam param;

	if ( m_pHttpAssoc == NULL || m_pHttpAssoc->m_pHttpCurl == NULL )
	{
		printf("HttpMethodGet ==> m_pHttpAssoc or m_pHttpAssoc->m_pHttpCurl is NULL! Return!\n");
		return -1;
	}

	strcpy_s( param.cURL, ConstURLLength, m_pHttpAssoc->MakeGetURL().c_str() );
	strcpy_s( param.cHead, ConstHeadLength, m_pHttpAssoc->GetHttpHeader().c_str() );

	m_pHttpAssoc->m_pHttpCurl->SetTransferParam( E_HTTPTYPE_GET, (LPVOID)&param );

	SetHttpProxyCfg();

	START_HTTP_TIME();

	if ( m_pHttpAssoc->m_pHttpCurl->Request() != 0 )
	{
		printf("HttpMethodGet ==> m_pHttpAssoc->m_pHttpCurl->Request() failed!\n");
		return -1;	
	}

	END_HTTP_TIME();

	return 0;
}

//int CloHttp::HttpMethodPostBinaryText()
//{
//	return HttpMethod(E_HTTPTYPE_POST_FORMDATA);
//}

//	TTaskParam param;
//	param.bFormData = m_pHttpAssoc->m_bFormData;
//
//	strcpy_s(param.cURL , ConstURLLength , HTTP_CT2A(m_pHttpAssoc->m_szURL) );
//
//	// head
//	strcpy_s(param.cHead , ConstHeadLength , m_pHttpAssoc->GetHttpHeader().c_str() );
//
//	// 二进
//	if( m_pHttpAssoc->m_binData.Length() > 0 )
//	{
//		param.uDataLength = m_pHttpAssoc->m_binData.Length();	
//		param.pData = new char[ m_pHttpAssoc->m_binData.Length() ];
//		memcpy(param.pData , m_pHttpAssoc->m_binData.Data() , m_pHttpAssoc->m_binData.Length() );
//	}
//
//
//	strcpy_s(param.cLocalFile   , ConstLocalFileLength , HTTP_CT2A(m_pHttpAssoc->m_szFile) );
//
//	// param 
//	std::string strParam = m_pHttpAssoc->MakeGetParam();
//	strcpy_s(param.cFormContents ,ConstFormContentLength , strParam.c_str() );
//
//	m_pHttpAssoc->m_pHttpCurl->SetTransferParam(E_HTTPTYPE_POST_TEXT_FILE, (LPVOID)&param);
//
//	// 设置代理信息
//	SetHttpProxyCfg();
//
//	START_HTTP_TIME();
//
//	// 开始下载
//	if( m_pHttpAssoc->m_pHttpCurl->Request() != 0 )
//		return -1;
//
//	END_HTTP_TIME();
//
//	return 0;
//}

//int	CloHttp::HttpMethodPostBinary()
//{//
//	return HttpMethod( E_HTTPTYPE_POST );
//}

int CloHttp::HttpMethodPost()
{
	return HttpMethod( E_HTTPTYPE_POST );
}


int CloHttp::HttpMethodUpload()
{
	return HttpMethod(E_HTTPTYPE_POST_FORMDATA);
}

int	CloHttp::HttpMethodPut()
{
	return HttpMethod(E_HTTPTYPE_PUT);
}

int	CloHttp::HttpMethodDelete()
{
	return HttpMethod(E_HTTPTYPE_DELETE);
}

int CloHttp::HttpMethod(int method)
{
	TTaskParam param;

	if ( m_pHttpAssoc == NULL || m_pHttpAssoc->m_pHttpCurl == NULL )
	{
		printf("HttpMethod ==> m_pHttpAssoc or m_pHttpAssoc->m_pHttpCurl is NULL! Return!\n");
		return -1;
	}

	param.bFormData = m_pHttpAssoc->m_bFormData;

	strcpy_s( param.cURL, ConstURLLength, HTTP_CT2A(m_pHttpAssoc->m_szURL) );
	strcpy_s( param.cHead, ConstHeadLength, m_pHttpAssoc->GetHttpHeader().c_str() );

	if ( m_pHttpAssoc->m_binData.Length() > 0 )
	{
		param.uDataLength = m_pHttpAssoc->m_binData.Length();	
		param.pData = new char[ m_pHttpAssoc->m_binData.Length() ];
		if ( param.pData != NULL )
		{
			memcpy( param.pData, m_pHttpAssoc->m_binData.Data(), m_pHttpAssoc->m_binData.Length() );
		}
		else
		{
			printf("HttpMethod ==> new failed!\n");
		}
	}
	else
	{
		std::string postParam( m_pHttpAssoc->MakeGetParam() );
		if ( postParam.length() > 0 )
		{
			int dataLength = 0;
			param.uDataLength = dataLength = postParam.length();
			dataLength++;
			param.pData = new char[dataLength];
			if ( param.pData != NULL )
			{
				strcpy_s( param.pData, dataLength, postParam.c_str() );
				param.pData[param.uDataLength] = '\0';
			}
			else
			{
				printf("HttpMethod ==> new failed!\n");
			}
		}
	}

	memcpy( &param.form, &m_pHttpAssoc->m_Httpform, sizeof(THttpForm) );

	m_pHttpAssoc->m_pHttpCurl->SetTransferParam( method, (LPVOID)&param );

	SetHttpProxyCfg();

	START_HTTP_TIME();

	if ( m_pHttpAssoc->m_pHttpCurl->Request() != 0 )
	{
		printf("HttpMethod ==> m_pHttpAssoc->m_pHttpCurl->Request() failed!\n");
		return -1;
	}

	END_HTTP_TIME();

	return 0;
}

void CloHttp::SetHttpProxyCfg()
{
	if ( m_pHttpAssoc == NULL && m_pHttpAssoc->m_pHttpCurl == NULL )
	{
		printf("SetHttpProxyCfg ==> m_pHttpAssoc or m_pHttpAssoc->m_pHttpCurl is NULL!\n");
		return;
	}

	if ( m_pHttpAssoc->m_nProxyType == E_PROXYTYPE_SOCKS5 || m_pHttpAssoc->m_nProxyType == E_PROXYTYPE_HTTP )
	{
		THttpProxyInfo proxyInfo;
		strcpy_s( proxyInfo.cServer, ConstProxyServerLength, HTTP_CT2A(m_pHttpAssoc->m_szProxySrvAddr) );
		proxyInfo.wPort = m_pHttpAssoc->m_nProxySrvPort;

		strcpy_s( proxyInfo.cUser, ConstProxyUserLength, HTTP_CT2A(m_pHttpAssoc->m_szProxyUserName) );
		strcpy_s( proxyInfo.cPassword, ConstProxyPwdLength, HTTP_CT2A(m_pHttpAssoc->m_szProxyUserPwd) );
		proxyInfo.nProxyType = m_pHttpAssoc->m_nProxyType;

		m_pHttpAssoc->m_pHttpCurl->SetProxyInfo(proxyInfo);
	}
	return;
}

//------------------------------------------------------------------------------------//
// CloHttpResponse

CloHttpResponse::CloHttpResponse(void)
{
	m_pResponseAssoc = new CloHttpResponseAssoc();
}

CloHttpResponse::~CloHttpResponse(void)
{
	if ( m_pResponseAssoc != NULL )
	{
		delete m_pResponseAssoc;
		m_pResponseAssoc = NULL;
	}
}

static 
bool GetHttpMemoryBufferLength(const THttpMemoryBuffer* buffer, unsigned long& dwBodyLen)
{
	if ( buffer->buffer != NULL && buffer->size > 0 )
	{
		dwBodyLen = buffer->size;
		return true;
	}
	return false;
}

static
unsigned long GetHttpMemoryBuffer(const THttpMemoryBuffer* buffer, char * pbyBuff, unsigned long cbBuff, unsigned long& offsize)
{
	if ( buffer->buffer != NULL && buffer->size > 0 )
	{
		if ( offsize >= buffer->size )
			return 0;

		unsigned long copyDataLength = (unsigned long)(buffer->size - offsize);
		if ( cbBuff < copyDataLength )
			copyDataLength = cbBuff;

		memcpy(pbyBuff, buffer->buffer + offsize , copyDataLength);

		offsize += copyDataLength;

		return copyDataLength;
	}
	return 0;
}


bool CloHttpResponse::GetHeaderLength(unsigned long& len) const
{
	return GetHttpMemoryBufferLength( &m_pResponseAssoc->m_httpHeader, len );
}

unsigned long CloHttpResponse::ReadHeader(char * pbyBuff, unsigned long cbBuff, unsigned long& offsize) const
{
	return GetHttpMemoryBuffer( &m_pResponseAssoc->m_httpHeader, pbyBuff, cbBuff, offsize );
}

bool CloHttpResponse::ReadHeaderCallback(fun_http_resp_header_callback cb , void* pUsrData) const
{
	if ( cb == NULL )
	{
		printf("ReadHeaderCallback ==>   cb is NULL, Error!\n");
		return false;
	}

	cb( GetHttpCode(), m_pResponseAssoc->m_httpHeader.buffer, m_pResponseAssoc->m_httpHeader.size, pUsrData );

	return true;
}

bool CloHttpResponse::GetBodyLength(unsigned long & dwBodyLen) const
{
	return GetHttpMemoryBufferLength( &m_pResponseAssoc->m_httpBody, dwBodyLen );
}

unsigned long CloHttpResponse::ReadBody(char * pbyBuff, unsigned long cbBuff, unsigned long& offsize) const
{
	return GetHttpMemoryBuffer( &m_pResponseAssoc->m_httpBody, pbyBuff, cbBuff, offsize );
}

bool CloHttpResponse::ReadBodyCallback(fun_http_resp_body_callback cb, void* pUsrData) const
{
	if ( cb == NULL )
	{
		printf("ReadBodyCallback ==>   cb is NULL, Error!\n");
		return false;
	}

	cb( GetHttpCode(), m_pResponseAssoc->m_httpBody.buffer, m_pResponseAssoc->m_httpBody.size, pUsrData );

	return true;
}

long CloHttpResponse::GetHttpCode(void) const
{
	return m_pResponseAssoc->m_lhttpcode;
}

///////////////////////////////////////////////////////////////////////////////////////////////
/** http header 信息 */

CloHttpHeader::CloHttpHeader()
{
	m_pHeaderAssoc = new CHttpMap();
	m_httpState[0] = '\0';
}

CloHttpHeader::~CloHttpHeader()
{
	if ( m_pHeaderAssoc != NULL )
	{
		delete ( (CHttpMap*)m_pHeaderAssoc );
		m_pHeaderAssoc = NULL;
	}	
}

typedef struct tagHttpHeaderValue
{
	char* szVal;
	int   len;

	tagHttpHeaderValue()
	{
		szVal = NULL;
		len = 0;
	}
}THttpHeaderValue;

static
void gfun_str_trim(char* &sz1,char* &szEnd)
{
	// 除掉后面的空格符号
	while( (*szEnd) == 0x20 && szEnd >= sz1 ){
		szEnd--;
	}
	// 除掉前面的空格符号
	while( (*sz1) == 0x20 && sz1 <= szEnd ){
		sz1++;
	}
}

static
bool gfun_Header_line_parse(char* szLine, 
							char* szEnd ,
							THttpHeaderValue &Key , THttpHeaderValue& Value)
{
	char* szTmpEnd = szEnd;
	char* sz1 = strchr(szLine , ':');
	if( sz1 )
	{
		// 需要提出 key 非空格字符
		char* szkey = szLine;
		while( (*szkey) == 0x20 && szkey < sz1 ){
			szkey++;
		}
		Key.szVal = szkey;
		Key.len   = (int)(sz1 - szkey);
		(*sz1) = '\0';

		sz1++;
		if( sz1 < szEnd )
		{
			gfun_str_trim(sz1,szTmpEnd);
			( *szTmpEnd ) = '\0';
			// value
			Value.szVal = sz1;
			Value.len   = (int)(szTmpEnd - sz1);
		}
		return true;
	}

	// 不对
	return false;	
}


void CloHttpHeader::Parse( const char* c_httpHeader_buffer, unsigned long httpHeader_size )
{
	CHttpMap* pHeadMap = ( (CHttpMap*)m_pHeaderAssoc );
	pHeadMap->Clear();
	m_httpState[0] = '\0';

	if( !c_httpHeader_buffer || httpHeader_size <= 0 )
		return ;
	/*
	解释头如下格式,它们之间"\r\n" 分行
	Exmaple:

	HTTP/1.1 200 OK
	Server: Apache-Coyote/1.1
	Content-Type: text/html;charset=GBK
	Content-Length: 406
	Date: Tue, 28 Jul 2009 07:05:31 GMT

	*/
	BOOL bHttpState = true;

	char* httpHeader_buffer = new char[httpHeader_size+1];
	if ( httpHeader_buffer != NULL )
	{
		memcpy( httpHeader_buffer, c_httpHeader_buffer, httpHeader_size );
		httpHeader_buffer[ httpHeader_size ] = '\0';
	}

	char* szBegin = httpHeader_buffer;
	char* szEnd   = httpHeader_buffer + httpHeader_size;
	char* szLine  = szBegin;
	while( szBegin < szEnd )
	{
		if( (*szBegin) == 0x0d && *(szBegin+1) == 0x0a )
		{
			if( bHttpState )
			{
				// 分析第一个，是状态
				char* szxx = szLine;
				char* szyy = szBegin;
				gfun_str_trim(szxx,szyy);
				*(szyy) = '\0';
				const HTTPChar *st = HTTP_CA2T( szxx );
				HTTP_TCSCPY_S( m_httpState,HTTP_TCSLEN(st) + 1, HTTP_CA2T( st ) );
				bHttpState = FALSE;				
			}
			else
			{
				THttpHeaderValue Key,Value;
				if( gfun_Header_line_parse(szLine , szBegin , Key , Value) ){
					// 加入
					pHeadMap->Add( HTTP_CA2T(Key.szVal), HTTP_CA2T(Value.szVal) );
				}
			}
			// 下一行
			szBegin++;
			szBegin++;
			szLine = szBegin;
		}
		else
			szBegin++;
	}
	delete[] httpHeader_buffer;
}

bool CloHttpHeader::GetContentLength (unsigned long & cbContLen) const
{
	const HTTPChar* szValue = GetHeader( HTTP_T("Content-Length"),0);
	if( szValue )
	{
		cbContLen = (unsigned long)( HTTP_TTOL(szValue) );
		return true;
	}
	return false;
}

int CloHttpHeader::GetHeadercount(const HTTPChar* szName) const
{
	CHttpMap* pHeadMap = ( (CHttpMap*)m_pHeaderAssoc );
	return pHeadMap->Count(szName);
}

const HTTPChar* CloHttpHeader::GetHeader(const HTTPChar* szName, unsigned long nIdx/* = 0*/) const
{
	CHttpMap* pHeadMap = ( (CHttpMap*)m_pHeaderAssoc );
	try
	{
		if( !pHeadMap->Exists(szName, nIdx) )
			return NULL;

		return pHeadMap->Get(szName,nIdx).szValue;
	}
	catch(...)
	{
		return NULL;
	}
	return NULL;
}

/** 读取类似于 " 200 OK 这样的信息 "*/
const HTTPChar*	CloHttpHeader::GetHeaderState() const
{
	return m_httpState;
}
