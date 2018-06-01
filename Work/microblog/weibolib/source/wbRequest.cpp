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

	//1.���Oauth��accountToken(ƽ̨API)
	case WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN):
		MALLOC(  t_wb_keda_get_accounttoken );
		break;
	//1.���Oauth��accountToken(΢��API)
	case WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN):
		MALLOC(  t_wb_keda_get_accounttoken );
		break;

	//2.��¼ƽ̨API
	case WEIBO_OPTION(KEDA_API_P_LOGIN):
		MALLOC(  t_wb_keda_login );
		break;
	//2.��¼΢��API
	case WEIBO_OPTION(KEDA_API_W_LOGIN):
		MALLOC(  t_wb_keda_login );
		break;

	//3.��õ�ǰ��¼�û�����ע�û���΢��
	case WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS):
		MALLOC(  t_wb_keda_get_me_and_friends );
		break;
	//4.����û�������΢�����ݣ���Ѷ��
	case WEIBO_OPTION(KEDA_API_GET_USER_DATA):
		MALLOC(  t_wb_keda_get_user_data );
		break;
	//5.����û�������΢�����ݣ�ȫ����
	case WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA):
		MALLOC(  t_wb_keda_get_user_all_data );
		break;
	//6.���һ�������Լ��ظ�
	case WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY):
		MALLOC(  t_wb_keda_get_subject_and_reply );
		break;
	//7.����һ��΢��
	case WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH):
		MALLOC(  t_wb_keda_message_publish );
		break;
	//8.�ظ�һ��΢��
	case WEIBO_OPTION(KEDA_API_MESSAGE_REPLY):
		MALLOC(  t_wb_keda_message_reply );
		break;
	//9.����û���Ϣ
	case WEIBO_OPTION(KEDA_API_GET_USER_INFO):
		MALLOC(  t_wb_keda_get_user_info );
		break;


	//1.��ù�˾�㳡��΢������
	case WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA):
		MALLOC(  t_wb_keda_get_company_public_data );
		break;
	//2.���Ⱥ���΢������
	case WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA):
		MALLOC(  t_wb_keda_get_groups_data );
		break;
	//3.���@��ǰ�û���΢������
	case WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA):
		MALLOC(  t_wb_keda_get_mentions_data );
		break;
	//4.��ûظ��ҵ�΢������
	case WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA):
		MALLOC(  t_wb_keda_get_replies_data );
		break;
	//5.������޹���΢������
	case WEIBO_OPTION(KEDA_API_GET_LIKES_DATA):
		MALLOC(  t_wb_keda_get_likes_data );
		break;
	//6.������ղص�΢������
	case WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA):
		MALLOC(  t_wb_keda_get_favorites_data );
		break;
	//7.���ĳ�������΢������
	case WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA):
		MALLOC(  t_wb_keda_get_topics_data );
		break;


	//8.��ù�˾����
	case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY):
		MALLOC(  t_wb_keda_get_friendship_company );
		break;
	//9.����ҵĹ�ע�б�
	case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION):
		MALLOC(  t_wb_keda_get_friendship_attention );
		break;
	//10.����ҵķ�˿�б�
	case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS):
		MALLOC(  t_wb_keda_get_friendship_followers );
		break;
	//11.����ҵĺ����б�
	case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS):
		MALLOC(  t_wb_keda_get_friendship_friends );
		break;



	//12.��ӹ�ע
	case WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE):
		MALLOC(  t_wb_keda_friendship_create);
		break;
	//13.ȡ����ע
	case WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY):
		MALLOC(  t_wb_keda_friendship_destroy );
		break;
	//14.����ղ�
	case WEIBO_OPTION(KEDA_API_FAVORITES_CREATE):
		MALLOC(  t_wb_keda_favorites_create );
		break;
	//15.ȡ���ղ�
	case WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY):
		MALLOC(  t_wb_keda_favorites_destroy );
		break;
	//16.��ӻ���
	case WEIBO_OPTION(KEDA_API_TOPIC_CREATE):
		MALLOC(  t_wb_keda_topic_create );
		break;
	//17.ȡ������ 
	case WEIBO_OPTION(KEDA_API_TOPIC_DESTROY):
		MALLOC(  t_wb_keda_topic_destroy );
		break;
	//18.���like
	case WEIBO_OPTION(KEDA_API_LIKE_CREATE):
		MALLOC(  t_wb_keda_like_create );
		break;
	//19.ȡ��like
	case WEIBO_OPTION(KEDA_API_LIKE_DESTROY):
		MALLOC(  t_wb_keda_like_create );
		break;


	//20.��ȡ�ҵ�Ⱥ���б�
	case WEIBO_OPTION(KEDA_API_GET_GROUP_USER):
		MALLOC(  t_wb_keda_get_group_user );
		break;
	//21.������е�Ⱥ���б�
	case WEIBO_OPTION(KEDA_API_GET_GROUP_ALL):
		MALLOC(  t_wb_keda_get_group_all );
		break;
	//22.���Ⱥ���� 
	case WEIBO_OPTION(KEDA_API_GROUP_CREATE):
		MALLOC(  t_wb_keda_group_create );
		break;
	//23.����Ⱥ����
	case WEIBO_OPTION(KEDA_API_GROUP_JOIN):
		MALLOC(  t_wb_keda_group_join );
		break;
	//24.�˳�Ⱥ����
	case WEIBO_OPTION(KEDA_API_GROUP_QUIT):
		MALLOC(  t_wb_keda_group_quit );
		break;


	//25.�鿴�ҵ�˽���б�
	case WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST):
		MALLOC(  t_wb_keda_get_privatemessage_list );
		break;
	//26.�鿴����ĳ�˵�˽�ŵ������¼ 
	case WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER):
		MALLOC(  t_wb_keda_get_privatemessage_user );
		break;
	//27.����˽�ŵ�ĳ��
	case WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND):
		MALLOC(  t_wb_keda_privatemessage_send );
		break;
	//28.ɾ��˽��
	case WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE):
		MALLOC(  t_wb_keda_privatemessage_delete );
		break;


	//11.��֤�ն��˺�
	case WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT):
		MALLOC(  t_wb_keda_ams_validateClientAccount );
		break;
	//12.��ѯ�˺���ϸ��Ϣ
	case WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO):
		MALLOC(  t_wb_keda_ams_getAccountInfo);
		break;
	//13.�޸��˺���ϸ��Ϣ
	case WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO):
		MALLOC(  t_wb_keda_ams_updateAccountInfo );
		break;
	//14.�޸��˺�����
	case WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD):
		MALLOC(  t_wb_keda_ams_updatePassword );
		break;


	//1.��ù�˾��֯�ܹ���������Ա��Ϣ
	case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL):
		MALLOC(  t_wb_keda_get_department_all );
		break;
	//2..��ȡ��˾��֯�ܹ���Ϣ 
	case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY):
		MALLOC(  t_wb_keda_get_department_company);
		break;
	//3.��ȡ��������Ա��Ϣ(�������Ӳ���)
	case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER):
		MALLOC(  t_wb_keda_get_department_user );
		break;
	//4.��ȡ��������Ա��Ϣ(�����Ӳ�����Ա)
	case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER):
		MALLOC(  t_wb_keda_get_department_allUser );
		break;
	//5.����֯�ܹ�������������Ϣ
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT):
		MALLOC(  t_wb_keda_department_searchDepartment );
		break;
	//6.����֯�ܹ���������Ա��Ϣ
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER):
		MALLOC(  t_wb_keda_department_searchUser );
		break;

	//7.��ȡ�����µ�Ա��������״̬
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID):
		MALLOC(  t_wb_keda_department_online_by_depId );
		break;
	//8.��ȡ��֯���û�������״̬
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID):
		MALLOC(  t_wb_keda_department_online_by_userId );
		break;

	//9.��ȡͷ��
	case WEIBO_OPTION(KEDA_API_GET_PORTRAIT):
		MALLOC(  t_wb_keda_get_portrait );
		break;

	//10.������ѯ�˺���ϸ��Ϣ
	case WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO):
		MALLOC(  t_wb_keda_ams_batchGetAccountInfo );
		break;

	//11.�޸��û�ͷ��
	case WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT):
		MALLOC(  t_wb_keda_confirmPortrait );
		break;

	//12.��ȡ��֯�ܹ��汾��Ϣ
	case WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION):
		MALLOC(  t_wb_keda_department_get_version );
		break;

	//13.��ͼƬ�͸�����΢������
	case WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD):
		MALLOC(  t_wb_keda_message_upload );
		break;

	//14.ɾ���Լ���΢��
	case WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD):
		MALLOC(  t_wb_keda_message_delete_thread );
		break;

	//15.ɾ���Լ��Ļظ�
	case WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY):
		MALLOC(  t_wb_keda_message_delete_reply );
		break;

	//16.��ȡ@���ޡ���˿�ȳ�ʼ��Ϣ
	case WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL):
		MALLOC(  t_wb_keda_notify_get_all );
		break;

	//17.�����û�����(ƽ̨API)
	case WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT):
		MALLOC(  t_wb_keda_keep_heartbeat );
		break;
	//17.�����û�����(΢��API)
	case WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT):
		MALLOC(  t_wb_keda_keep_heartbeat );
		break;

	//18.����û���Ϣ(ͨ��email)
	case WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL):
		MALLOC(  t_wb_keda_get_user_info_by_email );
		break;

	//19.��õ�ǰ��¼�û���Ϣ
	case WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO):
		MALLOC(  t_wb_keda_get_current_user_info );
		break;

	//20.��ȡ��ƪ���۵�url
	case WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL):
		MALLOC(  t_wb_keda_get_article_url );
		break;

	//21.�޸ĸ��˽���
	case WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF):
		MALLOC(  t_wb_keda_ams_updateBrief );
		break;

	//22.������ģ�����ұ���ҵ��ϵ��
	case WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR):
		MALLOC(  t_wb_keda_ams_list_account_by_str );
		break;

	//23.��������
	case WEIBO_OPTION(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL):
		MALLOC(  t_wb_keda_ams_password_reset_by_mail );
		break;

	//24.��ȡ�޹��ҵ�΢������
	case WEIBO_OPTION(KEDA_API_AMS_GET_LIKES_ME_DATA):
		MALLOC(  t_wb_keda_ams_get_likes_me_data );
		break;

	//25.�����Ƿ�ģ��������
	case WEIBO_OPTION(KEDA_API_AMS_SET_IS_LIKE_QUERY):
		MALLOC(  t_wb_keda_ams_set_is_like_query );
		break;

	//26.���������ϵ����֤ģʽ
	case WEIBO_OPTION(KEDA_API_AMS_SET_AUTH_MODE):
		MALLOC(  t_wb_keda_ams_set_auth_mode );
		break;

	//27.������ѯ��ҵ��ϵ��	
	case WEIBO_OPTION(KEDA_API_AMS_CONDITION_QUERY):
		MALLOC(  t_wb_keda_ams_condition_query );
		break;

    // �������
    case WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN):
        MALLOC(  t_wb_keda_get_accounttoken );
        break;

        //2.��¼ƽ̨API
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

