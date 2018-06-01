/**
*  @brief URL Request.
*  @file  wbURL.h
*/

#ifndef __wb_URL_H__
#define __wb_URL_H__

/** ����http ����ֵ AND url
*/

#include "libhttp/loHttp+.h"
#include "libWeibo/weibo.h"

/** ��http session �������ã�׼��������Ҫ���������� */
int  Weibo_url_set(lohttp::LOHttp* pHttp , WEIBOoption option , const void* t_wb , const struct t_wb_REQ_EX* req_ex);

/** ��������������URL��Ϣ */
int  Weibo_url_get(struct t_wb_URI* outURI , WEIBOoption option , const void* t_wb , const struct t_wb_REQ_EX* req_ex);

#endif //__wb_URL_H__
