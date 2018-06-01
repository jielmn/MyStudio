/**
*  @brief  Interface function implements
*  @file   weibo.cpp
**/


#include <stdlib.h>
#include <string.h>
#include <string>


#include "libWeibo/weibo.h"
#include "wbOption.h"
#include "wbRequest.h"
#include "wbURL.h"
#include "httpstack.h"
#include "wbString.h"

#ifdef  __cplusplus
extern "C" {
#endif

	WEIBO_API void loWeibo_oauth_copy(struct t_wb_oauth* pdest , const struct t_wb_oauth* psrc)
	{
		if( psrc && pdest )
		{
			fun_wb_status_callback fstatus_cb = pdest->fstatus_cb;
			fun_wb_body_callback   fbody_cb = pdest->fbody_cb;
			fun_wb_header_callback fheader_cb = pdest->fheader_cb;

			memcpy(pdest , psrc , sizeof(struct t_wb_oauth) );

			pdest->fbody_cb = fbody_cb;
			pdest->fstatus_cb = fstatus_cb;
			pdest->fheader_cb = fheader_cb;
		}
	}

	/** 创建一个请求，每个事件都需要 创建一个请求（一定要通过 weibo_destroy_request（） 进行消毁 */
	WEIBO_API WEIBORequest* loWeibo_request_create(void)
	{
		WEIBORequest* request = malloc( sizeof( struct t_wb_Request ) );
		if( !request )
			return 0;

		memset( request , 0 , sizeof( struct t_wb_Request ) );

		return request;
	}

	/** 进行一些参数操作填写，然后执行(回调函数的设置...) */
	WEIBO_API int loWeibo_request_setopt(WEIBORequest* request , WEIBOReqoption option , ...)
	{
		int ret = -1;
		va_list arg;

		struct t_wb_Request* reqSession = (struct t_wb_Request*)request;
		if( !reqSession )
			return -1;

		va_start(arg, option);

		ret = loWeibo_request_setopt(reqSession, option, arg);

		va_end(arg);

		return ret;
	}

	/** 执行,等待回调数据的执行，其执行结果，会通过 weibo_request_setopt（）设置的方法，告之开发者  */
	WEIBO_API int loWeibo_request_start(WEIBORequest* request , bool bAsyn)
	{
		struct t_wb_Request* reqSession = (struct t_wb_Request*)request;
		if( !reqSession )
			return ERR_requestParam_nil;

		if( bAsyn )
		{
			return Weibo_start_request_asyn(reqSession);
		}
		else
		{
			return Weibo_start_request(reqSession);
		}
		return ERR_Fail;
	}

	WEIBO_API void loWeibo_request_wait(WEIBORequest* request)
	{
		struct t_wb_Request* reqSession = (struct t_wb_Request*)request;
		if( !reqSession )
			return ;
		Weibo_wait_request(reqSession);
	}

	WEIBO_API int loWeibo_request_stop(WEIBORequest* request)
	{
		struct t_wb_Request* reqSession = (struct t_wb_Request*)request;

		return Weibo_stop_request( reqSession );
	}

    WEIBO_API void *  loWeibo_request_get_sendData(WEIBORequest* request)
    {
        if ( NULL == request )
        {
            return NULL;
        }

        struct t_wb_Request* reqSession = (struct t_wb_Request*)request;
        return reqSession->sendata_;
    }


	/** 消毁这个请求 */
	WEIBO_API int loWeibo_request_destroy(WEIBORequest* request)
	{
		struct t_wb_Request* reqSession = (struct t_wb_Request *)request;

		if ( reqSession == NULL )
		{
			return -1;
		}

		if ( reqSession->sendata_ != NULL )
		{
			Weibo_senddata_free( reqSession->sendata_ );
		}

		Weibo_httpsession_destroy( reqSession->session_ );

		if ( reqSession->freq_destroy_ != NULL && reqSession->puserdata_ != NULL )
		{
			reqSession->freq_destroy_( reqSession->puserdata_ );
		}

		free( reqSession );
		reqSession = NULL;

		return 0;
	}

	WEIBO_API int loWeibo_get_url(struct t_wb_URI* outURI, WEIBOoption option, const void* t_wb, const struct t_wb_REQ_EX* req_ex)
	{
		return Weibo_url_get( outURI, option, t_wb,req_ex );
	}

    WEIBO_API void loWeibo_init()
    {
        InitHttpStack( 0 );
    }

    WEIBO_API int  loWeibo_request_startex( const char * szUrl, const char * szParam, int method, WeiboCallback pfnCallback, 
                                            void * context, TRestPathCookies * cookies, unsigned long dwCnt )
    {
        if ( 0 == szUrl || !(method == WEIBO_METHOD_GET || method == WEIBO_METHOD_POST) || 0 == pfnCallback )
        {
            return -1;
        }

        std::string   sUrl  = szUrl;
        std::string   sData = szParam;

        CHttpRequest  request;

        if ( method == WEIBO_METHOD_GET )
        {
            sUrl += "?";
            sUrl += szParam;

            request.SetUrl( sUrl );
            request.SetMethod( CHttpRequest::HTTP_METHOD_GET );
            request.m_callback = pfnCallback;
        }
        else
        {
            request.SetUrl( sUrl );
            request.SetData( sData );
            request.SetMethod( CHttpRequest::HTTP_METHOD_POST );
            request.m_callback = pfnCallback;
        }
        
        request["User-Agent"] = "Movision SDK/1.0";
        request["Accept"] = "*/*";
        request["Content-Type"] = "application/x-www-form-urlencoded";

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


        std::string sHost;
        unsigned short wPort;
        std::string sPathAll, sPath;

        CHttp::ParseUrl( sUrl, sHost, wPort, sPathAll );

        if ( sPathAll.length() > 0 )
        {
            std::string::size_type pos = sPathAll.find( '/', 1 );
            if ( pos != std::string::npos )
            {
                sPath = sPathAll.substr( 0, pos );
            }
            else
            {
                sPath = sPathAll;
            }
        }
        
        


        unsigned long i, j;
        unsigned long index = 0;
        char buf[8192];

        for ( i = 0; i < dwCnt; i++ )
        {
            TRestPathCookies * pCookie = &cookies[i];

            if ( 0 == STRICMP( pCookie->szPath, sPath.c_str() ) )
            {
                for ( j = 0; j < 4; j++ )
                {
                    TRestCookie * pSubCookie = &pCookie->cookies[j];

                    if ( '\0' != pSubCookie->szCookieKey[0] && index < 4 )
                    {
                        SNPRINTF( buf, 8192, "%s=%s; Path=%s", pSubCookie->szCookieKey, pSubCookie->szCookieValue, pCookie->szPath );
                        request.AppendOption( "Cookie", buf );
                        index++;
                    }
                }
            }
            else if ( 0 == STRICMP( pCookie->szPath, "/" ) )
            {
                for ( j = 0; j < 4; j++ )
                {
                    TRestCookie * pSubCookie = &pCookie->cookies[j];

                    if ( '\0' != pSubCookie->szCookieKey[0] && index < 4 )
                    {
                        SNPRINTF( buf, 8192, "%s=%s; Path=/", pSubCookie->szCookieKey, pSubCookie->szCookieValue );
                        request.AppendOption( "Cookie", buf );
                        index++;
                    }
                }
            }
        }

        CHttp::GetInstance()->AddRequest( request, context );
        return 0;
    }

    static int loWeiboUrlEncode_( char ** ppBuf, unsigned long * pdwSize, const char * szValue )
    {
        unsigned long i, j;
        unsigned long dwValueLen = strlen(szValue);
        char sca[64] = { '%' };

        for ( i = 0, j = 0; i < dwValueLen; i++ )
        {
            unsigned char c = szValue[i];
            if( isalpha( c ) || isdigit( c ) ||
                c == '-' ||	c == '_' || 	c == '.' ||	c == '!' ||	c == '~' ||	c == '*' ||	c == '\'' || c == '(' || c == ')' || c == '=' || c == '?' || c == '&' )
            {
                if ( j + 1 < *pdwSize )
                {
                    (*ppBuf)[j] = c;
                    j++;
                }
                else
                {
                    return -1;
                }
            }
            else if( c == ' ' )
            {
                if ( j + 3 < *pdwSize )
                {
                    memcpy( (*ppBuf) + j,  "%20", 3 );
                    j += 3;
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                if ( j + 3 < *pdwSize )
                {
                    sprintf( sca + 1 , "%02X", c );
                    memcpy( (*ppBuf) + j, sca, 3 );
                    j += 3;
                }
            }
        }

        *pdwSize -= j;
        (*ppBuf) += j;
        return 0;
    }

    WEIBO_API int  loWeiboUrlEncode( char * szParams, unsigned long dwSize, const char * szKey, const char * szValue, int bUtf8 )
    {
        if ( 0 == szParams || 0 == dwSize ||  0 == szKey || 0 == szValue )
        {
            return -1;
        }

        unsigned long dwLen = strlen( szParams );
        if ( dwLen > dwSize )
        {
            return -1;
        }

        char *        pTarget = szParams + dwLen;
        unsigned long dwLeft  = dwSize - dwLen;


        char * szKeyUtf8    = 0;
        char * szValueUtf8  = 0;

        const char * szKey_   = 0;
        const char * szValue_ = 0;


#ifdef SUPPORT_UTF8
        bUtf8 = TRUE;
#endif


        if ( !bUtf8 )
        {
            MbcsToUTF8( szKey,   &szKeyUtf8 );
            MbcsToUTF8( szValue, &szValueUtf8 );

            szKey_   = szKeyUtf8;
            szValue_ = szValueUtf8;

            if ( 0 == szKeyUtf8 || 0 == szValueUtf8 )
            {
                if ( szKeyUtf8 )
                {
                    free( szKeyUtf8 );
                }

                if ( szValueUtf8 )
                {
                    free( szValueUtf8 );
                }

                return -1;
            }
        }
        else
        {
            szKey_   = szKey;
            szValue_ = szValue;
        }


        if ( dwLen > 0 )
        {
            if ( -1 == loWeiboUrlEncode_( &pTarget, &dwLeft, "&" ) )
            {
                if ( !bUtf8 )
                {
                    if ( szKeyUtf8 )
                    {
                        free( szKeyUtf8 );
                    }

                    if ( szValueUtf8 )
                    {
                        free( szValueUtf8 );
                    }
                }

                return -1;
            }
        }

        
        if ( -1 == loWeiboUrlEncode_( &pTarget, &dwLeft, szKey_ ) )
        {
            if ( !bUtf8 )
            {
                if ( szKeyUtf8 )
                {
                    free( szKeyUtf8 );
                }

                if ( szValueUtf8 )
                {
                    free( szValueUtf8 );
                }
            }

            return -1;
        }
        
        
        if ( -1 == loWeiboUrlEncode_( &pTarget, &dwLeft, "=" ) )
        {
            if ( !bUtf8 )
            {
                if ( szKeyUtf8 )
                {
                    free( szKeyUtf8 );
                }

                if ( szValueUtf8 )
                {
                    free( szValueUtf8 );
                }
            }

            return -1;
        }


        if ( -1 == loWeiboUrlEncode_( &pTarget, &dwLeft, szValue_ ) )
        {
            if ( !bUtf8 )
            {
                if ( szKeyUtf8 )
                {
                    free( szKeyUtf8 );
                }

                if ( szValueUtf8 )
                {
                    free( szValueUtf8 );
                }
            }

            return -1;
        }

        *pTarget = '\0';


        if ( !bUtf8 )
        {
            if ( szKeyUtf8 )
            {
                free( szKeyUtf8 );
            }

            if ( szValueUtf8 )
            {
                free( szValueUtf8 );
            }
        }

        return 0;
    }

    WEIBO_API int  loWeiboUrlEncodeInt( char * szParams, unsigned long dwSize, const char * szKey, int  nValue, int bUtf8 )
    {
        char  szValue[64] = { 0 };
        sprintf( szValue, "%d", nValue );

#ifdef SUPPORT_UTF8
        bUtf8 = TRUE;
#endif

        return loWeiboUrlEncode( szParams, dwSize, szKey, szValue, bUtf8 );
    }

#ifdef  __cplusplus
}
#endif

