/**
*  @brief Micro blog request option.
*  @file  wbRequest.h
*/

#ifndef __wb_Request_H__
#define __wb_Request_H__

#include "libWeibo/weibo.h"
#include "wbSession.h"


struct t_wb_Request
{
	// 用于消毁掉puserdata_数据的函数，通过回调 WEIBO_OPTION(REQ_DESTROY_FUNCTION) 进行设置
	fun_req_destroy freq_destroy_;
	// 自定义数据，设置后会随着回调函数返回给开发者使用，通过 WEIBO_OPTION(REQ_USERDATA) 进行设置
	void* puserdata_;

	void* sendata_;
	WEIBOoption datatype_;

	struct t_wb_HttpSession* session_;
	struct t_wb_REQ_EX exdata_;
};

void* Weibo_senddata_create(int type, const void* senddata);
void Weibo_senddata_free(void*& senddata);

int  Weibo_start_request_asyn(struct t_wb_Request* reqSession);
int  Weibo_start_request(struct t_wb_Request* reqSession);

int  Weibo_stop_request(struct t_wb_Request* reqSession);
void Weibo_wait_request(struct t_wb_Request* reqSession);

#endif // __wb_Request_H__
