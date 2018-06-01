#ifndef __lo_HTTP_H__
#define __lo_HTTP_H__

#include <libhttp/loHttpDef.h>
#include "httpstack.h"

namespace lohttp
{	
	class CloHttpResponse;
	class CloHttpAssoc;
	class CloHttpResponseAssoc;

	/** 一个CloHttp对象对应一个线程阻塞式的下载任务 */
	class loHTTP_API CloHttp
	{
	public:
		CloHttp();
		~CloHttp(void);	
	public:		
		void SetCallbackOfBuffer( fun_httpbuffer_callback fbuffer_cb );		
		void SetCallbackOfProgress( fun_httpprogress_callback fprogress_cb );
		void SetCallbackOfCode( fun_httpcode_callback fcode_cb );
		void SetCallbackOfRespone( fun_httprespone_callback fresp_cb );
		void SetCallbackOfRelease( fun_httprelease_callback frelease_cb );

		void SetUserData( void* puserdata );
		void SetURL( const HTTPChar* szURL );
		void SetFileName( const HTTPChar* szPath, const HTTPChar* szFile );
		void SetHttpForm( const struct THttpForm* pform );
		void SetUserAgent( const HTTPChar* szAgent );

		void AddParam( const HTTPChar* szName, const HTTPChar* szValue, unsigned long dwParamAttr = lohttp::ParamNormal );
		void ClearParams();

		void AddHeader( const HTTPChar* szName, const HTTPChar* szValue );
		void ClearHeader();

		void AddBinaryData( const char* szBinaryData, long lLen );
		void ClearBinaryData( void );

		void SetProxySrvAddr( int proxyType, const HTTPChar* szProxySrv, unsigned short nProxySrvPort );
		void SetProxyAccount( const HTTPChar* szUserName = NULL, const HTTPChar* szPassword = NULL );
		void ClearProxy();

		void EnableFormData( bool bForm = false );
//		void SetUploadParam(const HTTPChar* lpszParamName, const HTTPChar* lpszFile);

		int Request( const HTTPChar* szUrl, int method );

		void RequestAsyn( const HTTPChar* szUrl, int method, void *p_threadpool );
		void Wait();

		bool Cancel( unsigned long msTimeout = 1000 );

		CloHttpResponse* GetRespone(void) const;
		CloHttpResponse* DetachRespone(void);
		
		THttpTime* GetHttpTime() const; // 请求时间( 不精确 )

        static  void  OnHttpStackCb(int nError, DWORD dwCode, const char * szData, DWORD dwDataLen, 
            const char * szHeader, DWORD dwHeaderLen,  void * context );

	private:
		void InitURL( const HTTPChar* szUrl, int method );
		//int  Split_url_copy_keyval(HTTPChar* val , const HTTPChar* start , const HTTPChar* end);
		//int  Split_url_param(const HTTPChar*& next , const HTTPChar*& key , const HTTPChar*& qe , const HTTPChar*& split ,const char endflag );

		int	 HttpMethodGet();
		int	 HttpMethodDownload();
		int	 HttpMethodDelete();
		int	 HttpMethodPut();
		int	 HttpMethodPost();
		int	 HttpMethodUpload();

//		int	 HttpMethodPostBinary();
//		int	 HttpMethodPostBinaryText();

		int  HttpMethod( int method );

		void SetHttpProxyCfg();

		CloHttpResponse* CreateHttpResponse(DWORD dwCode = 0, const char * pBody = 0, DWORD dwBody = 0, const char * pHeader = 0, DWORD dwHeader = 0);

		unsigned long StartAsyn();

	protected:
		CloHttpAssoc* m_pHttpAssoc;
		friend class CloHttpAssoc;

    public:
        int  m_bStartAsynFinished;
	};

	class loHTTP_API CloHttpResponse
	{
	public:
		CloHttpResponse(void);
		~CloHttpResponse(void);

	public:
		bool GetHeaderLength( unsigned long& len ) const;

		unsigned long ReadHeader( char * pbyBuff, unsigned long cbBuff, unsigned long& offsize ) const;
		bool ReadHeaderCallback( fun_http_resp_header_callback cb, void* pUsrData ) const;

		bool GetBodyLength( unsigned long& dwBodyLen ) const;

		unsigned long ReadBody( char *pbyBuff, unsigned long cbBuff, unsigned long& offsize ) const;
	    bool ReadBodyCallback( fun_http_resp_body_callback cb, void* pUsrData ) const;

		long GetHttpCode(void) const;

	private:
		CloHttpResponseAssoc* m_pResponseAssoc;
		friend class CloHttp;
	};

	class loHTTP_API CloHttpHeader
	{
	public:
		CloHttpHeader();
		~CloHttpHeader();

		void Parse( const char* header, unsigned long len );

		int GetHeadercount( const HTTPChar* szName ) const;
		const HTTPChar*	GetHeader( const HTTPChar* szName, unsigned long nIdx = 0 ) const;

		const HTTPChar*	GetHeaderState() const;

        bool GetContentLength ( unsigned long& cbContLen ) const;

	private:
		void* m_pHeaderAssoc;
		HTTPChar m_httpState[64];
	};

}

#endif //__lo_HTTP_H__
