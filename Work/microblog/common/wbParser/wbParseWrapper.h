#ifndef __WB_PARSE_WRAPPER_H__
#define __WB_PARSE_WRAPPER_H__

#include "wbParseStruct.h"

#if (defined(WIN32) || defined(_WIN32) ) && !defined(WEIBO_PARSER_STATICLIB)

	#if defined(BUILDING_LIBWBPARSER)
		#define WBPARSER_API  __declspec(dllexport)
	#else
		#define WBPARSER_API  __declspec(dllimport)
	#endif

#else  // ����ƽ̨�ģ������Ǿ�̬����

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

		/** �ش�����չ��Ϣ */
		struct t_WBParse_Extern_info
		{
			struct t_Media
			{
				int				 nCmd;		///< Command information
				int 			 iMediaSize;///< media size
				t_wbParse_Media* pMedia;	///< media��Ϣ
			} _Media;

			struct t_MMId
			{
				int              iMMIDtype; ///< MMID type
				t_wbParser_MMId* _pMMId;    ///< MMID
			} _MMId;
		};

//--------------------  2012-03-16 add by sunfei v1.0�ӿ� ----------------------------

		// keda - ��ù�˾��֯�ܹ���������Ա��Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentAll,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentAll,Get);

		// keda - ��ȡ��˾��֯�ܹ���Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentCompany,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentCompany,Get);

		// keda - ��ȡ��������Ա��Ϣ(�������Ӳ���)
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentUser,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentUser,Get);

		// keda - ��ȡ��������Ա��Ϣ(�����Ӳ�����Ա)
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentAllUser,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentAllUser,Get);

		// keda - ����֯�ܹ�������������Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( SearchDepartment,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( SearchDepartment,Get);

		// keda - ����֯�ܹ���������Ա��Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( SearchUser,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( SearchUser,Get);

		// keda - ��ͼƬ�͸�����΢������ 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( StatuseAndPic,Upload);
		WBPARSER_API void WBFREE_FUNC_DECLARE( StatuseAndPic,Upload);

		// keda - ��ȡ@���ޡ���˿�ȳ�ʼ��Ϣ 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Notifies,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Notifies,Get);

//-----------------------------------  end  ------------------------------------------


		// keda - ��ȡ�޹��ҵ�΢������ 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GetLikeMe,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( GetLikeMe,Get);


//--------------------  2012-03-07 add by sunfei v1.0�ӿ� ----------------------------

		// keda - ��֤�ն��˺�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ValidateClientAccount,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( ValidateClientAccount,Get);

		// keda - ��ѯ�˺���ϸ��Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( AccountInfo,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( AccountInfo,Get);

		// keda - ������ѯ�˺���ϸ��Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( BatchAccountInfo,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( BatchAccountInfo,Get);

		// keda - ������ģ�����ұ���ҵ��ϵ��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ListAccountByStr,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( ListAccountByStr,Get);

		// keda - ������ѯ��ҵ��ϵ��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ConditionQuery,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( ConditionQuery,Get);

//-----------------------------------  end  ------------------------------------------


//--------------------  2012-02-29 add by sunfei v1.0�ӿ� ----------------------------

		// keda - ��ù�˾����
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FriendshipCompany,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FriendshipCompany,Get);

		// keda - ����ҵĹ�ע�б�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FriendshipAttention,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FriendshipAttention,Get);

		// keda - ����ҵķ�˿�б�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FriendshipFollowers,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FriendshipFollowers,Get);

		// keda - ����ҵĺ����б�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FriendshipFriends,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FriendshipFriends,Get);

		// keda - ��ӻ��� + ɾ������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( TopicsCreateAndDestroy,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( TopicsCreateAndDestroy,Get);

		// keda - ���like + ȡ��like
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( LikeCreateAndDestroy,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( LikeCreateAndDestroy,Get);

		// keda - ��ȡ�ҵ�Ⱥ���б� + ������е�Ⱥ���б�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Groups,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( Groups,Get);

		// keda - ���Ⱥ����
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GroupCreate,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( GroupCreate,Get);

		// keda - �鿴�ҵ�˽���б�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( PrivateContacters,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( PrivateContacters,Get);

		// keda - �鿴����ĳ�˵�˽�������¼
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( PrivateFeeds,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( PrivateFeeds,Get);

		// keda - ����˽�ŵ�ĳ��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( PrivateFeed,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( PrivateFeed,Get);

//-----------------------------------  end  ------------------------------------------



// ------------ 2012-01-09 add by sunfei v0.1�ӿ� ---------------------------------- 

		// keda - ��ȡ�û���Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( UserInfo,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( UserInfo,Get);

		// keda - ���һ�����⼰���Ӧ�Ļظ�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( SubjectAndReply,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( SubjectAndReply,Get);

		// keda - ����һ��΢��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MessagePublish,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MessagePublish,Get);

		// keda - �ظ�һ��΢��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MessageReply,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MessageReply,Get);

		// keda - ����û�������΢�����ݣ�ȫ����
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MessagePublishA,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MessagePublishA,Get);

		// keda - ����û�������΢�����ݣ���Ѷ��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MessagePublishF,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MessagePublishF,Get);

		// keda - ��õ�ǰ��¼�û������ע�û���΢��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MeAndFriend,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MeAndFriend,Get);

// ------------ 2012-02-21 add by sunfei v1.0�ӿ� ---------------------------------- 

		// keda - ��ù�˾�㳡��΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( CompanyPublic,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( CompanyPublic,Get);

		// keda - ���Ⱥ���΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GroupsData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( GroupsData,Get);

		// keda - ���@��ǰ�û���΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MentionsData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( MentionsData,Get);

		// keda - ��ûظ��ҵ�΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( RepliesData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( RepliesData,Get);

		// keda - ������޹���΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( LikesData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( LikesData,Get);

		// keda - ������ղص�΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FavoritesData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( FavoritesData,Get);

		// keda - ���ĳ�������΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( TopicsData,Get);
		WBPARSER_API void WBFREE_FUNC_DECLARE( TopicsData,Get);

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( WeiboMsg,Get);
        WBPARSER_API void WBFREE_FUNC_DECLARE( WeiboMsg,Get);

// ------------------------------------------------------------------------

		/** ������� */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE(WEIBO_ERR,ITEM);
		WBPARSER_API void WBFREE_FUNC_DECLARE(WEIBO_ERR,ITEM);

		/** cookie���� */
		WBPARSER_API void WBPARSE_FUNC_DECLARE(Cookie,BASE);

		/** ����cookie,����ָ���Ĵ� */
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
