#ifndef __lo_HTTP_ASSOC_H__
#define __lo_HTTP_ASSOC_H__

#include <list>
#include "libhttp/loHttpDef.h"
#include "loHttpMap.h"
#include "loHttpStr.h"
#include "loHttpStruct.h"
#include "loHttpCurl.h"
#include "loBytebuffer.h"
#include "loHttpThread.h"


namespace lohttp
{

#ifndef BUILD_CHARACTER_SET

	typedef lohttp::CloHttpMapT<lohttp::CloHttpStrA> CHttpMap;

#else

	#ifdef UNICODE
		typedef lohttp::CloHttpMapT<lohttp::CloHttpStrW> CHttpMap;
	#else
		typedef lohttp::CloHttpMapT<lohttp::CloHttpStrA> CHttpMap;
	#endif

#endif

	class CloHttpResponse;

	struct t_lo_httpCB
	{
		fun_httpbuffer_callback fbuffer_cb;
		fun_httpprogress_callback fprogress_cb;
		fun_httpcode_callback fcode_cb;
		fun_httprespone_callback frespone_cb;
		fun_httprelease_callback frelease_cb;
	};

	class CloHttpAssoc:public IloTaskCallBack
	{
	public:
		CloHttpAssoc(void);
		~CloHttpAssoc();

		void ClearParams();
		void ClearHeader();
		void ClearProxy();

		std::string MakeGetParam();
		std::string MakeGetURL();
		std::string GetHttpHeader();

		static THREAD_RETURN_TYPE STDCALLTYPE HttpThread(void* pParam);

	private:
		void OnTaskState( long lState );
		int  OnTaskProgress( long lCurBytes, long lTotalBytes );
		int  OnTaskBuffer( unsigned char* pBuffer, long lBuffer );
		void OnTaskCode( long code, const char *errString );
		void OnTaskTime( const THttpTime *pDownloadTime ) ;

		void OnStart();
		void OnFinished( CloHttpResponse* resp );

	private:
		HTTPChar m_szURL[ConstURLLength];

		HTTPChar m_szProxyUserName[ConstProxyUserLength];
		HTTPChar m_szProxyUserPwd[ConstProxyPwdLength];
		HTTPChar m_szProxySrvAddr[ConstProxyServerLength];
		int  m_nProxyType;
		int  m_nProxySrvPort;


		//// POST文件时，表头的参数
		//HTTPChar	m_szUploadParam[ConstUploadParamLength];
		//// POST本地的文件
		//HTTPChar	m_szUploadFile[MAX_PATH+1];

		HTTPChar m_szFile[MAX_PATH+1];
		HTTPChar m_szPath[MAX_PATH+1];

		THREAD_HANDLE m_hInetTh;

		int m_nMethod;
		int m_ierrCode;
		THttpTime m_time;

		void* m_pUserData;       // t_lo_http
		struct t_lo_httpCB m_cb;

		bool m_bFormData;
		struct THttpForm m_Httpform;

		locom::CloByteBuffer m_binData;        // 二进制数据
		std::list<THttpURLParam*> m_lstParam;  // 请求参数
		std::list<THttpHeader*> m_lstHeader;   // 请求的Http Header

		CloHttpCurl* m_pHttpCurl;
		CloHttpResponse* m_pRespone;

		friend class CloHttpCurl;
		friend class CloHttp;
	};

	class CloHttpResponseAssoc
	{
	public:
		CloHttpResponseAssoc();
		~CloHttpResponseAssoc();

		void SetHttpResponse( THttpMemoryBuffer *pHeader, THttpMemoryBuffer *pBody );

		void SetHttpCode( long httpcode )
		{
			m_lhttpcode = httpcode;
		}
		void ClearHttpResponse();

	private:
		THttpMemoryBuffer m_httpHeader;
		THttpMemoryBuffer m_httpBody;

		long m_lhttpcode;

		friend class CloHttpResponse;
	};

}

#endif  // __lo_HTTP_ASSOC_H__

