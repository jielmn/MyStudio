// KedaWeiBoAPI.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include <stdio.h>
#include <string.h>

#include "KedaWeiBoAPI.h"
#include "inner.h"
#include "httpstack.h"
#include "wbParser/splitstr.h"

static BOOL _IsBlankChar( char ch )
{
    if ( ' ' == ch || '\t' == ch || '\r' == ch || '\n' == ch )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_SetCommonProxy( const TWeiboProxyParam * pProxyParam )
{
    if ( 0 == pProxyParam )
    {
        return -1;
    }

    CGlobalDataWeibo * pGlobal = CGlobalDataWeibo::GetInstance();
    pGlobal->SetCommonProxyParam( pProxyParam );

    return 0;
}




/// ===��  �ƴ﹫˾ ΢��v0.1�ӿ�

//1.Oauth��accountToken�Ļ��(ƽ̨API)  -- 82
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_accounttoken *pwb_keda_get_accounttoken = (struct t_wb_keda_get_accounttoken *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN), pwb_keda_get_accounttoken);

	loWeibo_request_start(pWBReq , true);

	return 0;
}





//1.Oauth��accountToken�Ļ��(΢��API)  -- 82
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_accounttoken *pwb_keda_get_accounttoken = (struct t_wb_keda_get_accounttoken *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN), pwb_keda_get_accounttoken);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//2.��¼ƽ̨API  -- 83
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_login *pwb_keda_login = (struct t_wb_keda_login *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_P_LOGIN), pwb_keda_login);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//2.��¼΢��API  -- 83
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_login *pwb_keda_login = (struct t_wb_keda_login *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_W_LOGIN), pwb_keda_login);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//3.��õ�ǰ��¼�û�����ע�û���΢��  -- 84
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_me_and_friends(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_me_and_friends *pwb_keda_get_me_and_friends = (struct t_wb_keda_get_me_and_friends *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS), pwb_keda_get_me_and_friends);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//4.����û�������΢�����ݣ���Ѷ��  -- 85
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_user_data *pwb_keda_get_user_data = (struct t_wb_keda_get_user_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_USER_DATA), pwb_keda_get_user_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



//5.����û�������΢�����ݣ�ȫ����  -- 86
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_all_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_user_all_data *pwb_keda_get_user_all_data = (struct t_wb_keda_get_user_all_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA), pwb_keda_get_user_all_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}




//6.���һ�������Լ��ظ�  -- 87
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_subject_and_reply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_subject_and_reply *pwb_keda_get_subject_and_reply = (struct t_wb_keda_get_subject_and_reply *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY), pwb_keda_get_subject_and_reply);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//7.����һ��΢��  -- 88
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_publish(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_message_publish *pwb_keda_message_publish = (struct t_wb_keda_message_publish *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH), pwb_keda_message_publish);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



//8.�ظ�һ��΢��  -- 89
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_reply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_message_reply *pwb_keda_message_reply = (struct t_wb_keda_message_reply *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_REPLY), pwb_keda_message_reply);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



//9.����û���Ϣ  -- 90
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_user_info *pwb_keda_get_user_info = (struct t_wb_keda_get_user_info *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_USER_INFO), pwb_keda_get_user_info);

	loWeibo_request_start(pWBReq , true);

	return 0;
}





/// ===��  �ƴ﹫˾ ΢��v1.0�ӿ� ��һ����


//1.��ù�˾�㳡��΢������  -- 91
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_company_public_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_company_public_data *pwb_keda_get_company_public_data = (struct t_wb_keda_get_company_public_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA), pwb_keda_get_company_public_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//2.���Ⱥ���΢������  -- 92
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_groups_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_groups_data *pwb_keda_get_groups_data = (struct t_wb_keda_get_groups_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA), pwb_keda_get_groups_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//3.���@��ǰ�û���΢������  -- 93
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_mentions_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_mentions_data *pwb_keda_get_mentions_data = (struct t_wb_keda_get_mentions_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA), pwb_keda_get_mentions_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//4.��ûظ��ҵ�΢������  -- 94
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_replies_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_replies_data *pwb_keda_get_replies_data = (struct t_wb_keda_get_replies_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA), pwb_keda_get_replies_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//5.������޹���΢������  -- 95
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_likes_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_likes_data *pwb_keda_get_likes_data = (struct t_wb_keda_get_likes_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_LIKES_DATA), pwb_keda_get_likes_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//6.������ղص�΢������  -- 96
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_favorites_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_favorites_data *pwb_keda_get_favorites_data = (struct t_wb_keda_get_favorites_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA), pwb_keda_get_favorites_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//7.���ĳ�������΢������  -- 97
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_topics_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_topics_data *pwb_keda_get_topics_data = (struct t_wb_keda_get_topics_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA), pwb_keda_get_topics_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//8.��ù�˾����  -- 98
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_company(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_friendship_company *pwb_keda_get_friendship_company = (struct t_wb_keda_get_friendship_company *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY), pwb_keda_get_friendship_company);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//9.����ҵĹ�ע�б�  -- 99
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_attention(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_friendship_attention *pwb_keda_get_friendship_attention = (struct t_wb_keda_get_friendship_attention *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION), pwb_keda_get_friendship_attention);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//10.����ҵķ�˿�б�  -- 100
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_followers(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_friendship_followers *pwb_keda_get_friendship_followers = (struct t_wb_keda_get_friendship_followers *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS), pwb_keda_get_friendship_followers);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//11.����ҵĺ����б�  -- 101
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_friends(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_friendship_friends *pwb_keda_get_friendship_friends = (struct t_wb_keda_get_friendship_friends *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS), pwb_keda_get_friendship_friends);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



/// ===��  �ƴ﹫˾ ΢��v1.0�ӿ� �ڶ�����

//12.��ӹ�ע  -- 102
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_friendship_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_friendship_create *pwb_keda_friendship_create = (struct t_wb_keda_friendship_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE), pwb_keda_friendship_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//13.ȡ����ע  -- 103
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_friendship_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_friendship_destroy *pwb_keda_friendship_destroy = (struct t_wb_keda_friendship_destroy *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY), pwb_keda_friendship_destroy);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//14.����ղ�  -- 104
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_favorites_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_favorites_create *pwb_keda_favorites_create = (struct t_wb_keda_favorites_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_FAVORITES_CREATE), pwb_keda_favorites_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//15.ȡ���ղ�  -- 105
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_favorites_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_favorites_destroy *pwb_keda_favorites_destroy = (struct t_wb_keda_favorites_destroy *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY), pwb_keda_favorites_destroy);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//16.��ӻ���  -- 106
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_topic_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_topic_create *pwb_keda_topic_create = (struct t_wb_keda_topic_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_TOPIC_CREATE), pwb_keda_topic_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//17.ȡ������  -- 107
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_topic_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_topic_destroy *pwb_keda_topic_destroy = (struct t_wb_keda_topic_destroy *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_TOPIC_DESTROY), pwb_keda_topic_destroy);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//18.���like  -- 108
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_like_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_like_create *pwb_keda_like_create = (struct t_wb_keda_like_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_LIKE_CREATE), pwb_keda_like_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//19.ȡ��like  -- 109
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_like_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_like_destroy *pwb_keda_like_destroy = (struct t_wb_keda_like_destroy *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_LIKE_DESTROY), pwb_keda_like_destroy);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//20.��ȡ�ҵ�Ⱥ���б�  -- 110
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_group_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_group_user *pwb_keda_get_group_user = (struct t_wb_keda_get_group_user *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_GROUP_USER), pwb_keda_get_group_user);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//21.������е�Ⱥ���б�  -- 111
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_group_all(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_group_all *pwb_keda_get_group_all = (struct t_wb_keda_get_group_all *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_GROUP_ALL), pwb_keda_get_group_all);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//22.���Ⱥ����  -- 112
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_group_create *pwb_keda_group_create = (struct t_wb_keda_group_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GROUP_CREATE), pwb_keda_group_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//23.����Ⱥ����  -- 113
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_join(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_group_join *pwb_keda_group_join = (struct t_wb_keda_group_join *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GROUP_JOIN), pwb_keda_group_join);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//24.�˳�Ⱥ����  -- 114
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_quit(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_group_quit *pwb_keda_group_quit = (struct t_wb_keda_group_quit *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GROUP_QUIT), pwb_keda_group_quit);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//25.�鿴�ҵ�˽���б�  -- 115
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_privatemessage_list(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_privatemessage_list *pwb_keda_get_privatemessage_list = (struct t_wb_keda_get_privatemessage_list *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST), pwb_keda_get_privatemessage_list);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//26.�鿴����ĳ�˵�˽�ŵ������¼  -- 116
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_privatemessage_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_privatemessage_user *pwb_keda_get_privatemessage_user = (struct t_wb_keda_get_privatemessage_user *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER), pwb_keda_get_privatemessage_user);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//27.����˽�ŵ�ĳ��  -- 117
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_privatemessage_send(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_privatemessage_send *pwb_keda_privatemessage_send = (struct t_wb_keda_privatemessage_send *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND), pwb_keda_privatemessage_send);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//28.ɾ��˽��  -- 118
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_privatemessage_delete(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_privatemessage_delete *pwb_keda_privatemessage_delete = (struct t_wb_keda_privatemessage_delete *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE), pwb_keda_privatemessage_delete);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



/// ===��  �ƴ﹫˾ ΢��v1.0�ӿ� ��������

//29.��֤�ն��˺�  -- 119
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_validateclientaccount(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_validateClientAccount *pwb_keda_ams_validateClientAccount = (struct t_wb_keda_ams_validateClientAccount *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT), pwb_keda_ams_validateClientAccount);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//30.��ѯ�˺���ϸ��Ϣ  -- 120
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_getaccountinfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_getAccountInfo *pwb_keda_ams_getAccountInfo = (struct t_wb_keda_ams_getAccountInfo *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO), pwb_keda_ams_getAccountInfo);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//31.�޸��˺���ϸ��Ϣ  -- 121
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updateaccountinfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_updateAccountInfo *pwb_keda_ams_updateAccountInfo = (struct t_wb_keda_ams_updateAccountInfo *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO), pwb_keda_ams_updateAccountInfo);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//32.�޸��˺�����  -- 122
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updatepassword(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_updatePassword *pwb_keda_ams_updatePassword = (struct t_wb_keda_ams_updatePassword *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD), pwb_keda_ams_updatePassword);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


/// ===��  �ƴ﹫˾ ΢��v1.0�ӿ� ���Ĳ���

//33.��ù�˾��֯�ܹ���������Ա��Ϣ  -- 123
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_all(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_department_all *pwb_keda_get_department_all = (struct t_wb_keda_get_department_all *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL), pwb_keda_get_department_all);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//34.��ȡ��˾��֯�ܹ���Ϣ  -- 124
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_company(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_department_company *pwb_keda_get_department_company = (struct t_wb_keda_get_department_company *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY), pwb_keda_get_department_company);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//35.��ȡ��������Ա��Ϣ(�������Ӳ���)  -- 125
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_department_user *pwb_keda_get_department_user = (struct t_wb_keda_get_department_user *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER), pwb_keda_get_department_user);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//36.��ȡ��������Ա��Ϣ(�����Ӳ�����Ա)  -- 126
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_alluser(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_department_allUser *pwb_keda_get_department_allUser = (struct t_wb_keda_get_department_allUser *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER), pwb_keda_get_department_allUser);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//37.����֯�ܹ�������������Ϣ  -- 127
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_searchdepartment(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_department_searchDepartment *pwb_keda_department_searchDepartment = (struct t_wb_keda_department_searchDepartment *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT), pwb_keda_department_searchDepartment);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//38.����֯�ܹ���������Ա��Ϣ  -- 128
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_searchuser(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_department_searchUser *pwb_keda_department_searchUser= (struct t_wb_keda_department_searchUser *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER), pwb_keda_department_searchUser);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



//39.��ȡ�����µ�Ա��������״̬  -- 129
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_onlineByDepId(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_department_online_by_depId *pwb_keda_department_online_by_depId = (struct t_wb_keda_department_online_by_depId *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID), pwb_keda_department_online_by_depId);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//40.��ȡ��֯���û�������״̬  -- 130
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_onlineByUserId(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_department_online_by_userId *pwb_keda_department_online_by_userId = (struct t_wb_keda_department_online_by_userId *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID), pwb_keda_department_online_by_userId);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//41.��ȡͷ��  -- 131
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_portrait(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_portrait *pwb_keda_get_portrait = (struct t_wb_keda_get_portrait *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_PORTRAIT), pwb_keda_get_portrait);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//42.������ѯ�˺���ϸ��Ϣ  -- 132
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_batchGetAccountInfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_batchGetAccountInfo *pwb_keda_ams_batchGetAccountInfo = (struct t_wb_keda_ams_batchGetAccountInfo *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO), pwb_keda_ams_batchGetAccountInfo);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//43.�޸��û�ͷ��  -- 133
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_confirm_portrait(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_confirmPortrait *pwb_keda_confirmPortrait = (struct t_wb_keda_confirmPortrait *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT), pwb_keda_confirmPortrait);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//44.��ȡ��֯�ܹ��汾��Ϣ  -- 134
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_getDepartmentVersion(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_department_get_version *pwb_keda_department_get_version = (struct t_wb_keda_department_get_version *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION), pwb_keda_department_get_version);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//45.��ͼƬ�͸�����΢������  -- 135
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_upload(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_message_upload *pwb_keda_message_upload = (struct t_wb_keda_message_upload *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD), pwb_keda_message_upload);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//46.ɾ���Լ���΢��  -- 136
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_deleteThread(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_message_delete_thread *pwb_keda_message_delete_thread = (struct t_wb_keda_message_delete_thread *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD), pwb_keda_message_delete_thread);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//47.ɾ���Լ��Ļظ�  -- 137
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_deleteReply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_message_delete_reply *pwb_keda_message_delete_reply = (struct t_wb_keda_message_delete_reply *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY), pwb_keda_message_delete_reply);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//48.��ȡ@���ޡ���˿�ȳ�ʼ��Ϣ  -- 138
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_notify_getAll(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_notify_get_all *pwb_keda_notify_get_all = (struct t_wb_keda_notify_get_all *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL), pwb_keda_notify_get_all);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//49.�����û�����(ƽ̨API)  -- 139
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_keep_heartbeat *pwb_keda_keep_heartbeat = (struct t_wb_keda_keep_heartbeat *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT), pwb_keda_keep_heartbeat);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//49.�����û�����(΢��API)  -- 139
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_keep_heartbeat *pwb_keda_keep_heartbeat = (struct t_wb_keda_keep_heartbeat *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT), pwb_keda_keep_heartbeat);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//50.����û���Ϣ(ͨ��email)  -- 140
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_info_by_email(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_user_info_by_email *pwb_keda_get_user_info_by_email = (struct t_wb_keda_get_user_info_by_email *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL), pwb_keda_get_user_info_by_email);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//51.��õ�ǰ��¼�û���Ϣ  -- 141
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_current_user_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_current_user_info *pwb_keda_get_current_user_info = (struct t_wb_keda_get_current_user_info *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO), pwb_keda_get_current_user_info);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//52.��ȡ��ƪ���۵�url  -- 142
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_article_url(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_get_article_url *pwb_keda_get_article_url = (struct t_wb_keda_get_article_url *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL), pwb_keda_get_article_url);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//53.�޸ĸ��˽���  -- 143
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updateBrief(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_updateBrief *pwb_keda_ams_updateBrief = (struct t_wb_keda_ams_updateBrief *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF), pwb_keda_ams_updateBrief);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//54.������ģ�����ұ���ҵ��ϵ��  -- 144
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_list_account_by_str(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_list_account_by_str *pwb_keda_ams_list_account_by_str = (struct t_wb_keda_ams_list_account_by_str *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR), pwb_keda_ams_list_account_by_str);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//55.��������  -- 145
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_password_reset_by_mail(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_password_reset_by_mail *pwb_keda_ams_password_reset_by_mail = (struct t_wb_keda_ams_password_reset_by_mail *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL), pwb_keda_ams_password_reset_by_mail);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//56.��ȡ�޹��ҵ�΢������  -- 146
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_get_likes_me_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_get_likes_me_data *pwb_keda_ams_get_likes_me_data = (struct t_wb_keda_ams_get_likes_me_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_GET_LIKES_ME_DATA), pwb_keda_ams_get_likes_me_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//57.�����Ƿ�ģ��������  -- 147
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_set_is_like_query(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_set_is_like_query *pwb_keda_ams_set_is_like_query = (struct t_wb_keda_ams_set_is_like_query *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_SET_IS_LIKE_QUERY), pwb_keda_ams_set_is_like_query);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//58.���������ϵ����֤ģʽ  -- 148
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_set_auth_mode(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_set_auth_mode *pwb_keda_ams_set_auth_mode = (struct t_wb_keda_ams_set_auth_mode *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_SET_AUTH_MODE), pwb_keda_ams_set_auth_mode);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//59.������ѯ��ҵ��ϵ��  -- 149
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_condition_query(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq����pt_paraInΪNULL��error��\n");
		return -1;
	}

	struct t_wb_keda_ams_condition_query *pwb_keda_ams_condition_query = (struct t_wb_keda_ams_condition_query *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_CONDITION_QUERY), pwb_keda_ams_condition_query);

	loWeibo_request_start(pWBReq , true);

	return 0;
}






// �������

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
    if (pWBReq == NULL || pt_paraIn == NULL)
    {
        printf("pWBReq����pt_paraInΪNULL��error��\n");
        return -1;
    }

    struct t_wb_keda_get_accounttoken *pwb_keda_get_accounttoken = (struct t_wb_keda_get_accounttoken *)pt_paraIn;

    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) ,          pt_customStruct );
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN), pwb_keda_get_accounttoken);

    loWeibo_request_start(pWBReq , true);

    return 0;
}


//  ��¼����ƽ̨
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
    if (pWBReq == NULL || pt_paraIn == NULL)
    {
        printf("pWBReq����pt_paraInΪNULL��error��\n");
        return -1;
    }

    struct t_wb_keda_login *pwb_keda_login = (struct t_wb_keda_login *)pt_paraIn;

    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_MEETING_LOGIN), pwb_keda_login);

    loWeibo_request_start(pWBReq , true);

    return 0;
}


KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
    if (pWBReq == NULL || pt_paraIn == NULL)
    {
        printf("pWBReq����pt_paraInΪNULL��error��\n");
        return -1;
    }

    struct t_wb_keda_keep_heartbeat *pwb_keda_keep_heartbeat = (struct t_wb_keda_keep_heartbeat *)pt_paraIn;

    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_MEETING_KEEP_HEARTBEAT), pwb_keda_keep_heartbeat);

    loWeibo_request_start(pWBReq , true);

    return 0;
}



KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_meeting_list(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
    if (pWBReq == NULL || pt_paraIn == NULL)
    {
        printf("pWBReq����pt_paraInΪNULL��error��\n");
        return -1;
    }

    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_MEETING_MEETING_LIST), pt_paraIn );

    loWeibo_request_start(pWBReq , true);

    return 0;
}

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_meeting_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
    if (pWBReq == NULL || pt_paraIn == NULL)
    {
        printf("pWBReq����pt_paraInΪNULL��error��\n");
        return -1;
    }

    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_MEETING_MEETING_INFO), pt_paraIn );

    loWeibo_request_start(pWBReq , true);

    return 0;
}



KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_booking_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
    if (pWBReq == NULL || pt_paraIn == NULL)
    {
        printf("pWBReq����pt_paraInΪNULL��error��\n");
        return -1;
    }

    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_MEETING_BOOKING_INFO), pt_paraIn );

    loWeibo_request_start(pWBReq , true);

    return 0;
}


KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m(WEIBORequest *pWBReq, WEIBOoption e, void *pt_paraIn)
{
    if (pWBReq == NULL || pt_paraIn == NULL)
    {
        printf("pWBReq����pt_paraInΪNULL��error��\n");
        return -1;
    }

    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) ,  pWBReq );
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  e, pt_paraIn );

    loWeibo_request_start(pWBReq , true);

    return 0;
}

KEDAWEIBOAPI_API void  SetKedaWeiBoLogCallback( POnKedaWeiBoLogCallback pfnLog )
{
    SetLogCallback( (POnMicroblogCallback )pfnLog );
}

KEDAWEIBOAPI_API void  fnKedaWeiboDebugInfo( )
{
    CHttp::GetInstance()->PrintDebugInfo();
}

KEDAWEIBOAPI_API const char *  fnGetKedaWeiboCompileTime( )
{
    return CGlobalDataWeibo::GetInstance()->GetCompileTime();
}

KEDAWEIBOAPI_API const char *  fnGetKedaWeiboVersion( )
{
    return _API_VERSION_;
}

KEDAWEIBOAPI_API void fnKedaWeiBoAPI_SetCallBack( fun_wb_header_callback pfnCallback )
{
    g_pfnHeaderCallback = pfnCallback;
}

static void UpdateCookie( TRestPathCookies * cookies, unsigned long dwCnt, 
                          const char * cookieKey, const char * cookieValue, const char * cookiePath )
{
    TRestPathCookies * pCookie    = 0;
    TRestCookie      * pSubCookie = 0;
    unsigned long i;

    for ( i = 0; i < dwCnt; i++ )
    {
        TRestPathCookies * cookie = cookies + i;

        // ��Ϊû��ʹ��
        if ( '\0' == cookie->szPath[0] )
        {
            if ( 0 == pCookie )
            {
                pCookie = cookie;
            }
        }
        else if ( 0 == STRICMP(cookie->szPath, cookiePath) )
        {
            pCookie = cookie;
            break;
        }
    }

    // û�п��õ�cookie����
    if ( 0 == pCookie )
    {
        return;
    }

    if ( '\0' == pCookie->szPath[0] )
    {
        strncpy( pCookie->szPath, cookiePath, 64 );
    }

    for ( i = 0; i < 4; i++ )
    {
        TRestCookie * pItem = pCookie->cookies + i;

        // ��Ϊû��ʹ��
        if ( '\0' == pItem->szCookieKey[0] )
        {
            if ( 0 == pSubCookie )
            {
                pSubCookie = pItem;
            }
        }
        else if ( 0 == STRICMP(pItem->szCookieKey, cookieKey) )
        {
            pSubCookie = pItem;
            break;
        }
    }

    // û�л���
    if ( 0 == pSubCookie )
    {
        return;
    }

    if ( '\0' == pSubCookie->szCookieKey[0] )
    {
        strncpy( pSubCookie->szCookieKey, cookieKey, 64 );
    }

    strncpy( pSubCookie->szCookieValue, cookieValue, 256 );
}

KEDAWEIBOAPI_API void fnKedaRest_GetCookies( TRestPathCookies * cookies, unsigned long dwCnt, const char * szHeader )
{
    // ȡ��ÿ��cookie
    const char* key   = 0;
    const char* qe    = 0;
    const char* token = 0;

    const char* s     = szHeader;
    const char* linestart, *lineend ;

    size_t set_cookie_len = strlen("Set-Cookie:");

    char   cookieKey[64];
    char   cookieValue[256];
    char   cookiePath[64];
    int    index = 0;

    size_t key_len   = 0;
    size_t value_len = 0;

    while( wb_readline( s , linestart , lineend ) )
    {
        //Set-Cookie:
        if( STRNICMP( "Set-Cookie:" , linestart , set_cookie_len ) == 0 )
        {
            linestart += set_cookie_len;
            
            // linestart ���ӣ� SSO_COOKIE_KEY=MTM2Mzc1ODI1ODM5Nzo1YTYxNWRjZmYxZTlmN2YwMmZhZDNjNWNhMDFkMjQwZQ; Domain=172.16.217.185; Expires=Fri, 20-Mar-2015 05:44:18 GMT; Path=/
            // ��Ҫ�ָ��';'
            key   = 0;
            qe    = 0;
            index = 0;

            memset( cookieKey,   0,    sizeof(cookieKey) );
            memset( cookieValue, 0,    sizeof(cookieValue) );
            memset( cookiePath,  0,   sizeof(cookiePath) );

            token = linestart;
            while ( 1 )
            {
                // ����Ƿָ����-����
                if ( ';' == *token || token == lineend )
                {
                    if ( 0 != key && 0 != qe )
                    {
                        key_len   = qe - key;
                        value_len = token - (qe+1);

                        // SSO_COOKIE_KEY= ��
                        if ( 0 == index )
                        {
                            strncpy( cookieKey,   key,  GET_MIN( key_len, 64 ) );
                            strncpy( cookieValue, qe+1, GET_MIN( value_len, 256 ) );
                        }
                        // Domain=, Path= ��
                        else
                        {
                            if ( 0 == STRNICMP( key, "Path=", 5 ) )
                            {
                                strncpy( cookiePath, qe+1, GET_MIN( value_len, 64 ) );
                            }
                        }
                    }

                    key = 0;
                    qe  = 0;

                    index++;

                    if ( token == lineend )
                    {
                        break;
                    }
                }
                else
                {
                    if ( 0 == key )
                    {
                        if ( !_IsBlankChar(*token) && ('=' != *token) )
                        {
                            key = token;
                        }
                    }
                    else if ( (0 == qe) && ('=' == *token) )
                    {
                        qe = token;
                    }
                }
                
                token++;
            }

            // ����cookie
            if ( '\0' != cookieKey[0] )
            {
                // û������path�ģ�Ĭ��Ϊroot·��
                if ( '\0' == cookiePath )
                {
                    strncpy( cookiePath, "/", 64 );
                }

                UpdateCookie( cookies, dwCnt, cookieKey, cookieValue, cookiePath );
            }
            // END
        }

        s = lineend;
        if( *s == '\0' )
            break;

        // ���� 0x0d , 0x0a
        s = ++lineend ;
        if( *s == '\0' )
            break;

        s = ++lineend ;
        if( *s == '\0' )
            break;
    }
}