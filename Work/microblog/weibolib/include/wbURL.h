/**
*  @brief URL Request.
*  @file  wbURL.h
*/

#ifndef __wb_URL_H__
#define __wb_URL_H__

/** 设置http 参数值 AND url
*/

#include "libhttp/loHttp+.h"
#include "libWeibo/weibo.h"

/** 对http session 进行设置，准备好所需要的请求条件 */
int  Weibo_url_set(lohttp::LOHttp* pHttp , WEIBOoption option , const void* t_wb , const struct t_wb_REQ_EX* req_ex);

/** 根据条件，生成URL信息 */
int  Weibo_url_get(struct t_wb_URI* outURI , WEIBOoption option , const void* t_wb , const struct t_wb_REQ_EX* req_ex);

#endif //__wb_URL_H__
