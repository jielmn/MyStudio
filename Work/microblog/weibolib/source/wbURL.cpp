/**
*  @brief URL Request.
*  @file  wbURL.cpp
**/

#include "wbURL.h"
#include "wbRequest.h"
#include "wbLog.h"
#include "wbString.h"
#include "oauth.h"
#include "strconv.h"
#include "Urlcode.h"
#include <string.h>
#include <stdlib.h>
#include <string>
#include <time.h>

#include "inner.h"

#if defined(WIN32) || defined(_WIN32)
	#pragma warning(disable:4996)
#endif

#if defined(WIN32) || defined(WINCE)
	#define STRDUP	_strdup
#else
	#define STRDUP	strdup
#endif


const int URI_MAX_LEN = 1024 * 32;   // 原来为2048
static const int MAX_POSTCONTENT_LEN = 2048;


#define URICHECK_RETURN_KEDA( funcallback ) ret = funcallback;\
	if ( 0 != ret ) return ret;\
	Weibo_url_base_keda( URI, pstruct, req_ex );

static
const lohttp::HTTPChar* Weibo_app_key_keda(void)
{
	static lohttp::HTTPChar* app_key = (lohttp::HTTPChar*)HTTP_T("kedacom");
	return app_key;
}

static 
long Weibo_url_readfile(char** outdata, const WBChar* szFile)
{
	long fSize = 0;
	char* data = NULL;
	FILE* fp = NULL;
	wchar_t* outstrW = NULL;

	if ( *szFile == '\0' )
	{
		printf("Weibo_url_readfile ==> *szFile is 0! Error!\n");
		return 0;
	}

#if defined(WIN32) || defined(_WIN32)

	fp = ::_wfopen( WBChar_2WChar(szFile), L"rb" );
	SafeFreeA_lo( outstrW );

#else

	fp = fopen(szFile,"rb");

#endif

	if ( fp == NULL )
	{
		printf("Weibo_url_readfile ==> fopen failed!\n");
		return 0;
	}

	::fseek( fp, 0, SEEK_END );
	fSize = ::ftell(fp);
	::fseek( fp, 0, SEEK_SET );

	data = new char[ fSize ];
	if ( data == NULL )
	{
		printf("Weibo_url_readfile ==> new failed!\n");
		::fclose(fp);
		fp = NULL;
		return 0;
	}

	if ( (size_t)fSize != ::fread(data, sizeof(char), fSize, fp) )
	{
		printf("Weibo_url_readfile ==> fread error!\n");
		delete data;
		data = NULL;
		::fclose(fp);
		fp = NULL;
		return 0;
	}
	::fclose(fp);

	*outdata = data;
	return fSize;
}


// static
const char* Weibo_consumer_key(void)
{
	static const char* consumer_key = "1016351065";
	return consumer_key;
}

// static
const char* Weibo_consumer_secret(void)
{
	static const char* consumer_secret = "186ea22b3ea58bfa90923f55fe8b2749";
	return consumer_secret;
}


// 上传文件 form方式 


static
void Weibo_url_form(void** httpost, void** last, lohttp::fun_formadd_write_callback cb, const char* name, const char* value)
{
	char* outstr = NULL;
	if ( lo_UrlDecodeA(&outstr, value, -1) > 0 )  // 还原没有urlencode之前 这与一般post处理不一样
	{
		value = outstr;
	}
	cb( httpost, last, lohttp::HTTP_FORMTYPE_COPYNAME, name, lohttp::HTTP_FORMTYPE_COPYCONTENTS, value, lohttp::HTTP_FORMTYPE_END );
	SafeFreeA_lo( outstr );
}

static
void Weibo_url_uploadimage_form(void** httpost, void** last, lohttp::fun_formadd_write_callback cb, void* pformdata)
{
	struct t_wb_upload_form* pupload = (struct t_wb_upload_form *)pformdata;
	char *outstr = 0;
	char *argn = 0, *argv = 0;
	char *post = 0;
	int nIndex = -1;

	if ( pupload ==	NULL )
	{
		printf("Weibo_url_uploadimage_form ==>   pupload is NULL! Error!\n");
		return ;
	}
	post = pupload->postcontent;

	while ( pupload->postcontent[0] != '\0' )
	{
		if ( *post == '\0' )
		{// 最后一个参数或只有一个参数的情况
			if ( argv && argn )
			{
				Weibo_url_form( httpost, last, cb, argn, argv );
			}
			break;
		}

		if ( !argn )
			argn = post;

		if ( *post == '=' )
		{
			*post = '\0';
			argv = ++post;
		}
		else if ( *post == '&' )
		{// 一个参数完成
			*post = '\0';
			post++;

			if ( argv && argn )
			{
				Weibo_url_form( httpost, last, cb, argn, argv );
				argv = 0;
				argn = 0;
			}
		}
		else
		{
			post++;
		}
	}

	for ( nIndex = 0; nIndex < pupload->fileCount; nIndex++ )
	{
		pupload->fSize = Weibo_url_readfile( &pupload->data, pupload->file[nIndex] );
		if ( pupload->fSize != 0 )
		{
			cb(httpost, last, lohttp::HTTP_FORMTYPE_COPYNAME,     pupload->file_formname, 
							  lohttp::HTTP_FORMTYPE_BUFFER,       HTTPChar_2UTF8(pupload->file[nIndex]), 
							  lohttp::HTTP_FORMTYPE_BUFFERPTR,    pupload->data, 
							  lohttp::HTTP_FORMTYPE_BUFFERLENGTH, pupload->fSize, 
							  lohttp::HTTP_FORMTYPE_END );

			SafeFreeA_lo(outstr);
		}
		else
		{
			printf("Weibo_url_uploadimage_form ==>   Weibo_url_readfile failed! nIndex = %d\n", nIndex);
		}
	}
}

static
void Weibo_url_uploadform_free(void* pformdata)
{
	struct t_wb_upload_form* pupload = (struct t_wb_upload_form*)pformdata;
	if ( pupload != NULL )
	{
		if ( pupload->fSize && pupload->data )
		{
			delete[] pupload->data;
		}
		free( pupload );
		pupload = NULL;
	}
}

static 
struct t_wb_upload_form* Weibo_url_uploadform_create(void)
{
	struct t_wb_upload_form* pobject = (struct t_wb_upload_form*)malloc( sizeof(struct t_wb_upload_form));
	int nIndex = -1;

	if ( pobject != NULL )
	{
		pobject->data = NULL;
		pobject->fSize = 0;
		pobject->postcontent[0] = '\0';
		pobject->file_formname[0] = '\0';

		for ( nIndex =0; nIndex < 10; nIndex++ )
		{
// 			pobject->file[nIndex][0] = '\0';
			memset(pobject->file[nIndex], 0, WB_REAL_LEN(MAX_FILE_PATH));
		}
	}
	return pobject;
}


static
int Weibo_url_cookie_set( lohttp::LOHttp* pHttp, const char *sue, const char *sup )
{
	if( sue && sup )
	{// header 
		lohttp::HTTPChar  cookieval[ 1024 ];
		wchar_t* outstrW = 0;

		HTTP_TCSCPY_S( cookieval , HTTP_T("SUE=") );
		HTTP_TCSCAT_S( cookieval , Char_2HTTPChar(sue) );
		SafeFreeA_lo( outstrW );

		HTTP_TCSCAT_S( cookieval , HTTP_T(";SUP=") );
		HTTP_TCSCAT_S( cookieval , Char_2HTTPChar(sup) );
		SafeFreeA_lo( outstrW );

		// cookie 
		lohttp::Http_setopt(pHttp , lohttp::LOHTTP_OPT_addheader , HTTP_T("Cookie") , cookieval );
	}
	return 0;
}

// 2012-01-14 add by sunfei
static
int Weibo_url_cookie_set_keda( lohttp::LOHttp* pHttp, const char *jsessionid )
{
	if ( jsessionid != NULL )
	{
		lohttp::HTTPChar cookieval[1024] = {0};
		wchar_t* outstrW = 0;

// 		HTTP_TCSCPY_S( cookieval , HTTP_T("JSESSIONID=") );
// 		HTTP_TCSCAT_S( cookieval , Char_2HTTPChar(jsessionid) );
		HTTP_TCSCPY_S( cookieval , Char_2HTTPChar(jsessionid) );
		SafeFreeA_lo( outstrW );

		lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_addheader, HTTP_T("Cookie"), cookieval );
	}
	return 0;
}
// add end


static
int Weibo_url_oauth_url(char** req_url, char** postarg,	const lohttp::HTTPChar* uri, bool post,
						const char *t_key, const char *t_secret, const char *c_key, const char *c_secret)
{
	char* outstr = NULL;

	if ( !c_key || *c_key =='\0' )
	{
		c_key =  Weibo_consumer_key(); ///< consumer key
	}

	if ( !c_secret || *c_secret =='\0' )
	{
		c_secret = Weibo_consumer_secret(); ///< consumer secret
	}

	if( post )
	{
		*req_url = oauth_sign_url2( HTTPChar_2Char(uri) , postarg, OA_HMAC, NULL, c_key, c_secret, t_key , t_secret);
		if( !(*req_url) )
		{
			SafeFreeA_lo( outstr );
			return -1; // oauth 算出来uri为空
		}
	}
	else
	{
		*req_url = oauth_sign_url2( HTTPChar_2Char(uri), NULL, OA_HMAC, NULL, c_key, c_secret , t_key , t_secret );

		if( !(*req_url) )
		{
			SafeFreeA_lo( outstr );
			return -1; // oauth 算出来uri为空
		}
	}
	SafeFreeA_lo( outstr );
	return 0;
}


static 
const lohttp::HTTPChar* Weibo_url_format_key( int format )
{
	static const lohttp::HTTPChar* format_key[] =
	{
		HTTP_T(".json"),
		HTTP_T(".xml"),
		HTTP_T(""),
	};
	if ( format > E_FORMAT_XML || format < E_FORMAT_JSON )
	{
		format = 2;
	}
	return format_key[format];
}



//static
//void Weibo_url_random(lohttp::HTTPChar* URI)
//{
//	// 增加尾部随机数的处理
//	HTTP_TCSCAT_S(URI,HTTP_T("rnd=0.") );
//	srand((unsigned int)time(NULL));
//	int paramval = rand();
//	WBChar val[16] = {0};
//	if( paramval < 0 )
//		return ;
//	_TSPRINTF(val ,"%d", HTTP_T(paramval) );
//	HTTP_TCSCAT_S( URI,val );
//}


//----------------------------------------------------------------------------------//
// for declare function
#define WEIBO_url_callback(name) \
	static int Weibo_url_url_##name##_cb(lohttp::HTTPChar URI[] , int* httpmethod  , const void* t_wb , const struct t_wb_REQ_EX* req_ex)

// for enum
#define WEIBO_url_fun(name) Weibo_url_url_##name##_cb

#ifndef WEIBO_struct_cast

#define WEIBO_struct_cast( STRUCT ) \
	const struct STRUCT* pstruct = (const struct STRUCT*)t_wb;
#endif

#define HTTP_SET_GET_METHOD() \
	if(httpmethod) *httpmethod = lohttp::E_HTTP_GET_METHOD;

#define HTTP_SET_POST_METHOD() \
	if(httpmethod) *httpmethod = lohttp::E_HTTP_POST_METHOD;

#define HTTP_SET_DELETE_METHOD() \
	if(httpmethod) *httpmethod = lohttp::E_HTTP_DELETE_METHOD;

#define HTTP_SET_UPLOAD_METHOD() \
	if(httpmethod) *httpmethod = lohttp::E_HTTP_UPLOAD_METHOD;


#define PARAM_NORMAL         (lohttp::ParamNormal)
#define PARAM_ENCODEDNAME    (lohttp::ParamEncodedName)
#define PARAM_ENCODEDVALUE   (lohttp::ParamEncodedValue)
#define PARAM_ENCODED        (lohttp::ParamEncodedName|lohttp::ParamEncodedValue)
#define PARAM_UTF8NAME       (lohttp::ParamUTF8Name)
#define PARAM_UTF8VALUE      (lohttp::ParamUTF8Value)

#ifdef  SUPPORT_UTF8
#define PARAM_ENCODE_UTF8    (lohttp::ParamEncodedValue)
#else
#define PARAM_ENCODE_UTF8    (lohttp::ParamEncodedValue|lohttp::ParamUTF8Value)
#endif


enum
{
	ParamFMT = 0,
	ParamFMT_UTF8 = 0x01,
	ParamFMT_URLENCODE = 0x02,
};

static 
void Weibo_url_paramformat( lohttp::HTTPChar URI[], const lohttp::HTTPChar* param, int paramformat )
{
	int vallen= 0;
	char* outstr = 0;
	char* urlenc = 0;

	if ( paramformat & ParamFMT_UTF8 )
	{
		HTTPChar_2UTF8( param );
	}
	else
	{
		outstr = (char *)HTTPChar_2Char( param );
	}

	if ( paramformat & ParamFMT_URLENCODE )
	{
		vallen = lo_UrlEncodeA( &urlenc, outstr, -1 );

		if ( outstr != (char *)param && outstr != NULL )
		{
			free( outstr );
			outstr = NULL;
		}

		if ( vallen <= 0 )
		{
			printf("Weibo_url_paramformat ==>  vallen <= 0  Error!\n");
			return ;
		}
	}
	else
	{
		urlenc = outstr;
	}

	HTTP_TCSCAT_S( URI , Char_2HTTPChar( urlenc ) );

	if ( urlenc != (char *)param && urlenc != NULL )
	{
		free( urlenc );
		urlenc = NULL;
	}

	return;
}

static
int Weibo_url_get_paramname_format(int paramformat)
{
	int paramf = 0;
	if( paramformat & lohttp::ParamEncodedName  )
		paramf |= ParamFMT_URLENCODE;

	if( paramformat & lohttp::ParamUTF8Name) 
		paramf |= ParamFMT_UTF8;
	return paramf;
}

static
int Weibo_url_get_paramvalue_format(int  paramformat)
{
	int paramf = 0;
	if( paramformat & lohttp::ParamEncodedValue  )
		paramf |= ParamFMT_URLENCODE;

	if( paramformat & lohttp::ParamUTF8Value) 
		paramf |= ParamFMT_UTF8;
	return paramf;
}

static 
void Weibo_url_generate_URI( lohttp::HTTPChar URI[], const lohttp::HTTPChar* paramName, const WBChar* paramval, int paramformat )
{
	if ( paramName == NULL || *paramName == '\0' )
	{
		printf( "Weibo_url_generate_URI ==> paramName非法！\n" );
		return ;
	}
	if ( paramval == NULL || *paramval == '\0' )
	{
		printf( "Weibo_url_generate_URI ==> paramval非法！\n" );
		return ;
	}
	Weibo_url_paramformat( URI, paramName, Weibo_url_get_paramname_format( paramformat ) );

	HTTP_TCSCAT_S( URI, HTTP_T("=") );

	Weibo_url_paramformat( URI, WBChar_2HTTPChar( paramval ), Weibo_url_get_paramvalue_format( paramformat ) );
}

static
void Weibo_url_generate_URI_INT(lohttp::HTTPChar URI[], const lohttp::HTTPChar* paramName, const long paramval)
{
	WBChar val[16] = {0};
	if ( paramval < 0 )
	{
		printf("Weibo_url_generate_URI_INT ==> paramval < 0, Error!\n");
		return ;
	}

	_TSPRINTF( val,"%ld", paramval );

	Weibo_url_generate_URI( URI, paramName, val, PARAM_NORMAL );
}

// 2011-12-22 add by sunfei 用于在认证之后后续接口加上oauth的认证值
static 
void Weibo_url_base_keda( lohttp::HTTPChar URI[], const void* t_wb, const struct t_wb_REQ_EX* req_ex )
{
	//WEIBO_struct_cast(t_wb_oauth);

	if ( req_ex == NULL || *(req_ex->accountToken) == '\0' )
		Weibo_url_generate_URI( URI, HTTP_T("?account_token"), Weibo_app_key_keda(), PARAM_NORMAL );
	else
		Weibo_url_generate_URI( URI, HTTP_T("?account_token"), Char_2HTTPChar(req_ex->accountToken), PARAM_NORMAL );

	return;
}



static
const lohttp::HTTPChar* Weibo_url_geturi( WEIBOoption option )
{
	struct t_urlstruct
	{
		int option;
		const lohttp::HTTPChar* url;
	};

	static const struct t_urlstruct url_vector[] = 
	{
		{WEIBO_OPTION(UNK) , HTTP_T("")},
		{WEIBO_OPTION(BASE) ,HTTP_T("")},

		{WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN)    , HTTP_T("/api/accountToken")},              // 获得Oauth的accountToken(平台API)
		{WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN)    , HTTP_T("/tsoapi/accountToken")},           // 获得Oauth的accountToken(微博API)

		{WEIBO_OPTION(KEDA_API_P_LOGIN)               , HTTP_T("/api/login")},                     // 登录平台API
		{WEIBO_OPTION(KEDA_API_W_LOGIN)               , HTTP_T("/tsoapi/login")},                  // 登录微博API

		{WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS)    , HTTP_T("/tsoapi/messages/friends/t_data")},   // 获得当前登录用户及其关注用户的微博
		{WEIBO_OPTION(KEDA_API_GET_USER_DATA)         , HTTP_T("/tsoapi/messages/user/t_data")},      // 获得用户发布的微博数据（友讯）
		{WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA)     , HTTP_T("/tsoapi/messages/user/all/t_data")},  // 获得用户发布的微博数据（全部）
		{WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY) , HTTP_T("/tsoapi/messages/comments/show")},    // 获得一条主题以及回复
		{WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH)       , HTTP_T("/tsoapi/messages/update")},           // 发布一条微博
		{WEIBO_OPTION(KEDA_API_MESSAGE_REPLY)         , HTTP_T("/tsoapi/messages/reply")},            // 回复一条微博
		{WEIBO_OPTION(KEDA_API_GET_USER_INFO)         , HTTP_T("/tsoapi/users/show")},                // 获得用户信息

		{WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA)  , HTTP_T("/tsoapi/messages/company/public_t_data")},  // 获得公司广场的微博数据
		{WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA)          , HTTP_T("/tsoapi/messages/groups/t_data")},          // 获得群组的微博数据
		{WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA)        , HTTP_T("/tsoapi/messages/mentions/t_data")},        // 获得@当前用户的微博数据
		{WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA)         , HTTP_T("/tsoapi/messages/replies/t_data")},         // 获得回复我的微博数据
		{WEIBO_OPTION(KEDA_API_GET_LIKES_DATA)           , HTTP_T("/tsoapi/messages/likes/t_data")},           // 获得我赞过的微博数据
		{WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA)       , HTTP_T("/tsoapi/messages/favorites/t_data")},       // 获得我收藏的微博数据
		{WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA)          , HTTP_T("/tsoapi/messages/topics/t_data")},          // 获得某个话题的微博数据

		{WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY)   , HTTP_T("/tsoapi/friendship/company")},     // 获得公司人脉
		{WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION) , HTTP_T("/tsoapi/friendship/attention")},   // 获得我的关注列表
		{WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS) , HTTP_T("/tsoapi/friendship/followers")},   // 获得我的粉丝列表
		{WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS)   , HTTP_T("/tsoapi/friendship/friends")},     // 获得我的好友列表

		{WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE)        , HTTP_T("/tsoapi/friendship/create")},      // 添加关注
		{WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY)       , HTTP_T("/tsoapi/friendship/destroy")},     // 取消关注
		{WEIBO_OPTION(KEDA_API_FAVORITES_CREATE)         , HTTP_T("/tsoapi/favorites/create")},       // 添加收藏
		{WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY)        , HTTP_T("/tsoapi/favorites/destroy")},      // 取消收藏
		{WEIBO_OPTION(KEDA_API_TOPIC_CREATE)             , HTTP_T("/tsoapi/topic/create")},           // 添加话题
		{WEIBO_OPTION(KEDA_API_TOPIC_DESTROY)            , HTTP_T("/tsoapi/topic/destroy")},          // 取消话题
		{WEIBO_OPTION(KEDA_API_LIKE_CREATE)              , HTTP_T("/tsoapi/like/create")},            // 添加like
		{WEIBO_OPTION(KEDA_API_LIKE_DESTROY)             , HTTP_T("/tsoapi/like/destroy")},           // 取消like

		{WEIBO_OPTION(KEDA_API_GET_GROUP_USER)           , HTTP_T("/tsoapi/group/user")},             // 获取我的群组列表
		{WEIBO_OPTION(KEDA_API_GET_GROUP_ALL)            , HTTP_T("/tsoapi/group/all")},              // 获得所有的群组列表
		{WEIBO_OPTION(KEDA_API_GROUP_CREATE)             , HTTP_T("/tsoapi/group/create")},           // 添加群操作
		{WEIBO_OPTION(KEDA_API_GROUP_JOIN)               , HTTP_T("/tsoapi/group/join")},             // 加入群操作
		{WEIBO_OPTION(KEDA_API_GROUP_QUIT)               , HTTP_T("/tsoapi/group/quit")},             // 退出群操作

		{WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST)  , HTTP_T("/tsoapi/privateMessage/list")},    // 查看我的私信列表
		{WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER)  , HTTP_T("/tsoapi/privateMessage/user")},    // 查看我与某人的私信的聊天记录	
		{WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND)      , HTTP_T("/tsoapi/privateMessage/send")},    // 发送私信到某人
		{WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE)    , HTTP_T("/tsoapi/privateMessage/delete")},  // 删除私信



		{WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT), HTTP_T("/api/ams/validateClientAccount")}, // 验证终端账号
		{WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO)       , HTTP_T("/api/ams/getAccountInfo")},        // 查询账号详细信息
		{WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO)    , HTTP_T("/api/ams/updateAccountInfo")},     // 修改账号详细信息
		{WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD)       , HTTP_T("/api/ams/updatePassword")},        // 修改账号密码

		{WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL)            , HTTP_T("/api/department/all")},             // 获得公司组织架构和所有人员信息
		{WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY)        , HTTP_T("/api/department/company")},         // 获取公司组织架构信息
		{WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER)           , HTTP_T("/api/department/user")},            // 获取部门下人员信息(不包含子部门)  
		{WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER)        , HTTP_T("/api/department/allUser")},         // 获取部门下人员信息(包含子部门人员)
		{WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT)   , HTTP_T("/api/department/searchDepartment")},// 在组织架构中搜索部门信息
		{WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER)         , HTTP_T("/api/department/searchUser")},      // 在组织架构中搜索人员信息
		{WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID)    , HTTP_T("/api/department/department/userOnline")},// 获取部门下的员工的在线状态
		{WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID)   , HTTP_T("/api/department/userOnline")},           // 获取组织里用户的在线状态


		{WEIBO_OPTION(KEDA_API_GET_PORTRAIT)                  , HTTP_T("")},                                      // 获取头像


		{WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO)       , HTTP_T("/api/ams/batchGetAccountInfo")},          // 批量查询账号详细信息
		{WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT)              , HTTP_T("/api/users/confirmPortrait")},            // 修改用户头像
		{WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION)        , HTTP_T("/api/department/getDepartmentVersion")},  // 获取组织架构版本信息


		{WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD)                , HTTP_T("/tsoapi/messages/upload")},                  // 带图片和附件的微博发布
		{WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD)         , HTTP_T("/tsoapi/messages/deleteThread")},            // 删除自己的微博
		{WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY)          , HTTP_T("/tsoapi/messages/deleteReply")},             // 删除自己的回复
		{WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL)                , HTTP_T("/tsoapi/notify/getAllNotify")},              // 获取@、赞、粉丝等初始消息


		{WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT)              , HTTP_T("/api/users/keepHeartbeat")},                 // 保持用户心跳(平台API)
		{WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT)              , HTTP_T("/tsoapi/users/keepHeartbeat")},              // 保持用户心跳(微博API)


		{WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL)        , HTTP_T("/tsoapi/users/getUserByEmail")},             // 获得用户信息(通过email)
		{WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO)         , HTTP_T("/tsoapi/users/getCurrentUser")},             // 获得当前登录用户信息
		{WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL)               , HTTP_T("/tsoapi/messages/getArticleUrl")},           // 获取长篇大论的url


		{WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF)               , HTTP_T("/api/ams/updateBrief")},                  // 修改个人介绍
		{WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR)       , HTTP_T("/api/ams/listAccountByEntAndSn")},        // 按姓名模糊查找本企业联系人
		{WEIBO_OPTION(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL)    , HTTP_T("/api/ams/password/reset/mail")},          // 忘记密码
		{WEIBO_OPTION(KEDA_API_AMS_GET_LIKES_ME_DATA)         , HTTP_T("/tsoapi/messages/likesme/t_data")},       // 获取赞过我的微博数据

		{WEIBO_OPTION(KEDA_API_AMS_SET_IS_LIKE_QUERY)         , HTTP_T("/api/ams/setIsLikeQuery")},              // 设置是否被模糊搜索到
		{WEIBO_OPTION(KEDA_API_AMS_SET_AUTH_MODE)             , HTTP_T("/api/ams/setAuthMode")},                 // 设置添加联系人认证模式
		{WEIBO_OPTION(KEDA_API_AMS_CONDITION_QUERY)           , HTTP_T("/api/ams/conditionQueryForPage")},       // 条件查询企业联系人


        // 会议管理
        {WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN)      , HTTP_T("/meetingapi/accountToken")},  
        {WEIBO_OPTION(KEDA_MEETING_LOGIN)               ,   HTTP_T("/meetingapi/login")},
        {WEIBO_OPTION(KEDA_MEETING_KEEP_HEARTBEAT)      ,   HTTP_T("/meetingapi/users/keepHeartbeat")},       

        {WEIBO_OPTION(KEDA_MEETING_MEETING_LIST)      ,     HTTP_T("/meetingapi/meeting/list")},       
        {WEIBO_OPTION(KEDA_MEETING_MEETING_INFO)      ,     HTTP_T("/meetingapi/meeting/getById")},       
        {WEIBO_OPTION(KEDA_MEETING_BOOKING_INFO)      ,     HTTP_T("/meetingapi/meeting/listByRoom")},    
        {WEIBO_OPTION(KEDA_MEETING_MEETING_LIST_OF_PERSON)      ,     HTTP_T("/meetingapi/meeting/listByUser")},    

        {WEIBO_OPTION(KEDA_MEETING_ADD_MEETING)      ,     HTTP_T("/meetingapi/meeting/add")},    
        {WEIBO_OPTION(KEDA_MEETING_MODIFY_MEETING)   ,     HTTP_T("/meetingapi/meeting/update")},  
        {WEIBO_OPTION(KEDA_MEETING_DELETE_MEETING)   ,     HTTP_T("/meetingapi/meeting/delete")},  
        {WEIBO_OPTION(KEDA_MEETING_START_MEETING)   ,     HTTP_T("/meetingapi/meeting/start")},  
        {WEIBO_OPTION(KEDA_MEETING_END_MEETING)   ,       HTTP_T("/meetingapi/meeting/end")},  
        {WEIBO_OPTION(KEDA_MEETING_FEED)   ,              HTTP_T("/meetingapi/meeting/feedback")},  

        {WEIBO_OPTION(KEDA_MEETING_QUERY_FREE_ROOMS)   ,              HTTP_T("/meetingapi/room/listFreeByCondition")},  
        {WEIBO_OPTION(KEDA_MEETING_RECOMMENDED_V_ROOM)   ,       HTTP_T("/meetingapi/room/getRecommendedVRoom")},  
        {WEIBO_OPTION(KEDA_MEETING_NORMAL_ROOMS)   ,             HTTP_T("/meetingapi/room/listNormal")},  
        {WEIBO_OPTION(KEDA_MEETING_ROOM_INFO)   ,                HTTP_T("/meetingapi/room/getById")},  
        {WEIBO_OPTION(KEDA_MEETING_ROOM_STATUS)   ,              HTTP_T("/meetingapi/room/getStatusById")},  
        {WEIBO_OPTION(KEDA_MEETING_ROOM_LIST_BY_NAME)   ,        HTTP_T("/meetingapi/room/searchByName")},  
        {WEIBO_OPTION(KEDA_MEETING_QUERY_ROOMS)   ,              HTTP_T("/meetingapi/room/advanceSearch")},  
        {WEIBO_OPTION(KEDA_MEETING_TEMPLATE_LIST)   ,            HTTP_T("/meetingapi/template/list")},  
        {WEIBO_OPTION(KEDA_MEETING_TEMPLATE_INFO)   ,            HTTP_T("/meetingapi/template/getTemplateById")}, 
        {WEIBO_OPTION(KEDA_MEETING_NOTIFY_ADD)   ,                   HTTP_T("/meetingapi/notify/add")}, 

        {WEIBO_OPTION(KEDA_MEETING_DEADLINE)   ,                 HTTP_T("/meetingapi/meeting/getDeadLine")}, 
        {WEIBO_OPTION(KEDA_MEETING_LOCK_ROOM)   ,                HTTP_T("/meetingapi/room/lockForOne")}, 
        {WEIBO_OPTION(KEDA_MEETING_LIST_NOTIFY_BY_MAX)   ,       HTTP_T("/meetingapi/notify/listNotifyByMax")}, 
        {WEIBO_OPTION(KEDA_MEETING_GET_NOTIFY_BY_ID)   ,         HTTP_T("/meetingapi/notify/getById")}, 

        {WEIBO_OPTION(KEDA_MEETING_DEL_BOOKED_ROOM)   ,         HTTP_T("/meetingapi/room/deleteBookedMeetingRoom")}, 
        {WEIBO_OPTION(KEDA_MEETING_IF_EXIST_LOCK_BY_FORMKEY)   ,  HTTP_T("/meetingapi/room/isExistLockByFormKey")}, 

        {WEIBO_OPTION(KEDA_MEETING_GET_REGULAR)   ,  HTTP_T("/meetingapi/regularMeeting/getById")}, 

        

	};

	if ( WEIBO_OPTION(BASE) >= option || WEIBO_OPTION(LAST) <= option )
	{
		printf( "Weibo_url_geturi ==> WEIBO_OPTION超出正常范围！\n" );
		return NULL;
	}
	return url_vector[option].url;
}

static
const char* get_ip( WEIBOoption option, const struct t_wb_REQ_EX* req_ex )
{
	if ( req_ex->dest_ip[0] == '\0' )
	{
		printf("get_ip ==>   dest_ip is invalid! Error!\n");
		return "";
	}
	return req_ex->dest_ip;
}

static
int Weibo_url_get_uri( lohttp::HTTPChar URI[], WEIBOoption option, const struct t_wb_REQ_EX* req_ex, bool format = true )
{
	const lohttp::HTTPChar *uri = Weibo_url_geturi( option );
	if( uri == NULL )
	{
		printf( "Weibo_url_get_uri ==> uri为NULL！\n" );
		return -1;
	};
	HTTP_SPRINTF( URI, ("%s%s%s"), get_ip( option, req_ex ), uri, Weibo_url_format_key( format?req_ex->format_:-1 ) );
	return 0;
}


static
int Weibo_url_http_geturl( char** outreq_url, char** outpostarg, const lohttp::HTTPChar* uri, int httpmethod,const void* t_wb, const struct t_wb_REQ_EX* req_ex )
{
	const char *t_key    = NULL;
	const char *t_secret = NULL;
	char* req_url = NULL;
	char* postarg = NULL;

	bool post = ( httpmethod == lohttp::E_HTTP_GET_METHOD )?false:true;

	WEIBO_struct_cast(t_wb_base);

	if ( pstruct != NULL )
	{
		t_key = pstruct->wbauth_.oauth_token_;
		t_secret = pstruct->wbauth_.oauth_token_secret_;
	}

	if ( 0 == req_ex->cookie_ )
	{
		if ( 0 != Weibo_url_oauth_url( &req_url, &postarg, uri, post, t_key, t_secret, req_ex->appkey_ , req_ex->secret_ ) )
		{
			SafeFreeA_lo(postarg);
			SafeFreeA_lo(req_url);
			printf( "Weibo_url_http_geturl ==> Weibo_url_oauth_url失败！\n" );
			return -1;
		}
	}
	else if( post != false )
	{		
		const char* req_url_ = (const char *)HTTPChar_2Char( uri );
		const char* postarg_ = strchr( req_url_, '?' );
		if ( postarg_ != NULL )
		{			
			*( (char* )postarg_ ) = '\0';

			postarg = STRDUP( postarg_ + sizeof(char) );
			req_url = STRDUP( req_url_ );

			*( (char* )postarg_ ) = '?';
		}
	}
	else
	{
		req_url = STRDUP( uri );
	}
	*outreq_url = req_url;
	*outpostarg = postarg;
	return 0;
}

static
int Weibo_url_http_seturl( lohttp::LOHttp* pHttp, const lohttp::HTTPChar* uri, int httpmethod, 
						   const void* t_wb, const struct t_wb_REQ_EX* req_ex, bool bcookie = true )
{ 
	char* req_url = 0;
	char* postarg = 0;

	WEIBO_struct_cast(t_wb_base);

	if ( 0 != Weibo_url_http_geturl( &req_url, &postarg, uri, httpmethod, t_wb, req_ex ) )
	{
		SafeFreeA_lo(req_url);
		SafeFreeA_lo(postarg);
		printf( "Weibo_url_http_seturl ==> Weibo_url_http_geturl失败！\n" );
		return -1;
	}
	// printf( "\n\nHTTP URL:\n	%s\n	post:%s\n\n", req_url, postarg ? postarg : "" );
    MICROBLOG_LOG( "\nHTTP URL(Set):\n	%s\n	post:%s\n\n", req_url, postarg ? postarg : "" );

	lohttp::Http_seturl( pHttp, Char_2HTTPChar(req_url) );
	SafeFreeA_lo(req_url);

	if ( postarg != NULL )
	{
		lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_addparam, Char_2HTTPChar( postarg ), NULL, PARAM_NORMAL );

		SafeFreeA_lo(postarg);
	}

	// 2012-01-14 add by sunfei 用于自定义cookie值处理
	if ( req_ex->cookie_ == 123 )
	{
		Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[0] );
		Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[1] );
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[2] );
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[3] );
	}
	else if ( req_ex->cookie_ && pstruct && bcookie )
	{
		Weibo_url_cookie_set( pHttp, pstruct->wbauth_.oauth_token_, pstruct->wbauth_.oauth_token_secret_ );
	}
	// add end

	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_setmethod, httpmethod );

	return 0;
}

// 2012-05-02 add by sunfei 用于【修改用户头像】
static
int Weibo_url_upload_form_portrait( lohttp::LOHttp* pHttp, const lohttp::HTTPChar* uri, int httpmethod, const void* t_wb, 
								    const char* file_formname, const struct t_wb_REQ_EX* req_ex, bool bcookie = true )
{
	char* req_url = 0;
	char* postarg = 0;
	struct t_wb_upload_form* pformdata = 0;

	WEIBO_struct_cast(t_wb_keda_confirmPortrait);

	if ( 0 != Weibo_url_http_geturl( &req_url, &postarg, uri, httpmethod, t_wb, req_ex ) )
	{
		SafeFreeA_lo(req_url);
		SafeFreeA_lo(postarg);
		printf( "Weibo_url_upload_form_portrait ==> Weibo_url_http_geturl失败！\n" );
		return -1;
	}

	// printf( "\n\nHTTP URL:\n	%s\n	post:%s\n\n", req_url, postarg?postarg:"" );
    MICROBLOG_LOG( "\nHTTP URL(Set):\n	%s\n	post:%s\n\n", req_url, postarg?postarg:"" );

	pformdata = Weibo_url_uploadform_create();

	_TCSCPY_S( pformdata->file_formname, file_formname );
	_TCSCPY_S( pformdata->file[0], pstruct->filepath );
	pformdata->fileCount = 1;

	if( postarg != NULL )
	{
		strncpy( pformdata->postcontent, postarg, MAX_POSTCONTENT_LEN );
		pformdata->postcontent[ MAX_POSTCONTENT_LEN ] = '\0';
	}

	{
		lohttp::THttpForm uploadForm;
		uploadForm.fformadd_cb = Weibo_url_uploadimage_form;
		uploadForm.ffromdata_free_cb = Weibo_url_uploadform_free;
		uploadForm.pformdata = pformdata ;

		lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_setform, &uploadForm );
	}

	lohttp::Http_seturl( pHttp, Char_2HTTPChar(req_url) );

	if ( req_ex->cookie_ == 123 )
	{
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[0] );
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[1] );
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[2] );
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[3] );
	}
	else if ( req_ex->cookie_ && pstruct && bcookie )
	{
		Weibo_url_cookie_set( pHttp, pstruct->wbauth_.oauth_token_, pstruct->wbauth_.oauth_token_secret_ );
	}

	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_setmethod, httpmethod );

	SafeFreeA_lo(req_url);
	SafeFreeA_lo(postarg);

	return 0;
}
// add end

// 2012-05-03 add by sunfei 用于【带图片和附件的微博发布】
static
int Weibo_url_upload_form_both( lohttp::LOHttp* pHttp, const lohttp::HTTPChar* uri, int httpmethod, const void* t_wb, 
							    const char* file_formname, const struct t_wb_REQ_EX* req_ex, bool bcookie = true )
{
	char* req_url = 0;
	char* postarg = 0;
	struct t_wb_upload_form* pformdata = 0;
	int nIndex = -1;

	WEIBO_struct_cast(t_wb_keda_message_upload);

	if ( 0 != Weibo_url_http_geturl( &req_url, &postarg, uri, httpmethod, t_wb, req_ex ) )
	{
		SafeFreeA_lo(req_url);
		SafeFreeA_lo(postarg);
		printf( "Weibo_url_upload_form_both ==> Weibo_url_http_geturl失败！\n" );
		return -1;
	}

	// printf( "\n\nHTTP URL:\n	%s\n	post:%s\n\n", req_url, postarg?postarg:"" );
    MICROBLOG_LOG( "\nHTTP URL(Set):\n	%s\n	post:%s\n\n", req_url, postarg?postarg:"" );

	pformdata = Weibo_url_uploadform_create();
	_TCSCPY_S( pformdata->file_formname, file_formname );

	if ( postarg != NULL )
	{
		strncpy( pformdata->postcontent, postarg, MAX_POSTCONTENT_LEN );
		pformdata->postcontent[ MAX_POSTCONTENT_LEN ] = '\0';
	}

	for ( nIndex = 0; nIndex < pstruct->fileCount; nIndex++ )
	{
		_TCSCPY_S( pformdata->file[nIndex], pstruct->filepath[nIndex] );
	}

	pformdata->fileCount = pstruct->fileCount;

	{
		lohttp::THttpForm uploadForm;
		uploadForm.fformadd_cb = Weibo_url_uploadimage_form;
		uploadForm.ffromdata_free_cb = Weibo_url_uploadform_free;
		uploadForm.pformdata = pformdata ;

		lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_setform, &uploadForm );
	}

	lohttp::Http_seturl( pHttp, Char_2HTTPChar(req_url) );

	if ( req_ex->cookie_ == 123 )
	{
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[0] );
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[1] );
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[2] );
        Weibo_url_cookie_set_keda( pHttp, req_ex->cookies[3] );
	}
	else if( req_ex->cookie_ && pstruct && bcookie )
	{
		Weibo_url_cookie_set( pHttp, pstruct->wbauth_.oauth_token_, pstruct->wbauth_.oauth_token_secret_ );
	}

	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_setmethod, httpmethod );

	SafeFreeA_lo(req_url);
	SafeFreeA_lo(postarg);

	return 0;
}
// add end

//static
//void Weibo_url_custom_seturl( lohttp::LOHttp* pHttp, const void* t_wb )
//{
//	WEIBO_struct_cast(t_wb_custom);
//
//	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_setmethod, pstruct->method_ );
//	lohttp::Http_setopt( pHttp, lohttp::LOHTTP_OPT_addparam, WBChar_2HTTPChar(pstruct->szPost_), NULL, PARAM_NORMAL );
//	lohttp::Http_seturl( pHttp, WBChar_2HTTPChar( pstruct->szUrl_ ) );
//}


//------------------------------------------------------------------------------------------------------------//
std::string StringReplace(const std::string& input,			//   原始串
						  const std::string& find,			//   替换源串
						  const std::string& replaceWith)   //   替换目的串
{
	std::string strOut(input);
	int curPos = 0;

	int pos;
	while((pos = strOut.find(find, curPos)) != -1)
	{
		//   一次替换
		strOut.replace(pos, find.size(), replaceWith);
		//   防止循环替换!!
		curPos = pos + replaceWith.size();
	}
	return strOut;
}


WEIBO_url_callback(UNK)
{
	return -1;
}

WEIBO_url_callback(BASE)
{
	return -1;
}



//1.获得Oauth的accountToken(平台API)
WEIBO_url_callback(KEDA_API_P_GET_ACCOUNTTOKEN)
{
	WEIBO_struct_cast(t_wb_keda_get_accounttoken);

	Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN), req_ex, false/*不需要格式*/ );

	Weibo_url_generate_URI( URI, HTTP_T("?oauth_consumer_key"), Char_2WBChar(pstruct->wbauth_.oauth_token_), PARAM_NORMAL );
	Weibo_url_generate_URI( URI, HTTP_T("&oauth_consumer_secret"), Char_2WBChar(pstruct->wbauth_.oauth_token_secret_), PARAM_NORMAL );

	HTTP_SET_POST_METHOD();
	return 0;
}

//1.获得Oauth的accountToken(微博API)
WEIBO_url_callback(KEDA_API_W_GET_ACCOUNTTOKEN)
{
	WEIBO_struct_cast(t_wb_keda_get_accounttoken);

	Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN), req_ex, false/*不需要格式*/ );

	Weibo_url_generate_URI( URI, HTTP_T("?oauth_consumer_key"), Char_2WBChar(pstruct->wbauth_.oauth_token_), PARAM_NORMAL );
	Weibo_url_generate_URI( URI, HTTP_T("&oauth_consumer_secret"), Char_2WBChar(pstruct->wbauth_.oauth_token_secret_), PARAM_NORMAL );

	HTTP_SET_POST_METHOD();
	return 0;
}

//2.登录平台API	
WEIBO_url_callback(KEDA_API_P_LOGIN)
{
	int ret = -1;
	WEIBO_struct_cast(t_wb_keda_login);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_P_LOGIN), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&username"), Char_2WBChar(pstruct->username), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&password"), Char_2WBChar(pstruct->password), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//2.登录微博API
WEIBO_url_callback(KEDA_API_W_LOGIN)
{
	int ret = -1;
	WEIBO_struct_cast(t_wb_keda_login);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_W_LOGIN), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&username"), Char_2WBChar(pstruct->username), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&password"), Char_2WBChar(pstruct->password), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//3.获得当前登录用户及关注用户的微博
WEIBO_url_callback(KEDA_API_GET_ME_AND_FRIENDS)
{
	int ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_me_and_friends);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//4.获得用户发布的微博数据（友讯）
WEIBO_url_callback(KEDA_API_GET_USER_DATA)
{
	int ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_user_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_USER_DATA), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//5.获得用户发布的微博数据（全部）
WEIBO_url_callback(KEDA_API_GET_USER_ALL_DATA)
{
	int ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_user_all_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//6.获得一条主题以及回复
WEIBO_url_callback(KEDA_API_GET_SUBJECT_AND_REPLY)
{
	int ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_subject_and_reply);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&thread_id"), pstruct->thread_id );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//7.发布一条微博
WEIBO_url_callback(KEDA_API_MESSAGE_PUBLISH)
{
	int ret = -1;
	WEIBO_struct_cast(t_wb_keda_message_publish);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH) , req_ex , false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&status"), Char_2WBChar(pstruct->status), PARAM_ENCODE_UTF8 );

	if ( pstruct->group_id > 0 )  // group_id为0非法
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&group_id"), pstruct->group_id );
	}

	HTTP_SET_POST_METHOD();
	return 0;
}

//8.回复一条微博
WEIBO_url_callback(KEDA_API_MESSAGE_REPLY)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_message_reply);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_MESSAGE_REPLY), req_ex, false/*不需要格式*/) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&thread_id"), pstruct->thread_id );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_id"), pstruct->reply_id );

	Weibo_url_generate_URI( URI, HTTP_T("&comment"), Char_2WBChar(pstruct->comment), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//9.获得用户信息
WEIBO_url_callback(KEDA_API_GET_USER_INFO)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_user_info);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_USER_INFO), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	HTTP_SET_GET_METHOD();
	return 0;
}



//1.获得公司广场的微博数据
WEIBO_url_callback(KEDA_API_GET_COMPANY_PUBLIC_DATA)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_company_public_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//2.获得群组的微博数据
WEIBO_url_callback(KEDA_API_GET_GROUPS_DATA)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_groups_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&group_id"), pstruct->group_id );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//3.获得@当前用户的微博数据
WEIBO_url_callback(KEDA_API_GET_MENTIONS_DATA)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_mentions_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//4.获得回复我的微博数据
WEIBO_url_callback(KEDA_API_GET_REPLIES_DATA)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_replies_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//5.获得我赞过的微博数据
WEIBO_url_callback(KEDA_API_GET_LIKES_DATA)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_likes_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_LIKES_DATA), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//6.获得我收藏的微博数据
WEIBO_url_callback(KEDA_API_GET_FAVORITES_DATA)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_favorites_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//7.获得某个话题的微博数据
WEIBO_url_callback(KEDA_API_GET_TOPICS_DATA)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_topics_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&topic_id"), pstruct->topic_id );

	if ( pstruct->since_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&since_id"), pstruct->since_id );
	}
	else if ( pstruct->max_id >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&max_id"), pstruct->max_id );
	}

	if ( pstruct->count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
	}
	if ( pstruct->reply_count >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_count"), pstruct->reply_count );
	}
	if ( pstruct->reply_style >= 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&reply_style"), pstruct->reply_style );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//8.获得公司人脉
WEIBO_url_callback(KEDA_API_GET_FRIENDSHIP_COMPANY)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_friendship_company);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->current_page > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&current_page"), pstruct->current_page );
	}
	if ( pstruct->page_size > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&page_size"), pstruct->page_size );
	}
	if ( pstruct->key[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&key"), Char_2WBChar(pstruct->key), PARAM_ENCODE_UTF8 );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//9.获得我的关注列表
WEIBO_url_callback(KEDA_API_GET_FRIENDSHIP_ATTENTION)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_friendship_attention);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	if ( pstruct->current_page > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&current_page"), pstruct->current_page );
	}
	if ( pstruct->page_size > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&page_size"), pstruct->page_size );
	}
	if ( pstruct->key[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&key"), Char_2WBChar(pstruct->key), PARAM_ENCODE_UTF8 );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//10.获得我的粉丝列表
WEIBO_url_callback(KEDA_API_GET_FRIENDSHIP_FOLLOWERS)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_friendship_followers);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	if ( pstruct->current_page > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&current_page"), pstruct->current_page );
	}
	if ( pstruct->page_size > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&page_size"), pstruct->page_size );
	}
	if ( pstruct->key[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&key"), Char_2WBChar(pstruct->key), PARAM_ENCODE_UTF8 );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//11.获得我的好友列表
WEIBO_url_callback(KEDA_API_GET_FRIENDSHIP_FRIENDS)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_friendship_friends);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	if ( pstruct->current_page > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&current_page"), pstruct->current_page );
	}
	if ( pstruct->page_size > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&page_size"), pstruct->page_size );
	}
	if ( pstruct->key[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&key"), Char_2WBChar(pstruct->key), PARAM_ENCODE_UTF8 );
	}
	HTTP_SET_GET_METHOD();
	return 0;
}

//12.添加关注
WEIBO_url_callback(KEDA_API_FRIENDSHIP_CREATE)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_friendship_create);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//13.取消关注
WEIBO_url_callback(KEDA_API_FRIENDSHIP_DESTROY)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_friendship_destroy);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//14.添加收藏
WEIBO_url_callback(KEDA_API_FAVORITES_CREATE)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_favorites_create);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_FAVORITES_CREATE), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&thread_id"), pstruct->thread_id );

	HTTP_SET_POST_METHOD();
	return 0;
}

//15.取消收藏
WEIBO_url_callback(KEDA_API_FAVORITES_DESTROY)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_favorites_destroy);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&thread_id"), pstruct->thread_id );

	HTTP_SET_POST_METHOD();
	return 0;
}

//16.添加话题 
WEIBO_url_callback(KEDA_API_TOPIC_CREATE)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_topic_create);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_TOPIC_CREATE), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&thread_id"), pstruct->thread_id );
	Weibo_url_generate_URI( URI, HTTP_T("&topic_name"), Char_2WBChar(pstruct->topic_name), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//17.取消话题
WEIBO_url_callback(KEDA_API_TOPIC_DESTROY)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_topic_destroy);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_TOPIC_DESTROY), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&thread_id"), pstruct->thread_id );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&topic_id"), pstruct->topic_id );

	HTTP_SET_POST_METHOD();
	return 0;
}

//18.添加like
WEIBO_url_callback(KEDA_API_LIKE_CREATE)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_like_create);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_LIKE_CREATE), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&thread_id"), pstruct->thread_id );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&feed_id"), pstruct->feed_id );

	HTTP_SET_POST_METHOD();
	return 0;
}

//19.取消like
WEIBO_url_callback(KEDA_API_LIKE_DESTROY)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_like_destroy);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_LIKE_DESTROY), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&thread_id"), pstruct->thread_id );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&feed_id"), pstruct->feed_id );

	HTTP_SET_POST_METHOD();
	return 0;
}

//20.获取我的群组列表
WEIBO_url_callback(KEDA_API_GET_GROUP_USER)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_group_user);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_GROUP_USER), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->current_page > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&current_page"), pstruct->current_page );
	}
	if ( pstruct->page_size > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&page_size"), pstruct->page_size );
	}
	if ( pstruct->key[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&key"), Char_2WBChar(pstruct->key), PARAM_ENCODE_UTF8 );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//21.获得所有的群组列表
WEIBO_url_callback(KEDA_API_GET_GROUP_ALL)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_group_all);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_GROUP_ALL), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->current_page > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&current_page"), pstruct->current_page );
	}
	if ( pstruct->page_size > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&page_size"), pstruct->page_size );
	}
	if ( pstruct->key[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&key"), Char_2WBChar(pstruct->key), PARAM_ENCODE_UTF8 );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//22.添加群操作
WEIBO_url_callback(KEDA_API_GROUP_CREATE)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_group_create);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GROUP_CREATE), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&group_name"), Char_2WBChar(pstruct->group_name), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&group_briefly"), Char_2WBChar(pstruct->group_briefly), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//23.加入群操作
WEIBO_url_callback(KEDA_API_GROUP_JOIN)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_group_join);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GROUP_JOIN), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&guid"), Char_2WBChar(pstruct->guid), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//24.退出群操作
WEIBO_url_callback(KEDA_API_GROUP_QUIT)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_group_quit);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GROUP_QUIT), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&guid"), Char_2WBChar(pstruct->guid), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//25.查看我的私信列表
WEIBO_url_callback(KEDA_API_GET_PRIVATEMESSAGE_LIST)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_privatemessage_list);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->current_page > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&current_page"), pstruct->current_page );
	}
	if ( pstruct->page_size > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&page_size"), pstruct->page_size );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//26.查看我与某人的私信的聊天记录
WEIBO_url_callback(KEDA_API_GET_PRIVATEMESSAGE_USER)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_privatemessage_user);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->current_page > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&current_page"), pstruct->current_page );
	}
	if ( pstruct->page_size > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&page_size"), pstruct->page_size );
	}
	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );

	HTTP_SET_GET_METHOD();
	return 0;
}

//27.发送私信到某人
WEIBO_url_callback(KEDA_API_PRIVATEMESSAGE_SEND)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_privatemessage_send);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&uid"), Char_2WBChar(pstruct->uid), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&body"), Char_2WBChar(pstruct->body), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//28.删除私信
WEIBO_url_callback(KEDA_API_PRIVATEMESSAGE_DELETE)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_privatemessage_delete);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&id"), pstruct->id );

	HTTP_SET_POST_METHOD();
	return 0;
}



//11.验证终端账号
WEIBO_url_callback(KEDA_API_AMS_VALIDATECLIENTACCOUNT)
{
	WEIBO_struct_cast(t_wb_keda_ams_validateClientAccount);

	Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT), req_ex, false/*不需要格式*/ );

	Weibo_url_generate_URI( URI, HTTP_T("?oauth_consumer_key"), Char_2WBChar(pstruct->wbauth_.oauth_token_), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&oauth_consumer_secret"), Char_2WBChar(pstruct->wbauth_.oauth_token_secret_), PARAM_ENCODE_UTF8 );

	Weibo_url_generate_URI( URI, HTTP_T("&username"), Char_2WBChar(pstruct->username), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&password"), Char_2WBChar(pstruct->password), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&deviceType"), Char_2WBChar(pstruct->deviceType), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&apiHost"), Char_2WBChar(pstruct->apiHost), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//12.查询账号详细信息
WEIBO_url_callback(KEDA_API_AMS_GETACCOUNTINFO)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_getAccountInfo);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&username"), Char_2WBChar(pstruct->username), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//13.修改账号详细信息
WEIBO_url_callback(KEDA_API_AMS_UPDATEACCOUNTINFO)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_updateAccountInfo);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&username"), Char_2WBChar(pstruct->username), PARAM_ENCODE_UTF8 );

	if ( pstruct->bitMask & UPDATE_ACCOUNT_INFO_BIT_INDEX_MOBILE )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&mobileNum"), Char_2WBChar(pstruct->mobileNum), PARAM_ENCODE_UTF8 );
	}

	if ( pstruct->bitMask & UPDATE_ACCOUNT_INFO_BIT_INDEX_EXT )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&extNum"), Char_2WBChar(pstruct->extNum), PARAM_ENCODE_UTF8 );
	}

	if ( pstruct->bitMask & UPDATE_ACCOUNT_INFO_BIT_INDEX_SEAT )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&seat"), Char_2WBChar(pstruct->seat), PARAM_ENCODE_UTF8 );
	}

	if ( pstruct->bitMask & UPDATE_ACCOUNT_INFO_BIT_INDEX_POSITION )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&position"), Char_2WBChar(pstruct->position), PARAM_ENCODE_UTF8 );
	}

	if ( pstruct->bitMask & UPDATE_ACCOUNT_INFO_BIT_INDEX_BRIEF )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&brief"), Char_2WBChar(pstruct->brief), PARAM_ENCODE_UTF8 );
	}

    if ( pstruct->bitMask & UPDATE_ACCOUNT_INFO_BIT_INDEX_SEX )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&sex"), pstruct->sex );
    }

    if ( pstruct->bitMask & UPDATE_ACCOUNT_INFO_BIT_INDEX_NAME )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&name"), Char_2WBChar(pstruct->name), PARAM_ENCODE_UTF8 );
    }

	HTTP_SET_POST_METHOD();
	return 0;
}

//14.修改账号密码
WEIBO_url_callback(KEDA_API_AMS_UPDATEPASSWORD)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_updatePassword);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&username"), Char_2WBChar(pstruct->username), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&oldPassword"), Char_2WBChar(pstruct->oldPassword), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&newPassword"), Char_2WBChar(pstruct->newPassword), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}



//1.获得公司组织架构和所有人员信息
WEIBO_url_callback(KEDA_API_GET_DEPARTMENT_ALL)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_department_all);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL), req_ex, false/*不需要格式*/ ) );

	HTTP_SET_GET_METHOD();
	return 0;
}

//2..获取公司组织架构信息
WEIBO_url_callback(KEDA_API_GET_DEPARTMENT_COMPANY)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_department_company);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->department_id > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&department_id"), pstruct->department_id );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//3.获取部门下人员信息(不包含子部门)
WEIBO_url_callback(KEDA_API_GET_DEPARTMENT_USER)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_department_user);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&department_id"), pstruct->department_id );

	HTTP_SET_GET_METHOD();
	return 0;
}

//4.获取部门下人员信息(包含子部门人员)
WEIBO_url_callback(KEDA_API_GET_DEPARTMENT_ALLUSER)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_department_allUser);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&department_id"), pstruct->department_id );

	HTTP_SET_GET_METHOD();
	return 0;
}

//5.在组织架构中搜索部门信息
WEIBO_url_callback(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_department_searchDepartment);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&department_name"), Char_2WBChar(pstruct->department_name), PARAM_ENCODE_UTF8 );

	HTTP_SET_GET_METHOD();
	return 0;
}

//6.在组织架构中搜索人员信息
WEIBO_url_callback(KEDA_API_DEPARTMENT_SEARCHUSER)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_department_searchUser);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&user_name"), Char_2WBChar(pstruct->user_name), PARAM_ENCODE_UTF8 );

	HTTP_SET_GET_METHOD();
	return 0;
}

//7.获取部门下的员工的在线状态 
WEIBO_url_callback(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_department_online_by_depId);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&department_id"), pstruct->department_id );

	HTTP_SET_GET_METHOD();
	return 0;
}

//8.获取组织里用户的在线状态 
WEIBO_url_callback(KEDA_API_DEPARTMENT_ONLINE_BY_USERID)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_department_online_by_userId);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&user_ids"), Char_2WBChar(pstruct->user_ids), PARAM_ENCODE_UTF8 );

	HTTP_SET_GET_METHOD();
	return 0;
}

//9.获取头像 
WEIBO_url_callback(KEDA_API_GET_PORTRAIT)
{
	WEIBO_struct_cast(t_wb_keda_get_portrait);

	if ( pstruct->portrait32[0] != '\0' )
	{
		if ( 0 == strncmp( pstruct->portrait32, "http://", 7 ) )
		{
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait32 ), 0 );
		}
		else
		{
			Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_PORTRAIT), req_ex, false/*不需要格式*/ );
			HTTP_TCSCAT_S( URI, HTTP_T("/") );
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait32 ), 0 );
		}
	}
	else if ( pstruct->portrait40[0] != '\0' )
	{
		if ( 0 == strncmp( pstruct->portrait40, "http://", 7 ) )
		{
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait40 ), 0 );
		}
		else
		{
			Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_PORTRAIT), req_ex, false/*不需要格式*/ );
			HTTP_TCSCAT_S( URI, HTTP_T("/") );
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait40 ), 0 );
		}
	}
	else if ( pstruct->portrait64[0] != '\0' )
	{
		if ( 0 == strncmp( pstruct->portrait64, "http://", 7 ) )
		{
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait64 ), 0 );
		}
		else
		{
			Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_PORTRAIT), req_ex, false/*不需要格式*/ );
			HTTP_TCSCAT_S( URI, HTTP_T("/") );
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait64 ), 0 );
		}
	}
	else if ( pstruct->portrait128[0] != '\0' )
	{
		if ( 0 == strncmp( pstruct->portrait128, "http://", 7 ) )
		{
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait128 ), 0 );
		}
		else
		{
			Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_PORTRAIT), req_ex, false/*不需要格式*/ );
			HTTP_TCSCAT_S( URI, HTTP_T("/") );
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait128 ), 0 );
		}
	}
	else if ( pstruct->portrait256[0] != '\0' )
	{
		if ( 0 == strncmp( pstruct->portrait256, "http://", 7 ) )
		{
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait256 ), 0 );
		}
		else
		{
			Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_PORTRAIT), req_ex, false/*不需要格式*/ );
			HTTP_TCSCAT_S( URI, HTTP_T("/") );
			Weibo_url_paramformat( URI, WBChar_2HTTPChar( pstruct->portrait256 ), 0 );
		}
	}
	else
	{
		printf("未设置图片地址，无法正确获取图片信息\n");
	}

	HTTP_SET_GET_METHOD();
	return 0;
}

//10.批量查询账号详细信息
WEIBO_url_callback(KEDA_API_AMS_BATCHGETACCOUNTINFO)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_batchGetAccountInfo);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&usernames"), Char_2WBChar(pstruct->usernames), PARAM_NORMAL );

	HTTP_SET_POST_METHOD();
	return 0;
}

//11.修改用户头像
WEIBO_url_callback(KEDA_API_CONFIRM_PORTRAIT)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_confirmPortrait);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT) , req_ex , false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&x1"), pstruct->x1 );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&y1"), pstruct->y1 );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&width"), pstruct->width );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&height"), pstruct->height );

	HTTP_SET_UPLOAD_METHOD();
	return 0;
}

//12.获取组织架构版本信息
WEIBO_url_callback(KEDA_API_DEPARTMENT_GET_VERSION)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_department_get_version);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION) , req_ex , false/*不需要格式*/ ) );

	HTTP_SET_GET_METHOD();
	return 0;
}

//13.带图片和附件的微博发布
WEIBO_url_callback(KEDA_API_MESSAGE_UPLOAD)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_message_upload);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD) , req_ex , false/*不需要格式*/ ) );

// 	Weibo_url_generate_URI_INT(URI,HTTP_T("&group_id"), pstruct->group_id );

	Weibo_url_generate_URI(URI , HTTP_T("&status")   , StringReplace(pstruct->szwbInfo_,"%","%25").c_str() , PARAM_ENCODE_UTF8 );

	HTTP_SET_UPLOAD_METHOD();
	return 0;
}

//14.删除自己的微博
WEIBO_url_callback(KEDA_API_MESSAGE_DELETE_THREAD)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_message_delete_thread);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD) , req_ex , false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&feed_id"), pstruct->feed_id );

	HTTP_SET_POST_METHOD();
	return 0;
}

//15.删除自己的回复
WEIBO_url_callback(KEDA_API_MESSAGE_DELETE_REPLY)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_message_delete_reply);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY) , req_ex , false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&feed_id"), pstruct->feed_id );

	HTTP_SET_POST_METHOD();
	return 0;
}

//16.获取@、赞、粉丝等初始消息
WEIBO_url_callback(KEDA_API_NOTIFY_GET_ALL)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_notify_get_all);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL) , req_ex , false/*不需要格式*/ ) );

	HTTP_SET_GET_METHOD();
	return 0;
}

//17.保持用户心跳(平台API)
WEIBO_url_callback(KEDA_API_P_KEEP_HEARTBEAT)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_keep_heartbeat);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT) , req_ex , false/*不需要格式*/ ) );

	HTTP_SET_POST_METHOD();
	return 0;
}

//17.保持用户心跳(微博API)
WEIBO_url_callback(KEDA_API_W_KEEP_HEARTBEAT)
{
	int   ret = -1;
	WEIBO_struct_cast(t_wb_keda_keep_heartbeat);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT) , req_ex , false/*不需要格式*/ ) );

	HTTP_SET_POST_METHOD();
	return 0;
}

//18.获得用户信息(通过email)
WEIBO_url_callback(KEDA_API_GET_USER_INFO_BY_EMAIL)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_user_info_by_email);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI,  WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI , HTTP_T("&email") , Char_2WBChar(pstruct->email) , PARAM_NORMAL );

	HTTP_SET_GET_METHOD();
	return 0;
}

//19.获得当前登录用户信息	
WEIBO_url_callback(KEDA_API_GET_CURRENT_USER_INFO)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_current_user_info);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI,  WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO), req_ex, false/*不需要格式*/ ) );

	HTTP_SET_GET_METHOD();
	return 0;
}

//20.获取长篇大论的url
WEIBO_url_callback(KEDA_API_GET_ARTICLE_URL)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_get_article_url);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&thread_id"), pstruct->thread_id );

	HTTP_SET_GET_METHOD();
	return 0;
}

//21.修改个人介绍 
WEIBO_url_callback(KEDA_API_AMS_UPDATEBRIEF)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_updateBrief);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&username"), Char_2WBChar(pstruct->username), PARAM_NORMAL );
	Weibo_url_generate_URI( URI, HTTP_T("&brief"), Char_2WBChar(pstruct->brief), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//22.按姓名模糊查找本企业联系人 
WEIBO_url_callback(KEDA_API_AMS_LIST_ACCOUNT_BY_STR)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_list_account_by_str);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&username"), Char_2WBChar(pstruct->username), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI( URI, HTTP_T("&name"), Char_2WBChar(pstruct->name), PARAM_ENCODE_UTF8 );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&startRow"), pstruct->startRow );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&pageSize"), pstruct->pageSize );

	HTTP_SET_POST_METHOD();
	return 0;
}

//23.忘记密码
WEIBO_url_callback(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_password_reset_by_mail);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&mail"), Char_2WBChar(pstruct->mail), PARAM_ENCODE_UTF8 );

	HTTP_SET_POST_METHOD();
	return 0;
}

//24.获取赞过我的微博数据
WEIBO_url_callback(KEDA_API_AMS_GET_LIKES_ME_DATA)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_get_likes_me_data);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_GET_LIKES_ME_DATA), req_ex, false/*不需要格式*/ ) );

	if ( pstruct->current_page > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&current_page"), pstruct->current_page );
	}
	if ( pstruct->page_size > 0 )
	{
		Weibo_url_generate_URI_INT( URI, HTTP_T("&page_size"), pstruct->page_size );
	}

	HTTP_SET_GET_METHOD();
	return 0;
}


//25.设置是否被模糊搜索到
WEIBO_url_callback(KEDA_API_AMS_SET_IS_LIKE_QUERY)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_set_is_like_query);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_SET_IS_LIKE_QUERY), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&jid"), Char_2WBChar(pstruct->jid), PARAM_ENCODE_UTF8 );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&isLikeQuery"), pstruct->isLikeQuery );

	HTTP_SET_POST_METHOD();
	return 0;
}

//26.设置添加联系人认证模式
WEIBO_url_callback(KEDA_API_AMS_SET_AUTH_MODE)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_set_auth_mode);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_SET_AUTH_MODE), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI( URI, HTTP_T("&jid"), Char_2WBChar(pstruct->jid), PARAM_ENCODE_UTF8 );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&authMode"), pstruct->authMode );

	HTTP_SET_POST_METHOD();
	return 0;
}

//27.条件查询企业联系人
WEIBO_url_callback(KEDA_API_AMS_CONDITION_QUERY)
{
	int  ret = -1;
	WEIBO_struct_cast(t_wb_keda_ams_condition_query);

	URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_API_AMS_CONDITION_QUERY), req_ex, false/*不需要格式*/ ) );

	Weibo_url_generate_URI_INT( URI, HTTP_T("&inEnt"), pstruct->inEnt );

	if ( pstruct->email[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&email"), Char_2WBChar(pstruct->email), PARAM_ENCODE_UTF8 );
	}
	if ( pstruct->e164[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&e164"), Char_2WBChar(pstruct->e164), PARAM_ENCODE_UTF8 );
	}
	if ( pstruct->name[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&name"), Char_2WBChar(pstruct->name), PARAM_ENCODE_UTF8 );
	}
	if ( pstruct->mobilePhone[0] != '\0' )
	{
		Weibo_url_generate_URI( URI, HTTP_T("&mobilePhone"), Char_2WBChar(pstruct->mobilePhone), PARAM_ENCODE_UTF8 );
	}

	Weibo_url_generate_URI_INT( URI, HTTP_T("&startRow"), pstruct->startRow );
	Weibo_url_generate_URI_INT( URI, HTTP_T("&pageSize"), pstruct->pageSize );

	HTTP_SET_POST_METHOD();
	return 0;
}


// 会议管理
WEIBO_url_callback(KEDA_MEETING_GET_ACCOUNTTOKEN)
{
    WEIBO_struct_cast(t_wb_keda_get_accounttoken);

    Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN), req_ex, false/*不需要格式*/ );

    Weibo_url_generate_URI( URI, HTTP_T("?oauth_consumer_key"), Char_2WBChar(pstruct->wbauth_.oauth_token_), PARAM_NORMAL );
    Weibo_url_generate_URI( URI, HTTP_T("&oauth_consumer_secret"), Char_2WBChar(pstruct->wbauth_.oauth_token_secret_), PARAM_NORMAL );

    HTTP_SET_POST_METHOD();
    return 0;
}


//2.登录会议管理API
WEIBO_url_callback(KEDA_MEETING_LOGIN)
{
    int ret = -1;
    WEIBO_struct_cast(t_wb_keda_login);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri( URI, WEIBO_OPTION(KEDA_MEETING_LOGIN), req_ex, false/*不需要格式*/ ) );

    Weibo_url_generate_URI( URI, HTTP_T("&username"), Char_2WBChar(pstruct->username), PARAM_ENCODE_UTF8 );
    Weibo_url_generate_URI( URI, HTTP_T("&password"), Char_2WBChar(pstruct->password), PARAM_ENCODE_UTF8 );

    HTTP_SET_POST_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_KEEP_HEARTBEAT)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_keep_heartbeat);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_KEEP_HEARTBEAT) , req_ex , false/*不需要格式*/ ) );

    HTTP_SET_POST_METHOD();
    return 0;
}


WEIBO_url_callback(KEDA_MEETING_MEETING_LIST)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_meeting_list);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_MEETING_LIST) , req_ex , false/*不需要格式*/ ) );

    if ( '\0' != pstruct->startDay[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&startDay"), Char_2WBChar(pstruct->startDay), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->endDay[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&endDay"), Char_2WBChar(pstruct->endDay), PARAM_ENCODE_UTF8 );
    }
    
    if ( pstruct->isConfirm >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&isConfirm"), pstruct->isConfirm );
    }

    if ( pstruct->count >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
    }

    if ( pstruct->start >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&start"), pstruct->start );
    }

    if ( pstruct->order >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&order"), pstruct->order );
    }

    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_MEETING_INFO)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_meeting_info);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_MEETING_INFO) , req_ex , false/*不需要格式*/ ) );

    Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingId"), pstruct->id );

    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_BOOKING_INFO)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_booking_info);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_BOOKING_INFO) , req_ex , false/*不需要格式*/ ) );

    if ( '\0' != pstruct->startDay[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&startTime"), Char_2WBChar(pstruct->startDay), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->endDay[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&endTime"), Char_2WBChar(pstruct->endDay), PARAM_ENCODE_UTF8 );
    }

    if ( pstruct->count >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
    }

    if ( pstruct->start >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&start"), pstruct->start );
    }

    if ( pstruct->order >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&order"), pstruct->order );
    }

    if ( pstruct->meetingRoomId >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingRoomId"), pstruct->meetingRoomId );
    }



    HTTP_SET_GET_METHOD();
    return 0;
}


WEIBO_url_callback(KEDA_MEETING_MEETING_LIST_OF_PERSON)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_meeting_list_of_person);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_MEETING_LIST_OF_PERSON) , req_ex , false/*不需要格式*/ ) );

    if ( '\0' != pstruct->startDay[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&startTime"), Char_2WBChar(pstruct->startDay), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->endDay[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&endTime"), Char_2WBChar(pstruct->endDay), PARAM_ENCODE_UTF8 );
    }

    if ( pstruct->count >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
    }

    if ( pstruct->start >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&start"), pstruct->start );
    }

    if ( pstruct->order >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&order"), pstruct->order );
    }

    if ( '\0' != pstruct->moId[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&moId"), Char_2WBChar(pstruct->moId), PARAM_ENCODE_UTF8 );
    }



    HTTP_SET_GET_METHOD();
    return 0;
}


WEIBO_url_callback(KEDA_MEETING_ADD_MEETING)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_set_meeting);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_ADD_MEETING) , req_ex , false/*不需要格式*/ ) );

    if ( '\0' != pstruct->subject[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&subject"), Char_2WBChar(pstruct->subject), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->day[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&day"), Char_2WBChar(pstruct->day), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->startShortTime[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&startShortTime"), Char_2WBChar(pstruct->startShortTime), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->endShortTime[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&endShortTime"), Char_2WBChar(pstruct->endShortTime), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->phone )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&phoneNO"), Char_2WBChar(pstruct->phone), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->mobile[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&mobilePhone"), Char_2WBChar(pstruct->mobile), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->brief[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&brief"), Char_2WBChar(pstruct->brief), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->template_id[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&templateId"), Char_2WBChar(pstruct->template_id), PARAM_ENCODE_UTF8 );
    }

    //if ( pstruct->rooms_cnt > 0 )
    //{
    //    char buf[1024+1] = { 0 };
    //    char * p = buf;
    //    int len = 0;
    //    int i;

    //    for ( i = 0; i < pstruct->rooms_cnt; i++ )
    //    {
    //        if ( len > 0 )
    //        {
    //            if ( 1024 - len > 0 )
    //            {
    //                *p = ',';
    //                p++;
    //            }
    //        }

    //        SNPRINTF( p, 1024 - len, "%d", pstruct->rooms[i] );

    //        size_t n = strlen( p );
    //        p += n;
    //        len += n;
    //    }


    //    Weibo_url_generate_URI( URI, HTTP_T("&meetingRoomIds"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    //}

    if ( pstruct->participant_cnt > 0 )
    {
        char buf[8192+1] = { 0 };
        char * p = buf;
        int len = 0;
        int i;

        for ( i = 0; i < pstruct->participant_cnt; i++ )
        {
            if ( len > 0 )
            {
                if ( 8192 - len > 0 )
                {
                    *p = ',';
                    p++;
                }
            }

            SNPRINTF( p, 8192 - len, "%s", pstruct->participants[i] );

            size_t n = strlen( p );
            p += n;
            len += n;
        }


        Weibo_url_generate_URI( URI, HTTP_T("&personAttendings"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    }

    //if ( pstruct->notify_type > 0 )
    //{
    //    char buf[1024+1] = { 0 };

    //    if ( pstruct->notify_type & MEETING_NOTIFY_TYPE_WEIBO )
    //    {
    //        strcpy( buf, "1" );
    //    }

    //    if ( pstruct->notify_type & MEETING_NOTIFY_TYPE_EMAIL )
    //    {
    //        if ( strlen( buf) > 0 )
    //        {
    //            strcat( buf, "," );
    //        }
    //        strcat( buf, "2" );
    //    }


    //    Weibo_url_generate_URI( URI, HTTP_T("&noticeTypes"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    //}


    if ( pstruct->isVideoMeeting >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&isVideoMeeting"), pstruct->isVideoMeeting );
    }

    if ( pstruct->isAutoCall >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&isAutoCall"), pstruct->isAutoCall );
    }

    //if ( pstruct->vroom_id >= 0 )
    //{
    //    Weibo_url_generate_URI_INT( URI, HTTP_T("&VRoomId"), pstruct->vroom_id );
    //}

    if ( '\0' != pstruct->meetingFormKey[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&meetingFormKey"), Char_2WBChar(pstruct->meetingFormKey), PARAM_ENCODE_UTF8 );
    }
    

    HTTP_SET_POST_METHOD();
    return 0;
}



WEIBO_url_callback(KEDA_MEETING_MODIFY_MEETING)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_set_meeting);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_MODIFY_MEETING) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->meetingId >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingId"), pstruct->meetingId );
    }

    if ( '\0' != pstruct->subject[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&subject"), Char_2WBChar(pstruct->subject), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->day[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&day"), Char_2WBChar(pstruct->day), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->startShortTime[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&startShortTime"), Char_2WBChar(pstruct->startShortTime), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->endShortTime[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&endShortTime"), Char_2WBChar(pstruct->endShortTime), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->phone )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&phoneNO"), Char_2WBChar(pstruct->phone), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->mobile[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&mobilePhone"), Char_2WBChar(pstruct->mobile), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->brief[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&brief"), Char_2WBChar(pstruct->brief), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->template_id[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&templateId"), Char_2WBChar(pstruct->template_id), PARAM_ENCODE_UTF8 );
    }

    //if ( pstruct->rooms_cnt > 0 )
    //{
    //    char buf[1024+1] = { 0 };
    //    char * p = buf;
    //    int len = 0;
    //    int i;

    //    for ( i = 0; i < pstruct->rooms_cnt; i++ )
    //    {
    //        if ( len > 0 )
    //        {
    //            if ( 1024 - len > 0 )
    //            {
    //                *p = ',';
    //                p++;
    //            }
    //        }

    //        SNPRINTF( p, 1024 - len, "%d", pstruct->rooms[i] );

    //        size_t n = strlen( p );
    //        p += n;
    //        len += n;
    //    }


    //    Weibo_url_generate_URI( URI, HTTP_T("&meetingRoomIds"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    //}

    if ( pstruct->participant_cnt > 0 )
    {
        char buf[8192+1] = { 0 };
        char * p = buf;
        int len = 0;
        int i;

        for ( i = 0; i < pstruct->participant_cnt; i++ )
        {
            if ( len > 0 )
            {
                if ( 8192 - len > 0 )
                {
                    *p = ',';
                    p++;
                }
            }

            SNPRINTF( p, 8192 - len, "%s", pstruct->participants[i] );

            size_t n = strlen( p );
            p += n;
            len += n;
        }


        Weibo_url_generate_URI( URI, HTTP_T("&personAttendings"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    }

    //if ( pstruct->notify_type > 0 )
    //{
    //    char buf[1024+1] = { 0 };

    //    if ( pstruct->notify_type & MEETING_NOTIFY_TYPE_WEIBO )
    //    {
    //        strcpy( buf, "1" );
    //    }

    //    if ( pstruct->notify_type & MEETING_NOTIFY_TYPE_EMAIL )
    //    {
    //        if ( strlen( buf) > 0 )
    //        {
    //            strcat( buf, "," );
    //        }
    //        strcat( buf, "2" );
    //    }


    //    Weibo_url_generate_URI( URI, HTTP_T("&noticeTypes"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    //}


    if ( pstruct->isVideoMeeting >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&isVideoMeeting"), pstruct->isVideoMeeting );
    }

    if ( pstruct->isAutoCall >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&isAutoCall"), pstruct->isAutoCall );
    }

    //if ( pstruct->vroom_id >= 0 )
    //{
    //    Weibo_url_generate_URI_INT( URI, HTTP_T("&VRoomId"), pstruct->vroom_id );
    //}

    if ( '\0' != pstruct->meetingFormKey[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&meetingFormKey"), Char_2WBChar(pstruct->meetingFormKey), PARAM_ENCODE_UTF8 );
    }


    HTTP_SET_POST_METHOD();
    return 0;
}


WEIBO_url_callback(KEDA_MEETING_DELETE_MEETING)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_meeting_id);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_DELETE_MEETING) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->meetingId >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingId"), pstruct->meetingId );
    }

    HTTP_SET_POST_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_START_MEETING)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_meeting_id);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_START_MEETING) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->meetingId >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingId"), pstruct->meetingId );
    }

    HTTP_SET_POST_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_END_MEETING)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_meeting_id);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_END_MEETING) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->meetingId >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingId"), pstruct->meetingId );
    }

    HTTP_SET_POST_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_FEED)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_meeting_feed);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_FEED) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->meetingId >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingId"), pstruct->meetingId );
    }

    if ( pstruct->accept >=0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&accept"), pstruct->accept );
    }

    if ( pstruct->joinType >=0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&joinType"), pstruct->joinType );
    }

    if ( '\0' != pstruct->participateNO[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&participateNO"), Char_2WBChar(pstruct->participateNO), PARAM_ENCODE_UTF8 );
    }

    if ( pstruct->reasonId >= 0 )
    {
         Weibo_url_generate_URI_INT( URI, HTTP_T("&reasonId"), pstruct->reasonId );
    }

    HTTP_SET_POST_METHOD();
    return 0;
}


WEIBO_url_callback(KEDA_MEETING_QUERY_FREE_ROOMS)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_query_rooms);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_QUERY_FREE_ROOMS) , req_ex , false/*不需要格式*/ ) );



    if ( '\0' != pstruct->startTime[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&startTime"), Char_2WBChar(pstruct->startTime), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->endTime[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&endTime"), Char_2WBChar(pstruct->endTime), PARAM_ENCODE_UTF8 );
    }

    if ( pstruct->start >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&start"), pstruct->start );
    }

    if ( pstruct->count >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
    }

    if ( pstruct->order >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&order"), pstruct->order );
    }

    if ( pstruct->peopleAdmit >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&peopleAdmit"), pstruct->peopleAdmit );
    }




    if ( pstruct->rooms_cnt > 0 )
    {
        char buf[1024+1] = { 0 };
        char * p = buf;
        int len = 0;
        int i;

        for ( i = 0; i < pstruct->rooms_cnt; i++ )
        {
            if ( len > 0 )
            {
                if ( 1024 - len > 0 )
                {
                    *p = ',';
                    p++;
                }
            }

            SNPRINTF( p, 1024 - len, "%d", pstruct->rooms[i] );

            size_t n = strlen( p );
            p += n;
            len += n;
        }

        Weibo_url_generate_URI( URI, HTTP_T("&meetingRoomIds"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    }

    if ( pstruct->deviceIdMask > 0 )
    {
        char buf[1024+1] = { 0 };

        if ( pstruct->deviceIdMask & DEVICE_ID_VIDEO_TERMINAL )
        {
            strcpy( buf, "1" );
        }

        if ( pstruct->deviceIdMask & DEVICE_ID_PROJECTOR )
        {
            if ( strlen( buf) > 0 )
            {
                strcat( buf, "," );
            }
            strcat( buf, "2" );
        }

        if ( pstruct->deviceIdMask & DEVICE_ID_AIR_CONDITIONING )
        {
            if ( strlen( buf) > 0 )
            {
                strcat( buf, "," );
            }
            strcat( buf, "3" );
        }

        if ( pstruct->deviceIdMask & DEVICE_ID_WHITE_BORD )
        {
            if ( strlen( buf) > 0 )
            {
                strcat( buf, "," );
            }
            strcat( buf, "4" );
        }


        Weibo_url_generate_URI( URI, HTTP_T("&deviceIds"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    }

    HTTP_SET_GET_METHOD();
    return 0;
}


WEIBO_url_callback(KEDA_MEETING_RECOMMENDED_V_ROOM)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_query_rooms);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_RECOMMENDED_V_ROOM) , req_ex , false/*不需要格式*/ ) );



    if ( '\0' != pstruct->startTime[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&startTime"), Char_2WBChar(pstruct->startTime), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->endTime[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&endTime"), Char_2WBChar(pstruct->endTime), PARAM_ENCODE_UTF8 );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_NORMAL_ROOMS)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_query_rooms);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_NORMAL_ROOMS) , req_ex , false/*不需要格式*/ ) );


    if ( pstruct->start >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&start"), pstruct->start );
    }

    if ( pstruct->count >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
    }

    if ( pstruct->order >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&order"), pstruct->order );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_ROOM_INFO)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_id);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_ROOM_INFO) , req_ex , false/*不需要格式*/ ) );


    if ( pstruct->id >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingRoomId"), pstruct->id );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_ROOM_STATUS)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_id);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_ROOM_STATUS) , req_ex , false/*不需要格式*/ ) );


    if ( pstruct->id >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingRoomId"), pstruct->id );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_ROOM_LIST_BY_NAME)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_query_rooms);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_ROOM_LIST_BY_NAME) , req_ex , false/*不需要格式*/ ) );


    if ( pstruct->start >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&start"), pstruct->start );
    }

    if ( pstruct->count >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
    }

    if ( pstruct->order >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&order"), pstruct->order );
    }

    if ( '\0' != pstruct->name[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&meetingRoomName"), Char_2WBChar(pstruct->name), PARAM_ENCODE_UTF8 );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_QUERY_ROOMS)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_query_rooms);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_QUERY_ROOMS) , req_ex , false/*不需要格式*/ ) );


    if ( pstruct->start >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&start"), pstruct->start );
    }

    if ( pstruct->count >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
    }

    if ( pstruct->order >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&order"), pstruct->order );
    }

    if ( pstruct->peopleAdmit >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&peopleAdmit"), pstruct->peopleAdmit );
    }


    if ( pstruct->deviceIdMask > 0 )
    {
        char buf[1024+1] = { 0 };

        if ( pstruct->deviceIdMask & DEVICE_ID_VIDEO_TERMINAL )
        {
            strcpy( buf, "1" );
        }

        if ( pstruct->deviceIdMask & DEVICE_ID_PROJECTOR )
        {
            if ( strlen( buf) > 0 )
            {
                strcat( buf, "," );
            }
            strcat( buf, "2" );
        }

        if ( pstruct->deviceIdMask & DEVICE_ID_AIR_CONDITIONING )
        {
            if ( strlen( buf) > 0 )
            {
                strcat( buf, "," );
            }
            strcat( buf, "3" );
        }

        if ( pstruct->deviceIdMask & DEVICE_ID_WHITE_BORD )
        {
            if ( strlen( buf) > 0 )
            {
                strcat( buf, "," );
            }
            strcat( buf, "4" );
        }

        Weibo_url_generate_URI( URI, HTTP_T("&deviceIds"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}


WEIBO_url_callback(KEDA_MEETING_TEMPLATE_LIST)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_query_rooms);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_TEMPLATE_LIST) , req_ex , false/*不需要格式*/ ) );


    if ( pstruct->start >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&start"), pstruct->start );
    }

    if ( pstruct->count >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&count"), pstruct->count );
    }

    if ( pstruct->order >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&order"), pstruct->order );
    }

    HTTP_SET_GET_METHOD();
    return 0;
}


WEIBO_url_callback(KEDA_MEETING_TEMPLATE_INFO)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_common_id);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_TEMPLATE_INFO) , req_ex , false/*不需要格式*/ ) );


    if ( '\0' != pstruct->id[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&id"), Char_2WBChar(pstruct->id), PARAM_ENCODE_UTF8 );
    }

    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_NOTIFY_ADD)
{
    int   ret = -1;
    WEIBO_struct_cast(t_wb_keda_m_notify);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_NOTIFY_ADD) , req_ex , false/*不需要格式*/ ) );


    if ( pstruct->id >= 0  )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingId"), pstruct->id );
    }


    if ( pstruct->notifyMask > 0 )
    {
        char buf[1024+1] = { 0 };

        if ( pstruct->notifyMask & MEETING_NOTIFY_TYPE_WEIBO )
        {
            strcpy( buf, "1" );
        }

        if ( pstruct->notifyMask & MEETING_NOTIFY_TYPE_EMAIL )
        {
            if ( strlen( buf) > 0 )
            {
                strcat( buf, "," );
            }
            strcat( buf, "2" );
        }


        Weibo_url_generate_URI( URI, HTTP_T("&noticeTypes"), Char_2WBChar(buf), PARAM_ENCODE_UTF8 );
    }

    if ( pstruct->titleType > 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&titleType"), pstruct->titleType );
    }

    HTTP_SET_POST_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_DEADLINE)
{
    int   ret = -1;
    
    WEIBO_struct_cast(t_wb_base);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_DEADLINE) , req_ex , false/*不需要格式*/ ) );

    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_LOCK_ROOM)
{
    int   ret = -1;

    WEIBO_struct_cast(t_wb_keda_m_lock_room);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_LOCK_ROOM) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->room_id >= 0  )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingRoomId"), pstruct->room_id );
    }


    if ( '\0' != pstruct->startTime[0] > 0 )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&startTime"), Char_2WBChar(pstruct->startTime), PARAM_ENCODE_UTF8 );
    }

    if ( '\0' != pstruct->endTime[0] > 0 )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&endTime"), Char_2WBChar(pstruct->endTime), PARAM_ENCODE_UTF8 );
    }

    
    HTTP_SET_POST_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_LIST_NOTIFY_BY_MAX)
{
    int   ret = -1;

    WEIBO_struct_cast(t_wb_keda_m_list_notify_by_max);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_LIST_NOTIFY_BY_MAX) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->notifyId >= 0  )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&notifyId"), pstruct->notifyId );
    }

    if ( pstruct->messageType >= 0  )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&messageType"), pstruct->messageType );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_GET_NOTIFY_BY_ID)
{
    int   ret = -1;

    WEIBO_struct_cast(t_wb_keda_m_get_notify_by_id);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_GET_NOTIFY_BY_ID) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->notifyId >= 0  )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&notifyId"), pstruct->notifyId );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_DEL_BOOKED_ROOM)
{
    int   ret = -1;

    WEIBO_struct_cast(t_wb_keda_m_meeting_id);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_DEL_BOOKED_ROOM) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->meetingId >= 0  )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&meetingId"), pstruct->meetingId );
    }


    HTTP_SET_POST_METHOD();
    return 0;
}


WEIBO_url_callback(KEDA_MEETING_IF_EXIST_LOCK_BY_FORMKEY)
{
    int   ret = -1;

    WEIBO_struct_cast(t_wb_keda_m_if_exist_lock_by_formkey);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_IF_EXIST_LOCK_BY_FORMKEY) , req_ex , false/*不需要格式*/ ) );

    if ( '\0' != pstruct->formKey[0] )
    {
        Weibo_url_generate_URI( URI, HTTP_T("&meetingFormKey"), Char_2WBChar(pstruct->formKey), PARAM_ENCODE_UTF8 );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}

WEIBO_url_callback(KEDA_MEETING_GET_REGULAR)
{
    int   ret = -1;

    WEIBO_struct_cast(t_wb_keda_m_meeting_info);

    URICHECK_RETURN_KEDA( Weibo_url_get_uri(URI ,  WEIBO_OPTION(KEDA_MEETING_GET_REGULAR) , req_ex , false/*不需要格式*/ ) );

    if ( pstruct->id >= 0 )
    {
        Weibo_url_generate_URI_INT( URI, HTTP_T("&id"), pstruct->id );
    }


    HTTP_SET_GET_METHOD();
    return 0;
}




typedef int (*f_url_callback)(lohttp::HTTPChar URI[], int* httpmethod, const void* t_wb, const struct t_wb_REQ_EX* req_ex);
static 
f_url_callback vector_url_callbak[]=
{
	WEIBO_url_fun(UNK),
	WEIBO_url_fun(BASE),

	WEIBO_url_fun(KEDA_API_P_GET_ACCOUNTTOKEN),   // 获得Oauth的accountToken(平台API)
	WEIBO_url_fun(KEDA_API_W_GET_ACCOUNTTOKEN),   // 获得Oauth的accountToken(微博API)

	WEIBO_url_fun(KEDA_API_P_LOGIN),              // 登录平台API
	WEIBO_url_fun(KEDA_API_W_LOGIN),              // 登录微博API

	WEIBO_url_fun(KEDA_API_GET_ME_AND_FRIENDS),   // 获得当前登录用户及关注用户的微博
	WEIBO_url_fun(KEDA_API_GET_USER_DATA),        // 获得用户发布的微博数据（友讯）
	WEIBO_url_fun(KEDA_API_GET_USER_ALL_DATA),    // 获得用户发布的微博数据（全部）
	WEIBO_url_fun(KEDA_API_GET_SUBJECT_AND_REPLY),// 获得一条主题以及回复
	WEIBO_url_fun(KEDA_API_MESSAGE_PUBLISH),      // 发布一条微博
	WEIBO_url_fun(KEDA_API_MESSAGE_REPLY),        // 回复一条微博
	WEIBO_url_fun(KEDA_API_GET_USER_INFO),        // 获得用户信息

	WEIBO_url_fun(KEDA_API_GET_COMPANY_PUBLIC_DATA),   // 获得公司广场的微博数据
	WEIBO_url_fun(KEDA_API_GET_GROUPS_DATA),           // 获得群组的微博数据
	WEIBO_url_fun(KEDA_API_GET_MENTIONS_DATA),         // 获得@当前用户的微博数据
	WEIBO_url_fun(KEDA_API_GET_REPLIES_DATA),          // 获得回复我的微博数据
	WEIBO_url_fun(KEDA_API_GET_LIKES_DATA),            // 获得我赞过的微博数据
	WEIBO_url_fun(KEDA_API_GET_FAVORITES_DATA),        // 获得我收藏的微博数据
	WEIBO_url_fun(KEDA_API_GET_TOPICS_DATA),           // 获得某个话题的微博数据

	WEIBO_url_fun(KEDA_API_GET_FRIENDSHIP_COMPANY),    // 获得公司人脉
	WEIBO_url_fun(KEDA_API_GET_FRIENDSHIP_ATTENTION),  // 获得我的关注列表
	WEIBO_url_fun(KEDA_API_GET_FRIENDSHIP_FOLLOWERS),  // 获得我的粉丝列表
	WEIBO_url_fun(KEDA_API_GET_FRIENDSHIP_FRIENDS),    // 获得我的好友列表

	WEIBO_url_fun(KEDA_API_FRIENDSHIP_CREATE),         // 添加关注
	WEIBO_url_fun(KEDA_API_FRIENDSHIP_DESTROY),        // 取消关注
	WEIBO_url_fun(KEDA_API_FAVORITES_CREATE),          // 添加收藏
	WEIBO_url_fun(KEDA_API_FAVORITES_DESTROY),         // 取消收藏
	WEIBO_url_fun(KEDA_API_TOPIC_CREATE),              // 添加话题
	WEIBO_url_fun(KEDA_API_TOPIC_DESTROY),             // 取消话题
	WEIBO_url_fun(KEDA_API_LIKE_CREATE),               // 添加like
	WEIBO_url_fun(KEDA_API_LIKE_DESTROY),              // 取消like

	WEIBO_url_fun(KEDA_API_GET_GROUP_USER),            // 获取我的群组列表
	WEIBO_url_fun(KEDA_API_GET_GROUP_ALL),             // 获得所有的群组列表
	WEIBO_url_fun(KEDA_API_GROUP_CREATE),              // 添加群操作
	WEIBO_url_fun(KEDA_API_GROUP_JOIN),                // 加入群操作
	WEIBO_url_fun(KEDA_API_GROUP_QUIT),                // 退出群操作

	WEIBO_url_fun(KEDA_API_GET_PRIVATEMESSAGE_LIST),   // 查看我的私信列表
	WEIBO_url_fun(KEDA_API_GET_PRIVATEMESSAGE_USER),   // 查看我与某人的私信的聊天记录
	WEIBO_url_fun(KEDA_API_PRIVATEMESSAGE_SEND),       // 发送私信到某人
	WEIBO_url_fun(KEDA_API_PRIVATEMESSAGE_DELETE),     // 删除私信

	WEIBO_url_fun(KEDA_API_AMS_VALIDATECLIENTACCOUNT), // 验证终端账号
	WEIBO_url_fun(KEDA_API_AMS_GETACCOUNTINFO),        // 查询账号详细信息
	WEIBO_url_fun(KEDA_API_AMS_UPDATEACCOUNTINFO),     // 修改账号详细信息
	WEIBO_url_fun(KEDA_API_AMS_UPDATEPASSWORD),        // 修改账号密码

	WEIBO_url_fun(KEDA_API_GET_DEPARTMENT_ALL),            // 获得公司组织架构和所有人员信息
	WEIBO_url_fun(KEDA_API_GET_DEPARTMENT_COMPANY),        // 获取公司组织架构信息
	WEIBO_url_fun(KEDA_API_GET_DEPARTMENT_USER),           // 获取部门下人员信息(不包含子部门)
	WEIBO_url_fun(KEDA_API_GET_DEPARTMENT_ALLUSER),        // 获取部门下人员信息(包含子部门人员)
	WEIBO_url_fun(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT),   // 在组织架构中搜索部门信息
	WEIBO_url_fun(KEDA_API_DEPARTMENT_SEARCHUSER),         // 在组织架构中搜索人员信息

	WEIBO_url_fun(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID),    // 获取部门下的员工的在线状态
	WEIBO_url_fun(KEDA_API_DEPARTMENT_ONLINE_BY_USERID),   // 获取组织里用户的在线状态

	WEIBO_url_fun(KEDA_API_GET_PORTRAIT),                  // 获取头像
	WEIBO_url_fun(KEDA_API_AMS_BATCHGETACCOUNTINFO),       // 批量查询账号详细信息
	WEIBO_url_fun(KEDA_API_CONFIRM_PORTRAIT),              // 修改用户头像

	WEIBO_url_fun(KEDA_API_DEPARTMENT_GET_VERSION),        // 获取组织架构版本信息
	WEIBO_url_fun(KEDA_API_MESSAGE_UPLOAD),                // 带图片和附件的微博发布

	WEIBO_url_fun(KEDA_API_MESSAGE_DELETE_THREAD),         // 删除自己的微博	
	WEIBO_url_fun(KEDA_API_MESSAGE_DELETE_REPLY),          // 删除自己的回复
	WEIBO_url_fun(KEDA_API_NOTIFY_GET_ALL),                // 获取@、赞、粉丝等初始消息

	WEIBO_url_fun(KEDA_API_P_KEEP_HEARTBEAT),              // 保持用户心跳(平台API)
	WEIBO_url_fun(KEDA_API_W_KEEP_HEARTBEAT),              // 保持用户心跳(微博API)

	WEIBO_url_fun(KEDA_API_GET_USER_INFO_BY_EMAIL),        // 获得用户信息(通过email)

	WEIBO_url_fun(KEDA_API_GET_CURRENT_USER_INFO),         // 获得当前登录用户信息
	WEIBO_url_fun(KEDA_API_GET_ARTICLE_URL),               // 获取长篇大论的url
	WEIBO_url_fun(KEDA_API_AMS_UPDATEBRIEF),               // 修改个人介绍
	WEIBO_url_fun(KEDA_API_AMS_LIST_ACCOUNT_BY_STR),       // 按姓名模糊查找本企业联系人
	WEIBO_url_fun(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL),    // 忘记密码
	WEIBO_url_fun(KEDA_API_AMS_GET_LIKES_ME_DATA),         // 获取赞过我的微博数据

	WEIBO_url_fun(KEDA_API_AMS_SET_IS_LIKE_QUERY),         // 设置是否被模糊搜索到
	WEIBO_url_fun(KEDA_API_AMS_SET_AUTH_MODE),             // 设置添加联系人认证模式
	WEIBO_url_fun(KEDA_API_AMS_CONDITION_QUERY),           // 条件查询企业联系人


    // 会议管理
    WEIBO_url_fun(KEDA_MEETING_GET_ACCOUNTTOKEN),    
    WEIBO_url_fun(KEDA_MEETING_LOGIN),              
    WEIBO_url_fun(KEDA_MEETING_KEEP_HEARTBEAT), 

    WEIBO_url_fun(KEDA_MEETING_MEETING_LIST), 
    WEIBO_url_fun(KEDA_MEETING_MEETING_INFO), 
    WEIBO_url_fun(KEDA_MEETING_BOOKING_INFO), 
    WEIBO_url_fun(KEDA_MEETING_MEETING_LIST_OF_PERSON), 
    WEIBO_url_fun(KEDA_MEETING_ADD_MEETING), 
    WEIBO_url_fun(KEDA_MEETING_MODIFY_MEETING), 
    WEIBO_url_fun(KEDA_MEETING_DELETE_MEETING), 
    WEIBO_url_fun(KEDA_MEETING_START_MEETING), 
    WEIBO_url_fun(KEDA_MEETING_END_MEETING), 
    WEIBO_url_fun(KEDA_MEETING_FEED), 
    WEIBO_url_fun(KEDA_MEETING_QUERY_FREE_ROOMS), 
    WEIBO_url_fun(KEDA_MEETING_RECOMMENDED_V_ROOM), 
    WEIBO_url_fun(KEDA_MEETING_NORMAL_ROOMS), 
    WEIBO_url_fun(KEDA_MEETING_ROOM_INFO), 
    WEIBO_url_fun(KEDA_MEETING_ROOM_STATUS), 
    WEIBO_url_fun(KEDA_MEETING_ROOM_LIST_BY_NAME), 
    WEIBO_url_fun(KEDA_MEETING_QUERY_ROOMS), 
    WEIBO_url_fun(KEDA_MEETING_TEMPLATE_LIST), 
    WEIBO_url_fun(KEDA_MEETING_TEMPLATE_INFO), 
    WEIBO_url_fun(KEDA_MEETING_NOTIFY_ADD), 
    WEIBO_url_fun(KEDA_MEETING_DEADLINE), 
    WEIBO_url_fun(KEDA_MEETING_LOCK_ROOM), 
    WEIBO_url_fun(KEDA_MEETING_LIST_NOTIFY_BY_MAX),     
    WEIBO_url_fun(KEDA_MEETING_GET_NOTIFY_BY_ID),   
    WEIBO_url_fun(KEDA_MEETING_DEL_BOOKED_ROOM),   
    WEIBO_url_fun(KEDA_MEETING_IF_EXIST_LOCK_BY_FORMKEY),   
    WEIBO_url_fun(KEDA_MEETING_GET_REGULAR),   
    
};

int Weibo_url_set(lohttp::LOHttp* pHttp, WEIBOoption option, const void* t_wb, const struct t_wb_REQ_EX* req_ex)
{
	lohttp::HTTPChar URI[URI_MAX_LEN] = {0};
	int httpmethod = 0;

	if ( WEIBO_OPTION(UNK) >= option ||
		 WEIBO_OPTION(LAST) <= option )
	{
		printf("Weibo_url_set ==> 超出WEIBO_OPTION的正常范围！\n");
		return -1;
	}

	if ( 0 == vector_url_callbak[option]( URI, &httpmethod, t_wb ,req_ex ) )
	{
		switch( option )
		{
		case WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN):
		case WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN):

		case WEIBO_OPTION(KEDA_API_P_LOGIN):
		case WEIBO_OPTION(KEDA_API_W_LOGIN):

		case WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS):
		case WEIBO_OPTION(KEDA_API_GET_USER_DATA):
		case WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA):
		case WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY):
		case WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH):
		case WEIBO_OPTION(KEDA_API_MESSAGE_REPLY):
		case WEIBO_OPTION(KEDA_API_GET_USER_INFO):

		case WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA):
		case WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA):
		case WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA):
		case WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA):
		case WEIBO_OPTION(KEDA_API_GET_LIKES_DATA):
		case WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA):
		case WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA):

		case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY):
		case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION):
		case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS):
		case WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS):

		case WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE):
		case WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY):
		case WEIBO_OPTION(KEDA_API_FAVORITES_CREATE):
		case WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY):
		case WEIBO_OPTION(KEDA_API_TOPIC_CREATE):
		case WEIBO_OPTION(KEDA_API_TOPIC_DESTROY):
		case WEIBO_OPTION(KEDA_API_LIKE_CREATE):
		case WEIBO_OPTION(KEDA_API_LIKE_DESTROY):

		case WEIBO_OPTION(KEDA_API_GET_GROUP_USER):
		case WEIBO_OPTION(KEDA_API_GET_GROUP_ALL):
		case WEIBO_OPTION(KEDA_API_GROUP_CREATE):
		case WEIBO_OPTION(KEDA_API_GROUP_JOIN):
		case WEIBO_OPTION(KEDA_API_GROUP_QUIT):

		case WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST):
		case WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER):
		case WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND):
		case WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE):

		case WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT):
		case WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO):
		case WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO):
		case WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD):

		case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL):
		case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY):
		case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER):
		case WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER):
		case WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT):
		case WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER):

		case WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID):
		case WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID):
		case WEIBO_OPTION(KEDA_API_GET_PORTRAIT):
		case WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO):
		case WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION):

		case WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD):
		case WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY):
		case WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL):

		case WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT):
		case WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT):

		case WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL):

		case WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO):
		case WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL):
		case WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF):
		case WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR):
		case WEIBO_OPTION(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL):
		case WEIBO_OPTION(KEDA_API_AMS_GET_LIKES_ME_DATA):

		case WEIBO_OPTION(KEDA_API_AMS_SET_IS_LIKE_QUERY):
		case WEIBO_OPTION(KEDA_API_AMS_SET_AUTH_MODE):
		case WEIBO_OPTION(KEDA_API_AMS_CONDITION_QUERY):

        case WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN):
        case WEIBO_OPTION(KEDA_MEETING_LOGIN):
        case WEIBO_OPTION(KEDA_MEETING_KEEP_HEARTBEAT):

        case WEIBO_OPTION(KEDA_MEETING_MEETING_LIST):
        case WEIBO_OPTION(KEDA_MEETING_MEETING_INFO):
        case WEIBO_OPTION(KEDA_MEETING_BOOKING_INFO):
        case WEIBO_OPTION(KEDA_MEETING_MEETING_LIST_OF_PERSON):
        case WEIBO_OPTION(KEDA_MEETING_ADD_MEETING):
        case WEIBO_OPTION(KEDA_MEETING_MODIFY_MEETING):
        case WEIBO_OPTION(KEDA_MEETING_DELETE_MEETING):
        case WEIBO_OPTION(KEDA_MEETING_START_MEETING):
        case WEIBO_OPTION(KEDA_MEETING_END_MEETING):
        case WEIBO_OPTION(KEDA_MEETING_FEED):
        case WEIBO_OPTION(KEDA_MEETING_QUERY_FREE_ROOMS):
        case WEIBO_OPTION(KEDA_MEETING_RECOMMENDED_V_ROOM):
        case WEIBO_OPTION(KEDA_MEETING_NORMAL_ROOMS):
        case WEIBO_OPTION(KEDA_MEETING_ROOM_INFO):
        case WEIBO_OPTION(KEDA_MEETING_ROOM_STATUS):
        case WEIBO_OPTION(KEDA_MEETING_ROOM_LIST_BY_NAME):
        case WEIBO_OPTION(KEDA_MEETING_QUERY_ROOMS):
        case WEIBO_OPTION(KEDA_MEETING_TEMPLATE_LIST):
        case WEIBO_OPTION(KEDA_MEETING_TEMPLATE_INFO):
        case WEIBO_OPTION(KEDA_MEETING_NOTIFY_ADD):
        case WEIBO_OPTION(KEDA_MEETING_DEADLINE):
        case WEIBO_OPTION(KEDA_MEETING_LOCK_ROOM):
        case WEIBO_OPTION(KEDA_MEETING_LIST_NOTIFY_BY_MAX):
        case WEIBO_OPTION(KEDA_MEETING_GET_NOTIFY_BY_ID):
        case WEIBO_OPTION(KEDA_MEETING_DEL_BOOKED_ROOM):
        case WEIBO_OPTION(KEDA_MEETING_IF_EXIST_LOCK_BY_FORMKEY):
        case WEIBO_OPTION(KEDA_MEETING_GET_REGULAR):
			{
				return Weibo_url_http_seturl( pHttp, URI, httpmethod, t_wb, req_ex, false );
			}
			break;

		case WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT):  // 【修改用户头像】
			{
				return Weibo_url_upload_form_portrait( pHttp, URI, httpmethod, t_wb, "file", req_ex, false );
			}
			break;

		case WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD):   // 【带图片和附件的微博发布】
			{
				return Weibo_url_upload_form_both( pHttp, URI, httpmethod, t_wb, "file", req_ex, false );
			}
			break;

		default:
			{
				printf("Weibo_url_set ==> 没有case匹配当前的API！\n");
			}
			break;
		}
	}
	else
	{
		printf("Weibo_url_set ==> vector_url_callbak失败！\n");

	}
	return -1;
}


int Weibo_url_get(struct t_wb_URI* outURI, WEIBOoption option, const void* t_wb, const struct t_wb_REQ_EX* req_ex)
{
	int  ret = -1;
	lohttp::HTTPChar URI[URI_MAX_LEN]={0};
	int httpmethod = 0;
	char* req_url = NULL;
	char* postarg = NULL;

	if ( WEIBO_OPTION(BASE) >= option ||
		 WEIBO_OPTION(LAST) <= option || !outURI )
	{
		printf("Weibo_url_get ==> option is out of range! Error!\n");
		return -1;
	}

	if ( 0 != vector_url_callbak[option](URI, &httpmethod, t_wb, req_ex) )
	{
		return -1;
	}

	ret = Weibo_url_http_geturl( &req_url, &postarg, URI, httpmethod, t_wb, req_ex );

	if ( 0 == ret )
	{
		if ( req_url ) strcpy( outURI->uri_, req_url );
		if ( postarg ) strcpy( outURI->post_, postarg );
	}

	SafeFreeA_lo(req_url);
	SafeFreeA_lo(postarg);

	return ret;
}
