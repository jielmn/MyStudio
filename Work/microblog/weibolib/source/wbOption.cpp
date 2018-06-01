/**
*  @brief Micro blog option fundation.
*  @file  wbOption.cpp
**/

#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "libWeibo/weibo.h"

#include "wbRequest.h"
#include "wbOption.h"
#include "wbString.h"

#include "inner.h"


//---------------------------------------------------------------------------------//
// for request option function 

// for declare function
#define WEIBO_request_callback(name) \
	static int Weibo_request_##name##_cb(struct t_wb_Request* reqSession, va_list param)

static
void WEIBO_modify_senddata_format(struct t_wb_Request* reqSession)
{
	if ( reqSession->sendata_ != NULL )
	{
		struct t_wb_oauth* poauth = (struct t_wb_oauth *)reqSession->sendata_;
		poauth->format_ = reqSession->exdata_.format_;
	}
}

// for enum
#define WEIBO_request_fun(name) Weibo_request_##name##_cb


WEIBO_request_callback(REQ_UNK)
{
	return -1;
}

WEIBO_request_callback(REQ_DESTROY_FUNCTION) ///< 当请求被消毁时，通过回调函数告开发者
{
	if ( reqSession->freq_destroy_ && reqSession->puserdata_ )
	{
		reqSession->freq_destroy_(reqSession->puserdata_);
		reqSession->puserdata_ = 0;
	}
	reqSession->freq_destroy_ = va_arg( param, fun_req_destroy );
	return 0;
}

WEIBO_request_callback(REQ_SENDDATA) // 发送的数据
{
	WEIBOoption type = (WEIBOoption)va_arg( param, int );
	if ( WEIBO_OPTION(BASE) > type || WEIBO_OPTION(LAST) <= type )
	{
		printf("set REQ_SENDDATA failed! out of range!\n");
		return -1;
	}

	Weibo_senddata_free( reqSession->sendata_ );

	reqSession->sendata_ = Weibo_senddata_create( type, va_arg(param, void*) );
	reqSession->datatype_= type;

	WEIBO_modify_senddata_format( reqSession );
	return 0;
}

WEIBO_request_callback(REQ_USERDATA) // 开发者自定义数据,设置后会随着回调函数返回给开发者使用
{
	if ( reqSession->freq_destroy_ != NULL && reqSession->puserdata_ != NULL )
	{
		reqSession->freq_destroy_( reqSession->puserdata_ );
		reqSession->freq_destroy_ = NULL;
	}
	reqSession->puserdata_ = va_arg( param, void* );
	return 0;
}

WEIBO_request_callback(REQ_PROXYDATA) // 请求的代理，目前不支持
{
	return 0;
}

WEIBO_request_callback(REQ_RESULT_DATAFORMAT)
{
	reqSession->exdata_.format_ = va_arg( param, int );

	WEIBO_modify_senddata_format(reqSession);
	return 0;
}

WEIBO_request_callback(REQ_COOKIE)
{
	reqSession->exdata_.cookie_ = va_arg( param, int );
	return 0;
}

WEIBO_request_callback(REQ_APPKEY)
{
	const char *key = va_arg(param, const char*);
	if ( key != NULL )
	{
		strncpy(reqSession->exdata_.appkey_, key, 256 );
	}
	return 0;
}

WEIBO_request_callback(REQ_SECRETKEY)
{
	const char *key = va_arg(param, const char*);
	if ( key != NULL )
	{
		strncpy(reqSession->exdata_.secret_, key, 256 );
	}
	return 0;
}


WEIBO_request_callback(REQ_ACCOUNTTOKEN)
{
	const char *key = va_arg(param, const char*);
	if ( key != NULL )
	{
		strncpy(reqSession->exdata_.accountToken, key, 256 );
	}
	return 0;
}


WEIBO_request_callback(REQ_COOKIE_1)
{
	const char *key = va_arg(param, const char*);
	if ( key != NULL )
	{
		strncpy(reqSession->exdata_.cookies[0], key, 512);
	}
	return 0;
}

WEIBO_request_callback(REQ_COOKIE_2)
{
	const char *key = va_arg(param, const char*);
	if ( key != NULL )
	{
		strncpy(reqSession->exdata_.cookies[1], key, 512);
	}
	return 0;
}

WEIBO_request_callback(REQ_COOKIE_3)
{
    const char *key = va_arg(param, const char*);
    if ( key != NULL )
    {
        strncpy(reqSession->exdata_.cookies[2], key, 512);
    }
    return 0;
}

WEIBO_request_callback(REQ_COOKIE_4)
{
    const char *key = va_arg(param, const char*);
    if ( key != NULL )
    {
        strncpy(reqSession->exdata_.cookies[3], key, 512);
    }
    return 0;
}

// TRestPathCookies *    
// DWORD ( count )
// path
WEIBO_request_callback(REQ_COOKIE_ALL)
{
    TRestPathCookies * cookies = va_arg(param, TRestPathCookies *);
    unsigned long      dwCnt   = va_arg(param, unsigned long);
    const char *       path    = va_arg(param, const char *);

    unsigned long i, j;
    unsigned long index = 0;

    for ( i = 0; i < dwCnt; i++ )
    {
        TRestPathCookies * pCookie = &cookies[i];

        if ( 0 == STRICMP( pCookie->szPath, path ) )
        {
            for ( j = 0; j < 4; j++ )
            {
                TRestCookie * pSubCookie = &pCookie->cookies[j];

                if ( '\0' != pSubCookie->szCookieKey[0] && index < 4 )
                {
                    SNPRINTF( reqSession->exdata_.cookies[index], 512, "%s=%s; Path=%s", pSubCookie->szCookieKey, pSubCookie->szCookieValue, pCookie->szPath );
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
                    SNPRINTF( reqSession->exdata_.cookies[index], 512, "%s=%s; Path=/", pSubCookie->szCookieKey, pSubCookie->szCookieValue );
                    index++;
                }
            }
        }
    }

    return 0;
}

WEIBO_request_callback(REQ_THREAD_POOL_HANDLE)
{
	reqSession->exdata_.pThreadPool = va_arg(param, void*);
	if ( reqSession->exdata_.pThreadPool == NULL)
	{
		printf("reqSession->exdata_.pThreadPool is NULL! Warning!\n");
	}
	return 0;
}


WEIBO_request_callback(REQ_DEST_IP)
{
	const char *key = va_arg( param, const char* );
	if ( key != NULL )
	{
		strncpy( reqSession->exdata_.dest_ip, key, 64 );
	}
	return 0;
}


typedef int (*fun_request_format_callback)(struct t_wb_Request* reqSession , va_list param);
static fun_request_format_callback vector_request_format_cb[ WEIBO_OPTION(REQ_LAST) ] =
{
	WEIBO_request_fun(REQ_UNK),
	WEIBO_request_fun(REQ_DESTROY_FUNCTION),

	WEIBO_request_fun(REQ_SENDDATA),
	WEIBO_request_fun(REQ_USERDATA),
	WEIBO_request_fun(REQ_PROXYDATA),
	WEIBO_request_fun(REQ_RESULT_DATAFORMAT),
	WEIBO_request_fun(REQ_COOKIE),

	WEIBO_request_fun(REQ_APPKEY),
	WEIBO_request_fun(REQ_SECRETKEY),

	WEIBO_request_fun(REQ_ACCOUNTTOKEN),

	WEIBO_request_fun(REQ_COOKIE_1),
	WEIBO_request_fun(REQ_COOKIE_2),
    WEIBO_request_fun(REQ_COOKIE_3),
    WEIBO_request_fun(REQ_COOKIE_4),
    WEIBO_request_fun(REQ_COOKIE_ALL),

	WEIBO_request_fun(REQ_THREAD_POOL_HANDLE),

	WEIBO_request_fun(REQ_DEST_IP),
};

int loWeibo_request_setopt(struct t_wb_Request* reqSession ,  WEIBOReqoption option , va_list param)
{
	if ( reqSession == NULL )
	{
		printf("loWeibo_request_setopt ==>   reqSession is NULL! Error!\n");
		return -1;
	}

	if ( WEIBO_OPTION(REQ_UNK) >= option ||
         WEIBO_OPTION(REQ_LAST) <= option )
	{
		printf("loWeibo_request_setopt ==>   option is Invalid! Error!\n");
		return -1;
	}
	return vector_request_format_cb[option](reqSession, param);
}
