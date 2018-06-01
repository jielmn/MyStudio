/**
*  @brief Micro blog request option.
*  @file  wbRequest.cpp
**/


#include "wbRequest.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



#define MALLOC(obj) \
	{\
	    size = sizeof(struct obj);\
		pnewstruct = malloc( size );\
		memset(pnewstruct,0,sizeof(obj));\
	}

void* Weibo_senddata_create(int type, const void* senddata)
{
	void* pnewstruct = NULL;
	int size = 0;

	if ( senddata == NULL )
	{
		printf("Weibo_senddata_create ==> senddata is NULL! Return!\n");
		return NULL;
	}

	switch( type )
	{
	case WEIBO_OPTION(BASE):
		MALLOC( t_wb_oauth ) ;
		break;

	//1.获得Oauth的accountToken(平台API)
	case WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN):
		MALLOC(  t_wb_keda_get_accounttoken );
		break;
	//1.获得Oauth的accountToken(微博API)
	case WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN):
		MALLOC(  t_wb_keda_get_accounttoken );
		break;

	//2.登录平台API
	case WEIBO_OPTION(KEDA_API_P_LOGIN):
		MALLOC(  t_wb_keda_login );
		break;
	//2.登录微博API
	case WEIBO_OPTION(KEDA_API_W_LOGIN):
		MALLOC(  t_wb_keda_login );
		break;

	//3.获得当前登录用户及关注用户的微博
	case WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS):
		MALLOC(  t_wb_keda_get_me_and_friends );
		break;
	//4.获得用户发布的微博数据（友讯）
	case WEIBO_OPTION(KEDA_API_GET_USER_DATA):
		MALLOC(  t_wb_keda_get_user_data );
		break;
	//5.获得用户发布的微博数据（全部）
	case WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA):
		MALLOC(  t_wb_keda_get_user_all_data );
		break;
	//6.获得一条主题以及回复
	case WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY):
		MALLOC(  t_wb_keda_get_subject_and_reply );
		break;
	//7.发布一条微博
	case WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH):
		MALLOC(  t_wb_keda_message_publish );
		break;
	//8.回复一条微博
	case WEIBO_OPTION(KEDA_API_MESSAGE_REPLY):
		MALLOC(  t_wb_keda_message_reply );
		break;
	//9.获得用户信息
	case WEIBO_OPTION(KEDA_API_GET_USER_INFO):
		MALLOC(  t_wb_keda_get_user_info );
		break;


	//1.获得公司广场的微博数据
	case WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA):
		MALLOC(  t_wb_keda_get_company_public_data );
		break;
	//2.获得群组的微博数据
	case WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA):
		MALLOC(  t_wb_keda_get_groups_data );
		break;
	//3.获得@当前用户的微博数据
	case WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA):
		MALLOC(  t_wb_keda_get_mentions_data );
		break;
	//4.获得回复我的微博数据
	case WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA):
		MALLOC(  t_wb_keda_get_replies_data );
		break;
	//5.获得我赞过的微博数据
	case WEIBO_OPTION(KEDA_API_GET_LIKES_DATA):
		MALLOC(  t_wb_keda_get_likes_data );
		break;
	//6.获得我收藏的微博数据
	case WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA):
		MALLOC(  t_wb_keda_get_favorites_data );
		break;
	//7.获得某个话题的微博数据
	case WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA):
		MALLOC(  t_wb_keda_get_topics_data );
		break;


	//8.获得公司人脉
	case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY):
		MALLOC(  t_wb_keda_get_friendship_company );
		break;
	//9.获得我的关注列表
	case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION):
		MALLOC(  t_wb_keda_get_friendship_attention );
		break;
	//10.获得我的粉丝列表
	case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS):
		MALLOC(  t_wb_keda_get_friendship_followers );
		break;
	//11.获得我的好友列表
	case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS):
		MALLOC(  t_wb_keda_get_friendship_friends );
		break;



	//12.添加关注
	case WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE):
		MALLOC(  t_wb_keda_friendship_create);
		break;
	//13.取消关注
	case WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY):
		MALLOC(  t_wb_keda_friendship_destroy );
		break;
	//14.添加收藏
	case WEIBO_OPTION(KEDA_API_FAVORITES_CREATE):
		MALLOC(  t_wb_keda_favorites_create );
		break;
	//15.取消收藏
	case WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY):
		MALLOC(  t_wb_keda_favorites_destroy );
		break;
	//16.添加话题
	case WEIBO_OPTION(KEDA_API_TOPIC_CREATE):
		MALLOC(  t_wb_keda_topic_create );
		break;
	//17.取消话题 
	case WEIBO_OPTION(KEDA_API_TOPIC_DESTROY):
		MALLOC(  t_wb_keda_topic_destroy );
		break;
	//18.添加like
	case WEIBO_OPTION(KEDA_API_LIKE_CREATE):
		MALLOC(  t_wb_keda_like_create );
		break;
	//19.取消like
	case WEIBO_OPTION(KEDA_API_LIKE_DESTROY):
		MALLOC(  t_wb_keda_like_create );
		break;


	//20.获取我的群组列表
	case WEIBO_OPTION(KEDA_API_GET_GROUP_USER):
		MALLOC(  t_wb_keda_get_group_user );
		break;
	//21.获得所有的群组列表
	case WEIBO_OPTION(KEDA_API_GET_GROUP_ALL):
		MALLOC(  t_wb_keda_get_group_all );
		break;
	//22.添加群操作 
	case WEIBO_OPTION(KEDA_API_GROUP_CREATE):
		MALLOC(  t_wb_keda_group_create );
		break;
	//23.加入群操作
	case WEIBO_OPTION(KEDA_API_GROUP_JOIN):
		MALLOC(  t_wb_keda_group_join );
		break;
	//24.退出群操作
	case WEIBO_OPTION(KEDA_API_GROUP_QUIT):
		MALLOC(  t_wb_keda_group_quit );
		break;


	//25.查看我的私信列表
	case WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST):
		MALLOC(  t_wb_keda_get_privatemessage_list );
		break;
	//26.查看我与某人的私信的聊天记录 
	case WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER):
		MALLOC(  t_wb_keda_get_privatemessage_user );
		break;
	//27.发送私信到某人
	case WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND):
		MALLOC(  t_wb_keda_privatemessage_send );
		break;
	//28.删除私信
	case WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE):
		MALLOC(  t_wb_keda_privatemessage_delete );
		break;


	//11.验证终端账号
	case WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT):
		MALLOC(  t_wb_keda_ams_validateClientAccount );
		break;
	//12.查询账号详细信息
	case WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO):
		MALLOC(  t_wb_keda_ams_getAccountInfo);
		break;
	//13.修改账号详细信息
	case WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO):
		MALLOC(  t_wb_keda_ams_updateAccountInfo );
		break;
	//14.修改账号密码
	case WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD):
		MALLOC(  t_wb_keda_ams_updatePassword );
		break;


	//1.获得公司组织架构和所有人员信息
	case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL):
		MALLOC(  t_wb_keda_get_department_all );
		break;
	//2..获取公司组织架构信息 
	case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY):
		MALLOC(  t_wb_keda_get_department_company);
		break;
	//3.获取部门下人员信息(不包含子部门)
	case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER):
		MALLOC(  t_wb_keda_get_department_user );
		break;
	//4.获取部门下人员信息(包含子部门人员)
	case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER):
		MALLOC(  t_wb_keda_get_department_allUser );
		break;
	//5.在组织架构中搜索部门信息
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT):
		MALLOC(  t_wb_keda_department_searchDepartment );
		break;
	//6.在组织架构中搜索人员信息
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER):
		MALLOC(  t_wb_keda_department_searchUser );
		break;

	//7.获取部门下的员工的在线状态
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID):
		MALLOC(  t_wb_keda_department_online_by_depId );
		break;
	//8.获取组织里用户的在线状态
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID):
		MALLOC(  t_wb_keda_department_online_by_userId );
		break;

	//9.获取头像
	case WEIBO_OPTION(KEDA_API_GET_PORTRAIT):
		MALLOC(  t_wb_keda_get_portrait );
		break;

	//10.批量查询账号详细信息
	case WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO):
		MALLOC(  t_wb_keda_ams_batchGetAccountInfo );
		break;

	//11.修改用户头像
	case WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT):
		MALLOC(  t_wb_keda_confirmPortrait );
		break;

	//12.获取组织架构版本信息
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION):
		MALLOC(  t_wb_keda_department_get_version );
		break;

	//13.带图片和附件的微博发布
	case WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD):
		MALLOC(  t_wb_keda_message_upload );
		break;

	//14.删除自己的微博
	case WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD):
		MALLOC(  t_wb_keda_message_delete_thread );
		break;

	//15.删除自己的回复
	case WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY):
		MALLOC(  t_wb_keda_message_delete_reply );
		break;

	//16.获取@、赞、粉丝等初始消息
	case WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL):
		MALLOC(  t_wb_keda_notify_get_all );
		break;

	//17.保持用户心跳(平台API)
	case WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT):
		MALLOC(  t_wb_keda_keep_heartbeat );
		break;
	//17.保持用户心跳(微博API)
	case WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT):
		MALLOC(  t_wb_keda_keep_heartbeat );
		break;

	//18.获得用户信息(通过email)
	case WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL):
		MALLOC(  t_wb_keda_get_user_info_by_email );
		break;

	//19.获得当前登录用户信息
	case WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO):
		MALLOC(  t_wb_keda_get_current_user_info );
		break;

	//20.获取长篇大论的url
	case WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL):
		MALLOC(  t_wb_keda_get_article_url );
		break;

	//21.修改个人介绍
	case WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF):
		MALLOC(  t_wb_keda_ams_updateBrief );
		break;

	//22.按姓名模糊查找本企业联系人
	case WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR):
		MALLOC(  t_wb_keda_ams_list_account_by_str );
		break;

	//23.忘记密码
	case WEIBO_OPTION(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL):
		MALLOC(  t_wb_keda_ams_password_reset_by_mail );
		break;

	//24.获取赞过我的微博数据
	case WEIBO_OPTION(KEDA_API_AMS_GET_LIKES_ME_DATA):
		MALLOC(  t_wb_keda_ams_get_likes_me_data );
		break;

	//25.设置是否被模糊搜索到
	case WEIBO_OPTION(KEDA_API_AMS_SET_IS_LIKE_QUERY):
		MALLOC(  t_wb_keda_ams_set_is_like_query );
		break;

	//26.设置添加联系人认证模式
	case WEIBO_OPTION(KEDA_API_AMS_SET_AUTH_MODE):
		MALLOC(  t_wb_keda_ams_set_auth_mode );
		break;

	//27.条件查询企业联系人	
	case WEIBO_OPTION(KEDA_API_AMS_CONDITION_QUERY):
		MALLOC(  t_wb_keda_ams_condition_query );
		break;

    // 会议管理
    case WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN):
        MALLOC(  t_wb_keda_get_accounttoken );
        break;

        //2.登录平台API
    case WEIBO_OPTION(KEDA_MEETING_LOGIN):
        MALLOC(  t_wb_keda_login );
        break;

    case WEIBO_OPTION(KEDA_MEETING_KEEP_HEARTBEAT):
        MALLOC(  t_wb_keda_keep_heartbeat );
        break;

    case WEIBO_OPTION(KEDA_MEETING_MEETING_LIST):
        MALLOC(  t_wb_keda_m_meeting_list );
        break;

    case WEIBO_OPTION(KEDA_MEETING_MEETING_INFO):
        MALLOC(  t_wb_keda_m_meeting_info );
        break;

    case WEIBO_OPTION(KEDA_MEETING_BOOKING_INFO):
        MALLOC(  t_wb_keda_m_booking_info );
        break;

    case WEIBO_OPTION(KEDA_MEETING_MEETING_LIST_OF_PERSON):
        MALLOC(  t_wb_keda_m_meeting_list_of_person );
        break;

    case WEIBO_OPTION(KEDA_MEETING_ADD_MEETING):
    case WEIBO_OPTION(KEDA_MEETING_MODIFY_MEETING):
        MALLOC(  t_wb_keda_m_set_meeting );
        break;

    case WEIBO_OPTION(KEDA_MEETING_DELETE_MEETING):
    case WEIBO_OPTION(KEDA_MEETING_START_MEETING):
    case WEIBO_OPTION(KEDA_MEETING_END_MEETING):
        MALLOC(  t_wb_keda_m_meeting_id );
        break;

    case WEIBO_OPTION(KEDA_MEETING_FEED):
        MALLOC(  t_wb_keda_m_meeting_feed );
        break;

    case WEIBO_OPTION(KEDA_MEETING_QUERY_FREE_ROOMS):
    case WEIBO_OPTION(KEDA_MEETING_RECOMMENDED_V_ROOM):
    case WEIBO_OPTION(KEDA_MEETING_NORMAL_ROOMS):
    case WEIBO_OPTION(KEDA_MEETING_ROOM_LIST_BY_NAME):
    case WEIBO_OPTION(KEDA_MEETING_QUERY_ROOMS):
        MALLOC(  t_wb_keda_m_query_rooms );
        break;

    case WEIBO_OPTION(KEDA_MEETING_ROOM_INFO):
    case WEIBO_OPTION(KEDA_MEETING_ROOM_STATUS):
        MALLOC(  t_wb_keda_m_id );
        break;

    case WEIBO_OPTION(KEDA_MEETING_TEMPLATE_LIST):
        MALLOC(  t_wb_keda_m_common_query );
        break;

    case WEIBO_OPTION(KEDA_MEETING_TEMPLATE_INFO):
        MALLOC(  t_wb_keda_m_common_id );
        break;

    case WEIBO_OPTION(KEDA_MEETING_NOTIFY_ADD):
        MALLOC(  t_wb_keda_m_notify );
        break;

    case WEIBO_OPTION(KEDA_MEETING_DEADLINE):
        MALLOC( t_wb_base ) ;
        break;

    case WEIBO_OPTION(KEDA_MEETING_LOCK_ROOM):
        MALLOC( t_wb_keda_m_lock_room ) ;
        break;

    case WEIBO_OPTION(KEDA_MEETING_LIST_NOTIFY_BY_MAX):
        MALLOC( t_wb_keda_m_list_notify_by_max ) ;
        break;

    case WEIBO_OPTION(KEDA_MEETING_GET_NOTIFY_BY_ID):
        MALLOC( t_wb_keda_m_get_notify_by_id ) ;
        break;

    case WEIBO_OPTION(KEDA_MEETING_DEL_BOOKED_ROOM):
        MALLOC( t_wb_keda_m_meeting_id ) ;
        break;

    case WEIBO_OPTION(KEDA_MEETING_IF_EXIST_LOCK_BY_FORMKEY):
        MALLOC( t_wb_keda_m_if_exist_lock_by_formkey ) ;
        break;

    case WEIBO_OPTION(KEDA_MEETING_GET_REGULAR):
        MALLOC( t_wb_keda_m_meeting_info ) ;
        break;

	default:
		return 0;
	}

	if ( pnewstruct != NULL && size != 0 )
	{
		memcpy( pnewstruct, senddata, size );
	}
	return pnewstruct;
}

void Weibo_senddata_free(void*& senddata)
{
	if ( senddata != NULL ) 
	{
		free( senddata );
		senddata = NULL;
	}
}

int  Weibo_start_request_asyn(struct t_wb_Request* reqSession)
{
	if ( reqSession == NULL )
	{
		printf("Weibo_start_request_asyn ==>   reqSession is NULL! Error!\n");
		return ERR_reqSessionParam_nil;
	}

	if ( reqSession->session_ == NULL )
	{
		reqSession->session_ = Weibo_httpsession_create();
		if ( reqSession->session_ == NULL )
		{
			printf("Weibo_start_request_asyn ==>   Weibo_httpsession_create failed!\n");
			return ERR_createhttpSession_nil;
		}
	}
	reqSession->session_->bAsyn_ = true;
	return Weibo_session_start(reqSession->session_, reqSession);
}

int  Weibo_start_request(struct t_wb_Request* reqSession)
{
	if ( reqSession == NULL )
	{
		return ERR_reqSessionParam_nil;
	}

	if ( reqSession->session_ == NULL )
	{
		reqSession->session_ = Weibo_httpsession_create();
		if ( reqSession->session_ == NULL )
		{
			return ERR_createhttpSession_nil;
		}
	}
	reqSession->session_->bAsyn_ = false;
	return Weibo_session_start( reqSession->session_, reqSession);
}

int Weibo_stop_request(struct t_wb_Request* reqSession)
{
	if ( reqSession == NULL )
	{
		printf("Weibo_stop_request ==> reqSession is NULL! Return ERR_reqSessionParam_nil!\n");
		return ERR_reqSessionParam_nil;
	}

	if ( reqSession->session_ == NULL )
	{
		printf("Weibo_stop_request ==> reqSession->session_ is NULL! Return ERR_OK!\n");
		return ERR_OK;
	}
	return Weibo_session_stop( reqSession->session_);
}

void Weibo_wait_request(struct t_wb_Request* reqSession)
{
	if ( reqSession == NULL )
	{
		printf("Weibo_wait_request ==> reqSession is NULL! Return!\n");
		return ;
	}

	if ( reqSession->session_ == NULL )
	{
		printf("Weibo_wait_request ==> reqSession->session_ is NULL! Return!\n");
		return ;
	}
	return Weibo_session_wait( reqSession->session_ );
}

