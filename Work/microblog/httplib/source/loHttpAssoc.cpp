#include "setup.h"
#include "loHttpAssoc.h"
#include "loHttpCurl.h"
#include "libhttp/loHttp.h"
#include "loUrlcode.h"
#include "lostrconv.h"

using namespace lohttp;

CloHttpAssoc::CloHttpAssoc()
{
	m_pHttpCurl = new CloHttpCurl(this);

	m_bFormData = false;
	m_nMethod = 0;
	m_hInetTh = 0;
	m_pRespone = NULL;
	m_pUserData = NULL;
	m_ierrCode = 0;

	memset( m_szURL, 0, sizeof(HTTPChar) * ConstURLLength );

	ClearProxy();
	ClearParams();
	ClearHeader();

	//memset(m_szUploadParam, 0, sizeof(HTTPChar)*ConstUploadParamLength );
	//memset(m_szUploadFile, 0, sizeof(HTTPChar)*(MAX_PATH+1) );

	memset( m_szFile, 0, sizeof(HTTPChar) * (MAX_PATH+1) );
	memset( m_szPath, 0, sizeof(HTTPChar) * (MAX_PATH+1) );

	memset( &m_cb, 0, sizeof( struct t_lo_httpCB ) );
	memset( &m_Httpform, 0, sizeof( THttpForm ) );
	memset( &m_time, 0, sizeof( THttpTime ) );

}

CloHttpAssoc::~CloHttpAssoc(void)
{
	ClearProxy();
	ClearParams();
	ClearHeader();

	SAFE_DELETE( m_pRespone );

	if ( m_Httpform.ffromdata_free_cb != NULL && m_Httpform.pformdata != NULL )
	{
		m_Httpform.ffromdata_free_cb( m_Httpform.pformdata );
		m_Httpform.pformdata = NULL;
	}

	if ( m_pUserData != NULL && m_cb.frelease_cb != NULL )
	{
		m_cb.frelease_cb( m_pUserData );
		m_pUserData = NULL;
	}

	SAFE_DELETE( m_pHttpCurl );
}

void CloHttpAssoc::OnTaskState( long lState )
{
	if ( m_cb.fcode_cb != NULL )
	{
		m_cb.fcode_cb( lState, m_ierrCode, &m_time, m_pUserData );
	}
	return;
}

int CloHttpAssoc::OnTaskProgress( long lCurBytes, long lTotalBytes )
{
	if ( m_cb.fprogress_cb != NULL )
	{
		return m_cb.fprogress_cb( lCurBytes, lTotalBytes, m_pUserData );
	}
	return -1;
}

int CloHttpAssoc::OnTaskBuffer( unsigned char* pBuffer, long lBuffer )
{
	if ( m_cb.fbuffer_cb != NULL )
	{
		return m_cb.fbuffer_cb( pBuffer, lBuffer, m_pUserData );
	}
	return -1;
}

void CloHttpAssoc::OnTaskCode( long code, const char* errString )
{
	m_ierrCode = code;
	return;
}

void CloHttpAssoc::OnTaskTime( const THttpTime *pTime )
{
	memcpy( &m_time, pTime, sizeof(THttpTime) );
	return;
}

void CloHttpAssoc::OnStart()
{
	SAFE_DELETE( m_pRespone );
	m_ierrCode = 0;

	return;
}

void CloHttpAssoc::OnFinished( CloHttpResponse* resp )
{
	if ( resp != NULL )
	{
		m_pRespone = resp;

		if ( m_cb.frespone_cb != NULL )
		{
			m_cb.frespone_cb( m_pRespone, m_pUserData ); 
		}
	}
	return;
}

void CloHttpAssoc::ClearParams()
{
	std::list<THttpURLParam *>::iterator itemIter;
	for( itemIter = m_lstParam.begin(); itemIter != m_lstParam.end(); itemIter ++ )
	{
		THttpURLParam *param = *itemIter;
		SAFE_DELETE(param);
	}
	m_lstParam.clear();
}

void CloHttpAssoc::ClearHeader()
{
	std::list<THttpHeader *>::iterator itemIter;
	for( itemIter = m_lstHeader.begin(); itemIter != m_lstHeader.end(); itemIter ++ )
	{
		THttpHeader * header = *itemIter;
		SAFE_DELETE(header);
	}
	m_lstHeader.clear();
}

void CloHttpAssoc::ClearProxy()
{
	m_nProxyType = E_PROXYTYPE_NONE;
	m_nProxySrvPort = 808;

	memset( m_szProxySrvAddr, 0, sizeof(HTTPChar) * ConstProxyServerLength );
	memset( m_szProxyUserName, 0, sizeof(HTTPChar) * ConstProxyUserLength );
	memset( m_szProxyUserPwd, 0, sizeof(HTTPChar) * ConstProxyPwdLength );

	return;
}


std::string CloHttpAssoc::MakeGetURL()
{		
	std::string strURL  = HTTP_CT2A( m_szURL );
	std::string strParam = MakeGetParam();

	if ( strParam.length() > 0 )
	{
		const HTTPChar* z = HTTP_TCSCHR( m_szURL, HTTP_T('?') );
		if ( z == NULL )
		{
			strURL += "?";
		}
		else
		{
			if ( NULL == HTTP_TCSCHR( z, HTTP_T('&') ) )
			{
				strURL += "&";
			}
		}
		strURL += strParam;
	}
	return strURL;
}

std::string CloHttpAssoc::GetHttpHeader()
{
	std::string strRet;

	std::list<THttpHeader *>::iterator itemIter;
	for( itemIter = m_lstHeader.begin(); itemIter != m_lstHeader.end(); itemIter ++ )
	{
		THttpHeader * header = *itemIter;
		strRet += HTTP_CT2A( header->pszName );
		strRet += (": ");
		strRet += HTTP_CT2A( header->pszValue );
		strRet += ("\r\n");
	}
	return strRet;
}

std::string CloHttpAssoc::MakeGetParam()
{
	std::string strParam;
	std::string strName, strValue;
	bool bFirst = true;

	std::list<THttpURLParam *>::iterator itemIter;
	for( itemIter = m_lstParam.begin(); itemIter != m_lstParam.end(); itemIter ++ )
	{
		THttpURLParam * param = *itemIter;	

		// to utf8
		if( (param->dwAttrib & lohttp::ParamUTF8Name ) )
		{
			strName = HTTP_STR_TO_UTF8( param->pszName );
		}
		else if( param->pszName )
		{
			strName = HTTP_CT2A( param->pszName );
		}
		else
		{
			strName ="";
		}

		if( (param->dwAttrib &  lohttp::ParamEncodedName) )
		{
			std::string ecout;
			CloURLCode::UrlEncodeA( ecout , strName.c_str() , strName.length() );
			strName = ecout;
		}

		if( (param->dwAttrib & lohttp::ParamUTF8Value ) )
		{//
			strValue = HTTP_STR_TO_UTF8(param->pszValue);
		}
		else if( param->pszValue )
		{
			strValue = HTTP_CT2A(param->pszValue);
		}
		else
		{
			strValue="";
		}

		if( (param->dwAttrib & lohttp::ParamEncodedValue) )
		{
			std::string ecout;
			CloURLCode::UrlEncodeA( ecout , strValue.c_str() , strValue.length() );
			strValue = ecout;
		}

		if( strName.length() <= 0 )
			continue;

		if( false == bFirst )
		{
			strParam += "&";
		}

		strParam += strName;

		if( strValue.length() )
		{
			strParam += "=";
			strParam +=  strValue;
		}
		bFirst = false;
	}
	return strParam;
}

THREAD_RETURN_TYPE STDCALLTYPE CloHttpAssoc::HttpThread( void *pParam )
{
		CloHttp* pHttptask = (CloHttp *)pParam;
		if ( pHttptask != NULL )
		{
			THREAD_RETURN_TYPE ret = (THREAD_RETURN_TYPE)pHttptask->StartAsyn();
			return ret;
		}
		else
		{
			printf("HttpThread ==> pHttptask(CloHttp) is NULL! Error!\n");
			return (THREAD_RETURN_TYPE)0;
		}
}
//-------------------------------------------------------------------------------//
// CloHttpResponseAssoc

CloHttpResponseAssoc::CloHttpResponseAssoc(void)
{
	m_lhttpcode = 0;
}

CloHttpResponseAssoc::~CloHttpResponseAssoc(void)
{
	ClearHttpResponse();
}

void CloHttpResponseAssoc::SetHttpResponse(THttpMemoryBuffer * pHeader, THttpMemoryBuffer * pBody)
{
	// modified by loach 2009-07-27
	SAFE_NEW_MEMORYBUFFER(&m_httpHeader,pHeader->size+1,pHeader->size);
	COPY_MEMORYBUFFER(pHeader,&m_httpHeader);
	m_httpHeader.buffer[pHeader->size] = '\0';

	SAFE_NEW_MEMORYBUFFER(&m_httpBody,pBody->size+1,pBody->size);
	COPY_MEMORYBUFFER(pBody,&m_httpBody);
	m_httpBody.buffer[pBody->size] = '\0';

}

void CloHttpResponseAssoc::ClearHttpResponse()
{
	SAFE_DELETE_MEMORYBUFFER(&m_httpHeader);
	SAFE_DELETE_MEMORYBUFFER(&m_httpBody);	
}
