#include <stdio.h>
#include <stdlib.h>
#include "wbParser/wbParseObj.h"
#include "wbParser/wbParseMalloc.h"
#include "wbParser/splitstr.h"
#include "wbParseMacro.h"
#include "strconv.h"
#include "libWeibo/weibo.h"
#include "inner.h"

using namespace wbParserNS ;

#define WBPARSER_KEY_IDX(name)         WBParser_##name##_KEY
#define WBPARSER_KEY_STATEIDX(name)    WBParser_state_##name##_KEY
#define WBPARSER_KEY_COMMENTIDX(name)  WBParser_comment_##name##_KEY
#define WBPARSER_KEY_DIRMSGIDX(name)   WBParser_directmsg_##name##_KEY
#define WBPARSER_KEY_GEO(name)         WBParser_geo_##name##_KEY
#define WBPARSER_KEY_UREADER(name)     WBParser_unreader_##name##_KEY
#define WBPARSER_KEY_COMCOUNT(name)    WBParser_comcount_##name##_KEY
#define WBPARSER_KEY_ACCESS_LMT(name)  WBParser_accesslmt_##name##_KEY
#define WBPARSER_KEY_EMOTION(name)     WBParser_emotion##name##_KEY
#define WBPARSER_KEY_XMLROOT(name)     WBParser_xmlroot_##name##KEY


#define WBPARSER_KEY_KEDA_USER_IDX(name)         WBParser_keda_user_##name##_KEY
#define WBPARSER_KEY_KEDA_MESS_IDX(name)         WBParser_keda_mess_##name##_KEY
#define WBPARSER_KEY_KEDA_TOPIC_IDX(name)        WBParser_keda_topic_##name##_KEY
#define WBPARSER_KEY_KEDA_LIKE_IDX(name)         WBParser_keda_like_##name##_KEY
#define WBPARSER_KEY_KEDA_GROUP_IDX(name)        WBParser_keda_group_##name##_KEY
#define WBPARSER_KEY_KEDA_PRIMES_IDX(name)       WBParser_keda_privatemessage_##name##_KEY
#define WBPARSER_KEY_KEDA_AMS_IDX(name)          WBParser_keda_ams_##name##_KEY
#define WBPARSER_KEY_KEDA_DEPARTMENT_IDX(name)   WBParser_keda_department_##name##_KEY
#define WBPARSER_KEY_KEDA_NOTIFY_IDX(name)       WBParser_keda_notify_##name##_KEY

#define WBPARSER_KEY_MEETING_IDX(name)           WBParser_keda_meeting_##name##_KEY



/////////////////////////
//关键字定义
// 2012-01-17 comment by sunfei 下面的枚举定义和字符串数组定义必须一一对应

typedef enum 
{
	// 2012-05-08 add by sunfei 用于【获取@、赞、粉丝等初始消息】API

	// <notifys>下的关键字
	WBPARSER_KEY_KEDA_NOTIFY_IDX(notify),

		// <notify>下的关键字
		WBPARSER_KEY_KEDA_NOTIFY_IDX(amount),
		WBPARSER_KEY_KEDA_NOTIFY_IDX(messageType),

	// 2012-03-16 add by sunfei

	// <departments>下的关键字
	WBPARSER_KEY_KEDA_DEPARTMENT_IDX(totalNum),   // 该关键字值只用于 /api/department/searchDepartment 和 /api/department/searchUser
	WBPARSER_KEY_KEDA_DEPARTMENT_IDX(data),       // 该关键字值只用于 /api/department/searchDepartment 和 /api/department/searchUser

	// <departments>下的关键字
	WBPARSER_KEY_KEDA_DEPARTMENT_IDX(department),

		// <department>下的关键字
		WBPARSER_KEY_KEDA_DEPARTMENT_IDX(departmentId),
		WBPARSER_KEY_KEDA_DEPARTMENT_IDX(departmentName),
		WBPARSER_KEY_KEDA_DEPARTMENT_IDX(fullPath),
		WBPARSER_KEY_KEDA_DEPARTMENT_IDX(parentId),
		WBPARSER_KEY_KEDA_DEPARTMENT_IDX(leaf),
		WBPARSER_KEY_KEDA_DEPARTMENT_IDX(users),

			// <users>下的关键字
			WBPARSER_KEY_KEDA_DEPARTMENT_IDX(user),

				// <user>下的关键字
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(name),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(email),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(mugshot128),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(mugshot64),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(mugshot40),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(personalBriefly),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(followersNum),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(fansNum),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(friendsNum),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(threadNum),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(beenFollowed),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(beenFollowBoth),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(enableWeibo),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(online),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(defaultUid),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(uid),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(e164),
				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(jid),

				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(profile),

					// <profile>下的关键字
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(userId),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(jobTitle),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(location),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(workPhone),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(mobilePhone),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(birthYear),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(birthMonth),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(birthDay),

				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(onlineState),

					// <onlineState>下的关键字
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(wbOnline),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(tlOnline),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(tdOnline),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(tpOnline),
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(tsOnline),

				WBPARSER_KEY_KEDA_DEPARTMENT_IDX(depts),

					// <depts>下的关键字
					WBPARSER_KEY_KEDA_DEPARTMENT_IDX(dept),

						// <dept>下的关键字
// 						WBPARSER_KEY_KEDA_DEPARTMENT_IDX(departmentId),
// 						WBPARSER_KEY_KEDA_DEPARTMENT_IDX(departmentName),
// 						WBPARSER_KEY_KEDA_DEPARTMENT_IDX(fullPath),
						WBPARSER_KEY_KEDA_DEPARTMENT_IDX(deptPosition),

	// add end


	// 2012-03-07 add by sunfei

	// <page>下的关键字
	WBPARSER_KEY_KEDA_AMS_IDX(totalCount),

	WBPARSER_KEY_KEDA_AMS_IDX(entUsers),

		// <result>和<entUsers>下的关键字
		WBPARSER_KEY_KEDA_AMS_IDX(entUser),

			// <entUser>下的关键字
			WBPARSER_KEY_KEDA_AMS_IDX(entName),
			WBPARSER_KEY_KEDA_AMS_IDX(entRealName),
			WBPARSER_KEY_KEDA_AMS_IDX(cn),
			WBPARSER_KEY_KEDA_AMS_IDX(sn),
			WBPARSER_KEY_KEDA_AMS_IDX(enable),
			WBPARSER_KEY_KEDA_AMS_IDX(privileged),
			WBPARSER_KEY_KEDA_AMS_IDX(entMail),
			WBPARSER_KEY_KEDA_AMS_IDX(subMail),
			WBPARSER_KEY_KEDA_AMS_IDX(jid),
			WBPARSER_KEY_KEDA_AMS_IDX(e164),
			WBPARSER_KEY_KEDA_AMS_IDX(mobileNum),
			WBPARSER_KEY_KEDA_AMS_IDX(extNum),
			WBPARSER_KEY_KEDA_AMS_IDX(seat),
			WBPARSER_KEY_KEDA_AMS_IDX(brief),
			WBPARSER_KEY_KEDA_AMS_IDX(position),
			WBPARSER_KEY_KEDA_AMS_IDX(portrait32),
			WBPARSER_KEY_KEDA_AMS_IDX(portrait40),
			WBPARSER_KEY_KEDA_AMS_IDX(portrait64),
			WBPARSER_KEY_KEDA_AMS_IDX(portrait128),
			WBPARSER_KEY_KEDA_AMS_IDX(portrait256),
			WBPARSER_KEY_KEDA_AMS_IDX(password),

			WBPARSER_KEY_KEDA_AMS_IDX(enableWeibo),
			WBPARSER_KEY_KEDA_AMS_IDX(enableMeeting),
			WBPARSER_KEY_KEDA_AMS_IDX(enableHD),
			WBPARSER_KEY_KEDA_AMS_IDX(enableCall),
			WBPARSER_KEY_KEDA_AMS_IDX(enableRoam),
			WBPARSER_KEY_KEDA_AMS_IDX(weiboAdmin),
			WBPARSER_KEY_KEDA_AMS_IDX(meetingAdmin),
			WBPARSER_KEY_KEDA_AMS_IDX(isLikeQuery),
			WBPARSER_KEY_KEDA_AMS_IDX(authMode),
			WBPARSER_KEY_KEDA_AMS_IDX(sex),

			WBPARSER_KEY_KEDA_AMS_IDX(depts),

		// <result>下的关键字
		WBPARSER_KEY_KEDA_AMS_IDX(entConfig),

			// <entConfig>下的关键字
			WBPARSER_KEY_KEDA_AMS_IDX(xmppDomain),
			WBPARSER_KEY_KEDA_AMS_IDX(apiUrl),
			WBPARSER_KEY_KEDA_AMS_IDX(weiboUrl),
			WBPARSER_KEY_KEDA_AMS_IDX(webImUrl),
			WBPARSER_KEY_KEDA_AMS_IDX(tcpImUrl),
			WBPARSER_KEY_KEDA_AMS_IDX(tcpPort),
			WBPARSER_KEY_KEDA_AMS_IDX(ulsUrl),
			WBPARSER_KEY_KEDA_AMS_IDX(ulsPort),

	// add end


	// 2012-09-18 add by sunfei

	// <page>下的关键字 -- 获取赞过我的微博数据API
	WBPARSER_KEY_KEDA_LIKE_IDX(pageSize),
	WBPARSER_KEY_KEDA_LIKE_IDX(totalCount),
	WBPARSER_KEY_KEDA_LIKE_IDX(currentPage),
	WBPARSER_KEY_KEDA_LIKE_IDX(totalPage),
	WBPARSER_KEY_KEDA_LIKE_IDX(avaCount),
	WBPARSER_KEY_KEDA_LIKE_IDX(data),

		// <data>下的关键字
		WBPARSER_KEY_KEDA_LIKE_IDX(messages),

	// add end

	// 2012-03-02 add by sunfei

	// <privateContacters>下的关键字
	WBPARSER_KEY_KEDA_PRIMES_IDX(pageSize),
	WBPARSER_KEY_KEDA_PRIMES_IDX(totalCount),
	WBPARSER_KEY_KEDA_PRIMES_IDX(currentPage),
	WBPARSER_KEY_KEDA_PRIMES_IDX(totalPage),
	WBPARSER_KEY_KEDA_PRIMES_IDX(avaCount),
	WBPARSER_KEY_KEDA_PRIMES_IDX(data),

		// <data>下的关键字
		WBPARSER_KEY_KEDA_PRIMES_IDX(privateContacter),

			// <privateContacter>下的关键字
			WBPARSER_KEY_KEDA_PRIMES_IDX(count),
			WBPARSER_KEY_KEDA_PRIMES_IDX(privateFeed),

				// <privateFeed>下的关键字
				WBPARSER_KEY_KEDA_PRIMES_IDX(id),
				WBPARSER_KEY_KEDA_PRIMES_IDX(content),
				WBPARSER_KEY_KEDA_PRIMES_IDX(createdAt),
				WBPARSER_KEY_KEDA_PRIMES_IDX(clientType),
				WBPARSER_KEY_KEDA_PRIMES_IDX(sender),
				WBPARSER_KEY_KEDA_PRIMES_IDX(receiver),

					// <sender>和<receiver>下的关键字
// 					WBPARSER_KEY_KEDA_PRIMES_IDX(id),
					WBPARSER_KEY_KEDA_PRIMES_IDX(uid),
					WBPARSER_KEY_KEDA_PRIMES_IDX(name),
					WBPARSER_KEY_KEDA_PRIMES_IDX(email),
					WBPARSER_KEY_KEDA_PRIMES_IDX(mugshot64),
					WBPARSER_KEY_KEDA_PRIMES_IDX(mugshot40),
					WBPARSER_KEY_KEDA_PRIMES_IDX(beenFollowed),

	// add end



	// 2012-03-01 add by sunfei

	// 0.begin  【获得话题信息】【获得like信息】

	// <topic>下的关键字
	WBPARSER_KEY_KEDA_TOPIC_IDX(id),
	WBPARSER_KEY_KEDA_TOPIC_IDX(threadId),
	WBPARSER_KEY_KEDA_TOPIC_IDX(name),
	WBPARSER_KEY_KEDA_TOPIC_IDX(createdBy),
	WBPARSER_KEY_KEDA_TOPIC_IDX(createdAt),
	WBPARSER_KEY_KEDA_TOPIC_IDX(feedCount),
	WBPARSER_KEY_KEDA_TOPIC_IDX(type),


	// <like>下的关键字
	WBPARSER_KEY_KEDA_LIKE_IDX(userName),
	WBPARSER_KEY_KEDA_LIKE_IDX(uid),
	WBPARSER_KEY_KEDA_LIKE_IDX(feedId),
	WBPARSER_KEY_KEDA_LIKE_IDX(threadId),


	// <groups>下的关键字
	WBPARSER_KEY_KEDA_GROUP_IDX(pageSize),
	WBPARSER_KEY_KEDA_GROUP_IDX(totalCount),
	WBPARSER_KEY_KEDA_GROUP_IDX(currentPage),
	WBPARSER_KEY_KEDA_GROUP_IDX(totalPage),
	WBPARSER_KEY_KEDA_GROUP_IDX(avaCount),
	WBPARSER_KEY_KEDA_GROUP_IDX(data),

		// <data> 下的关键字
		WBPARSER_KEY_KEDA_GROUP_IDX(group),

			// <group> 下的关键字
			WBPARSER_KEY_KEDA_GROUP_IDX(id),
			WBPARSER_KEY_KEDA_GROUP_IDX(uid),
			WBPARSER_KEY_KEDA_GROUP_IDX(name),
			WBPARSER_KEY_KEDA_GROUP_IDX(groupBriefly),
			WBPARSER_KEY_KEDA_GROUP_IDX(createdBy),
			WBPARSER_KEY_KEDA_GROUP_IDX(createdAt),
			WBPARSER_KEY_KEDA_GROUP_IDX(groupStatistics),

				// <groupStatistics> 下的关键字
				WBPARSER_KEY_KEDA_GROUP_IDX(groupId),
				WBPARSER_KEY_KEDA_GROUP_IDX(feedNum),
				WBPARSER_KEY_KEDA_GROUP_IDX(memberNum),
				WBPARSER_KEY_KEDA_GROUP_IDX(groupUid),
				WBPARSER_KEY_KEDA_GROUP_IDX(topicNum),
	// 0.end

	// add end


	// 2012-01-17 update by sunfei

	// 1.begin  【获得用户信息】

	// <users> 下的关键字
	WBPARSER_KEY_KEDA_USER_IDX(pageSize),
	WBPARSER_KEY_KEDA_USER_IDX(totalCount),
	WBPARSER_KEY_KEDA_USER_IDX(currentPage),
	WBPARSER_KEY_KEDA_USER_IDX(totalPage),
	WBPARSER_KEY_KEDA_USER_IDX(avaCount),
	WBPARSER_KEY_KEDA_USER_IDX(data),

		// <data> 下的关键字
		WBPARSER_KEY_KEDA_USER_IDX(user),

			// <user> 下的关键字
			WBPARSER_KEY_KEDA_USER_IDX(id),
			WBPARSER_KEY_KEDA_USER_IDX(name),
			WBPARSER_KEY_KEDA_USER_IDX(email),
			WBPARSER_KEY_KEDA_USER_IDX(mugshot128),
			WBPARSER_KEY_KEDA_USER_IDX(mugshot64),
			WBPARSER_KEY_KEDA_USER_IDX(mugshot40),
			WBPARSER_KEY_KEDA_USER_IDX(personalBriefly),
			WBPARSER_KEY_KEDA_USER_IDX(followersNum),
			WBPARSER_KEY_KEDA_USER_IDX(fansNum),
			WBPARSER_KEY_KEDA_USER_IDX(friendsNum),
			WBPARSER_KEY_KEDA_USER_IDX(threadNum),

			WBPARSER_KEY_KEDA_USER_IDX(profile),

				// <profile> 下的关键字
				WBPARSER_KEY_KEDA_USER_IDX(userId),
				WBPARSER_KEY_KEDA_USER_IDX(jobTitle),
				WBPARSER_KEY_KEDA_USER_IDX(location),
				WBPARSER_KEY_KEDA_USER_IDX(workPhone),
				WBPARSER_KEY_KEDA_USER_IDX(mobilePhone),
				WBPARSER_KEY_KEDA_USER_IDX(birthYear),
				WBPARSER_KEY_KEDA_USER_IDX(birthMonth),
				WBPARSER_KEY_KEDA_USER_IDX(birthDay),

			WBPARSER_KEY_KEDA_USER_IDX(onlineState),

				// <onlineState>下的关键字
				WBPARSER_KEY_KEDA_USER_IDX(wbOnline),
				WBPARSER_KEY_KEDA_USER_IDX(tlOnline),
				WBPARSER_KEY_KEDA_USER_IDX(tdOnline),
				WBPARSER_KEY_KEDA_USER_IDX(tpOnline),
				WBPARSER_KEY_KEDA_USER_IDX(tsOnline),

			WBPARSER_KEY_KEDA_USER_IDX(depts),

				// <depts>下的关键字
				WBPARSER_KEY_KEDA_USER_IDX(dept),

					// <dept>下的关键字
					WBPARSER_KEY_KEDA_USER_IDX(departmentId),
					WBPARSER_KEY_KEDA_USER_IDX(departmentName),
					WBPARSER_KEY_KEDA_USER_IDX(fullPath),
					WBPARSER_KEY_KEDA_USER_IDX(deptPosition),


			WBPARSER_KEY_KEDA_USER_IDX(beenFollowed),
			WBPARSER_KEY_KEDA_USER_IDX(beenFollowBoth),
			WBPARSER_KEY_KEDA_USER_IDX(enableWeibo),
			WBPARSER_KEY_KEDA_USER_IDX(online),
			WBPARSER_KEY_KEDA_USER_IDX(defaultUid),
			WBPARSER_KEY_KEDA_USER_IDX(uid),
			WBPARSER_KEY_KEDA_USER_IDX(e164),
			WBPARSER_KEY_KEDA_USER_IDX(jid),

	// 1.end


	// 2.begin  
	// 【回复一条微博】、【发布一条微博】、【获得一条主题以及回复】、
	// 【获得用户发布的微博数据（全部）】、【获得用户发布的微博数据（友讯）】
	// 【获得当前登录用户及其关注用户的微博】

	// <messages> 下的关键字
	WBPARSER_KEY_KEDA_MESS_IDX(meta),                   ///< 
		WBPARSER_KEY_KEDA_MESS_IDX(readFeedId),         ///< 
		WBPARSER_KEY_KEDA_MESS_IDX(oldestFeedId),       ///< 
		WBPARSER_KEY_KEDA_MESS_IDX(totalCount),         ///< 

	WBPARSER_KEY_KEDA_MESS_IDX(threadMessage),          ///<   这个以后可能要改成小写
		WBPARSER_KEY_KEDA_MESS_IDX(subjectStyleThread),       ///< 
		WBPARSER_KEY_KEDA_MESS_IDX(replyStyleThread),         ///< 

			// <subjectStyleThread> 或 <replyStyleThread> 下的关键字
			WBPARSER_KEY_KEDA_MESS_IDX(id),                 ///< 
			WBPARSER_KEY_KEDA_MESS_IDX(lastFeedId),         ///< 

			WBPARSER_KEY_KEDA_MESS_IDX(subject),            ///<

				// <subject> 下的关键字
// 				WBPARSER_KEY_KEDA_MESS_IDX(id),                 ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(body),               ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(threadId),           ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(directMessage),      ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(broadcast),          ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(systemMessage),      ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(feedType),           ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(createdBy),          ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(createdAt),          ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(clientType),         ///< 

				WBPARSER_KEY_KEDA_MESS_IDX(referencedUserIds),  ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(int),            ///< 这个名字要改(据说改不了了)
				WBPARSER_KEY_KEDA_MESS_IDX(topicIds),           ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(int),            ///< 这个名字要改(据说改不了了)
				WBPARSER_KEY_KEDA_MESS_IDX(likeByIds),          ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(int),            ///< 这个名字要改(据说改不了了)

				WBPARSER_KEY_KEDA_MESS_IDX(attachments),              ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(attachment),           ///<
						WBPARSER_KEY_KEDA_MESS_IDX(extensionName),    ///<
						WBPARSER_KEY_KEDA_MESS_IDX(fileName),         ///<
						WBPARSER_KEY_KEDA_MESS_IDX(fileUrl),          ///<

				WBPARSER_KEY_KEDA_MESS_IDX(pictures),                 ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(picture),              ///<
// 						WBPARSER_KEY_KEDA_MESS_IDX(extensionName),    ///<
						WBPARSER_KEY_KEDA_MESS_IDX(picturePreviewUrl),///<
						WBPARSER_KEY_KEDA_MESS_IDX(pictureTinyUrl),   ///<
						WBPARSER_KEY_KEDA_MESS_IDX(pictureMobileUrl),   ///<
						WBPARSER_KEY_KEDA_MESS_IDX(pictureName),      ///<
						WBPARSER_KEY_KEDA_MESS_IDX(pictureUrl),       ///<

				WBPARSER_KEY_KEDA_MESS_IDX(isBookMarked),       ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(hot),                ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(title),              ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(isDelete),           ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(replyNum),           ///< 
				WBPARSER_KEY_KEDA_MESS_IDX(replyNoLoadNum),     ///< 
                WBPARSER_KEY_KEDA_MESS_IDX(createLikeTime),     ///< 

			WBPARSER_KEY_KEDA_MESS_IDX(refReply),            ///<
			WBPARSER_KEY_KEDA_MESS_IDX(reply),               ///<

			WBPARSER_KEY_KEDA_MESS_IDX(replies),                ///<

				// <replies> 下的关键字
				WBPARSER_KEY_KEDA_MESS_IDX(feedMessage),        ///< 

					// <feedMessage> 下的关键字
// 					WBPARSER_KEY_KEDA_MESS_IDX(id),                 ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(body),               ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(threadId),           ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(repliedToId),        ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(repliedToBody),      ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(repliedToUserId),    ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(directMessage),      ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(broadcast),          ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(systemMessage),      ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(feedType),           ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(createdBy),          ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(createdAt),          ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(clientType),         ///< 

// 					WBPARSER_KEY_KEDA_MESS_IDX(referencedUserIds),  ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(topicIds),           ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(likeByIds),          ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(attachments),        ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(pictures),           ///< 

// 					WBPARSER_KEY_KEDA_MESS_IDX(isBookMarked),       ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(hot),                ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(isDelete),           ///< 

	WBPARSER_KEY_KEDA_MESS_IDX(relatedData),         ///< 

		// <relatedData> 下的关键字
		WBPARSER_KEY_KEDA_MESS_IDX(relatedUsers),        ///< 
			WBPARSER_KEY_KEDA_MESS_IDX(user),            ///< 这个建议改成 “userEntry”
				WBPARSER_KEY_KEDA_MESS_IDX(userId),            ///< 这个建议改成 “userId”
				WBPARSER_KEY_KEDA_MESS_IDX(userData),          ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(id),            ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(uid),           ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(name),          ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(email),         ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(mugshot64),     ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(mugshot40),     ///< 
					WBPARSER_KEY_KEDA_MESS_IDX(beenFollowed),  ///< 

		WBPARSER_KEY_KEDA_MESS_IDX(relatedGroups),       ///< 
			WBPARSER_KEY_KEDA_MESS_IDX(group),            ///< 这个建议改成 “groupEntry”
				WBPARSER_KEY_KEDA_MESS_IDX(groupId),            ///< 这个建议改成 “groupId”
				WBPARSER_KEY_KEDA_MESS_IDX(groupData),          ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(id),            ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(uid),           ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(name),          ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(email),         ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(mugshot64),     ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(mugshot40),     ///< 

		WBPARSER_KEY_KEDA_MESS_IDX(relatedTopics),       ///< 
			WBPARSER_KEY_KEDA_MESS_IDX(topic),            ///< 这个建议改成 “topicEntry”
				WBPARSER_KEY_KEDA_MESS_IDX(topicId),            ///< 这个建议改成 “topicId”
				WBPARSER_KEY_KEDA_MESS_IDX(topicData),         ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(id),            ///< 
// 					WBPARSER_KEY_KEDA_MESS_IDX(name),          ///< 
	// 2.end
	// add end
    WBPARSER_KEY_MEETING_IDX(total),
    WBPARSER_KEY_MEETING_IDX(relatedData),
    WBPARSER_KEY_MEETING_IDX(relatedRooms),
        WBPARSER_KEY_MEETING_IDX(room),
            WBPARSER_KEY_MEETING_IDX(id),
            WBPARSER_KEY_MEETING_IDX(name),

    WBPARSER_KEY_MEETING_IDX(meetings),
        WBPARSER_KEY_MEETING_IDX(meeting),
            //id
            WBPARSER_KEY_MEETING_IDX(subject),
            WBPARSER_KEY_MEETING_IDX(startTime),
            WBPARSER_KEY_MEETING_IDX(endTime),
            WBPARSER_KEY_MEETING_IDX(status),
            WBPARSER_KEY_MEETING_IDX(rooms),
                WBPARSER_KEY_MEETING_IDX(int),

	WBPARSER_KEY_IDX(end),

}key_index;

static
const WBParseCHAR* weibo_key_array[] =
{
	// 2012-05-08 add by sunfei 用于【获取@、赞、粉丝等初始消息】API

	// <notifys>下的关键字
	_WBC("notify"),

		// <notify>下的关键字
		_WBC("amount"),
		_WBC("messageType"),

	// 2012-03-16 add by sunfei

	// <departments>下的关键字
	_WBC("totalNum"),           // 该关键字值只用于 /api/department/searchDepartment 和 /api/department/searchUser
	_WBC("data"),               // 该关键字值只用于 /api/department/searchDepartment 和 /api/department/searchUser

	// <departments>下的关键字
	_WBC("department"),

		// <department>下的关键字
		_WBC("departmentId"),
		_WBC("departmentName"),
		_WBC("fullPath"),
		_WBC("parentId"),
		_WBC("leaf"),
		_WBC("users"),

			// <users>下的关键字
			_WBC("user"),

				// <user>下的关键字
				_WBC("name"),
				_WBC("email"),
				_WBC("mugshot128"),
				_WBC("mugshot64"),
				_WBC("mugshot40"),
				_WBC("personalBriefly"),
				_WBC("followersNum"),
				_WBC("fansNum"),
				_WBC("friendsNum"),
				_WBC("threadNum"),
				_WBC("beenFollowed"),
				_WBC("beenFollowBoth"),
				_WBC("enableWeibo"),
				_WBC("online"),
				_WBC("defaultUid"),
				_WBC("uid"),
				_WBC("e164"),
				_WBC("jid"),

				_WBC("profile"),

					// <profile>下的关键字
					_WBC("userId"),
					_WBC("jobTitle"),
					_WBC("location"),
					_WBC("workPhone"),
					_WBC("mobilePhone"),
					_WBC("birthYear"),
					_WBC("birthMonth"),
					_WBC("birthDay"),

				_WBC("onlineState"),

					// <onlineState>下的关键字
					_WBC("wbOnline"),
					_WBC("tlOnline"),
					_WBC("tdOnline"),
					_WBC("tpOnline"),
					_WBC("tsOnline"),

				_WBC("depts"),

					// <depts>下的关键字
					_WBC("dept"),

						// <dept>下的关键字
// 						_WBC("departmentId"),
// 						_WBC("departmentName"),
// 						_WBC("fullPath"),
						_WBC("deptPosition"),


	// add end


	// 2012-03-07 add by sunfei

	// <page>下的关键字
	_WBC("totalCount"),

	_WBC("entUsers"),

		// <result>下的关键字
		_WBC("entUser"),

			// <entUser>下的关键字
			_WBC("entName"),
			_WBC("entRealName"),
			_WBC("cn"),
			_WBC("sn"),
			_WBC("enable"),
			_WBC("privileged"),
			_WBC("entMail"),
			_WBC("subMail"),
			_WBC("jid"),
			_WBC("e164"),
			_WBC("mobileNum"),
			_WBC("extNum"),
			_WBC("seat"),
			_WBC("brief"),
			_WBC("position"),
			_WBC("portrait32"),
			_WBC("portrait40"),
			_WBC("portrait64"),
			_WBC("portrait128"),
			_WBC("portrait256"),
			_WBC("password"),

			_WBC("enableWeibo"),
			_WBC("enableMeeting"),
			_WBC("enableHD"),
			_WBC("enableCall"),
			_WBC("enableRoam"),
			_WBC("weiboAdmin"),
			_WBC("meetingAdmin"),
			_WBC("isLikeQuery"),
			_WBC("authMode"),
			_WBC("sex"),

			_WBC("depts"),

		// <result>下的关键字
		_WBC("entConfig"),

			// <entConfig>下的关键字
			_WBC("xmppDomain"),
			_WBC("apiUrl"),
			_WBC("weiboUrl"),
			_WBC("webImUrl"),
			_WBC("tcpImUrl"),
			_WBC("tcpPort"),
			_WBC("ulsUrl"),
			_WBC("ulsPort"),

	// add end


	// 2012-09-18 add by sunfei

	// <page>下的关键字 -- 获取赞过我的微博数据API
	_WBC("pageSize"),
	_WBC("totalCount"),
	_WBC("currentPage"),
	_WBC("totalPage"),
	_WBC("avaCount"),
	_WBC("data"),

		// <data>下的关键字
		_WBC("messages"),

	// add end


	// 2012-03-02 add by sunfei

	// <privateContacters>下的关键字
	_WBC("pageSize"),
	_WBC("totalCount"),
	_WBC("currentPage"),
	_WBC("totalPage"),
	_WBC("avaCount"),
	_WBC("data"),

		// <data>下的关键字
		_WBC("privateContacter"),

			// <privateContacter>下的关键字
			_WBC("count"),
			_WBC("privateFeed"),

				// <privateFeed>下的关键字
				_WBC("id"),
				_WBC("content"),
				_WBC("createdAt"),
				_WBC("clientType"),
				_WBC("sender"),
				_WBC("receiver"),

					// <sender>和<receiver>下的关键字
// 					_WBC("id"),
					_WBC("uid"),
					_WBC("name"),
					_WBC("email"),
					_WBC("mugshot64"),
					_WBC("mugshot40"),
					_WBC("beenFollowed"),

	// add end



	// 2012-03-01 add by sunfei

	// 0.begin  【获得话题信息】【获得like信息】

	// <topic>下的关键字
	_WBC("id"),
	_WBC("threadId"),
	_WBC("name"),
	_WBC("createdBy"),
	_WBC("createdAt"),
	_WBC("feedCount"),
	_WBC("type"),


	// <like>下的关键字
	_WBC("userName"),
	_WBC("uid"),
	_WBC("feedId"),
	_WBC("threadId"),


	// <groups>下的关键字
	_WBC("pageSize"),
	_WBC("totalCount"),
	_WBC("currentPage"),
	_WBC("totalPage"),
	_WBC("avaCount"),
	_WBC("data"),

		// <data> 下的关键字
		_WBC("group"),

			// <group> 下的关键字
			_WBC("id"),
			_WBC("uid"),
			_WBC("name"),
			_WBC("groupBriefly"),
			_WBC("createdBy"),
			_WBC("createdAt"),
			_WBC("groupStatistics"),

				// <groupStatistics> 下的关键字
				_WBC("groupId"),
				_WBC("feedNum"),
				_WBC("memberNum"),
				_WBC("groupUid"),
				_WBC("topicNum"),

	// 0.end

	// add end


	// 2012-01-17 update by sunfei

	// 1.begin  【获得用户信息】

	// <users> 下的关键字
	_WBC("pageSize"),        ///< 
	_WBC("totalCount"),      ///< 
	_WBC("currentPage"),     ///< 
	_WBC("totalPage"),       ///< 
	_WBC("avaCount"),        ///< 
	_WBC("data"),            ///< 

		// <data> 下的关键字
		_WBC("user"),            ///< 

			// <user> 下的关键字
			_WBC("id"),               ///< 用户数字id，如“124”
			_WBC("name"),             ///< 用户名，如“孙飞”
			_WBC("email"),            ///< 电子邮箱
			_WBC("mugshot128"),       ///< 图片128
			_WBC("mugshot64"),        ///< 图片64
			_WBC("mugshot40"),        ///< 图片40
			_WBC("personalBriefly"),  ///< 个性签名
			_WBC("followersNum"),     ///< 关注数目
			_WBC("fansNum"),          ///< 粉丝数目
			_WBC("friendsNum"),       ///< 好友数目（互相关注数目）
			_WBC("threadNum"),        ///< 微博数目

			_WBC("profile"),          ///< 详细个人信息

				// <profile> 下的关键字
				_WBC("userId"),           ///< 是否关注
				_WBC("jobTitle"),         ///< 是否相互关注
				_WBC("location"),         ///< 是否在线
				_WBC("workPhone"),        ///< 默认用户id，值同uid
				_WBC("mobilePhone"),      ///< 用户id，如“sunfei”
				_WBC("birthYear"),
				_WBC("birthMonth"),
				_WBC("birthDay"),

			_WBC("onlineState"),

				// <onlineState>下的关键字
				_WBC("wbOnline"),
				_WBC("tlOnline"),
				_WBC("tdOnline"),
				_WBC("tpOnline"),
				_WBC("tsOnline"),

			_WBC("depts"),

				// <depts>下的关键字
				_WBC("dept"),

					// <dept>下的关键字
					_WBC("departmentId"),
					_WBC("departmentName"),
					_WBC("fullPath"),
					_WBC("deptPosition"),

			_WBC("beenFollowed"),     ///< 是否关注
			_WBC("beenFollowBoth"),   ///< 是否相互关注
			_WBC("enableWeibo"),      ///< 微博是否可用
			_WBC("online"),           ///< 是否在线
			_WBC("defaultUid"),       ///< 默认用户id，值同uid
			_WBC("uid"),              ///< 用户id，如“sunfei”
			_WBC("e164"),
			_WBC("jid"),

	// 1.end


	// 2.begin 
	// 【回复一条微博】、【发布一条微博】、【获得一条主题以及回复】、
	// 【获得用户发布的微博数据（全部）】、【获得用户发布的微博数据（友讯）】
	// 【获得当前登录用户及其关注用户的微博】

	// <messages> 下的关键字
	_WBC("meta"),                   ///< 
		_WBC("readFeedId"),         ///< 
		_WBC("oldestFeedId"),       ///< 
		_WBC("totalCount"),         ///< 

	_WBC("threadMessage"),          ///<   这个以后可能要改成小写
		_WBC("subjectStyleThread"),       ///< 
		_WBC("replyStyleThread"),         ///< 

			// <subjectStyleThread> 或 <replyStyleThread> 下的关键字
			_WBC("id"),                 ///< 
			_WBC("lastFeedId"),         ///< 

			_WBC("subject"),            ///<

				// <subject> 下的关键字
// 				_WBC("id"),                 ///< 
				_WBC("body"),               ///< 
				_WBC("threadId"),           ///< 
				_WBC("directMessage"),      ///< 
				_WBC("broadcast"),          ///< 
				_WBC("systemMessage"),      ///< 
				_WBC("feedType"),           ///< 
				_WBC("createdBy"),          ///< 
				_WBC("createdAt"),          ///< 
				_WBC("clientType"),         ///< 

				_WBC("referencedUserIds"),  ///< 
					_WBC("int"),            ///< 这个名字要改(据说改不了了)
				_WBC("topicIds"),           ///< 
// 					_WBC("int"),            ///< 这个名字要改(据说改不了了)
				_WBC("likeByIds"),          ///< 
// 					_WBC("int"),            ///< 这个名字要改(据说改不了了)

				_WBC("attachments"),              ///< 
					_WBC("attachment"),           ///<
						_WBC("extensionName"),    ///<
						_WBC("fileName"),         ///<
						_WBC("fileUrl"),          ///<

				_WBC("pictures"),                 ///< 
					_WBC("picture"),              ///<
// 						_WBC("extensionName"),    ///<
						_WBC("picturePreviewUrl"),///<
						_WBC("pictureTinyUrl"),   ///<
						_WBC("pictureMobileUrl"), ///<
						_WBC("pictureName"),      ///<
						_WBC("pictureUrl"),       ///<

				_WBC("isBookMarked"),       ///< 
				_WBC("hot"),                ///< 
				_WBC("title"),              ///< 
				_WBC("isDelete"),           ///< 
				_WBC("replyNum"),           ///< 
				_WBC("replyNoLoadNum"),     ///< 

                _WBC("createLikeTime"),     ///< 

			_WBC("refReply"),                ///<
			_WBC("reply"),                ///<

			_WBC("replies"),                ///<

				// <replies> 下的关键字
				_WBC("feedMessage"),        ///< 

					// <feedMessage> 下的关键字
// 					_WBC("id"),                 ///< 
// 					_WBC("body"),               ///< 
// 					_WBC("threadId"),           ///< 
					_WBC("repliedToId"),        ///< 
					_WBC("repliedToBody"),      ///< 
					_WBC("repliedToUserId"),    ///< 
// 					_WBC("directMessage"),      ///< 
// 					_WBC("broadcast"),          ///< 
// 					_WBC("systemMessage"),      ///< 
// 					_WBC("feedType"),           ///< 
// 					_WBC("createdBy"),          ///< 
// 					_WBC("createdAt"),          ///< 
// 					_WBC("clientType"),         ///< 

// 					_WBC("referencedUserIds"),  ///< 
// 					_WBC("topicIds"),           ///< 
// 					_WBC("likeByIds"),          ///< 
// 					_WBC("attachments"),        ///< 
// 					_WBC("pictures"),           ///< 

// 					_WBC("isBookMarked"),       ///< 
// 					_WBC("hot"),                ///< 
// 					_WBC("isDelete"),           ///< 

	_WBC("relatedData"),         ///< 

		// <relatedData> 下的关键字
		_WBC("relatedUsers"),        ///< 
			_WBC("user"),            ///< 这个建议改成 “userEntry”
				_WBC("userId"),            ///< 这个建议改成 “userId”
				_WBC("userData"),          ///< 
// 					_WBC("id"),            ///< 
					_WBC("uid"),           ///< 
					_WBC("name"),          ///< 
					_WBC("email"),         ///< 
					_WBC("mugshot64"),     ///< 
					_WBC("mugshot40"),     ///<
					_WBC("beenFollowed"),  ///< 

		_WBC("relatedGroups"),       ///< 
			_WBC("group"),            ///< 这个建议改成 “groupEntry”
				_WBC("groupId"),            ///< 这个建议改成 “groupId”
				_WBC("groupData"),          ///< 
// 					_WBC("id"),            ///< 
// 					_WBC("uid"),           ///< 
// 					_WBC("name"),          ///< 
// 					_WBC("email"),         ///< 
// 					_WBC("mugshot64"),     ///< 
// 					_WBC("mugshot40"),     ///<

		_WBC("relatedTopics"),       ///< 
			_WBC("topic"),            ///< 这个建议改成 “topicEntry”
				_WBC("topicId"),            ///< 这个建议改成 “topicId”
				_WBC("topicData"),         ///< 
// 					_WBC("id"),            ///< 
// 					_WBC("name"),          ///< 
	// 2.end
    _WBC("total"),
    _WBC("relatedData"),
    _WBC("relatedRooms"),
        _WBC("room"),
            _WBC("id"),
            _WBC("name"),

    _WBC("meetings"),
        _WBC("meeting"),
            //id
            _WBC("subject"),
            _WBC("startTime"),
            _WBC("endTime"),
            _WBC("status"),
            _WBC("rooms"),
                _WBC("int"),

    // add end

};


const WBParseCHAR* wbParse_GetKey(int idx)
{
	if ( idx >= WBPARSER_KEY_IDX(end) || idx < 0 )
	{
		return 0;
	}
	return weibo_key_array[idx];
}

#define GET_VALUEKEY(idx) wbParse_GetKey(idx)
#define WEIBO_PARSER_CAST(t,ptr) (t*)ptr


struct t_wbParse_Error* CWBXmlParser::parse_error(wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Error* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Error,wbParse_Malloc_Error(1) );

		*pmemSize += sizeof(t_wbParse_Error);

		parse_error(obj,ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_error(wbParserNS::REQOBJ *obj , t_wbParse_Error * ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int len = 0;
	const char* txt = NULL;

	//
	//txt = wbParserNS::GetCHAR_Key_XML_EX("request" , obj , len );
	GET_STR_EX(XML,obj,"request",txt,len );
	if( len && txt ) {
		strncpy( ptr->request ,txt , WBPARSER_USE_LEN(request) );
	}

	//txt = wbParserNS::GetCHAR_Key_JSON_EX("error_code" , obj, len);
	GET_STR_EX(XML,obj,"error_code",txt,len);
	if( len && txt ) {
		strncpy( ptr->error_code,txt , WBPARSER_USE_LEN(error_code) );
	}

	//txt = wbParserNS::GetCHAR_Key_JSON_EX("error" , obj, len);
	GET_STR_EX(XML,obj,"error",txt,len);
	if( len && txt ) {
		strncpy( ptr->error,txt,WBPARSER_USE_LEN(error) );
		ptr->error_sub_code = atoi(ptr->error);
	}
}



// 2012-05-08 add by sunfei 用于【获取@、赞、粉丝等初始消息】API

t_wbParse_Keda_Notify  *CWBXmlParser::parse_keda_notify(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Notify* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Notify,wbParse_Malloc_Keda_Notify(1));

		*pmemSize += sizeof(t_wbParse_Keda_Notify);

		parse_keda_notify(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_notify(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Notify *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// amount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_NOTIFY_IDX(amount)),ptr->amount );

	// messageType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_NOTIFY_IDX(messageType)),ptr->messageType, WBPARSER_USE_LEN(messageType) );

}

// add end


t_wbParse_Keda_Notifies  *CWBXmlParser::parse_keda_notifies(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Notifies* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Notifies,wbParse_Malloc_Keda_Notifies(1));

		*pmemSize += sizeof(t_wbParse_Keda_Notifies);

		parse_keda_notifies(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_notifies(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Notifies *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->notifyNum = elementNum;

	if( elementNum ) 
	{
		ptr->notify = (t_wbParse_Keda_Notify *)wbParse_Malloc_Keda_Notify(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_Notify) * elementNum;

		if(ptr->notify) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_notify(pChild,ptr->notify + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}





// 2012-03-16 add by sunfei

t_wbParse_Keda_departmentUsers  *CWBXmlParser::parse_keda_departmentUsers(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_departmentUsers* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_departmentUsers,wbParse_Malloc_Keda_DepartmentUsers(1));

		*pmemSize += sizeof(t_wbParse_Keda_departmentUsers);

		parse_keda_departmentUsers(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_departmentUsers(wbParserNS::REQOBJ *obj , t_wbParse_Keda_departmentUsers *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->userNum = elementNum;

	if( elementNum ) 
	{
		ptr->user = (t_wbParse_Keda_User *)wbParse_Malloc_Keda_User(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_User) * elementNum;

		if(ptr->user) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_user(pChild,ptr->user + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}


t_wbParse_Keda_Department  *CWBXmlParser::parse_keda_department(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Department* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Department,wbParse_Malloc_Keda_Department(1));

		*pmemSize += sizeof(t_wbParse_Keda_Department);

		parse_keda_department(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_department(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Department *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// departmentId
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(departmentId)),ptr->departmentId );

	// departmentName
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(departmentName)),ptr->departmentName, WBPARSER_USE_LEN(departmentName) );

	// fullPath
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(fullPath)),ptr->fullPath,WBPARSER_USE_LEN(fullPath) );

	// parentId
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(parentId)),ptr->parentId );

	// leaf
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(leaf)),ptr->leaf, WBPARSER_USE_LEN(leaf) );


	USE_WEIBOPARSE_OBJ_PTR;

	// users
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(users)),ptr->users,pmemSize,parse_keda_departmentUsers);
}



t_wbParse_Keda_Departments  *CWBXmlParser::parse_keda_departments(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Departments* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Departments,wbParse_Malloc_Keda_Departments(1));

		*pmemSize += sizeof(t_wbParse_Keda_Departments);

		parse_keda_departments(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_departments(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Departments *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->departmentNum = elementNum;

	if( elementNum ) 
	{
		ptr->department = (t_wbParse_Keda_Department *)wbParse_Malloc_Keda_Department(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_Department) * elementNum;

		if(ptr->department) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_department(pChild,ptr->department + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}




t_wbParse_Keda_searchDepartment  *CWBXmlParser::parse_keda_searchDepartment(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_searchDepartment* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_searchDepartment,wbParse_Malloc_Keda_SearchDepartment(1));

		*pmemSize += sizeof(t_wbParse_Keda_searchDepartment);

		parse_keda_searchDepartment(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_searchDepartment(wbParserNS::REQOBJ *obj , t_wbParse_Keda_searchDepartment *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// totalNum
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(totalNum)),ptr->totalNum );

	USE_WEIBOPARSE_OBJ_PTR;

	// data
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(data)),ptr->data,pmemSize,parse_keda_departments);
}



t_wbParse_Keda_searchUser  *CWBXmlParser::parse_keda_searchUser(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_searchUser* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_searchUser,wbParse_Malloc_Keda_SearchUser(1));

		*pmemSize += sizeof(t_wbParse_Keda_searchUser);

		parse_keda_searchUser(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_searchUser(wbParserNS::REQOBJ *obj , t_wbParse_Keda_searchUser *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// totalNum
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(totalNum)),ptr->totalNum );

	USE_WEIBOPARSE_OBJ_PTR;

	// data
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(data)),ptr->data,pmemSize,parse_keda_users_data);
}


// add end





t_wbParse_Keda_Messages_Data  *CWBXmlParser::parse_keda_messages_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Messages_Data* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Messages_Data,wbParse_Malloc_Keda_Messages_Data(1));

		*pmemSize += sizeof(t_wbParse_Keda_Messages_Data);

		parse_keda_messages_data(obj, ptr, pmemSize);
	}
	return ptr;
}
void CWBXmlParser::parse_keda_messages_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Messages_Data *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	// messages
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(messages)),ptr->messages,pmemSize,parse_keda_messages);
}



t_wbParse_Keda_Like_Page  *CWBXmlParser::parse_keda_likePage(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Like_Page* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Like_Page,wbParse_Malloc_Keda_LikePage(1));

		*pmemSize += sizeof(t_wbParse_Keda_Like_Page);

		parse_keda_likePage(obj, ptr, pmemSize);
	}
	return ptr;
}
void CWBXmlParser::parse_keda_likePage(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Like_Page *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// pageSize
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(pageSize)),ptr->pageSize );

	// totalCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(totalCount)),ptr->totalCount );

	// currentPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(currentPage)),ptr->currentPage );

	// totalPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(totalPage)),ptr->totalPage );

	// avaCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(avaCount)),ptr->avaCount );


	USE_WEIBOPARSE_OBJ_PTR;
	// data
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(data)),ptr->data,pmemSize,parse_keda_messages_data);
}



// 2012-03-07 add by sunfei

t_wbParse_Keda_EntUser  *CWBXmlParser::parse_keda_entUser(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_EntUser* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_EntUser,wbParse_Malloc_Keda_EntUser(1));

		*pmemSize += sizeof(t_wbParse_Keda_EntUser);

		parse_keda_entUser(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_entUser(wbParserNS::REQOBJ *obj , t_wbParse_Keda_EntUser *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;



    GET_STR(XML,obj,"moid",          ptr->moId,      WBPARSER_USE_LEN(moId) );

    GET_STR(XML,obj,"companyMoid",  ptr->companyMoId, WBPARSER_USE_LEN(companyMoId) );



	// entName
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(entName)),ptr->entName, WBPARSER_USE_LEN(entName) );

	// entRealName
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(entRealName)),ptr->entRealName, WBPARSER_USE_LEN(entRealName) );

	// cn
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(cn)),ptr->cn, WBPARSER_USE_LEN(cn) );

	// sn
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(sn)),ptr->sn, WBPARSER_USE_LEN(sn) );

	// enable
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(enable)),ptr->enable, WBPARSER_USE_LEN(enable) );

	// privileged
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(privileged)),ptr->privileged, WBPARSER_USE_LEN(privileged) );

	// entMail
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(entMail)),ptr->entMail, WBPARSER_USE_LEN(entMail) );

	// subMail
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(subMail)),ptr->subMail, WBPARSER_USE_LEN(subMail) );

	// e164
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(e164)),ptr->e164, WBPARSER_USE_LEN(e164) );

	// jid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(jid)),ptr->jid, WBPARSER_USE_LEN(jid) );

	// mobileNum
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(mobileNum)),ptr->mobileNum, WBPARSER_USE_LEN(mobileNum) );

	// extNum
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(extNum)),ptr->extNum, WBPARSER_USE_LEN(extNum) );

	// seat
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(seat)),ptr->seat, WBPARSER_USE_LEN(seat) );

	// brief
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(brief)),ptr->brief, WBPARSER_USE_LEN(brief) );

	// position
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(position)),ptr->position, WBPARSER_USE_LEN(position) );

	// portrait32
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(portrait32)),ptr->portrait32, WBPARSER_USE_LEN(portrait32) );

	// portrait40
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(portrait40)),ptr->portrait40, WBPARSER_USE_LEN(portrait40) );

	// portrait64
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(portrait64)),ptr->portrait64, WBPARSER_USE_LEN(portrait64) );

	// portrait128
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(portrait128)),ptr->portrait128, WBPARSER_USE_LEN(portrait128) );

	// portrait256
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(portrait256)),ptr->portrait256, WBPARSER_USE_LEN(portrait256) );

	// password
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(password)),ptr->password, WBPARSER_USE_LEN(password) );

	// enableWeibo
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(enableWeibo)),ptr->enableWeibo, WBPARSER_USE_LEN(enableWeibo) );

	// enableMeeting
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(enableMeeting)),ptr->enableMeeting, WBPARSER_USE_LEN(enableMeeting) );

	// enableHD
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(enableHD)),ptr->enableHD, WBPARSER_USE_LEN(enableHD) );

	// enableCall
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(enableCall)),ptr->enableCall, WBPARSER_USE_LEN(enableCall) );

	// enableRoam
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(enableRoam)),ptr->enableRoam, WBPARSER_USE_LEN(enableRoam) );

	// weiboAdmin
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(weiboAdmin)),ptr->weiboAdmin, WBPARSER_USE_LEN(weiboAdmin) );

	// meetingAdmin
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(meetingAdmin)),ptr->meetingAdmin, WBPARSER_USE_LEN(meetingAdmin) );

	// isLikeQuery
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(isLikeQuery)),ptr->isLikeQuery, WBPARSER_USE_LEN(isLikeQuery) );

	// authMode
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(authMode)),ptr->authMode, WBPARSER_USE_LEN(authMode) );

	// sex
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(sex)),ptr->sex, WBPARSER_USE_LEN(sex) );


	USE_WEIBOPARSE_OBJ_PTR;

	// depts
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(depts)),ptr->depts,pmemSize,parse_keda_depts);

}


t_wbParse_Keda_EntConfig  *CWBXmlParser::parse_keda_entConfig(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_EntConfig* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_EntConfig,wbParse_Malloc_Keda_EntConfig(1));

		*pmemSize += sizeof(t_wbParse_Keda_EntConfig);

		parse_keda_entConfig(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_entConfig(wbParserNS::REQOBJ *obj , t_wbParse_Keda_EntConfig *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// xmppDomain
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(xmppDomain)),ptr->xmppDomain, WBPARSER_USE_LEN(xmppDomain) );

	// apiUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(apiUrl)),ptr->apiUrl, WBPARSER_USE_LEN(apiUrl) );

	// weiboUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(weiboUrl)),ptr->weiboUrl, WBPARSER_USE_LEN(weiboUrl) );

	// webImUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(webImUrl)),ptr->webImUrl, WBPARSER_USE_LEN(webImUrl) );

	// tcpImUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(tcpImUrl)),ptr->tcpImUrl, WBPARSER_USE_LEN(tcpImUrl) );

	// tcpPort
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(tcpPort)),ptr->tcpPort, WBPARSER_USE_LEN(tcpPort) );

	// ulsUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(ulsUrl)),ptr->ulsUrl, WBPARSER_USE_LEN(ulsUrl) );

	// ulsPort
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(ulsPort)),ptr->ulsPort, WBPARSER_USE_LEN(ulsPort) );
}

t_wbParse_Keda_Result  *CWBXmlParser::parse_keda_result(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Result* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Result,wbParse_Malloc_Keda_Result(1));

		*pmemSize += sizeof(t_wbParse_Keda_Result);

		parse_keda_result(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_result(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Result *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	// entUser
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(entUser)),ptr->entUser,pmemSize,parse_keda_entUser);

	// entConfig
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(entConfig)),ptr->entConfig,pmemSize,parse_keda_entConfig);
}



t_wbParse_Keda_EntUsers  *CWBXmlParser::parse_keda_entUsers(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_EntUsers* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_EntUsers,wbParse_Malloc_Keda_EntUsers(1));

		*pmemSize += sizeof(t_wbParse_Keda_EntUsers);

		parse_keda_entUsers(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_entUsers(wbParserNS::REQOBJ *obj , t_wbParse_Keda_EntUsers *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->entUserNum = elementNum;

	if( elementNum ) 
	{
		ptr->entUser = (t_wbParse_Keda_EntUser *)wbParse_Malloc_Keda_EntUser(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_EntUser) * elementNum;

		if(ptr->entUser) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_entUser(pChild,ptr->entUser + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}


t_wbParse_Keda_Page  *CWBXmlParser::parse_keda_page(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Page* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Page,wbParse_Malloc_Keda_Page(1));

		*pmemSize += sizeof(t_wbParse_Keda_Page);

		parse_keda_page(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_page(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Page *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// totalCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(totalCount)),ptr->totalCount );

	USE_WEIBOPARSE_OBJ_PTR;

	// entUsers
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_AMS_IDX(entUsers)),ptr->entUsers,pmemSize,parse_keda_entUsers);
}



// add end




// 2012-02-29 add by sunfei

t_wbParse_Keda_Users_Data  *CWBXmlParser::parse_keda_users_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Users_Data* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Users_Data,wbParse_Malloc_Keda_Users_Data(1));

		*pmemSize += sizeof(t_wbParse_Keda_Users_Data);

		parse_keda_users_data(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_users_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Users_Data *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->userNum = elementNum;

	if( elementNum ) 
	{
		ptr->user = (t_wbParse_Keda_User *)wbParse_Malloc_Keda_User(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_User) * elementNum;

		if(ptr->user) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_user(pChild,ptr->user + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}

}

t_wbParse_Keda_Users  *CWBXmlParser::parse_keda_users(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Users* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Users,wbParse_Malloc_Keda_Users(1));

		*pmemSize += sizeof(t_wbParse_Keda_Users);

		parse_keda_users(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_users(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Users *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// pageSize
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(pageSize)),ptr->pageSize );

	// totalCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(totalCount)),ptr->totalCount );

	// currentPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(currentPage)),ptr->currentPage );

	// totalPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(totalPage)),ptr->totalPage );

	// avaCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(avaCount)),ptr->avaCount );


	USE_WEIBOPARSE_OBJ_PTR;
	// data
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(data)),ptr->data,pmemSize,parse_keda_users_data);

}





t_wbParse_Keda_Topic  *CWBXmlParser::parse_keda_topic(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Topic* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Topic,wbParse_Malloc_Keda_Topic(1));

		*pmemSize += sizeof(t_wbParse_Keda_Topic);

		parse_keda_topic(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_topic(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Topic *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_TOPIC_IDX(id)),ptr->id, WBPARSER_USE_LEN(id) );

	// threadId
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_TOPIC_IDX(threadId)),ptr->threadId, WBPARSER_USE_LEN(threadId) );

	// name
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_TOPIC_IDX(name)),ptr->name, WBPARSER_USE_LEN(name) );

	// createdBy
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_TOPIC_IDX(createdBy)),ptr->createdBy, WBPARSER_USE_LEN(createdBy) );

	// createdAt
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_TOPIC_IDX(createdAt)),ptr->createdAt, WBPARSER_USE_LEN(createdAt) );

	// feedCount
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_TOPIC_IDX(feedCount)),ptr->feedCount, WBPARSER_USE_LEN(feedCount) );

	// type
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_TOPIC_IDX(type)),ptr->type, WBPARSER_USE_LEN(type) );

}



t_wbParse_Keda_Like  *CWBXmlParser::parse_keda_like(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Like* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Like,wbParse_Malloc_Keda_Like(1));

		*pmemSize += sizeof(t_wbParse_Keda_Like);

		parse_keda_like(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_like(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Like *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// userName
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(userName)),ptr->userName, WBPARSER_USE_LEN(userName) );

	// uid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(uid)),ptr->uid, WBPARSER_USE_LEN(uid) );

	// feedId
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(feedId)),ptr->feedId, WBPARSER_USE_LEN(feedId) );

	// threadId
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_LIKE_IDX(threadId)),ptr->threadId, WBPARSER_USE_LEN(threadId) );

}







t_wbParse_Keda_GroupStatistics  *CWBXmlParser::parse_keda_groupStatistics(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_GroupStatistics* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_GroupStatistics,wbParse_Malloc_Keda_GroupStatistics(1));

		*pmemSize += sizeof(t_wbParse_Keda_GroupStatistics);

		parse_keda_groupStatistics(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_groupStatistics(wbParserNS::REQOBJ *obj , t_wbParse_Keda_GroupStatistics *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// groupId
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(groupId)),ptr->groupId, WBPARSER_USE_LEN(groupId) );

	// pagfeedNumeSize
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(feedNum)),ptr->feedNum );

	// memberNum
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(memberNum)),ptr->memberNum );

	// groupUid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(groupUid)),ptr->groupUid, WBPARSER_USE_LEN(groupUid) );

	// topicNum
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(topicNum)),ptr->topicNum );

}

t_wbParse_Keda_Group  *CWBXmlParser::parse_keda_group(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Group* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Group,wbParse_Malloc_Keda_Group(1));

		*pmemSize += sizeof(t_wbParse_Keda_Group);

		parse_keda_group(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_group(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Group *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(id)),ptr->id, WBPARSER_USE_LEN(id) );

	// uid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(uid)),ptr->uid, WBPARSER_USE_LEN(uid) );

	// name
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(name)),ptr->name, WBPARSER_USE_LEN(name) );

	// groupBriefly
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(groupBriefly)),ptr->groupBriefly, WBPARSER_USE_LEN(groupBriefly) );

	// createdBy
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(createdBy)),ptr->createdBy, WBPARSER_USE_LEN(createdBy) );

	// createdAt
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(createdAt)),ptr->createdAt, WBPARSER_USE_LEN(createdAt) );

	USE_WEIBOPARSE_OBJ_PTR;
	// groupStatistics
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(groupStatistics)),ptr->groupStatistics,pmemSize,parse_keda_groupStatistics);

}

t_wbParse_Keda_Groups_Data  *CWBXmlParser::parse_keda_groups_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Groups_Data* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Groups_Data,wbParse_Malloc_Keda_Groups_Data(1));

		*pmemSize += sizeof(t_wbParse_Keda_Groups_Data);

		parse_keda_groups_data(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_groups_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Groups_Data *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->groupNum = elementNum;

	if( elementNum ) 
	{
		ptr->group = (t_wbParse_Keda_Group *)wbParse_Malloc_Keda_Group(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_Group) * elementNum;

		if(ptr->group) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_group(pChild,ptr->group + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}

}

t_wbParse_Keda_Groups  *CWBXmlParser::parse_keda_groups(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Groups* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Groups,wbParse_Malloc_Keda_Groups(1));

		*pmemSize += sizeof(t_wbParse_Keda_Groups);

		parse_keda_groups(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_groups(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Groups *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// pageSize
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(pageSize)),ptr->pageSize );

	// totalCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(totalCount)),ptr->totalCount );

	// currentPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(currentPage)),ptr->currentPage );

	// totalPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(totalPage)),ptr->totalPage );

	// avaCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(avaCount)),ptr->avaCount );


	USE_WEIBOPARSE_OBJ_PTR;
	// data
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(data)),ptr->data,pmemSize,parse_keda_groups_data);

}





t_wbParse_Keda_GroupCreate  *CWBXmlParser::parse_keda_groupcreate(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_GroupCreate* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_GroupCreate,wbParse_Malloc_Keda_GroupCreate(1));

		*pmemSize += sizeof(t_wbParse_Keda_GroupCreate);

		parse_keda_groupcreate(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_groupcreate(wbParserNS::REQOBJ *obj , t_wbParse_Keda_GroupCreate *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// uid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(uid)),ptr->uid, WBPARSER_USE_LEN(uid) );

	// name
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(name)),ptr->name, WBPARSER_USE_LEN(name) );

	// groupBriefly
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_GROUP_IDX(groupBriefly)),ptr->groupBriefly, WBPARSER_USE_LEN(groupBriefly) );


}

// add end





// 2012-03-02 add by sunfei



t_wbParse_Keda_Sender  *CWBXmlParser::parse_keda_sender(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Sender* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Sender,wbParse_Malloc_Keda_Sender(1));

		*pmemSize += sizeof(t_wbParse_Keda_Sender);

		parse_keda_sender(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_sender(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Sender *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(id)),ptr->id, WBPARSER_USE_LEN(id) );

	// uid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(uid)),ptr->uid, WBPARSER_USE_LEN(uid) );

	// name
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(name)),ptr->name, WBPARSER_USE_LEN(name) );

	// email
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(email)),ptr->email, WBPARSER_USE_LEN(email) );

	// mugshot64
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(mugshot64)),ptr->mugshot64, WBPARSER_USE_LEN(mugshot64) );

	// mugshot40
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(mugshot40)),ptr->mugshot40, WBPARSER_USE_LEN(mugshot40) );

	// beenFollowed
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(beenFollowed)),ptr->beenFollowed, WBPARSER_USE_LEN(beenFollowed) );
}




t_wbParse_Keda_Receiver  *CWBXmlParser::parse_keda_receiver(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Receiver* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Receiver,wbParse_Malloc_Keda_Receiver(1));

		*pmemSize += sizeof(t_wbParse_Keda_Receiver);

		parse_keda_receiver(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_receiver(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Receiver *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(id)),ptr->id, WBPARSER_USE_LEN(id) );

	// uid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(uid)),ptr->uid, WBPARSER_USE_LEN(uid) );

	// name
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(name)),ptr->name, WBPARSER_USE_LEN(name) );

	// email
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(email)),ptr->email, WBPARSER_USE_LEN(email) );

	// mugshot64
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(mugshot64)),ptr->mugshot64, WBPARSER_USE_LEN(mugshot64) );

	// mugshot40
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(mugshot40)),ptr->mugshot40, WBPARSER_USE_LEN(mugshot40) );

	// beenFollowed
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(beenFollowed)),ptr->beenFollowed, WBPARSER_USE_LEN(beenFollowed) );
}





t_wbParse_Keda_PrivateFeed  *CWBXmlParser::parse_keda_privateFeed(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_PrivateFeed* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_PrivateFeed,wbParse_Malloc_Keda_PrivateFeed(1));

		*pmemSize += sizeof(t_wbParse_Keda_PrivateFeed);

		parse_keda_privateFeed(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_privateFeed(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivateFeed *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(id)),ptr->id, WBPARSER_USE_LEN(id) );

	// content
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(content)),ptr->content, WBPARSER_USE_LEN(content) );

	// createdAt
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(createdAt)),ptr->createdAt, WBPARSER_USE_LEN(createdAt) );

	// clientType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(clientType)),ptr->clientType, WBPARSER_USE_LEN(clientType) );

	USE_WEIBOPARSE_OBJ_PTR;

	// sender
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(sender)),ptr->sender,pmemSize,parse_keda_sender);

	// receiver
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(receiver)),ptr->receiver,pmemSize,parse_keda_receiver);

}




t_wbParse_Keda_PrivateContacter  *CWBXmlParser::parse_keda_privateContacter(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_PrivateContacter* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_PrivateContacter,wbParse_Malloc_Keda_PrivateContacter(1));

		*pmemSize += sizeof(t_wbParse_Keda_PrivateContacter);

		parse_keda_privateContacter(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_privateContacter(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivateContacter *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// count
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(count)),ptr->count );

	USE_WEIBOPARSE_OBJ_PTR;

	// privateFeed
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(privateFeed)),ptr->privateFeed,pmemSize,parse_keda_privateFeed);

}

t_wbParse_Keda_PrivCont_Data  *CWBXmlParser::parse_keda_privCont_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_PrivCont_Data* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_PrivCont_Data,wbParse_Malloc_Keda_PrivCont_Data(1));

		*pmemSize += sizeof(t_wbParse_Keda_PrivCont_Data);

		parse_keda_privCont_data(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_privCont_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivCont_Data *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->privContNum = elementNum;

	if( elementNum ) 
	{
		ptr->privateContacter = (t_wbParse_Keda_PrivateContacter *)wbParse_Malloc_Keda_PrivateContacter(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_PrivateContacter) * elementNum;

		if(ptr->privateContacter) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_privateContacter(pChild,ptr->privateContacter + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}



t_wbParse_Keda_PrivateContacters  *CWBXmlParser::parse_keda_privateContacters(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_PrivateContacters* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_PrivateContacters,wbParse_Malloc_Keda_PrivateContacters(1));

		*pmemSize += sizeof(t_wbParse_Keda_PrivateContacters);

		parse_keda_privateContacters(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_privateContacters(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivateContacters *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// pageSize
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(pageSize)),ptr->pageSize );

	// totalCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(totalCount)),ptr->totalCount );

	// currentPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(currentPage)),ptr->currentPage );

	// totalPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(totalPage)),ptr->totalPage );

	// avaCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(avaCount)),ptr->avaCount );


	USE_WEIBOPARSE_OBJ_PTR;
	// data
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(data)),ptr->data,pmemSize,parse_keda_privCont_data);

}












t_wbParse_Keda_PrivFeeds_Data  *CWBXmlParser::parse_keda_privFeeds_data(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_PrivFeeds_Data* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_PrivFeeds_Data,wbParse_Malloc_Keda_PrivFeeds_Data(1));

		*pmemSize += sizeof(t_wbParse_Keda_PrivFeeds_Data);

		parse_keda_privFeeds_data(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_privFeeds_data(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivFeeds_Data *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->privateFeedNum = elementNum;

	if( elementNum ) 
	{
		ptr->privateFeed = (t_wbParse_Keda_PrivateFeed *)wbParse_Malloc_Keda_PrivateFeed(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_PrivateFeed) * elementNum;

		if(ptr->privateFeed) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_privateFeed(pChild,ptr->privateFeed + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}

t_wbParse_Keda_PrivateFeeds  *CWBXmlParser::parse_keda_privateFeeds(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_PrivateFeeds* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_PrivateFeeds,wbParse_Malloc_Keda_PrivateFeeds(1));

		*pmemSize += sizeof(t_wbParse_Keda_PrivateFeeds);

		parse_keda_privateFeeds(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_privateFeeds(wbParserNS::REQOBJ *obj , t_wbParse_Keda_PrivateFeeds *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// pageSize
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(pageSize)),ptr->pageSize );

	// totalCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(totalCount)),ptr->totalCount );

	// currentPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(currentPage)),ptr->currentPage );

	// totalPage
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(totalPage)),ptr->totalPage );

	// avaCount
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(avaCount)),ptr->avaCount );


	USE_WEIBOPARSE_OBJ_PTR;
	// data
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_PRIMES_IDX(data)),ptr->data,pmemSize,parse_keda_privFeeds_data);

}





// add end







// ----------------------------------------

// 2012-01-09 add by sunfei


t_wbParse_Keda_UserProfile  *CWBXmlParser::parse_keda_user_profile(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_UserProfile* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_UserProfile,wbParse_Malloc_Keda_UserProfile(1));

		*pmemSize += sizeof(t_wbParse_Keda_UserProfile);

		parse_keda_user_profile(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_user_profile(wbParserNS::REQOBJ *obj , t_wbParse_Keda_UserProfile * ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// userId
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(userId)),ptr->userId,WBPARSER_USE_LEN(userId) );

	// jobTitle
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(jobTitle)),ptr->jobTitle,WBPARSER_USE_LEN(jobTitle) );

	// location
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(location)),ptr->location,WBPARSER_USE_LEN(location) );

	// workPhone
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(workPhone)),ptr->workPhone,WBPARSER_USE_LEN(workPhone) );

	// mobilePhone
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(mobilePhone)),ptr->mobilePhone,WBPARSER_USE_LEN(mobilePhone) );

	// birthYear
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(birthYear)),ptr->birthYear );

	// birthMonth
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(birthMonth)),ptr->birthMonth );

	// birthDay
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(birthDay)),ptr->birthDay );

}



t_wbParse_Keda_OnlineState  *CWBXmlParser::parse_keda_onlineState(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_OnlineState* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_OnlineState,wbParse_Malloc_Keda_OnlineState(1));

		*pmemSize += sizeof(t_wbParse_Keda_OnlineState);

		parse_keda_onlineState(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_onlineState(wbParserNS::REQOBJ *obj , t_wbParse_Keda_OnlineState * ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// wbOnline
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(wbOnline)),ptr->wbOnline,WBPARSER_USE_LEN(wbOnline) );

	// tlOnline
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(tlOnline)),ptr->tlOnline,WBPARSER_USE_LEN(tlOnline) );

	// tdOnline
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(tdOnline)),ptr->tdOnline,WBPARSER_USE_LEN(tdOnline) );

	// tpOnline
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(tpOnline)),ptr->tpOnline,WBPARSER_USE_LEN(tpOnline) );

	// tsOnline
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(tsOnline)),ptr->tsOnline,WBPARSER_USE_LEN(tsOnline) );
}



t_wbParse_Keda_Dept  *CWBXmlParser::parse_keda_dept(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Dept* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Dept,wbParse_Malloc_Keda_Dept(1));

		*pmemSize += sizeof(t_wbParse_Keda_Dept);

		parse_keda_dept(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_dept(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Dept * ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// departmentId
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(departmentId)),ptr->departmentId );

	// departmentName
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(departmentName)),ptr->departmentName,WBPARSER_USE_LEN(departmentName) );

	// fullPath
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(fullPath)),ptr->fullPath,WBPARSER_USE_LEN(fullPath) );

	// deptPosition
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_DEPARTMENT_IDX(deptPosition)),ptr->deptPosition,WBPARSER_USE_LEN(deptPosition) );
}

t_wbParse_Keda_Depts  *CWBXmlParser::parse_keda_depts(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Depts* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Depts,wbParse_Malloc_Keda_Depts(1));

		*pmemSize += sizeof(t_wbParse_Keda_Depts);

		parse_keda_depts(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_depts(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Depts * ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->deptNum = elementNum;

	if( elementNum ) 
	{
		ptr->dept = (t_wbParse_Keda_Dept *)wbParse_Malloc_Keda_Dept(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_Dept) * elementNum;

		if(ptr->dept) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_dept(pChild,ptr->dept + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}



t_wbParse_Keda_User  *CWBXmlParser::parse_keda_user(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_User* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_User,wbParse_Malloc_Keda_User(1));

		*pmemSize += sizeof(t_wbParse_Keda_User);

		parse_keda_user(obj, ptr, pmemSize);
	}
	return ptr;
}

void CWBXmlParser::parse_keda_user(wbParserNS::REQOBJ *obj , t_wbParse_Keda_User *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	// GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );
    GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// name
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(name)),ptr->name,WBPARSER_USE_LEN(name) );

	// email
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(email)),ptr->email,WBPARSER_USE_LEN(email) );

	// mugshot128
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(mugshot128)),ptr->mugshot128,WBPARSER_USE_LEN(mugshot128) );

	// mugshot64
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(mugshot64)),ptr->mugshot64,WBPARSER_USE_LEN(mugshot64) );

	// mugshot40
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(mugshot40)),ptr->mugshot40,WBPARSER_USE_LEN(mugshot40) );

	// personalBriefly
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(personalBriefly)),ptr->personalBriefly,WBPARSER_USE_LEN(personalBriefly) );

	// followersNum
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(followersNum)),ptr->followersNum );

	// fansNum
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(fansNum)),ptr->fansNum );

	// friendsNum
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(friendsNum)),ptr->friendsNum );

	// threadNum
	GET_LONG(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(threadNum)),ptr->threadNum );


	USE_WEIBOPARSE_OBJ_PTR;

	// profile
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(profile)),ptr->profile,pmemSize,parse_keda_user_profile);

	// onlineState
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(onlineState)),ptr->onlineState,pmemSize,parse_keda_onlineState);

	// depts
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(depts)),ptr->depts,pmemSize,parse_keda_depts);


	// beenFollowed
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(beenFollowed)),ptr->beenFollowed,WBPARSER_USE_LEN(beenFollowed) );

	// beenFollowBoth
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(beenFollowBoth)),ptr->beenFollowBoth,WBPARSER_USE_LEN(beenFollowBoth) );

	// enableWeibo
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(enableWeibo)),ptr->enableWeibo,WBPARSER_USE_LEN(enableWeibo) );

	// online
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(online)),ptr->online,WBPARSER_USE_LEN(online) );

	// defaultUid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(defaultUid)),ptr->defaultUid,WBPARSER_USE_LEN(defaultUid) );

	// uid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(uid)),ptr->uid,WBPARSER_USE_LEN(uid) );

	// e164
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(e164)),ptr->e164,WBPARSER_USE_LEN(e164) );

	// jid
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_USER_IDX(jid)),ptr->jid,WBPARSER_USE_LEN(jid) );
}



t_wbParse_Keda_Meta  *CWBXmlParser::parse_keda_meta(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Meta* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Meta,wbParse_Malloc_Keda_Meta(1));

		*pmemSize += sizeof(t_wbParse_Keda_Meta);

		parse_keda_meta(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_meta(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Meta *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// readFeedId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(readFeedId)),ptr->readFeedId,WBPARSER_USE_LEN(readFeedId) );

	// oldestFeedId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(oldestFeedId)),ptr->oldestFeedId,WBPARSER_USE_LEN(oldestFeedId) );

	// totalCount
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(totalCount)),ptr->totalCount,WBPARSER_USE_LEN(totalCount) );
}



t_wbParse_Keda_UserData  *CWBXmlParser::parse_keda_userData(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_UserData* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_UserData,wbParse_Malloc_Keda_UserData(1));

		*pmemSize += sizeof(t_wbParse_Keda_UserData);

		parse_keda_userData(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_userData(wbParserNS::REQOBJ *obj , t_wbParse_Keda_UserData *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	// GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );
    GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// uid
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(uid)),ptr->uid,WBPARSER_USE_LEN(uid) );

	// name
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(name)),ptr->name,WBPARSER_USE_LEN(name) );

	// email
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(email)),ptr->email,WBPARSER_USE_LEN(email) );

	// mugshot64
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(mugshot64)),ptr->mugshot64,WBPARSER_USE_LEN(mugshot64) );

	// mugshot40
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(mugshot40)),ptr->mugshot40,WBPARSER_USE_LEN(mugshot40) );

	// beenFollowed
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(beenFollowed)),ptr->beenFollowed,WBPARSER_USE_LEN(beenFollowed) );

}

t_wbParse_Keda_UserEntry  *CWBXmlParser::parse_keda_userEntry(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_UserEntry* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_UserEntry,wbParse_Malloc_Keda_UserEntry(1));

		*pmemSize += sizeof(t_wbParse_Keda_UserEntry);

		parse_keda_userEntry(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_userEntry(wbParserNS::REQOBJ *obj , t_wbParse_Keda_UserEntry *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// userId
	// GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(userId)),ptr->userId,WBPARSER_USE_LEN(userId) );
    GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(userId)),ptr->userId,WBPARSER_USE_LEN(userId) );

	USE_WEIBOPARSE_OBJ_PTR;

	// userData
	GET_OBJECT_SUNFEI(XML, obj, GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(userData)), ptr->userData, pmemSize, parse_keda_userData);

}

t_wbParse_Keda_RelatedUsers  *CWBXmlParser::parse_keda_relatedUsers(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_RelatedUsers* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_RelatedUsers,wbParse_Malloc_Keda_RelatedUsers(1));

		*pmemSize += sizeof(t_wbParse_Keda_RelatedUsers);

		parse_keda_relatedUsers(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_relatedUsers(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RelatedUsers *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->userEntryNum = elementNum;

	if( elementNum ) 
	{
		ptr->userEntry = (t_wbParse_Keda_UserEntry *)wbParse_Malloc_Keda_UserEntry(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_UserEntry) * elementNum;

		if(ptr->userEntry) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_userEntry(pChild,ptr->userEntry + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}






t_wbParse_Keda_GroupData  *CWBXmlParser::parse_keda_groupData(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_GroupData* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_GroupData,wbParse_Malloc_Keda_GroupData(1));

		*pmemSize += sizeof(t_wbParse_Keda_GroupData);

		parse_keda_groupData(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_groupData(wbParserNS::REQOBJ *obj , t_wbParse_Keda_GroupData *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// uid
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(uid)),ptr->uid,WBPARSER_USE_LEN(uid) );

	// name
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(name)),ptr->name,WBPARSER_USE_LEN(name) );

	// mugshot64
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(mugshot64)),ptr->mugshot64,WBPARSER_USE_LEN(mugshot64) );

	// mugshot40
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(mugshot40)),ptr->mugshot40,WBPARSER_USE_LEN(mugshot40) );
}

t_wbParse_Keda_GroupEntry  *CWBXmlParser::parse_keda_groupEntry(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_GroupEntry* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_GroupEntry,wbParse_Malloc_Keda_GroupEntry(1));

		*pmemSize += sizeof(t_wbParse_Keda_GroupEntry);

		parse_keda_groupEntry(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_groupEntry(wbParserNS::REQOBJ *obj , t_wbParse_Keda_GroupEntry *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// groupId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(groupId)),ptr->groupId,WBPARSER_USE_LEN(groupId) );

	USE_WEIBOPARSE_OBJ_PTR;

	// groupData
	GET_OBJECT_SUNFEI(XML, obj, GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(groupData)), ptr->groupData, pmemSize, parse_keda_groupData);

}


t_wbParse_Keda_RelatedGroups  *CWBXmlParser::parse_keda_relatedGroups(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_RelatedGroups* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_RelatedGroups,wbParse_Malloc_Keda_RelatedGroups(1));

		*pmemSize += sizeof(t_wbParse_Keda_RelatedGroups);

		parse_keda_relatedGroups(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_relatedGroups(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RelatedGroups *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->groupEntryNum = elementNum;

	if( elementNum ) 
	{
		ptr->groupEntry = (t_wbParse_Keda_GroupEntry *)wbParse_Malloc_Keda_GroupEntry(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_GroupEntry) * elementNum;

		if(ptr->groupEntry) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_groupEntry(pChild,ptr->groupEntry + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}







t_wbParse_Keda_TopicData  *CWBXmlParser::parse_keda_topicData(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_TopicData* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_TopicData,wbParse_Malloc_Keda_TopicData(1));

		*pmemSize += sizeof(t_wbParse_Keda_TopicData);

		parse_keda_topicData(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_topicData(wbParserNS::REQOBJ *obj , t_wbParse_Keda_TopicData *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// name
	GET_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(name)),ptr->name,WBPARSER_USE_LEN(name) );

}


t_wbParse_Keda_TopicEntry  *CWBXmlParser::parse_keda_topicEntry(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_TopicEntry* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_TopicEntry,wbParse_Malloc_Keda_TopicEntry(1));

		*pmemSize += sizeof(t_wbParse_Keda_TopicEntry);

		parse_keda_topicEntry(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_topicEntry(wbParserNS::REQOBJ *obj , t_wbParse_Keda_TopicEntry *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// topicId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(topicId)),ptr->topicId,WBPARSER_USE_LEN(topicId) );

	USE_WEIBOPARSE_OBJ_PTR;

	// topicData
	GET_OBJECT_SUNFEI(XML, obj, GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(topicData)), ptr->topicData, pmemSize, parse_keda_topicData);

}



t_wbParse_Keda_RelatedTopics *CWBXmlParser::parse_keda_relatedTopics(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_RelatedTopics* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_RelatedTopics,wbParse_Malloc_Keda_RelatedTopics(1));

		*pmemSize += sizeof(t_wbParse_Keda_RelatedTopics);

		parse_keda_relatedTopics(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_relatedTopics(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RelatedTopics *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->topicEntryNum = elementNum;

	if( elementNum ) 
	{
		ptr->topicEntry = (t_wbParse_Keda_TopicEntry *)wbParse_Malloc_Keda_TopicEntry(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_TopicEntry) * elementNum;

		if(ptr->topicEntry) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_topicEntry(pChild,ptr->topicEntry + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}





t_wbParse_Keda_RelatedData  *CWBXmlParser::parse_keda_relatedData(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_RelatedData* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_RelatedData,wbParse_Malloc_Keda_RelatedData(1));

		*pmemSize += sizeof(t_wbParse_Keda_RelatedData);

		parse_keda_relatedData(obj, ptr, pmemSize );
	}
	return ptr;
}



void CWBXmlParser::parse_keda_relatedData(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RelatedData *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	GET_OBJECT_SUNFEI(XML, obj, GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(relatedUsers)), ptr->relatedUsers, pmemSize, parse_keda_relatedUsers);
	GET_OBJECT_SUNFEI(XML, obj, GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(relatedGroups)), ptr->relatedGroups, pmemSize, parse_keda_relatedGroups);
	GET_OBJECT_SUNFEI(XML, obj, GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(relatedTopics)), ptr->relatedTopics, pmemSize, parse_keda_relatedTopics);

}


t_wbParse_Keda_FeedMessage  *CWBXmlParser::parse_keda_feedMessage(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_FeedMessage* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_FeedMessage,wbParse_Malloc_Keda_FeedMessage(1));

		*pmemSize += sizeof(t_wbParse_Keda_FeedMessage);

		parse_keda_feedMessage(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_feedMessage(wbParserNS::REQOBJ *obj , t_wbParse_Keda_FeedMessage *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// body
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(body)),ptr->body,WBPARSER_USE_LEN(body) );

	// threadId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(threadId)),ptr->threadId,WBPARSER_USE_LEN(threadId) );

	// repliedToId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(repliedToId)),ptr->repliedToId,WBPARSER_USE_LEN(repliedToId) );

	// repliedToBody
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(repliedToBody)),ptr->repliedToBody,WBPARSER_USE_LEN(repliedToBody) );

	// repliedToUserId
	// GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(repliedToUserId)),ptr->repliedToUserId,WBPARSER_USE_LEN(repliedToUserId) );
    GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(repliedToUserId)),ptr->repliedToUserId,WBPARSER_USE_LEN(repliedToUserId) );

	// directMessage
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(directMessage)),ptr->directMessage,WBPARSER_USE_LEN(directMessage) );

	// broadcast
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(broadcast)),ptr->broadcast,WBPARSER_USE_LEN(broadcast) );

	// systemMessage
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(systemMessage)),ptr->systemMessage,WBPARSER_USE_LEN(systemMessage) );

	// feedType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(feedType)),ptr->feedType,WBPARSER_USE_LEN(feedType) );

	// createdBy
	// GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdBy)),ptr->createdBy,WBPARSER_USE_LEN(createdBy) );
    GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdBy)),ptr->createdBy,WBPARSER_USE_LEN(createdBy) );

	// createdAt
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdAt)),ptr->createdAt,WBPARSER_USE_LEN(createdAt) );

	// clientType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(clientType)),ptr->clientType,WBPARSER_USE_LEN(clientType) );


	// 2012-02-06 add by sunfei

	USE_WEIBOPARSE_OBJ_PTR;

	// referencedUserIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(referencedUserIds)),ptr->referencedUserIds, pmemSize, parse_keda_referencedUserIds);

	// topicIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(topicIds)),ptr->topicIds, pmemSize, parse_keda_topicIds);

	// likeByIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(likeByIds)),ptr->likeByIds, pmemSize, parse_keda_likeByIds);

	// pictures
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(pictures)),ptr->pictures, pmemSize, parse_keda_pictures);

	// attachments
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(attachments)),ptr->attachments, pmemSize, parse_keda_attachments);

	// add end


	// isBookMarked
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(isBookMarked)),ptr->isBookMarked,WBPARSER_USE_LEN(isBookMarked) );

	// hot
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(hot)),ptr->hot,WBPARSER_USE_LEN(hot) );

	// isDelete
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(isDelete)),ptr->isDelete,WBPARSER_USE_LEN(isDelete) );

    // createLikeTime
    GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createLikeTime)),ptr->createLikeTime, WBPARSER_USE_LEN(createLikeTime) );
}






t_wbParse_Keda_RefReply  *CWBXmlParser::parse_keda_refReply(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_RefReply* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_RefReply,wbParse_Malloc_Keda_RefReply(1));

		*pmemSize += sizeof(t_wbParse_Keda_RefReply);

		parse_keda_refReply(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_refReply(wbParserNS::REQOBJ *obj , t_wbParse_Keda_RefReply *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// body
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(body)),ptr->body,WBPARSER_USE_LEN(body) );

	// threadId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(threadId)),ptr->threadId,WBPARSER_USE_LEN(threadId) );

	// repliedToId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(repliedToId)),ptr->repliedToId,WBPARSER_USE_LEN(repliedToId) );

	// repliedToBody
// 	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(repliedToBody)),ptr->repliedToBody,WBPARSER_USE_LEN(repliedToBody) );

	// directMessage
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(directMessage)),ptr->directMessage,WBPARSER_USE_LEN(directMessage) );

	// broadcast
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(broadcast)),ptr->broadcast,WBPARSER_USE_LEN(broadcast) );

	// systemMessage
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(systemMessage)),ptr->systemMessage,WBPARSER_USE_LEN(systemMessage) );

	// feedType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(feedType)),ptr->feedType,WBPARSER_USE_LEN(feedType) );

	// createdBy
	// GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdBy)),ptr->createdBy,WBPARSER_USE_LEN(createdBy) );
    GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdBy)),ptr->createdBy,WBPARSER_USE_LEN(createdBy) );

	// createdAt
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdAt)),ptr->createdAt,WBPARSER_USE_LEN(createdAt) );

	// clientType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(clientType)),ptr->clientType,WBPARSER_USE_LEN(clientType) );


	// 2012-02-06 add by sunfei

	USE_WEIBOPARSE_OBJ_PTR;

	// referencedUserIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(referencedUserIds)),ptr->referencedUserIds, pmemSize, parse_keda_referencedUserIds);

	// topicIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(topicIds)),ptr->topicIds, pmemSize, parse_keda_topicIds);

	// likeByIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(likeByIds)),ptr->likeByIds, pmemSize, parse_keda_likeByIds);

	// pictures
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(pictures)),ptr->pictures, pmemSize, parse_keda_pictures);

	// attachments
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(attachments)),ptr->attachments, pmemSize, parse_keda_attachments);


	// add end


	// isBookMarked
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(isBookMarked)),ptr->isBookMarked,WBPARSER_USE_LEN(isBookMarked) );

	// hot
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(hot)),ptr->hot,WBPARSER_USE_LEN(hot) );

	// isDelete
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(isDelete)),ptr->isDelete,WBPARSER_USE_LEN(isDelete) );
}




t_wbParse_Keda_Reply  *CWBXmlParser::parse_keda_reply(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Reply* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Reply,wbParse_Malloc_Keda_Reply(1));

		*pmemSize += sizeof(t_wbParse_Keda_Reply);

		parse_keda_reply(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_reply(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Reply *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// body
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(body)),ptr->body,WBPARSER_USE_LEN(body) );

	// threadId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(threadId)),ptr->threadId,WBPARSER_USE_LEN(threadId) );

	// repliedToId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(repliedToId)),ptr->repliedToId,WBPARSER_USE_LEN(repliedToId) );

	// repliedToBody
// 	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(repliedToBody)),ptr->repliedToBody,WBPARSER_USE_LEN(repliedToBody) );

	// directMessage
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(directMessage)),ptr->directMessage,WBPARSER_USE_LEN(directMessage) );

	// broadcast
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(broadcast)),ptr->broadcast,WBPARSER_USE_LEN(broadcast) );

	// systemMessage
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(systemMessage)),ptr->systemMessage,WBPARSER_USE_LEN(systemMessage) );

	// feedType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(feedType)),ptr->feedType,WBPARSER_USE_LEN(feedType) );

	// createdBy
	// GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdBy)),ptr->createdBy,WBPARSER_USE_LEN(createdBy) );
    GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdBy)),ptr->createdBy,WBPARSER_USE_LEN(createdBy) );

	// createdAt
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdAt)),ptr->createdAt,WBPARSER_USE_LEN(createdAt) );

	// clientType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(clientType)),ptr->clientType,WBPARSER_USE_LEN(clientType) );


	// 2012-02-06 add by sunfei

	USE_WEIBOPARSE_OBJ_PTR;

	// referencedUserIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(referencedUserIds)),ptr->referencedUserIds, pmemSize, parse_keda_referencedUserIds);

	// topicIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(topicIds)),ptr->topicIds, pmemSize, parse_keda_topicIds);

	// likeByIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(likeByIds)),ptr->likeByIds, pmemSize, parse_keda_likeByIds);

	// pictures
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(pictures)),ptr->pictures, pmemSize, parse_keda_pictures);

	// attachments
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(attachments)),ptr->attachments, pmemSize, parse_keda_attachments);

	// add end


	// isBookMarked
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(isBookMarked)),ptr->isBookMarked,WBPARSER_USE_LEN(isBookMarked) );

	// hot
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(hot)),ptr->hot,WBPARSER_USE_LEN(hot) );

	// isDelete
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(isDelete)),ptr->isDelete,WBPARSER_USE_LEN(isDelete) );
}




// 2012-02-06 add by sunfei

t_wbParse_Keda_Picture  *CWBXmlParser::parse_keda_picture(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Picture* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Picture,wbParse_Malloc_Keda_Picture(1));

		*pmemSize += sizeof(t_wbParse_Keda_Picture);

		parse_keda_picture(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_picture(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Picture *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// extensionName
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(extensionName)),ptr->extensionName,WBPARSER_USE_LEN(extensionName) );

	// picturePreviewUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(picturePreviewUrl)),ptr->picturePreviewUrl,WBPARSER_USE_LEN(picturePreviewUrl) );

	// pictureTinyUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(pictureTinyUrl)),ptr->pictureTinyUrl,WBPARSER_USE_LEN(pictureTinyUrl) );

	// pictureMobileUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(pictureMobileUrl)),ptr->pictureMobileUrl,WBPARSER_USE_LEN(pictureMobileUrl) );

	// pictureName
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(pictureName)),ptr->pictureName,WBPARSER_USE_LEN(pictureName) );

	// pictureUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(pictureUrl)),ptr->pictureUrl,WBPARSER_USE_LEN(pictureUrl) );

    // tiny mobile
    GET_STR(XML, obj, "pictureTinyMobileUrl",  ptr->pictureTinyMobileUrl,  WBPARSER_USE_LEN(pictureMobileUrl) );

    // large mobile
    GET_STR(XML, obj, "pictureLargeMobileUrl", ptr->pictureLargeMobileUrl, WBPARSER_USE_LEN(pictureMobileUrl) );
}



t_wbParse_Keda_Pictures  *CWBXmlParser::parse_keda_pictures(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Pictures* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Pictures,wbParse_Malloc_Keda_Pictures(1));

		*pmemSize += sizeof(t_wbParse_Keda_Pictures);

		parse_keda_pictures(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_pictures(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Pictures *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->pictureNum = elementNum;

	if( elementNum ) 
	{
		ptr->picture = (t_wbParse_Keda_Picture *)wbParse_Malloc_Keda_Picture(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_Picture) * elementNum;

		if(ptr->picture) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_picture(pChild,ptr->picture + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}


t_wbParse_Keda_Attachment  *CWBXmlParser::parse_keda_attachment(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Attachment* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Attachment,wbParse_Malloc_Keda_Attachment(1));

		*pmemSize += sizeof(t_wbParse_Keda_Attachment);

		parse_keda_attachment(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_attachment(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Attachment *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// extensionName
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(extensionName)),ptr->extensionName,WBPARSER_USE_LEN(extensionName) );

	// fileName
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(fileName)),ptr->fileName,WBPARSER_USE_LEN(fileName) );

	// fileUrl
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(fileUrl)),ptr->fileUrl,WBPARSER_USE_LEN(fileUrl) );

}



t_wbParse_Keda_Attachments  *CWBXmlParser::parse_keda_attachments(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Attachments* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Attachments,wbParse_Malloc_Keda_Attachments(1));

		*pmemSize += sizeof(t_wbParse_Keda_Attachments);

		parse_keda_attachments(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_attachments(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Attachments *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->attachmentNum = elementNum;

	if( elementNum ) 
	{
		ptr->attachment = (t_wbParse_Keda_Attachment *)wbParse_Malloc_Keda_Attachment(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_Attachment) * elementNum;

		if(ptr->attachment) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_attachment(pChild,ptr->attachment + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}








t_wbParse_Keda_Id  *CWBXmlParser::parse_keda_id(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Id* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Id,wbParse_Malloc_Keda_Id(1));

		*pmemSize += sizeof(t_wbParse_Keda_Id);

		parse_keda_id(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_id(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Id *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// 2012-02-07 add by sunfei 这里的XMl元素名字为int与C++的默认类型相同，故在结构定义时使用了id作为其相应的名字
	// 这里的处理方式与其他地方不太一样，注意

	ptr->id = (WBParseLONG)wbParserNS::GetLong_XML(obj);

}

t_wbParse_Keda_ReferencedUserIds  *CWBXmlParser::parse_keda_referencedUserIds(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_ReferencedUserIds* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_ReferencedUserIds,wbParse_Malloc_Keda_ReferencedUserIds(1));

		*pmemSize += sizeof(t_wbParse_Keda_ReferencedUserIds);

		parse_keda_referencedUserIds(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_referencedUserIds(wbParserNS::REQOBJ *obj , t_wbParse_Keda_ReferencedUserIds *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->referencedUserIdNum = elementNum;

	if( elementNum ) 
	{
		ptr->referencedUserId = (t_wbParse_Keda_Id *)wbParse_Malloc_Keda_Id(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_Id) * elementNum;

		if(ptr->referencedUserId) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_id(pChild,ptr->referencedUserId + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}


t_wbParse_Keda_TopicIds  *CWBXmlParser::parse_keda_topicIds(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_TopicIds* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_TopicIds,wbParse_Malloc_Keda_TopicIds(1));

		*pmemSize += sizeof(t_wbParse_Keda_TopicIds);

		parse_keda_topicIds(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_topicIds(wbParserNS::REQOBJ *obj , t_wbParse_Keda_TopicIds *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->topicIdNum = elementNum;

	if( elementNum ) 
	{
		ptr->topicId = (t_wbParse_Keda_Id *)wbParse_Malloc_Keda_Id(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_Id) * elementNum;

		if(ptr->topicId) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_id(pChild,ptr->topicId + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}


t_wbParse_Keda_LikeByIds  *CWBXmlParser::parse_keda_likeByIds(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_LikeByIds* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_LikeByIds,wbParse_Malloc_Keda_LikeByIds(1));

		*pmemSize += sizeof(t_wbParse_Keda_LikeByIds);

		parse_keda_likeByIds(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_likeByIds(wbParserNS::REQOBJ *obj , t_wbParse_Keda_LikeByIds *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->likeByIdNum = elementNum;

	if( elementNum ) 
	{
		ptr->likeById = (t_wbParse_Keda_Id *)wbParse_Malloc_Keda_Id(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_Id) * elementNum;

		if(ptr->likeById) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_id(pChild,ptr->likeById + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}


// add end

///////////////////////////





t_wbParse_Keda_Subject  *CWBXmlParser::parse_keda_subject(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Subject* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Subject,wbParse_Malloc_Keda_Subject(1));

		*pmemSize += sizeof(t_wbParse_Keda_Subject);

		parse_keda_subject(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_subject(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Subject *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// body
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(body)),ptr->body,WBPARSER_USE_LEN(body) );

	// threadId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(threadId)),ptr->threadId,WBPARSER_USE_LEN(threadId) );

	// directMessage
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(directMessage)),ptr->directMessage,WBPARSER_USE_LEN(directMessage) );

	// broadcast
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(broadcast)),ptr->broadcast,WBPARSER_USE_LEN(broadcast) );

	// systemMessage
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(systemMessage)),ptr->systemMessage,WBPARSER_USE_LEN(systemMessage) );

	// feedType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(feedType)),ptr->feedType,WBPARSER_USE_LEN(feedType) );

	// createdBy
	// GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdBy)),ptr->createdBy,WBPARSER_USE_LEN(createdBy) );
    GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdBy)),ptr->createdBy,WBPARSER_USE_LEN(createdBy) );

	// createdAt
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createdAt)),ptr->createdAt,WBPARSER_USE_LEN(createdAt) );

	// clientType
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(clientType)),ptr->clientType,WBPARSER_USE_LEN(clientType) );


	// 2012-02-06 add by sunfei

	USE_WEIBOPARSE_OBJ_PTR;

	// referencedUserIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(referencedUserIds)),ptr->referencedUserIds, pmemSize, parse_keda_referencedUserIds);

	// topicIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(topicIds)),ptr->topicIds, pmemSize, parse_keda_topicIds);

	// likeByIds
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(likeByIds)),ptr->likeByIds, pmemSize, parse_keda_likeByIds);

	// pictures
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(pictures)),ptr->pictures, pmemSize, parse_keda_pictures);

	// attachments
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(attachments)),ptr->attachments, pmemSize, parse_keda_attachments);

	// add end




	// isBookMarked
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(isBookMarked)),ptr->isBookMarked,WBPARSER_USE_LEN(isBookMarked) );

	// hot
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(hot)),ptr->hot,WBPARSER_USE_LEN(hot) );

	// title
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(title)),ptr->title,WBPARSER_USE_LEN(title) );

	// isDelete
	GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(isDelete)),ptr->isDelete,WBPARSER_USE_LEN(isDelete) );

	// replyNum
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(replyNum)),ptr->replyNum,WBPARSER_USE_LEN(replyNum) );

	// replyNoLoadNum
	GET_LONG_TO_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(replyNoLoadNum)),ptr->replyNoLoadNum,WBPARSER_USE_LEN(replyNoLoadNum) );


    GET_STR(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(createLikeTime)),ptr->createLikeTime,WBPARSER_USE_LEN(createLikeTime) );
}


t_wbParse_Keda_Replies  *CWBXmlParser::parse_keda_replies(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Replies* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Replies,wbParse_Malloc_Keda_Replies(1));

		*pmemSize += sizeof(t_wbParse_Keda_Replies);

		parse_keda_replies(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_replies(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Replies *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	int idx = 0 ;
	int elementNum = 0;

	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->feedMessNum = elementNum;

	if( elementNum ) 
	{
		ptr->feedMessage = (t_wbParse_Keda_FeedMessage *)wbParse_Malloc_Keda_FeedMessage(elementNum);

		*pmemSize += sizeof(t_wbParse_Keda_FeedMessage) * elementNum;

		if(ptr->feedMessage) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			while( pChild ) {
				parse_keda_feedMessage(pChild,ptr->feedMessage + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Idx_XML(idx,obj);
			}
		}
	}
}


t_wbParse_Keda_SubjectStyleThread  *CWBXmlParser::parse_keda_subjectStyleThread(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_SubjectStyleThread* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_SubjectStyleThread,wbParse_Malloc_Keda_SubjectStyleThread(1));

		*pmemSize += sizeof(t_wbParse_Keda_SubjectStyleThread);

		parse_keda_subjectStyleThread(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_subjectStyleThread(wbParserNS::REQOBJ *obj , t_wbParse_Keda_SubjectStyleThread *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// lastFeedId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(lastFeedId)),ptr->lastFeedId,WBPARSER_USE_LEN(lastFeedId) );


	USE_WEIBOPARSE_OBJ_PTR;

// 	// subject
// 	GET_OBJECT(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(subject)),ptr->subject,parse_keda_subject);
// 
// 	// replies
// 	GET_OBJECT(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(replies)),ptr->replies,parse_keda_replies);

	// subject
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(subject)),ptr->subject, pmemSize, parse_keda_subject);

	// replies
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(replies)),ptr->replies, pmemSize, parse_keda_replies);

}



t_wbParse_Keda_ReplyStyleThread  *CWBXmlParser::parse_keda_replyStyleThread(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_ReplyStyleThread* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_ReplyStyleThread,wbParse_Malloc_Keda_ReplyStyleThread(1));

		*pmemSize += sizeof(t_wbParse_Keda_ReplyStyleThread);

		parse_keda_replyStyleThread(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_replyStyleThread(wbParserNS::REQOBJ *obj , t_wbParse_Keda_ReplyStyleThread *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	// id
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(id)),ptr->id,WBPARSER_USE_LEN(id) );

	// lastFeedId
	GET_LONG_TO_STR( XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(lastFeedId)),ptr->lastFeedId,WBPARSER_USE_LEN(lastFeedId) );


	USE_WEIBOPARSE_OBJ_PTR;

// 	// subject
// 	GET_OBJECT(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(subject)),ptr->subject,parse_keda_subject);
// 
// 	// refReply
// 	GET_OBJECT(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(refReply)),ptr->refReply,parse_keda_refReply);
// 
// 	// reply
// 	GET_OBJECT(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(reply)),ptr->reply,parse_keda_reply);

	// subject
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(subject)),ptr->subject, pmemSize, parse_keda_subject);

	// refReply
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(refReply)),ptr->refReply, pmemSize, parse_keda_refReply);

	// reply
	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(reply)),ptr->reply, pmemSize, parse_keda_reply);

    // createLikeTime
    GET_STR_( XML, obj, "createLikeTime",          ptr->createLikeTime );

}




t_wbParse_Keda_ThreadMessage  *CWBXmlParser::parse_keda_threadMessage(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_ThreadMessage* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_ThreadMessage,wbParse_Malloc_Keda_ThreadMessage(1));

		*pmemSize += sizeof(t_wbParse_Keda_ThreadMessage);

		parse_keda_threadMessage(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_threadMessage(wbParserNS::REQOBJ *obj , t_wbParse_Keda_ThreadMessage *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

	int idx = 0 ;

// 	int elementNum = 0;
// 	GET_OBJECT_SIZE(XML,obj,elementNum);

	ptr->subjectStyleThreadNum = wbParserNS::GetObject_XML_Key_SIZE("subjectStyleThread", obj);

	ptr->replyStyleThreadNum = wbParserNS::GetObject_XML_Key_SIZE("replyStyleThread", obj);;

	if( ptr->subjectStyleThreadNum ) 
	{
		ptr->subjectStyleThread = (t_wbParse_Keda_SubjectStyleThread *)wbParse_Malloc_Keda_SubjectStyleThread(ptr->subjectStyleThreadNum);

		*pmemSize += sizeof(t_wbParse_Keda_SubjectStyleThread) * ptr->subjectStyleThreadNum;

		if(ptr->subjectStyleThread) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML("subjectStyleThread", idx, obj);
			while( pChild ) {
				parse_keda_subjectStyleThread(pChild,ptr->subjectStyleThread + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Key_Idx_XML("subjectStyleThread", idx, obj);
			}
		}
	}

	idx = 0;
	if( ptr->replyStyleThreadNum ) 
	{
		ptr->replyStyleThread = (t_wbParse_Keda_ReplyStyleThread *)wbParse_Malloc_Keda_ReplyStyleThread(ptr->replyStyleThreadNum);

		*pmemSize += sizeof(t_wbParse_Keda_ReplyStyleThread) * ptr->replyStyleThreadNum;

		if(ptr->replyStyleThread) {
			wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML("replyStyleThread", idx, obj);
			while( pChild ) {
				parse_keda_replyStyleThread(pChild,ptr->replyStyleThread + idx, pmemSize);
				++idx;
				pChild = wbParserNS::GetObject_Key_Idx_XML("replyStyleThread", idx, obj);
			}
		}
	}
}


t_wbParse_Keda_Messages  *CWBXmlParser::parse_keda_messages(/*[in]*/wbParserNS::REQOBJ *obj, unsigned long *pmemSize)
{
	t_wbParse_Keda_Messages* ptr = 0;
	if( obj )
	{
		ptr = WEIBO_PARSER_CAST(t_wbParse_Keda_Messages,wbParse_Malloc_Keda_Messages(1));

		*pmemSize += sizeof(t_wbParse_Keda_Messages);

		parse_keda_messages(obj, ptr, pmemSize );
	}
	return ptr;
}

void CWBXmlParser::parse_keda_messages(wbParserNS::REQOBJ *obj , t_wbParse_Keda_Messages *ptr, unsigned long *pmemSize)
{
	if( !ptr || !obj )
		return ;

	USE_WEIBOPARSE_OBJ_PTR;

// 	GET_OBJECT(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(meta)),ptr->meta,parse_keda_meta);
// 
// 	GET_OBJECT(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(threadMessage)),ptr->threadMessage,parse_keda_threadMessage);
// 
// 	GET_OBJECT(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(relatedData)),ptr->relatedData,parse_keda_relatedData);

	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(meta)),ptr->meta, pmemSize, parse_keda_meta);

	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(threadMessage)),ptr->threadMessage, pmemSize, parse_keda_threadMessage);

	GET_OBJECT_SUNFEI(XML,obj,GET_VALUEKEY(WBPARSER_KEY_KEDA_MESS_IDX(relatedData)),ptr->relatedData, pmemSize, parse_keda_relatedData);
}


// add end

// ----------------------------------------








void CWBXmlParser::parse_keda_meeting_list(wbParserNS::REQOBJ *obj , t_wbParse_m_meeting_list * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    REQOBJ * pRelatedData = wbParserNS::GetObject_Key_XML("relatedData",obj) ;
    if ( pRelatedData )
    {
        REQOBJ * pRelatedRooms = wbParserNS::GetObject_Key_XML("relatedRooms",pRelatedData) ;
        if ( pRelatedRooms )
        {
            parse_keda_meeting_short_rooms( pRelatedRooms, &ptr->related_rooms, pmemSize );
        }
    }

    REQOBJ * pMeetings = wbParserNS::GetObject_Key_XML("meetings",obj) ;
    if ( pMeetings )
    {
        parse_keda_meeting_meetings( pMeetings, &ptr->meetings, pmemSize );
    }
}


void CWBXmlParser::parse_keda_meeting_short_rooms(wbParserNS::REQOBJ *obj , t_wbParse_m_short_rooms * ptr, unsigned long *pmemSize,
                                                   const char * szRoomId)
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "room";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_m_short_room *)malloc( ptr->count * sizeof(t_wbParse_m_short_room) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_m_short_room) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_keda_meeting_short_room( pChild, &ptr->items[idx], pmemSize, szRoomId );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}

void CWBXmlParser::parse_keda_meeting_short_room(wbParserNS::REQOBJ *obj , t_wbParse_m_short_room * ptr, unsigned long *pmemSize, 
                                                 const char * szRoomId )
{
    if( !ptr || !obj )
        return ;

    if ( szRoomId )
    {
        GET_LONG( XML, obj, szRoomId,          ptr->id );
    }
    else
    {
        GET_LONG( XML, obj, "id",             ptr->id );
    }

    GET_STR_( XML, obj, "name",        ptr->name );
    GET_STR_( XML, obj, "regionName",  ptr->regionName );
}

void CWBXmlParser::parse_keda_meeting_meetings(wbParserNS::REQOBJ *obj , t_wbParse_m_short_meetings * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "meeting";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_m_short_meeting *)malloc( ptr->count * sizeof(t_wbParse_m_short_meeting) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_m_short_meeting) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_keda_meeting_meeting( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}


void CWBXmlParser::parse_keda_meeting_meeting( wbParserNS::REQOBJ *obj , t_wbParse_m_short_meeting * ptr, 
                                               unsigned long *pmemSize  )
{
    if( !ptr || !obj )
        return ;

    GET_LONG( XML, obj, "id",             ptr->id );    
    GET_STR_( XML, obj, "subject",        ptr->subject );
    GET_STR_( XML, obj, "startTime",      ptr->startTime );
    GET_STR_( XML, obj, "endTime",        ptr->endTime );
    GET_LONG( XML, obj, "status",         ptr->status );
    GET_LONG( XML, obj, "isVideoMeeting", ptr->isVideoMeeting );
    GET_LONG( XML, obj, "feedbackStatus", ptr->feedbackStatus );
    GET_STR_( XML, obj, "organigerMoid",  ptr->organizerMoid );
    GET_LONG( XML, obj, "regularId",      ptr->regularId );

    REQOBJ * pRooms = wbParserNS::GetObject_Key_XML("rooms",obj) ;
    if ( pRooms )
    {
        parse_keda_meeting_room_ids( pRooms, &ptr->room_ids, pmemSize );
    }
}


void CWBXmlParser::parse_keda_meeting_room_ids(wbParserNS::REQOBJ *obj , t_wbParse_m_room_ids * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "int";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (WBParseINT *)malloc( ptr->count * sizeof(WBParseINT) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(WBParseINT) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_keda_meeting_room_id( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}


void CWBXmlParser::parse_keda_meeting_room_id(wbParserNS::REQOBJ *obj , WBParseINT * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    *ptr = wbParserNS::GetInt_XML( obj );
}



void  CWBXmlParser::parse_keda_meeting_persons(wbParserNS::REQOBJ *obj , t_wbParse_m_persons * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "person";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_m_person *)malloc( ptr->count * sizeof(t_wbParse_m_person) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_m_person) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_keda_meeting_person( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}

void  CWBXmlParser::parse_keda_meeting_person(wbParserNS::REQOBJ *obj , t_wbParse_m_person * ptr, unsigned long *pmemSize )
{
    GET_STR_( XML, obj, "moId",                ptr->moId );
    GET_STR_( XML, obj, "entMail",             ptr->email );
    GET_STR_( XML, obj, "name",                ptr->name );
    GET_STR_( XML, obj, "participateNO",       ptr->participantNo );
    GET_LONG( XML, obj, "isParticipated",      ptr->isParticipant );
    GET_LONG( XML, obj, "participateType",     ptr->participantType );
}

void CWBXmlParser::parse_keda_meeting_info(wbParserNS::REQOBJ *obj , t_wbParse_m_meeting_info * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    USE_WEIBOPARSE_OBJ_PTR;

    GET_LONG( XML, obj, "id",               ptr->id );
    GET_STR_( XML, obj, "subject",          ptr->subject );
    GET_STR_( XML, obj, "startTime",        ptr->startTime );
    GET_STR_( XML, obj, "endTime",          ptr->endTime );
    GET_STR_( XML, obj, "organiger",        ptr->organizer );
    GET_STR_( XML, obj, "organigerMail",    ptr->organizerMail );
    GET_STR_( XML, obj, "organigerMoId",    ptr->organizerMoid );
    GET_STR_( XML, obj, "phoneNO",          ptr->phone );
    GET_STR_( XML, obj, "mobilePhone",      ptr->mobile );
    GET_STR_( XML, obj, "brief",            ptr->brief );
    GET_LONG( XML, obj, "isVideoMeeting",   ptr->isVideo );
    GET_LONG( XML, obj, "status",           ptr->status );

    

    REQOBJ * pVMeeting = wbParserNS::GetObject_Key_XML("vMeeting",obj) ;
    if ( pVMeeting )
    {
        GET_STR_( XML, pVMeeting, "e164",             ptr->vMeeting.e164 );
        GET_STR_( XML, pVMeeting, "templateId",       ptr->vMeeting.templateId );
        GET_STR_( XML, pVMeeting, "templateName",     ptr->vMeeting.templateName );
        GET_STR_( XML, pVMeeting, "password",         ptr->vMeeting.password );
        GET_LONG( XML, pVMeeting, "shortNO",          ptr->vMeeting.shortNO );
        GET_LONG( XML, pVMeeting, "state",            ptr->vMeeting.state );
        GET_LONG( XML, pVMeeting, "isAutoCall",       ptr->vMeeting.isAutoCall );    
    }



    REQOBJ * pRooms = wbParserNS::GetObject_Key_XML("rooms",obj) ;
    if ( pRooms )
    {
        parse_keda_meeting_short_rooms( pRooms, &ptr->rooms, pmemSize );
    }   

    REQOBJ * pPersons = wbParserNS::GetObject_Key_XML("persons",obj) ;
    if ( pPersons )
    {
        parse_keda_meeting_persons( pPersons, &ptr->participants, pmemSize );
    }
}

// meeting ids
void CWBXmlParser::parse_keda_meeting_ids(wbParserNS::REQOBJ *obj , t_wbParse_m_meeting_ids * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "int";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (WBParseINT *)malloc( ptr->count * sizeof(WBParseINT) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(WBParseINT) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_keda_meeting_id( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}

// meeting id
void CWBXmlParser::parse_keda_meeting_id(wbParserNS::REQOBJ *obj , WBParseINT * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    *ptr = wbParserNS::GetInt_XML( obj );
}

void CWBXmlParser::parse_keda_regular(wbParserNS::REQOBJ *obj , t_wbParse_m_regular_info * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    USE_WEIBOPARSE_OBJ_PTR;

    GET_LONG( XML, obj, "id",               ptr->id );
    GET_STR_( XML, obj, "subject",          ptr->subject );
    GET_STR_( XML, obj, "organiger",        ptr->organizer );
    GET_STR_( XML, obj, "organigerMail",    ptr->organizerMail );
    GET_STR_( XML, obj, "phoneNO",          ptr->phone );
    GET_STR_( XML, obj, "brief",            ptr->brief );
    GET_LONG( XML, obj, "status",           ptr->status );
    GET_STR_( XML, obj, "mobilePhone",      ptr->mobile );
    GET_LONG( XML, obj, "isVideoMeeting",   ptr->isVideo );
    GET_STR_( XML, obj, "startTime",        ptr->startTime );
    GET_STR_( XML, obj, "endTime",          ptr->endTime );
    GET_STR_( XML, obj, "startDate",        ptr->startDate );
    GET_STR_( XML, obj, "endDate",          ptr->endDate );

    GET_LONG( XML, obj, "repeatType",       ptr->repeatType );
    GET_LONG( XML, obj, "frequency",        ptr->frequency );
    GET_STR_( XML, obj, "weekDays",         ptr->weekDays );


    REQOBJ * pMeetings = wbParserNS::GetObject_Key_XML("meetings",obj) ;
    if ( pMeetings )
    {
        parse_keda_meeting_ids( pMeetings, &ptr->meeting_ids, pmemSize );
    }   


}





void CWBXmlParser::parse_keda_m_room(wbParserNS::REQOBJ *obj , t_wbParse_m_room * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    GET_LONG( XML, obj,  "id",            ptr->id );
    GET_STR_( XML, obj,  "name",          ptr->name );
    GET_STR_( XML, obj,  "regionName",    ptr->regionName );
    GET_STR_( XML, obj,  "startTime",     ptr->startTime );
    GET_STR_( XML, obj,  "endTime",       ptr->endTime );
    GET_LONG( XML, obj,  "peopleAdmit",   ptr->peopleAdmit );

    // 设备号
    ptr->deviceMask = 0;
    
    WBParseCHAR  deviceIds[1024] = { 0 };
    char *       pStart     = 0;
    char *       pDelimiter = 0;

    GET_STR_( XML, obj,  "deviceIds", deviceIds );
    pStart = deviceIds;
    pDelimiter = strchr( pStart, ',' );

    while( pStart )
    {
        char  tmp[1024] = { 0 };
        int   len = 0;

        if ( pDelimiter )
        {
            len = pDelimiter - pStart;
        }
        else
        {
            len = strlen( pStart );
        }
        
        if ( len > 0 )
        {
            int nId = 0;
            memcpy( tmp, pStart, len );
            if ( sscanf( tmp, " %d", &nId ) > 0 && nId > 0 )
            {
                ::SetBit( ptr->deviceMask, nId - 1 );
            }
        }

        if ( pDelimiter )
        {
            pStart = pDelimiter + 1;
            pDelimiter = strchr( pStart, ',' );
        }
        else
        {
            pStart = 0;
        }
    }
}

void CWBXmlParser::parse_keda_m_rooms(wbParserNS::REQOBJ *obj , t_wbParse_m_rooms * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "room";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_m_room *)malloc( ptr->count * sizeof(t_wbParse_m_room) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_m_room) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_keda_m_room( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}



void CWBXmlParser::parse_keda_m_room_list(wbParserNS::REQOBJ *obj , t_wbParse_m_room_list * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    GET_LONG( XML, obj,  "total",            ptr->total );

    REQOBJ * pRooms = wbParserNS::GetObject_Key_XML("rooms",obj) ;
    if ( pRooms )
    {
        parse_keda_m_rooms( pRooms, &ptr->rooms, pmemSize );
    }
}




void CWBXmlParser::parse_keda_m_short_room( wbParserNS::REQOBJ *obj , t_wbParse_m_short_room * ptr, unsigned long *pmemSize,
                                            const TXmlChildNamesParam * pChildAttrs )
{
    if( !ptr || !obj || !pChildAttrs || pChildAttrs->dwCnt != 2 )
        return ;

    GET_LONG( XML, obj, pChildAttrs->szChildNames[0],       ptr->id );
    GET_STR_( XML, obj, pChildAttrs->szChildNames[1],       ptr->name );
}

void CWBXmlParser::parse_keda_m_short_rooms(wbParserNS::REQOBJ *obj , t_wbParse_m_short_rooms * ptr, unsigned long *pmemSize, 
                                            const char * szChildName, const TXmlChildNamesParam * pChildAttrs)
{
    if( !ptr || !obj || !szChildName || !pChildAttrs || !pChildAttrs->dwCnt )
        return ;

    int   idx              = 0 ;
    const char * szItem    = szChildName;

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_m_short_room *)malloc( ptr->count * sizeof(t_wbParse_m_short_room) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_m_short_room) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_keda_m_short_room( pChild, &ptr->items[idx], pmemSize, pChildAttrs );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}


void CWBXmlParser::parse_keda_m_normal_room_list(wbParserNS::REQOBJ *obj , t_wbParse_m_normal_room_list * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;


    GET_LONG( XML, obj, "start",         ptr->start );
    GET_LONG( XML, obj, "count",         ptr->count );

    REQOBJ * pData = wbParserNS::GetObject_Key_XML("data",obj) ;
    if ( pData )
    {
        TXmlChildNamesParam  tChildAttr;
        tChildAttr.dwCnt = 2;
        tChildAttr.szChildNames[0]    = "meetingRoomId";
        tChildAttr.szChildNames[1]    = "meetingRoomName";

        parse_keda_m_short_rooms( pData, &ptr->rooms, pmemSize, "meetingRoom", &tChildAttr );
    }
}





void CWBXmlParser::parse_keda_room_info(wbParserNS::REQOBJ *obj , t_wbParse_m_room_info * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    GET_LONG( XML, obj,   "id",                    ptr->id );
    GET_LONG( XML, obj,   "peopleAdmit",           ptr->peopleAdmit );
    GET_LONG( XML, obj,   "area",                  ptr->area );
    GET_LONG( XML, obj,   "video",                 ptr->hasVideo );
    GET_STR_( XML, obj,   "name",                  ptr->name );
    GET_STR_( XML, obj,   "phoneNO",               ptr->phone );
    GET_STR_( XML, obj,   "e164",                  ptr->e164 );
    GET_STR_( XML, obj,   "deviceAdmin",           ptr->deviceAdmin );
    GET_STR_( XML, obj,   "adminPhoneNO",          ptr->adminPhone );
    GET_STR_( XML, obj,   "remark",                ptr->remark );
    ptr->deviceMask = 0;

    REQOBJ * pDevices = wbParserNS::GetObject_Key_XML("deviceIds",obj) ;
    if ( pDevices )
    {
        int   idx = 0 ;
        const char * szItem = "int";

        wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, pDevices);
        while( pChild ) 
        {

            long n = wbParserNS::GetInt_XML( pChild );
            if ( 1 == n )
            {
                ptr->deviceMask |= DEVICE_ID_VIDEO_TERMINAL;
            }
            else if ( 2 == n )
            {
                ptr->deviceMask |= DEVICE_ID_PROJECTOR;
            }
            else if ( 3 == n )
            {
                ptr->deviceMask |= DEVICE_ID_AIR_CONDITIONING;
            }
            else if ( 4 == n )
            {
                ptr->deviceMask |= DEVICE_ID_WHITE_BORD;
            }

            ++idx;
            pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, pDevices);
        }
    }
}


void CWBXmlParser::parse_keda_room_status(wbParserNS::REQOBJ *obj , t_wbParse_m_room_status * ptr, unsigned long *pmemSize)

{
    if( !ptr || !obj )
        return ;

    GET_LONG( XML, obj,   "meetingRoomID",             ptr->id );
    GET_STR_( XML, obj,   "meetingRoomName",           ptr->name );
    GET_LONG( XML, obj,   "status",                    ptr->status );
}


void CWBXmlParser::parse_keda_room_by_name(wbParserNS::REQOBJ *obj , t_wbParse_m_short_rooms * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;


    REQOBJ * pRooms = wbParserNS::GetObject_Key_XML("rooms",obj) ;
    if ( pRooms )
    {
        TXmlChildNamesParam  tChildAttr;
        tChildAttr.dwCnt = 2;
        tChildAttr.szChildNames[0]    = "id";
        tChildAttr.szChildNames[1]    = "name";

        parse_keda_m_short_rooms( pRooms, ptr, pmemSize, "room", &tChildAttr );
    }
}

void CWBXmlParser::parse_keda_template(wbParserNS::REQOBJ *obj , t_wbParse_m_template * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    GET_STR_( XML, obj,   "id",         ptr->id );
    GET_STR_( XML, obj,   "name",       ptr->name );
}

void CWBXmlParser::parse_keda_templates(wbParserNS::REQOBJ *obj , t_wbParse_m_templates * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "template";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_m_template *)malloc( ptr->count * sizeof(t_wbParse_m_template) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_m_template) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_keda_template( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}

void CWBXmlParser::parse_keda_template_list(wbParserNS::REQOBJ *obj , t_wbParse_m_templateList * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    REQOBJ * pTemplates = wbParserNS::GetObject_Key_XML("templates",obj) ;
    if ( pTemplates )
    {
        parse_keda_templates( pTemplates, &ptr->templates, pmemSize );
    }
}

//void CWBXmlParser::parse_keda_m_person(wbParserNS::REQOBJ *obj , t_wbParse_m_person * ptr, unsigned long *pmemSize)
//{
//    if( !ptr || !obj )
//        return ;
//
//    GET_STR_( XML, obj,  "entMail",         ptr->email );
//    GET_STR_( XML, obj,  "name",            ptr->name  );
//    GET_STR_( XML, obj,  "moId",            ptr->moId );
//    GET_STR_( XML, obj,  "participateNO",   ptr->participantNo );
//    GET_LONG( XML, obj,  "isParticipated",  ptr->isParticipant );
//    GET_LONG( XML, obj,  "participateType", ptr->participantType );
//}
//
//void  CWBXmlParser::parse_keda_m_persons( wbParserNS::REQOBJ *obj , t_wbParse_m_persons * ptr, unsigned long *pmemSize )
//{
//    if( !ptr || !obj )
//        return ;
//
//    int   idx              = 0 ;
//    const char * szItem    = "person";
//
//    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
//    if ( ptr->count )
//    {
//        ptr->items = (t_wbParse_m_person *)malloc( ptr->count * sizeof(t_wbParse_m_person) );
//        if ( ptr->items )
//        {
//            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_m_person) );
//            
//            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
//            while( pChild ) 
//            {
//                parse_keda_m_person( pChild, &ptr->items[idx], pmemSize );
//                
//                ++idx;
//                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
//            }
//        }
//    }
//}


void CWBXmlParser::parse_keda_template_info(wbParserNS::REQOBJ *obj , t_wbParse_m_template_info * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    GET_STR_( XML, obj, "id",                 ptr->id );
    GET_STR_( XML, obj, "name",               ptr->name );
    GET_STR_( XML, obj, "resolution",         ptr->resolution );
    GET_STR_( XML, obj, "bandwidth",          ptr->bandwith );

    REQOBJ * pPersons= wbParserNS::GetObject_Key_XML("persons",obj) ;
    if ( pPersons )
    {
        parse_keda_meeting_persons( pPersons, &ptr->persons, pmemSize );
    }
}

void CWBXmlParser::parse_keda_notify_content_meeting( wbParserNS::REQOBJ *obj , t_wbParse_m_notify_content_meeting * ptr )
{
    if( !ptr || !obj )
        return ;

    GET_LONG( XML, obj, "meetingId",        ptr->id );
    GET_STR_( XML, obj, "subject",          ptr->subject );
    GET_STR_( XML, obj, "startTime",        ptr->startTime );
    GET_STR_( XML, obj, "endTime",          ptr->endTime );
    GET_STR_( XML, obj, "phoneNO",          ptr->phone );
    GET_STR_( XML, obj, "mobilePhone",      ptr->mobilePhone );
    GET_LONG( XML, obj, "isVideoMeeting",   ptr->isVideoMeeting );
    GET_STR_( XML, obj, "organigerMoid",    ptr->organigerMoid );
    GET_STR_( XML, obj, "organiger",        ptr->organiger );
    GET_STR_( XML, obj, "beforeSubject",    ptr->beforeSubject );

    REQOBJ * pRooms = wbParserNS::GetObject_Key_XML("rooms",obj) ;
    if ( pRooms )
    {
        parse_keda_meeting_short_rooms( pRooms, &ptr->rooms, 0, "roomId" );
    }   
}

void CWBXmlParser::parse_keda_notify_content( wbParserNS::REQOBJ *obj , void * ptr, int nType )
{
    if( !ptr || !obj || !(nType>=102 && nType<=119) )
        return ;

    switch( nType )
    {
    case 102:
    case 103:
        {
            t_wbParse_m_notify_content_meeting * p = (t_wbParse_m_notify_content_meeting *)ptr;
            parse_keda_notify_content_meeting( obj, p );
        }
        break;

    case 104:
    case 108:
    case 109:
    case 115:
    case 116:
    case 117:
    case 118:
    case 119:
        {
            t_wbParse_m_notify_content_meeting_id * p = (t_wbParse_m_notify_content_meeting_id *)ptr;
            GET_LONG( XML, obj, "meetingId",        p->meetingId );

            GET_STR_( XML, obj, "subject",          p->subject );
            GET_STR_( XML, obj, "startTime",        p->startTime );
            GET_STR_( XML, obj, "endTime",          p->endTime );
            GET_STR_( XML, obj, "organigerMoid",    p->organigerMoid );
            
        }
        break;

    case 105:
    case 106:
    case 107:
        {
            t_wbParse_m_notify_content_regular_id * p = (t_wbParse_m_notify_content_regular_id *)ptr;
            GET_LONG( XML, obj, "regularId",        p->regularId );
            GET_STR_( XML, obj, "organigerMoid",    p->organigerMoid );
        }
        break;

    case 110:
        {
            t_wbParse_m_notify_content_cancel_room * p = (t_wbParse_m_notify_content_cancel_room *)ptr;
            GET_LONG( XML, obj, "meetingId",        p->meetingId );
            GET_STR_( XML, obj, "subject",          p->subject );
            GET_STR_( XML, obj, "startTime",        p->startTime );
            GET_STR_( XML, obj, "endTime",          p->endTime );
            GET_STR_( XML, obj, "organigerMoid",    p->organigerMoid );

            REQOBJ * pRooms = wbParserNS::GetObject_Key_XML("rooms",obj) ;
            if ( pRooms )
            {
                parse_keda_meeting_short_rooms( pRooms, &p->rooms );
            }
        }
        break;

    case 111:
        {
            t_wbParse_m_notify_content_deadline_changed * p = (t_wbParse_m_notify_content_deadline_changed *)ptr;
            GET_LONG( XML, obj, "headline",        p->headline );
        }
        break;

    case 112:
        {
            t_wbParse_m_notify_content_user_feedback * p = (t_wbParse_m_notify_content_user_feedback *)ptr;

            GET_LONG( XML, obj, "meetingId",           p->meetingId );
            GET_STR_( XML, obj, "moId",                p->participant.moId );
            GET_LONG( XML, obj, "participateType",     p->participant.participantType );
            GET_LONG( XML, obj, "isParticipated",      p->participant.isParticipant );

            
            GET_LONG( XML, obj, "reasonId",            p->reasonId );
            GET_LONG( XML, obj, "isVideoMeeting",      p->isVideoMeeting );
        }
        break;

    case 113:
    case 114:
        {
            t_wbParse_m_notify_content_room_changed * p = (t_wbParse_m_notify_content_room_changed *)ptr;
            GET_LONG( XML, obj, "roomId",           p->roomId );
        }
        break;

    default:
        break;
    }
}

void CWBXmlParser::parse_keda_notify_item(wbParserNS::REQOBJ *obj , t_wbParse_m_notify * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    GET_LONG( XML, obj, "id",                 ptr->id );
    GET_STR_( XML, obj, "receiverMoId",       ptr->receiverMoId );
    GET_LONG( XML, obj, "messgaeType",        ptr->messgaeType );
    GET_STR_( XML, obj, "companyMoId",        ptr->companyMoId );
    GET_STR_( XML, obj, "createTime",         ptr->createTime );

    // 解析content
    size_t   dwSize   = 0;
    REQOBJ * pContent = wbParserNS::GetObject_Key_XML("content",obj) ;
    if ( pContent )
    {
        switch( ptr->messgaeType )
        {
        case 102:
        case 103:
            dwSize = sizeof(t_wbParse_m_notify_content_meeting);
            break;

        case 104:
        case 108:
        case 109:
        case 115:
        case 116:
        case 117:
        case 118:
        case 119:
            dwSize = sizeof(t_wbParse_m_notify_content_meeting_id);
            break;

        case 105:
        case 106:
        case 107:
            dwSize = sizeof(t_wbParse_m_notify_content_regular_id);
            break;

        case 110:
            dwSize = sizeof(t_wbParse_m_notify_content_cancel_room);
            break;

        case 111:
            dwSize = sizeof(t_wbParse_m_notify_content_deadline_changed);
            break;

        case 112:
            dwSize = sizeof(t_wbParse_m_notify_content_user_feedback);
            break;

        case 113:
        case 114:
            dwSize = sizeof(t_wbParse_m_notify_content_room_changed);
            break;

        default:
            break;
        }
    }

    ptr->content = malloc( dwSize );
    if ( ptr->content )
    {
        memset( ptr->content, 0, dwSize );
        parse_keda_notify_content( pContent, ptr->content, ptr->messgaeType );
    }
}

void CWBXmlParser::parse_keda_list_notify_by_max(wbParserNS::REQOBJ *obj , t_wbParse_m_notifys * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "notify";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_m_notify *)malloc( ptr->count * sizeof(t_wbParse_m_notify) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_m_notify) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_keda_notify_item( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}


void CWBXmlParser::parse_app_all_notifys(wbParserNS::REQOBJ *obj , t_wbParse_app_notifys * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "notify";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_app_notify *)malloc( ptr->count * sizeof(t_wbParse_app_notify) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_app_notify) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_app_notify_item( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}

void CWBXmlParser::parse_app_notify_item(wbParserNS::REQOBJ *obj , t_wbParse_app_notify * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    GET_LONG( XML, obj, "amount",           ptr->amount );
    GET_STR_( XML, obj, "messageType",      ptr->messageType );
}


void CWBXmlParser::parse_monitor_group( wbParserNS::REQOBJ *obj , t_wbParse_monitor_group * ptr, unsigned long *pmemSize )
{
    if( !ptr || !obj )
        return ;

    GET_STR_( XML, obj, "id",               ptr->id );
    GET_STR_( XML, obj, "groupId",          ptr->groupId );
    GET_STR_( XML, obj, "groupName",        ptr->groupName );
    GET_STR_( XML, obj, "parentId",         ptr->parentId );
    GET_STR_( XML, obj, "newGroup",         ptr->newGroup );
    GET_STR_( XML, obj, "leaf",             ptr->leaf );

    REQOBJ * pDevices = wbParserNS::GetObject_Key_XML("devices",obj) ;
    if ( pDevices )
    {
        parse_monitor_devices( pDevices, &ptr->devices, 0 );
    }
}

void CWBXmlParser::parse_monitor_devices(wbParserNS::REQOBJ *obj , t_wbParse_monitor_devices * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "device";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_monitor_device *)malloc( ptr->count * sizeof(t_wbParse_monitor_device) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_monitor_device) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_monitor_device( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}

void CWBXmlParser::parse_monitor_device(wbParserNS::REQOBJ *obj , t_wbParse_monitor_device * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    GET_STR_( XML, obj, "id",                   ptr->id );
    GET_STR_( XML, obj, "name",                 ptr->name );
    GET_STR_( XML, obj, "deviceId",             ptr->deviceId );
    GET_STR_( XML, obj, "deviceGUID",           ptr->deviceGUID );
    GET_STR_( XML, obj, "e164",                 ptr->e164 );
    GET_STR_( XML, obj, "deviceType",           ptr->deviceType );
    GET_STR_( XML, obj, "description",          ptr->description );
}


void CWBXmlParser::parse_monitor_groups(wbParserNS::REQOBJ *obj , t_wbParse_monitor_groups * ptr, unsigned long *pmemSize)
{
    if( !ptr || !obj )
        return ;

    int   idx              = 0 ;
    const char * szItem    = "group";

    ptr->count = wbParserNS::GetObject_XML_Key_SIZE(szItem, obj);
    if ( ptr->count )
    {
        ptr->items = (t_wbParse_monitor_group *)malloc( ptr->count * sizeof(t_wbParse_monitor_group) );
        if ( ptr->items )
        {
            memset( ptr->items, 0, ptr->count * sizeof(t_wbParse_monitor_group) );

            wbParserNS::REQOBJ *pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            while( pChild ) 
            {
                parse_monitor_group( pChild, &ptr->items[idx], pmemSize );

                ++idx;
                pChild = wbParserNS::GetObject_Key_Idx_XML(szItem, idx, obj);
            }
        }
    }
}