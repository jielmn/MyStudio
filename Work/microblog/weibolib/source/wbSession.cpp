/**
*  @brief Micro blog session.
*  @file  wbSession.cpp
**/

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "wbSession.h"
#include "wbRequest.h"
#include "wbURL.h"
#include "wbLog.h"
#include "wbCheck.h"

#include "libhttp/loHttp+.h"
#include "inner.h"


int Weibo_httpbuffer_callback( unsigned char* pBuffer, long lBuffer, void* pUsrData )
{
	wbPRINTF("Weibo_httpbuffer_callback = %s \n\n", pBuffer );
	return 0;
}

int Weibo_httpprogress_callback( long lCurBytes, long lTotalBytes, void* pUsrData )
{
// 	wbPRINTF("\nWeibo_httpprogress_callback ==> CurBytes = %d   TotalBytes = %d\n\n", lCurBytes, lTotalBytes);
	return 0;
}

// 解析HTTP的下载状态回调
void Weibo_httpcode_callback( int state, int errCode, const lohttp::THttpTime* pTime, void* pUsrData )
{
	struct t_wb_Request* request = (struct t_wb_Request*)pUsrData;
	if ( request == NULL )
	{
		printf("Weibo_httpcode_callback ==> request is NULL! Return!\n");
		return ;
	}

	//switch(state)
	//{
	//case lohttp::E_HTTPSTATUS_START:
	//	wbPRINTF("   state = %d , errCode = %d , 开始下载 \n\n",state, errCode );
	//	break;

	//case lohttp::E_HTTPSTATUS_STOP:
	//	wbPRINTF("   state = %d , errCode = %d , 停止下载 \n\n",state, errCode );
	//	break;

	//case lohttp::E_HTTPSTATUS_CANCEL:
	//	wbPRINTF("   state = %d , errCode = %d 下载取消 \n\n",state, errCode );
	//	break;

	//case lohttp::E_HTTPSTATUS_COMPLETE:
	//	wbPRINTF("   state = %d , errCode = %d 整个过程完成 \n\n",state, errCode );
	//	break;
	//}

	if ( request->sendata_ != NULL )
	{
		struct t_wb_base* base = (struct t_wb_base *)request->sendata_;
		if ( base->wbauth_.fstatus_cb != NULL )
		{
			base->wbauth_.fstatus_cb( state, errCode, request->puserdata_ );
		}
	}
}

// 解析HTTP的回复包回调
void Weibo_httprespone_callback(const lohttp::LOHttpResponse* resp , void* pUsrData)
{
	struct t_wb_Request* request = (struct t_wb_Request*)pUsrData;
	if ( request == NULL )
	{
		printf("Weibo_httprespone_callback ==> request is NULL! Return!\n");
		return ;
	}

	wbPRINTF("---------开始解析回应包---------\n\n");

	if ( request->sendata_ != NULL )
	{
		struct t_wb_base* base = (struct t_wb_base*)request->sendata_;

		//if ( base->wbauth_.fheader_cb != NULL )
		//{
		//	lohttp::Http_resp_readheader( resp, (lohttp::fun_http_resp_header_callback)base->wbauth_.fheader_cb, request->puserdata_ );
		//}

        if ( g_pfnHeaderCallback != NULL )
        {
        	lohttp::Http_resp_readheader( resp, (lohttp::fun_http_resp_header_callback)g_pfnHeaderCallback, request->puserdata_ );
        }
        

		if ( base->wbauth_.fbody_cb != NULL )
		{
			lohttp::Http_resp_readbody( resp, (lohttp::fun_http_resp_body_callback)base->wbauth_.fbody_cb, request->puserdata_ );
		}
	}

	wbPRINTF("\n\n---------结束解析回应包---------\n\n");
}

void Weibo_httprelease_callback(void* pUsrData)
{
	wbPRINTF("Weibo_httprelease_callback 资源释放掉 \n\n");
	struct t_wb_Request* request = (struct t_wb_Request*)pUsrData;
	if ( request == NULL )
	{
		return ;
	}
	if ( request->freq_destroy_ != NULL )
	{
		request->freq_destroy_(request->puserdata_);
		request->puserdata_ = NULL;
	}
}


static
void Weibo_session_callback( lohttp::LOHttp* pHttp, struct t_wb_Request* request )
{ 
	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_callbackOfBuffer   , Weibo_httpbuffer_callback );
	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_callbackOfProgress , Weibo_httpprogress_callback );
	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_callbackOfCode     , Weibo_httpcode_callback );
	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_callbackOfRespone  , Weibo_httprespone_callback );
 	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_callbackOfRelease  , Weibo_httprelease_callback );

	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_userdata , request );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct t_wb_HttpSession* Weibo_httpsession_create(void)
{
	struct t_wb_HttpSession* psession = (struct t_wb_HttpSession*)malloc( sizeof( struct t_wb_HttpSession ) );

	if ( psession != NULL )
	{
		memset( psession, 0, sizeof( struct t_wb_HttpSession ) );
	}
	else
	{
		printf("Weibo_httpsession_create ==> malloc failed!\n");
	}

	return psession;
}

void Weibo_httpsession_destroy(struct t_wb_HttpSession*& session)
{
	if ( session == NULL )
	{
		printf("Weibo_httpsession_destroy ==> session is NULL! Return!\n");
		return;
	}

	if ( session->phttp_ != NULL )
	{
		lohttp::Http_destroyobject( session->phttp_ );
		session->phttp_ = NULL;
	}
	free(session);
	session = NULL;
}

int Weibo_session_start(struct t_wb_HttpSession* session, void* userdata)
{
	struct t_wb_Request* request = (struct t_wb_Request *)userdata;
	if ( session == NULL || request == NULL )
	{
		wbASSERT(0);
		return ERR_Fail;
	}

	if ( session->phttp_ == NULL )
	{
		session->phttp_ = lohttp::Http_createobject();
		if ( session->phttp_ == NULL )
		{
			wbASSERT(0);
			return ERR_createhttpobject_nil;
		}
	}

	lohttp::Http_setopt(session->phttp_, lohttp::LOHTTP_OPT_clearparam, 0);
	lohttp::Http_setopt(session->phttp_, lohttp::LOHTTP_OPT_clearheader, 0);
	lohttp::Http_setopt(session->phttp_, lohttp::LOHTTP_OPT_clearbinarydata, 0);

	if ( 0 != Weibo_check_valid( request->datatype_, request->sendata_) )
	{
		return ERR_checkvalid_fail;
	}

	if ( 0 != Weibo_url_set( session->phttp_, request->datatype_, request->sendata_, &request->exdata_ ) )
	{
		return ERR_url_set_fail; // 设置URL有问题
	}

	Weibo_session_callback( session->phttp_, request );

	lohttp::Http_setopt( session->phttp_, lohttp::LOHTTP_OPT_setuseragent, HTTP_T("Movision SDK/1.0") );

	if ( session->bAsyn_ )
	{
		return lohttp::Http_start_asyn( session->phttp_, request->exdata_.pThreadPool );
	}

	return lohttp::Http_start( session->phttp_ );
}

int Weibo_session_stop(struct t_wb_HttpSession* session)
{
	if ( session == NULL )
	{
		printf("Weibo_session_stop ==> session is NULL! Return!\n");
		return 0;
	}

	if ( session->phttp_ == NULL )
	{
		printf("Weibo_session_stop ==> session->phttp_ is NULL! Return!\n");
		return 0;
	}

	lohttp::Http_cancel( session->phttp_ );
	return 0;
}

void Weibo_session_wait(struct t_wb_HttpSession* session)
{
	if ( session == NULL )
	{
		printf("Weibo_session_wait ==> session is NULL! Return!\n");
		return ;
	}

	if ( session->phttp_ == NULL )
	{
		printf("Weibo_session_wait ==> session->phttp_ is NULL! Return!\n");
		return ;
	}
	lohttp::Http_wait( session->phttp_ );
}
