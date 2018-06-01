#ifndef __lo__HTTP_C_H__
#define __lo__HTTP_C_H__


#include "libhttp/loHttpDef.h"

BEGIN_EXTERN_C

namespace lohttp
{
	typedef enum 
	{
		LOHTTP_OPT_UNK = 0,
		LOHTTP_OPT_callbackOfBuffer,
		LOHTTP_OPT_callbackOfProgress,
		LOHTTP_OPT_callbackOfCode,
		LOHTTP_OPT_callbackOfRespone,
		LOHTTP_OPT_callbackOfRelease,
		LOHTTP_OPT_userdata,
		LOHTTP_OPT_setmethod,
		LOHTTP_OPT_addparam,
		LOHTTP_OPT_clearparam,
		LOHTTP_OPT_addheader,
		LOHTTP_OPT_clearheader,
		LOHTTP_OPT_addbinarydata,
		LOHTTP_OPT_clearbinarydata,
		LOHTTP_OPT_setfile,
		LOHTTP_OPT_setproxysrvaddr,
		LOHTTP_OPT_setproxyaccount,
		LOHTTP_OPT_setuseragent,
		LOHTTP_OPT_setform,
		LOHTTP_OPT_LAST,
	}LOHttpoption;

	loHTTP_API LOHttp* Http_createobject( void );
	loHTTP_API void Http_destroyobject( LOHttp* phttp );

	loHTTP_API int Http_setopt( LOHttp* phttp, LOHttpoption option, ... );
	loHTTP_API void Http_seturl( LOHttp* phttp, const HTTPChar* szUrl );

	loHTTP_API int Http_start( LOHttp* phttp );
	loHTTP_API int Http_start_asyn( LOHttp* phttp, LOThreadPool* pthreadpool );
	loHTTP_API void Http_wait( LOHttp* phttp );
	loHTTP_API int Http_cancel( LOHttp* phttp, unsigned long msTimeout = 1000 );

	loHTTP_API LOHttpResponse* Http_getRespone( LOHttp* phttp );
	loHTTP_API LOHttpResponse* Http_detachRespone( LOHttp* phttp );

	loHTTP_API long Http_resp_code( const LOHttpResponse* resp );

	loHTTP_API bool Http_resp_getheaderlength( const LOHttpResponse* resp, unsigned long& dwHeaderLen );
	loHTTP_API bool Http_resp_readheader( const LOHttpResponse* resp, fun_http_resp_header_callback cb , void* pUsrData );

	loHTTP_API bool Http_resp_getbodylength( const LOHttpResponse* resp, unsigned long& dwBodyLen );
	loHTTP_API bool Http_resp_readbody( const LOHttpResponse* resp, fun_http_resp_body_callback cb, void* pUsrData );

	/** 注：只有 Http_detachRespone 结合使用，否则会想不到的后果 */
	loHTTP_API void Http_resp_destroy( LOHttpResponse* resp );


	loHTTP_API LOHttpHeader* Http_header_create( void );
	loHTTP_API void Http_header_destroy( LOHttpHeader* header );

	loHTTP_API void Http_header_parse( LOHttpHeader* header, const char* context, unsigned long len );
	loHTTP_API bool Http_header_getcontentlength( const LOHttpHeader* resp, unsigned long& cbContLen );

	loHTTP_API int Http_header_count( const LOHttpHeader* header, const HTTPChar* szName );
	loHTTP_API const HTTPChar* Http_header_name( const LOHttpHeader* header, const HTTPChar* szName, unsigned long nIdx );

	loHTTP_API const HTTPChar* Http_header_state( const LOHttpHeader* header );

}

END_EXTERN_C

#endif /* __lo__HTTP_C_H__ */
