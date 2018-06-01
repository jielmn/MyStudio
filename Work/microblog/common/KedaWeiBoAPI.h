#ifndef _KEDA_WEIBO_API_
#define _KEDA_WEIBO_API_

#define  _API_VERSION_        "1.0.0.2"


#if (defined(WIN32) || defined(_WIN32) ) && !defined(KEDAWEIBOAPI_STATICLIB)

	#ifdef KEDAWEIBOAPI_EXPORTS
		#define KEDAWEIBOAPI_API __declspec(dllexport)
	#else
		#define KEDAWEIBOAPI_API __declspec(dllimport)
	#endif

#else  // ����ƽ̨�ģ������Ǿ�̬����

	#define KEDAWEIBOAPI_API

#endif


#include "libWeibo/weibo.h"

#ifdef  __cplusplus
extern "C" {
#endif

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_SetCommonProxy( const TWeiboProxyParam * pProxyParam );


/// ===��  �ƴ﹫˾ ΢��v0.1�ӿ�

//1.Oauth��accountToken�Ļ��(ƽ̨API)  -- 82
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//1.Oauth��accountToken�Ļ��(΢��API)  -- 82
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//2.��¼ƽ̨API  -- 83
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//2.��¼΢��API  -- 83
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//3.��õ�ǰ��¼�û�����ע�û���΢��  -- 84
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_me_and_friends(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//4.����û�������΢�����ݣ���Ѷ��  -- 85
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//5.����û�������΢�����ݣ�ȫ����  -- 86
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_all_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//6.���һ�������Լ��ظ�  -- 87
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_subject_and_reply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//7.����һ��΢��  -- 88
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_publish(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//8.�ظ�һ��΢��  -- 89
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_reply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//9.����û���Ϣ  -- 90
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===��  2012-03-03 �ƴ﹫˾ ΢��v1.0�ӿ� ��һ����

//1.��ù�˾�㳡��΢������  -- 91
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_company_public_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//2.���Ⱥ���΢������  -- 92
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_groups_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//3.���@��ǰ�û���΢������  -- 93
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_mentions_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//4.��ûظ��ҵ�΢������  -- 94
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_replies_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//5.������޹���΢������  -- 95
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_likes_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//6.������ղص�΢������  -- 96
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_favorites_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//7.���ĳ�������΢������  -- 97
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_topics_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//8.��ù�˾����  -- 98
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_company(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//9.����ҵĹ�ע�б�  -- 99
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_attention(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//10.����ҵķ�˿�б�  -- 100
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_followers(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//11.����ҵĺ����б�  -- 101
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_friends(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===��  2012-03-06 �ƴ﹫˾ ΢��v1.0�ӿ� �ڶ�����

//12.��ӹ�ע  -- 102
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_friendship_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//13.ȡ����ע  -- 103
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_friendship_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//14.����ղ�  -- 104
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_favorites_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//15.ȡ���ղ�  -- 105
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_favorites_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//16.��ӻ���  -- 106
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_topic_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//17.ȡ������  -- 107
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_topic_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//18.���like  -- 108
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_like_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//19.ȡ��like  -- 109
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_like_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//20.��ȡ�ҵ�Ⱥ���б�  -- 110
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_group_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//21.������е�Ⱥ���б�  -- 111
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_group_all(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//22.���Ⱥ����  -- 112
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//23.����Ⱥ����  -- 113
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_join(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//24.�˳�Ⱥ����  -- 114
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_quit(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//25.�鿴�ҵ�˽���б�  -- 115
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_privatemessage_list(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//26.�鿴����ĳ�˵�˽�ŵ������¼  -- 116
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_privatemessage_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//27.����˽�ŵ�ĳ��  -- 117
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_privatemessage_send(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//28.ɾ��˽��  -- 118
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_privatemessage_delete(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===��  2012-03-09 �ƴ﹫˾ ΢��v1.0�ӿ� ��������

//29.��֤�ն��˺�  -- 119
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_validateclientaccount(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//30.��ѯ�˺���ϸ��Ϣ  -- 120
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_getaccountinfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//31.�޸��˺���ϸ��Ϣ  -- 121
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updateaccountinfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//32.�޸��˺�����  -- 122
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updatepassword(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===��  2012-03-17 �ƴ﹫˾ ΢��v1.0�ӿ� ���Ĳ���

//33.��ù�˾��֯�ܹ���������Ա��Ϣ  -- 123
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_all(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//34.��ȡ��˾��֯�ܹ���Ϣ  -- 124
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_company(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//35.��ȡ��������Ա��Ϣ(�������Ӳ���)  -- 125
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//36.��ȡ��������Ա��Ϣ(�����Ӳ�����Ա)  -- 126
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_alluser(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//37.����֯�ܹ�������������Ϣ  -- 127
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_searchdepartment(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//38.����֯�ܹ���������Ա��Ϣ  -- 128
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_searchuser(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//39.��ȡ�����µ�Ա��������״̬  -- 129
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_onlineByDepId(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//40.��ȡ��֯���û�������״̬  -- 130
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_onlineByUserId(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);



/// ===>  ��ȡͷ��ӿ�

//41.��ȡͷ��  -- 131
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_portrait(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//42.������ѯ�˺���ϸ��Ϣ  -- 132
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_batchGetAccountInfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//43.�޸��û�ͷ��  -- 133
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_confirm_portrait(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//44.��ȡ��֯�ܹ��汾��Ϣ  -- 134
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_getDepartmentVersion(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//45.��ͼƬ�͸�����΢������  -- 135
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_upload(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//46.ɾ���Լ���΢��  -- 136
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_deleteThread(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//47.ɾ���Լ��Ļظ�  -- 137
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_deleteReply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//48.��ȡ@���ޡ���˿�ȳ�ʼ��Ϣ  -- 138
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_notify_getAll(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//49.�����û�����(ƽ̨API)  -- 139
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//49.�����û�����(΢��API)  -- 139
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//50.����û���Ϣ(ͨ��email)  -- 140
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_info_by_email(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//51.��õ�ǰ��¼�û���Ϣ  -- 141
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_current_user_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//52.��ȡ��ƪ���۵�url  -- 142
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_article_url(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//53.�޸ĸ��˽���  -- 143
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updateBrief(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//54.������ģ�����ұ���ҵ��ϵ��  -- 144
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_list_account_by_str(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//55.��������  -- 145
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_password_reset_by_mail(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//56.��ȡ�޹��ҵ�΢������  -- 146
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_get_likes_me_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//57.�����Ƿ�ģ��������  -- 147
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_set_is_like_query(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//58.���������ϵ����֤ģʽ  -- 148
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_set_auth_mode(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);

//59.������ѯ��ҵ��ϵ��  -- 149
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_condition_query(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn);


// �������
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

