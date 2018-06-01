// test_api.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <string.h>
// #include <windows.h>
#include "KedaWeiBoAPI.h"

// callback使用
#include "wbParser/splitstr.h"
#include "wbParser/wbParser.h"
#include "wbParser/wbParseWrapper.h"

#include "libhttp/thpool.h"
                                         
// #define TEST_API_ADDR                     "10.31.3.2"
// #define TEST_API_ADDR                     "172.16.3.64"
#define TEST_API_ADDR                     "172.16.235.230"

#define HTTP_TEST_API_ADDR                "http://" TEST_API_ADDR


#define TEST_ACCOUNT                      "keda@keda.com"
// #define TEST_ACCOUNT                      "sunfei@kedacom.com"

#define TEST_PASSWORD                     "111111"
// #define TEST_PASSWORD                     "1231234"


#define  M_ERROR_ELEMENT                 "errorCode"





char g_szPlatformAccountToken[128] = {0};
char g_szWeiboAccountToken[128] = {0};
char g_szMeetingAccountToken[128] = {0};

char g_szPlatformCookieValue[2][512] = {0};
char g_szWeiboCookieValue[2][512] = {0};
char g_szMeetingCookieValue[2][512] = {0};

enum httpstatus
{
	http_closed = 0,
	http_start,
	http_stop,
	http_cancel,
	http_complete,
};

enum APIType
{
	emUnknown_API = 0,
	emPlatform_API,
	emWeibo_API,
    emMeeting_API,
};

bool Wb_init_wb_struct( int option , void* t_wb);


struct t_wb_allstruct
{
	// 2012-01-04 add by sunfei 科达公司 微博v0.1接口
	struct t_wb_keda_get_accounttoken           keda_p_get_accounttoken;    //1. 获得Oauth的accountToken(平台API)
	struct t_wb_keda_get_accounttoken           keda_w_get_accounttoken;    //1. 获得Oauth的accountToken(微博API)

	struct t_wb_keda_login                      keda_p_login;               //2. 登录平台API
	struct t_wb_keda_login                      keda_w_login;               //2. 登录微博API

	struct t_wb_keda_get_me_and_friends         keda_get_me_and_friends;    //3. 获得当前登录用户及关注用户的微博
	struct t_wb_keda_get_user_data              keda_get_user_data;         //4. 获得用户发布的微博数据（友讯）
	struct t_wb_keda_get_user_all_data          keda_get_user_all_data;     //5. 获得用户发布的微博数据（全部）
	struct t_wb_keda_get_subject_and_reply      keda_get_subject_and_reply; //6. 获得一条主题以及回复
	struct t_wb_keda_message_publish            keda_message_publish;       //7. 发布一条微博
	struct t_wb_keda_message_reply              keda_message_reply;         //8. 回复一条微博
	struct t_wb_keda_get_user_info              keda_get_user_info;         //9. 获得用户信息

	// 2012-03-03 add by sunfei 科达公司 微博v1.0接口 第一部分
	struct t_wb_keda_get_company_public_data    keda_get_company_public_data;  //1.获得公司广场的微博数据
	struct t_wb_keda_get_groups_data            keda_get_groups_data;          //2.获得群组的微博数据
	struct t_wb_keda_get_mentions_data          keda_get_mentions_data;        //3.获得@当前用户的微博数据 
	struct t_wb_keda_get_replies_data           keda_get_replies_data;         //4.获得回复我的微博数据
	struct t_wb_keda_get_likes_data             keda_get_likes_data;           //5.获得我赞过的微博数据
	struct t_wb_keda_get_favorites_data         keda_get_favorites_data;       //6.获得我收藏的微博数据
	struct t_wb_keda_get_topics_data            keda_get_topics_data;          //7.获得某个话题的微博数据
	struct t_wb_keda_get_friendship_company     keda_get_friendship_company;   //8.获得公司人脉
	struct t_wb_keda_get_friendship_attention   keda_get_friendship_attention; //9.获得我的关注列表 
	struct t_wb_keda_get_friendship_followers   keda_get_friendship_followers; //10.获得我的粉丝列表
	struct t_wb_keda_get_friendship_friends     keda_get_friendship_friends;   //11.获得我的好友列表

	// 2012-03-06 add by sunfei 科达公司 微博v1.0接口 第二部分
	struct t_wb_keda_friendship_create          keda_friendship_create;        //12.添加关注
	struct t_wb_keda_friendship_destroy         keda_friendship_destroy;       //13.取消关注
	struct t_wb_keda_favorites_create           keda_favorites_create;         //14.添加收藏
	struct t_wb_keda_favorites_destroy          keda_favorites_destroy;        //15.取消收藏
	struct t_wb_keda_topic_create               keda_topic_create;             //16.添加话题
	struct t_wb_keda_topic_destroy              keda_topic_destroy;            //17.取消话题
	struct t_wb_keda_like_create                keda_like_create;              //18.添加like
	struct t_wb_keda_like_destroy               keda_like_destroy;             //19.取消like
	struct t_wb_keda_get_group_user             keda_get_group_user;           //20.获取我的群组列表
	struct t_wb_keda_get_group_all              keda_get_group_all;            //21.获得所有的群组列表
	struct t_wb_keda_group_create               keda_group_create;             //22.添加群操作
	struct t_wb_keda_group_join                 keda_group_join;               //23.加入群操作
	struct t_wb_keda_group_quit                 keda_group_quit;               //24.退出群操作
	struct t_wb_keda_get_privatemessage_list    keda_get_privatemessage_list;  //25.查看我的私信列表
	struct t_wb_keda_get_privatemessage_user    keda_get_privatemessage_user;  //26.查看我与某人的私信的聊天记录
	struct t_wb_keda_privatemessage_send        keda_privatemessage_send;      //27.发送私信到某人
	struct t_wb_keda_privatemessage_delete      keda_privatemessage_delete;    //28.删除私信

	// 2012-03-09 add by sunfei 科达公司 微博v1.0接口 第三部分
	struct t_wb_keda_ams_validateClientAccount  keda_ams_validateClientAccount;   //1.验证终端账号
	struct t_wb_keda_ams_getAccountInfo         keda_ams_getAccountInfo;          //2.查询账号详细信息
	struct t_wb_keda_ams_updateAccountInfo      keda_ams_updateAccountInfo;       //3.修改账号详细信息 
	struct t_wb_keda_ams_updatePassword         keda_ams_updatePassword;          //4.修改账号密码

	// 2012-03-17 add by sunfei 科达公司 微博v1.0接口 第四部分
	struct t_wb_keda_get_department_all           keda_get_department_all;          //1.获得公司组织架构和所有人员信息
	struct t_wb_keda_get_department_company       keda_get_department_company;      //2.获取公司组织架构信息
	struct t_wb_keda_get_department_user          keda_get_department_user;         //3.获取部门下人员信息(不包含子部门) 
	struct t_wb_keda_get_department_allUser       keda_get_department_allUser;      //4.获取部门下人员信息(包含子部门人员)
	struct t_wb_keda_department_searchDepartment  keda_department_searchDepartment; //5.在组织架构中搜索部门信息 
	struct t_wb_keda_department_searchUser        keda_department_searchUser;       //6.在组织架构中搜索人员信息

	struct t_wb_keda_department_online_by_depId   keda_department_online_by_depId;   //7.获取部门下的员工的在线状态 
	struct t_wb_keda_department_online_by_userId  keda_department_online_by_userId;  //8.获取组织里用户的在线状态

	struct t_wb_keda_get_portrait                 keda_get_portrait;                 //9.获取头像
	struct t_wb_keda_ams_batchGetAccountInfo      keda_ams_batchGetAccountInfo;      //10.批量查询账号详细信息
	struct t_wb_keda_confirmPortrait              keda_confirmPortrait;              //11.修改用户头像
	struct t_wb_keda_department_get_version       keda_department_get_version;       //12.获取组织架构版本信息
	struct t_wb_keda_message_upload               keda_message_upload;               //13.带图片和附件的微博发布

	struct t_wb_keda_message_delete_thread        keda_message_delete_thread;        //14.删除自己的微博
	struct t_wb_keda_message_delete_reply         keda_message_delete_reply;         //15.删除自己的回复
	struct t_wb_keda_notify_get_all               keda_notify_get_all;               //16.获取@、赞、粉丝等初始消息

	struct t_wb_keda_keep_heartbeat               keda_p_keep_heartbeat;             //17.保持用户心跳
	struct t_wb_keda_keep_heartbeat               keda_w_keep_heartbeat;             //17.保持用户心跳

	struct t_wb_keda_get_user_info_by_email       keda_get_user_info_by_email;       //18.获得用户信息(通过email)

	struct t_wb_keda_get_current_user_info        keda_get_current_user_info;        //19.获得当前登录用户信息
	struct t_wb_keda_get_article_url              keda_get_article_url;              //20.获取长篇大论的url
	struct t_wb_keda_ams_updateBrief              keda_ams_updateBrief;              //21.修改个人介绍
	struct t_wb_keda_ams_list_account_by_str      keda_ams_list_account_by_str;      //22.按姓名模糊查找本企业联系人


    // 会议管理
    struct t_wb_keda_get_accounttoken             keda_meeting_get_accounttoken;         //1. 获得会议的accountToken
    struct t_wb_keda_login                        keda_m_login;                          //2. 登录
    struct t_wb_keda_keep_heartbeat               keda_m_keep_heartbeat;                 //3. 保持用户心跳

    struct t_wb_keda_m_meeting_list               keda_m_meeting_list;
    struct t_wb_keda_m_meeting_info               keda_m_meeting_info;
    struct t_wb_keda_m_booking_info               keda_m_booking_info;
    struct t_wb_keda_m_meeting_list_of_person    keda_m_meeting_list_person;

};

void set_parameter( WEIBORequest* pWBReq, pthreadpool p_threadpool, APIType type, bool bIsCustomCookie = true )
{
	loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_DEST_IP), HTTP_TEST_API_ADDR );
	loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_THREAD_POOL_HANDLE), p_threadpool );

	switch ( type )
	{
	case emPlatform_API:
		{
			loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_ACCOUNTTOKEN), g_szPlatformAccountToken );
			if ( true == bIsCustomCookie )
			{
				loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE), 123 );
				loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE_1), g_szPlatformCookieValue[0] );
				loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE_2), g_szPlatformCookieValue[1] );				
			}
			else
			{
				loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE), 2 );
			}
		}
		break;
	case emWeibo_API:
		{
			loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_ACCOUNTTOKEN), g_szWeiboAccountToken );
			if ( true == bIsCustomCookie )
			{
				loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE), 123 );
				loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE_1), g_szWeiboCookieValue[0] );
				loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE_2), g_szWeiboCookieValue[1] );				
			}
			else
			{
				loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE), 2 );
			}
		}
		break;
    case emMeeting_API:
        {
            loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_ACCOUNTTOKEN), g_szMeetingAccountToken );
            if ( true == bIsCustomCookie )
            {
                loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE), 123 );
                loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE_1), g_szMeetingCookieValue[0] );
                loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE_2), g_szMeetingCookieValue[1] );				
            }
            else
            {
                loWeibo_request_setopt( pWBReq, WEIBO_OPTION(REQ_COOKIE), 2 );
            }
        }
        break;
    }
	return;
}





void OnLogInfo( const char * szLog )
{
    printf( szLog );
}

int main(int argc, char* argv[])
{
	struct t_wb_allstruct twball;
	memset( &twball, 0, sizeof( struct t_wb_allstruct ) );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN) , &twball.keda_p_get_accounttoken );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN) , &twball.keda_w_get_accounttoken );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_P_LOGIN) , &twball.keda_p_login );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_W_LOGIN) , &twball.keda_w_login );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS) , &twball.keda_get_me_and_friends );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_USER_DATA) , &twball.keda_get_user_data );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA) , &twball.keda_get_user_all_data );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY) , &twball.keda_get_subject_and_reply );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH) , &twball.keda_message_publish );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_MESSAGE_REPLY) , &twball.keda_message_reply );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_USER_INFO) , &twball.keda_get_user_info );


	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA) , &twball.keda_get_company_public_data );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA) , &twball.keda_get_groups_data );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA) , &twball.keda_get_mentions_data );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA) , &twball.keda_get_replies_data );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_LIKES_DATA) , &twball.keda_get_likes_data );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA) , &twball.keda_get_favorites_data );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA) , &twball.keda_get_topics_data );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY) , &twball.keda_get_friendship_company );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION) , &twball.keda_get_friendship_attention );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS) , &twball.keda_get_friendship_followers );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS) , &twball.keda_get_friendship_friends );


	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE) , &twball.keda_friendship_create );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY) , &twball.keda_friendship_destroy );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_FAVORITES_CREATE) , &twball.keda_favorites_create );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY) , &twball.keda_favorites_destroy );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_TOPIC_CREATE) , &twball.keda_topic_create );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_TOPIC_DESTROY) , &twball.keda_topic_destroy );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_LIKE_CREATE) , &twball.keda_like_create );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_LIKE_DESTROY) , &twball.keda_like_destroy );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_GROUP_USER) , &twball.keda_get_group_user );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_GROUP_ALL) , &twball.keda_get_group_all );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GROUP_CREATE) , &twball.keda_group_create );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GROUP_JOIN) , &twball.keda_group_join );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GROUP_QUIT) , &twball.keda_group_quit );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST) , &twball.keda_get_privatemessage_list );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER) , &twball.keda_get_privatemessage_user );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND) , &twball.keda_privatemessage_send );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE) , &twball.keda_privatemessage_delete );


	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT) , &twball.keda_ams_validateClientAccount );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO) , &twball.keda_ams_getAccountInfo );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO) , &twball.keda_ams_updateAccountInfo );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD) , &twball.keda_ams_updatePassword );


	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL) , &twball.keda_get_department_all );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY) , &twball.keda_get_department_company );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER) , &twball.keda_get_department_user );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER) , &twball.keda_get_department_allUser );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT) , &twball.keda_department_searchDepartment );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER) , &twball.keda_department_searchUser );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID) , &twball.keda_department_online_by_depId );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID) , &twball.keda_department_online_by_userId );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_PORTRAIT) , &twball.keda_get_portrait );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO) , &twball.keda_ams_batchGetAccountInfo );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT) , &twball.keda_confirmPortrait );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION) , &twball.keda_department_get_version );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD) , &twball.keda_message_upload );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD) , &twball.keda_message_delete_thread );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY) , &twball.keda_message_delete_reply );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL) , &twball.keda_notify_get_all );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT) , &twball.keda_p_keep_heartbeat );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT) , &twball.keda_w_keep_heartbeat );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL) , &twball.keda_get_user_info_by_email );

	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO) , &twball.keda_get_current_user_info );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL) , &twball.keda_get_article_url );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF) , &twball.keda_ams_updateBrief );
	Wb_init_wb_struct( WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR) , &twball.keda_ams_list_account_by_str );


    // 会议管理
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN) , &twball.keda_meeting_get_accounttoken );
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_LOGIN) ,            &twball.keda_m_login );
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_KEEP_HEARTBEAT) ,   &twball.keda_m_keep_heartbeat );


    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_MEETING_LIST) ,     &twball.keda_m_meeting_list );
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_MEETING_INFO) ,     &twball.keda_m_meeting_info );
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_BOOKING_INFO) ,     &twball.keda_m_booking_info );
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_MEETING_LIST_OF_PERSON) ,     &twball.keda_m_meeting_list_person );
    
    

    struct t_wb_keda_m_set_meeting   t_m_add_meeting;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_ADD_MEETING) ,     &t_m_add_meeting );

    struct t_wb_keda_m_set_meeting   t_m_modify_meeting;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_MODIFY_MEETING) ,     &t_m_modify_meeting );

    struct t_wb_keda_m_meeting_id   t_m_del_meeting;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_DELETE_MEETING) ,     &t_m_del_meeting );

    struct t_wb_keda_m_meeting_id   t_m_start_meeting;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_START_MEETING) ,     &t_m_start_meeting );

    struct t_wb_keda_m_meeting_id   t_m_end_meeting;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_END_MEETING) ,     &t_m_end_meeting );

    struct t_wb_keda_m_meeting_feed t_m_meeting_feed;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_FEED) ,     &t_m_meeting_feed );

    struct t_wb_keda_m_query_rooms   t_m_query_free_rooms;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_QUERY_FREE_ROOMS) ,     &t_m_query_free_rooms );

    struct t_wb_keda_m_query_rooms   t_m_recommended_v_room;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_RECOMMENDED_V_ROOM) ,     &t_m_recommended_v_room );

    struct t_wb_keda_m_query_rooms   t_m_normal_rooms;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_NORMAL_ROOMS) ,     &t_m_normal_rooms );

    struct t_wb_keda_m_id            t_m_room_info;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_ROOM_INFO) ,     &t_m_room_info );

    struct t_wb_keda_m_id            t_m_room_status;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_ROOM_STATUS) ,   &t_m_room_status );

    struct t_wb_keda_m_query_rooms   t_m_query_room_by_name;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_ROOM_LIST_BY_NAME) ,   &t_m_query_room_by_name );

    struct t_wb_keda_m_query_rooms   t_m_query_rooms;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_QUERY_ROOMS) ,   &t_m_query_rooms );

    t_wb_keda_m_common_query        t_m_template_list;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_TEMPLATE_LIST) ,   &t_m_template_list );

    struct t_wb_keda_m_common_id    t_m_template_info;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_TEMPLATE_INFO) ,   &t_m_template_info );

    struct t_wb_keda_m_notify           t_m_notify;
    Wb_init_wb_struct( WEIBO_OPTION(KEDA_MEETING_NOTIFY) ,          &t_m_notify );



	pthreadpool tp_handle;
	tp_handle = thpool_init(1);

	getchar();


    

    SetKedaWeiBoLogCallback( ::OnLogInfo );

////////////////// 会议管理 API 



    WEIBORequest* pWBReq_m = loWeibo_request_create();
    loWeibo_request_setopt(pWBReq_m , WEIBO_OPTION(REQ_DEST_IP), HTTP_TEST_API_ADDR);   // 上层给出
    loWeibo_request_setopt(pWBReq_m , WEIBO_OPTION(REQ_COOKIE), 2);
    loWeibo_request_setopt(pWBReq_m , WEIBO_OPTION(REQ_THREAD_POOL_HANDLE), tp_handle);

    memcpy( twball.keda_meeting_get_accounttoken.wbauth_.oauth_token_,        "TrueLink", WB_REAL_LEN(OAUTH_TOKEN) );
    memcpy( twball.keda_meeting_get_accounttoken.wbauth_.oauth_token_secret_, "12345678", WB_REAL_LEN(OAUTH_TOKEN_SECRET) );

    fnKedaWeiBoAPI_m_get_accounttoken( pWBReq_m, pWBReq_m, &twball.keda_meeting_get_accounttoken );

    getchar();




    WEIBORequest* pWBReq_login_m = loWeibo_request_create();
    loWeibo_request_setopt(pWBReq_login_m , WEIBO_OPTION(REQ_DEST_IP), HTTP_TEST_API_ADDR);   // 上层给出
    loWeibo_request_setopt(pWBReq_login_m , WEIBO_OPTION(REQ_ACCOUNTTOKEN), g_szMeetingAccountToken );

    loWeibo_request_setopt(pWBReq_login_m , WEIBO_OPTION(REQ_COOKIE), 2);
    loWeibo_request_setopt(pWBReq_login_m , WEIBO_OPTION(REQ_THREAD_POOL_HANDLE) ,tp_handle);

    memcpy( twball.keda_m_login.username, TEST_ACCOUNT, WB_REAL_LEN(UID));
    memcpy( twball.keda_m_login.password, TEST_PASSWORD, WB_REAL_LEN(PWD));

    fnKedaWeiBoAPI_m_api_login(pWBReq_login_m, pWBReq_login_m, &twball.keda_m_login);

    getchar();

    //Sleep( 100 );
    //fnKedaWeiboDebugInfo();
    //getchar();

#if 0

    WEIBORequest* pWBReq_heart_m = loWeibo_request_create();
    set_parameter( pWBReq_heart_m, tp_handle, emMeeting_API );
    fnKedaWeiBoAPI_m_keep_heartBeat(pWBReq_heart_m, pWBReq_heart_m, &twball.keda_m_keep_heartbeat);
    getchar();



    WEIBORequest* pWBReq_m_list = loWeibo_request_create();
    set_parameter( pWBReq_m_list, tp_handle, emMeeting_API );

    strcpy( twball.keda_m_meeting_list.startDay, "2012-01-01 00:00:00" );
    strcpy( twball.keda_m_meeting_list.endDay,   "2012-12-01 00:00:00" );
    twball.keda_m_meeting_list.start = 1;
    twball.keda_m_meeting_list.count = 2;
    twball.keda_m_meeting_list.order = 1;

    fnKedaWeiBoAPI_m_meeting_list(pWBReq_m_list, pWBReq_m_list, &twball.keda_m_meeting_list);

    getchar();



    WEIBORequest* pWBReq_m_info = loWeibo_request_create();
    set_parameter( pWBReq_m_info, tp_handle, emMeeting_API );

    twball.keda_m_meeting_info.id = 100;
    
    fnKedaWeiBoAPI_m_meeting_info(pWBReq_m_info, pWBReq_m_info, &twball.keda_m_meeting_info);

    getchar();


    WEIBORequest* pWBReq_m_list_1 = loWeibo_request_create();
    set_parameter( pWBReq_m_list_1, tp_handle, emMeeting_API );

    strcpy( twball.keda_m_booking_info.startDay, "2012-01-01 00:00:00" );
    strcpy( twball.keda_m_booking_info.endDay,   "2012-12-01 00:00:00" );
    twball.keda_m_booking_info.start = 1;
    twball.keda_m_booking_info.count = 2;
    twball.keda_m_booking_info.order = 1;
    twball.keda_m_booking_info.meetingRoomId = 4;

    fnKedaWeiBoAPI_m_booking_info(pWBReq_m_list_1, pWBReq_m_list_1, &twball.keda_m_booking_info);

    getchar();


    WEIBORequest* pWBReq_m_list_2 = loWeibo_request_create();
    set_parameter( pWBReq_m_list_2, tp_handle, emMeeting_API );

    strcpy( twball.keda_m_meeting_list_person.startDay, "2012-01-01 00:00:00" );
    strcpy( twball.keda_m_meeting_list_person.endDay,   "2012-12-01 00:00:00" );
    twball.keda_m_meeting_list_person.start = 1;
    twball.keda_m_meeting_list_person.count = 2;
    twball.keda_m_meeting_list_person.order = 1;
    strcpy( twball.keda_m_meeting_list_person.moId, "keda@keda.com" );

    fnKedaWeiBoAPI_m(pWBReq_m_list_2, WEIBO_OPTION(KEDA_MEETING_MEETING_LIST_OF_PERSON),  &twball.keda_m_meeting_list_person);

    getchar();



    WEIBORequest* pWBReq_m_add = loWeibo_request_create();
    set_parameter( pWBReq_m_add, tp_handle, emMeeting_API );

    strcpy( t_m_add_meeting.brief, "test1..." );
    strcpy( t_m_add_meeting.day, "2012-12-03" );
    strcpy( t_m_add_meeting.endShortTime,   "10:00" );
    strcpy( t_m_add_meeting.startShortTime, "09:00" );
    t_m_add_meeting.isAutoCall = 1;
    t_m_add_meeting.isVideoMeeting = 0;
    strcpy( t_m_add_meeting.mobile, "13400001111");
    t_m_add_meeting.notify_type = MEETING_NOTIFY_TYPE_WEIBO | MEETING_NOTIFY_TYPE_EMAIL;
    t_m_add_meeting.participant_cnt = 2;
    strcpy( t_m_add_meeting.participants[0], "keda@keda.com" );
    strcpy( t_m_add_meeting.participants[1], "keda10@keda.com" );
    strcpy( t_m_add_meeting.phone, "021-111");
    t_m_add_meeting.rooms_cnt = 1;
    t_m_add_meeting.rooms[0] = 4;
    // t_m_add_meeting.rooms[1] = 4;
    strcpy( t_m_add_meeting.subject, "测试" );
    strcpy( t_m_add_meeting.template_id, "0099#0001" );
    t_m_add_meeting.vroom_id = 0;

    fnKedaWeiBoAPI_m(pWBReq_m_add, WEIBO_OPTION(KEDA_MEETING_ADD_MEETING),  &t_m_add_meeting);

    getchar();




    WEIBORequest* pWBReq_m_modify = loWeibo_request_create();
    set_parameter( pWBReq_m_modify, tp_handle, emMeeting_API );

    t_m_modify_meeting.meetingId = 119;

    strcpy( t_m_modify_meeting.brief, "test1..." );
    strcpy( t_m_modify_meeting.day, "2012-12-04" );
    strcpy( t_m_modify_meeting.endShortTime,   "10:00" );
    strcpy( t_m_modify_meeting.startShortTime, "09:00" );
    t_m_modify_meeting.isAutoCall = 1;
    t_m_modify_meeting.isVideoMeeting = 0;
    strcpy( t_m_modify_meeting.mobile, "13400001111");
    t_m_modify_meeting.notify_type = MEETING_NOTIFY_TYPE_WEIBO | MEETING_NOTIFY_TYPE_EMAIL;
    t_m_modify_meeting.participant_cnt = 2;
    strcpy( t_m_modify_meeting.participants[0], "keda@keda.com" );
    strcpy( t_m_modify_meeting.participants[1], "keda10@keda.com" );
    strcpy( t_m_modify_meeting.phone, "021-111");
    t_m_modify_meeting.rooms_cnt = 1;
    t_m_modify_meeting.rooms[0] = 4;
    // t_m_add_meeting.rooms[1] = 4;
    strcpy( t_m_modify_meeting.subject, "测试" );
    strcpy( t_m_modify_meeting.template_id, "0099#0001" );
    t_m_modify_meeting.vroom_id = 0;

    fnKedaWeiBoAPI_m(pWBReq_m_modify, WEIBO_OPTION(KEDA_MEETING_MODIFY_MEETING),  &t_m_modify_meeting);

    getchar();


    WEIBORequest* pWBReq_m_del = loWeibo_request_create();
    set_parameter( pWBReq_m_del, tp_handle, emMeeting_API );

    t_m_del_meeting.meetingId = 119;

    fnKedaWeiBoAPI_m(pWBReq_m_del, WEIBO_OPTION(KEDA_MEETING_DELETE_MEETING),  &t_m_del_meeting);

    getchar();


    WEIBORequest* pWBReq_m_start = loWeibo_request_create();
    set_parameter( pWBReq_m_start, tp_handle, emMeeting_API );

    t_m_start_meeting.meetingId = 118;

    fnKedaWeiBoAPI_m(pWBReq_m_start, WEIBO_OPTION(KEDA_MEETING_START_MEETING),  &t_m_start_meeting);

    getchar();


    WEIBORequest* pWBReq_m_end = loWeibo_request_create();
    set_parameter( pWBReq_m_end, tp_handle, emMeeting_API );

    t_m_end_meeting.meetingId = 118;

    fnKedaWeiBoAPI_m(pWBReq_m_end, WEIBO_OPTION(KEDA_MEETING_END_MEETING),  &t_m_end_meeting);

    getchar();

    
    WEIBORequest* pWBReq_m_feed = loWeibo_request_create();
    set_parameter( pWBReq_m_feed, tp_handle, emMeeting_API );

    t_m_meeting_feed.meetingId = 117;
    t_m_meeting_feed.accept    = 1;
    t_m_meeting_feed.joinType  = JOIN_TYPE_HARDWARE;

    fnKedaWeiBoAPI_m(pWBReq_m_feed, WEIBO_OPTION(KEDA_MEETING_FEED),  &t_m_meeting_feed);

    getchar();


    WEIBORequest* pWBReq_m_query_rooms = loWeibo_request_create();
    set_parameter( pWBReq_m_query_rooms, tp_handle, emMeeting_API );

    t_m_query_free_rooms.rooms_cnt = 2;
    t_m_query_free_rooms.rooms[0] = 4;
    t_m_query_free_rooms.rooms[1] = 1;

    strcpy( t_m_query_free_rooms.startTime,   "2012-12-31 09:00" );
    strcpy( t_m_query_free_rooms.endTime,     "2013-12-31 10:00" );

    t_m_query_free_rooms.start = 0;
    t_m_query_free_rooms.count = 2;
    t_m_query_free_rooms.order = 0;

    t_m_query_free_rooms.peopleAdmit = 1;
    t_m_query_free_rooms.deviceIdMask = 0; //DEVICE_ID_AIR_CONDITIONING | DEVICE_ID_VIDEO_TERMINAL;


    fnKedaWeiBoAPI_m(pWBReq_m_query_rooms, WEIBO_OPTION(KEDA_MEETING_QUERY_FREE_ROOMS),  &t_m_query_free_rooms);

    getchar();

    WEIBORequest* pWBReq_m_recommended = loWeibo_request_create();
    set_parameter( pWBReq_m_recommended, tp_handle, emMeeting_API );

    strcpy( t_m_recommended_v_room.startTime,   "2012-12-31 09:00" );
    strcpy( t_m_recommended_v_room.endTime,     "2013-12-31 10:00" );

    fnKedaWeiBoAPI_m(pWBReq_m_recommended, WEIBO_OPTION(KEDA_MEETING_RECOMMENDED_V_ROOM),  &t_m_recommended_v_room);

    getchar();

    WEIBORequest* pWBReq_m_normal = loWeibo_request_create();
    set_parameter( pWBReq_m_normal, tp_handle, emMeeting_API );

    t_m_normal_rooms.start = 0;
    t_m_normal_rooms.count = 2;
    t_m_normal_rooms.order = 0;

    fnKedaWeiBoAPI_m(pWBReq_m_normal, WEIBO_OPTION(KEDA_MEETING_NORMAL_ROOMS),  &t_m_normal_rooms);

    getchar();
    
    WEIBORequest* pWBReq_m_room_info = loWeibo_request_create();
    set_parameter( pWBReq_m_room_info, tp_handle, emMeeting_API );

    t_m_room_info.id = 4;

    fnKedaWeiBoAPI_m(pWBReq_m_room_info, WEIBO_OPTION(KEDA_MEETING_ROOM_INFO),  &t_m_room_info);

    getchar();


    WEIBORequest* pWBReq_m_room_status = loWeibo_request_create();
    set_parameter( pWBReq_m_room_status, tp_handle, emMeeting_API );

    t_m_room_status.id = 4;

    fnKedaWeiBoAPI_m(pWBReq_m_room_status, WEIBO_OPTION(KEDA_MEETING_ROOM_STATUS),  &t_m_room_status);

    getchar();

    
    WEIBORequest* pWBReq_m_rooms_byname = loWeibo_request_create();
    set_parameter( pWBReq_m_rooms_byname, tp_handle, emMeeting_API );

    strcpy( t_m_query_room_by_name.name, "上海" );
    t_m_query_room_by_name.start = 0;
    t_m_query_room_by_name.count = 2;
    t_m_query_room_by_name.order = 0;

    fnKedaWeiBoAPI_m(pWBReq_m_rooms_byname, WEIBO_OPTION(KEDA_MEETING_ROOM_LIST_BY_NAME),  &t_m_query_room_by_name);

    getchar();

    
    WEIBORequest* pWBReq_m_rooms = loWeibo_request_create();
    set_parameter( pWBReq_m_rooms, tp_handle, emMeeting_API );

    t_m_query_rooms.peopleAdmit = 4;
    t_m_query_rooms.deviceIdMask = DEVICE_ID_VIDEO_TERMINAL | DEVICE_ID_AIR_CONDITIONING;
    t_m_query_rooms.start = 0;
    t_m_query_rooms.count = 2;
    t_m_query_rooms.order = 0;

    fnKedaWeiBoAPI_m(pWBReq_m_rooms, WEIBO_OPTION(KEDA_MEETING_QUERY_ROOMS),  &t_m_query_rooms);

    getchar();

    WEIBORequest* pWBReq_m_templates = loWeibo_request_create();
    set_parameter( pWBReq_m_templates, tp_handle, emMeeting_API );

    t_m_template_list.start = 0;
    t_m_template_list.count = 2;
    t_m_template_list.order = 0;

    fnKedaWeiBoAPI_m(pWBReq_m_templates, WEIBO_OPTION(KEDA_MEETING_TEMPLATE_LIST),  &t_m_template_list);

    getchar();



    WEIBORequest* pWBReq_m_template_info = loWeibo_request_create();
    set_parameter( pWBReq_m_template_info, tp_handle, emMeeting_API );

    strcpy( t_m_template_info.id, "444555#999986" );

    fnKedaWeiBoAPI_m(pWBReq_m_template_info, WEIBO_OPTION(KEDA_MEETING_TEMPLATE_INFO),  &t_m_template_info);

    getchar();


    WEIBORequest* pWBReq_m_notify = loWeibo_request_create();
    set_parameter( pWBReq_m_notify, tp_handle, emMeeting_API );

    t_m_notify.id = 120;
    t_m_notify.notifyMask = MEETING_NOTIFY_TYPE_WEIBO | MEETING_NOTIFY_TYPE_EMAIL;

    fnKedaWeiBoAPI_m(pWBReq_m_notify, WEIBO_OPTION(KEDA_MEETING_NOTIFY),  &t_m_notify);

    getchar();

#endif

////////////////// 结束 ( 会议管理 API )

    

#if 0
// ==============================================================



	//29.验证终端账号  -- 119

	WEIBORequest* pWBReq_38 = loWeibo_request_create();
	loWeibo_request_setopt(pWBReq_38 , WEIBO_OPTION(REQ_DEST_IP), HTTP_TEST_API_ADDR);   // 上层给出
	loWeibo_request_setopt(pWBReq_38 , WEIBO_OPTION(REQ_COOKIE), 2);

	loWeibo_request_setopt(pWBReq_38 , WEIBO_OPTION(REQ_THREAD_POOL_HANDLE), tp_handle);

	memcpy( twball.keda_ams_validateClientAccount.wbauth_.oauth_token_, "TrueLink", WB_REAL_LEN(OAUTH_TOKEN) );
	memcpy( twball.keda_ams_validateClientAccount.wbauth_.oauth_token_secret_, "12345678", WB_REAL_LEN(OAUTH_TOKEN_SECRET) );

	// memcpy( twball.keda_ams_validateClientAccount.username, "sunfei@kedacom.com", WB_REAL_LEN(UID));
    memcpy( twball.keda_ams_validateClientAccount.username, TEST_ACCOUNT, WB_REAL_LEN(UID));
    
	// memcpy( twball.keda_ams_validateClientAccount.password, "888888", WB_REAL_LEN(PWD));
    memcpy( twball.keda_ams_validateClientAccount.password, TEST_PASSWORD, WB_REAL_LEN(PWD));

	memcpy( twball.keda_ams_validateClientAccount.deviceType, "TrueLink", WB_REAL_LEN(OPT_VALUE));
	memcpy( twball.keda_ams_validateClientAccount.apiHost, TEST_API_ADDR, WB_REAL_LEN(IP));

	fnKedaWeiBoAPI_ams_validateclientaccount(pWBReq_38, pWBReq_38, &twball.keda_ams_validateClientAccount);

	getchar();


// ==============================================================

	//1.Oauth的accountToken的获得(平台API)  -- 82

	WEIBORequest* pWBReq_p = loWeibo_request_create();
	loWeibo_request_setopt(pWBReq_p , WEIBO_OPTION(REQ_DEST_IP), HTTP_TEST_API_ADDR);   // 上层给出
	loWeibo_request_setopt(pWBReq_p , WEIBO_OPTION(REQ_COOKIE), 2);

	loWeibo_request_setopt(pWBReq_p , WEIBO_OPTION(REQ_THREAD_POOL_HANDLE), tp_handle);

	memcpy( twball.keda_p_get_accounttoken.wbauth_.oauth_token_, "TrueLink", WB_REAL_LEN(OAUTH_TOKEN) );
	memcpy( twball.keda_p_get_accounttoken.wbauth_.oauth_token_secret_, "12345678", WB_REAL_LEN(OAUTH_TOKEN_SECRET) );

	fnKedaWeiBoAPI_p_get_accounttoken(pWBReq_p, pWBReq_p, &twball.keda_p_get_accounttoken);

	getchar();

// ==============================================================

	//1.Oauth的accountToken的获得(微博API)  -- 82

	WEIBORequest* pWBReq_w = loWeibo_request_create();
	loWeibo_request_setopt(pWBReq_w , WEIBO_OPTION(REQ_DEST_IP), HTTP_TEST_API_ADDR);   // 上层给出
	loWeibo_request_setopt(pWBReq_w , WEIBO_OPTION(REQ_COOKIE), 2);

	loWeibo_request_setopt(pWBReq_w , WEIBO_OPTION(REQ_THREAD_POOL_HANDLE) ,tp_handle);

	memcpy( twball.keda_w_get_accounttoken.wbauth_.oauth_token_, "TrueLink", WB_REAL_LEN(OAUTH_TOKEN) );
	memcpy( twball.keda_w_get_accounttoken.wbauth_.oauth_token_secret_, "12345678", WB_REAL_LEN(OAUTH_TOKEN_SECRET) );

	fnKedaWeiBoAPI_w_get_accounttoken(pWBReq_w, pWBReq_w, &twball.keda_w_get_accounttoken);

	getchar();

// ==============================================================

	//2.登录平台API  -- 83

	WEIBORequest* pWBReq_login_p = loWeibo_request_create();
	loWeibo_request_setopt(pWBReq_login_p , WEIBO_OPTION(REQ_DEST_IP), HTTP_TEST_API_ADDR);   // 上层给出
	loWeibo_request_setopt(pWBReq_login_p , WEIBO_OPTION(REQ_ACCOUNTTOKEN), g_szPlatformAccountToken);

	loWeibo_request_setopt(pWBReq_login_p , WEIBO_OPTION(REQ_COOKIE), 2);
	loWeibo_request_setopt(pWBReq_login_p , WEIBO_OPTION(REQ_THREAD_POOL_HANDLE) ,tp_handle);

	memcpy( twball.keda_p_login.username, TEST_ACCOUNT, WB_REAL_LEN(UID));
	memcpy( twball.keda_p_login.password, TEST_PASSWORD, WB_REAL_LEN(PWD));

	fnKedaWeiBoAPI_p_api_login(pWBReq_login_p, pWBReq_login_p, &twball.keda_p_login);

	getchar();


// ==============================================================

	//2.登录微博API  -- 83

	WEIBORequest* pWBReq_login_w = loWeibo_request_create();
	loWeibo_request_setopt(pWBReq_login_w , WEIBO_OPTION(REQ_DEST_IP), HTTP_TEST_API_ADDR);
	loWeibo_request_setopt(pWBReq_login_w , WEIBO_OPTION(REQ_ACCOUNTTOKEN), g_szWeiboAccountToken);

	loWeibo_request_setopt(pWBReq_login_w , WEIBO_OPTION(REQ_COOKIE) ,2);
	loWeibo_request_setopt(pWBReq_login_w , WEIBO_OPTION(REQ_THREAD_POOL_HANDLE) ,tp_handle);

	memcpy( twball.keda_w_login.username, TEST_ACCOUNT, WB_REAL_LEN(UID));
	memcpy( twball.keda_w_login.password, TEST_PASSWORD, WB_REAL_LEN(PWD));

	fnKedaWeiBoAPI_w_api_login(pWBReq_login_w, pWBReq_login_w, &twball.keda_w_login);

	getchar();

// ==============================================================

	//3.获得当前登录用户及关注用户的微博  -- 84

	WEIBORequest* pWBReq_3 = loWeibo_request_create();
	set_parameter( pWBReq_3, tp_handle, emWeibo_API );

	twball.keda_get_me_and_friends.since_id = -1;
	twball.keda_get_me_and_friends.max_id = 65535;
	twball.keda_get_me_and_friends.count = 20;
	twball.keda_get_me_and_friends.reply_count = 10;
	twball.keda_get_me_and_friends.reply_style = 0;

	fnKedaWeiBoAPI_get_me_and_friends(pWBReq_3, pWBReq_3, &twball.keda_get_me_and_friends);

	getchar();

// ==============================================================

	//4.获得用户发布的微博数据（友讯）  -- 85

	WEIBORequest* pWBReq_4 = loWeibo_request_create();
	set_parameter( pWBReq_4, tp_handle, emWeibo_API );

	twball.keda_get_user_data.since_id = 20;
	twball.keda_get_user_data.max_id = 65535;
	twball.keda_get_user_data.count = 20;
	twball.keda_get_user_data.reply_count = 10;
	twball.keda_get_user_data.reply_style = 0;

	fnKedaWeiBoAPI_get_user_data(pWBReq_4, pWBReq_4, &twball.keda_get_user_data);

	getchar();

// ==============================================================

	//5.获得用户发布的微博数据（全部）  -- 86

	WEIBORequest* pWBReq_5 = loWeibo_request_create();
	set_parameter( pWBReq_5, tp_handle, emWeibo_API );

	twball.keda_get_user_all_data.since_id = -1;
	twball.keda_get_user_all_data.max_id = 65535;
	twball.keda_get_user_all_data.count = 20;
	twball.keda_get_user_all_data.reply_count = 10;
	twball.keda_get_user_all_data.reply_style = 0;

	memcpy( twball.keda_get_user_all_data.uid, "sunfei", WB_REAL_LEN(UID));

	fnKedaWeiBoAPI_get_user_all_data(pWBReq_5, pWBReq_5, &twball.keda_get_user_all_data);

// ==============================================================
 
	//6.获得一条主题以及回复  -- 87

	WEIBORequest* pWBReq_6 = loWeibo_request_create();
	set_parameter( pWBReq_6, tp_handle, emWeibo_API );

	twball.keda_get_subject_and_reply.since_id = -1;
	twball.keda_get_subject_and_reply.max_id = 65535;
	twball.keda_get_subject_and_reply.count = 20;
	twball.keda_get_subject_and_reply.reply_style = 0;

	twball.keda_get_subject_and_reply.thread_id = 123;

	fnKedaWeiBoAPI_get_subject_and_reply(pWBReq_6, pWBReq_6, &twball.keda_get_subject_and_reply);

// ==============================================================
 
	//7.发布一条微博  -- 88

	WEIBORequest* pWBReq_7 = loWeibo_request_create();
	set_parameter( pWBReq_7, tp_handle, /*emWeibo_API*/emPlatform_API );

	twball.keda_message_publish.group_id = 11;
	memcpy( twball.keda_message_publish.status, "当前微博为来自REST DEMO的--发布测试，请忽略", WB_REAL_LEN(WB_INFO));

	fnKedaWeiBoAPI_message_publish(pWBReq_7, pWBReq_7, &twball.keda_message_publish);

	getchar();
	getchar();

// ==============================================================

	//8.回复一条微博  -- 89

	WEIBORequest* pWBReq_8 = loWeibo_request_create();
	set_parameter( pWBReq_8, tp_handle, emWeibo_API );

	twball.keda_message_reply.thread_id = 4579;
	twball.keda_message_reply.reply_id = 31652;
	memcpy( twball.keda_message_reply.comment, "当前微博为来自REST DEMO的--回复测试，请忽略", WB_REAL_LEN(WB_INFO));

	fnKedaWeiBoAPI_message_reply(pWBReq_8, pWBReq_8, &twball.keda_message_reply);

	getchar();

// ==============================================================

	//9.获得用户信息  -- 90

	WEIBORequest* pWBReq_9 = loWeibo_request_create();
	set_parameter( pWBReq_9, tp_handle, emWeibo_API );

	memcpy( twball.keda_get_user_info.uid, "sunfei", WB_REAL_LEN(UID));

	fnKedaWeiBoAPI_get_user_info(pWBReq_9, pWBReq_9, &twball.keda_get_user_info);

	getchar();

// ==============================================================

	//1.获得公司广场的微博数据  -- 91

	WEIBORequest* pWBReq_10 = loWeibo_request_create();
	set_parameter( pWBReq_10, tp_handle, emWeibo_API );

	twball.keda_get_company_public_data.since_id = -1;
	twball.keda_get_company_public_data.max_id = 65535;
	twball.keda_get_company_public_data.count = 20;
	twball.keda_get_company_public_data.reply_count = 10;
	twball.keda_get_company_public_data.reply_style = 0;

	fnKedaWeiBoAPI_get_company_public_data(pWBReq_10, pWBReq_10, &twball.keda_get_company_public_data);

	getchar();

// ==============================================================

	//2.获得群组的微博数据  -- 92

	WEIBORequest* pWBReq_11 = loWeibo_request_create();
	set_parameter( pWBReq_11, tp_handle, emWeibo_API );

	twball.keda_get_groups_data.since_id = -1;
	twball.keda_get_groups_data.max_id = 65535;
	twball.keda_get_groups_data.count = 20;
	twball.keda_get_groups_data.reply_count = 10;
	twball.keda_get_groups_data.reply_style = 0;

	twball.keda_get_groups_data.group_id = 10;

	fnKedaWeiBoAPI_get_groups_data(pWBReq_11, pWBReq_11, &twball.keda_get_groups_data);

	getchar();

// ==============================================================

	//3.获得@当前用户的微博数据  -- 93

	WEIBORequest* pWBReq_12 = loWeibo_request_create();
	set_parameter( pWBReq_12, tp_handle, emWeibo_API );

	twball.keda_get_mentions_data.since_id = -1;
	twball.keda_get_mentions_data.max_id = 65535;
	twball.keda_get_mentions_data.count = 20;
	twball.keda_get_mentions_data.reply_count = 10;
	twball.keda_get_mentions_data.reply_style = 0;

	fnKedaWeiBoAPI_get_mentions_data(pWBReq_12, pWBReq_12, &twball.keda_get_mentions_data);

// ==============================================================

	//4.获得回复我的微博数据  -- 94

	WEIBORequest* pWBReq_13 = loWeibo_request_create();
	set_parameter( pWBReq_13, tp_handle, emWeibo_API );

	twball.keda_get_replies_data.since_id = -1;
	twball.keda_get_replies_data.max_id = 65535;
	twball.keda_get_replies_data.count = 20;
	twball.keda_get_replies_data.reply_count = 10;
	twball.keda_get_replies_data.reply_style = 0;

	fnKedaWeiBoAPI_get_replies_data(pWBReq_13, pWBReq_13, &twball.keda_get_replies_data);

// ==============================================================
	
	//5.获得我赞过的微博数据  -- 95

	WEIBORequest* pWBReq_14 = loWeibo_request_create();
	set_parameter( pWBReq_14, tp_handle, emWeibo_API );

	twball.keda_get_likes_data.since_id = -1;
	twball.keda_get_likes_data.max_id = 65535;
	twball.keda_get_likes_data.count = 20;
	twball.keda_get_likes_data.reply_count = 10;
	twball.keda_get_likes_data.reply_style = 0;

	memcpy( twball.keda_get_likes_data.uid, "sunfei", WB_REAL_LEN(UID));

	fnKedaWeiBoAPI_get_likes_data(pWBReq_14, pWBReq_14, &twball.keda_get_likes_data);

// ==============================================================
	
	//6.获得我收藏的微博数据  -- 96

	WEIBORequest* pWBReq_15 = loWeibo_request_create();
	set_parameter( pWBReq_15, tp_handle, emWeibo_API );

	twball.keda_get_favorites_data.since_id = -1;
	twball.keda_get_favorites_data.max_id = 65535;
	twball.keda_get_favorites_data.count = 20;
	twball.keda_get_favorites_data.reply_count = 10;
	twball.keda_get_favorites_data.reply_style = 0;

	fnKedaWeiBoAPI_get_favorites_data(pWBReq_15, pWBReq_15, &twball.keda_get_favorites_data);

// ==============================================================
	
	//7.获得某个话题的微博数据  -- 97

	WEIBORequest* pWBReq_16 = loWeibo_request_create();
	set_parameter( pWBReq_16, tp_handle, emWeibo_API );

	twball.keda_get_topics_data.since_id = -1;
	twball.keda_get_topics_data.max_id = 65535;
	twball.keda_get_topics_data.count = 20;
	twball.keda_get_topics_data.reply_count = 10;
	twball.keda_get_topics_data.reply_style = 0;

	twball.keda_get_topics_data.topic_id = 629;

	fnKedaWeiBoAPI_get_topics_data(pWBReq_16, pWBReq_16, &twball.keda_get_topics_data);

// ==============================================================
	
	//8.获得公司人脉  -- 98

	WEIBORequest* pWBReq_17 = loWeibo_request_create();
	set_parameter( pWBReq_17, tp_handle, emWeibo_API );

	twball.keda_get_friendship_company.current_page = 1;
	twball.keda_get_friendship_company.page_size = 20;
// 	memcpy( twball.keda_get_friendship_company.key, "sunfei", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_get_friendship_company(pWBReq_17, pWBReq_17, &twball.keda_get_friendship_company);

// ==============================================================
	
	//9.获得我的关注列表  -- 99

	WEIBORequest* pWBReq_18 = loWeibo_request_create();
	set_parameter( pWBReq_18, tp_handle, emWeibo_API );

	twball.keda_get_friendship_attention.current_page = 1;
	twball.keda_get_friendship_attention.page_size = 20;

	memcpy( twball.keda_get_friendship_attention.uid, "sunfei", WB_REAL_LEN(UID));
// 	memcpy( twball.keda_get_friendship_attention.key, "sunfei", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_get_friendship_attention(pWBReq_18, pWBReq_18, &twball.keda_get_friendship_attention);

// ==============================================================

	//10.获得我的粉丝列表  -- 100

	WEIBORequest* pWBReq_19 = loWeibo_request_create();
	set_parameter( pWBReq_19, tp_handle, emWeibo_API );

	twball.keda_get_friendship_followers.current_page = 1;
	twball.keda_get_friendship_followers.page_size = 20;

	memcpy( twball.keda_get_friendship_followers.uid, "sunfei", WB_REAL_LEN(UID));
// 	memcpy( twball.keda_get_friendship_followers.key, "sunfei", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_get_friendship_followers(pWBReq_19, pWBReq_19, &twball.keda_get_friendship_followers);

// ==============================================================

	//11.获得我的好友列表  -- 101

	WEIBORequest* pWBReq_20 = loWeibo_request_create();
	set_parameter( pWBReq_20, tp_handle, emWeibo_API );

	twball.keda_get_friendship_friends.current_page = 1;
	twball.keda_get_friendship_friends.page_size = 20;

	memcpy( twball.keda_get_friendship_friends.uid, "sunfei", WB_REAL_LEN(UID));
// 	memcpy( twball.keda_get_friendship_friends.key, "sunfei", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_get_friendship_friends(pWBReq_20, pWBReq_20, &twball.keda_get_friendship_friends);

// ==============================================================

	//12.添加关注  -- 102

	WEIBORequest* pWBReq_21 = loWeibo_request_create();
	set_parameter( pWBReq_21, tp_handle, emWeibo_API );

	memcpy( twball.keda_friendship_create.uid, "huangchunhua", WB_REAL_LEN(UID));

	fnKedaWeiBoAPI_friendship_create(pWBReq_21, pWBReq_21, &twball.keda_friendship_create);

// ==============================================================

	//13.取消关注  -- 103

	WEIBORequest* pWBReq_22 = loWeibo_request_create();
	set_parameter( pWBReq_22, tp_handle, emWeibo_API );

	memcpy( twball.keda_friendship_destroy.uid, "huangchunhua", WB_REAL_LEN(UID));

	fnKedaWeiBoAPI_friendship_destroy(pWBReq_22, pWBReq_22, &twball.keda_friendship_destroy);

// ==============================================================

	//14.添加收藏  -- 104

	WEIBORequest* pWBReq_23 = loWeibo_request_create();
	set_parameter( pWBReq_23, tp_handle, emWeibo_API );

	twball.keda_favorites_create.thread_id = 4579;

	fnKedaWeiBoAPI_favorites_create(pWBReq_23, pWBReq_23, &twball.keda_favorites_create);

// ==============================================================

	//15.取消收藏  -- 105

	WEIBORequest* pWBReq_24 = loWeibo_request_create();
	set_parameter( pWBReq_24, tp_handle, emWeibo_API );

	twball.keda_favorites_destroy.thread_id = 4579;

	fnKedaWeiBoAPI_favorites_destroy(pWBReq_24, pWBReq_24, &twball.keda_favorites_destroy);

// ==============================================================

	//16.添加话题  -- 106

	WEIBORequest* pWBReq_25 = loWeibo_request_create();
	set_parameter( pWBReq_25, tp_handle, emWeibo_API );

	twball.keda_topic_create.thread_id = 4579;
	memcpy( twball.keda_topic_create.topic_name, "爱情", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_topic_create(pWBReq_25, pWBReq_25, &twball.keda_topic_create);

// ==============================================================

	//17.取消话题  -- 107

	WEIBORequest* pWBReq_26 = loWeibo_request_create();
	set_parameter( pWBReq_26, tp_handle, emWeibo_API );

	twball.keda_topic_destroy.thread_id = 4579;
	twball.keda_topic_destroy.topic_id = 752;

	fnKedaWeiBoAPI_topic_destroy(pWBReq_26, pWBReq_26, &twball.keda_topic_destroy);

// ==============================================================

	//18.添加like  -- 108

	WEIBORequest* pWBReq_27 = loWeibo_request_create();
	set_parameter( pWBReq_27, tp_handle, emWeibo_API );

	twball.keda_like_create.thread_id = 4579;
	twball.keda_like_create.feed_id = 26910;

	fnKedaWeiBoAPI_like_create(pWBReq_27, pWBReq_27, &twball.keda_like_create);

// ==============================================================

	//19.取消like  -- 109

	WEIBORequest* pWBReq_28 = loWeibo_request_create();
	set_parameter( pWBReq_28, tp_handle, emWeibo_API );

	twball.keda_like_destroy.thread_id = 4579;
	twball.keda_like_destroy.feed_id = 26910;

	fnKedaWeiBoAPI_like_destroy(pWBReq_28, pWBReq_28, &twball.keda_like_destroy);

// ==============================================================

	//20.获取我的群组列表  -- 110

	WEIBORequest* pWBReq_29 = loWeibo_request_create();
	set_parameter( pWBReq_29, tp_handle, emWeibo_API );

	twball.keda_get_group_user.current_page = 1;
	twball.keda_get_group_user.page_size = 20;
// 	memcpy( twball.keda_get_group_user.key, "sunfei", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_get_group_user(pWBReq_29, pWBReq_29, &twball.keda_get_group_user);

// ==============================================================

	//21.获得所有的群组列表  -- 111

	WEIBORequest* pWBReq_30 = loWeibo_request_create();
	set_parameter( pWBReq_30, tp_handle, emWeibo_API );

	twball.keda_get_group_all.current_page = 1;
	twball.keda_get_group_all.page_size = 20;
// 	memcpy( twball.keda_get_group_all.key, "sunfei", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_get_group_all(pWBReq_30, pWBReq_30, &twball.keda_get_group_all);

// ==============================================================

	//22.添加群操作  -- 112

	WEIBORequest* pWBReq_31 = loWeibo_request_create();
	set_parameter( pWBReq_31, tp_handle, emWeibo_API );

	memcpy( twball.keda_group_create.group_name, "爱情", WB_REAL_LEN(OPT_VALUE));
	memcpy( twball.keda_group_create.group_briefly, "友谊地久天长", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_group_create(pWBReq_31, pWBReq_31, &twball.keda_group_create);

// ==============================================================

	//23.加入群操作  -- 113

	WEIBORequest* pWBReq_32 = loWeibo_request_create();
	set_parameter( pWBReq_32, tp_handle, emWeibo_API );

	memcpy( twball.keda_group_join.guid, "100618", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_group_join(pWBReq_32, pWBReq_32, &twball.keda_group_join);

// ==============================================================

	//24.退出群操作  -- 114

	WEIBORequest* pWBReq_33 = loWeibo_request_create();
	set_parameter( pWBReq_33, tp_handle, emWeibo_API );

	memcpy( twball.keda_group_quit.guid, "100618", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_group_quit(pWBReq_33, pWBReq_33, &twball.keda_group_quit);

// ==============================================================

	//25.查看我的私信列表  -- 115

	WEIBORequest* pWBReq_34 = loWeibo_request_create();
	set_parameter( pWBReq_34, tp_handle, emWeibo_API );

	twball.keda_get_privatemessage_list.current_page = 1;
	twball.keda_get_privatemessage_list.page_size = 20;

	fnKedaWeiBoAPI_get_privatemessage_list(pWBReq_34, pWBReq_34, &twball.keda_get_privatemessage_list);

// ==============================================================

	//26.查看我与某人的私信的聊天记录  -- 116

	WEIBORequest* pWBReq_35 = loWeibo_request_create();
	set_parameter( pWBReq_35, tp_handle, emWeibo_API );

	twball.keda_get_privatemessage_user.current_page = 1;
	twball.keda_get_privatemessage_user.page_size = 20;
	memcpy( twball.keda_get_privatemessage_user.uid, "huangchunhua", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_get_privatemessage_user(pWBReq_35, pWBReq_35, &twball.keda_get_privatemessage_user);

// ==============================================================

	//27.发送私信到某人  -- 117

	WEIBORequest* pWBReq_36 = loWeibo_request_create();
	set_parameter( pWBReq_36, tp_handle, emWeibo_API );

	memcpy( twball.keda_privatemessage_send.uid, "huangchunhua", WB_REAL_LEN(UID));
	memcpy( twball.keda_privatemessage_send.body, "骑白马的一定是王子么", WB_REAL_LEN(PM_INFO));

	fnKedaWeiBoAPI_privatemessage_send(pWBReq_36, pWBReq_36, &twball.keda_privatemessage_send);

// ==============================================================

	//28.删除私信  -- 118

	WEIBORequest* pWBReq_37 = loWeibo_request_create();
	set_parameter( pWBReq_37, tp_handle, emWeibo_API );

	twball.keda_privatemessage_delete.id = 971;

	fnKedaWeiBoAPI_privatemessage_delete(pWBReq_37, pWBReq_37, &twball.keda_privatemessage_delete);

// ==============================================================

	//29.验证终端账号  -- 119

// 	WEIBORequest* pWBReq_38 = loWeibo_request_create();
// 	loWeibo_request_setopt(pWBReq_38 , WEIBO_OPTION(REQ_PLATFORM_API_IP), "http://10.31.3.2");
// 	loWeibo_request_setopt(pWBReq_38 , WEIBO_OPTION(REQ_WEIBO_API_IP), "http://10.31.3.2");
// 	loWeibo_request_setopt(pWBReq_38 , WEIBO_OPTION(REQ_COOKIE) ,2);
// 
// 	loWeibo_request_setopt(pWBReq_38 , WEIBO_OPTION(REQ_THREAD_POOL_HANDLE) ,tp_handle);
// 
// 	memcpy( twball.keda_ams_validateClientAccount.wbauth_.oauth_token_, "TrueLink", WB_REAL_LEN(OAUTH_TOKEN) );
// 	memcpy( twball.keda_ams_validateClientAccount.wbauth_.oauth_token_secret_, "12345678", WB_REAL_LEN(OAUTH_TOKEN_SECRET) );
// 
// 	memcpy( twball.keda_ams_validateClientAccount.username, "sunfei@kedacom.com", WB_REAL_LEN(UID));
// 	memcpy( twball.keda_ams_validateClientAccount.password, "888888", WB_REAL_LEN(PWD));
// 	memcpy( twball.keda_ams_validateClientAccount.deviceType, "TrueLink", WB_REAL_LEN(OPT_VALUE));
// 
// 	fnKedaWeiBoAPI_ams_validateclientaccount(pWBReq_38, pWBReq_38, &twball.keda_ams_validateClientAccount);

// ==============================================================

	//30.查询账号详细信息  -- 120

	WEIBORequest* pWBReq_39 = loWeibo_request_create();
	set_parameter( pWBReq_39, tp_handle, emPlatform_API );

	memcpy( twball.keda_ams_getAccountInfo.username, "sunfei@kedacom.com", WB_REAL_LEN(UID));

	fnKedaWeiBoAPI_ams_getaccountinfo(pWBReq_39, pWBReq_39, &twball.keda_ams_getAccountInfo);

// ==============================================================

	//31.修改账号详细信息  -- 121

	WEIBORequest* pWBReq_40 = loWeibo_request_create();
	set_parameter( pWBReq_40, tp_handle, emPlatform_API );

	memcpy( twball.keda_ams_updateAccountInfo.username, "sunfei@kedacom.com", WB_REAL_LEN(UID));
	memcpy( twball.keda_ams_updateAccountInfo.mobileNum, "130xxxx8137", WB_REAL_LEN(OPT_VALUE));
	memcpy( twball.keda_ams_updateAccountInfo.extNum, "8484", WB_REAL_LEN(OPT_VALUE));
	memcpy( twball.keda_ams_updateAccountInfo.seat, "3F-10L-04", WB_REAL_LEN(OPT_VALUE));
	memcpy( twball.keda_ams_updateAccountInfo.position, "云协师", WB_REAL_LEN(OPT_VALUE));

	memcpy( twball.keda_ams_updateAccountInfo.brief, "01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", WB_REAL_LEN(BRIEF));


	fnKedaWeiBoAPI_ams_updateaccountinfo(pWBReq_40, pWBReq_40, &twball.keda_ams_updateAccountInfo);

// ==============================================================

	//32.修改账号密码  -- 122

	WEIBORequest* pWBReq_41 = loWeibo_request_create();
	set_parameter( pWBReq_41, tp_handle, emPlatform_API );

	memcpy( twball.keda_ams_updatePassword.username, "sunfei@kedacom.com", WB_REAL_LEN(UID));
	memcpy( twball.keda_ams_updatePassword.oldPassword, "888888", WB_REAL_LEN(PWD));
	memcpy( twball.keda_ams_updatePassword.newPassword, "888888", WB_REAL_LEN(PWD));

	fnKedaWeiBoAPI_ams_updatepassword(pWBReq_41, pWBReq_41, &twball.keda_ams_updatePassword);

// ==============================================================

	//33.获得公司组织架构和所有人员信息  -- 123

	WEIBORequest* pWBReq_42 = loWeibo_request_create();
	set_parameter( pWBReq_42, tp_handle, emPlatform_API );

	fnKedaWeiBoAPI_department_get_all(pWBReq_42, pWBReq_42, &twball.keda_get_department_all);

// ==============================================================

	//34.获取公司组织架构信息  -- 124

	WEIBORequest* pWBReq_43 = loWeibo_request_create();
	set_parameter( pWBReq_43, tp_handle, emPlatform_API );

// 	memcpy( twball.keda_get_department_company.wbopt_.szOpt_[Type_department_id], "1", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_department_get_company(pWBReq_43, pWBReq_43, &twball.keda_get_department_company);

// ==============================================================

	//35.获取部门下人员信息(不包含子部门)  -- 125

	WEIBORequest* pWBReq_44 = loWeibo_request_create();
	set_parameter( pWBReq_44, tp_handle, emPlatform_API );

// 	memcpy( twball.keda_get_department_user.wbopt_.szOpt_[Type_department_id], "2", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_department_get_user(pWBReq_44, pWBReq_44, &twball.keda_get_department_user);

// ==============================================================

	//36.获取部门下人员信息(包含子部门人员)  -- 126

	WEIBORequest* pWBReq_45 = loWeibo_request_create();
	set_parameter( pWBReq_45, tp_handle, emPlatform_API );

	twball.keda_get_department_allUser.department_id = 258;

	fnKedaWeiBoAPI_department_get_alluser(pWBReq_45, pWBReq_45, &twball.keda_get_department_allUser);

// ==============================================================

	//37.在组织架构中搜索部门信息  -- 127

	WEIBORequest* pWBReq_46 = loWeibo_request_create();
	set_parameter( pWBReq_46, tp_handle, emPlatform_API );

	memcpy( twball.keda_department_searchDepartment.department_name, "IT开发部", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_department_searchdepartment(pWBReq_46, pWBReq_46, &twball.keda_department_searchDepartment);

// ==============================================================

	//38.在组织架构中搜索人员信息  -- 128

	WEIBORequest* pWBReq_47 = loWeibo_request_create();
	set_parameter( pWBReq_47, tp_handle, emPlatform_API );

	memcpy( twball.keda_department_searchUser.user_name, "孙", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_department_searchuser(pWBReq_47, pWBReq_47, &twball.keda_department_searchUser);

// ==============================================================

	//39.获取部门下的员工的在线状态  -- 129

	WEIBORequest* pWBReq_48 = loWeibo_request_create();
	set_parameter( pWBReq_48, tp_handle, emPlatform_API );

	twball.keda_department_online_by_depId.department_id = 258;

	fnKedaWeiBoAPI_department_onlineByDepId(pWBReq_48, pWBReq_48, &twball.keda_department_online_by_depId);

// ==============================================================

	//40.获取组织里用户的在线状态  -- 130

	WEIBORequest* pWBReq_49 = loWeibo_request_create();
	set_parameter( pWBReq_49, tp_handle, emPlatform_API );

	memcpy( twball.keda_department_online_by_userId.user_ids, "1,3,5,7", WB_REAL_LEN(USER_IDS));

	fnKedaWeiBoAPI_department_onlineByUserId(pWBReq_49, pWBReq_49, &twball.keda_department_online_by_userId);

// ==============================================================

	//41.获取头像  -- 131

	WEIBORequest* pWBReq_50 = loWeibo_request_create();
	set_parameter( pWBReq_50, tp_handle, emWeibo_API );

	memcpy( twball.keda_get_portrait.portrait128, "photos/portrait/124/head_128.jpg", WB_REAL_LEN(URL));

	fnKedaWeiBoAPI_get_portrait(pWBReq_50, pWBReq_50, &twball.keda_get_portrait);

// ==============================================================

	//42.批量查询账号详细信息  -- 132

	WEIBORequest* pWBReq_51 = loWeibo_request_create();
	set_parameter( pWBReq_51, tp_handle, emPlatform_API );

	memcpy( twball.keda_ams_batchGetAccountInfo.usernames, "0512111885627;zhouyanhua@kedacom.com;0512111885625", WB_REAL_LEN(MULTI_USER));

	fnKedaWeiBoAPI_ams_batchGetAccountInfo(pWBReq_51, pWBReq_51, &twball.keda_ams_batchGetAccountInfo);

// ==============================================================

	//43.修改用户头像  -- 133

	WEIBORequest* pWBReq_52 = loWeibo_request_create();
	set_parameter( pWBReq_52, tp_handle, emPlatform_API );

	twball.keda_confirmPortrait.x1 = 0;
	twball.keda_confirmPortrait.y1 = 0;
	twball.keda_confirmPortrait.width = 375;
	twball.keda_confirmPortrait.height = 375;

	memcpy( twball.keda_confirmPortrait.filepath, "F:\\sunfei-test.jpg", WB_REAL_LEN(MAX_FILE_PATH));

	fnKedaWeiBoAPI_confirm_portrait(pWBReq_52, pWBReq_52, &twball.keda_confirmPortrait);

// ==============================================================

	//44.获取组织架构版本信息  -- 134

	WEIBORequest* pWBReq_53 = loWeibo_request_create();
	set_parameter( pWBReq_53, tp_handle, emPlatform_API );

	fnKedaWeiBoAPI_department_getDepartmentVersion(pWBReq_53, pWBReq_53, &twball.keda_department_get_version);

// ==============================================================

	//45.带图片和附件的微博发布  -- 135

	WEIBORequest* pWBReq_54 = loWeibo_request_create();
	set_parameter( pWBReq_54, tp_handle, emWeibo_API );

// 	twball.keda_message_upload.group_id = 10;

	memcpy( twball.keda_message_upload.szwbInfo_, "测试【带图片和附件的微博发布】API", WB_REAL_LEN(WB_INFO));

	twball.keda_message_upload.fileCount = 3;

	memcpy( twball.keda_message_upload.filepath[0], "F:\\sunfei-test.jpg", WB_REAL_LEN(MAX_FILE_PATH));
	memcpy( twball.keda_message_upload.filepath[1], "F:\\sunfei-tt.jpg", WB_REAL_LEN(MAX_FILE_PATH));
	memcpy( twball.keda_message_upload.filepath[2], "F:\\进行测试的附件.txt", WB_REAL_LEN(MAX_FILE_PATH));

	fnKedaWeiBoAPI_message_upload(pWBReq_54, pWBReq_54, &twball.keda_message_upload);

// ==============================================================

	//46.删除自己的微博  -- 136

	WEIBORequest* pWBReq_55 = loWeibo_request_create();
	set_parameter( pWBReq_55, tp_handle, emWeibo_API );

	twball.keda_message_delete_thread.feed_id = 41132;

	fnKedaWeiBoAPI_message_deleteThread(pWBReq_55, pWBReq_55, &twball.keda_message_delete_thread);

// ==============================================================

	//47.删除自己的回复  -- 137

	WEIBORequest* pWBReq_56 = loWeibo_request_create();
	set_parameter( pWBReq_56, tp_handle, emWeibo_API );

	twball.keda_message_delete_reply.feed_id = 41133;

	fnKedaWeiBoAPI_message_deleteReply(pWBReq_56, pWBReq_56, &twball.keda_message_delete_reply);

// ==============================================================

	//48.获取@、赞、粉丝等初始消息  -- 138

	WEIBORequest* pWBReq_57 = loWeibo_request_create();
	set_parameter( pWBReq_57, tp_handle, emWeibo_API );

	fnKedaWeiBoAPI_notify_getAll(pWBReq_57, pWBReq_57, &twball.keda_notify_get_all);

// ==============================================================

	//49.保持用户心跳(平台API)  -- 139

	WEIBORequest* pWBReq_heart_p = loWeibo_request_create();
	set_parameter( pWBReq_heart_p, tp_handle, emPlatform_API );

	fnKedaWeiBoAPI_p_keep_heartBeat(pWBReq_heart_p, pWBReq_heart_p, &twball.keda_p_keep_heartbeat);

// ==============================================================

	//49.保持用户心跳(微博API)  -- 139

	WEIBORequest* pWBReq_heart_w = loWeibo_request_create();
	set_parameter( pWBReq_heart_w, tp_handle, emWeibo_API );

	fnKedaWeiBoAPI_w_keep_heartBeat(pWBReq_heart_w, pWBReq_heart_w, &twball.keda_w_keep_heartbeat);

// ==============================================================

	//50.获得用户信息(通过email)  -- 140

	WEIBORequest* pWBReq_59 = loWeibo_request_create();
	set_parameter( pWBReq_59, tp_handle, emWeibo_API );

	memcpy( twball.keda_get_user_info_by_email.email, "sunfei@kedacom.com", WB_REAL_LEN(OPT_VALUE));

	fnKedaWeiBoAPI_get_user_info_by_email(pWBReq_59, pWBReq_59, &twball.keda_get_user_info_by_email);

// ==============================================================

	//51.获得当前登录用户信息  -- 141

	WEIBORequest* pWBReq_60 = loWeibo_request_create();
	set_parameter( pWBReq_60, tp_handle, emWeibo_API );

	fnKedaWeiBoAPI_get_current_user_info(pWBReq_60, pWBReq_60, &twball.keda_get_current_user_info);

// ==============================================================

	//52.获取长篇大论的url  -- 142

	WEIBORequest* pWBReq_61 = loWeibo_request_create();
	set_parameter( pWBReq_61, tp_handle, emWeibo_API );

	twball.keda_get_article_url.thread_id = 11940;  // 长篇大论对应的微博消息主题ID长篇大论对应的微博消息主题ID

	fnKedaWeiBoAPI_get_article_url(pWBReq_61, pWBReq_61, &twball.keda_get_article_url);

// ==============================================================

	//53.修改个人介绍  -- 143

	WEIBORequest* pWBReq_62 = loWeibo_request_create();
	set_parameter( pWBReq_62, tp_handle, emPlatform_API );

	memcpy( twball.keda_ams_updateBrief.username, "sunfei@kedacom.com", WB_REAL_LEN(UID));
	memcpy( twball.keda_ams_updateBrief.brief, "012345678901234567890", WB_REAL_LEN(BRIEF));

	fnKedaWeiBoAPI_ams_updateBrief(pWBReq_62, pWBReq_62, &twball.keda_ams_updateBrief);

// ==============================================================

	//54.按姓名模糊查找本企业联系人  -- 144

	WEIBORequest* pWBReq_63 = loWeibo_request_create();
	set_parameter( pWBReq_63, tp_handle, emPlatform_API );

	memcpy( twball.keda_ams_list_account_by_str.username, "sunfei@kedacom.com", WB_REAL_LEN(UID));
	memcpy( twball.keda_ams_list_account_by_str.name, "飞", WB_REAL_LEN(UID));
	twball.keda_ams_list_account_by_str.startRow = 0;
	twball.keda_ams_list_account_by_str.pageSize = 30;

	fnKedaWeiBoAPI_ams_list_account_by_str(pWBReq_63, pWBReq_63, &twball.keda_ams_list_account_by_str);

// ==============================================================

	getchar();

#endif

	thpool_destroy( tp_handle );

	getchar();

	return 0;
}




/*******************************************/
/*    @brief weibo callback defination     */
/*******************************************/


struct t_wb_callback_byloach
{
	fun_wb_status_callback fstatus_cb;
	fun_wb_body_callback  fbody_cb;
	fun_wb_header_callback fheader_cb;
};
struct t_wb_callback_byloach* Wb_get_struct_callback( int option );


#define WEIBO_struct_statuscallback_init(name) \
	static void Wb_struct_statuscallback_init_##name##_cb(int state , int errCode , void* pUserdata)

#define WEIBO_struct_statuscallback_init_fun(name) Wb_struct_statuscallback_init_##name##_cb

#define WEIBO_struct_bodycallback_init(name) \
	static void Wb_struct_bodycallback_init_##name##_cb(unsigned long httpCode , const char* body , unsigned long len , void* pUserdata)

#define WEIBO_struct_bodycallback_init_fun(name) Wb_struct_bodycallback_init_##name##_cb

#define WEIBO_struct_headercallback_init(name) \
	static void Wb_struct_headercallback_init_##name##_cb(unsigned long httpCode , const char* header , unsigned long len , void* pUserdata)

#define WEIBO_struct_headercallback_init_fun(name) Wb_struct_headercallback_init_##name##_cb


WEIBO_struct_statuscallback_init(BASE)
{
}

WEIBO_struct_bodycallback_init(BASE)
{
	printf("   BASE : \n    %s \n\n" , body );
}


/************************************************************************/
/*                                                                      */
/* 作者：孙飞                                                           */
/* 用途：回调XML数据                                                    */
/* 更新日期：2012-01-17                                                 */
/*                                                                      */
/************************************************************************/


#define STATUS_CALLBACK_PROCESS() \
switch( state ) \
{ \
case http_closed: \
case http_start: \
case http_stop: \
case http_cancel: \
	break; \
case http_complete: \
	{ \
		WEIBORequest *pWBReq = (WEIBORequest *)pUserdata; \
		if ( pWBReq != NULL ) \
		{ \
			loWeibo_request_destroy(pWBReq); \
		} \
	} \
	break; \
} \



// 2012-01-04 update by sunfei 完成keda微博API

//1.获得Oauth的accountToken(平台API)  -- 82
WEIBO_struct_statuscallback_init(KEDA_API_P_GET_ACCOUNTTOKEN)
{
	STATUS_CALLBACK_PROCESS();	
}

WEIBO_struct_bodycallback_init(KEDA_API_P_GET_ACCOUNTTOKEN)
{
	printf("【KEDA_API_P_GET_ACCOUNTTOKEN】 -- 获得Oauth的accountToken(平台API)\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if ( body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if ( pRoot != NULL )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *p = wbParserNS::GetCHAR_Key_XML_EX("accountToken", pRoot, outlen);
			if ( p != NULL )
			{
				memcpy( g_szPlatformAccountToken, p, outlen );
				printf("获得 平台API <accountToken> = %s\n\n", p);
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_P_GET_ACCOUNTTOKEN】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_P_GET_ACCOUNTTOKEN】 -- 获得Oauth的accountToken(平台API)\n");
}


//1.获得Oauth的accountToken(微博API)  -- 82
WEIBO_struct_statuscallback_init(KEDA_API_W_GET_ACCOUNTTOKEN)
{
	STATUS_CALLBACK_PROCESS();	
}

WEIBO_struct_bodycallback_init(KEDA_API_W_GET_ACCOUNTTOKEN)
{
	printf("【KEDA_API_W_GET_ACCOUNTTOKEN】 -- 获得Oauth的accountToken(微博API)\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if ( body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if ( pRoot != NULL )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *p = wbParserNS::GetCHAR_Key_XML_EX("accountToken", pRoot, outlen);
			if ( p != NULL )
			{
				memcpy( g_szWeiboAccountToken, p, outlen );
				printf("获得 微博API <accountToken> = %s\n\n", p);
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_W_GET_ACCOUNTTOKEN】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_W_GET_ACCOUNTTOKEN】 -- 获得Oauth的accountToken(微博API)\n");
}

//2.登录平台API  -- 83
WEIBO_struct_statuscallback_init(KEDA_API_P_LOGIN)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_headercallback_init(KEDA_API_P_LOGIN)
{
	printf("【KEDA_API_P_LOGIN】 -- 登录平台API -- 解析头\n");

	printf("   header = 0x%x    len = %d    httpCode = %d\n\n" , header, len, httpCode );

	WBParseCHAR cookieValue[2][512] = {0};

	printf("\n%s\n" , header );
	if( httpCode == 200 )
	{
        // FILE * fp = fopen( "cookie.txt", "wb" );
        // fwrite( header, 1, strlen(header), fp );
        // fclose( fp );

		wb_parse_cookie_keda( header, cookieValue, 2 );

		printf("获得 %s\n\n" , cookieValue[0] );
		printf("获得 %s\n\n" , cookieValue[1] );

		memset(g_szPlatformCookieValue[0], 0, sizeof(g_szPlatformCookieValue[0]));
		memset(g_szPlatformCookieValue[1], 0, sizeof(g_szPlatformCookieValue[1]));

		memcpy(g_szPlatformCookieValue[0], cookieValue[0], strlen(cookieValue[0]));
		memcpy(g_szPlatformCookieValue[1], cookieValue[1], strlen(cookieValue[1]));
	}
	printf("【KEDA_API_P_LOGIN】 -- 登录平台API -- 解析头\n\n");
}

WEIBO_struct_bodycallback_init(KEDA_API_P_LOGIN)
{
	printf("【KEDA_API_P_LOGIN】 -- 登录平台API -- 解析body\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pUsername = wbParserNS::GetCHAR_Key_XML_EX("username", pRoot, outlen);
			if ( pUsername != NULL )
			{
				printf("获得 <username> = %s\n", pUsername);
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_P_LOGIN】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_P_LOGIN】 -- 登录平台API -- 解析body\n");
}


//2.登录微博API  -- 83
WEIBO_struct_statuscallback_init(KEDA_API_W_LOGIN)
{
	STATUS_CALLBACK_PROCESS();
}


WEIBO_struct_headercallback_init(KEDA_API_W_LOGIN)
{
	printf("【KEDA_API_W_LOGIN】 -- 登录微博API -- 解析头\n");

	printf("   header = 0x%x    len = %d    httpCode = %d\n\n" , header, len, httpCode );

	WBParseCHAR cookieValue[2][512] = {0};

	printf("\n%s\n" , header );
	if( httpCode == 200 )
	{
		wb_parse_cookie_keda( header, cookieValue, 2 );

		printf("获得 %s\n\n" , cookieValue[0] );
		printf("获得 %s\n\n" , cookieValue[1] );

		memset(g_szWeiboCookieValue[0], 0, sizeof(g_szWeiboCookieValue[0]));
		memset(g_szWeiboCookieValue[1], 0, sizeof(g_szWeiboCookieValue[1]));

		memcpy(g_szWeiboCookieValue[0], cookieValue[0], strlen(cookieValue[0]));
		memcpy(g_szWeiboCookieValue[1], cookieValue[1], strlen(cookieValue[1]));
	}
	printf("【KEDA_API_W_LOGIN】 -- 登录微博API -- 解析头\n\n");
}

WEIBO_struct_bodycallback_init(KEDA_API_W_LOGIN)
{
	printf("【KEDA_API_W_LOGIN】 -- 登录微博API -- 解析body\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pUsername = wbParserNS::GetCHAR_Key_XML_EX("username", pRoot, outlen);
			if ( pUsername != NULL )
			{
				printf("获得 <username> = %s\n", pUsername);
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_W_LOGIN】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_W_LOGIN】 -- 登录微博API -- 解析body\n");
}




//3.获得当前登录用户及其关注用户的微博  -- 84
WEIBO_struct_statuscallback_init(KEDA_API_GET_ME_AND_FRIENDS)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_ME_AND_FRIENDS)
{
	printf("【KEDA_API_GET_ME_AND_FRIENDS】 -- 获得当前登录用户及其关注用户的微博\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}
			else
			{
				iCount = 0;	
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MeAndFriend, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n\n");
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( MeAndFriend,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_ME_AND_FRIENDS】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_ME_AND_FRIENDS】 -- 获得当前登录用户及其关注用户的微博\n");
}

//4.获得用户发布的微博数据（友讯）  -- 85
WEIBO_struct_statuscallback_init(KEDA_API_GET_USER_DATA)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_USER_DATA)
{
	printf("【KEDA_API_GET_USER_DATA】 -- 获得用户发布的微博数据（友讯）\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MessagePublishF, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n\n");
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( MessagePublishF,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_USER_DATA】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_USER_DATA】 -- 获得用户发布的微博数据（友讯）\n");
}

//5.获得用户发布的微博数据（全部）  -- 86
WEIBO_struct_statuscallback_init(KEDA_API_GET_USER_ALL_DATA)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_USER_ALL_DATA)
{
	printf("【KEDA_API_GET_USER_ALL_DATA】 -- 获得用户发布的微博数据（全部）\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MessagePublishA, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n\n");
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( MessagePublishA,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_USER_ALL_DATA】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_USER_ALL_DATA】 -- 获得用户发布的微博数据（全部）\n");
}

//6.获得一条主题以及回复  -- 87
WEIBO_struct_statuscallback_init(KEDA_API_GET_SUBJECT_AND_REPLY)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_SUBJECT_AND_REPLY)
{
	printf("【KEDA_API_GET_SUBJECT_AND_REPLY】 -- 获得一条主题以及回复\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(SubjectAndReply, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n\n");
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( SubjectAndReply,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_SUBJECT_AND_REPLY】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_SUBJECT_AND_REPLY】 -- 获得一条主题以及回复\n");
}

//7.发布一条微博  -- 88
WEIBO_struct_statuscallback_init(KEDA_API_MESSAGE_PUBLISH)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_MESSAGE_PUBLISH)
{
	printf("【KEDA_API_MESSAGE_PUBLISH】 -- 发布一条微博\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MessagePublish, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n\n");
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( MessagePublish, Get, handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_MESSAGE_PUBLISH】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_MESSAGE_PUBLISH】 -- 发布一条微博\n");
}

//8.回复一条微博  -- 89
WEIBO_struct_statuscallback_init(KEDA_API_MESSAGE_REPLY)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_MESSAGE_REPLY)
{
	printf("【KEDA_API_MESSAGE_REPLY】 -- 回复一条微博\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MessageReply, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n\n");
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( MessageReply, Get, handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_MESSAGE_REPLY】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_MESSAGE_REPLY】 -- 回复一条微博\n");
}

//9.获得用户信息  -- 90
WEIBO_struct_statuscallback_init(KEDA_API_GET_USER_INFO)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_USER_INFO)
{
	printf("【KEDA_API_GET_USER_INFO】 -- 获得用户信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_User *pt_UserInfo = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(UserInfo, Get, body, len, iCount,(void**)&pt_UserInfo, NULL, NULL);

				printf("获得信息成功！\n\n");
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( UserInfo,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_USER_INFO】  Parse_data_XML 失败！");
		}
	}

	printf("【KEDA_API_GET_USER_INFO】 -- 获得用户信息\n");
}




// 2012-03-04 add by sunfei 科达公司微博v1.0接口

//1.获得公司广场的微博数据  -- 91
WEIBO_struct_statuscallback_init(KEDA_API_GET_COMPANY_PUBLIC_DATA)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_COMPANY_PUBLIC_DATA)
{
	printf("【KEDA_API_GET_COMPANY_PUBLIC】 -- 获得公司广场的微博数据\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				unsigned long iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				unsigned long iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(CompanyPublic, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( CompanyPublic,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_COMPANY_PUBLIC_DATA】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_COMPANY_PUBLIC_DATA】 -- 获得公司广场的微博数据\n");
}

//2.获得群组的微博数据  -- 92
WEIBO_struct_statuscallback_init(KEDA_API_GET_GROUPS_DATA)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_GROUPS_DATA)
{
	printf("【KEDA_API_GET_GROUPS_DATA】 -- 获得群组的微博数据\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(GroupsData, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( GroupsData,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_GROUPS_DATA】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_GROUPS_DATA】 -- 获得群组的微博数据\n");
}

//3.获得@当前用户的微博数据   -- 93
WEIBO_struct_statuscallback_init(KEDA_API_GET_MENTIONS_DATA)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_MENTIONS_DATA)
{
	printf("【KEDA_API_GET_MENTIONS_DATA】 -- 获得@当前用户的微博数据\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MentionsData, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( MentionsData,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_MENTIONS_DATA】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_MENTIONS_DATA】 -- 获得@当前用户的微博数据\n");
}

//4.获得回复我的微博数据  -- 94
WEIBO_struct_statuscallback_init(KEDA_API_GET_REPLIES_DATA)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_REPLIES_DATA)
{
	printf("【KEDA_API_GET_REPLIES_DATA】 -- 获得回复我的微博数据\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(RepliesData, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( RepliesData,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_REPLIES_DATA】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_REPLIES_DATA】 -- 获得回复我的微博数据\n");
}

//5.获得我赞过的微博数据  -- 95
WEIBO_struct_statuscallback_init(KEDA_API_GET_LIKES_DATA)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_LIKES_DATA)
{
	printf("【KEDA_API_GET_LIKES_DATA】 -- 获得我赞过的微博数据\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(LikesData, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( LikesData,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_LIKES_DATA】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_LIKES_DATA】 -- 获得我赞过的微博数据\n");
}

//6.获得我收藏的微博数据  -- 96
WEIBO_struct_statuscallback_init(KEDA_API_GET_FAVORITES_DATA)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_FAVORITES_DATA)
{
	printf("【KEDA_API_GET_FAVORITES_DATA】 -- 获得我收藏的微博数据\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(FavoritesData, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( FavoritesData,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_FAVORITES_DATA】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_FAVORITES_DATA】 -- 获得我收藏的微博数据\n");
}

//7.获得某个话题的微博数据  -- 97
WEIBO_struct_statuscallback_init(KEDA_API_GET_TOPICS_DATA)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_TOPICS_DATA)
{
	printf("【KEDA_API_GET_TOPICS_DATA】 -- 获得某个话题的微博数据\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(TopicsData, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( TopicsData,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_TOPICS_DATA】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_TOPICS_DATA】 -- 获得某个话题的微博数据\n");
}

//8.获得公司人脉  -- 98
WEIBO_struct_statuscallback_init(KEDA_API_GET_FRIENDSHIP_COMPANY)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_FRIENDSHIP_COMPANY)
{
	printf("【KEDA_API_GET_FRIENDSHIP_COMPANY】 -- 获得公司人脉\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Users *pt_Company_Info = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(FriendshipCompany, Get, body, len, iCount,(void**)&pt_Company_Info, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( FriendshipCompany,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_FRIENDSHIP_COMPANY】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_FRIENDSHIP_COMPANY】 -- 获得公司人脉\n");
}

//9.获得我的关注列表  -- 99
WEIBO_struct_statuscallback_init(KEDA_API_GET_FRIENDSHIP_ATTENTION)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_FRIENDSHIP_ATTENTION)
{
	printf("【KEDA_API_GET_FRIENDSHIP_ATTENTION】 -- 获得我的关注列表\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Users *pt_Attention_Info = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(FriendshipAttention, Get, body, len, iCount,(void**)&pt_Attention_Info, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( FriendshipAttention,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_FRIENDSHIP_ATTENTION】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_FRIENDSHIP_ATTENTION】 -- 获得我的关注列表\n");
}

//10.获得我的粉丝列表  -- 100
WEIBO_struct_statuscallback_init(KEDA_API_GET_FRIENDSHIP_FOLLOWERS)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_FRIENDSHIP_FOLLOWERS)
{
	printf("【KEDA_API_GET_FRIENDSHIP_FOLLOWERS】 -- 获得我的粉丝列表\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Users *pt_Followers_Info = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(FriendshipFollowers, Get, body, len, iCount,(void**)&pt_Followers_Info, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( FriendshipFollowers,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_FRIENDSHIP_FOLLOWERS】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_FRIENDSHIP_FOLLOWERS】 -- 获得我的粉丝列表\n");
}


//11.获得我的好友列表  -- 101
WEIBO_struct_statuscallback_init(KEDA_API_GET_FRIENDSHIP_FRIENDS)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_FRIENDSHIP_FRIENDS)
{
	printf("【KEDA_API_GET_FRIENDSHIP_FRIENDS】 -- 获得我的好友列表\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Users *pt_Friends_Info = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(FriendshipFriends, Get, body, len, iCount,(void**)&pt_Friends_Info, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( FriendshipFriends,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_FRIENDSHIP_FRIENDS】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_FRIENDSHIP_FRIENDS】 -- 获得我的好友列表\n");
}

//12.添加关注  -- 102
WEIBO_struct_statuscallback_init(KEDA_API_FRIENDSHIP_CREATE)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_FRIENDSHIP_CREATE)
{
	printf("【KEDA_API_FRIENDSHIP_CREATE】 -- 添加关注\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				outlen = 0;
				const char *puid = wbParserNS::GetCHAR_Key_XML_EX("uid", pRoot, outlen);
				if ( puid != NULL )
				{
					printf("获得 <uid> = %s\n", puid);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_FRIENDSHIP_CREATE】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_FRIENDSHIP_CREATE】 -- 添加关注\n");
}

//13.取消关注  -- 103
WEIBO_struct_statuscallback_init(KEDA_API_FRIENDSHIP_DESTROY)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_FRIENDSHIP_DESTROY)
{
	printf("【KEDA_API_FRIENDSHIP_DESTROY】 -- 取消关注\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				outlen = 0;
				const char *puid = wbParserNS::GetCHAR_Key_XML_EX("uid", pRoot, outlen);
				if ( puid != NULL )
				{
					printf("获得 <uid> = %s\n", puid);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_FRIENDSHIP_DESTROY】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_FRIENDSHIP_DESTROY】 -- 取消关注\n");
}

//14.添加收藏  -- 104
WEIBO_struct_statuscallback_init(KEDA_API_FAVORITES_CREATE)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_FAVORITES_CREATE)
{
	printf("【KEDA_API_FAVORITES_CREATE】 -- 添加收藏\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				outlen = 0;
				const char *pthreadId = wbParserNS::GetCHAR_Key_XML_EX("threadId", pRoot, outlen);
				if ( pthreadId != NULL )
				{
					printf("获得 <threadId> = %s\n", pthreadId);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_FAVORITES_CREATE】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_FAVORITES_CREATE】 -- 添加收藏\n");
}

//15.取消收藏  -- 105
WEIBO_struct_statuscallback_init(KEDA_API_FAVORITES_DESTROY)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_FAVORITES_DESTROY)
{
	printf("【KEDA_API_FAVORITES_DESTROY】 -- 取消收藏\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				outlen = 0;
				const char *pthreadId = wbParserNS::GetCHAR_Key_XML_EX("threadId", pRoot, outlen);
				if ( pthreadId != NULL )
				{
					printf("获得 <threadId> = %s\n", pthreadId);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_FAVORITES_DESTROY】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_FAVORITES_DESTROY】 -- 取消收藏\n");
}

//16.添加话题  -- 106
WEIBO_struct_statuscallback_init(KEDA_API_TOPIC_CREATE)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_TOPIC_CREATE)
{
	printf("【KEDA_API_TOPIC_CREATE】 -- 添加话题\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Topic *pt_Topic_Create = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(TopicsCreateAndDestroy, Get, body, len, iCount,(void**)&pt_Topic_Create, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( TopicsCreateAndDestroy,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_TOPIC_CREATE】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_TOPIC_CREATE】 -- 添加话题\n");
}

//17.取消话题  -- 107
WEIBO_struct_statuscallback_init(KEDA_API_TOPIC_DESTROY)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_TOPIC_DESTROY)
{
	printf("【KEDA_API_TOPIC_DESTROY】 -- 取消话题\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Topic *pt_Topic_Destroy = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(TopicsCreateAndDestroy, Get, body, len, iCount,(void**)&pt_Topic_Destroy, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( TopicsCreateAndDestroy, Get, handle );
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_TOPIC_DESTROY】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_TOPIC_DESTROY】 -- 取消话题\n");
}

//18.添加like  -- 108
WEIBO_struct_statuscallback_init(KEDA_API_LIKE_CREATE)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_LIKE_CREATE)
{
	printf("【KEDA_API_LIKE_CREATE】 -- 添加like\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Like *pt_Like_Create = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(LikeCreateAndDestroy, Get, body, len, iCount,(void**)&pt_Like_Create, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( LikeCreateAndDestroy,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_LIKE_CREATE】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_LIKE_CREATE】 -- 添加like\n");
}

//19.取消like  -- 109
WEIBO_struct_statuscallback_init(KEDA_API_LIKE_DESTROY)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_LIKE_DESTROY)
{
	printf("【KEDA_API_LIKE_DESTROY】 -- 取消like\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		// 解析XML结构并按照指定文件名在本地保存
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Like *pt_Like_Destroy = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(LikeCreateAndDestroy, Get, body, len, iCount,(void**)&pt_Like_Destroy, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( LikeCreateAndDestroy,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_LIKE_DESTROY】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_LIKE_DESTROY】 -- 取消like\n");
}

//20.获取我的群组列表  -- 110
WEIBO_struct_statuscallback_init(KEDA_API_GET_GROUP_USER)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_GROUP_USER)
{
	printf("【KEDA_API_GET_GROUP_USER】 -- 获取我的群组列表\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Groups *pt_Groups = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(Groups, Get, body, len, iCount,(void**)&pt_Groups, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( Groups,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_GROUP_USER】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_GROUP_USER】 -- 获取我的群组列表\n");
}

//21.获得所有的群组列表  -- 111
WEIBO_struct_statuscallback_init(KEDA_API_GET_GROUP_ALL)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_GROUP_ALL)
{
	printf("【KEDA_API_GET_GROUP_ALL】 -- 获得所有的群组列表\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Groups *pt_Groups = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(Groups, Get, body, len, iCount,(void**)&pt_Groups, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( Groups,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_GROUP_ALL】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_GROUP_ALL】 -- 获得所有的群组列表\n");
}

//22.添加群操作  -- 112
WEIBO_struct_statuscallback_init(KEDA_API_GROUP_CREATE)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GROUP_CREATE)
{
	printf("【KEDA_API_GROUP_CREATE】 -- 添加群操作\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_GroupCreate *pt_Group_Create = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(GroupCreate, Get, body, len, iCount,(void**)&pt_Group_Create, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( GroupCreate,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GROUP_CREATE】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GROUP_CREATE】 -- 添加群操作\n");
}

//23.加入群操作  -- 113
WEIBO_struct_statuscallback_init(KEDA_API_GROUP_JOIN)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GROUP_JOIN)
{
	printf("【KEDA_API_GROUP_JOIN】 -- 加入群操作\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		// 解析XML结构并按照指定文件名在本地保存
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				outlen = 0;
				const char *pguid = wbParserNS::GetCHAR_Key_XML_EX("guid", pRoot, outlen);
				if ( pguid != NULL )
				{
					printf("获得 <guid> = %s\n", pguid);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GROUP_JOIN】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GROUP_JOIN】 -- 加入群操作\n");
}

//24.退出群操作  -- 114
WEIBO_struct_statuscallback_init(KEDA_API_GROUP_QUIT)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GROUP_QUIT)
{
	printf("【KEDA_API_GROUP_QUIT】 -- 退出群操作\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				outlen = 0;
				const char *pguid = wbParserNS::GetCHAR_Key_XML_EX("guid", pRoot, outlen);
				if ( pguid != NULL )
				{
					printf("获得 <guid> = %s\n", pguid);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GROUP_QUIT】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GROUP_QUIT】 -- 退出群操作\n");
}

//25.查看我的私信列表  -- 115
WEIBO_struct_statuscallback_init(KEDA_API_GET_PRIVATEMESSAGE_LIST)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_PRIVATEMESSAGE_LIST)
{
	printf("【KEDA_API_GET_PRIVATEMESSAGE_LIST】 -- 查看我的私信列表\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_PrivateContacters *pt_PrivateContacters = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(PrivateContacters, Get, body, len, iCount,(void**)&pt_PrivateContacters, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( PrivateContacters,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_PRIVATEMESSAGE_LIST】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_PRIVATEMESSAGE_LIST】 -- 查看我的私信列表\n");
}

//26.查看我与某人的私信的聊天记录  -- 116
WEIBO_struct_statuscallback_init(KEDA_API_GET_PRIVATEMESSAGE_USER)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_PRIVATEMESSAGE_USER)
{
	printf("【KEDA_API_GET_PRIVATEMESSAGE_USER】 -- 查看我与某人的私信的聊天记录\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_PrivateFeeds *pt_PrivateFeeds = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(PrivateFeeds, Get, body, len, iCount,(void**)&pt_PrivateFeeds, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( PrivateFeeds,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_PRIVATEMESSAGE_USER】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_PRIVATEMESSAGE_USER】 -- 查看我与某人的私信的聊天记录\n");
}

//27.发送私信到某人  -- 117
WEIBO_struct_statuscallback_init(KEDA_API_PRIVATEMESSAGE_SEND)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_PRIVATEMESSAGE_SEND)
{
	printf("【KEDA_API_PRIVATEMESSAGE_SEND】 -- 发送私信到某人\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_PrivateFeed *pt_PrivateFeed = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(PrivateFeed, Get, body, len, iCount,(void**)&pt_PrivateFeed, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( PrivateFeed,Get,handle);
				}
			}
			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_PRIVATEMESSAGE_SEND】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_PRIVATEMESSAGE_SEND】 -- 发送私信到某人\n");
}

//28.删除私信  -- 118
WEIBO_struct_statuscallback_init(KEDA_API_PRIVATEMESSAGE_DELETE)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_PRIVATEMESSAGE_DELETE)
{
	printf("【KEDA_API_PRIVATEMESSAGE_DELETE】 -- 删除私信\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				outlen = 0;
				const char *pid = wbParserNS::GetCHAR_Key_XML_EX("id", pRoot, outlen);
				if ( pid != NULL )
				{
					printf("获得 <id> = %s\n", pid);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_PRIVATEMESSAGE_DELETE】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_PRIVATEMESSAGE_DELETE】 -- 删除私信\n");
}





// 2012-03-06 add by sunfei 增加v1.0接口 第三部分


//11.验证终端账号  -- 119
WEIBO_struct_statuscallback_init(KEDA_API_AMS_VALIDATECLIENTACCOUNT)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_AMS_VALIDATECLIENTACCOUNT)
{
	printf("【KEDA_API_AMS_VALIDATECLIENTACCOUNT】 -- 验证终端账号\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Result *pt_Result = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(ValidateClientAccount, Get, body, len, iCount,(void**)&pt_Result, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( ValidateClientAccount,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_AMS_VALIDATECLIENTACCOUNT】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_AMS_VALIDATECLIENTACCOUNT】 -- 验证终端账号\n");
}


//12.查询账号详细信息  -- 120
WEIBO_struct_statuscallback_init(KEDA_API_AMS_GETACCOUNTINFO)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_AMS_GETACCOUNTINFO)
{
	printf("【KEDA_API_AMS_GETACCOUNTINFO】 -- 查询账号详细信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_EntUser *pt_EntUser = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(AccountInfo, Get, body, len, iCount,(void**)&pt_EntUser, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( AccountInfo,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_AMS_GETACCOUNTINFO】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_AMS_GETACCOUNTINFO】 -- 查询账号详细信息\n");
}

//13.修改账号详细信息  -- 121
WEIBO_struct_statuscallback_init(KEDA_API_AMS_UPDATEACCOUNTINFO)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_AMS_UPDATEACCOUNTINFO)
{
	printf("【KEDA_API_AMS_UPDATEACCOUNTINFO】 -- 修改账号详细信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				printf("获得信息成功！\n");
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_AMS_UPDATEACCOUNTINFO】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_AMS_UPDATEACCOUNTINFO】 -- 修改账号详细信息\n");
}

//14.修改账号密码  -- 122
WEIBO_struct_statuscallback_init(KEDA_API_AMS_UPDATEPASSWORD)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_AMS_UPDATEPASSWORD)
{
	printf("【KEDA_API_AMS_UPDATEPASSWORD】 -- 修改账号密码\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				printf("获得信息成功！\n");
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_AMS_UPDATEPASSWORD】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_AMS_UPDATEPASSWORD】 -- 修改账号密码\n");
}


// add end


// 2012-03-15 add by sunfei 增加v1.0接口 第四部分


//1.获得公司组织架构和所有人员信息  -- 123
WEIBO_struct_statuscallback_init(KEDA_API_GET_DEPARTMENT_ALL)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_DEPARTMENT_ALL)
{
	printf("【KEDA_API_GET_DEPARTMENT_ALL】 -- 获得公司组织架构和所有人员信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Departments *pt_Departments = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(DepartmentAll, Get, body, len, iCount,(void**)&pt_Departments, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( DepartmentAll,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_DEPARTMENT_ALL】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_DEPARTMENT_ALL】 -- 获得公司组织架构和所有人员信息\n");
}


//2..获取公司组织架构信息  -- 124
WEIBO_struct_statuscallback_init(KEDA_API_GET_DEPARTMENT_COMPANY)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_DEPARTMENT_COMPANY)
{
	printf("【KEDA_API_GET_DEPARTMENT_COMPANY】 -- 获取公司组织架构信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Departments *pt_Departments = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(DepartmentCompany, Get, body, len, iCount,(void**)&pt_Departments, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( DepartmentCompany,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_DEPARTMENT_COMPANY】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_DEPARTMENT_COMPANY】 -- 获取公司组织架构信息\n");
}


//3.获取部门下人员信息(不包含子部门)  -- 125
WEIBO_struct_statuscallback_init(KEDA_API_GET_DEPARTMENT_USER)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_DEPARTMENT_USER)
{
	printf("【KEDA_API_GET_DEPARTMENT_USER】 -- 获取部门下人员信息(不包含子部门)\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Department *pt_Department = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(DepartmentUser, Get, body, len, iCount,(void**)&pt_Department, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( DepartmentUser,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_DEPARTMENT_USER】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_DEPARTMENT_USER】 -- 获取部门下人员信息(不包含子部门)\n");
}


//4.获取部门下人员信息(包含子部门人员)  -- 126
WEIBO_struct_statuscallback_init(KEDA_API_GET_DEPARTMENT_ALLUSER)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_DEPARTMENT_ALLUSER)
{
	printf("【KEDA_API_GET_DEPARTMENT_ALLUSER】 -- 获取部门下人员信息(包含子部门人员)\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Department *pt_Department = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(DepartmentAllUser, Get, body, len, iCount,(void**)&pt_Department, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( DepartmentAllUser,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_DEPARTMENT_ALLUSER】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_DEPARTMENT_ALLUSER】 -- 获取部门下人员信息(包含子部门人员)\n");
}


//5.在组织架构中搜索部门信息  -- 127
WEIBO_struct_statuscallback_init(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT)
{
	printf("【KEDA_API_DEPARTMENT_SEARCHDEPARTMENT】 -- 在组织架构中搜索部门信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_searchDepartment *pt_searchDepartment = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(SearchDepartment, Get, body, len, iCount,(void**)&pt_searchDepartment, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( SearchDepartment,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_DEPARTMENT_SEARCHDEPARTMENT】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_DEPARTMENT_SEARCHDEPARTMENT】 -- 在组织架构中搜索部门信息\n");
}


//6.在组织架构中搜索人员信息  -- 128
WEIBO_struct_statuscallback_init(KEDA_API_DEPARTMENT_SEARCHUSER)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_DEPARTMENT_SEARCHUSER)
{
	printf("【KEDA_API_DEPARTMENT_SEARCHUSER】 -- 在组织架构中搜索人员信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_searchUser *pt_searchUser = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(SearchUser, Get, body, len, iCount,(void**)&pt_searchUser, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( SearchUser,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_DEPARTMENT_SEARCHUSER】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_DEPARTMENT_SEARCHUSER】 -- 在组织架构中搜索人员信息\n");
}


//7.获取部门下的员工的在线状态  -- 129
WEIBO_struct_statuscallback_init(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID)
{
	printf("【KEDA_API_DEPARTMENT_ONLINE_BY_DEPID】 -- 获取部门下的员工的在线状态\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Department *pt_Department = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(DepartmentUser, Get, body, len, iCount,(void**)&pt_Department, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( DepartmentUser,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_DEPARTMENT_ONLINE_BY_DEPID】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_DEPARTMENT_ONLINE_BY_DEPID】 -- 获取部门下的员工的在线状态\n");
}


//8.获取组织里用户的在线状态  -- 130
WEIBO_struct_statuscallback_init(KEDA_API_DEPARTMENT_ONLINE_BY_USERID)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_DEPARTMENT_ONLINE_BY_USERID)
{
	printf("【KEDA_API_DEPARTMENT_ONLINE_BY_USERID】 -- 获取组织里用户的在线状态\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_searchUser *pt_searchUser = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(SearchUser, Get, body, len, iCount,(void**)&pt_searchUser, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( SearchUser,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_DEPARTMENT_ONLINE_BY_USERID】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_DEPARTMENT_ONLINE_BY_USERID】 -- 获取组织里用户的在线状态\n");
}


//9.获取头像  -- 131
WEIBO_struct_statuscallback_init(KEDA_API_GET_PORTRAIT)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_PORTRAIT)
{
	printf("【KEDA_API_GET_PORTRAIT】 -- 获取头像\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		FILE * file = fopen("tmp.jpg","wb");
		fseek(file,0,SEEK_SET);
		fwrite(body,1,len,file);
		fclose(file);
	}
	else
	{
		printf("Error! Maybe 404 Not Found\n");
	}
	printf("【KEDA_API_GET_PORTRAIT】 -- 获取头像\n");
}


//10.批量查询账号详细信息  -- 132
WEIBO_struct_statuscallback_init(KEDA_API_AMS_BATCHGETACCOUNTINFO)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_AMS_BATCHGETACCOUNTINFO)
{
	printf("【KEDA_API_AMS_BATCHGETACCOUNTINFO】 -- 批量查询账号详细信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_EntUsers *pt_EntUsers = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(BatchAccountInfo, Get, body, len, iCount,(void**)&pt_EntUsers, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( BatchAccountInfo,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_AMS_BATCHGETACCOUNTINFO】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_AMS_BATCHGETACCOUNTINFO】 -- 批量查询账号详细信息\n");
}

//11.修改用户头像  -- 133
WEIBO_struct_statuscallback_init(KEDA_API_CONFIRM_PORTRAIT)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_CONFIRM_PORTRAIT)
{
	printf("【KEDA_API_CONFIRM_PORTRAIT】 -- 修改用户头像\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				printf("修改用户头像 成功！\n");
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_CONFIRM_PORTRAIT】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_CONFIRM_PORTRAIT】 -- 修改用户头像\n");
}


//12.获取组织架构版本信息  -- 134
WEIBO_struct_statuscallback_init(KEDA_API_DEPARTMENT_GET_VERSION)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_DEPARTMENT_GET_VERSION)
{
	printf("【KEDA_API_DEPARTMENT_GET_VERSION】 -- 获取组织架构版本信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				long ver = wbParserNS::GetLONG_Key_XML("version", pRoot);
				printf("组织架构 版本号 = %d\n", ver);
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_DEPARTMENT_GET_VERSION】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_DEPARTMENT_GET_VERSION】 -- 获取组织架构版本信息\n");
}

//13.带图片和附件的微博发布  -- 135
WEIBO_struct_statuscallback_init(KEDA_API_MESSAGE_UPLOAD)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_MESSAGE_UPLOAD)
{
	printf("【KEDA_API_MESSAGE_UPLOAD】 -- 带图片和附件的微博发布\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Messages *pt_Messages = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(StatuseAndPic, Upload, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( StatuseAndPic,Upload,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_MESSAGE_UPLOAD】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_MESSAGE_UPLOAD】 -- 带图片和附件的微博发布\n");
}

//14.删除自己的微博  -- 136
WEIBO_struct_statuscallback_init(KEDA_API_MESSAGE_DELETE_THREAD)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_MESSAGE_DELETE_THREAD)
{
	printf("【KEDA_API_MESSAGE_DELETE_THREAD】 -- 删除自己的微博\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				printf("删除自己的微博 成功！\n");
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_MESSAGE_DELETE_THREAD】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_MESSAGE_DELETE_THREAD】 -- 删除自己的微博\n");
}


//15.删除自己的回复  -- 137
WEIBO_struct_statuscallback_init(KEDA_API_MESSAGE_DELETE_REPLY)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_MESSAGE_DELETE_REPLY)
{
	printf("【KEDA_API_MESSAGE_DELETE_REPLY】 -- 删除自己的回复\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				printf("删除自己的回复 成功！\n");
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_MESSAGE_DELETE_REPLY】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_MESSAGE_DELETE_REPLY】 -- 删除自己的回复\n");
}

//16.获取@、赞、粉丝等初始消息  -- 138
WEIBO_struct_statuscallback_init(KEDA_API_NOTIFY_GET_ALL)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_NOTIFY_GET_ALL)
{
	printf("【KEDA_API_NOTIFY_GET_ALL】 -- 获取@、赞、粉丝等初始消息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_Notifies *pt_Notifies = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(Notifies, Get, body, len, iCount,(void**)&pt_Notifies, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( Notifies,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_NOTIFY_GET_ALL】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_NOTIFY_GET_ALL】 -- 获取@、赞、粉丝等初始消息\n");
}

//17.保持用户心跳(平台API)   -- 139
WEIBO_struct_statuscallback_init(KEDA_API_P_KEEP_HEARTBEAT)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_P_KEEP_HEARTBEAT)
{
	printf("【KEDA_API_P_KEEP_HEARTBEAT】 -- 保持用户心跳(平台API)\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if ( pRoot != NULL )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if ( pError != NULL )
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if ( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				printf("(平台API)保持用户心跳 成功！\n");
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_P_KEEP_HEARTBEAT】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_P_KEEP_HEARTBEAT】 -- 保持用户心跳(平台API)\n");
}

//17.保持用户心跳(微博API)   -- 139
WEIBO_struct_statuscallback_init(KEDA_API_W_KEEP_HEARTBEAT)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_W_KEEP_HEARTBEAT)
{
	printf("【KEDA_API_W_KEEP_HEARTBEAT】 -- 保持用户心跳(微博API)\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if ( pRoot != NULL )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if ( pError != NULL )
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if ( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				printf("(微博API)保持用户心跳 成功！\n");
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_W_KEEP_HEARTBEAT】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_W_KEEP_HEARTBEAT】 -- 保持用户心跳(微博API)\n");
}


//18.获得用户信息(通过email)  -- 140
WEIBO_struct_statuscallback_init(KEDA_API_GET_USER_INFO_BY_EMAIL)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_USER_INFO_BY_EMAIL)
{
	printf("【KEDA_API_GET_USER_INFO_BY_EMAIL】 -- 获得用户信息(通过email)\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_User *pt_UserInfo = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(UserInfo, Get, body, len, iCount,(void**)&pt_UserInfo, NULL, NULL);

				printf("获得信息成功！\n\n");
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( UserInfo,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_USER_INFO_BY_EMAIL】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_USER_INFO_BY_EMAIL】 -- 获得用户信息(通过email)\n");
}


//19.获得当前登录用户信息  -- 141
WEIBO_struct_statuscallback_init(KEDA_API_GET_CURRENT_USER_INFO)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_CURRENT_USER_INFO)
{
	printf("【KEDA_API_GET_CURRENT_USER_INFO】 -- 获得当前登录用户信息\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

	if (body != NULL && len != 0 && httpCode == 200)
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if( pRoot )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if (pError)
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Keda_User *pt_UserInfo = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(UserInfo, Get, body, len, iCount,(void**)&pt_UserInfo, NULL, NULL);

				printf("获得信息成功！\n\n");
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( UserInfo,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_CURRENT_USER_INFO】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_CURRENT_USER_INFO】 -- 获得当前登录用户信息\n");
}


//20.获取长篇大论的url  -- 142
WEIBO_struct_statuscallback_init(KEDA_API_GET_ARTICLE_URL)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_GET_ARTICLE_URL)
{
	printf("【KEDA_API_GET_ARTICLE_URL】 -- 获取长篇大论的url\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n", body, len, httpCode );

	if ( body != NULL && len != 0 && httpCode == 200 )
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if ( pRoot != NULL )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pURL = wbParserNS::GetCHAR_Key_XML_EX("url", pRoot, outlen);
			const char *pthreadId = wbParserNS::GetCHAR_Key_XML_EX("threadId", pRoot, outlen);
			if ( pURL != NULL && pthreadId != NULL )
			{
				printf("获得 <url> = %s\n\n", pURL);
				printf("获得 <threadId> = %s\n\n", pthreadId);
			}
			else
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_GET_ARTICLE_URL】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_GET_ARTICLE_URL】 -- 获取长篇大论的url\n");
}

//21.修改个人介绍  -- 143
WEIBO_struct_statuscallback_init(KEDA_API_AMS_UPDATEBRIEF)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_AMS_UPDATEBRIEF)
{
	printf("【KEDA_API_AMS_UPDATEBRIEF】 -- 修改个人介绍\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n", body, len, httpCode );

	if ( body != NULL && len != 0 && httpCode == 200 )
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if ( pRoot != NULL )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if ( pError != NULL )
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if ( handle != NULL )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}
			else
			{
				printf("修改个人介绍成功！\n\n");
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_AMS_UPDATEBRIEF】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_AMS_UPDATEBRIEF】 -- 修改个人介绍\n");
}

//22.按姓名模糊查找本企业联系人
WEIBO_struct_statuscallback_init(KEDA_API_AMS_LIST_ACCOUNT_BY_STR)
{
	STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_API_AMS_LIST_ACCOUNT_BY_STR)
{
	printf("【KEDA_API_AMS_LIST_ACCOUNT_BY_STR】 -- 按姓名模糊查找本企业联系人\n");

	printf("   body = 0x%x    len = %d    httpCode = %d\n\n", body, len, httpCode );

	if ( body != NULL && len != 0 && httpCode == 200 )
	{
		wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
		if ( pRoot != NULL )
		{
			unsigned long iCount = 0;
			int outlen = 0;
			const char *pError = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			if ( pError != NULL )
			{
				iCount = 0;
				wbParserNS::t_wbParse_Error *pt_error = NULL;
				wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
				printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
				printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

				if ( handle != NULL )
				{
					wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
				}
			}
			else
			{
				printf("按姓名模糊查找本企业联系人成功！\n\n");

				iCount = 0;
				wbParserNS::t_wbParse_Keda_Page *pt_Page = NULL;
				wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(ListAccountByStr, Get, body, len, iCount,(void**)&pt_Page, NULL, NULL);

				printf("获得信息成功！\n");
				printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

				if( handle )
				{
					wbParserNS::USE_WBFREE_FUNC( ListAccountByStr,Get,handle);
				}
			}

			wbParserNS::Parse_free_XML(pRoot);
		}
		else
		{
			printf("【KEDA_API_AMS_LIST_ACCOUNT_BY_STR】  Parse_data_XML 失败！");
		}
	}
	printf("【KEDA_API_AMS_LIST_ACCOUNT_BY_STR】 -- 按姓名模糊查找本企业联系人\n");
}










// 会议管理
WEIBO_struct_statuscallback_init(KEDA_MEETING_GET_ACCOUNTTOKEN)
{
    STATUS_CALLBACK_PROCESS();	
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_GET_ACCOUNTTOKEN)
{
    printf("【KEDA_MEETING_GET_ACCOUNTTOKEN】 -- 获得会议的accountToken\n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );


    if ( body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            unsigned long iCount = 0;
            int outlen = 0;
            const char *p = wbParserNS::GetCHAR_Key_XML_EX("accountToken", pRoot, outlen);
            if ( p != NULL )
            {
                memcpy( g_szMeetingAccountToken, p, outlen );
                printf("获得 会议管理 <accountToken> = %s\n\n", p);
            }
            else
            {
                iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle =  wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR,ITEM,handle);
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_GET_ACCOUNTTOKEN 】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_LOGIN)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_headercallback_init(KEDA_MEETING_LOGIN)
{
    printf("【KEDA_MEETING_LOGIN】 -- 登录会议平台API -- 解析头\n");

    printf("   header = 0x%x    len = %d    httpCode = %d\n\n" , header, len, httpCode );

    WBParseCHAR cookieValue[2][512] = {0};

    printf("\n%s\n" , header );
    if( httpCode == 200 )
    {
        wb_parse_cookie_keda( header, cookieValue, 2 );

        printf("获得 %s\n\n" , cookieValue[0] );
        printf("获得 %s\n\n" , cookieValue[1] );

        memset(g_szMeetingCookieValue[0], 0, sizeof(g_szMeetingCookieValue[0]));
        memset(g_szMeetingCookieValue[1], 0, sizeof(g_szMeetingCookieValue[1]));

        memcpy(g_szMeetingCookieValue[0], cookieValue[0], strlen(cookieValue[0]));
        memcpy(g_szMeetingCookieValue[1], cookieValue[1], strlen(cookieValue[1]));
    }
    printf("【KEDA_MEETING_LOGIN】 -- 登录会议平台API -- 解析头\n\n");
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_LOGIN)
{
    printf("【KEDA_MEETING_LOGIN】 -- 登录会议平台API -- 解析body\n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if( pRoot )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                int outlen = 0;
                const char *pUsername = wbParserNS::GetCHAR_Key_XML_EX("username", pRoot, outlen);
                if ( pUsername != NULL )
                {
                    printf("获得 <username> = %s\n", pUsername);
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_LOGIN】  Parse_data_XML 失败！");
        }
    }
    printf("【KEDA_MEETING_LOGIN】 -- 登录会议平台API -- 解析body\n");
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_KEEP_HEARTBEAT)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_KEEP_HEARTBEAT)
{
    printf("【KEDA_MEETING_KEEP_HEARTBEAT】 -- 保持用户心跳(会议API)\n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                printf("(平台API)保持用户心跳 成功！\n");
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_KEEP_HEARTBEAT】  Parse_data_XML 失败！");
        }
    }
    printf("【KEDA_MEETING_KEEP_HEARTBEAT】 -- 保持用户心跳(会议API)\n");
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_MEETING_LIST)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_MEETING_LIST)
{
    printf("【KEDA_MEETING_MEETING_LIST】 -- 会议列表 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Keda_m_meeting_list * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MeetingList, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( MeetingList,Get,handle);
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_MEETING_LIST】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_MEETING_INFO)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_MEETING_INFO)
{
    printf("【KEDA_MEETING_MEETING_INFO】 -- 会议详细信息 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_m_meeting_info * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MeetingInfo, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( MeetingInfo,Get,handle);
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_MEETING_INFO】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_BOOKING_INFO)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_BOOKING_INFO)
{
    printf("【KEDA_MEETING_BOOKING_INFO】 -- 会议室预定信息 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Keda_m_meeting_list * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MeetingList, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC(MeetingList,Get,handle);
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_BOOKING_INFO】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_MEETING_LIST_OF_PERSON)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_MEETING_LIST_OF_PERSON)
{
    printf("【KEDA_MEETING_MEETING_LIST_OF_PERSON】 -- 某人会议信息 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Keda_m_meeting_list * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(MeetingList, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC(MeetingList,Get,handle);
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_MEETING_LIST_OF_PERSON】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_ADD_MEETING)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_ADD_MEETING)
{
    printf("【KEDA_MEETING_ADD_MEETING】 -- 添加会议 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                int outlen = 0;
                const char *pMeetingId = wbParserNS::GetCHAR_Key_XML_EX("meetingId", pRoot, outlen);
                if ( pMeetingId != NULL )
                {
                    printf("获得 <meetingId> = %s\n", pMeetingId );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_ADD_MEETING】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_MODIFY_MEETING)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_MODIFY_MEETING)
{
    printf("【KEDA_MEETING_MODIFY_MEETING】 -- 修改会议 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                int outlen = 0;
                const char *pMeetingId = wbParserNS::GetCHAR_Key_XML_EX("meetingId", pRoot, outlen);
                if ( pMeetingId != NULL )
                {
                    printf("获得 <meetingId> = %s\n", pMeetingId );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_MODIFY_MEETING】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_DELETE_MEETING)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_DELETE_MEETING)
{
    printf("【KEDA_MEETING_DELETE_MEETING】 -- 删除会议 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                int outlen = 0;
                const char *pMeetingId = wbParserNS::GetCHAR_Key_XML_EX("meetingId", pRoot, outlen);
                if ( pMeetingId != NULL )
                {
                    printf("获得 <meetingId> = %s\n", pMeetingId );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_DELETE_MEETING】  Parse_data_XML 失败！");
        }
    }
}

WEIBO_struct_statuscallback_init(KEDA_MEETING_START_MEETING)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_START_MEETING)
{
    printf("【KEDA_MEETING_START_MEETING】 -- 开始会议 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                int outlen = 0;
                const char *pMeetingId = wbParserNS::GetCHAR_Key_XML_EX("meetingId", pRoot, outlen);
                if ( pMeetingId != NULL )
                {
                    printf("获得 <meetingId> = %s\n", pMeetingId );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_START_MEETING】  Parse_data_XML 失败！");
        }
    }
}

WEIBO_struct_statuscallback_init(KEDA_MEETING_END_MEETING)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_END_MEETING)
{
    printf("【KEDA_MEETING_END_MEETING】 -- 结束会议 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                int outlen = 0;
                const char *pMeetingId = wbParserNS::GetCHAR_Key_XML_EX("meetingId", pRoot, outlen);
                if ( pMeetingId != NULL )
                {
                    printf("获得 <meetingId> = %s\n", pMeetingId );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_END_MEETING】  Parse_data_XML 失败！");
        }
    }
}

WEIBO_struct_statuscallback_init(KEDA_MEETING_FEED)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_FEED)
{
    printf("【KEDA_MEETING_FEED】 -- 会议反馈 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                int outlen = 0;
                const char *pMeetingId = wbParserNS::GetCHAR_Key_XML_EX("meetingId", pRoot, outlen);
                if ( pMeetingId != NULL )
                {
                    printf("获得 <meetingId> = %s\n", pMeetingId );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_FEED】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_QUERY_FREE_ROOMS)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_QUERY_FREE_ROOMS)
{
    printf("【KEDA_MEETING_QUERY_FREE_ROOMS】 -- 查询会议室 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_m_freerooms * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(FreeRoomList, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC(FreeRoomList,Get,handle);
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_QUERY_FREE_ROOMS】  Parse_data_XML 失败！");
        }
    }
}

WEIBO_struct_statuscallback_init(KEDA_MEETING_RECOMMENDED_V_ROOM)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_RECOMMENDED_V_ROOM)
{
    printf("【KEDA_MEETING_RECOMMENDED_V_ROOM】 -- 推荐的虚拟会议室 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                int outlen = 0;
                const char *pRoomId   = wbParserNS::GetCHAR_Key_XML_EX("id",   pRoot, outlen);
                const char *pRoomName = wbParserNS::GetCHAR_Key_XML_EX("name", pRoot, outlen);

                if ( pRoomId != NULL )
                {
                    printf("获得 <id> = %s\n", pRoomId );
                }

                if ( pRoomName != NULL )
                {
                    printf("获得 <name> = %s\n", pRoomName );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_RECOMMENDED_V_ROOM】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_NORMAL_ROOMS)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_NORMAL_ROOMS)
{
    printf("【KEDA_MEETING_NORMAL_ROOMS】 -- 查询正常的会议室 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_m_page_rooms * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC( RoomsPerPage, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( RoomsPerPage,Get,handle);
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_NORMAL_ROOMS】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_ROOM_INFO)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_ROOM_INFO)
{
    printf("【KEDA_MEETING_ROOM_INFO】 -- 查询会议室 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_m_room_info * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC( RoomInfo, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( RoomInfo, Get, handle );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_ROOM_INFO】  Parse_data_XML 失败！");
        }
    }
}

WEIBO_struct_statuscallback_init(KEDA_MEETING_ROOM_STATUS)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_ROOM_STATUS)
{
    printf("【KEDA_MEETING_ROOM_STATUS】 -- 查询会议室状态 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {

            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_ROOM_STATUS】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_ROOM_LIST_BY_NAME)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_ROOM_LIST_BY_NAME)
{
    printf("【KEDA_MEETING_ROOM_LIST_BY_NAME】 -- 根据名称查询会议室 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Keda_m_related_rooms * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC( RoomsByName, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( RoomsByName, Get, handle );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_ROOM_LIST_BY_NAME】  Parse_data_XML 失败！");
        }
    }
}

WEIBO_struct_statuscallback_init(KEDA_MEETING_QUERY_ROOMS)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_QUERY_ROOMS)
{
    printf("【KEDA_MEETING_QUERY_ROOMS】 -- 查询会议室 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Keda_m_related_rooms * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC( RoomsByName, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( RoomsByName, Get, handle );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_QUERY_ROOMS】  Parse_data_XML 失败！");
        }
    }
}


WEIBO_struct_statuscallback_init(KEDA_MEETING_TEMPLATE_LIST)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_TEMPLATE_LIST)
{
    printf("【KEDA_MEETING_TEMPLATE_LIST】 -- 查询会议模板 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_m_common_items * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC( TemplateList, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( TemplateList, Get, handle );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_TEMPLATE_LIST】  Parse_data_XML 失败！");
        }
    }
}

WEIBO_struct_statuscallback_init(KEDA_MEETING_TEMPLATE_INFO)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_TEMPLATE_INFO)
{
    printf("【KEDA_MEETING_TEMPLATE_INFO】 -- 查询会议模板信息 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_m_template_info * pt_Messages = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC( TemplateInfo, Get, body, len, iCount,(void**)&pt_Messages, NULL, NULL);

                printf("获得信息成功！\n");
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( TemplateInfo, Get, handle );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_TEMPLATE_INFO】  Parse_data_XML 失败！");
        }
    }
}



WEIBO_struct_statuscallback_init(KEDA_MEETING_NOTIFY)
{
    STATUS_CALLBACK_PROCESS();
}

WEIBO_struct_bodycallback_init(KEDA_MEETING_NOTIFY)
{
    printf("【KEDA_MEETING_NOTIFY】 -- 会议通知 \n");

    printf("   body = 0x%x    len = %d    httpCode = %d\n\n" , body, len, httpCode );

    if (body != NULL && len != 0 && httpCode == 200)
    {
        wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML(body);
        if ( pRoot != NULL )
        {
            if ( 0 == strcmp( wbParserNS::Get_XML_Name(pRoot), M_ERROR_ELEMENT ) )
            {
                unsigned long iCount = 0;
                wbParserNS::t_wbParse_Error *pt_error = NULL;
                wbParserNS::WBPARSE_HANDLE handle = wbParserNS::USE_WBPARSE_FUNC(WEIBO_ERR, ITEM, body, len, iCount,(void**)&pt_error, NULL, NULL);
                printf("获得 <code> = %d   <error> = %s   <request> = %s\n\n", pt_error->error_sub_code, pt_error->error, pt_error->request);
                printf("XML Size = %d    MemMalloc Size = %d\n\n", len, iCount);

                if ( handle )
                {
                    wbParserNS::USE_WBFREE_FUNC( WEIBO_ERR, ITEM, handle);
                }
            }
            else
            {
                int outlen = 0;
                const char *pMeetingId = wbParserNS::GetCHAR_Key_XML_EX("meetingId", pRoot, outlen);
                if ( pMeetingId != NULL )
                {
                    printf("获得 <meetingId> = %s\n", pMeetingId );
                }
            }

            wbParserNS::Parse_free_XML(pRoot);
        }
        else
        {
            printf("【KEDA_MEETING_NOTIFY】  Parse_data_XML 失败！");
        }
    }
}

static
struct t_wb_callback_byloach callback_byloach[  WEIBO_OPTION(LAST) ] =
{
#define INIT_CALLBACK_BYLOACH(NAME) {  WEIBO_struct_statuscallback_init_fun(NAME) , WEIBO_struct_bodycallback_init_fun(NAME) , NULL}
#define INIT_CALLBACK_BYSUNFEI(NAME) { WEIBO_struct_statuscallback_init_fun(NAME) , WEIBO_struct_bodycallback_init_fun(NAME) , WEIBO_struct_headercallback_init_fun(NAME)}

	{0,0,0},
	INIT_CALLBACK_BYLOACH(BASE),

	INIT_CALLBACK_BYLOACH(KEDA_API_P_GET_ACCOUNTTOKEN),     // 获得Oauth的accountToken(平台API)
	INIT_CALLBACK_BYLOACH(KEDA_API_W_GET_ACCOUNTTOKEN),     // 获得Oauth的accountToken(微博API)

	INIT_CALLBACK_BYSUNFEI(KEDA_API_P_LOGIN),               // 登录平台API
	INIT_CALLBACK_BYSUNFEI(KEDA_API_W_LOGIN),               // 登录微博API

	INIT_CALLBACK_BYLOACH(KEDA_API_GET_ME_AND_FRIENDS),     // 获得当前登录用户及关注用户的微博
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_USER_DATA),          // 获得用户发布的微博数据（友讯）
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_USER_ALL_DATA),      // 获得用户发布的微博数据（全部）
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_SUBJECT_AND_REPLY),  // 获得一条主题以及回复
	INIT_CALLBACK_BYLOACH(KEDA_API_MESSAGE_PUBLISH),        // 发布一条微博
	INIT_CALLBACK_BYLOACH(KEDA_API_MESSAGE_REPLY),          // 回复一条微博
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_USER_INFO),          // 获得用户信息

	INIT_CALLBACK_BYLOACH(KEDA_API_GET_COMPANY_PUBLIC_DATA),  // 获得公司广场的微博数据
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_GROUPS_DATA),          // 获得群组的微博数据
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_MENTIONS_DATA),        // 获得@当前用户的微博数据
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_REPLIES_DATA),         // 获得回复我的微博数据
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_LIKES_DATA),           // 获得我赞过的微博数据
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_FAVORITES_DATA),       // 获得我收藏的微博数据
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_TOPICS_DATA),          // 获得某个话题的微博数据
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_FRIENDSHIP_COMPANY),     // 获得公司人脉
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_FRIENDSHIP_ATTENTION),   // 获得我的关注列表
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_FRIENDSHIP_FOLLOWERS),   // 获得我的粉丝列表
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_FRIENDSHIP_FRIENDS),     // 获得我的好友列表

	INIT_CALLBACK_BYLOACH(KEDA_API_FRIENDSHIP_CREATE),          // 添加关注
	INIT_CALLBACK_BYLOACH(KEDA_API_FRIENDSHIP_DESTROY),         // 取消关注
	INIT_CALLBACK_BYLOACH(KEDA_API_FAVORITES_CREATE),           // 添加收藏
	INIT_CALLBACK_BYLOACH(KEDA_API_FAVORITES_DESTROY),          // 取消收藏
	INIT_CALLBACK_BYLOACH(KEDA_API_TOPIC_CREATE),               // 添加话题
	INIT_CALLBACK_BYLOACH(KEDA_API_TOPIC_DESTROY),              // 取消话题
	INIT_CALLBACK_BYLOACH(KEDA_API_LIKE_CREATE),                // 添加like
	INIT_CALLBACK_BYLOACH(KEDA_API_LIKE_DESTROY),               // 取消like
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_GROUP_USER),             // 获取我的群组列表
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_GROUP_ALL),              // 获得所有的群组列表
	INIT_CALLBACK_BYLOACH(KEDA_API_GROUP_CREATE),               // 添加群操作
	INIT_CALLBACK_BYLOACH(KEDA_API_GROUP_JOIN),                 // 加入群操作
	INIT_CALLBACK_BYLOACH(KEDA_API_GROUP_QUIT),                 // 退出群操作
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_PRIVATEMESSAGE_LIST),    // 查看我的私信列表
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_PRIVATEMESSAGE_USER),    // 查看我与某人的私信的聊天记录
	INIT_CALLBACK_BYLOACH(KEDA_API_PRIVATEMESSAGE_SEND),        // 发送私信到某人
	INIT_CALLBACK_BYLOACH(KEDA_API_PRIVATEMESSAGE_DELETE),      // 删除私信

	INIT_CALLBACK_BYLOACH(KEDA_API_AMS_VALIDATECLIENTACCOUNT),  // 验证终端账号
	INIT_CALLBACK_BYLOACH(KEDA_API_AMS_GETACCOUNTINFO),         // 查询账号详细信息
	INIT_CALLBACK_BYLOACH(KEDA_API_AMS_UPDATEACCOUNTINFO),      // 修改账号详细信息
	INIT_CALLBACK_BYLOACH(KEDA_API_AMS_UPDATEPASSWORD),         // 修改账号密码

	INIT_CALLBACK_BYLOACH(KEDA_API_GET_DEPARTMENT_ALL),         // 获得公司组织架构和所有人员信息
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_DEPARTMENT_COMPANY),     // 获取公司组织架构信息
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_DEPARTMENT_USER),        // 获取部门下人员信息(不包含子部门)
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_DEPARTMENT_ALLUSER),     // 获取部门下人员信息(包含子部门人员)
	INIT_CALLBACK_BYLOACH(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT),// 在组织架构中搜索部门信息
	INIT_CALLBACK_BYLOACH(KEDA_API_DEPARTMENT_SEARCHUSER),      // 在组织架构中搜索人员信息

	INIT_CALLBACK_BYLOACH(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID),  // 获取部门下的员工的在线状态
	INIT_CALLBACK_BYLOACH(KEDA_API_DEPARTMENT_ONLINE_BY_USERID), // 获取组织里用户的在线状态

	INIT_CALLBACK_BYLOACH(KEDA_API_GET_PORTRAIT),                // 获取头像
	INIT_CALLBACK_BYLOACH(KEDA_API_AMS_BATCHGETACCOUNTINFO),     // 批量查询账号详细信息
	INIT_CALLBACK_BYLOACH(KEDA_API_CONFIRM_PORTRAIT),            // 修改用户头像
	INIT_CALLBACK_BYLOACH(KEDA_API_DEPARTMENT_GET_VERSION),      // 获取组织架构版本信息
	INIT_CALLBACK_BYLOACH(KEDA_API_MESSAGE_UPLOAD),              // 带图片和附件的微博发布

	INIT_CALLBACK_BYLOACH(KEDA_API_MESSAGE_DELETE_THREAD),       // 删除自己的微博
	INIT_CALLBACK_BYLOACH(KEDA_API_MESSAGE_DELETE_REPLY),        // 删除自己的回复
	INIT_CALLBACK_BYLOACH(KEDA_API_NOTIFY_GET_ALL),              // 获取@、赞、粉丝等初始消息

	INIT_CALLBACK_BYLOACH(KEDA_API_P_KEEP_HEARTBEAT),            // 保持用户心跳(平台API)
	INIT_CALLBACK_BYLOACH(KEDA_API_W_KEEP_HEARTBEAT),            // 保持用户心跳(微博API)

	INIT_CALLBACK_BYLOACH(KEDA_API_GET_USER_INFO_BY_EMAIL),      // 获得用户信息(通过email)

	INIT_CALLBACK_BYLOACH(KEDA_API_GET_CURRENT_USER_INFO),       // 获得当前登录用户信息
	INIT_CALLBACK_BYLOACH(KEDA_API_GET_ARTICLE_URL),             // 获取长篇大论的url
	INIT_CALLBACK_BYLOACH(KEDA_API_AMS_UPDATEBRIEF),             // 修改个人介绍
	INIT_CALLBACK_BYLOACH(KEDA_API_AMS_LIST_ACCOUNT_BY_STR),     // 按姓名模糊查找本企业联系人

    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 },


    // 会议管理
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_GET_ACCOUNTTOKEN),         // 获得会议的accountToken
    INIT_CALLBACK_BYSUNFEI(KEDA_MEETING_LOGIN),                   // 登录
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_KEEP_HEARTBEAT),           // 保持用户心跳

    INIT_CALLBACK_BYLOACH(KEDA_MEETING_MEETING_LIST),             // 会议列表
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_MEETING_INFO),             // 会议详细信息
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_BOOKING_INFO),             // 会议室预定信息
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_MEETING_LIST_OF_PERSON),             // 某人的会议信息

    INIT_CALLBACK_BYLOACH(KEDA_MEETING_ADD_MEETING),             // 添加会议
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_MODIFY_MEETING),          // 修改会议
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_DELETE_MEETING),          // 修改会议
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_START_MEETING),           // 开始会议
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_END_MEETING),             // 结束会议
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_FEED),                    // 会议反馈
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_QUERY_FREE_ROOMS),                    
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_RECOMMENDED_V_ROOM),       
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_NORMAL_ROOMS),       
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_ROOM_INFO),       
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_ROOM_STATUS),       
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_ROOM_LIST_BY_NAME),       
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_QUERY_ROOMS),       
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_TEMPLATE_LIST),       
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_TEMPLATE_INFO),   
    INIT_CALLBACK_BYLOACH(KEDA_MEETING_NOTIFY),   
};

struct t_wb_callback_byloach* Wb_get_struct_callback( int option )
{
	if( WEIBO_OPTION(UNK) >= option || WEIBO_OPTION(LAST) <= option )
	{
		option = 0;
	}
	return &callback_byloach[option];
}






/**********************************************************/
/*     @brief weibo initialize struct & callback          */
/**********************************************************/

#define WEIBO_struct_init_callback(name) \
	static bool Wb_initstruct_##name##_cb(void* t_wb)

#define WEIBO_struct_init_fun(name) Wb_initstruct_##name##_cb

#define WEIBO_struct_cast( STRUCT ) \
struct STRUCT* pstruct = (struct STRUCT*)t_wb;\
	memset(pstruct ,0 , sizeof(struct STRUCT) );

#define SET_struct_callback_object(option , poauth ) \
{\
struct t_wb_callback_byloach* t_cb = Wb_get_struct_callback( option );\
	(poauth).fstatus_cb = t_cb->fstatus_cb;\
	(poauth).fbody_cb = t_cb->fbody_cb;\
	(poauth).fheader_cb = t_cb->fheader_cb;\
}

WEIBO_struct_init_callback(BASE)
{
	WEIBO_struct_cast(t_wb_oauth);

	return true;
}

//< ======

//1.获得Oauth的accountToken(平台API)
WEIBO_struct_init_callback(KEDA_API_P_GET_ACCOUNTTOKEN)
{
	WEIBO_struct_cast(t_wb_keda_get_accounttoken);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN), pstruct->wbauth_ );
	return true;
}

//1.获得Oauth的accountToken(微博API)
WEIBO_struct_init_callback(KEDA_API_W_GET_ACCOUNTTOKEN)
{
	WEIBO_struct_cast(t_wb_keda_get_accounttoken);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN), pstruct->wbauth_ );
	return true;
}

//2.登录平台API
WEIBO_struct_init_callback(KEDA_API_P_LOGIN)
{
	WEIBO_struct_cast(t_wb_keda_login);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_P_LOGIN), pstruct->wbauth_ );
	return true;
}

//2.登录微博API
WEIBO_struct_init_callback(KEDA_API_W_LOGIN)
{
	WEIBO_struct_cast(t_wb_keda_login);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_W_LOGIN), pstruct->wbauth_ );
	return true;
}

//3.获得当前登录用户及关注用户的微博
WEIBO_struct_init_callback(KEDA_API_GET_ME_AND_FRIENDS)
{
	WEIBO_struct_cast(t_wb_keda_get_me_and_friends);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS), pstruct->wbauth_ );
	return true;
}

//4.获得用户发布的微博数据（友讯） 
WEIBO_struct_init_callback(KEDA_API_GET_USER_DATA)
{
	WEIBO_struct_cast(t_wb_keda_get_user_data);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_USER_DATA), pstruct->wbauth_ );
	return true;
}

//5.获得用户发布的微博数据（全部） 
WEIBO_struct_init_callback(KEDA_API_GET_USER_ALL_DATA)
{
	WEIBO_struct_cast(t_wb_keda_get_user_all_data);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA), pstruct->wbauth_ );
	return true;
}

//6.获得一条主题以及回复 
WEIBO_struct_init_callback(KEDA_API_GET_SUBJECT_AND_REPLY)
{
	WEIBO_struct_cast(t_wb_keda_get_subject_and_reply);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY), pstruct->wbauth_ );
	return true;
}

//7.发布一条微博
WEIBO_struct_init_callback(KEDA_API_MESSAGE_PUBLISH)
{
	WEIBO_struct_cast(t_wb_keda_message_publish);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH), pstruct->wbauth_ );
	return true;
}

//8.回复一条微博 
WEIBO_struct_init_callback(KEDA_API_MESSAGE_REPLY)
{
	WEIBO_struct_cast(t_wb_keda_message_reply);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_MESSAGE_REPLY), pstruct->wbauth_ );
	return true;
}

//9.获得用户信息
WEIBO_struct_init_callback(KEDA_API_GET_USER_INFO)
{
	WEIBO_struct_cast(t_wb_keda_get_user_info);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_USER_INFO), pstruct->wbauth_ );
	return true;
}

//< ======

//1.获得公司广场的微博数据
WEIBO_struct_init_callback(KEDA_API_GET_COMPANY_PUBLIC_DATA)
{
	WEIBO_struct_cast(t_wb_keda_get_company_public_data);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA), pstruct->wbauth_ );
	return true;
}

//2.获得群组的微博数据
WEIBO_struct_init_callback(KEDA_API_GET_GROUPS_DATA)
{
	WEIBO_struct_cast(t_wb_keda_get_groups_data);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA), pstruct->wbauth_ );
	return true;
}

//3.获得@当前用户的微博数据
WEIBO_struct_init_callback(KEDA_API_GET_MENTIONS_DATA)
{
	WEIBO_struct_cast(t_wb_keda_get_mentions_data);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA), pstruct->wbauth_ );
	return true;
}

//4.获得回复我的微博数据 
WEIBO_struct_init_callback(KEDA_API_GET_REPLIES_DATA)
{
	WEIBO_struct_cast(t_wb_keda_get_replies_data);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA), pstruct->wbauth_ );
	return true;
}

//5.获得我赞过的微博数据
WEIBO_struct_init_callback(KEDA_API_GET_LIKES_DATA)
{
	WEIBO_struct_cast(t_wb_keda_get_likes_data);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_LIKES_DATA), pstruct->wbauth_ );
	return true;
}

//6.获得我收藏的微博数据 
WEIBO_struct_init_callback(KEDA_API_GET_FAVORITES_DATA)
{
	WEIBO_struct_cast(t_wb_keda_get_favorites_data);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA), pstruct->wbauth_ );
	return true;
}

//7.获得某个话题的微博数据
WEIBO_struct_init_callback(KEDA_API_GET_TOPICS_DATA)
{
	WEIBO_struct_cast(t_wb_keda_get_topics_data);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA), pstruct->wbauth_ );
	return true;
}

//8.获得公司人脉 
WEIBO_struct_init_callback(KEDA_API_GET_FRIENDSHIP_COMPANY)
{
	WEIBO_struct_cast(t_wb_keda_get_friendship_company);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY), pstruct->wbauth_ );
	return true;
}

//9.获得我的关注列表
WEIBO_struct_init_callback(KEDA_API_GET_FRIENDSHIP_ATTENTION)
{
	WEIBO_struct_cast(t_wb_keda_get_friendship_attention);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION), pstruct->wbauth_ );
	return true;
}

//10.获得我的粉丝列表
WEIBO_struct_init_callback(KEDA_API_GET_FRIENDSHIP_FOLLOWERS)
{
	WEIBO_struct_cast(t_wb_keda_get_friendship_followers);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS), pstruct->wbauth_ );
	return true;
}

//11.获得我的好友列表
WEIBO_struct_init_callback(KEDA_API_GET_FRIENDSHIP_FRIENDS)
{
	WEIBO_struct_cast(t_wb_keda_get_friendship_friends);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS), pstruct->wbauth_ );
	return true;
}

//< ======

//12.添加关注
WEIBO_struct_init_callback(KEDA_API_FRIENDSHIP_CREATE)
{
	WEIBO_struct_cast(t_wb_keda_friendship_create);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE), pstruct->wbauth_ );
	return true;
}

//13.取消关注 
WEIBO_struct_init_callback(KEDA_API_FRIENDSHIP_DESTROY)
{
	WEIBO_struct_cast(t_wb_keda_friendship_destroy);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY), pstruct->wbauth_ );
	return true;
}

//14.添加收藏
WEIBO_struct_init_callback(KEDA_API_FAVORITES_CREATE)
{
	WEIBO_struct_cast(t_wb_keda_favorites_create);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_FAVORITES_CREATE), pstruct->wbauth_ );
	return true;
}

//15.取消收藏
WEIBO_struct_init_callback(KEDA_API_FAVORITES_DESTROY)
{
	WEIBO_struct_cast(t_wb_keda_favorites_destroy);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY), pstruct->wbauth_ );
	return true;
}

//16.添加话题
WEIBO_struct_init_callback(KEDA_API_TOPIC_CREATE)
{
	WEIBO_struct_cast(t_wb_keda_topic_create);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_TOPIC_CREATE), pstruct->wbauth_ );
	return true;
}

//17.取消话题 
WEIBO_struct_init_callback(KEDA_API_TOPIC_DESTROY)
{
	WEIBO_struct_cast(t_wb_keda_topic_destroy);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_TOPIC_DESTROY), pstruct->wbauth_ );
	return true;
}

//18.添加like
WEIBO_struct_init_callback(KEDA_API_LIKE_CREATE)
{
	WEIBO_struct_cast(t_wb_keda_like_create);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_LIKE_CREATE), pstruct->wbauth_ );
	return true;
}

//19.取消like 
WEIBO_struct_init_callback(KEDA_API_LIKE_DESTROY)
{
	WEIBO_struct_cast(t_wb_keda_like_destroy);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_LIKE_DESTROY), pstruct->wbauth_ );
	return true;
}

//20.获取我的群组列表
WEIBO_struct_init_callback(KEDA_API_GET_GROUP_USER)
{
	WEIBO_struct_cast(t_wb_keda_get_group_user);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_GROUP_USER), pstruct->wbauth_ );
	return true;
}

//21.获得所有的群组列表
WEIBO_struct_init_callback(KEDA_API_GET_GROUP_ALL)
{
	WEIBO_struct_cast(t_wb_keda_get_group_all);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_GROUP_ALL), pstruct->wbauth_ );
	return true;
}

//22.添加群操作
WEIBO_struct_init_callback(KEDA_API_GROUP_CREATE)
{
	WEIBO_struct_cast(t_wb_keda_group_create);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GROUP_CREATE), pstruct->wbauth_ );
	return true;
}

//23.加入群操作
WEIBO_struct_init_callback(KEDA_API_GROUP_JOIN)
{
	WEIBO_struct_cast(t_wb_keda_group_join);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GROUP_JOIN), pstruct->wbauth_ );
	return true;
}

//24.退出群操作
WEIBO_struct_init_callback(KEDA_API_GROUP_QUIT)
{
	WEIBO_struct_cast(t_wb_keda_group_quit);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GROUP_QUIT), pstruct->wbauth_ );
	return true;
}

//25.查看我的私信列表
WEIBO_struct_init_callback(KEDA_API_GET_PRIVATEMESSAGE_LIST)
{
	WEIBO_struct_cast(t_wb_keda_get_privatemessage_list);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST), pstruct->wbauth_ );
	return true;
}

//26.查看我与某人的私信的聊天记录 
WEIBO_struct_init_callback(KEDA_API_GET_PRIVATEMESSAGE_USER)
{
	WEIBO_struct_cast(t_wb_keda_get_privatemessage_user);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER), pstruct->wbauth_ );
	return true;
}

//27.发送私信到某人
WEIBO_struct_init_callback(KEDA_API_PRIVATEMESSAGE_SEND)
{
	WEIBO_struct_cast(t_wb_keda_privatemessage_send);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND), pstruct->wbauth_ );
	return true;
}

//28.删除私信 
WEIBO_struct_init_callback(KEDA_API_PRIVATEMESSAGE_DELETE)
{
	WEIBO_struct_cast(t_wb_keda_privatemessage_delete);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE), pstruct->wbauth_ );
	return true;
}

//< ======

//29.验证终端账号 
WEIBO_struct_init_callback(KEDA_API_AMS_VALIDATECLIENTACCOUNT)
{
	WEIBO_struct_cast(t_wb_keda_ams_validateClientAccount);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT), pstruct->wbauth_ );
	return true;
}

//30.查询账号详细信息 
WEIBO_struct_init_callback(KEDA_API_AMS_GETACCOUNTINFO)
{
	WEIBO_struct_cast(t_wb_keda_ams_getAccountInfo);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO), pstruct->wbauth_ );
	return true;
}

//31.修改账号详细信息 
WEIBO_struct_init_callback(KEDA_API_AMS_UPDATEACCOUNTINFO)
{
	WEIBO_struct_cast(t_wb_keda_ams_updateAccountInfo);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO), pstruct->wbauth_ );
	return true;
}

//32.修改账号密码 
WEIBO_struct_init_callback(KEDA_API_AMS_UPDATEPASSWORD)
{
	WEIBO_struct_cast(t_wb_keda_ams_updatePassword);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD), pstruct->wbauth_ );
	return true;
}

//< ======

//33.获得公司组织架构和所有人员信息
WEIBO_struct_init_callback(KEDA_API_GET_DEPARTMENT_ALL)
{
	WEIBO_struct_cast(t_wb_keda_get_department_all);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL), pstruct->wbauth_ );
	return true;
}

//34.获取公司组织架构信息 
WEIBO_struct_init_callback(KEDA_API_GET_DEPARTMENT_COMPANY)
{
	WEIBO_struct_cast(t_wb_keda_get_department_company);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY), pstruct->wbauth_ );
	return true;
}

//35.获取部门下人员信息(不包含子部门) 
WEIBO_struct_init_callback(KEDA_API_GET_DEPARTMENT_USER)
{
	WEIBO_struct_cast(t_wb_keda_get_department_user);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER), pstruct->wbauth_ );
	return true;
}

//36.获取部门下人员信息(包含子部门人员)
WEIBO_struct_init_callback(KEDA_API_GET_DEPARTMENT_ALLUSER)
{
	WEIBO_struct_cast(t_wb_keda_get_department_allUser);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER), pstruct->wbauth_ );
	return true;
}

//37.在组织架构中搜索部门信息 
WEIBO_struct_init_callback(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT)
{
	WEIBO_struct_cast(t_wb_keda_department_searchDepartment);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT), pstruct->wbauth_ );
	return true;
}

//38.在组织架构中搜索人员信息
WEIBO_struct_init_callback(KEDA_API_DEPARTMENT_SEARCHUSER)
{
	WEIBO_struct_cast(t_wb_keda_department_searchUser);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER), pstruct->wbauth_ );
	return true;
}

//39.获取部门下的员工的在线状态
WEIBO_struct_init_callback(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID)
{
	WEIBO_struct_cast(t_wb_keda_department_online_by_depId);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID), pstruct->wbauth_ );
	return true;
}

//40.获取组织里用户的在线状态
WEIBO_struct_init_callback(KEDA_API_DEPARTMENT_ONLINE_BY_USERID)
{
	WEIBO_struct_cast(t_wb_keda_department_online_by_userId);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID), pstruct->wbauth_ );
	return true;
}

//41.获取头像
WEIBO_struct_init_callback(KEDA_API_GET_PORTRAIT)
{
	WEIBO_struct_cast(t_wb_keda_get_portrait);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_PORTRAIT), pstruct->wbauth_ );
	return true;
}

//42. 批量查询账号详细信息
WEIBO_struct_init_callback(KEDA_API_AMS_BATCHGETACCOUNTINFO)
{
	WEIBO_struct_cast(t_wb_keda_ams_batchGetAccountInfo);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO), pstruct->wbauth_ );
	return true;
}

//43. 修改用户头像
WEIBO_struct_init_callback(KEDA_API_CONFIRM_PORTRAIT)
{
	WEIBO_struct_cast(t_wb_keda_confirmPortrait);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT), pstruct->wbauth_ );
	return true;
}

//44. 获取组织架构版本信息
WEIBO_struct_init_callback(KEDA_API_DEPARTMENT_GET_VERSION)
{
	WEIBO_struct_cast(t_wb_keda_department_get_version);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION), pstruct->wbauth_ );
	return true;
}

//45. 带图片和附件的微博发布
WEIBO_struct_init_callback(KEDA_API_MESSAGE_UPLOAD)
{
	WEIBO_struct_cast(t_wb_keda_message_upload);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD), pstruct->wbauth_ );
	return true;
}

//46. 删除自己的微博
WEIBO_struct_init_callback(KEDA_API_MESSAGE_DELETE_THREAD)
{
	WEIBO_struct_cast(t_wb_keda_message_delete_thread);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD), pstruct->wbauth_ );
	return true;
}

//47. 删除自己的回复
WEIBO_struct_init_callback(KEDA_API_MESSAGE_DELETE_REPLY)
{
	WEIBO_struct_cast(t_wb_keda_message_delete_reply);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY), pstruct->wbauth_ );
	return true;
}

//48. 获取@、赞、粉丝等初始消息
WEIBO_struct_init_callback(KEDA_API_NOTIFY_GET_ALL)
{
	WEIBO_struct_cast(t_wb_keda_notify_get_all);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL), pstruct->wbauth_ );
	return true;
}

//49. 保持用户心跳(平台API)
WEIBO_struct_init_callback(KEDA_API_P_KEEP_HEARTBEAT)
{
	WEIBO_struct_cast(t_wb_keda_keep_heartbeat);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT), pstruct->wbauth_ );
	return true;
}

//49. 保持用户心跳(微博API)
WEIBO_struct_init_callback(KEDA_API_W_KEEP_HEARTBEAT)
{
	WEIBO_struct_cast(t_wb_keda_keep_heartbeat);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT), pstruct->wbauth_ );
	return true;
}

//50. 获得用户信息(通过email)
WEIBO_struct_init_callback(KEDA_API_GET_USER_INFO_BY_EMAIL)
{
	WEIBO_struct_cast(t_wb_keda_get_user_info_by_email);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL), pstruct->wbauth_ );
	return true;
}

//51. 获得当前登录用户信息
WEIBO_struct_init_callback(KEDA_API_GET_CURRENT_USER_INFO)
{
	WEIBO_struct_cast(t_wb_keda_get_current_user_info);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO), pstruct->wbauth_ );
	return true;
}

//52. 获取长篇大论的url
WEIBO_struct_init_callback(KEDA_API_GET_ARTICLE_URL)
{
	WEIBO_struct_cast(t_wb_keda_get_article_url);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL), pstruct->wbauth_ );
	return true;
}

//53. 修改个人介绍
WEIBO_struct_init_callback(KEDA_API_AMS_UPDATEBRIEF)
{
	WEIBO_struct_cast(t_wb_keda_ams_updateBrief);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF), pstruct->wbauth_ );
	return true;
}

//54. 按姓名模糊查找本企业联系人
WEIBO_struct_init_callback(KEDA_API_AMS_LIST_ACCOUNT_BY_STR)
{
	WEIBO_struct_cast(t_wb_keda_ams_list_account_by_str);
	SET_struct_callback_object( WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR), pstruct->wbauth_ );
	return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_GET_ACCOUNTTOKEN)
{
    WEIBO_struct_cast(t_wb_keda_get_accounttoken);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_LOGIN)
{
    WEIBO_struct_cast(t_wb_keda_login);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_LOGIN), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_KEEP_HEARTBEAT)
{
    WEIBO_struct_cast(t_wb_keda_keep_heartbeat);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_KEEP_HEARTBEAT), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_MEETING_LIST)
{
    WEIBO_struct_cast(t_wb_keda_m_meeting_list);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_MEETING_LIST), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_MEETING_INFO)
{
    WEIBO_struct_cast(t_wb_keda_m_meeting_info);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_MEETING_INFO), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_BOOKING_INFO)
{
    WEIBO_struct_cast(t_wb_keda_m_booking_info);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_BOOKING_INFO), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_MEETING_LIST_OF_PERSON)
{
    WEIBO_struct_cast(t_wb_keda_m_meeting_list_of_person);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_MEETING_LIST_OF_PERSON), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_ADD_MEETING)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_ADD_MEETING), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_MODIFY_MEETING)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_MODIFY_MEETING), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_DELETE_MEETING)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_DELETE_MEETING), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_START_MEETING)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_START_MEETING), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_END_MEETING)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_END_MEETING), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_FEED)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_FEED), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_QUERY_FREE_ROOMS)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_QUERY_FREE_ROOMS), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_RECOMMENDED_V_ROOM)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_RECOMMENDED_V_ROOM), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_NORMAL_ROOMS)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_NORMAL_ROOMS), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_ROOM_INFO)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_ROOM_INFO), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_ROOM_STATUS)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_ROOM_STATUS), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_ROOM_LIST_BY_NAME)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_ROOM_LIST_BY_NAME), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_QUERY_ROOMS)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_QUERY_ROOMS), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_TEMPLATE_LIST)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_TEMPLATE_LIST), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_TEMPLATE_INFO)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_TEMPLATE_INFO), pstruct->wbauth_ );
    return true;
}

WEIBO_struct_init_callback(KEDA_MEETING_NOTIFY)
{
    WEIBO_struct_cast(t_wb_base);
    SET_struct_callback_object( WEIBO_OPTION(KEDA_MEETING_NOTIFY), pstruct->wbauth_ );
    return true;
}

typedef bool (*fun_init_wb_struct_callback)(void* t_wb);
static fun_init_wb_struct_callback vector_init_callback[ WEIBO_OPTION(LAST) ] =
{
	NULL,
	WEIBO_struct_init_fun(BASE),                             //1

	WEIBO_struct_init_fun(KEDA_API_P_GET_ACCOUNTTOKEN),     //82
	WEIBO_struct_init_fun(KEDA_API_W_GET_ACCOUNTTOKEN),     //82

	WEIBO_struct_init_fun(KEDA_API_P_LOGIN),                //83
	WEIBO_struct_init_fun(KEDA_API_W_LOGIN),                //83

	WEIBO_struct_init_fun(KEDA_API_GET_ME_AND_FRIENDS),     //84
	WEIBO_struct_init_fun(KEDA_API_GET_USER_DATA),          //85
	WEIBO_struct_init_fun(KEDA_API_GET_USER_ALL_DATA),      //86
	WEIBO_struct_init_fun(KEDA_API_GET_SUBJECT_AND_REPLY),  //87
	WEIBO_struct_init_fun(KEDA_API_MESSAGE_PUBLISH),        //88
	WEIBO_struct_init_fun(KEDA_API_MESSAGE_REPLY),          //89
	WEIBO_struct_init_fun(KEDA_API_GET_USER_INFO),          //90

	WEIBO_struct_init_fun(KEDA_API_GET_COMPANY_PUBLIC_DATA),  //91
	WEIBO_struct_init_fun(KEDA_API_GET_GROUPS_DATA),          //92
	WEIBO_struct_init_fun(KEDA_API_GET_MENTIONS_DATA),        //93
	WEIBO_struct_init_fun(KEDA_API_GET_REPLIES_DATA),         //94
	WEIBO_struct_init_fun(KEDA_API_GET_LIKES_DATA),           //95
	WEIBO_struct_init_fun(KEDA_API_GET_FAVORITES_DATA),       //96
	WEIBO_struct_init_fun(KEDA_API_GET_TOPICS_DATA),          //97
	WEIBO_struct_init_fun(KEDA_API_GET_FRIENDSHIP_COMPANY),   //98
	WEIBO_struct_init_fun(KEDA_API_GET_FRIENDSHIP_ATTENTION), //99
	WEIBO_struct_init_fun(KEDA_API_GET_FRIENDSHIP_FOLLOWERS), //100
	WEIBO_struct_init_fun(KEDA_API_GET_FRIENDSHIP_FRIENDS),   //101

	WEIBO_struct_init_fun(KEDA_API_FRIENDSHIP_CREATE),        //102
	WEIBO_struct_init_fun(KEDA_API_FRIENDSHIP_DESTROY),       //103
	WEIBO_struct_init_fun(KEDA_API_FAVORITES_CREATE),         //104
	WEIBO_struct_init_fun(KEDA_API_FAVORITES_DESTROY),        //105
	WEIBO_struct_init_fun(KEDA_API_TOPIC_CREATE),             //106
	WEIBO_struct_init_fun(KEDA_API_TOPIC_DESTROY),            //107
	WEIBO_struct_init_fun(KEDA_API_LIKE_CREATE),              //108
	WEIBO_struct_init_fun(KEDA_API_LIKE_DESTROY),             //109
	WEIBO_struct_init_fun(KEDA_API_GET_GROUP_USER),           //110
	WEIBO_struct_init_fun(KEDA_API_GET_GROUP_ALL),            //111
	WEIBO_struct_init_fun(KEDA_API_GROUP_CREATE),             //112
	WEIBO_struct_init_fun(KEDA_API_GROUP_JOIN),               //113
	WEIBO_struct_init_fun(KEDA_API_GROUP_QUIT),               //114
	WEIBO_struct_init_fun(KEDA_API_GET_PRIVATEMESSAGE_LIST),  //115
	WEIBO_struct_init_fun(KEDA_API_GET_PRIVATEMESSAGE_USER),  //116
	WEIBO_struct_init_fun(KEDA_API_PRIVATEMESSAGE_SEND),      //117
	WEIBO_struct_init_fun(KEDA_API_PRIVATEMESSAGE_DELETE),    //118

	WEIBO_struct_init_fun(KEDA_API_AMS_VALIDATECLIENTACCOUNT), //119
	WEIBO_struct_init_fun(KEDA_API_AMS_GETACCOUNTINFO),        //120
	WEIBO_struct_init_fun(KEDA_API_AMS_UPDATEACCOUNTINFO),     //121
	WEIBO_struct_init_fun(KEDA_API_AMS_UPDATEPASSWORD),        //122

	WEIBO_struct_init_fun(KEDA_API_GET_DEPARTMENT_ALL),          //123
	WEIBO_struct_init_fun(KEDA_API_GET_DEPARTMENT_COMPANY),      //124
	WEIBO_struct_init_fun(KEDA_API_GET_DEPARTMENT_USER),         //125
	WEIBO_struct_init_fun(KEDA_API_GET_DEPARTMENT_ALLUSER),      //126
	WEIBO_struct_init_fun(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT), //127
	WEIBO_struct_init_fun(KEDA_API_DEPARTMENT_SEARCHUSER),       //128

	WEIBO_struct_init_fun(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID),  //129
	WEIBO_struct_init_fun(KEDA_API_DEPARTMENT_ONLINE_BY_USERID), //130
	WEIBO_struct_init_fun(KEDA_API_GET_PORTRAIT),                //131
	WEIBO_struct_init_fun(KEDA_API_AMS_BATCHGETACCOUNTINFO),     //132
	WEIBO_struct_init_fun(KEDA_API_CONFIRM_PORTRAIT),            //133
	WEIBO_struct_init_fun(KEDA_API_DEPARTMENT_GET_VERSION),      //134
	WEIBO_struct_init_fun(KEDA_API_MESSAGE_UPLOAD),              //135

	WEIBO_struct_init_fun(KEDA_API_MESSAGE_DELETE_THREAD),       //136
	WEIBO_struct_init_fun(KEDA_API_MESSAGE_DELETE_REPLY),        //137
	WEIBO_struct_init_fun(KEDA_API_NOTIFY_GET_ALL),              //138

	WEIBO_struct_init_fun(KEDA_API_P_KEEP_HEARTBEAT),            //139
	WEIBO_struct_init_fun(KEDA_API_W_KEEP_HEARTBEAT),            //139

	WEIBO_struct_init_fun(KEDA_API_GET_USER_INFO_BY_EMAIL),      //140

	WEIBO_struct_init_fun(KEDA_API_GET_CURRENT_USER_INFO),       //141
	WEIBO_struct_init_fun(KEDA_API_GET_ARTICLE_URL),             //142
	WEIBO_struct_init_fun(KEDA_API_AMS_UPDATEBRIEF),             //143
	WEIBO_struct_init_fun(KEDA_API_AMS_LIST_ACCOUNT_BY_STR),     //144

    0, 0, 0, 0, 0,    


    // 会议管理
    WEIBO_struct_init_fun(KEDA_MEETING_GET_ACCOUNTTOKEN),        // 150
    WEIBO_struct_init_fun(KEDA_MEETING_LOGIN),                   // 151
    WEIBO_struct_init_fun(KEDA_MEETING_KEEP_HEARTBEAT),          // 152

    WEIBO_struct_init_fun(KEDA_MEETING_MEETING_LIST),            // 153
    WEIBO_struct_init_fun(KEDA_MEETING_MEETING_INFO),            // 154
    WEIBO_struct_init_fun(KEDA_MEETING_BOOKING_INFO),            // 155
    WEIBO_struct_init_fun(KEDA_MEETING_MEETING_LIST_OF_PERSON),            // 156
    WEIBO_struct_init_fun(KEDA_MEETING_ADD_MEETING),            // 157
    WEIBO_struct_init_fun(KEDA_MEETING_MODIFY_MEETING),            // 158
    WEIBO_struct_init_fun(KEDA_MEETING_DELETE_MEETING),            // 159
    WEIBO_struct_init_fun(KEDA_MEETING_START_MEETING),             // 160
    WEIBO_struct_init_fun(KEDA_MEETING_END_MEETING),             // 161
    WEIBO_struct_init_fun(KEDA_MEETING_FEED),             // 162
    WEIBO_struct_init_fun(KEDA_MEETING_QUERY_FREE_ROOMS),             // 163
    WEIBO_struct_init_fun(KEDA_MEETING_RECOMMENDED_V_ROOM),             // 164
    WEIBO_struct_init_fun(KEDA_MEETING_NORMAL_ROOMS),             // 165
    WEIBO_struct_init_fun(KEDA_MEETING_ROOM_INFO),             // 166
    WEIBO_struct_init_fun(KEDA_MEETING_ROOM_STATUS),             // 167
    WEIBO_struct_init_fun(KEDA_MEETING_ROOM_LIST_BY_NAME),             // 168
    WEIBO_struct_init_fun(KEDA_MEETING_QUERY_ROOMS),             // 169

    WEIBO_struct_init_fun(KEDA_MEETING_TEMPLATE_LIST),             // 170
    WEIBO_struct_init_fun(KEDA_MEETING_TEMPLATE_INFO),             // 171
    WEIBO_struct_init_fun(KEDA_MEETING_NOTIFY),                    // 172
};


bool Wb_init_wb_struct( int option , void* t_wb)
{
	if( WEIBO_OPTION(UNK) >= option ||
		WEIBO_OPTION(LAST) <= option )
	{
		return false;
	}
	return ( vector_init_callback[ option ] ) ? vector_init_callback[ option ](t_wb):false;
}





