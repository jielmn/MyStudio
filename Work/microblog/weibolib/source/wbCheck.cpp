/**
*  @brief Check the option is validate
*  @file  wbCheck.cpp
**/

#include <assert.h>

#include "libWeibo/weibo.h"
#include "wbCheck.h"
#include "wbLog.h"


#define WEIBO_check_callback(name) \
	static int Weibo_check_valid_##name##_cb(const void* t_wb)

#define WEIBO_check_fun(name) Weibo_check_valid_##name##_cb

#ifndef WEIBO_struct_cast

#define WEIBO_struct_cast( STRUCT ) \
	const struct STRUCT* pstruct = (const struct STRUCT*)t_wb;

#endif

static const char* errorText[] =
{
	"",
	"\n\nError:\n	weibo id is null.\n\n",
	"\n\nError:\n	user id is null.\n\n",
	"\n\nError:\n	text is null.\n\n",
	"\n\nError:\n	image file name is null.\n\n",
	"\n\nError:\n	weibo comment id is null.\n\n",
	"\n\nError:\n	user password is null.\n\n",
	"\n\nError:\n	oauth verify code is null.\n\n",
	"\n\nError:\n	pstruct->wbuid_.uidtype_ == t_wb_uid::IDType_unk.\n\n",

	"\n\nError:\n	keyword is null\n\n",
	"\n\nError:\n	trend id is null\n\n",
	"\n\nError:\n	tag id is null\n\n",

	// register
	"\n\nError:\n	Register:nike name is null\n\n",
	"\n\nError:\n	Register:gender is null\n\n",
	"\n\nError:\n	Register:mail is null\n\n",
	"\n\nError:\n	Register:password is null\n\n",
	"\n\nError:\n	Register:register ip is null\n\n",

	// update tgt
	"\n\nError:\n	UpdateTgt:SUE is null\n\n",
	"\n\nError:\n	UpdateTgt:SUP is null\n\n",
	"\n\nError:\n	UpdateTgt:Verify Tgt is null\n\n",

	// invite mail
	"\n\nError:\n	InviteMail:Mail ID is null\n\n",
	"\n\nError:\n	InviteMail:Mail Password is null\n\n",
	"\n\nError:\n	InviteMail:Mail Type is null\n\n",
	"\n\nError:\n	InviteMail:Mail List is null\n\n",
	"\n\nError:\n	InviteMail:Mail Nick name is null\n\n",

};


WEIBO_check_callback(UNK)
{
	return Err_OK;
}

WEIBO_check_callback(BASE)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_P_GET_ACCOUNTTOKEN)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_W_GET_ACCOUNTTOKEN)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_P_LOGIN)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_W_LOGIN)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_ME_AND_FRIENDS)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_USER_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_USER_ALL_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_SUBJECT_AND_REPLY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_MESSAGE_PUBLISH)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_MESSAGE_REPLY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_USER_INFO)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_COMPANY_PUBLIC_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_GROUPS_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_MENTIONS_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_REPLIES_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_LIKES_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_FAVORITES_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_TOPICS_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_FRIENDSHIP_COMPANY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_FRIENDSHIP_ATTENTION)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_FRIENDSHIP_FOLLOWERS)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_FRIENDSHIP_FRIENDS)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_FRIENDSHIP_CREATE)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_FRIENDSHIP_DESTROY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_FAVORITES_CREATE)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_FAVORITES_DESTROY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_TOPIC_CREATE)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_TOPIC_DESTROY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_LIKE_CREATE)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_LIKE_DESTROY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_GROUP_USER)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_GROUP_ALL)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GROUP_CREATE)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GROUP_JOIN)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GROUP_QUIT)
{
	return Err_OK;
}


WEIBO_check_callback(KEDA_API_GET_PRIVATEMESSAGE_LIST)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_PRIVATEMESSAGE_USER)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_PRIVATEMESSAGE_SEND)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_PRIVATEMESSAGE_DELETE)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_VALIDATECLIENTACCOUNT)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_GETACCOUNTINFO)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_UPDATEACCOUNTINFO)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_UPDATEPASSWORD)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_DEPARTMENT_ALL)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_DEPARTMENT_COMPANY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_DEPARTMENT_USER)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_DEPARTMENT_ALLUSER)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_DEPARTMENT_SEARCHUSER)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_DEPARTMENT_ONLINE_BY_USERID)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_PORTRAIT)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_BATCHGETACCOUNTINFO)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_CONFIRM_PORTRAIT)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_DEPARTMENT_GET_VERSION)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_MESSAGE_UPLOAD)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_MESSAGE_DELETE_THREAD)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_MESSAGE_DELETE_REPLY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_NOTIFY_GET_ALL)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_P_KEEP_HEARTBEAT)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_W_KEEP_HEARTBEAT)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_USER_INFO_BY_EMAIL)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_CURRENT_USER_INFO)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_GET_ARTICLE_URL)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_UPDATEBRIEF)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_LIST_ACCOUNT_BY_STR)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_GET_LIKES_ME_DATA)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_SET_IS_LIKE_QUERY)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_SET_AUTH_MODE)
{
	return Err_OK;
}

WEIBO_check_callback(KEDA_API_AMS_CONDITION_QUERY)
{
	return Err_OK;
}


typedef int (*f_check_callback)(const void* t_wb);
static 
f_check_callback vector_check_callbak[]=
{
	WEIBO_check_fun(UNK),
	WEIBO_check_fun(BASE),

	// 2012-01-04 update by sunfei 完成keda微博API
	WEIBO_check_fun(KEDA_API_P_GET_ACCOUNTTOKEN),
	WEIBO_check_fun(KEDA_API_W_GET_ACCOUNTTOKEN),

	WEIBO_check_fun(KEDA_API_P_LOGIN),
	WEIBO_check_fun(KEDA_API_W_LOGIN),

	WEIBO_check_fun(KEDA_API_GET_ME_AND_FRIENDS),

	WEIBO_check_fun(KEDA_API_GET_USER_DATA),
	WEIBO_check_fun(KEDA_API_GET_USER_ALL_DATA),
	WEIBO_check_fun(KEDA_API_GET_SUBJECT_AND_REPLY),
	WEIBO_check_fun(KEDA_API_MESSAGE_PUBLISH),
	WEIBO_check_fun(KEDA_API_MESSAGE_REPLY),
	WEIBO_check_fun(KEDA_API_GET_USER_INFO),
	// update end

	// 2012-02-21 add by sunfei 增加v1.0接口 第一部分
	WEIBO_check_fun(KEDA_API_GET_COMPANY_PUBLIC_DATA),
	WEIBO_check_fun(KEDA_API_GET_GROUPS_DATA),
	WEIBO_check_fun(KEDA_API_GET_MENTIONS_DATA),
	WEIBO_check_fun(KEDA_API_GET_REPLIES_DATA),
	WEIBO_check_fun(KEDA_API_GET_LIKES_DATA),
	WEIBO_check_fun(KEDA_API_GET_FAVORITES_DATA),
	WEIBO_check_fun(KEDA_API_GET_TOPICS_DATA),
	// add end

	// 2012-02-27 add by sunfei 增加v1.0接口 第二部分
	WEIBO_check_fun(KEDA_API_GET_FRIENDSHIP_COMPANY),
	WEIBO_check_fun(KEDA_API_GET_FRIENDSHIP_ATTENTION),
	WEIBO_check_fun(KEDA_API_GET_FRIENDSHIP_FOLLOWERS),
	WEIBO_check_fun(KEDA_API_GET_FRIENDSHIP_FRIENDS),

	WEIBO_check_fun(KEDA_API_FRIENDSHIP_CREATE),
	WEIBO_check_fun(KEDA_API_FRIENDSHIP_DESTROY),
	WEIBO_check_fun(KEDA_API_FAVORITES_CREATE),
	WEIBO_check_fun(KEDA_API_FAVORITES_DESTROY),
	WEIBO_check_fun(KEDA_API_TOPIC_CREATE),
	WEIBO_check_fun(KEDA_API_TOPIC_DESTROY),
	WEIBO_check_fun(KEDA_API_LIKE_CREATE),
	WEIBO_check_fun(KEDA_API_LIKE_DESTROY),

	WEIBO_check_fun(KEDA_API_GET_GROUP_USER),
	WEIBO_check_fun(KEDA_API_GET_GROUP_ALL),
	WEIBO_check_fun(KEDA_API_GROUP_CREATE),
	WEIBO_check_fun(KEDA_API_GROUP_JOIN),
	WEIBO_check_fun(KEDA_API_GROUP_QUIT),

	WEIBO_check_fun(KEDA_API_GET_PRIVATEMESSAGE_LIST),
	WEIBO_check_fun(KEDA_API_GET_PRIVATEMESSAGE_USER),
	WEIBO_check_fun(KEDA_API_PRIVATEMESSAGE_SEND),
	WEIBO_check_fun(KEDA_API_PRIVATEMESSAGE_DELETE),
	// add end

	// 2012-03-06 add by sunfei 增加v1.0接口 第三部分
	WEIBO_check_fun(KEDA_API_AMS_VALIDATECLIENTACCOUNT),
	WEIBO_check_fun(KEDA_API_AMS_GETACCOUNTINFO),
	WEIBO_check_fun(KEDA_API_AMS_UPDATEACCOUNTINFO),
	WEIBO_check_fun(KEDA_API_AMS_UPDATEPASSWORD),
	// add end

	// 2012-03-15 add by sunfei 增加v1.0接口 第四部分
	WEIBO_check_fun(KEDA_API_GET_DEPARTMENT_ALL),
	WEIBO_check_fun(KEDA_API_GET_DEPARTMENT_COMPANY),
	WEIBO_check_fun(KEDA_API_GET_DEPARTMENT_USER),
	WEIBO_check_fun(KEDA_API_GET_DEPARTMENT_ALLUSER),
	WEIBO_check_fun(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT),
	WEIBO_check_fun(KEDA_API_DEPARTMENT_SEARCHUSER),
	// add end
	WEIBO_check_fun(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID),
	WEIBO_check_fun(KEDA_API_DEPARTMENT_ONLINE_BY_USERID),

	WEIBO_check_fun(KEDA_API_GET_PORTRAIT),
	WEIBO_check_fun(KEDA_API_AMS_BATCHGETACCOUNTINFO),
	WEIBO_check_fun(KEDA_API_CONFIRM_PORTRAIT),

	WEIBO_check_fun(KEDA_API_DEPARTMENT_GET_VERSION),
	WEIBO_check_fun(KEDA_API_MESSAGE_UPLOAD),

	WEIBO_check_fun(KEDA_API_MESSAGE_DELETE_THREAD),
	WEIBO_check_fun(KEDA_API_MESSAGE_DELETE_REPLY),
	WEIBO_check_fun(KEDA_API_NOTIFY_GET_ALL),

	WEIBO_check_fun(KEDA_API_P_KEEP_HEARTBEAT),
	WEIBO_check_fun(KEDA_API_W_KEEP_HEARTBEAT),

	WEIBO_check_fun(KEDA_API_GET_USER_INFO_BY_EMAIL),

	WEIBO_check_fun(KEDA_API_GET_CURRENT_USER_INFO),
	WEIBO_check_fun(KEDA_API_GET_ARTICLE_URL),
	WEIBO_check_fun(KEDA_API_AMS_UPDATEBRIEF),
	WEIBO_check_fun(KEDA_API_AMS_LIST_ACCOUNT_BY_STR),
	WEIBO_check_fun(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL),
	WEIBO_check_fun(KEDA_API_AMS_GET_LIKES_ME_DATA),

	WEIBO_check_fun(KEDA_API_AMS_SET_IS_LIKE_QUERY),
	WEIBO_check_fun(KEDA_API_AMS_SET_AUTH_MODE),
	WEIBO_check_fun(KEDA_API_AMS_CONDITION_QUERY),
};


int Weibo_check_valid(int option ,const void* t_wb)
{
	if ( WEIBO_OPTION(UNK) >= option ||	WEIBO_OPTION(LAST) <= option )
	{
		return -1;//超出范围
	}

    if ( option >= ( sizeof(vector_check_callbak) / sizeof(f_check_callback) ) )
    {
        return 0;
    }

    if ( 0 == vector_check_callbak[option] )
    {
        return 0;
    }

	int err = vector_check_callbak[option](t_wb);
	if ( Err_OK != err )
	{
		wbPRINTF( errorText[ err ] );
	}
	return err;
}
