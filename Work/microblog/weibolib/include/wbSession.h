/**
*  @brief Micro blog session.
*  @file  wbSession.h
**/



#ifndef __wb_SESSION_H__
#define __wb_SESSION_H__

/** «Î«Ûsession ∂‘œÛ(http) */
struct t_wb_HttpSession
{
	void* phttp_;   // t_lo_http
	bool  bAsyn_;
};

struct t_wb_HttpSession* Weibo_httpsession_create(void);
void  Weibo_httpsession_destroy(struct t_wb_HttpSession*& session);

int  Weibo_session_start(struct t_wb_HttpSession* session , void* userdata);
int  Weibo_session_stop(struct t_wb_HttpSession* session);
void Weibo_session_wait(struct t_wb_HttpSession* session);

#endif /* __wb_SESSION_H__ */
