#if (defined(WIN32) || defined(_WIN32) )
	#include <windows.h>
#else
	#include <sys/time.h>
#endif

#include "wbParser/splitstr.h"
#include "wbParser/wbParseWrapper.h"
#include "wbParseImpl.h"
#include "Urlcode.h"
#include "strconv.h"

#include <tinyxml/tinyxml.h>

#define MMID_TYPE(p,type) if( pExternInfo){ pExternInfo->_MMId.iMMIDtype = type; }





#ifdef __cplusplus
extern "C"{
#endif //__cplusplus


	namespace wbParserNS
	{

//--------------------  2012-03-16 add by sunfei v1.0�ӿ� ----------------------------

		// keda - ��ù�˾��֯�ܹ���������Ա��Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentAll,Get)
		{
// 			iOutCount = 1;
			CParseKedaDepartmentsSingleT* pParseObj = new CONSTRUCT_KEDADEPARTMENTS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentAll,Get)
		{
			if (handle) delete ((CParseKedaDepartmentsSingleT*)handle);
		}

		// keda - ��ȡ��˾��֯�ܹ���Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentCompany,Get)
		{
// 			iOutCount = 1;
			CParseKedaDepartmentsSingleT* pParseObj = new CONSTRUCT_KEDADEPARTMENTS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentCompany,Get)
		{
			if (handle) delete ((CParseKedaDepartmentsSingleT*)handle);
		}


		// keda - ��ȡ��������Ա��Ϣ(�������Ӳ���)
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentUser,Get)
		{
// 			iOutCount = 1;
			CParseKedaDepartmentSingleT* pParseObj = new CONSTRUCT_KEDADEPARTMENT_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentUser,Get)
		{
			if (handle) delete ((CParseKedaDepartmentSingleT*)handle);
		}

		// keda - ��ȡ��������Ա��Ϣ(�����Ӳ�����Ա)
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( DepartmentAllUser,Get)
		{
			// 			iOutCount = 1;
			CParseKedaDepartmentSingleT* pParseObj = new CONSTRUCT_KEDADEPARTMENT_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( DepartmentAllUser,Get)
		{
			if (handle) delete ((CParseKedaDepartmentSingleT*)handle);
		}

		// keda - ����֯�ܹ�������������Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( SearchDepartment,Get)
		{
// 			iOutCount = 1;
			CParseKedaSearchDepartmentSingleT* pParseObj = new CONSTRUCT_KEDASEARCHDEPARTMENT_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( SearchDepartment,Get)
		{
			if (handle) delete ((CParseKedaSearchDepartmentSingleT*)handle);
		}

		// keda - ����֯�ܹ���������Ա��Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( SearchUser,Get)
		{
// 			iOutCount = 1;
			CParseKedaSearchUserSingleT* pParseObj = new CONSTRUCT_KEDASEARCHUSER_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( SearchUser,Get)
		{
			if (handle) delete ((CParseKedaSearchUserSingleT*)handle);
		}

//-----------------------------------  end  ------------------------------------------

		// keda - ��ȡ�޹��ҵ�΢������ 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GetLikeMe,Get)
		{
// 			iOutCount = 1;
			CParseKedaLikePageSingleT* pParseObj = new CONSTRUCT_KEDALIKEPAGE_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( GetLikeMe,Get)
		{
			if (handle) delete ((CParseKedaLikePageSingleT*)handle);
		}

//--------------------  2012-03-07 add by sunfei v1.0�ӿ� ----------------------------

		// keda - ��֤�ն��˺�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ValidateClientAccount,Get)
		{
// 			iOutCount = 1;
			CParseKedaResultSingleT* pParseObj = new CONSTRUCT_KEDARESULT_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( ValidateClientAccount,Get)
		{
			if (handle) delete ((CParseKedaResultSingleT*)handle);
		}

		// keda - ��ѯ�˺���ϸ��Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( AccountInfo,Get)
		{
// 			iOutCount = 1;
			CParseKedaEntUserSingleT* pParseObj = new CONSTRUCT_KEDAENTUSER_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( AccountInfo,Get)
		{
			if (handle) delete ((CParseKedaEntUserSingleT*)handle);
		}

		// keda - ������ѯ�˺���ϸ��Ϣ
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( BatchAccountInfo,Get)
		{
			// 			iOutCount = 1;
			CParseKedaEntUsersSingleT* pParseObj = new CONSTRUCT_KEDAENTUSERS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( BatchAccountInfo,Get)
		{
			if (handle) delete ((CParseKedaEntUsersSingleT*)handle);
		}

		// keda - ������ģ�����ұ���ҵ��ϵ��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ListAccountByStr,Get)
		{
			// 			iOutCount = 1;
			CParseKedaPageSingleT* pParseObj = new CONSTRUCT_KEDAPAGE_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( ListAccountByStr,Get)
		{
			if (handle) delete ((CParseKedaPageSingleT*)handle);
		}

		// keda - ������ѯ��ҵ��ϵ��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ConditionQuery,Get)
		{
			// 			iOutCount = 1;
			CParseKedaPageSingleT* pParseObj = new CONSTRUCT_KEDAPAGE_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( ConditionQuery,Get)
		{
			if (handle) delete ((CParseKedaPageSingleT*)handle);
		}

		// keda - ��ͼƬ�͸�����΢������ 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( StatuseAndPic,Upload)
		{
			// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( StatuseAndPic,Upload)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ��ȡ@���ޡ���˿�ȳ�ʼ��Ϣ 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( Notifies,Get)
		{
			// 			iOutCount = 1;
			CParseKedaNotifiesSingleT* pParseObj = new CONSTRUCT_KEDANOTIFIES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_DECLARE( Notifies,Get)
		{
			if (handle) delete ((CParseKedaNotifiesSingleT*)handle);
		}


//-----------------------------------  end  ------------------------------------------


//--------------------  2012-02-29 add by sunfei v1.0�ӿ� ----------------------------

		// keda - ��ù�˾���� 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( FriendshipCompany,Get)
		{
// 			iOutCount = 1;
			CParseKedaUsersSingleT* pParseObj = new CONSTRUCT_KEDAUSERS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( FriendshipCompany,Get)
		{
			if (handle) delete ((CParseKedaUsersSingleT*)handle);
		}

		// keda - ����ҵĹ�ע�б� 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( FriendshipAttention,Get)
		{
// 			iOutCount = 1;
			CParseKedaUsersSingleT* pParseObj = new CONSTRUCT_KEDAUSERS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( FriendshipAttention,Get)
		{
			if (handle) delete ((CParseKedaUsersSingleT*)handle);
		}

		// keda - ����ҵķ�˿�б� 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( FriendshipFollowers,Get)
		{
// 			iOutCount = 1;
			CParseKedaUsersSingleT* pParseObj = new CONSTRUCT_KEDAUSERS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( FriendshipFollowers,Get)
		{
			if (handle) delete ((CParseKedaUsersSingleT*)handle);
		}

		// keda - ����ҵĺ����б� 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( FriendshipFriends,Get)
		{
// 			iOutCount = 1;
			CParseKedaUsersSingleT* pParseObj = new CONSTRUCT_KEDAUSERS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( FriendshipFriends,Get)
		{
			if (handle) delete ((CParseKedaUsersSingleT*)handle);
		}

		// keda - ��ӻ��� 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( TopicsCreateAndDestroy,Get)
		{
// 			iOutCount = 1;
			CParseKedaTopicSingleT* pParseObj = new CONSTRUCT_KEDATOPIC_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( TopicsCreateAndDestroy,Get)
		{
			if (handle) delete ((CParseKedaUsersSingleT*)handle);
		}

		// keda - ���like + ȡ��like
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( LikeCreateAndDestroy,Get)
		{
// 			iOutCount = 1;
			CParseKedaLikeSingleT* pParseObj = new CONSTRUCT_KEDALIKE_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( LikeCreateAndDestroy,Get)
		{
			if (handle) delete ((CParseKedaLikeSingleT*)handle);
		}

		// keda - ��ȡ�ҵ�Ⱥ���б� + ������е�Ⱥ���б�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( Groups,Get)
		{
// 			iOutCount = 1;
			CParseKedaGroupsSingleT* pParseObj = new CONSTRUCT_KEDAGROUPS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( Groups,Get)
		{
			if (handle) delete ((CParseKedaGroupsSingleT*)handle);
		}

		// keda - ���Ⱥ����
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( GroupCreate,Get)
		{
// 			iOutCount = 1;
			CParseKedaGroupCreateSingleT* pParseObj = new CONSTRUCT_KEDAGROUPCREATE_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( GroupCreate,Get)
		{
			if (handle) delete ((CParseKedaGroupCreateSingleT*)handle);
		}

		// keda - �鿴�ҵ�˽���б�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( PrivateContacters,Get)
		{
// 			iOutCount = 1;
			CParseKedaPrivateContactersSingleT* pParseObj = new CONSTRUCT_KEDAPIRVATECONTACTERS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( PrivateContacters,Get)
		{
			if (handle) delete ((CParseKedaPrivateContactersSingleT*)handle);
		}

		// keda - �鿴����ĳ�˵�˽�������¼
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( PrivateFeeds,Get)
		{
// 			iOutCount = 1;
			CParseKedaPrivateFeedsSingleT* pParseObj = new CONSTRUCT_KEDAPIRVATEFEEDS_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( PrivateFeeds,Get)
		{
			if (handle) delete ((CParseKedaPrivateFeedsSingleT*)handle);
		}

		// keda - ����˽�ŵ�ĳ��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( PrivateFeed,Get)
		{
// 			iOutCount = 1;
			CParseKedaPrivateFeedSingleT* pParseObj = new CONSTRUCT_KEDAPIRVATEFEED_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( PrivateFeed,Get)
		{
			if (handle) delete ((CParseKedaPrivateFeedSingleT*)handle);
		}


//-----------------------------------  end  ------------------------------------------



//--------------------  2012-01-09 add by sunfei v0.1�ӿ� ----------------------------

		// keda - �����û�ID��ȡ�û����� 
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( UserInfo,Get)
		{
// 			iOutCount = 1;
			CParseKedaUserSingleT* pParseObj = new CONSTRUCT_KEDAUSER_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( UserInfo,Get)
		{
			if (handle) delete ((CParseKedaUserSingleT*)handle);
		}

		// keda - ���һ�����⼰���Ӧ�Ļظ�
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( SubjectAndReply,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( SubjectAndReply,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ����һ��΢��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( MessagePublish,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( MessagePublish,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - �ظ�һ��΢��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( MessageReply,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( MessageReply,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ����û�������΢�����ݣ�ȫ����
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( MessagePublishA,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( MessagePublishA,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ����û�������΢�����ݣ���Ѷ��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( MessagePublishF,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( MessagePublishF,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ��õ�ǰ��¼�û������ע�û���΢��
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( MeAndFriend,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( MeAndFriend,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

//-----------------------------------  end  ------------------------------------------

//--------------------  2012-01-09 add by sunfei v1.0�ӿ� ----------------------------

		// keda - ��ù�˾�㳡��΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( CompanyPublic,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( CompanyPublic,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ���Ⱥ���΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( GroupsData,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( GroupsData,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ���@��ǰ�û���΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( MentionsData,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( MentionsData,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ��ûظ��ҵ�΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( RepliesData,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( RepliesData,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ������޹���΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( LikesData,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( LikesData,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( WeiboMsg,Get)
        {
            CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( WeiboMsg,Get)
        {
            if (handle) delete ((CParseKedaMessagesSingleT*)handle);
        }

		// keda - ������ղص�΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( FavoritesData,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( FavoritesData,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}

		// keda - ���ĳ�������΢������
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS( TopicsData,Get)
		{
// 			iOutCount = 1;
			CParseKedaMessagesSingleT* pParseObj = new CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len);
			pParseObj->ParseBody(&iOutCount);
			ppout ? *ppout = pParseObj->get() : NULL;
			return ((WBPARSE_HANDLE)pParseObj);
		}
		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS( TopicsData,Get)
		{
			if (handle) delete ((CParseKedaMessagesSingleT*)handle);
		}


//-----------------------------------  end  ------------------------------------------


		/** ������� */
		WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_IMPLEMENTS(WEIBO_ERR,ITEM)
		{
			struct t_wbParse_Error *pError = (struct t_wbParse_Error *)malloc(sizeof(struct t_wbParse_Error));

			memset(pError->request, 0, WBPARSER_REAL_LEN(request));
			memset(pError->error, 0, WBPARSER_REAL_LEN(error));
			memset(pError->error_code, 0, WBPARSER_REAL_LEN(error_code));

			iOutCount = 0;

			int outlen = 0;
			wbParserNS::REQOBJ *pRoot = wbParserNS::Parse_data_XML( sc );

			long errorCode = wbParserNS::GetLONG_Key_XML("code", pRoot);
			const char *perrorDec = wbParserNS::GetCHAR_Key_XML_EX("error", pRoot, outlen);
			const char *prequest = wbParserNS::GetCHAR_Key_XML_EX("request", pRoot, outlen);
			const char *perrorCode = wbParserNS::GetCHAR_Key_XML_EX("code", pRoot, outlen);

			if ( prequest != NULL )
			{
				memcpy(pError->request, prequest, strlen(prequest));
			}
			if ( perrorDec != NULL )
			{
				memcpy(pError->error, perrorDec, strlen(perrorDec));
			}
			if ( perrorCode != NULL )
			{
				memcpy(pError->error_code, perrorCode, strlen(perrorCode));
			}

			pError->error_sub_code = errorCode;

			ppout ? *ppout = pError : NULL;

            if ( pRoot )
            {
                wbParserNS::Parse_free_XML(pRoot);
            }
			return (WBPARSE_HANDLE)pError;
		}


		WBPARSER_API void WBFREE_FUNC_IMPLEMENTS(WEIBO_ERR,ITEM)
		{
			struct t_wbParse_Error* pErr = (struct t_wbParse_Error*)handle;
			if( pErr){
				free(pErr);
			}
		}

		/** Cookie base */
		WBPARSER_API void WBPARSE_FUNC_IMPLEMENTS(Cookie,BASE) 
		{
			t_wbParse_Cookie* _cookie = ppout ? (t_wbParse_Cookie*)*ppout : NULL;
			std::string uid ,tgt , ticket , publickey , keyversion;
			if( !_cookie )
			{
				return ;
			}

			int     error = 0;
#ifdef _MSC_VER
			__int64 lservertime = 0;
#else
			long long lservertime = 0;
#endif

			if( !sc || len <= 0 )
			{
				error = -1;
			}
			else
			{

			}

			//
			if( _cookie && uid.size() && tgt.size() && ticket.size() )
			{// �ɹ���
				strncpy(_cookie->uid , uid.c_str() , 128);
				strncpy(_cookie->tgt , tgt.c_str() , 256 );
				strncpy(_cookie->publickey , publickey.c_str() , 512 );
				strncpy(_cookie->keyversion, keyversion.c_str(),64);
				strncpy(_cookie->ticket , ticket.c_str() , 256 );
				_cookie->lservertime = lservertime;
#ifdef WIN32
				_cookie->llocaltime  = GetTickCount();
#else
				struct timeval now;
				gettimeofday(&now, NULL);
				_cookie->llocaltime  = now.tv_sec * 1000 + now.tv_usec / 1000;
#endif
			}
			else
			{// ʧ�ܵ�
				_cookie->error = error;
			}
		}

		/** ����cookie,����ָ���Ĵ� */
		WBPARSER_API int Parse_CookieStr(const char *sc,char *token,char *secret)
		{
			return wb_parse_cookie(sc,token,secret);
		}

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MeetingList, Get )
        {
            CParseKedaMeetingListSingleT* pParseObj = new CONSTRUCT_MEETING_LIST_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( MeetingList, Get )
        {
            if (handle) delete ((CParseKedaMeetingListSingleT*)handle);
        }


        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MeetingInfo, Get )
        {
            CParseKedaMeetingInfoSingleT* pParseObj = new CONSTRUCT_MEETING_INFO_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( MeetingInfo, Get )
        {
            if (handle) delete ((CParseKedaMeetingInfoSingleT*)handle);
        }



        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( FreeRoomList, Get )
        {
            CParseKedaRoomListSingleT* pParseObj = new CONSTRUCT_ROOM_LIST_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( FreeRoomList, Get )
        {
            if (handle) delete ((CParseKedaRoomListSingleT*)handle);
        }


        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( NormalRoomList, Get )
        {
            CParseKedaNormalRoomListSingleT* pParseObj = new CONSTRUCT_PAGE_ROOMS_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( NormalRoomList, Get )
        {
            if (handle) delete ((CParseKedaNormalRoomListSingleT*)handle);
        }

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( RoomInfo, Get )
        {
            CParseKedaRoomInfoSingleT * pParseObj = new CONSTRUCT_ROOM_INFO_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( RoomInfo, Get )
        {
            if (handle) delete ((CParseKedaRoomInfoSingleT*)handle);
        }

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( RoomStatus, Get )
        {
            CParseKedaRoomStatusSingleT * pParseObj = new CONSTRUCT_ROOM_STATUS_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( RoomStatus, Get )
        {
            if (handle) delete ((CParseKedaRoomStatusSingleT*)handle);
        }


        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( RoomsByName, Get )
        {
            CParseKedaRoomsByNameSingleT * pParseObj = new CONSTRUCT_ROOMS_BY_NAME_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( RoomsByName, Get )
        {
            if (handle) delete ((CParseKedaRoomsByNameSingleT*)handle);
        }

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( TemplateList, Get )
        {
            CParseKedaTemplateListSingleT * pParseObj = new CONSTRUCT_COMMON_ITEMS_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( TemplateList, Get )
        {
            if (handle) delete ((CParseKedaTemplateListSingleT*)handle);
        }

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( TemplateInfo, Get )
        {
            CParseKedaTemplateInfoSingleT * pParseObj = new CONSTRUCT_TEMPLATE_INFO_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( TemplateInfo, Get )
        {
            if (handle) delete ((CParseKedaTemplateInfoSingleT*)handle);
        }


        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( ListNotifyByMax, Get )
        {
            CParseKedaMeetingNotifysSingleT * pParseObj = new CONSTRUCT_LIST_NOTIFY_BY_MAX_SINGLE(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( ListNotifyByMax, Get )
        {
            if (handle) delete ((CParseKedaMeetingNotifysSingleT*)handle);
        }

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GetNotifyById, Get )
        {
            CParseKedaMeetingGetNotifySingleT * pParseObj = new CONSTRUCT_GET_NOTIFY_BY_ID(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( GetNotifyById, Get )
        {
            if (handle) delete ((CParseKedaMeetingGetNotifySingleT*)handle);
        }

        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( GetRegular, Get )
        {
            CParseKedaMeetingGetRegularSingleT * pParseObj = new CONSTRUCT_GET_REGULAR(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( GetRegular, Get )
        {
            if (handle) delete ((CParseKedaMeetingGetRegularSingleT*)handle);
        }




        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( AllNotify,Get)
        {
            CParseAppGetAllNotifysSingleT * pParseObj = new CONSTRUCT_APP_GET_ALL_NOTIFYS(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( AllNotify,Get)
        {
            if (handle) delete ((CParseAppGetAllNotifysSingleT*)handle);
        }



        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MonitorGroup,Get)
        {
            CParseMonitorGroupSingleT * pParseObj = new CONSTRUCT_MONITOR_GROUP(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( MonitorGroup,Get)
        {
            if (handle) delete ((CParseMonitorGroupSingleT*)handle);
        }



        WBPARSER_API WBPARSE_HANDLE WBPARSE_FUNC_DECLARE( MonitorGroups,Get)
        {
            CParseMonitorGroupsSingleT * pParseObj = new CONSTRUCT_MONITOR_GROUPS(sc,len);
            pParseObj->ParseBody(&iOutCount);
            ppout ? *ppout = pParseObj->get() : NULL;
            return ((WBPARSE_HANDLE)pParseObj);
        }

        WBPARSER_API void WBFREE_FUNC_DECLARE( MonitorGroups,Get)
        {
            if (handle) delete ((CParseMonitorGroupsSingleT*)handle);
        }
	}



#ifdef __cplusplus
}
#endif //__cplusplus
