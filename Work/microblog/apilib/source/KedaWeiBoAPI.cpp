// KedaWeiBoAPI.cpp : 定义 DLL 应用程序的导出函数。
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




/// ===＞  科达公司 微博v0.1接口

//1.Oauth的accountToken的获得(平台API)  -- 82
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_accounttoken *pwb_keda_get_accounttoken = (struct t_wb_keda_get_accounttoken *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN), pwb_keda_get_accounttoken);

	loWeibo_request_start(pWBReq , true);

	return 0;
}





//1.Oauth的accountToken的获得(微博API)  -- 82
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_accounttoken *pwb_keda_get_accounttoken = (struct t_wb_keda_get_accounttoken *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN), pwb_keda_get_accounttoken);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//2.登录平台API  -- 83
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_login *pwb_keda_login = (struct t_wb_keda_login *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_P_LOGIN), pwb_keda_login);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//2.登录微博API  -- 83
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_login *pwb_keda_login = (struct t_wb_keda_login *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_W_LOGIN), pwb_keda_login);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//3.获得当前登录用户及关注用户的微博  -- 84
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_me_and_friends(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_me_and_friends *pwb_keda_get_me_and_friends = (struct t_wb_keda_get_me_and_friends *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS), pwb_keda_get_me_and_friends);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//4.获得用户发布的微博数据（友讯）  -- 85
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_user_data *pwb_keda_get_user_data = (struct t_wb_keda_get_user_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_USER_DATA), pwb_keda_get_user_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



//5.获得用户发布的微博数据（全部）  -- 86
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_all_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_user_all_data *pwb_keda_get_user_all_data = (struct t_wb_keda_get_user_all_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA), pwb_keda_get_user_all_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}




//6.获得一条主题以及回复  -- 87
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_subject_and_reply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_subject_and_reply *pwb_keda_get_subject_and_reply = (struct t_wb_keda_get_subject_and_reply *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY), pwb_keda_get_subject_and_reply);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//7.发布一条微博  -- 88
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_publish(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_message_publish *pwb_keda_message_publish = (struct t_wb_keda_message_publish *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH), pwb_keda_message_publish);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



//8.回复一条微博  -- 89
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_reply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_message_reply *pwb_keda_message_reply = (struct t_wb_keda_message_reply *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_REPLY), pwb_keda_message_reply);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



//9.获得用户信息  -- 90
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_user_info *pwb_keda_get_user_info = (struct t_wb_keda_get_user_info *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_USER_INFO), pwb_keda_get_user_info);

	loWeibo_request_start(pWBReq , true);

	return 0;
}





/// ===＞  科达公司 微博v1.0接口 第一部分


//1.获得公司广场的微博数据  -- 91
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_company_public_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_company_public_data *pwb_keda_get_company_public_data = (struct t_wb_keda_get_company_public_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA), pwb_keda_get_company_public_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//2.获得群组的微博数据  -- 92
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_groups_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_groups_data *pwb_keda_get_groups_data = (struct t_wb_keda_get_groups_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA), pwb_keda_get_groups_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//3.获得@当前用户的微博数据  -- 93
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_mentions_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_mentions_data *pwb_keda_get_mentions_data = (struct t_wb_keda_get_mentions_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA), pwb_keda_get_mentions_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//4.获得回复我的微博数据  -- 94
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_replies_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_replies_data *pwb_keda_get_replies_data = (struct t_wb_keda_get_replies_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA), pwb_keda_get_replies_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//5.获得我赞过的微博数据  -- 95
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_likes_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_likes_data *pwb_keda_get_likes_data = (struct t_wb_keda_get_likes_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_LIKES_DATA), pwb_keda_get_likes_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//6.获得我收藏的微博数据  -- 96
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_favorites_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_favorites_data *pwb_keda_get_favorites_data = (struct t_wb_keda_get_favorites_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA), pwb_keda_get_favorites_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//7.获得某个话题的微博数据  -- 97
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_topics_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_topics_data *pwb_keda_get_topics_data = (struct t_wb_keda_get_topics_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA), pwb_keda_get_topics_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//8.获得公司人脉  -- 98
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_company(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_friendship_company *pwb_keda_get_friendship_company = (struct t_wb_keda_get_friendship_company *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY), pwb_keda_get_friendship_company);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//9.获得我的关注列表  -- 99
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_attention(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_friendship_attention *pwb_keda_get_friendship_attention = (struct t_wb_keda_get_friendship_attention *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION), pwb_keda_get_friendship_attention);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//10.获得我的粉丝列表  -- 100
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_followers(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_friendship_followers *pwb_keda_get_friendship_followers = (struct t_wb_keda_get_friendship_followers *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS), pwb_keda_get_friendship_followers);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//11.获得我的好友列表  -- 101
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_friendship_friends(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_friendship_friends *pwb_keda_get_friendship_friends = (struct t_wb_keda_get_friendship_friends *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS), pwb_keda_get_friendship_friends);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



/// ===＞  科达公司 微博v1.0接口 第二部分

//12.添加关注  -- 102
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_friendship_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_friendship_create *pwb_keda_friendship_create = (struct t_wb_keda_friendship_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE), pwb_keda_friendship_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//13.取消关注  -- 103
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_friendship_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_friendship_destroy *pwb_keda_friendship_destroy = (struct t_wb_keda_friendship_destroy *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY), pwb_keda_friendship_destroy);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//14.添加收藏  -- 104
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_favorites_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_favorites_create *pwb_keda_favorites_create = (struct t_wb_keda_favorites_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_FAVORITES_CREATE), pwb_keda_favorites_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//15.取消收藏  -- 105
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_favorites_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_favorites_destroy *pwb_keda_favorites_destroy = (struct t_wb_keda_favorites_destroy *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY), pwb_keda_favorites_destroy);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//16.添加话题  -- 106
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_topic_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_topic_create *pwb_keda_topic_create = (struct t_wb_keda_topic_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_TOPIC_CREATE), pwb_keda_topic_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//17.取消话题  -- 107
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_topic_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_topic_destroy *pwb_keda_topic_destroy = (struct t_wb_keda_topic_destroy *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_TOPIC_DESTROY), pwb_keda_topic_destroy);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//18.添加like  -- 108
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_like_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_like_create *pwb_keda_like_create = (struct t_wb_keda_like_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_LIKE_CREATE), pwb_keda_like_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//19.取消like  -- 109
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_like_destroy(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_like_destroy *pwb_keda_like_destroy = (struct t_wb_keda_like_destroy *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_LIKE_DESTROY), pwb_keda_like_destroy);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//20.获取我的群组列表  -- 110
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_group_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_group_user *pwb_keda_get_group_user = (struct t_wb_keda_get_group_user *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_GROUP_USER), pwb_keda_get_group_user);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//21.获得所有的群组列表  -- 111
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_group_all(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_group_all *pwb_keda_get_group_all = (struct t_wb_keda_get_group_all *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_GROUP_ALL), pwb_keda_get_group_all);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//22.添加群操作  -- 112
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_create(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_group_create *pwb_keda_group_create = (struct t_wb_keda_group_create *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GROUP_CREATE), pwb_keda_group_create);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//23.加入群操作  -- 113
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_join(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_group_join *pwb_keda_group_join = (struct t_wb_keda_group_join *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GROUP_JOIN), pwb_keda_group_join);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//24.退出群操作  -- 114
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_group_quit(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_group_quit *pwb_keda_group_quit = (struct t_wb_keda_group_quit *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GROUP_QUIT), pwb_keda_group_quit);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//25.查看我的私信列表  -- 115
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_privatemessage_list(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_privatemessage_list *pwb_keda_get_privatemessage_list = (struct t_wb_keda_get_privatemessage_list *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST), pwb_keda_get_privatemessage_list);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//26.查看我与某人的私信的聊天记录  -- 116
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_privatemessage_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_privatemessage_user *pwb_keda_get_privatemessage_user = (struct t_wb_keda_get_privatemessage_user *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER), pwb_keda_get_privatemessage_user);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//27.发送私信到某人  -- 117
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_privatemessage_send(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_privatemessage_send *pwb_keda_privatemessage_send = (struct t_wb_keda_privatemessage_send *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND), pwb_keda_privatemessage_send);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//28.删除私信  -- 118
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_privatemessage_delete(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_privatemessage_delete *pwb_keda_privatemessage_delete = (struct t_wb_keda_privatemessage_delete *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE), pwb_keda_privatemessage_delete);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



/// ===＞  科达公司 微博v1.0接口 第三部分

//29.验证终端账号  -- 119
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_validateclientaccount(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_validateClientAccount *pwb_keda_ams_validateClientAccount = (struct t_wb_keda_ams_validateClientAccount *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT), pwb_keda_ams_validateClientAccount);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//30.查询账号详细信息  -- 120
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_getaccountinfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_getAccountInfo *pwb_keda_ams_getAccountInfo = (struct t_wb_keda_ams_getAccountInfo *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO), pwb_keda_ams_getAccountInfo);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//31.修改账号详细信息  -- 121
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updateaccountinfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_updateAccountInfo *pwb_keda_ams_updateAccountInfo = (struct t_wb_keda_ams_updateAccountInfo *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO), pwb_keda_ams_updateAccountInfo);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//32.修改账号密码  -- 122
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updatepassword(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_updatePassword *pwb_keda_ams_updatePassword = (struct t_wb_keda_ams_updatePassword *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD), pwb_keda_ams_updatePassword);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


/// ===＞  科达公司 微博v1.0接口 第四部分

//33.获得公司组织架构和所有人员信息  -- 123
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_all(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_department_all *pwb_keda_get_department_all = (struct t_wb_keda_get_department_all *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL), pwb_keda_get_department_all);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//34.获取公司组织架构信息  -- 124
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_company(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_department_company *pwb_keda_get_department_company = (struct t_wb_keda_get_department_company *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY), pwb_keda_get_department_company);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//35.获取部门下人员信息(不包含子部门)  -- 125
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_user(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_department_user *pwb_keda_get_department_user = (struct t_wb_keda_get_department_user *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER), pwb_keda_get_department_user);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//36.获取部门下人员信息(包含子部门人员)  -- 126
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_get_alluser(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_department_allUser *pwb_keda_get_department_allUser = (struct t_wb_keda_get_department_allUser *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER), pwb_keda_get_department_allUser);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//37.在组织架构中搜索部门信息  -- 127
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_searchdepartment(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_department_searchDepartment *pwb_keda_department_searchDepartment = (struct t_wb_keda_department_searchDepartment *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT), pwb_keda_department_searchDepartment);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//38.在组织架构中搜索人员信息  -- 128
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_searchuser(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_department_searchUser *pwb_keda_department_searchUser= (struct t_wb_keda_department_searchUser *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER), pwb_keda_department_searchUser);

	loWeibo_request_start(pWBReq , true);

	return 0;
}



//39.获取部门下的员工的在线状态  -- 129
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_onlineByDepId(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_department_online_by_depId *pwb_keda_department_online_by_depId = (struct t_wb_keda_department_online_by_depId *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID), pwb_keda_department_online_by_depId);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//40.获取组织里用户的在线状态  -- 130
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_onlineByUserId(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_department_online_by_userId *pwb_keda_department_online_by_userId = (struct t_wb_keda_department_online_by_userId *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID), pwb_keda_department_online_by_userId);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//41.获取头像  -- 131
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_portrait(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_portrait *pwb_keda_get_portrait = (struct t_wb_keda_get_portrait *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_PORTRAIT), pwb_keda_get_portrait);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//42.批量查询账号详细信息  -- 132
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_batchGetAccountInfo(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_batchGetAccountInfo *pwb_keda_ams_batchGetAccountInfo = (struct t_wb_keda_ams_batchGetAccountInfo *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO), pwb_keda_ams_batchGetAccountInfo);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//43.修改用户头像  -- 133
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_confirm_portrait(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_confirmPortrait *pwb_keda_confirmPortrait = (struct t_wb_keda_confirmPortrait *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT), pwb_keda_confirmPortrait);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//44.获取组织架构版本信息  -- 134
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_department_getDepartmentVersion(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_department_get_version *pwb_keda_department_get_version = (struct t_wb_keda_department_get_version *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION), pwb_keda_department_get_version);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//45.带图片和附件的微博发布  -- 135
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_upload(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_message_upload *pwb_keda_message_upload = (struct t_wb_keda_message_upload *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD), pwb_keda_message_upload);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//46.删除自己的微博  -- 136
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_deleteThread(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_message_delete_thread *pwb_keda_message_delete_thread = (struct t_wb_keda_message_delete_thread *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD), pwb_keda_message_delete_thread);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//47.删除自己的回复  -- 137
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_message_deleteReply(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_message_delete_reply *pwb_keda_message_delete_reply = (struct t_wb_keda_message_delete_reply *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY), pwb_keda_message_delete_reply);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//48.获取@、赞、粉丝等初始消息  -- 138
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_notify_getAll(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_notify_get_all *pwb_keda_notify_get_all = (struct t_wb_keda_notify_get_all *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL), pwb_keda_notify_get_all);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//49.保持用户心跳(平台API)  -- 139
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_p_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_keep_heartbeat *pwb_keda_keep_heartbeat = (struct t_wb_keda_keep_heartbeat *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT), pwb_keda_keep_heartbeat);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//49.保持用户心跳(微博API)  -- 139
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_w_keep_heartBeat(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_keep_heartbeat *pwb_keda_keep_heartbeat = (struct t_wb_keda_keep_heartbeat *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT), pwb_keda_keep_heartbeat);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//50.获得用户信息(通过email)  -- 140
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_user_info_by_email(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_user_info_by_email *pwb_keda_get_user_info_by_email = (struct t_wb_keda_get_user_info_by_email *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL), pwb_keda_get_user_info_by_email);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//51.获得当前登录用户信息  -- 141
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_current_user_info(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_current_user_info *pwb_keda_get_current_user_info = (struct t_wb_keda_get_current_user_info *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO), pwb_keda_get_current_user_info);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//52.获取长篇大论的url  -- 142
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_get_article_url(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if (pWBReq == NULL || pt_paraIn == NULL)
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_get_article_url *pwb_keda_get_article_url = (struct t_wb_keda_get_article_url *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL), pwb_keda_get_article_url);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//53.修改个人介绍  -- 143
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_updateBrief(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_updateBrief *pwb_keda_ams_updateBrief = (struct t_wb_keda_ams_updateBrief *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF), pwb_keda_ams_updateBrief);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//54.按姓名模糊查找本企业联系人  -- 144
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_list_account_by_str(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_list_account_by_str *pwb_keda_ams_list_account_by_str = (struct t_wb_keda_ams_list_account_by_str *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR), pwb_keda_ams_list_account_by_str);

	loWeibo_request_start(pWBReq , true);

	return 0;
}


//55.忘记密码  -- 145
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_password_reset_by_mail(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_password_reset_by_mail *pwb_keda_ams_password_reset_by_mail = (struct t_wb_keda_ams_password_reset_by_mail *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL), pwb_keda_ams_password_reset_by_mail);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//56.获取赞过我的微博数据  -- 146
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_get_likes_me_data(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_get_likes_me_data *pwb_keda_ams_get_likes_me_data = (struct t_wb_keda_ams_get_likes_me_data *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_GET_LIKES_ME_DATA), pwb_keda_ams_get_likes_me_data);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//57.设置是否被模糊搜索到  -- 147
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_set_is_like_query(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_set_is_like_query *pwb_keda_ams_set_is_like_query = (struct t_wb_keda_ams_set_is_like_query *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_SET_IS_LIKE_QUERY), pwb_keda_ams_set_is_like_query);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//58.设置添加联系人认证模式  -- 148
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_set_auth_mode(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_set_auth_mode *pwb_keda_ams_set_auth_mode = (struct t_wb_keda_ams_set_auth_mode *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_SET_AUTH_MODE), pwb_keda_ams_set_auth_mode);

	loWeibo_request_start(pWBReq , true);

	return 0;
}

//59.条件查询企业联系人  -- 149
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_ams_condition_query(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
	if ( pWBReq == NULL || pt_paraIn == NULL )
	{
		printf("pWBReq或者pt_paraIn为NULL，error！\n");
		return -1;
	}

	struct t_wb_keda_ams_condition_query *pwb_keda_ams_condition_query = (struct t_wb_keda_ams_condition_query *)pt_paraIn;

	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) , pt_customStruct );
	loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_API_AMS_CONDITION_QUERY), pwb_keda_ams_condition_query);

	loWeibo_request_start(pWBReq , true);

	return 0;
}






// 会议管理

KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_get_accounttoken(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
    if (pWBReq == NULL || pt_paraIn == NULL)
    {
        printf("pWBReq或者pt_paraIn为NULL，error！\n");
        return -1;
    }

    struct t_wb_keda_get_accounttoken *pwb_keda_get_accounttoken = (struct t_wb_keda_get_accounttoken *)pt_paraIn;

    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_XML);
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_USERDATA) ,          pt_customStruct );
    loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN), pwb_keda_get_accounttoken);

    loWeibo_request_start(pWBReq , true);

    return 0;
}


//  登录会议平台
KEDAWEIBOAPI_API int fnKedaWeiBoAPI_m_api_login(WEIBORequest *pWBReq, void *pt_customStruct, void *pt_paraIn)
{
    if (pWBReq == NULL || pt_paraIn == NULL)
    {
        printf("pWBReq或者pt_paraIn为NULL，error！\n");
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
        printf("pWBReq或者pt_paraIn为NULL，error！\n");
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
        printf("pWBReq或者pt_paraIn为NULL，error！\n");
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
        printf("pWBReq或者pt_paraIn为NULL，error！\n");
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
        printf("pWBReq或者pt_paraIn为NULL，error！\n");
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
        printf("pWBReq或者pt_paraIn为NULL，error！\n");
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

        // 认为没有使用
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

    // 没有可用的cookie缓存
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

        // 认为没有使用
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

    // 没有缓存
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
    // 取得每个cookie
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
            
            // linestart 例子： SSO_COOKIE_KEY=MTM2Mzc1ODI1ODM5Nzo1YTYxNWRjZmYxZTlmN2YwMmZhZDNjNWNhMDFkMjQwZQ; Domain=172.16.217.185; Expires=Fri, 20-Mar-2015 05:44:18 GMT; Path=/
            // 需要分割逗号';'
            key   = 0;
            qe    = 0;
            index = 0;

            memset( cookieKey,   0,    sizeof(cookieKey) );
            memset( cookieValue, 0,    sizeof(cookieValue) );
            memset( cookiePath,  0,   sizeof(cookiePath) );

            token = linestart;
            while ( 1 )
            {
                // 如果是分割符号-逗号
                if ( ';' == *token || token == lineend )
                {
                    if ( 0 != key && 0 != qe )
                    {
                        key_len   = qe - key;
                        value_len = token - (qe+1);

                        // SSO_COOKIE_KEY= 等
                        if ( 0 == index )
                        {
                            strncpy( cookieKey,   key,  GET_MIN( key_len, 64 ) );
                            strncpy( cookieValue, qe+1, GET_MIN( value_len, 256 ) );
                        }
                        // Domain=, Path= 等
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

            // 更新cookie
            if ( '\0' != cookieKey[0] )
            {
                // 没有设置path的，默认为root路径
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

        // 跳过 0x0d , 0x0a
        s = ++lineend ;
        if( *s == '\0' )
            break;

        s = ++lineend ;
        if( *s == '\0' )
            break;
    }
}