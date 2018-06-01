#ifndef __WB_PARSE_MALLOC_H__
#define __WB_PARSE_MALLOC_H__


#if defined(WIN32) || defined(WINCE)
#	define EXTERN extern
#else
#	define EXTERN 
#endif

namespace wbParserNS
{
	// cursor的接口Android需要使用
	EXTERN void *wbParse_Malloc_Cursor(int size);
	EXTERN void  wbParse_Free_Cursor(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Notify(int size);
	EXTERN void wbParse_Free_Keda_Notify(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Notifies(int size);
	EXTERN void wbParse_Free_Keda_Notifies(void *pvoid,int size);


//--------------------  2012-03-16 add by sunfei v1.0接口 ----------------------------


	EXTERN void *wbParse_Malloc_Keda_DepartmentUsers(int size);
	EXTERN void wbParse_Free_Keda_DepartmentUsers(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Department(int size);
	EXTERN void wbParse_Free_Keda_Department(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Departments(int size);
	EXTERN void wbParse_Free_Keda_Departments(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_SearchDepartment(int size);
	EXTERN void wbParse_Free_Keda_SearchDepartment(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_SearchUser(int size);
	EXTERN void wbParse_Free_Keda_SearchUser(void *pvoid,int size);


//--------------------  2012-09-18 add by sunfei v2.5.1接口 ----------------------------

	EXTERN void *wbParse_Malloc_Keda_Messages_Data(int size);
	EXTERN void wbParse_Free_Keda_Messages_Data(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_LikePage(int size);
	EXTERN void wbParse_Free_Keda_LikePage(void *pvoid,int size);


//--------------------  2012-03-07 add by sunfei v1.0接口 ----------------------------


	EXTERN void *wbParse_Malloc_Keda_EntUser(int size);
	EXTERN void wbParse_Free_Keda_EntUser(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_EntConfig(int size);
	EXTERN void wbParse_Free_Keda_EntConfig(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Result(int size);
	EXTERN void wbParse_Free_Keda_Result(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_EntUsers(int size);
	EXTERN void wbParse_Free_Keda_EntUsers(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Page(int size);
	EXTERN void wbParse_Free_Keda_Page(void *pvoid,int size);


//--------------------  2012-02-29 add by sunfei v1.0接口 ----------------------------

	EXTERN void *wbParse_Malloc_Keda_Users_Data(int size);
	EXTERN void wbParse_Free_Keda_Users_Data(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Users(int size);
	EXTERN void wbParse_Free_Keda_Users(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Topic(int size);
	EXTERN void wbParse_Free_Keda_Topic(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Like(int size);
	EXTERN void wbParse_Free_Keda_Like(void *pvoid,int size);


	
	EXTERN void *wbParse_Malloc_Keda_GroupStatistics(int size);
	EXTERN void wbParse_Free_Keda_GroupStatistics(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Group(int size);
	EXTERN void wbParse_Free_Keda_Group(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Groups_Data(int size);
	EXTERN void wbParse_Free_Keda_Groups_Data(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Groups(int size);
	EXTERN void wbParse_Free_Keda_Groups(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_GroupCreate(int size);
	EXTERN void wbParse_Free_Keda_GroupCreate(void *pvoid,int size);


	// 2012-03-02 add by sunfei 为私信添加

	EXTERN void *wbParse_Malloc_Keda_Sender(int size);
	EXTERN void wbParse_Free_Keda_Sender(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Receiver(int size);
	EXTERN void wbParse_Free_Keda_Receiver(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_PrivateFeed(int size);
	EXTERN void wbParse_Free_Keda_PrivateFeed(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_PrivateContacter(int size);
	EXTERN void wbParse_Free_Keda_PrivateContacter(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_PrivCont_Data(int size);
	EXTERN void wbParse_Free_Keda_PrivCont_Data(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_PrivateContacters(int size);
	EXTERN void wbParse_Free_Keda_PrivateContacters(void *pvoid,int size);



	EXTERN void *wbParse_Malloc_Keda_PrivFeeds_Data(int size);
	EXTERN void wbParse_Free_Keda_PrivFeeds_Data(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_PrivateFeeds(int size);
	EXTERN void wbParse_Free_Keda_PrivateFeeds(void *pvoid,int size);


	// add end

//-----------------------------------  end  ------------------------------------------


//--------------------  2012-01-09 add by sunfei v0.1接口 ----------------------------

	EXTERN void *wbParse_Malloc_Keda_UserProfile(int size);
	EXTERN void  wbParse_Free_Keda_UserProfile(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_OnlineState(int size);
	EXTERN void  wbParse_Free_Keda_OnlineState(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_Dept(int size);
	EXTERN void  wbParse_Free_Keda_Dept(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_Depts(int size);
	EXTERN void  wbParse_Free_Keda_Depts(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_User(int size);
	EXTERN void  wbParse_Free_Keda_User(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_Meta(int size);
	EXTERN void  wbParse_Free_Keda_Meta(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_RelatedData(int size);
	EXTERN void  wbParse_Free_Keda_RelatedData(void *p,int size);

//-----------------------------------  end  ------------------------------------------

	// 2012-02-06 add by sunfei

	EXTERN void *wbParse_Malloc_Keda_Id(int size);
	EXTERN void wbParse_Free_Keda_Id(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_ReferencedUserIds(int size);
	EXTERN void wbParse_Free_Keda_ReferencedUserIds(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_TopicIds(int size);
	EXTERN void wbParse_Free_Keda_TopicIds(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_LikeByIds(int size);
	EXTERN void wbParse_Free_Keda_LikeByIds(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Picture(int size);
	EXTERN void wbParse_Free_Keda_Picture(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Pictures(int size);
	EXTERN void wbParse_Free_Keda_Pictures(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Attachment(int size);
	EXTERN void wbParse_Free_Keda_Attachment(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_Attachments(int size);
	EXTERN void wbParse_Free_Keda_Attachments(void *pvoid,int size);

	// add end


	EXTERN void *wbParse_Malloc_Keda_FeedMessage(int size);
	EXTERN void  wbParse_Free_Keda_FeedMessage(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_Subject(int size);
	EXTERN void  wbParse_Free_Keda_Subject(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_RefReply(int size);
	EXTERN void  wbParse_Free_Keda_RefReply(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_Reply(int size);
	EXTERN void  wbParse_Free_Keda_Reply(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_Replies(int size);
	EXTERN void  wbParse_Free_Keda_Replies(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_SubjectStyleThread(int size);
	EXTERN void  wbParse_Free_Keda_SubjectStyleThread(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_ReplyStyleThread(int size);
	EXTERN void  wbParse_Free_Keda_ReplyStyleThread(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_ThreadMessage(int size);
	EXTERN void  wbParse_Free_Keda_ThreadMessage(void *p,int size);

	EXTERN void *wbParse_Malloc_Keda_Messages(int size);
	EXTERN void  wbParse_Free_Keda_Messages(void *p,int size);



	EXTERN void *wbParse_Malloc_Keda_UserData(int size);
	EXTERN void wbParse_Free_Keda_UserData(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_UserEntry(int size);
	EXTERN void wbParse_Free_Keda_UserEntry(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_RelatedUsers(int size);
	EXTERN void wbParse_Free_Keda_RelatedUsers(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_GroupData(int size);
	EXTERN void wbParse_Free_Keda_GroupData(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_GroupEntry(int size);
	EXTERN void wbParse_Free_Keda_GroupEntry(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_RelatedGroups(int size);
	EXTERN void wbParse_Free_Keda_RelatedGroups(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_TopicData(int size);
	EXTERN void wbParse_Free_Keda_TopicData(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_TopicEntry(int size);
	EXTERN void wbParse_Free_Keda_TopicEntry(void *pvoid,int size);

	EXTERN void *wbParse_Malloc_Keda_RelatedTopics(int size);
	EXTERN void wbParse_Free_Keda_RelatedTopics(void *pvoid,int size);

	// add end
// ----------------------------------------

	// 错误信息
	EXTERN void *wbParse_Malloc_Error(int size);
	EXTERN void  wbParse_Free_Error(void* p,int size);




    EXTERN void *wbParse_Malloc_MeetingList(int size);
    EXTERN void  wbParse_Free_MeetingList(void *p,int size);

    EXTERN void *wbParse_Malloc_MeetingInfo(int size);
    EXTERN void  wbParse_Free_MeetingInfo(void *pvoid,int size);



    EXTERN void *wbParse_Malloc_Meeting_RoomList(int size);
    EXTERN void wbParse_Free_Meeting_RoomList(void *pvoid,int size);

    EXTERN void *wbParse_Malloc_Meeting_NormalRoomList(int size);
    EXTERN void wbParse_Free_Meeting_NormalRoomList(void *pvoid,int size);

    EXTERN void *wbParse_Malloc_RoomInfo(int size);
    EXTERN void  wbParse_Free_RoomInfo(void *pvoid,int size);

    EXTERN void *wbParse_Malloc_RoomStatus(int size);
    EXTERN void  wbParse_Free_RoomStatus(void *pvoid,int size);

    //EXTERN void *wbParse_Malloc_Meeting_CommonItems(int size);
    //EXTERN void wbParse_Free_Meeting_CommonItems(void *pvoid,int size);

    EXTERN void *wbParse_Malloc_Meeting_TemplateList(int size);
    EXTERN void wbParse_Free_Meeting_TemplateList(void *pvoid,int size);

    EXTERN void *wbParse_Malloc_Meeting_TemplateInfo(int size);
    EXTERN void wbParse_Free_Meeting_TemplateInfo(void *pvoid,int size);


    EXTERN void *wbParse_Malloc_Meeting_ListNotifyByMax(int size);
    EXTERN void wbParse_Free_Meeting_ListNotifyByMax(void *pvoid,int size);

    EXTERN void *wbParse_Malloc_Meeting_Notify(int size);
    EXTERN void wbParse_Free_Meeting_Notify(void *pvoid,int size);

    EXTERN void *wbParse_Malloc_Meeting_Regular(int size);
    EXTERN void wbParse_Free_Meeting_Regular(void *pvoid,int size);


    EXTERN void *wbParse_Malloc_App_GetAllNotifys(int size);
    EXTERN void wbParse_Free_App_GetAllNotifys(void *pvoid,int size);

    EXTERN void *wbParse_Malloc_Monitor_Group(int size);
    EXTERN void wbParse_Free_Monitor_Group(void *pvoid,int size);

    EXTERN void *wbParse_Malloc_Monitor_Groups(int size);
    EXTERN void wbParse_Free_Monitor_Groups(void *pvoid,int size);
}

#endif //__WB_PARSE_MALLOC_H__
