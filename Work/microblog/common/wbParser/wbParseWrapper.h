#ifndef __WB_PARSE_WRAPPER_H__
#define __WB_PARSE_WRAPPER_H__

#include "wbParseStruct.h"

#if (defined(WIN32) || defined(_WIN32) ) && !defined(WEIBO_PARSER_STATICLIB)

	#if defined(BUILDING_LIBWBPARSER)
		#define WBPARSER_API  __declspec(dllexport)
	#else
		#define WBPARSER_API  __declspec(dllimport)
	#endif

#else  // 其他平台的，或者是静态链接

	#define WBPARSER_API

#endif


#define WBPARSE_FUNC_NAME(interfaceType,interfaceName) Parse_##interfaceType##_##interfaceName
#define WBFREE_FUNC_NAME(interfaceType,interfaceName)  Free_##interfaceType##_##interfaceName

// Macro for Parsing
#define WBPARSE_FUNC_DECLARE(interfaceType,interfaceName)\
	WBPARSE_FUNC_NAME(interfaceType,interfaceName)(const char* sc,const int len,unsigned long &iOutCount,void **ppout,t_WBParse_Extern_info *pExternInfo,t_wbParse_Cursor **ppCursor)

#define WBPARSE_FUNC_IMPLEMENTS(interfaceType,interfaceName)\
	WBPARSE_FUNC_NAME(interfaceType,interfaceName)(const char* sc,const int len,unsigned long &iOutCount,void **ppout,t_WBParse_Extern_info *pExternInfo,t_wbParse_Cursor **ppCursor )

// Macro for Free 
#define WBFREE_FUNC_DECLARE(interfaceType,interfaceName)\
	WBFREE_FUNC_NAME(interfaceType,interfaceName)(WBPARSE_HANDLE handle)

#define WBFREE_FUNC_IMPLEMENTS(interfaceType,interfaceName)\
	WBFREE_FUNC_NAME(interfaceType,interfaceName)(WBPARSE_HANDLE handle)


// Macro for using
#define USE_WBPARSE_FUNC(interfaceType,interfaceName,sc,len,iOutCount,ppout,pExternInfo,ppCursorInfo)\
	WBPARSE_FUNC_NAME(interfaceType,interfaceName)(sc,len,iOutCount,ppout,pExternInfo,ppCursorInfo);

#define USE_WBFREE_FUNC(interfaceType,interfaceName,handle)\
	WBFREE_FUNC_NAME(interfaceType,interfaceName)(handle)

#ifdef __cplusplus
extern "C"{
#endif


	namespace wbParserNS
	{

		typedef void* WBPARSE_HANDLE;

		/** 回传的扩展信息 */
		struct t_WBParse_Extern_info
		{
			struct t_Media
			{
				int				 nCmd;		///< Command information
				int 			 iMediaSize;///< media size
				t_wbParse_Media* pMedia;	///< media信息
			} _Media;

			struct t_MMId
			{
				int              iMMIDtype; ///< MMID type
				t_wbParser_MMId* _pMMId;    ///< MMID
			} _MMId;
		};

//--------------------  2012-03-16 add by sunfei v1.0接口 ----------------------------

		// keda - 获得公司组织架构和所有人员信息
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentAll,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentAll,Get);

		// keda - 获取公司组织架构信息
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentCompany,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentCompany,Get);

		// keda - 获取部门下人员信息(不包含子部门)
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentUser,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentUser,Get);

		// keda - 获取部门下人员信息(包含子部门人员)
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentAllUser,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentAllUser,Get);

		// keda - 在组织架构中搜索部门信息
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( SearchDepartment,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( SearchDepartment,Get);

		// keda - 在组织架构中搜索人员信息
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( SearchUser,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( SearchUser,Get);

		// keda - 带图片和附件的微博发布 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( StatuseAndPic,Upload);
		WBPARSER_API void WBFREE_FUNC_DECLARE( StatuseAndPic,Upload);

		// keda - 获取@、赞、粉丝等初始消息 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Notifies,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Notifies,Get);

//-----------------------------------  end  ------------------------------------------


		// keda - 获取赞过我的微博数据 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GetLikeMe,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( GetLikeMe,Get);


//--------------------  2012-03-07 add by sunfei v1.0接口 ----------------------------

		// keda - 验证终端账号
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ValidateClientAccount,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( ValidateClientAccount,Get);

		// keda - 查询账号详细信息
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( AccountInfo,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( AccountInfo,Get);

		// keda - 批量查询账号详细信息
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( BatchAccountInfo,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( BatchAccountInfo,Get);

		// keda - 按姓名模糊查找本企业联系人
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ListAccountByStr,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( ListAccountByStr,Get);

		// keda - 条件查询企业联系人
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ConditionQuery,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( ConditionQuery,Get);

//-----------------------------------  end  ------------------------------------------


//--------------------  2012-02-29 add by sunfei v1.0接口 ----------------------------

		// keda - 获得公司人脉
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FriendshipCompany,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FriendshipCompany,Get);

		// keda - 获得我的关注列表
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FriendshipAttention,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FriendshipAttention,Get);

		// keda - 获得我的粉丝列表
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FriendshipFollowers,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FriendshipFollowers,Get);

		// keda - 获得我的好友列表
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FriendshipFriends,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FriendshipFriends,Get);

		// keda - 添加话题 + 删除话题
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( TopicsCreateAndDestroy,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( TopicsCreateAndDestroy,Get);

		// keda - 添加like + 取消like
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( LikeCreateAndDestroy,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( LikeCreateAndDestroy,Get);

		// keda - 获取我的群组列表 + 获得所有的群组列表
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Groups,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Groups,Get);

		// keda - 添加群操作
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GroupCreate,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( GroupCreate,Get);

		// keda - 查看我的私信列表
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( PrivateContacters,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( PrivateContacters,Get);

		// keda - 查看我与某人的私信聊天记录
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( PrivateFeeds,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( PrivateFeeds,Get);

		// keda - 发送私信到某人
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( PrivateFeed,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( PrivateFeed,Get);

//-----------------------------------  end  ------------------------------------------



// ------------ 2012-01-09 add by sunfei v0.1接口 ---------------------------------- 

		// keda - 获取用户信息
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( UserInfo,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( UserInfo,Get);

		// keda - 获得一条主题及其对应的回复
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( SubjectAndReply,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( SubjectAndReply,Get);

		// keda - 发布一条微博
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MessagePublish,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MessagePublish,Get);

		// keda - 回复一条微博
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MessageReply,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MessageReply,Get);

		// keda - 获得用户发布的微博数据（全部）
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MessagePublishA,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MessagePublishA,Get);

		// keda - 获得用户发布的微博数据（友讯）
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MessagePublishF,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MessagePublishF,Get);

		// keda - 获得当前登录用户及其关注用户的微博
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MeAndFriend,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MeAndFriend,Get);

// ------------ 2012-02-21 add by sunfei v1.0接口 ---------------------------------- 

		// keda - 获得公司广场的微博数据
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( CompanyPublic,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( CompanyPublic,Get);

		// keda - 获得群组的微博数据
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GroupsData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( GroupsData,Get);

		// keda - 获得@当前用户的微博数据
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MentionsData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MentionsData,Get);

		// keda - 获得回复我的微博数据
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( RepliesData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( RepliesData,Get);

		// keda - 获得我赞过的微博数据
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( LikesData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( LikesData,Get);

		// keda - 获得我收藏的微博数据
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FavoritesData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FavoritesData,Get);

		// keda - 获得某个话题的微博数据
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( TopicsData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( TopicsData,Get);

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( WeiboMsg,Get);
        WBPARSER_API void WBFREE_FUNC_DECLARE( WeiboMsg,Get);

// ------------------------------------------------------------------------

		/** 错误解析 */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(WEIBO_ERR,ITEM);
		WBPARSER_API void WBFREE_FUNC_DECLARE(WEIBO_ERR,ITEM);

		/** cookie解析 */
		WBPARSER_API void WBPARSE_FUNC_DECLARE(Cookie,BASE);

		/** 解析cookie,返回指定的串 */
		WBPARSER_API int Parse_CookieStr(const char *sc,char *token,char *secret);



        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MeetingList, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( MeetingList, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MeetingInfo, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( MeetingInfo, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FreeRoomList, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( FreeRoomList, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( NormalRoomList, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( NormalRoomList, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( RoomInfo, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( RoomInfo, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( RoomStatus, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( RoomStatus, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( RoomsByName, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( RoomsByName, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( TemplateList, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( TemplateList, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( TemplateInfo, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( TemplateInfo, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ListNotifyByMax, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( ListNotifyByMax, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GetNotifyById, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( GetNotifyById, Get );

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GetRegular, Get );
        WBPARSER_API void WBFREE_FUNC_DECLARE( GetRegular, Get );


        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( AllNotify,Get);
        WBPARSER_API void WBFREE_FUNC_DECLARE( AllNotify,Get);

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MonitorGroup,Get);
        WBPARSER_API void WBFREE_FUNC_DECLARE( MonitorGroup,Get);

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MonitorGroups,Get);
        WBPARSER_API void WBFREE_FUNC_DECLARE( MonitorGroups,Get);
	}


#ifdef __cplusplus
}
#endif


#endif //__WB_PARSE_WRAPPER_H__
