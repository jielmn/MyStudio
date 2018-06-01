#include <assert.h>
#include <string.h>

#include "loHttpCurl.h"
#include "loFile.h"
#include "curl/curl.h"
#include "lo/lolock.h"

#include "spthread.h"
#include "libhttp/loHttp.h"
#include "httpstack.h"

DEFINE_NAMESPACE(lohttp)

#define USE_CLS_CURL_INIT
#ifdef  USE_CLS_CURL_INIT

	class CurlInit
	{
	public:
		CurlInit()
		{
			// curl_global_init(CURL_GLOBAL_ALL);		
            InitHttpStack( CloHttp::OnHttpStackCb );
		}
		~CurlInit()
		{
			// curl_global_cleanup();
            DeinitHttpStack();
		}
		static CurlInit& Instance()
		{
			static CurlInit inst;
			return inst;
		}
		void Init()	{}
		void UnInit() {}
	};

	#define CURL_INIT()  CurlInit::Instance().Init()
	#define CURL_UNINIT()  CurlInit::Instance().UnInit()

#else

	#define CURL_INIT()  curl_global_init(CURL_GLOBAL_ALL)
	#define CURL_UNINIT()  curl_global_cleanup()

#endif


void* BufferRealloc(void *ptr, size_t size)
{
	if ( ptr != NULL )
	{
		return realloc(ptr, size);
	}
	else
	{
		return malloc(size);
	}
}

static long iTaskId = 1;

static
size_t WriteBuffer( void *ptr, size_t size, size_t nmemb, void *data )
{
	size_t realsize = size * nmemb;
	THttpMemoryBuffer *mem = (THttpMemoryBuffer *)data;

	mem->buffer = (char *)BufferRealloc( mem->buffer, mem->size + realsize + 1 );
	if ( mem->buffer != NULL )
	{
		memcpy( &(mem->buffer[mem->size]), ptr, realsize );
		mem->size += realsize;
		mem->buffer[mem->size] = 0;
		return realsize;
	}
	else
	{
		printf("WriteBuffer ==>  BufferRealloc failed!\n");
		mem->size = 0;
		return 0;
	}
}

static 
std::string GetUrlFileName(std::string url)
{
	std::string::size_type pos = url.find_last_of("/");
	if ( pos == std::string::npos )
	{
		return std::string("");
	}
	pos ++;

	std::string ret(url.substr(pos, url.length() - pos));

	return ret;
}

/** 创建自定义头 */
static 
void BuildCustomHeader(struct curl_slist**chunk, char * pHeaderData)
{

}

// convert to enHTTPCode
static 
int CURLcode2HttpCode(CURLcode code)
{
	return code;
}

class CloHttpCurlAssoc
{
public:
	CloHttpCurlAssoc(IloTaskCallBack* pTaskCallback)
		:m_pHttpTask(pTaskCallback)
	{
		m_curlHandle = NULL;
		m_bUseProxy = FALSE;
		m_nType = -1;
		m_pTransferParam = NULL;

		m_lhttpCode = 0;
		m_errCode   = 0;
		m_nState      = E_HTTPSTATUS_CLOSED;
		m_bUserCancel = FALSE;

		strcpy(m_useAgent,"HTTP-Motrix/1.0");

		m_lTaskID = iTaskId++;

		CURL_INIT();
	}

	~CloHttpCurlAssoc()
	{
		CURL_UNINIT();
	}

	void SetErrCode(int code)
	{
		m_errCode = code;
	}

private:
	long m_lTaskID;

	THttpMemoryBuffer m_HeaderRecv;
	THttpMemoryBuffer m_BodyRecv;

	THttpFileBuffer	m_FileBufferRecv;

	BOOL m_bUseProxy;
	THttpProxyInfo m_ProxyInfo;

	int	m_nType;                    // EHttpTransferType
	TTaskParam*	m_pTransferParam;

	int	 m_nState;
	int  m_errCode;
	long m_lhttpCode;

	char m_useAgent[256];

	long m_lCurBytes;
	long m_lTotalBytes;

	BOOL m_bUserCancel;

	IloTaskCallBack* m_pHttpTask;

	CURL* m_curlHandle;

	locom::CloSmartLock<THREAD_CRITICAL_SECTION, locom::CloCriticalSection> m_cs;

	friend class CloHttpCurl;
};


static 
bool lohttp_formadd_callback(void** httpost , void** last , HTTPFormType type , void* value )
{
	struct curl_httppost **post_ = (struct curl_httppost **)httpost;
	struct curl_httppost **last_ = (struct curl_httppost **)last; 

	int formid = -1;
	switch(type)
	{
	case HTTP_FORMTYPE_COPYNAME:
		formid = CURLFORM_COPYNAME;
		break;
	case HTTP_FORMTYPE_PTRNAME:
		formid = CURLFORM_PTRNAME;
		break;
	case HTTP_FORMTYPE_NAMELENGTH:
		formid = CURLFORM_NAMELENGTH;
		break;
	case HTTP_FORMTYPE_COPYCONTENTS:
		formid = CURLFORM_COPYCONTENTS;
		break;
	case HTTP_FORMTYPE_PTRCONTENTS:
		formid = CURLFORM_PTRCONTENTS;
		break;
	case HTTP_FORMTYPE_CONTENTSLENGTH:
		formid = CURLFORM_CONTENTSLENGTH;
		break;
	case HTTP_FORMTYPE_FILECONTENT:
		formid = CURLFORM_FILECONTENT;
		break;
	case HTTP_FORMTYPE_ARRAY:
		formid = CURLFORM_ARRAY;
		break;
	case HTTP_FORMTYPE_OBSOLETE:
		formid = CURLFORM_OBSOLETE;
		break;
	case HTTP_FORMTYPE_FILE:
		formid = CURLFORM_FILE;
		break;
	case HTTP_FORMTYPE_BUFFER:
		formid = CURLFORM_BUFFER;
		break;
	case HTTP_FORMTYPE_BUFFERPTR:
		formid = CURLFORM_BUFFERPTR;
		break;
	case HTTP_FORMTYPE_BUFFERLENGTH:
		formid = CURLFORM_BUFFERLENGTH;
		break;
	case HTTP_FORMTYPE_CONTENTTYPE:
		formid = CURLFORM_CONTENTTYPE;
		break;
	case HTTP_FORMTYPE_CONTENTHEADER:
		formid = CURLFORM_CONTENTHEADER;
		break;
	case HTTP_FORMTYPE_FILENAME:
		formid = CURLFORM_FILENAME;
		break;
	case HTTP_FORMTYPE_OBSOLETE2:
		formid = CURLFORM_OBSOLETE2;
		break;
	case HTTP_FORMTYPE_STREAM:
		formid = CURLFORM_STREAM;
		break;
	}

	if ( formid <= CURLFORM_NOTHING || formid >= CURLFORM_LASTENTRY || formid == CURLFORM_END )
		return false;

	// return ( CURL_FORMADD_OK == curl_formadd(post_, last_ , formid , value , CURLFORM_END) )?true:false;
    return true;
}


//----------------------------------------------------------------------------------------//
CloHttpCurl::CloHttpCurl(IloTaskCallBack* pHttpTask)
	:m_pCurlAssoc( new CloHttpCurlAssoc(pHttpTask) )
{
    m_pContext = 0;
}

CloHttpCurl::~CloHttpCurl(void)
{
	StopTransfer();
	ClearBuffer();
	ClearParam();

	delete m_pCurlAssoc;
	m_pCurlAssoc = NULL;
}

void CloHttpCurl::ClearBuffer()
{
	SAFE_DELETE_MEMORYBUFFER( &m_pCurlAssoc->m_HeaderRecv );
	memset( &m_pCurlAssoc->m_HeaderRecv, 0, sizeof(THttpMemoryBuffer) );

	SAFE_DELETE_MEMORYBUFFER( &m_pCurlAssoc->m_BodyRecv );
	memset( &m_pCurlAssoc->m_BodyRecv, 0, sizeof(THttpMemoryBuffer) );
}

void CloHttpCurl::ClearParam()
{
	if ( m_pCurlAssoc->m_pTransferParam != NULL )
	{
		delete m_pCurlAssoc->m_pTransferParam;
		m_pCurlAssoc->m_pTransferParam  = NULL;
	}
}

/*
return value:
0: succeed;
-1: transfer already started
-2: task invalid
-3: start failed
*/
int CloHttpCurl::StartTransfer()
{
	if ( m_pCurlAssoc->m_curlHandle != NULL )
	{
		printf("StartTransfer ==>   m_curlHandle is not NULL! Error!\n");
		return -1;
	}

	if ( FALSE == IsTaskValid() )
	{
		printf("StartTransfer ==>   Task is not Valid! Error!\n");
		return -2;
	}

	return TransferProc();
}

void CloHttpCurl::StopTransfer()
{
	locom::CloAutoLock<THREAD_CRITICAL_SECTION, locom::CloCriticalSection> al(&m_pCurlAssoc->m_cs);

    if ( E_HTTPSTATUS_START == m_pCurlAssoc->m_nState )
	{
		int count = 0;
		m_pCurlAssoc->m_bUserCancel = TRUE;
		while ( m_pCurlAssoc->m_curlHandle != NULL )
		{
			sp_millisleep(200);
			count++;

			if ( count >= 15 )
			{
				break;
			}
		}
		SetState( E_HTTPSTATUS_CANCEL );
	}
// 	else
// 	{
// 		printf("StopTransfer ==>   m_pCurlAssoc->m_nState is not E_HTTPSTATUS_START! No need to stop!\n");
// 	}
	return;
}

int CloHttpCurl::Request()
{
	return StartTransfer();
}

void CloHttpCurl::CancelRequest()
{
	return StopTransfer();
}

void CloHttpCurl::SetProxyInfo(THttpProxyInfo &ProxyInfo)
{
	memset(&m_pCurlAssoc->m_ProxyInfo, 0, sizeof(ProxyInfo));
	memcpy(&m_pCurlAssoc->m_ProxyInfo, &ProxyInfo, sizeof(ProxyInfo));
}

/*
return value:
0:	Succeed;
-1: Task invalid;
-2: Task already set;
*/
int CloHttpCurl::SetTransferParam(int nType, LPVOID lpParam)
{
	m_pCurlAssoc->m_nType = nType;

	ClearParam();

	m_pCurlAssoc->m_pTransferParam = new TTaskParam();
	TTaskParam * pParam = (TTaskParam *)lpParam;
	*(m_pCurlAssoc->m_pTransferParam) = *pParam;

	if ( FALSE == IsTaskValid() )
	{
		printf("SetTransferParam ==>   Task is not Valid! Error!\n");
		return -1;
	}

	return 0;
}

BOOL CloHttpCurl::IsTaskValid()
{
	if ( m_pCurlAssoc->m_pTransferParam == NULL || m_pCurlAssoc->m_pTransferParam->cURL[0] == '\0' )
	{
		printf("IsTaskValid ==>   Param Error!\n");
		return FALSE;
	}

	return TRUE;
}

/*
return value:
0: succeed;
-1: failed;
-2: invalid task
*/
int CloHttpCurl::TransferProc()
{
	m_pCurlAssoc->m_bUserCancel = FALSE;
	m_pCurlAssoc->m_errCode = 0;

#define CALL_PROC(id , fun ) case id: return fun()
	switch(m_pCurlAssoc->m_nType)
	{
		CALL_PROC( E_HTTPTYPE_GET             , TransferProcGET );
		CALL_PROC( E_HTTPTYPE_DOWNLOAD        , TransferProcDOWNLOAD );
		CALL_PROC( E_HTTPTYPE_POST            , TransferProcPOST );
		CALL_PROC( E_HTTPTYPE_POST_FORMDATA   , TransferProcPOSTFormData );
// 		CALL_PROC( E_HTTPTYPE_POST_TEXT_FILE  , TransferProcPOSTFormData2 );
// 		CALL_PROC( E_HTTPTYPE_UPLOAD          , TransferProcUPLOAD );
		CALL_PROC( E_HTTPTYPE_PUT             , TransferProcPUT );
		CALL_PROC( E_HTTPTYPE_DELETE          , TransferProcDELETE );
	}
	return -2;
}

// 
#define GET_HTTP_CODE() \
	    m_pCurlAssoc->m_lhttpCode = 0;\
		curl_easy_getinfo(m_pCurlAssoc->m_curlHandle, CURLINFO_HTTP_CODE, &m_pCurlAssoc->m_lhttpCode)

int CloHttpCurl::TransferProcGET()
{
    SetState( E_HTTPSTATUS_START );

    CHttp * pHttp = CHttp::GetInstance();

    CHttpRequest request( m_pCurlAssoc->m_pTransferParam->cURL );
    request["User-Agent"] = "Movision SDK/1.0";
    request["Accept"] = "*/*";
    request["Content-Type"] = "application/x-www-form-urlencoded";


    if ( m_pCurlAssoc->m_pTransferParam->cHead[0] != '\0' )
    {
        request.SetOptions( m_pCurlAssoc->m_pTransferParam->cHead );
    }


    CGlobalDataWeibo * pGlobal = CGlobalDataWeibo::GetInstance();
    TWeiboProxyParam tProxyParam;
    pGlobal->GetCommonProxyParam( &tProxyParam );

    // 暂时只支持sock5代理
    if ( PROXYTYPE_SOCK5 == tProxyParam.nProxyType )
    {
        request.SetProxy( tProxyParam.szProxy, tProxyParam.wProxyPort, CHttpRequest::PROXY_TYPE_SOCK5 );
        request.SetProxyUser( tProxyParam.szUsername );
        request.SetProxyPwd( tProxyParam.szPassword );
    }


    pHttp->AddRequest( request, m_pContext );

    SetState( E_HTTPSTATUS_STOP );
    return 0;
}

int CloHttpCurl::TransferProcDOWNLOAD()
{
    SetState( E_HTTPSTATUS_START );
    SetState( E_HTTPSTATUS_STOP );
    return 0;
}

int CloHttpCurl::TransferProcDOWNLOAD_TOBUFFER()
{
    SetState( E_HTTPSTATUS_START );
    SetState( E_HTTPSTATUS_STOP );
    return 0;
}

int CloHttpCurl::TransferProcPOST()
{

    SetState( E_HTTPSTATUS_START );

    CHttp * pHttp = CHttp::GetInstance();

    CHttpRequest request( m_pCurlAssoc->m_pTransferParam->cURL, CHttpRequest::HTTP_METHOD_POST, m_pCurlAssoc->m_pTransferParam->pData );
    request["User-Agent"] = "Movision SDK/1.0";
    request["Accept"] = "*/*";
    request["Content-Type"] = "application/x-www-form-urlencoded";

    if ( m_pCurlAssoc->m_pTransferParam->cHead[0] != '\0' )
    {
        request.SetOptions( m_pCurlAssoc->m_pTransferParam->cHead );
    }


    CGlobalDataWeibo * pGlobal = CGlobalDataWeibo::GetInstance();
    TWeiboProxyParam tProxyParam;
    pGlobal->GetCommonProxyParam( &tProxyParam );

    // 暂时只支持sock5代理
    if ( PROXYTYPE_SOCK5 == tProxyParam.nProxyType )
    {
        request.SetProxy( tProxyParam.szProxy, tProxyParam.wProxyPort, CHttpRequest::PROXY_TYPE_SOCK5 );
        request.SetProxyUser( tProxyParam.szUsername );
        request.SetProxyPwd( tProxyParam.szPassword );
    }


    pHttp->AddRequest( request, m_pContext );

    SetState( E_HTTPSTATUS_STOP );
    return 0;
}

int CloHttpCurl::TransferProcPOSTFormData()
{
    SetState( E_HTTPSTATUS_START );

    CHttp * pHttp = CHttp::GetInstance();
    CHttpRequest request( m_pCurlAssoc->m_pTransferParam->cURL, CHttpRequest::HTTP_METHOD_POST, "", CHttpRequest::HTTP_CONTENT_TYPE_FORM );

    request["User-Agent"] = "Movision SDK/1.0";
    request["Accept"] = "*/*";
    request["Content-Type"] = "application/x-www-form-urlencoded";

    // 附加的
    if ( m_pCurlAssoc->m_pTransferParam->cHead[0] != '\0' )
    {
        request.SetOptions( m_pCurlAssoc->m_pTransferParam->cHead );
    }



    // 设置format
    struct t_wb_upload_form * pupload = (struct t_wb_upload_form *)m_pCurlAssoc->m_pTransferParam->form.pformdata;
    assert( pupload );


    const unsigned int  max_size = 1024 * 32;
    char buf[max_size];
    char * p      = pupload->postcontent;
    char * pAnd   = strchr( p, '&' );

    while( pAnd || 0 != p )
    {
        int len = 0;

        if ( pAnd )
        {
            len  = pAnd - p;
        }
        else
        {
            len = strlen( p );
        }

        int nMin = len < (max_size-1) ? len : (max_size-1);
        memcpy( buf, p, nMin );
        buf[nMin] = '\0';


        // 添加一个form data
        request.AppendFormatData( buf );
        // end


        if ( pAnd )
        {
            p    = pAnd + 1;
            pAnd = strchr( p, '&' );
        }
        else
        {
            p = 0;
        }
    }



    // 添加upload form
    for ( int nIndex = 0; nIndex < pupload->fileCount && nIndex < 10; nIndex++ )
    {
        request.AppendFormatData( "file", pupload->file[nIndex], CHttpFormData::FORMAT_DATA_TYPE_UPLOAD );
    }

    // end 设置format


    CGlobalDataWeibo * pGlobal = CGlobalDataWeibo::GetInstance();
    TWeiboProxyParam tProxyParam;
    pGlobal->GetCommonProxyParam( &tProxyParam );

    // 暂时只支持sock5代理
    if ( PROXYTYPE_SOCK5 == tProxyParam.nProxyType )
    {
        request.SetProxy( tProxyParam.szProxy, tProxyParam.wProxyPort, CHttpRequest::PROXY_TYPE_SOCK5 );
        request.SetProxyUser( tProxyParam.szUsername );
        request.SetProxyPwd( tProxyParam.szPassword );
    }


    pHttp->AddRequest( request, m_pContext );

    SetState( E_HTTPSTATUS_STOP );
    return 0;
}

int CloHttpCurl::TransferProcDELETE()
{
    SetState( E_HTTPSTATUS_START );
    SetState( E_HTTPSTATUS_STOP );
    return 0;
}

int CloHttpCurl::TransferProcPUT()
{
    SetState( E_HTTPSTATUS_START );
    SetState( E_HTTPSTATUS_STOP );
    return 0;
}

void CloHttpCurl::SetTransferCallback( LPVOID headerData, LPVOID bodyData )
{

}


void CloHttpCurl::SetState( int nNewState )
{
	m_pCurlAssoc->m_nState = nNewState;
	TaskState( m_pCurlAssoc->m_nState );
}


void CloHttpCurl::TaskState( int nState )
{
	if ( m_pCurlAssoc->m_pHttpTask != NULL )
	{
		m_pCurlAssoc->m_pHttpTask->OnTaskState( nState );
	}
}

int CloHttpCurl::TaskProgress( long lCurBytes, long lTotalBytes )
{
	m_pCurlAssoc->m_lCurBytes = lCurBytes;
	m_pCurlAssoc->m_lTotalBytes = lTotalBytes;

	if ( m_pCurlAssoc->m_pHttpTask != NULL )
	{
		return m_pCurlAssoc->m_pHttpTask->OnTaskProgress( lCurBytes, lTotalBytes );
	}
	return 0;
}

int CloHttpCurl::TaskBuffer( unsigned char* pBuffer, long lLength )
{
	if ( m_pCurlAssoc->m_pHttpTask != NULL )
	{
		return m_pCurlAssoc->m_pHttpTask->OnTaskBuffer( pBuffer, lLength );
	}
	return 0;
}

void CloHttpCurl::TaskCode(void)
{
	if ( m_pCurlAssoc->m_bUserCancel == FALSE && m_pCurlAssoc->m_pHttpTask != NULL )
	{
		m_pCurlAssoc->m_pHttpTask->OnTaskCode( m_pCurlAssoc->m_errCode , 0);
	}
}

int	CloHttpCurl::GetResponedHeader(THttpMemoryBuffer * pHeaderBuf)
{
	if ( m_pCurlAssoc->m_nState != E_HTTPSTATUS_STOP )
	{
		printf("GetResponedHeader ==>   m_pCurlAssoc->m_nState is not E_HTTPSTATUS_STOP, Error!\n");
		return -1;
	}
	SAFE_NEW_MEMORYBUFFER(pHeaderBuf,m_pCurlAssoc->m_HeaderRecv.size+1,m_pCurlAssoc->m_HeaderRecv.size);
	COPY_MEMORYBUFFER(&m_pCurlAssoc->m_HeaderRecv,pHeaderBuf);

	return 0;
}

int CloHttpCurl::GetResponedBody(THttpMemoryBuffer * pBodyBuf)
{
	if ( m_pCurlAssoc->m_nState != E_HTTPSTATUS_STOP )
	{
		printf("GetResponedBody ==>   m_pCurlAssoc->m_nState is not E_HTTPSTATUS_STOP, Error!\n");
		return -1;
	}
	SAFE_NEW_MEMORYBUFFER(pBodyBuf,(m_pCurlAssoc->m_BodyRecv.size+1),m_pCurlAssoc->m_BodyRecv.size);
	COPY_MEMORYBUFFER(&m_pCurlAssoc->m_BodyRecv,pBodyBuf);

	return 0;
}

void CloHttpCurl::UpdateProxy(char* lpszRequestURL)
{
	
}


size_t CloHttpCurl::ToBodyCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = WriteBuffer(ptr, size, nmemb, data);

	return realsize;
}

size_t CloHttpCurl::ToHeaderCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = WriteBuffer(ptr, size, nmemb, data);

	return realsize;
}

size_t CloHttpCurl::ToFileCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	if ( realsize )
	{
		THttpFileBuffer * fileBuf = (THttpFileBuffer *)data;

		// 将缓冲数据保存到文件中
		if ( fileBuf->fp )
		{
			size_t write = fwrite(ptr, size, nmemb, fileBuf->fp);
		}
	}
	return realsize;
}

size_t CloHttpCurl::ToBufferCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	CloHttpCurl * pClient = (CloHttpCurl *)data;
	size_t realsize = size * nmemb;
	if ( pClient != NULL )
	{
		if ( 0 != pClient->TaskBuffer( (unsigned char*)ptr, (long)realsize ) )
			return CURL_WRITEFUNC_PAUSE;
	}

	return realsize;
}

size_t CloHttpCurl::ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CloHttpCurl * pClient = (CloHttpCurl *)clientp;
	if ( pClient != NULL )
	{
		// 是否用户取消
		if ( pClient->IsUserCancel() )
		{
			return 1;
		}

		int nType = pClient->GetType();
		switch(nType)
		{
		case E_HTTPTYPE_GET:
		case E_HTTPTYPE_DOWNLOAD:
		case E_HTTPTYPE_DELETE:
			return pClient->TaskProgress((long)dlnow, (long)dltotal);
		case E_HTTPTYPE_POST:
		case E_HTTPTYPE_POST_FORMDATA:
		//case E_HTTPTYPE_UPLOAD:
		case E_HTTPTYPE_PUT:
			return pClient->TaskProgress((long)ulnow, (long)ultotal);
		}
	}
	return 0;
}

long CloHttpCurl::GetTaskID() const {return m_pCurlAssoc->m_lTaskID;}

int CloHttpCurl::GetType() const {return m_pCurlAssoc->m_nType;}

int CloHttpCurl::GetState() const {return m_pCurlAssoc->m_nState;}

int CloHttpCurl::GetErrCode() const {return m_pCurlAssoc->m_errCode;}

BOOL CloHttpCurl::IsUserCancel() const { return m_pCurlAssoc->m_bUserCancel;}

long CloHttpCurl::GetHttpCode() const { return m_pCurlAssoc->m_lhttpCode;}

void CloHttpCurl::SetUserAgent(const char* agent)
{
	if ( agent != NULL)
	{
		strncpy(m_pCurlAssoc->m_useAgent, agent, 256);
	}
}

const char* CloHttpCurl::GetUserAgent() const{
	return m_pCurlAssoc->m_useAgent;
}

END_NAMESPACE(lohttp)
