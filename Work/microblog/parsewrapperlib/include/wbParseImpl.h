#ifndef __WB_PARSEIMPL_H__
#define __WB_PARSEIMPL_H__

#include <assert.h>
#include <string>

#include "wbParser/wbParseWrapper.h"
#include "wbParser/wbParser.h"
#include "wbParser/wbParseMalloc.h"
#include "wbParser/wbParseObj.h"
#include "wbParseT.hpp"
#include "wbParseMacro.h"

#define WBParserImplNS CWBXmlParser


// 解析 <notifys> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Notifies >  CParseKedaNotifiesMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Notifies >   CParseKedaNotifiesSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Notifies > CParseKedaNotifiesListT;

#define CONSTRUCT_KEDANOTIFIES_MULTIPL(sc,len)\
	CParseKedaNotifiesMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Notifies,wbParserNS::wbParse_Free_Keda_Notifies,PARSE_T(parse_keda_notifies) )

#define CONSTRUCT_KEDANOTIFIES_SINGLE(sc,len)\
	CParseKedaNotifiesSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Notifies,wbParserNS::wbParse_Free_Keda_Notifies,PARSE_T(parse_keda_notifies) )

#define CONSTRUCT_KEDANOTIFIES_LIST(sc,len)\
	CParseKedaNotifiesListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Notifies,wbParserNS::wbParse_Free_Keda_Notifies,PARSE_T(parse_keda_notifies) )


//--------------------  2012-03-16 add by sunfei v1.0接口 ----------------------------

// 解析 <departments> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Departments >  CParseKedaDepartmentsMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Departments >   CParseKedaDepartmentsSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Departments > CParseKedaDepartmentsListT;

#define CONSTRUCT_KEDADEPARTMENTS_MULTIPL(sc,len)\
	CParseKedaDepartmentsMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Departments,wbParserNS::wbParse_Free_Keda_Departments,PARSE_T(parse_keda_departments) )

#define CONSTRUCT_KEDADEPARTMENTS_SINGLE(sc,len)\
	CParseKedaDepartmentsSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Departments,wbParserNS::wbParse_Free_Keda_Departments,PARSE_T(parse_keda_departments) )

#define CONSTRUCT_KEDADEPARTMENTS_LIST(sc,len)\
	CParseKedaDepartmentsListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Departments,wbParserNS::wbParse_Free_Keda_Departments,PARSE_T(parse_keda_departments) )



// 解析 <department> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Department >  CParseKedaDepartmentMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Department >   CParseKedaDepartmentSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Department > CParseKedaDepartmentListT;

#define CONSTRUCT_KEDADEPARTMENT_MULTIPL(sc,len)\
	CParseKedaDepartmentMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Department,wbParserNS::wbParse_Free_Keda_Department,PARSE_T(parse_keda_department) )

#define CONSTRUCT_KEDADEPARTMENT_SINGLE(sc,len)\
	CParseKedaDepartmentSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Department,wbParserNS::wbParse_Free_Keda_Department,PARSE_T(parse_keda_department) )

#define CONSTRUCT_KEDADEPARTMENT_LIST(sc,len)\
	CParseKedaDepartmentListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Department,wbParserNS::wbParse_Free_Keda_Department,PARSE_T(parse_keda_department) )



// 解析 <departments> 使用 -- 用于 /api/department/searchDepartment 返回的结构
typedef CParseGenericMultiplT< t_wbParse_Keda_searchDepartment >  CParseKedaSearchDepartmentMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_searchDepartment >   CParseKedaSearchDepartmentSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_searchDepartment > CParseKedaSearchDepartmentListT;

#define CONSTRUCT_KEDASEARCHDEPARTMENT_MULTIPL(sc,len)\
	CParseKedaSearchDepartmentMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_SearchDepartment,wbParserNS::wbParse_Free_Keda_SearchDepartment,PARSE_T(parse_keda_searchDepartment) )

#define CONSTRUCT_KEDASEARCHDEPARTMENT_SINGLE(sc,len)\
	CParseKedaSearchDepartmentSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_SearchDepartment,wbParserNS::wbParse_Free_Keda_SearchDepartment,PARSE_T(parse_keda_searchDepartment) )

#define CONSTRUCT_KEDASEARCHDEPARTMENT_LIST(sc,len)\
	CParseKedaSearchDepartmentListT(sc,len,wbParserNS::wbParse_Malloc_Keda_SearchDepartment,wbParserNS::wbParse_Free_Keda_SearchDepartment,PARSE_T(parse_keda_searchDepartment) )



// 解析 <users> 使用 -- 用于 /api/department/searchUser 返回的结构
typedef CParseGenericMultiplT< t_wbParse_Keda_searchUser >  CParseKedaSearchUserMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_searchUser >   CParseKedaSearchUserSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_searchUser > CParseKedaSearchUserListT;

#define CONSTRUCT_KEDASEARCHUSER_MULTIPL(sc,len)\
	CParseKedaSearchUserMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_SearchUser,wbParserNS::wbParse_Free_Keda_SearchUser,PARSE_T(parse_keda_searchUser) )

#define CONSTRUCT_KEDASEARCHUSER_SINGLE(sc,len)\
	CParseKedaSearchUserSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_SearchUser,wbParserNS::wbParse_Free_Keda_SearchUser,PARSE_T(parse_keda_searchUser) )

#define CONSTRUCT_KEDASEARCHUSER_LIST(sc,len)\
	CParseKedaSearchUserListT(sc,len,wbParserNS::wbParse_Malloc_Keda_SearchUser,wbParserNS::wbParse_Free_Keda_SearchUser,PARSE_T(parse_keda_searchUser) )





// 解析 <page> 使用 -- 获取赞过我的微博数据API
typedef CParseGenericMultiplT< t_wbParse_Keda_Like_Page >  CParseKedaLikePageMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Like_Page >   CParseKedaLikePageSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Like_Page > CParseKedaLikePageListT;

#define CONSTRUCT_KEDALIKEPAGE_MULTIPL(sc,len)\
	CParseKedaLikePageMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_LikePage,wbParserNS::wbParse_Free_Keda_LikePage,PARSE_T(parse_keda_likePage) )

#define CONSTRUCT_KEDALIKEPAGE_SINGLE(sc,len)\
	CParseKedaLikePageSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_LikePage,wbParserNS::wbParse_Free_Keda_LikePage,PARSE_T(parse_keda_likePage) )

#define CONSTRUCT_KEDALIKEPAGE_LIST(sc,len)\
	CParseKedaLikePageListT(sc,len,wbParserNS::wbParse_Malloc_Keda_LikePage,wbParserNS::wbParse_Free_Keda_LikePage,PARSE_T(parse_keda_likePage) )




//--------------------  2012-03-07 add by sunfei v1.0接口 ----------------------------

// 解析 <result> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Result >  CParseKedaResultMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Result >   CParseKedaResultSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Result > CParseKedaResultListT;

#define CONSTRUCT_KEDARESULT_MULTIPL(sc,len)\
	CParseKedaResultMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Result,wbParserNS::wbParse_Free_Keda_Result,PARSE_T(parse_keda_result) )

#define CONSTRUCT_KEDARESULT_SINGLE(sc,len)\
	CParseKedaResultSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Result,wbParserNS::wbParse_Free_Keda_Result,PARSE_T(parse_keda_result) )

#define CONSTRUCT_KEDARESULT_LIST(sc,len)\
	CParseKedaResultListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Result,wbParserNS::wbParse_Free_Keda_Result,PARSE_T(parse_keda_result) )


// 解析 <entUser> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_EntUser >  CParseKedaEntUserMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_EntUser >   CParseKedaEntUserSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_EntUser > CParseKedaEntUserListT;

#define CONSTRUCT_KEDAENTUSER_MULTIPL(sc,len)\
	CParseKedaEntUserMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_EntUser,wbParserNS::wbParse_Free_Keda_EntUser,PARSE_T(parse_keda_entUser) )

#define CONSTRUCT_KEDAENTUSER_SINGLE(sc,len)\
	CParseKedaEntUserSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_EntUser,wbParserNS::wbParse_Free_Keda_EntUser,PARSE_T(parse_keda_entUser) )

#define CONSTRUCT_KEDAENTUSER_LIST(sc,len)\
	CParseKedaEntUserListT(sc,len,wbParserNS::wbParse_Malloc_Keda_EntUser,wbParserNS::wbParse_Free_Keda_EntUser,PARSE_T(parse_keda_entUser) )



// 解析 <entUsers> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_EntUsers >  CParseKedaEntUsersMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_EntUsers >   CParseKedaEntUsersSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_EntUsers > CParseKedaEntUsersListT;

#define CONSTRUCT_KEDAENTUSERS_MULTIPL(sc,len)\
	CParseKedaEntUsersMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_EntUsers,wbParserNS::wbParse_Free_Keda_EntUsers,PARSE_T(parse_keda_entUsers) )

#define CONSTRUCT_KEDAENTUSERS_SINGLE(sc,len)\
	CParseKedaEntUsersSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_EntUsers,wbParserNS::wbParse_Free_Keda_EntUsers,PARSE_T(parse_keda_entUsers) )

#define CONSTRUCT_KEDAENTUSERS_LIST(sc,len)\
	CParseKedaEntUsersListT(sc,len,wbParserNS::wbParse_Malloc_Keda_EntUsers,wbParserNS::wbParse_Free_Keda_EntUsers,PARSE_T(parse_keda_entUsers) )


// 解析 <page> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Page >  CParseKedaPageMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Page >   CParseKedaPageSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Page > CParseKedaPageListT;

#define CONSTRUCT_KEDAPAGE_MULTIPL(sc,len)\
	CParseKedaPageMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Page,wbParserNS::wbParse_Free_Keda_Page,PARSE_T(parse_keda_page) )

#define CONSTRUCT_KEDAPAGE_SINGLE(sc,len)\
	CParseKedaPageSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Page,wbParserNS::wbParse_Free_Keda_Page,PARSE_T(parse_keda_page) )

#define CONSTRUCT_KEDAPAGE_LIST(sc,len)\
	CParseKedaPageListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Page,wbParserNS::wbParse_Free_Keda_Page,PARSE_T(parse_keda_page) )



//--------------------  2012-02-29 add by sunfei v1.0接口 ----------------------------

// 解析 <users> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Users >  CParseKedaUsersMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Users >   CParseKedaUsersSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Users > CParseKedaUsersListT;

#define CONSTRUCT_KEDAUSERS_MULTIPL(sc,len)\
	CParseKedaUsersMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Users,wbParserNS::wbParse_Free_Keda_Users,PARSE_T(parse_keda_users) )

#define CONSTRUCT_KEDAUSERS_SINGLE(sc,len)\
	CParseKedaUsersSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Users,wbParserNS::wbParse_Free_Keda_Users,PARSE_T(parse_keda_users) )

#define CONSTRUCT_KEDAUSERS_LIST(sc,len)\
	CParseKedaUsersListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Users,wbParserNS::wbParse_Free_Keda_Users,PARSE_T(parse_keda_users) )


// 解析 <topic> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Topic >  CParseKedaTopicMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Topic >   CParseKedaTopicSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Topic > CParseKedaTopicListT;

#define CONSTRUCT_KEDATOPIC_MULTIPL(sc,len)\
	CParseKedaTopicMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Topic,wbParserNS::wbParse_Free_Keda_Topic,PARSE_T(parse_keda_topic) )

#define CONSTRUCT_KEDATOPIC_SINGLE(sc,len)\
	CParseKedaTopicSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Topic,wbParserNS::wbParse_Free_Keda_Topic,PARSE_T(parse_keda_topic) )

#define CONSTRUCT_KEDATOPIC_LIST(sc,len)\
	CParseKedaTopicListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Topic,wbParserNS::wbParse_Free_Keda_Topic,PARSE_T(parse_keda_topic) )



// 解析 <like> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Like >  CParseKedaLikeMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Like >   CParseKedaLikeSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Like > CParseKedaLikeListT;

#define CONSTRUCT_KEDALIKE_MULTIPL(sc,len)\
	CParseKedaLikeMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Like,wbParserNS::wbParse_Free_Keda_Like,PARSE_T(parse_keda_like) )

#define CONSTRUCT_KEDALIKE_SINGLE(sc,len)\
	CParseKedaLikeSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Like,wbParserNS::wbParse_Free_Keda_Like,PARSE_T(parse_keda_like) )

#define CONSTRUCT_KEDALIKE_LIST(sc,len)\
	CParseKedaLikeListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Like,wbParserNS::wbParse_Free_Keda_Like,PARSE_T(parse_keda_like) )


// 解析 <groups> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Groups >  CParseKedaGroupsMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Groups >   CParseKedaGroupsSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Groups > CParseKedaGroupsListT;

#define CONSTRUCT_KEDAGROUPS_MULTIPL(sc,len)\
	CParseKedaGroupsMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Groups,wbParserNS::wbParse_Free_Keda_Groups,PARSE_T(parse_keda_groups) )

#define CONSTRUCT_KEDAGROUPS_SINGLE(sc,len)\
	CParseKedaGroupsSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Groups,wbParserNS::wbParse_Free_Keda_Groups,PARSE_T(parse_keda_groups) )

#define CONSTRUCT_KEDAGROUPS_LIST(sc,len)\
	CParseKedaGroupsListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Groups,wbParserNS::wbParse_Free_Keda_Groups,PARSE_T(parse_keda_groups) )


// 解析 <group> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_GroupCreate >  CParseKedaGroupCreateMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_GroupCreate >   CParseKedaGroupCreateSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_GroupCreate > CParseKedaGroupCreateListT;

#define CONSTRUCT_KEDAGROUPCREATE_MULTIPL(sc,len)\
	CParseKedaGroupCreateMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_GroupCreate,wbParserNS::wbParse_Free_Keda_GroupCreate,PARSE_T(parse_keda_groupcreate) )

#define CONSTRUCT_KEDAGROUPCREATE_SINGLE(sc,len)\
	CParseKedaGroupCreateSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_GroupCreate,wbParserNS::wbParse_Free_Keda_GroupCreate,PARSE_T(parse_keda_groupcreate) )

#define CONSTRUCT_KEDAGROUPCREATE_LIST(sc,len)\
	CParseKedaGroupCreateListT(sc,len,wbParserNS::wbParse_Malloc_Keda_GroupCreate,wbParserNS::wbParse_Free_Keda_GroupCreate,PARSE_T(parse_keda_groupcreate) )



// 解析 <privateContacters> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_PrivateContacters >  CParseKedaPrivateContactersMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_PrivateContacters >   CParseKedaPrivateContactersSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_PrivateContacters > CParseKedaPrivateContactersListT;

#define CONSTRUCT_KEDAPIRVATECONTACTERS_MULTIPL(sc,len)\
	CParseKedaPrivateContactersMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_PrivateContacters,wbParserNS::wbParse_Free_Keda_PrivateContacters,PARSE_T(parse_keda_privateContacters) )

#define CONSTRUCT_KEDAPIRVATECONTACTERS_SINGLE(sc,len)\
	CParseKedaPrivateContactersSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_PrivateContacters,wbParserNS::wbParse_Free_Keda_PrivateContacters,PARSE_T(parse_keda_privateContacters) )

#define CONSTRUCT_KEDAPIRVATECONTACTERS_LIST(sc,len)\
	CParseKedaPrivateContactersListT(sc,len,wbParserNS::wbParse_Malloc_Keda_PrivateContacters,wbParserNS::wbParse_Free_Keda_PrivateContacters,PARSE_T(parse_keda_privateContacters) )



// 解析 <privateFeeds> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_PrivateFeeds >  CParseKedaPrivateFeedsMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_PrivateFeeds >   CParseKedaPrivateFeedsSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_PrivateFeeds > CParseKedaPrivateFeedsListT;

#define CONSTRUCT_KEDAPIRVATEFEEDS_MULTIPL(sc,len)\
	CParseKedaPrivateFeedsMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_PrivateFeeds,wbParserNS::wbParse_Free_Keda_PrivateFeeds,PARSE_T(parse_keda_privateFeeds) )

#define CONSTRUCT_KEDAPIRVATEFEEDS_SINGLE(sc,len)\
	CParseKedaPrivateFeedsSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_PrivateFeeds,wbParserNS::wbParse_Free_Keda_PrivateFeeds,PARSE_T(parse_keda_privateFeeds) )

#define CONSTRUCT_KEDAPIRVATEFEEDS_LIST(sc,len)\
	CParseKedaPrivateFeedsListT(sc,len,wbParserNS::wbParse_Malloc_Keda_PrivateFeeds,wbParserNS::wbParse_Free_Keda_PrivateFeeds,PARSE_T(parse_keda_privateFeeds) )



// 解析 <privateFeed> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_PrivateFeed >  CParseKedaPrivateFeedMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_PrivateFeed >   CParseKedaPrivateFeedSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_PrivateFeed > CParseKedaPrivateFeedListT;

#define CONSTRUCT_KEDAPIRVATEFEED_MULTIPL(sc,len)\
	CParseKedaPrivateFeedMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_PrivateFeed,wbParserNS::wbParse_Free_Keda_PrivateFeed,PARSE_T(parse_keda_privateFeed) )

#define CONSTRUCT_KEDAPIRVATEFEED_SINGLE(sc,len)\
	CParseKedaPrivateFeedSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_PrivateFeed,wbParserNS::wbParse_Free_Keda_PrivateFeed,PARSE_T(parse_keda_privateFeed) )

#define CONSTRUCT_KEDAPIRVATEFEED_LIST(sc,len)\
	CParseKedaPrivateFeedListT(sc,len,wbParserNS::wbParse_Malloc_Keda_PrivateFeed,wbParserNS::wbParse_Free_Keda_PrivateFeed,PARSE_T(parse_keda_privateFeed) )



//-----------------------------------  end  ------------------------------------------


//--------------------  2012-01-09 add by sunfei v0.1接口 ----------------------------

// 解析 <user> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_User >  CParseKedaUserMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_User >   CParseKedaUserSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_User > CParseKedaUserListT;

#define CONSTRUCT_KEDAUSER_MULTIPL(sc,len)\
	CParseKedaUserMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_User,wbParserNS::wbParse_Free_Keda_User,PARSE_T(parse_keda_user) )

#define CONSTRUCT_KEDAUSER_SINGLE(sc,len)\
	CParseKedaUserSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_User,wbParserNS::wbParse_Free_Keda_User,PARSE_T(parse_keda_user) )

#define CONSTRUCT_KEDAUSER_LIST(sc,len)\
	CParseKedaUserListT(sc,len,wbParserNS::wbParse_Malloc_Keda_User,wbParserNS::wbParse_Free_Keda_User,PARSE_T(parse_keda_user) )


// 解析 <meta> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Meta >  CParseKedaMetaMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Meta >   CParseKedaMetaSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Meta > CParseKedaMetaListT;

#define CONSTRUCT_KEDAMETA_MULTIPL(sc,len)\
	CParseKedaMetaMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Meta,wbParserNS::wbParse_Free_Keda_Meta,PARSE_T(parse_keda_meta) )

#define CONSTRUCT_KEDAMETA_SINGLE(sc,len)\
	CParseKedaMetaSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Meta,wbParserNS::wbParse_Free_Keda_Meta,PARSE_T(parse_keda_meta) )

#define CONSTRUCT_KEDAMETA_LIST(sc,len)\
	CParseKedaMetaListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Meta,wbParserNS::wbParse_Free_Keda_Meta,PARSE_T(parse_keda_meta) )


// 解析 <relatedData> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_RelatedData >  CParseKedaRelatedDataMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_RelatedData >   CParseKedaRelatedDataSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_RelatedData > CParseKedaRelatedDataListT;

#define CONSTRUCT_KEDARD_MULTIPL(sc,len)\
	CParseKedaRelatedDataMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_RelatedData,wbParserNS::wbParse_Free_Keda_RelatedData,PARSE_T(parse_keda_relatedData) )

#define CONSTRUCT_KEDARD_SINGLE(sc,len)\
	CParseKedaRelatedDataSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_RelatedData,wbParserNS::wbParse_Free_Keda_RelatedData,PARSE_T(parse_keda_relatedData) )

#define CONSTRUCT_KEDARD_LIST(sc,len)\
	CParseKedaRelatedDataListT(sc,len,wbParserNS::wbParse_Malloc_Keda_RelatedData,wbParserNS::wbParse_Free_Keda_RelatedData,PARSE_T(parse_keda_relatedData) )


// 解析 <feedMessage> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_FeedMessage >  CParseKedaFeedMessageMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_FeedMessage >   CParseKedaFeedMessageSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_FeedMessage > CParseKedaFeedMessageListT;

#define CONSTRUCT_KEDAFM_MULTIPL(sc,len)\
	CParseKedaFeedMessageMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_FeedMessage,wbParserNS::wbParse_Free_Keda_FeedMessage,PARSE_T(parse_keda_feedMessage) )

#define CONSTRUCT_KEDAFM_SINGLE(sc,len)\
	CParseKedaFeedMessageSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_FeedMessage,wbParserNS::wbParse_Free_Keda_FeedMessage,PARSE_T(parse_keda_feedMessage) )

#define CONSTRUCT_KEDAFM_LIST(sc,len)\
	CParseKedaFeedMessageListT(sc,len,wbParserNS::wbParse_Malloc_Keda_FeedMessage,wbParserNS::wbParse_Free_Keda_FeedMessage,PARSE_T(parse_keda_feedMessage) )


// 解析 <messages> 使用
typedef CParseGenericMultiplT< t_wbParse_Keda_Messages >  CParseKedaMessagesMultiplT;
typedef CParseGenericSingleT< t_wbParse_Keda_Messages >   CParseKedaMessagesSingleT;
typedef CParseGenericByCursorT< t_wbParse_Keda_Messages > CParseKedaMessagesListT;

#define CONSTRUCT_KEDAMESSAGES_MULTIPL(sc,len)\
	CParseKedaMessagesMultiplT(sc,len,wbParserNS::wbParse_Malloc_Keda_Messages,wbParserNS::wbParse_Free_Keda_Messages,PARSE_T(parse_keda_messages) )

#define CONSTRUCT_KEDAMESSAGES_SINGLE(sc,len)\
	CParseKedaMessagesSingleT(sc,len,wbParserNS::wbParse_Malloc_Keda_Messages,wbParserNS::wbParse_Free_Keda_Messages,PARSE_T(parse_keda_messages) )

#define CONSTRUCT_KEDAMESSAGES_LIST(sc,len)\
	CParseKedaMessagesListT(sc,len,wbParserNS::wbParse_Malloc_Keda_Messages,wbParserNS::wbParse_Free_Keda_Messages,PARSE_T(parse_keda_messages) )

//-----------------------------------  end  ------------------------------------------



// 解析 <messages> 使用
typedef CParseGenericSingleT< t_wbParse_m_meeting_list >   CParseKedaMeetingListSingleT;


#define CONSTRUCT_MEETING_LIST_SINGLE(sc,len)\
    CParseKedaMeetingListSingleT(sc,len,wbParserNS::wbParse_Malloc_MeetingList,wbParserNS::wbParse_Free_MeetingList,PARSE_T(parse_keda_meeting_list) )
   




typedef CParseGenericSingleT< t_wbParse_m_meeting_info >   CParseKedaMeetingInfoSingleT;

#define CONSTRUCT_MEETING_INFO_SINGLE(sc,len)\
    CParseKedaMeetingInfoSingleT(sc,len,wbParserNS::wbParse_Malloc_MeetingInfo,wbParserNS::wbParse_Free_MeetingInfo,PARSE_T(parse_keda_meeting_info) )




typedef CParseGenericSingleT< t_wbParse_m_room_list >   CParseKedaRoomListSingleT;

#define CONSTRUCT_ROOM_LIST_SINGLE(sc,len)\
    CParseKedaRoomListSingleT(sc,len,wbParserNS::wbParse_Malloc_Meeting_RoomList,wbParserNS::wbParse_Free_Meeting_RoomList,PARSE_T(parse_keda_m_room_list) )


typedef CParseGenericSingleT< t_wbParse_m_normal_room_list >   CParseKedaNormalRoomListSingleT;

#define CONSTRUCT_PAGE_ROOMS_SINGLE(sc,len)\
    CParseKedaNormalRoomListSingleT(sc,len,wbParserNS::wbParse_Malloc_Meeting_NormalRoomList,wbParserNS::wbParse_Free_Meeting_NormalRoomList,PARSE_T(parse_keda_m_normal_room_list) )


typedef CParseGenericSingleT< t_wbParse_m_room_info >   CParseKedaRoomInfoSingleT;

#define CONSTRUCT_ROOM_INFO_SINGLE(sc,len)\
    CParseKedaRoomInfoSingleT(sc,len,wbParserNS::wbParse_Malloc_RoomInfo,wbParserNS::wbParse_Free_RoomInfo, PARSE_T(parse_keda_room_info) )


typedef CParseGenericSingleT< t_wbParse_m_room_status >   CParseKedaRoomStatusSingleT;

#define CONSTRUCT_ROOM_STATUS_SINGLE(sc,len)\
    CParseKedaRoomStatusSingleT(sc,len,wbParserNS::wbParse_Malloc_RoomStatus,wbParserNS::wbParse_Free_RoomStatus, PARSE_T(parse_keda_room_status) )




typedef CParseGenericSingleT< t_wbParse_m_short_rooms >   CParseKedaRoomsByNameSingleT;

#define CONSTRUCT_ROOMS_BY_NAME_SINGLE(sc,len)\
    CParseKedaRoomsByNameSingleT(sc,len,wbParserNS::wbParse_Malloc_Meeting_RoomList,wbParserNS::wbParse_Free_Meeting_RoomList,PARSE_T(parse_keda_room_by_name) )






typedef CParseGenericSingleT< t_wbParse_m_templateList >   CParseKedaTemplateListSingleT;

#define CONSTRUCT_COMMON_ITEMS_SINGLE(sc,len)\
    CParseKedaTemplateListSingleT(sc,len,wbParserNS::wbParse_Malloc_Meeting_TemplateList,wbParserNS::wbParse_Free_Meeting_TemplateList,PARSE_T(parse_keda_template_list) )




typedef CParseGenericSingleT< t_wbParse_m_template_info >   CParseKedaTemplateInfoSingleT;

#define CONSTRUCT_TEMPLATE_INFO_SINGLE(sc,len)\
    CParseKedaTemplateInfoSingleT(sc,len,wbParserNS::wbParse_Malloc_Meeting_TemplateInfo,wbParserNS::wbParse_Free_Meeting_TemplateInfo,PARSE_T(parse_keda_template_info) )





typedef CParseGenericSingleT< t_wbParse_m_notifys >   CParseKedaMeetingNotifysSingleT;

#define CONSTRUCT_LIST_NOTIFY_BY_MAX_SINGLE(sc,len)\
    CParseKedaMeetingNotifysSingleT(sc,len,wbParserNS::wbParse_Malloc_Meeting_ListNotifyByMax,wbParserNS::wbParse_Free_Meeting_ListNotifyByMax,PARSE_T(parse_keda_list_notify_by_max) )


typedef CParseGenericSingleT< t_wbParse_m_notify >   CParseKedaMeetingGetNotifySingleT;

#define CONSTRUCT_GET_NOTIFY_BY_ID(sc,len)\
    CParseKedaMeetingGetNotifySingleT(sc,len,wbParserNS::wbParse_Malloc_Meeting_Notify,wbParserNS::wbParse_Free_Meeting_Notify,PARSE_T(parse_keda_notify_item) )




typedef CParseGenericSingleT< t_wbParse_m_regular_info >   CParseKedaMeetingGetRegularSingleT;

#define CONSTRUCT_GET_REGULAR(sc,len)\
    CParseKedaMeetingGetRegularSingleT(sc,len,wbParserNS::wbParse_Malloc_Meeting_Regular,wbParserNS::wbParse_Free_Meeting_Regular,PARSE_T(parse_keda_regular) )





typedef CParseGenericSingleT< t_wbParse_app_notifys >   CParseAppGetAllNotifysSingleT;

#define CONSTRUCT_APP_GET_ALL_NOTIFYS(sc,len)\
    CParseAppGetAllNotifysSingleT(sc,len,wbParserNS::wbParse_Malloc_App_GetAllNotifys,wbParserNS::wbParse_Free_App_GetAllNotifys,PARSE_T(parse_app_all_notifys) )




typedef CParseGenericSingleT< t_wbParse_monitor_group >   CParseMonitorGroupSingleT;

#define CONSTRUCT_MONITOR_GROUP(sc,len)\
    CParseMonitorGroupSingleT(sc,len,wbParserNS::wbParse_Malloc_Monitor_Group,wbParserNS::wbParse_Free_Monitor_Group,PARSE_T(parse_monitor_group) )



typedef CParseGenericSingleT< t_wbParse_monitor_groups >   CParseMonitorGroupsSingleT;

#define CONSTRUCT_MONITOR_GROUPS(sc,len)\
    CParseMonitorGroupsSingleT(sc,len,wbParserNS::wbParse_Malloc_Monitor_Groups,wbParserNS::wbParse_Free_Monitor_Groups,PARSE_T(parse_monitor_groups) )




#endif // __WB_PARSEIMPL_H__


