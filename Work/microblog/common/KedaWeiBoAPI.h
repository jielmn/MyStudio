#ifndef _KEDA_WEIBO_API_
#define _KEDA_WEIBO_API_

#define  _API_VERSION_        "1.0.0.2"


#if (defined(WIN32) || defined(_WIN32) ) && !defined(KEDAWEIBOAPI_STATICLIB)

	#ifdef KEDAWEIBOAPI_EXPORTS
		#define KEDAWEIBOAPI_API __declspec(dllexport)
	#else
		#define KEDAWEIBOAPI_API __declspec(dllimport)
	#endif

#else  // 其他平台的，或者是静态链接

	#define KEDAWEIBOAPI_API

#endif


#include "libWeibo/weibo.h"

#ifdef  __cplusplus
extern "C" {
#endif

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_SetCommonProxy( const TWeiboProxyParam * pProxyParam );


/// ===＞  科达公司 微博v0.1接口

//1.Oauth的accountToken的获得(平台API)  -- 82
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//1.Oauth的accountToken的获得(微博API)  -- 82
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//2.登录平台API  -- 83
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//2.登录微博API  -- 83
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//3.获得当前登录用户及关注用户的微博  -- 84
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_me_and_friends(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//4.获得用户发布的微博数据（友讯）  -- 85
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//5.获得用户发布的微博数据（全部）  -- 86
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_all_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//6.获得一条主题以及回复  -- 87
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_subject_and_reply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//7.发布一条微博  -- 88
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_publish(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//8.回复一条微博  -- 89
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_reply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//9.获得用户信息  -- 90
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===＞  2012-03-03 科达公司 微博v1.0接口 第一部分

//1.获得公司广场的微博数据  -- 91
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_company_public_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//2.获得群组的微博数据  -- 92
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_groups_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//3.获得@当前用户的微博数据  -- 93
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_mentions_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//4.获得回复我的微博数据  -- 94
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_replies_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//5.获得我赞过的微博数据  -- 95
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_likes_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//6.获得我收藏的微博数据  -- 96
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_favorites_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//7.获得某个话题的微博数据  -- 97
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_topics_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//8.获得公司人脉  -- 98
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_company(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//9.获得我的关注列表  -- 99
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_attention(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//10.获得我的粉丝列表  -- 100
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_followers(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//11.获得我的好友列表  -- 101
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_friends(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===＞  2012-03-06 科达公司 微博v1.0接口 第二部分

//12.添加关注  -- 102
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_friendship_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//13.取消关注  -- 103
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_friendship_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//14.添加收藏  -- 104
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_favorites_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//15.取消收藏  -- 105
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_favorites_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//16.添加话题  -- 106
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_topic_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//17.取消话题  -- 107
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_topic_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//18.添加like  -- 108
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_like_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//19.取消like  -- 109
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_like_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//20.获取我的群组列表  -- 110
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_group_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//21.获得所有的群组列表  -- 111
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_group_all(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//22.添加群操作  -- 112
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//23.加入群操作  -- 113
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_join(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//24.退出群操作  -- 114
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_quit(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//25.查看我的私信列表  -- 115
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_privatemessage_list(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//26.查看我与某人的私信的聊天记录  -- 116
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_privatemessage_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//27.发送私信到某人  -- 117
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_privatemessage_send(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//28.删除私信  -- 118
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_privatemessage_delete(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===＞  2012-03-09 科达公司 微博v1.0接口 第三部分

//29.验证终端账号  -- 119
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_validateclientaccount(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//30.查询账号详细信息  -- 120
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_getaccountinfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//31.修改账号详细信息  -- 121
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updateaccountinfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//32.修改账号密码  -- 122
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updatepassword(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===＞  2012-03-17 科达公司 微博v1.0接口 第四部分

//33.获得公司组织架构和所有人员信息  -- 123
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_all(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//34.获取公司组织架构信息  -- 124
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_company(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//35.获取部门下人员信息(不包含子部门)  -- 125
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//36.获取部门下人员信息(包含子部门人员)  -- 126
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_alluser(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//37.在组织架构中搜索部门信息  -- 127
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_searchdepartment(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//38.在组织架构中搜索人员信息  -- 128
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_searchuser(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//39.获取部门下的员工的在线状态  -- 129
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_onlineByDepId(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//40.获取组织里用户的在线状态  -- 130
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_onlineByUserId(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===>  获取头像接口

//41.获取头像  -- 131
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_portrait(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//42.批量查询账号详细信息  -- 132
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_batchGetAccountInfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//43.修改用户头像  -- 133
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_confirm_portrait(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//44.获取组织架构版本信息  -- 134
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_getDepartmentVersion(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//45.带图片和附件的微博发布  -- 135
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_upload(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//46.删除自己的微博  -- 136
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_deleteThread(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//47.删除自己的回复  -- 137
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_deleteReply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//48.获取@、赞、粉丝等初始消息  -- 138
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_notify_getAll(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//49.保持用户心跳(平台API)  -- 139
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//49.保持用户心跳(微博API)  -- 139
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//50.获得用户信息(通过email)  -- 140
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_info_by_email(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//51.获得当前登录用户信息  -- 141
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_current_user_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//52.获取长篇大论的url  -- 142
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_article_url(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//53.修改个人介绍  -- 143
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updateBrief(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//54.按姓名模糊查找本企业联系人  -- 144
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_list_account_by_str(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//55.忘记密码  -- 145
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_password_reset_by_mail(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//56.获取赞过我的微博数据  -- 146
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_get_likes_me_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//57.设置是否被模糊搜索到  -- 147
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_set_is_like_query(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//58.设置添加联系人认证模式  -- 148
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_set_auth_mode(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//59.条件查询企业联系人  -- 149
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_condition_query(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);


// 会议管理
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_meeting_list(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_meeting_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_booking_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m(WEIBORequest *pWBReq, WEIBOoption e, void *pt_paraIn);


typedef void (*POnKedaWeiBoLogCallback) ( const char * szLog );

KEDAWEIBOAPI_API void  SetKedaWeiBoLogCallback( POnKedaWeiBoLogCallback pfnLog );
KEDAWEIBOAPI_API void  fnKedaWeiboDebugInfo( );

KEDAWEIBOAPI_API const char *  fnGetKedaWeiboCompileTime( );
KEDAWEIBOAPI_API const char *  fnGetKedaWeiboVersion( );


KEDAWEIBOAPI_API void fnKedaWeiBoAPI_SetCallBack( fun_wb_header_callback pfnCallback );
KEDAWEIBOAPI_API void fnKedaRest_GetCookies( TRestPathCookies * cookies, unsigned long dwCnt, const char * szHeader );

#ifdef  __cplusplus
}
#endif


#endif // _KEDA_WEIBO_API_

