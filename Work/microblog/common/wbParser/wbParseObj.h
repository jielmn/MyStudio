#ifndef __WB_PARSE_OBJ_H__
#define __WB_PARSE_OBJ_H__

#include "wbParseStruct.h"
#include "wbParser.h"



namespace wbParserNS
{
	class CWBXmlParser
	{
	public:
		/** 解析错误 */
		static t_wbParse_Error  *parse_error(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_error(wbParserNS::REQOBJ *obj , t_wbParse_Error * ptr, unsigned long *pmemSize=NULL);

		// 2012-05-08 add by sunfei 用于【获取@、赞、粉丝等初始消息】API

		static t_wbParse_Keda_Notify  *parse_keda_notify(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_notify(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Notify *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Notifies  *parse_keda_notifies(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_notifies(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Notifies *ptr, unsigned long *pmemSize=NULL);

		// 2012-03-16 add by sunfei

		static t_wbParse_Keda_departmentUsers  *parse_keda_departmentUsers(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_departmentUsers(wbParserNS::REQOBJ *obj , t_wbParse_Keda_departmentUsers *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Department  *parse_keda_department(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_department(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Department *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Departments  *parse_keda_departments(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_departments(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Departments *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_searchDepartment  *parse_keda_searchDepartment(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_searchDepartment(wbParserNS::REQOBJ *obj , t_wbParse_Keda_searchDepartment *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_searchUser  *parse_keda_searchUser(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_searchUser(wbParserNS::REQOBJ *obj , t_wbParse_Keda_searchUser *ptr, unsigned long *pmemSize=NULL);

		// add end


		static t_wbParse_Keda_Messages_Data  *parse_keda_messages_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_messages_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Messages_Data *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Like_Page  *parse_keda_likePage(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_likePage(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Like_Page *ptr, unsigned long *pmemSize=NULL);


		// 2012-03-07 add by sunfei

		static t_wbParse_Keda_EntUser  *parse_keda_entUser(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_entUser(wbParserNS::REQOBJ *obj , t_wbParse_Keda_EntUser *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_EntConfig  *parse_keda_entConfig(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_entConfig(wbParserNS::REQOBJ *obj , t_wbParse_Keda_EntConfig *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Result  *parse_keda_result(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_result(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Result *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_EntUsers  *parse_keda_entUsers(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_entUsers(wbParserNS::REQOBJ *obj , t_wbParse_Keda_EntUsers *ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Page  *parse_keda_page(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_page(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Page *ptr, unsigned long *pmemSize=NULL);

		// add end

		// 2012-02-29 add by sunfei

		static t_wbParse_Keda_Users_Data    *parse_keda_users_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_users_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Users_Data * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Users    *parse_keda_users(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_users(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Users * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Topic    *parse_keda_topic(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_topic(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Topic * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Like    *parse_keda_like(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_like(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Like * ptr, unsigned long *pmemSize=NULL);



		static t_wbParse_Keda_GroupStatistics    *parse_keda_groupStatistics(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_groupStatistics(wbParserNS::REQOBJ *obj , t_wbParse_Keda_GroupStatistics * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Group    *parse_keda_group(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_group(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Group * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Groups_Data    *parse_keda_groups_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_groups_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Groups_Data * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Groups    *parse_keda_groups(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_groups(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Groups * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_GroupCreate    *parse_keda_groupcreate(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_groupcreate(wbParserNS::REQOBJ *obj , t_wbParse_Keda_GroupCreate * ptr, unsigned long *pmemSize=NULL);

		// add end

		// 2012-03-02 add by sunfei
		static t_wbParse_Keda_Sender    *parse_keda_sender(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_sender(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Sender * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Receiver    *parse_keda_receiver(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_receiver(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Receiver * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_PrivateFeed    *parse_keda_privateFeed(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_privateFeed(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivateFeed * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_PrivateContacter    *parse_keda_privateContacter(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_privateContacter(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivateContacter * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_PrivCont_Data    *parse_keda_privCont_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_privCont_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivCont_Data * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_PrivateContacters    *parse_keda_privateContacters(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_privateContacters(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivateContacters * ptr, unsigned long *pmemSize=NULL);



		static t_wbParse_Keda_PrivFeeds_Data    *parse_keda_privFeeds_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_privFeeds_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivFeeds_Data * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_PrivateFeeds    *parse_keda_privateFeeds(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_privateFeeds(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivateFeeds * ptr, unsigned long *pmemSize=NULL);


		// add end


		// 2012-01-09 add by sunfei 

		static t_wbParse_Keda_User    *parse_keda_user(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_user(wbParserNS::REQOBJ *obj , t_wbParse_Keda_User * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_UserProfile    *parse_keda_user_profile(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_user_profile(wbParserNS::REQOBJ *obj , t_wbParse_Keda_UserProfile * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_OnlineState    *parse_keda_onlineState(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_onlineState(wbParserNS::REQOBJ *obj , t_wbParse_Keda_OnlineState * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Depts    *parse_keda_depts(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_depts(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Depts * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Dept    *parse_keda_dept(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_dept(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Dept * ptr, unsigned long *pmemSize=NULL);



		static t_wbParse_Keda_Messages    *parse_keda_messages(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_messages(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Messages * ptr, unsigned long *pmemSize=NULL);

		// ------
		static t_wbParse_Keda_Meta    *parse_keda_meta(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_meta(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Meta * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_ThreadMessage    *parse_keda_threadMessage(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_threadMessage(wbParserNS::REQOBJ *obj , t_wbParse_Keda_ThreadMessage * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_RelatedData    *parse_keda_relatedData(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_relatedData(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RelatedData * ptr, unsigned long *pmemSize=NULL);
		// ------

		// ======
		static t_wbParse_Keda_SubjectStyleThread    *parse_keda_subjectStyleThread(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_subjectStyleThread(wbParserNS::REQOBJ *obj , t_wbParse_Keda_SubjectStyleThread * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_ReplyStyleThread    *parse_keda_replyStyleThread(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_replyStyleThread(wbParserNS::REQOBJ *obj , t_wbParse_Keda_ReplyStyleThread * ptr, unsigned long *pmemSize=NULL);
		// ======

		// =-=-=-
		static t_wbParse_Keda_Subject    *parse_keda_subject(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_subject(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Subject * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Replies    *parse_keda_replies(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_replies(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Replies * ptr, unsigned long *pmemSize=NULL);
		// =-=-=-

		// ===---


		static t_wbParse_Keda_Id    *parse_keda_id(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_id(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Id * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_ReferencedUserIds    *parse_keda_referencedUserIds(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_referencedUserIds(wbParserNS::REQOBJ *obj , t_wbParse_Keda_ReferencedUserIds * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_TopicIds    *parse_keda_topicIds(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_topicIds(wbParserNS::REQOBJ *obj , t_wbParse_Keda_TopicIds * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_LikeByIds    *parse_keda_likeByIds(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_likeByIds(wbParserNS::REQOBJ *obj , t_wbParse_Keda_LikeByIds * ptr, unsigned long *pmemSize=NULL);



		static t_wbParse_Keda_Picture    *parse_keda_picture(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_picture(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Picture * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Pictures    *parse_keda_pictures(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_pictures(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Pictures * ptr, unsigned long *pmemSize=NULL);


		static t_wbParse_Keda_Attachment    *parse_keda_attachment(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_attachment(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Attachment * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Attachments    *parse_keda_attachments(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_attachments(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Attachments * ptr, unsigned long *pmemSize=NULL);


		static t_wbParse_Keda_FeedMessage    *parse_keda_feedMessage(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_feedMessage(wbParserNS::REQOBJ *obj , t_wbParse_Keda_FeedMessage * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_RefReply    *parse_keda_refReply(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_refReply(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RefReply * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_Reply   *parse_keda_reply(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_reply(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Reply * ptr, unsigned long *pmemSize=NULL);

		// ===---



		static t_wbParse_Keda_UserData    *parse_keda_userData(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_userData(wbParserNS::REQOBJ *obj , t_wbParse_Keda_UserData * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_UserEntry    *parse_keda_userEntry(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_userEntry(wbParserNS::REQOBJ *obj , t_wbParse_Keda_UserEntry * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_RelatedUsers    *parse_keda_relatedUsers(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_relatedUsers(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RelatedUsers * ptr, unsigned long *pmemSize=NULL);



		static t_wbParse_Keda_GroupData    *parse_keda_groupData(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_groupData(wbParserNS::REQOBJ *obj , t_wbParse_Keda_GroupData * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_GroupEntry    *parse_keda_groupEntry(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_groupEntry(wbParserNS::REQOBJ *obj , t_wbParse_Keda_GroupEntry * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_RelatedGroups   *parse_keda_relatedGroups(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_relatedGroups(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RelatedGroups * ptr, unsigned long *pmemSize=NULL);



		static t_wbParse_Keda_TopicData    *parse_keda_topicData(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_topicData(wbParserNS::REQOBJ *obj , t_wbParse_Keda_TopicData * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_TopicEntry    *parse_keda_topicEntry(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_topicEntry(wbParserNS::REQOBJ *obj , t_wbParse_Keda_TopicEntry * ptr, unsigned long *pmemSize=NULL);

		static t_wbParse_Keda_RelatedTopics   *parse_keda_relatedTopics(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize=NULL);
		static void parse_keda_relatedTopics(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RelatedTopics * ptr, unsigned long *pmemSize=NULL);

		// add end





        typedef struct tagXmlChildNamesParam
        {
            char *           szChildNames[16];
            unsigned int     dwCnt;

            tagXmlChildNamesParam()
            {
                memset( this, 0, sizeof(*this) );
            }
        }TXmlChildNamesParam, *PTXmlChildNamesParam;




        
        // meeting list ( root )
        static void parse_keda_meeting_list(wbParserNS::REQOBJ *obj , t_wbParse_m_meeting_list * ptr, unsigned long *pmemSize=NULL);

        // meeting list' related rooms
        static void parse_keda_meeting_short_rooms(wbParserNS::REQOBJ *obj , t_wbParse_m_short_rooms * ptr, unsigned long *pmemSize=NULL, 
                                                    const char * szRoomId = NULL);
        // meeting list' related room
        static void parse_keda_meeting_short_room(  wbParserNS::REQOBJ *obj , t_wbParse_m_short_room * ptr, unsigned long *pmemSize=NULL, 
                                                    const char * szRoomId = NULL );

        // meeting list's meetings
        static void parse_keda_meeting_meetings(wbParserNS::REQOBJ *obj , t_wbParse_m_short_meetings * ptr, unsigned long *pmemSize=NULL);
        // meeting list's meetings
        static void parse_keda_meeting_meeting(wbParserNS::REQOBJ *obj , t_wbParse_m_short_meeting * ptr, unsigned long *pmemSize=NULL);

        // meeting's room ids
        static void parse_keda_meeting_room_ids(wbParserNS::REQOBJ *obj , t_wbParse_m_room_ids * ptr, unsigned long *pmemSize=NULL);
        // meeting's room id
        static void parse_keda_meeting_room_id(wbParserNS::REQOBJ *obj , WBParseINT * ptr, unsigned long *pmemSize=NULL);

        
        // meeting info ( root )
        static void parse_keda_meeting_info(wbParserNS::REQOBJ *obj , t_wbParse_m_meeting_info * ptr, unsigned long *pmemSize=NULL);

        // persons
        static void parse_keda_meeting_persons(wbParserNS::REQOBJ *obj , t_wbParse_m_persons * ptr, unsigned long *pmemSize=NULL);
        // person
        static void parse_keda_meeting_person(wbParserNS::REQOBJ *obj , t_wbParse_m_person * ptr, unsigned long *pmemSize=NULL);


        // room list ( root )
        static void parse_keda_m_room_list(wbParserNS::REQOBJ *obj , t_wbParse_m_room_list * ptr, unsigned long *pmemSize=NULL);
        // rooms
        static void parse_keda_m_rooms(wbParserNS::REQOBJ *obj , t_wbParse_m_rooms * ptr, unsigned long *pmemSize=NULL);
        // room
        static void parse_keda_m_room(wbParserNS::REQOBJ *obj , t_wbParse_m_room * ptr, unsigned long *pmemSize=NULL);


        // normal room list ( root )
        static void parse_keda_m_normal_room_list(wbParserNS::REQOBJ *obj , t_wbParse_m_normal_room_list * ptr, unsigned long *pmemSize=NULL);
        // normal rooms
        static void parse_keda_m_short_rooms(wbParserNS::REQOBJ *obj , t_wbParse_m_short_rooms * ptr, unsigned long *pmemSize, 
                                             const char * szChildName, const TXmlChildNamesParam * pChildAttrs);
        // normal rooms
        static void parse_keda_m_short_room(wbParserNS::REQOBJ *obj , t_wbParse_m_short_room * ptr, unsigned long *pmemSize,
                                            const TXmlChildNamesParam * pChildAttrs);


        // room info ( root )
        static void parse_keda_room_info(wbParserNS::REQOBJ *obj , t_wbParse_m_room_info * ptr, unsigned long *pmemSize=NULL);

        // room status ( root )
        static void parse_keda_room_status(wbParserNS::REQOBJ *obj , t_wbParse_m_room_status * ptr, unsigned long *pmemSize=NULL);

        // room list by name ( root ) 
        static void parse_keda_room_by_name(wbParserNS::REQOBJ *obj , t_wbParse_m_short_rooms * ptr, unsigned long *pmemSize=NULL);
        




        // template list ( root element )
        static void parse_keda_template_list(wbParserNS::REQOBJ *obj , t_wbParse_m_templateList * ptr, unsigned long *pmemSize=NULL);
        // templates
        static void parse_keda_templates(wbParserNS::REQOBJ *obj , t_wbParse_m_templates * ptr, unsigned long *pmemSize=NULL);
        // template
        static void parse_keda_template(wbParserNS::REQOBJ *obj , t_wbParse_m_template * ptr, unsigned long *pmemSize=NULL);
        
        //// <person>
        //static void parse_keda_m_person(wbParserNS::REQOBJ *obj , t_wbParse_m_person * ptr, unsigned long *pmemSize=NULL);
        //// <persons>
        //static void parse_keda_m_persons(wbParserNS::REQOBJ *obj , t_wbParse_m_persons * ptr, unsigned long *pmemSize=NULL);

        // template info ( root )
        static void parse_keda_template_info(wbParserNS::REQOBJ *obj , t_wbParse_m_template_info * ptr, unsigned long *pmemSize=NULL);

        // notify list ( root element )
        static void parse_keda_list_notify_by_max(wbParserNS::REQOBJ *obj , t_wbParse_m_notifys * ptr, unsigned long *pmemSize=NULL);
        // notify item
        static void parse_keda_notify_item(wbParserNS::REQOBJ *obj , t_wbParse_m_notify * ptr, unsigned long *pmemSize=NULL);
        // notify content
        static void parse_keda_notify_content( wbParserNS::REQOBJ *obj , void * ptr, int nType );
        // content-meeting
        static void parse_keda_notify_content_meeting( wbParserNS::REQOBJ *obj , t_wbParse_m_notify_content_meeting * ptr );


        // regular info ( root )
        static void parse_keda_regular(wbParserNS::REQOBJ *obj , t_wbParse_m_regular_info * ptr, unsigned long *pmemSize=NULL);
        // meeting ids
        static void parse_keda_meeting_ids(wbParserNS::REQOBJ *obj , t_wbParse_m_meeting_ids * ptr, unsigned long *pmemSize=NULL);
        // meeting id
        static void parse_keda_meeting_id(wbParserNS::REQOBJ *obj , WBParseINT * ptr, unsigned long *pmemSize=NULL);


        // app: all notifys ( root )
        static void parse_app_all_notifys(wbParserNS::REQOBJ *obj , t_wbParse_app_notifys * ptr, unsigned long *pmemSize=NULL);

        static void parse_app_notify_item(wbParserNS::REQOBJ *obj , t_wbParse_app_notify * ptr, unsigned long *pmemSize=NULL);


        // monitor: group ( root )
        static void parse_monitor_group(wbParserNS::REQOBJ *obj , t_wbParse_monitor_group * ptr, unsigned long *pmemSize=NULL);

        static void parse_monitor_devices(wbParserNS::REQOBJ *obj , t_wbParse_monitor_devices * ptr, unsigned long *pmemSize=NULL);

        static void parse_monitor_device(wbParserNS::REQOBJ *obj , t_wbParse_monitor_device * ptr, unsigned long *pmemSize=NULL);

        // monitor: groups ( root )
        static void parse_monitor_groups(wbParserNS::REQOBJ *obj , t_wbParse_monitor_groups * ptr, unsigned long *pmemSize=NULL);
	};
}

#endif // __WB_PARSE_OBJ_H__
