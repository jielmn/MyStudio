#ifndef __WB_PARSE_STRUCT_H__
#define __WB_PARSE_STRUCT_H__

#include <string.h>
#include <stdlib.h>

#define WTRUE  true
#define WFALSE false

#ifndef BUILD_CHARACTER_SET
typedef char WBParseCHAR;
#	define _WBC(c) c
#	define WBPARSER_COPY(dst,size,source,srcsize) strncpy(dst,source,size)
#else

typedef TCHAR WBParseCHAR;

#	define _WBC(c) _T(c)
#	define WBPARSER_COPY(dst,size,source,srcsize) _tcsncpy_s(dst,size,source,srcsize)

#endif // BUILD_CHARACTER_SET

typedef int   WBParseINT;
typedef long  WBParseLONG;
typedef bool  WBParseBOOL;


namespace wbParserNS
{

#define WBPARSER_UINT_DEFINE(name,val)     const unsigned int WBPARSER_##name##_LEN = val
#define WBPARSER_USE_LEN(name)             WBPARSER_##name##_LEN
#define WBPARSER_REAL_LEN(name)            WBPARSER_##name##_LEN + 10 ///< 实际的长度
#define WBPARSER_OPTION_TYPE(name)         WBPT_OPTION_TYPE_##name    ///< 解析的类型
#define WBPARSER_INFOR_TYPE(name)		   WBPT_INFOR_TYPE_##name     ///< 结构的类型


	// 基本信息
	WBPARSER_UINT_DEFINE(text,2048);             ///< 微博信息内容

	WBPARSER_UINT_DEFINE(messageType,30);        ///< 新消息类型

	WBPARSER_UINT_DEFINE(departmentName,200);    ///< 组织架构-部门名称
	WBPARSER_UINT_DEFINE(fullPath,1000);         ///< 组织架构-部门全路径
	WBPARSER_UINT_DEFINE(deptPosition,100);      ///< 组织架构-部门职位

	WBPARSER_UINT_DEFINE(wbOnline,10);           ///< 微博在线状态
	WBPARSER_UINT_DEFINE(tlOnline,10);           ///< TrueLink在线状态
	WBPARSER_UINT_DEFINE(tdOnline,10);           ///< pad的在线状态(ios/android)
	WBPARSER_UINT_DEFINE(tpOnline,10);           ///< phone的在线状态(ios/android)
	WBPARSER_UINT_DEFINE(tsOnline,10);           ///< TrueSens在线状态

	WBPARSER_UINT_DEFINE(entName,100);            ///< 企业目录名字
	WBPARSER_UINT_DEFINE(entRealName,100);        ///< 企业名字
	WBPARSER_UINT_DEFINE(cn,50);                  ///< RDN,唯一名称
	WBPARSER_UINT_DEFINE(sn,50);                  ///< 真实姓名,可重名
	WBPARSER_UINT_DEFINE(enable,5);               ///< 禁用/启用，0/1
	WBPARSER_UINT_DEFINE(privileged,5);           ///< 特权,标识管理员，0/1
	WBPARSER_UINT_DEFINE(entMail,100);            ///< 企业邮箱,微博登录账号
	WBPARSER_UINT_DEFINE(subMail,100);            ///< 订阅邮箱地址,邮件发送
	WBPARSER_UINT_DEFINE(e164,16);                ///< E164号
	WBPARSER_UINT_DEFINE(jid,64);                 ///< xmpp账号
	WBPARSER_UINT_DEFINE(mobileNum,30);           ///< 联系电话
	WBPARSER_UINT_DEFINE(extNum,30);              ///< 分机
	WBPARSER_UINT_DEFINE(seat,30);                ///< 座位号
	WBPARSER_UINT_DEFINE(brief,220);              ///< 一句话介绍(70个汉字)
	WBPARSER_UINT_DEFINE(position,30);            ///< 职位
	WBPARSER_UINT_DEFINE(portrait32,256);         ///< 32位头像
	WBPARSER_UINT_DEFINE(portrait40,256);         ///< 40位头像
	WBPARSER_UINT_DEFINE(portrait64,256);         ///< 64位头像
	WBPARSER_UINT_DEFINE(portrait128,256);        ///< 128位头像
	WBPARSER_UINT_DEFINE(portrait256,256);        ///< 256位头像
	WBPARSER_UINT_DEFINE(password,40);            ///< XMPP登陆密码

	WBPARSER_UINT_DEFINE(enableWeibo,5);          ///< 微博权限(0/1,没有/有)
	WBPARSER_UINT_DEFINE(enableMeeting,5);        ///< 会议管理权限(0/1,没有/有)
	WBPARSER_UINT_DEFINE(enableHD,5);             ///< 高清权限(0/1,没有/有)
	WBPARSER_UINT_DEFINE(enableCall,5);           ///< 电话呼叫权限(0/1,没有/有)
	WBPARSER_UINT_DEFINE(enableRoam,5);           ///< 漫游权限(0/1,没有/有)
	WBPARSER_UINT_DEFINE(weiboAdmin,5);           ///< 微博管理员权限(0/1,没有/有)
	WBPARSER_UINT_DEFINE(meetingAdmin,5);         ///< 会议管理员权限(0/1,没有/有)
	WBPARSER_UINT_DEFINE(isLikeQuery,5);          ///< 是否支持模糊搜索(0:不可以；1:可以)
	WBPARSER_UINT_DEFINE(authMode,5);             ///< 添加联系人认证模式(0:无需认证；1:待认证；2:拒绝接受)
	WBPARSER_UINT_DEFINE(sex,5);                  ///< 性别(1/0,男/女)

	WBPARSER_UINT_DEFINE(xmppDomain,30);          ///< xmpp域名
	WBPARSER_UINT_DEFINE(apiUrl,100);             ///< 平台API地址
	WBPARSER_UINT_DEFINE(weiboUrl,100);           ///< 微博API地址
	WBPARSER_UINT_DEFINE(webImUrl,100);           ///< web客户端连接XMPP的web服务地址(终端不需要)
	WBPARSER_UINT_DEFINE(tcpImUrl,100);           ///< XMPP服务地址
	WBPARSER_UINT_DEFINE(tcpPort,10);             ///< XMPP服务端口
	WBPARSER_UINT_DEFINE(ulsUrl,100);             ///< ULS地址
	WBPARSER_UINT_DEFINE(ulsPort,10);             ///< ULS端口

	WBPARSER_UINT_DEFINE(content,900);                   ///< 私信内容(300个汉字)

	WBPARSER_UINT_DEFINE(feedCount,10);                  ///< 话题的消息数量
	WBPARSER_UINT_DEFINE(userName,60);                   ///< 用户名字
	WBPARSER_UINT_DEFINE(feedId,10);                     ///< 赞的消息ID
	WBPARSER_UINT_DEFINE(groupUid,20);                   ///< 组uid
	WBPARSER_UINT_DEFINE(groupBriefly,200);              ///< 组简介

	WBPARSER_UINT_DEFINE(leaf,5);                        ///< 是否含有子节点：ture：没有；false：有

	WBPARSER_UINT_DEFINE(mugshot128,256);                     ///< 用户头像地址
	WBPARSER_UINT_DEFINE(mugshot64,256);                      ///< 用户头像地址
	WBPARSER_UINT_DEFINE(mugshot40,256);                      ///< 用户头像地址
	WBPARSER_UINT_DEFINE(personalBriefly,210);               ///< 一句话介绍(70个汉字)
	WBPARSER_UINT_DEFINE(uid,30);                            ///< 用户UID/组UID
	WBPARSER_UINT_DEFINE(name,60);                           ///< 用户名字/组名字/话题名字
	WBPARSER_UINT_DEFINE(email,50);                          ///< 用户email(账号)
	WBPARSER_UINT_DEFINE(defaultUid,30);                     ///< 系统默认赋予的用户唯一标识（暂没有使用）
	WBPARSER_UINT_DEFINE(online,5);                          ///< 是否在线
	WBPARSER_UINT_DEFINE(beenFollowBoth,5);                  ///< 是否相关关注

	WBPARSER_UINT_DEFINE(userId,64);                         ///< 用户ID
	WBPARSER_UINT_DEFINE(jobTitle,60);                       ///< 职位
	WBPARSER_UINT_DEFINE(location,20);                       ///< 座位
	WBPARSER_UINT_DEFINE(workPhone,30);                      ///< 工作电话
	WBPARSER_UINT_DEFINE(mobilePhone,20);                    ///< 手机

	WBPARSER_UINT_DEFINE(id,64);				    	     ///< 消息ID/用户ID/组ID/话题ID
	WBPARSER_UINT_DEFINE(body,1500);                         ///< 主题的内容(最多280个字符 1汉字=2字符)
	WBPARSER_UINT_DEFINE(threadId,10);                       ///< 主题ID
	WBPARSER_UINT_DEFINE(directMessage,5);                   ///< 是否是私有的消息
	WBPARSER_UINT_DEFINE(broadcast,5);                       ///< 是否广播
	WBPARSER_UINT_DEFINE(systemMessage,5);                   ///< 是否是系统消息
	WBPARSER_UINT_DEFINE(feedType,10);                       ///< 消息类型
	WBPARSER_UINT_DEFINE(createdBy,64);                      ///< 创建人
	WBPARSER_UINT_DEFINE(createdAt,20);                      ///< 创建时间
	WBPARSER_UINT_DEFINE(clientType,30);                     ///< 消息来源于哪种客户端
	WBPARSER_UINT_DEFINE(type,20);                           ///< 动作类型(create/destroy)

	WBPARSER_UINT_DEFINE(isBookMarked,5);                    ///< 主题是否被收藏
	WBPARSER_UINT_DEFINE(hot,10);                            ///< 热度
	WBPARSER_UINT_DEFINE(title,150);                         ///< 长篇大论标题
	WBPARSER_UINT_DEFINE(isDelete,5);                        ///< 是否删除

	WBPARSER_UINT_DEFINE(repliedToId,10);                    ///< 回复的那条消息的ID
	WBPARSER_UINT_DEFINE(repliedToBody,1500);                ///< 回复的那条消息的内容(最多280个字符 1汉字=2字符)
	WBPARSER_UINT_DEFINE(repliedToUserId,64);                ///< 消息回复的那个人员的ID

	WBPARSER_UINT_DEFINE(lastFeedId,10);                     ///< 当前主题相关消息的最后消息ID

	WBPARSER_UINT_DEFINE(readFeedId,10);                     ///< 返回的消息中最新的消息ID
	WBPARSER_UINT_DEFINE(oldestFeedId,10);                   ///< 返回的消息中最后主题的最后回复ID
	WBPARSER_UINT_DEFINE(totalCount,10);                     ///< 总的数量

	WBPARSER_UINT_DEFINE(groupId,10);                        ///< 组ID
	WBPARSER_UINT_DEFINE(topicId,10);                        ///< 话题ID

	WBPARSER_UINT_DEFINE(extensionName,10);                  ///< 图片/附件的后缀名
	WBPARSER_UINT_DEFINE(picturePreviewUrl,256);             ///< 大图像的保存相对地址
	WBPARSER_UINT_DEFINE(pictureTinyUrl,256);                ///< 小图像的保存相对地址
	WBPARSER_UINT_DEFINE(pictureMobileUrl,256);              ///< Mobile用图像的保存相对地址
	WBPARSER_UINT_DEFINE(pictureName,256);                   ///< 图片名称
	WBPARSER_UINT_DEFINE(pictureUrl,256);                    ///< 原图片地址

	WBPARSER_UINT_DEFINE(fileName,256);                      ///< 附件名称
	WBPARSER_UINT_DEFINE(fileUrl,256);                       ///< 附件保存的相对地址

	WBPARSER_UINT_DEFINE(replyNum,5);                        ///< 回复的个数
	WBPARSER_UINT_DEFINE(replyNoLoadNum,5);                  ///< 未加载的个数
	WBPARSER_UINT_DEFINE(beenFollowed,5);                    ///< 是否关注
	// add end

    WBPARSER_UINT_DEFINE(createLikeTime, 32);


	WBPARSER_UINT_DEFINE(url,1024);
	WBPARSER_UINT_DEFINE(created_time,255);

	WBPARSER_UINT_DEFINE(request,255);
	WBPARSER_UINT_DEFINE(error,32);            ///< 错误描述
	WBPARSER_UINT_DEFINE(error_code,16);       ///< 错误码
	WBPARSER_UINT_DEFINE(TGT,255);



    WBPARSER_UINT_DEFINE(subject,   100);
    WBPARSER_UINT_DEFINE(datetime,  20);
    WBPARSER_UINT_DEFINE(organizer, 30);
    WBPARSER_UINT_DEFINE(organizerMail, 50);
    WBPARSER_UINT_DEFINE(phone,   30);
    WBPARSER_UINT_DEFINE(m_brief, 1500);
    WBPARSER_UINT_DEFINE(mobile,  20);
    WBPARSER_UINT_DEFINE(template_id,  20);
    WBPARSER_UINT_DEFINE(template_name,  64);
    WBPARSER_UINT_DEFINE(participantNo,  20);
    WBPARSER_UINT_DEFINE(remark,  100);
    WBPARSER_UINT_DEFINE(common,  64);
    WBPARSER_UINT_DEFINE(moId,    64);
    WBPARSER_UINT_DEFINE(regionName,    50);
    WBPARSER_UINT_DEFINE(companyMoId,    64);

	

	struct t_wbParse_Error;

	typedef enum 
	{
		EPS_OPENAPI ,  ///< from OpenAPI Http 
		EPS_STREAM  ,  ///< from STREAM
		EPS_XMPP    ,  ///< from xmpp

	} eParseSource;

	/** 解析信息头，每个结构体必须要有的 by welbon,2011-01-08*/
	struct t_wbParse_Header
	{
		int eSource;
	};
	

	/// 认证返回信息
	struct t_wbParse_Oauth
	{
		t_wbParse_Header ph_;

		char oauth_token[ 257 ];
		char oauth_token_secret[ 257 ];

		char uid_[128];

		char oauth_verifier[ 65 ];
	};

	/** cookie 信息 */
	struct t_wbParse_Cookie
	{
		t_wbParse_Header ph_; ///< 解析头

		// cookie
		int  error;
		char SUE[1024];
		char SUP[2048];
		char tgt[WBPARSER_REAL_LEN(TGT)];
		//
		char ticket[256];
		char publickey[512];
		char keyversion[64];
		char uid[128];

#ifdef _MSC_VER
		__int64 lservertime;//获取TGT时服务器时间
		__int64 llocaltime;//TGT返回时本机时间
#else
		long long lservertime;//获取TGT时服务器时间
		long long llocaltime;//TGT返回时本机时间
#endif
	};


	/** 更新TGT结果 */
	struct t_wbParse_updateTGT
	{
		t_wbParse_Header ph_; ///< 解析头

		int  error;
		char SUE[1024];
		char SUP[2048];
		char tgt[WBPARSER_REAL_LEN(TGT)];///< TGT	
	};

	struct t_wbParse_Cursor
	{
		t_wbParse_Header ph_; ///< 解析头

#ifdef _MSC_VER
		__int64 nextcur;
		__int64 prevcur;
#else
		long long nextcur;
		long long prevcur;
#endif

	};


// -------------------------------------

// 2012-03-07 update by sunfei <result> 使用的数据结构

	struct t_wbParse_Keda_Depts;

	struct t_wbParse_Keda_EntUser
	{
        WBParseCHAR moId[WBPARSER_REAL_LEN(moId)];                        ///< 
        WBParseCHAR companyMoId[WBPARSER_REAL_LEN(companyMoId)];          ///< 

		WBParseCHAR entName[WBPARSER_REAL_LEN(entName)];          ///< 企业目录名
		WBParseCHAR entRealName[WBPARSER_REAL_LEN(entRealName)];  ///< 企业名
		WBParseCHAR cn[WBPARSER_REAL_LEN(cn)];                    ///< RDN,唯一名称
		WBParseCHAR sn[WBPARSER_REAL_LEN(sn)];                    ///< 真实姓名,可重名
		WBParseCHAR enable[WBPARSER_REAL_LEN(enable)];            ///< 禁用/启用，0/1
		WBParseCHAR privileged[WBPARSER_REAL_LEN(privileged)];    ///< 特权,标识管理员，0/1
		WBParseCHAR entMail[WBPARSER_REAL_LEN(entMail)];          ///< 企业邮箱,微博登录账号
		WBParseCHAR subMail[WBPARSER_REAL_LEN(subMail)];          ///< 订阅邮箱地址,邮件发送
		WBParseCHAR e164[WBPARSER_REAL_LEN(e164)];                ///< E164号
		WBParseCHAR jid[WBPARSER_REAL_LEN(jid)];                  ///< xmpp账号
		WBParseCHAR mobileNum[WBPARSER_REAL_LEN(mobileNum)];      ///< 联系电话
		WBParseCHAR extNum[WBPARSER_REAL_LEN(extNum)];            ///< 分机
		WBParseCHAR seat[WBPARSER_REAL_LEN(seat)];                ///< 座位号
		WBParseCHAR brief[WBPARSER_REAL_LEN(brief)];              ///< 一句话介绍
		WBParseCHAR position[WBPARSER_REAL_LEN(position)];        ///< 职位
		WBParseCHAR portrait32[WBPARSER_REAL_LEN(portrait32)];    ///< 32位头像
		WBParseCHAR portrait40[WBPARSER_REAL_LEN(portrait40)];    ///< 40位头像
		WBParseCHAR portrait64[WBPARSER_REAL_LEN(portrait64)];    ///< 64位头像
		WBParseCHAR portrait128[WBPARSER_REAL_LEN(portrait128)];  ///< 128位头像
		WBParseCHAR portrait256[WBPARSER_REAL_LEN(portrait256)];  ///< 256位头像
		WBParseCHAR password[WBPARSER_REAL_LEN(password)];        ///< XMPP登陆密码

		WBParseCHAR enableWeibo[WBPARSER_REAL_LEN(enableWeibo)];     ///< 微博权限(0/1,没有/有)
		WBParseCHAR enableMeeting[WBPARSER_REAL_LEN(enableMeeting)]; ///< 会议管理权限(0/1,没有/有)
		WBParseCHAR enableHD[WBPARSER_REAL_LEN(enableHD)];           ///< 高清权限(0/1,没有/有)
		WBParseCHAR enableCall[WBPARSER_REAL_LEN(enableCall)];       ///< 电话呼叫权限(0/1,没有/有)
		WBParseCHAR enableRoam[WBPARSER_REAL_LEN(enableRoam)];       ///< 漫游权限(0/1,没有/有)
		WBParseCHAR weiboAdmin[WBPARSER_REAL_LEN(weiboAdmin)];       ///< 微博管理员权限(0/1,没有/有)
		WBParseCHAR meetingAdmin[WBPARSER_REAL_LEN(meetingAdmin)];   ///< 会议管理员权限(0/1,没有/有)
		WBParseCHAR isLikeQuery[WBPARSER_REAL_LEN(isLikeQuery)];     ///< 是否支持模糊搜索(0:不可以；1:可以)
		WBParseCHAR authMode[WBPARSER_REAL_LEN(authMode)];           ///< 添加联系人认证模式(0:无需认证；1:待认证；2:拒绝接受)
		WBParseCHAR sex[WBPARSER_REAL_LEN(sex)];                     ///< 性别(1/0,男/女)

		t_wbParse_Keda_Depts *depts;               ///< 部门信息
	};

	struct t_wbParse_Keda_EntConfig
	{
		WBParseCHAR xmppDomain[WBPARSER_REAL_LEN(xmppDomain)];  ///< xmpp域名
		WBParseCHAR apiUrl[WBPARSER_REAL_LEN(apiUrl)];          ///< 平台API地址
		WBParseCHAR weiboUrl[WBPARSER_REAL_LEN(weiboUrl)];      ///< 微博API地址
		WBParseCHAR webImUrl[WBPARSER_REAL_LEN(webImUrl)];      ///< web客户端连接的Im地址
		WBParseCHAR tcpImUrl[WBPARSER_REAL_LEN(tcpImUrl)];      ///< 终端连接的Im地址
		WBParseCHAR tcpPort[WBPARSER_REAL_LEN(tcpPort)];        ///< 终端连接的Im端口
		WBParseCHAR ulsUrl[WBPARSER_REAL_LEN(ulsUrl)];          ///< ULS地址
		WBParseCHAR ulsPort[WBPARSER_REAL_LEN(ulsPort)];        ///< ULS端口
	};

	struct t_wbParse_Keda_Result
	{
		t_wbParse_Keda_EntUser *entUser;     ///< 
		t_wbParse_Keda_EntConfig *entConfig; ///< 
	};

	struct t_wbParse_Keda_EntUsers
	{
		WBParseINT entUserNum;
		t_wbParse_Keda_EntUser *entUser;     ///< 
	};

	struct t_wbParse_Keda_Page
	{
		WBParseINT totalCount;
		t_wbParse_Keda_EntUsers *entUsers;   ///< 
	};


// add end


// -------------------------------------

	// 2012-03-01 update by sunfei <users> 使用的数据结构

	struct t_wbParse_Keda_UserProfile
	{
		WBParseCHAR userId[WBPARSER_REAL_LEN(userId)];					///< 用户数字id，如“124”
		WBParseCHAR jobTitle[WBPARSER_REAL_LEN(jobTitle)];	            ///< 职位，如“云协师”
		WBParseCHAR location[WBPARSER_REAL_LEN(location)];		        ///< 位置，如“3F-10L-04m”
		WBParseCHAR workPhone[WBPARSER_REAL_LEN(workPhone)];			///< 工作电话，如“8484”
		WBParseCHAR mobilePhone[WBPARSER_REAL_LEN(mobilePhone)];		///< 移动电话，如“130xxxx8137”

		WBParseINT birthYear;    ///< 年
		WBParseINT birthMonth;   ///< 月
		WBParseINT birthDay;     ///< 日
	};

	struct t_wbParse_Keda_OnlineState
	{
		WBParseCHAR wbOnline[WBPARSER_REAL_LEN(wbOnline)];		///< 微博在线状态
		WBParseCHAR tlOnline[WBPARSER_REAL_LEN(tlOnline)];	    ///< TrueLink在线状态
		WBParseCHAR tdOnline[WBPARSER_REAL_LEN(tdOnline)];		///< pad的在线状态(ios/android)
		WBParseCHAR tpOnline[WBPARSER_REAL_LEN(tpOnline)];		///< phone的在线状态(ios/android)
		WBParseCHAR tsOnline[WBPARSER_REAL_LEN(tsOnline)];		///< TrueSens在线状态
	};

	struct t_wbParse_Keda_Dept
	{
		WBParseINT departmentId;                                          ///< 如“51”
		WBParseCHAR departmentName[WBPARSER_REAL_LEN(departmentName)];    ///< 部门名字如“软件二部”
		WBParseCHAR fullPath[WBPARSER_REAL_LEN(fullPath)];	              ///< 如“科达科技/研发中心/软件二部”
		WBParseCHAR deptPosition[WBPARSER_REAL_LEN(deptPosition)];	      ///< 如“软件工程师”
	};

	struct t_wbParse_Keda_Depts
	{
		WBParseINT deptNum;            ///< 部门数目（一个人可能同时属于多个部门）
		t_wbParse_Keda_Dept *dept;     ///< 部门详细信息
	};

	struct t_wbParse_Keda_User 
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						       ///< 用户数字id，如“124”
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	                       ///< 微博昵称（科达都为实名，如“孙飞”）
		WBParseCHAR email[WBPARSER_REAL_LEN(email)];			           ///< 电子邮件，如“sunfei@kedacom.com”
		WBParseCHAR mugshot128[WBPARSER_REAL_LEN(mugshot128)];			   ///< 头像128像素，如“photos/portrait/124/head_128.jpg”
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];			   ///< 头像64像素
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];			   ///< 头像40像素
		WBParseCHAR personalBriefly[WBPARSER_REAL_LEN(personalBriefly)];   ///< 个性签名

		WBParseINT followersNum;    ///< 关注数目
		WBParseINT fansNum;         ///< 粉丝数目
		WBParseINT friendsNum;      ///< 好友数目（即互相关注数目）
		WBParseINT threadNum;       ///< 发布的微博数目

		t_wbParse_Keda_UserProfile *profile; ///< 用户的个人信息

		WBParseCHAR beenFollowed[WBPARSER_REAL_LEN(beenFollowed)];            ///< 是否被关注
		WBParseCHAR beenFollowBoth[WBPARSER_REAL_LEN(beenFollowBoth)];          ///< 是否相关关注
		WBParseCHAR enableWeibo[WBPARSER_REAL_LEN(enableWeibo)];             ///< 微博是否可用

		WBParseCHAR online[WBPARSER_REAL_LEN(online)];                     ///< 是否在线
		WBParseCHAR defaultUid[WBPARSER_REAL_LEN(defaultUid)];	           ///< 默认用户id，同uid
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	                       ///< 用户拼音id，如“sunfei”
		WBParseCHAR e164[WBPARSER_REAL_LEN(e164)];	                       ///< E164
		WBParseCHAR jid[WBPARSER_REAL_LEN(jid)];	                       ///< jid

		t_wbParse_Keda_OnlineState *onlineState;   ///< 各种登陆设备的在线状态
		t_wbParse_Keda_Depts *depts;               ///< 部门信息
	};

	struct t_wbParse_Keda_Users_Data
	{
		WBParseINT userNum;
		t_wbParse_Keda_User *user; ///< 每个用户的数据
	};

	struct t_wbParse_Keda_Users 
	{
		WBParseINT pageSize;       ///< 每页的记录数
		WBParseINT totalCount;     ///< 总记录数
		WBParseINT currentPage;    ///< 当前页码
		WBParseINT totalPage;      ///< 总页数
		WBParseINT avaCount;       ///< 当前页包含的条数

		t_wbParse_Keda_Users_Data *data; ///< 用户的具体数据
	};

	struct t_wbParse_Keda_Topic
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];                    ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	      ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	              ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	  ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	  ///< 
		WBParseCHAR feedCount[WBPARSER_REAL_LEN(feedCount)];      ///< 
		WBParseCHAR type[WBPARSER_REAL_LEN(type)];                ///< 
	};

	struct t_wbParse_Keda_Like
	{
		WBParseCHAR userName[WBPARSER_REAL_LEN(userName)];       ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	             ///< 
		WBParseCHAR feedId[WBPARSER_REAL_LEN(feedId)];	         ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	     ///< 
	};

	struct t_wbParse_Keda_GroupStatistics
	{
		WBParseCHAR groupId[WBPARSER_REAL_LEN(groupId)];          ///< 
		WBParseINT feedNum;	                                      ///< 
		WBParseINT memberNum;	                                  ///< 
		WBParseCHAR groupUid[WBPARSER_REAL_LEN(groupUid)];        ///< 
		WBParseINT topicNum;	                                  ///< 
	};

	struct t_wbParse_Keda_Group
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];                      ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	                ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	                ///< 
		WBParseCHAR groupBriefly[WBPARSER_REAL_LEN(groupBriefly)];	///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];        ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];        ///< 

		t_wbParse_Keda_GroupStatistics *groupStatistics;            ///< 组数据
	};

	struct t_wbParse_Keda_Groups_Data
	{
		WBParseINT groupNum;
		t_wbParse_Keda_Group *group; ///< 每个用户的组数据
	};

	struct t_wbParse_Keda_Groups
	{
		WBParseINT pageSize;       ///< 每页的记录数
		WBParseINT totalCount;     ///< 总记录数
		WBParseINT currentPage;    ///< 当前页码
		WBParseINT totalPage;      ///< 总页数
		WBParseINT avaCount;       ///< 当前页包含的条数

		t_wbParse_Keda_Groups_Data *data; ///< 用户的具体组数据
	};

	struct t_wbParse_Keda_GroupCreate
	{
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	                ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	                ///< 
		WBParseCHAR groupBriefly[WBPARSER_REAL_LEN(groupBriefly)];	///< 
	};

// add end



// 2012-03-02 add by sunfei 解析私信相关内容

	struct t_wbParse_Keda_Sender
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];	        ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	    ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	    ///< 
		WBParseCHAR email[WBPARSER_REAL_LEN(email)];	///< 
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];	    ///< 
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];		///< 
		WBParseCHAR beenFollowed[WBPARSER_REAL_LEN(beenFollowed)];	///< 
	};

	struct t_wbParse_Keda_Receiver
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];	        ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];	    ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];	    ///< 
		WBParseCHAR email[WBPARSER_REAL_LEN(email)];	///< 
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];	    ///< 
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];		///< 
		WBParseCHAR beenFollowed[WBPARSER_REAL_LEN(beenFollowed)];	///< 
	};

	struct t_wbParse_Keda_PrivateFeed
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];	                ///< 
		WBParseCHAR content[WBPARSER_REAL_LEN(content)];	    ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	///< 

		t_wbParse_Keda_Sender *sender;
		t_wbParse_Keda_Receiver *receiver;
	};

	struct t_wbParse_Keda_PrivateContacter
	{
		t_wbParse_Keda_PrivateFeed *privateFeed;    ///< 私信的具体内容
		WBParseINT count;                           ///< 私信的条数
	};

	struct t_wbParse_Keda_PrivCont_Data
	{
		WBParseINT privContNum;                            ///< 私信会话数目
		t_wbParse_Keda_PrivateContacter *privateContacter; ///< 具体一个私信会话
	};

	struct t_wbParse_Keda_PrivateContacters
	{
		WBParseINT pageSize;       ///< 每页的记录数
		WBParseINT totalCount;     ///< 总记录数
		WBParseINT currentPage;    ///< 当前页码
		WBParseINT totalPage;      ///< 总页数
		WBParseINT avaCount;       ///< 当前页包含的条数

		t_wbParse_Keda_PrivCont_Data *data; ///< 用户的具体组数据
	};

	struct t_wbParse_Keda_PrivFeeds_Data
	{
		WBParseINT privateFeedNum;               ///< 
		t_wbParse_Keda_PrivateFeed *privateFeed; ///< 
	};

	struct t_wbParse_Keda_PrivateFeeds
	{
		WBParseINT pageSize;       ///< 每页的记录数
		WBParseINT totalCount;     ///< 总记录数
		WBParseINT currentPage;    ///< 当前页码
		WBParseINT totalPage;      ///< 总页数
		WBParseINT avaCount;       ///< 当前页包含的条数

		t_wbParse_Keda_PrivFeeds_Data *data; ///< 用户的具体组数据
	};


// add end


// 2012-03-16 update by sunfei <departments> 使用的数据结构

	struct t_wbParse_Keda_departmentUsers
	{
		WBParseINT userNum;
		t_wbParse_Keda_User *user; ///< 
	};

	struct t_wbParse_Keda_Department
	{
		WBParseINT  departmentId;                                       ///< 
		WBParseCHAR departmentName[WBPARSER_REAL_LEN(departmentName)];  ///< 
		WBParseCHAR fullPath[WBPARSER_REAL_LEN(fullPath)];              ///< 
		WBParseINT  parentId;                                           ///< 
		WBParseCHAR leaf[WBPARSER_REAL_LEN(leaf)];                      ///< 

		t_wbParse_Keda_departmentUsers *users;                ///< 
	};

	struct t_wbParse_Keda_Departments
	{
		WBParseINT departmentNum;              ///< 
		t_wbParse_Keda_Department *department; ///< 
	};

	struct t_wbParse_Keda_searchDepartment
	{
		WBParseINT totalNum;               ///< 
		t_wbParse_Keda_Departments *data;  ///< 
	};

	struct t_wbParse_Keda_searchUser
	{
		WBParseINT totalNum;              ///< 
		t_wbParse_Keda_Users_Data *data;  ///< 
	};


// add end





// 2012-01-16 add by sunfei    <ThreadMessage> 使用的数据结构

	// @@ <subject> 结构定义

	struct t_wbParse_Keda_Id
	{
// 		WBParseCHAR id[WBPARSER_REAL_LEN(id)];	                   ///< 2012-02-07 改
		WBParseLONG id;	                                           ///< 
	};

	struct t_wbParse_Keda_ReferencedUserIds
	{
		WBParseINT referencedUserIdNum;
		t_wbParse_Keda_Id *referencedUserId;
	};

	struct t_wbParse_Keda_TopicIds
	{
		WBParseINT topicIdNum;
		t_wbParse_Keda_Id *topicId;
	};

	struct t_wbParse_Keda_LikeByIds
	{
		WBParseINT likeByIdNum;
		t_wbParse_Keda_Id *likeById;
	};

	struct t_wbParse_Keda_Picture
	{
		WBParseCHAR extensionName[WBPARSER_REAL_LEN(extensionName)];	         ///< 
		WBParseCHAR picturePreviewUrl[WBPARSER_REAL_LEN(picturePreviewUrl)];	 ///< 
		WBParseCHAR pictureTinyUrl[WBPARSER_REAL_LEN(pictureTinyUrl)];	         ///< 
		WBParseCHAR pictureMobileUrl[WBPARSER_REAL_LEN(pictureMobileUrl)];	     ///<
		WBParseCHAR pictureName[WBPARSER_REAL_LEN(pictureName)];	             ///< 
		WBParseCHAR pictureUrl[WBPARSER_REAL_LEN(pictureUrl)];	                 ///< 
        WBParseCHAR pictureTinyMobileUrl[WBPARSER_REAL_LEN(pictureMobileUrl)];	     ///<
        WBParseCHAR pictureLargeMobileUrl[WBPARSER_REAL_LEN(pictureMobileUrl)];	     ///< 
	};

	struct t_wbParse_Keda_Pictures
	{
		WBParseINT pictureNum;
		t_wbParse_Keda_Picture *picture;
	};


	struct t_wbParse_Keda_Attachment
	{
		WBParseCHAR extensionName[WBPARSER_REAL_LEN(extensionName)];	         ///< 
		WBParseCHAR fileName[WBPARSER_REAL_LEN(fileName)];	                     ///< 
		WBParseCHAR fileUrl[WBPARSER_REAL_LEN(fileUrl)];	                     ///< 
	};

	struct t_wbParse_Keda_Attachments
	{
		WBParseINT attachmentNum;
		t_wbParse_Keda_Attachment *attachment;
	};


	struct t_wbParse_Keda_Subject
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						           ///< 
		WBParseCHAR body[WBPARSER_REAL_LEN(body)];	                           ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	                   ///< 
		WBParseCHAR directMessage[WBPARSER_REAL_LEN(directMessage)];	       ///< 
		WBParseCHAR broadcast[WBPARSER_REAL_LEN(broadcast)];	               ///< 
		WBParseCHAR systemMessage[WBPARSER_REAL_LEN(systemMessage)];	       ///< 
		WBParseCHAR feedType[WBPARSER_REAL_LEN(feedType)];	                   ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	               ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	               ///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	               ///< 

		t_wbParse_Keda_ReferencedUserIds *referencedUserIds;                   ///< 
		t_wbParse_Keda_TopicIds *topicIds;	                                   ///< 
		t_wbParse_Keda_LikeByIds *likeByIds;	                               ///< 
		t_wbParse_Keda_Pictures *pictures;	                                   ///< 
		t_wbParse_Keda_Attachments *attachments;	                           ///< 

		WBParseCHAR isBookMarked[WBPARSER_REAL_LEN(isBookMarked)];	           ///< 
		WBParseCHAR hot[WBPARSER_REAL_LEN(hot)];	                           ///< 
		WBParseCHAR title[WBPARSER_REAL_LEN(title)];	                       ///< 
		WBParseCHAR isDelete[WBPARSER_REAL_LEN(isDelete)];	                   ///< 
		WBParseCHAR replyNum[WBPARSER_REAL_LEN(replyNum)];	                   ///< 
		WBParseCHAR replyNoLoadNum[WBPARSER_REAL_LEN(replyNoLoadNum)];	       ///< 

        WBParseCHAR createLikeTime[WBPARSER_REAL_LEN(createLikeTime)];	       ///< 
	};

	// @@ <subject> end


	// @@ <replies> 结构定义

	struct t_wbParse_Keda_FeedMessage
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						           ///< 
		WBParseCHAR body[WBPARSER_REAL_LEN(body)];	                           ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	                   ///< 
		WBParseCHAR repliedToId[WBPARSER_REAL_LEN(repliedToId)];	           ///< 
		WBParseCHAR repliedToBody[WBPARSER_REAL_LEN(repliedToBody)];	       ///< 
		WBParseCHAR repliedToUserId[WBPARSER_REAL_LEN(repliedToUserId)];	   ///<
		WBParseCHAR directMessage[WBPARSER_REAL_LEN(directMessage)];	       ///< 
		WBParseCHAR broadcast[WBPARSER_REAL_LEN(broadcast)];	               ///< 
		WBParseCHAR systemMessage[WBPARSER_REAL_LEN(systemMessage)];           ///< 
		WBParseCHAR feedType[WBPARSER_REAL_LEN(feedType)];	                   ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	               ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	               ///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	               ///< 

		t_wbParse_Keda_ReferencedUserIds *referencedUserIds;                   ///< 
		t_wbParse_Keda_TopicIds *topicIds;	                                   ///< 
		t_wbParse_Keda_LikeByIds *likeByIds;	                               ///< 
		t_wbParse_Keda_Pictures *pictures;	                                   ///< 
		t_wbParse_Keda_Attachments *attachments;	                           ///< 

		WBParseCHAR isBookMarked[WBPARSER_REAL_LEN(isBookMarked)];	           ///< 
		WBParseCHAR hot[WBPARSER_REAL_LEN(hot)];	                           ///< 
		WBParseCHAR isDelete[WBPARSER_REAL_LEN(isDelete)];	                   ///< 

        WBParseCHAR createLikeTime[WBPARSER_REAL_LEN(createLikeTime)];	                   ///< 
	};

	struct t_wbParse_Keda_Replies
	{
		WBParseINT feedMessNum;
		t_wbParse_Keda_FeedMessage *feedMessage;
	};

	// @@ <replies> end



	// 2012-02-08 add by sunfei

	struct t_wbParse_Keda_RefReply
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						           ///< 
		WBParseCHAR body[WBPARSER_REAL_LEN(body)];	                           ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	                   ///< 
		WBParseCHAR repliedToId[WBPARSER_REAL_LEN(repliedToId)];	           ///< 
// 		WBParseCHAR repliedToBody[WBPARSER_REAL_LEN(repliedToBody)];	       ///< 
		WBParseCHAR directMessage[WBPARSER_REAL_LEN(directMessage)];	       ///< 
		WBParseCHAR broadcast[WBPARSER_REAL_LEN(broadcast)];	               ///< 
		WBParseCHAR systemMessage[WBPARSER_REAL_LEN(systemMessage)];           ///< 
		WBParseCHAR feedType[WBPARSER_REAL_LEN(feedType)];	                   ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	               ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	               ///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	               ///< 

		t_wbParse_Keda_ReferencedUserIds *referencedUserIds;                   ///< 
		t_wbParse_Keda_TopicIds *topicIds;	                                   ///< 
		t_wbParse_Keda_LikeByIds *likeByIds;	                               ///< 
		t_wbParse_Keda_Pictures *pictures;	                                   ///< 
		t_wbParse_Keda_Attachments *attachments;	                           ///< 

		WBParseCHAR isBookMarked[WBPARSER_REAL_LEN(isBookMarked)];	           ///< 
		WBParseCHAR hot[WBPARSER_REAL_LEN(hot)];	                           ///< 
		WBParseCHAR isDelete[WBPARSER_REAL_LEN(isDelete)];	                   ///< 
	};

	struct t_wbParse_Keda_Reply
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						           ///< 
		WBParseCHAR body[WBPARSER_REAL_LEN(body)];	                           ///< 
		WBParseCHAR threadId[WBPARSER_REAL_LEN(threadId)];	                   ///< 
		WBParseCHAR repliedToId[WBPARSER_REAL_LEN(repliedToId)];	           ///< 
// 		WBParseCHAR repliedToBody[WBPARSER_REAL_LEN(repliedToBody)];	       ///< 
		WBParseCHAR directMessage[WBPARSER_REAL_LEN(directMessage)];	       ///< 
		WBParseCHAR broadcast[WBPARSER_REAL_LEN(broadcast)];	               ///< 
		WBParseCHAR systemMessage[WBPARSER_REAL_LEN(systemMessage)];           ///< 
		WBParseCHAR feedType[WBPARSER_REAL_LEN(feedType)];	                   ///< 
		WBParseCHAR createdBy[WBPARSER_REAL_LEN(createdBy)];	               ///< 
		WBParseCHAR createdAt[WBPARSER_REAL_LEN(createdAt)];	               ///< 
		WBParseCHAR clientType[WBPARSER_REAL_LEN(clientType)];	               ///< 

		t_wbParse_Keda_ReferencedUserIds *referencedUserIds;                   ///< 
		t_wbParse_Keda_TopicIds *topicIds;	                                   ///< 
		t_wbParse_Keda_LikeByIds *likeByIds;	                               ///< 
		t_wbParse_Keda_Pictures *pictures;	                                   ///< 
		t_wbParse_Keda_Attachments *attachments;	                           ///< 

		WBParseCHAR isBookMarked[WBPARSER_REAL_LEN(isBookMarked)];	           ///< 
		WBParseCHAR hot[WBPARSER_REAL_LEN(hot)];	                           ///< 
		WBParseCHAR isDelete[WBPARSER_REAL_LEN(isDelete)];	                   ///< 
	};

	// add end


	struct t_wbParse_Keda_SubjectStyleThread
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						       ///< 
		WBParseCHAR lastFeedId[WBPARSER_REAL_LEN(lastFeedId)];	           ///< 

		t_wbParse_Keda_Subject *subject;
		t_wbParse_Keda_Replies *replies;
	};

	struct t_wbParse_Keda_ReplyStyleThread
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];						       ///< 
		WBParseCHAR lastFeedId[WBPARSER_REAL_LEN(lastFeedId)];	           ///< 

		t_wbParse_Keda_Subject *subject;
		t_wbParse_Keda_RefReply *refReply;
		t_wbParse_Keda_Reply *reply;

        WBParseCHAR createLikeTime[WBPARSER_REAL_LEN(datetime)];
	};

	struct t_wbParse_Keda_ThreadMessage
	{
		WBParseINT subjectStyleThreadNum;
		t_wbParse_Keda_SubjectStyleThread *subjectStyleThread;

		WBParseINT replyStyleThreadNum;
		t_wbParse_Keda_ReplyStyleThread *replyStyleThread;
	};

	// @@ <ThreadMessage> end



	struct t_wbParse_Keda_Meta
	{
		WBParseCHAR readFeedId[WBPARSER_REAL_LEN(readFeedId)];			///< 返回的消息中最新的消息ID
		WBParseCHAR oldestFeedId[WBPARSER_REAL_LEN(oldestFeedId)];	    ///< 返回的消息中最后主题的最后回复ID
		WBParseCHAR totalCount[WBPARSER_REAL_LEN(totalCount)];		    ///< 总的数量
	};


	// 2012-01-16 add by sunfei    <relatedData> 使用的数据结构

	struct t_wbParse_Keda_UserData
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];			            ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];			        ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];			        ///< 
		WBParseCHAR email[WBPARSER_REAL_LEN(email)];			    ///< 
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];	    ///< 
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];		///< 
		WBParseCHAR beenFollowed[WBPARSER_REAL_LEN(beenFollowed)];	///< 
	};

	struct t_wbParse_Keda_GroupData
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];			            ///< 
		WBParseCHAR uid[WBPARSER_REAL_LEN(uid)];			        ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];			        ///< 
		WBParseCHAR mugshot64[WBPARSER_REAL_LEN(mugshot64)];	    ///< 
		WBParseCHAR mugshot40[WBPARSER_REAL_LEN(mugshot40)];		///< 
	};

	struct t_wbParse_Keda_TopicData
	{
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];			            ///< 
		WBParseCHAR name[WBPARSER_REAL_LEN(name)];			        ///< 
	};

	struct t_wbParse_Keda_UserEntry
	{
		WBParseCHAR userId[WBPARSER_REAL_LEN(userId)];			    ///< 
		t_wbParse_Keda_UserData *userData;
	};

	struct t_wbParse_Keda_GroupEntry
	{
		WBParseCHAR groupId[WBPARSER_REAL_LEN(groupId)];			///< 
		t_wbParse_Keda_GroupData *groupData;
	};

	struct t_wbParse_Keda_TopicEntry
	{
		WBParseCHAR topicId[WBPARSER_REAL_LEN(topicId)];			///< 
		t_wbParse_Keda_TopicData *topicData;
	};

	struct t_wbParse_Keda_RelatedUsers
	{
		WBParseINT userEntryNum;
		t_wbParse_Keda_UserEntry *userEntry;
	};

	struct t_wbParse_Keda_RelatedGroups
	{
		WBParseINT groupEntryNum;
		t_wbParse_Keda_GroupEntry *groupEntry;

	};

	struct t_wbParse_Keda_RelatedTopics
	{
		WBParseINT topicEntryNum;
		t_wbParse_Keda_TopicEntry *topicEntry;

	};

	struct t_wbParse_Keda_RelatedData
	{
		t_wbParse_Keda_RelatedUsers *relatedUsers;		///< 
		t_wbParse_Keda_RelatedGroups *relatedGroups;	///< 
		t_wbParse_Keda_RelatedTopics *relatedTopics;	///< 
	};

	// <relatedData> end


	struct t_wbParse_Keda_Messages
	{
		t_wbParse_Keda_Meta *meta;				                        ///< 
		t_wbParse_Keda_ThreadMessage *threadMessage;	                ///< 
		t_wbParse_Keda_RelatedData *relatedData;		                ///< 
	};



	// 2012-05-08 add by sunfei 用于【获取@、赞、粉丝等初始消息】API

	struct t_wbParse_Keda_Notify
	{
		WBParseINT amount;
		WBParseCHAR messageType[WBPARSER_REAL_LEN(messageType)];			///< 
	};

	struct t_wbParse_Keda_Notifies
	{
		WBParseINT notifyNum;
		t_wbParse_Keda_Notify *notify;				                        ///< 
	};


// 2012-09-18 add by sunfei 获取赞过我的微博数据

	struct t_wbParse_Keda_Messages_Data
	{
		t_wbParse_Keda_Messages *messages;
	};

	struct t_wbParse_Keda_Like_Page
	{
		WBParseINT pageSize;       ///< 每页的记录数
		WBParseINT totalCount;     ///< 总记录数
		WBParseINT currentPage;    ///< 当前页码
		WBParseINT totalPage;      ///< 总页数
		WBParseINT avaCount;       ///< 当前页包含的条数

		t_wbParse_Keda_Messages_Data *data; ///< 具体数据
	};

// add end


// -------------------------------------


	/** 错误 */
	struct t_wbParse_Error 
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR request[WBPARSER_REAL_LEN(request)];
		WBParseCHAR error[WBPARSER_REAL_LEN(error)];            ///< 错误描述
		WBParseCHAR error_code[WBPARSER_REAL_LEN(error_code)];  ///< 错误码
		unsigned int error_sub_code; ///< 子错误信息

        unsigned int params[4];                                // 保留的信息
	};

	/** 为富媒体而定 */
	struct t_wbParse_media_item
	{
		t_wbParse_Header ph_; ///< 解析头

		WBParseCHAR id[WBPARSER_REAL_LEN(id)];			 ///<mediaid
		WBParseCHAR url[WBPARSER_REAL_LEN(url)];		 ///<media- url
		WBParseCHAR screen[WBPARSER_REAL_LEN(url)];		 ///<media- screen
		WBParseCHAR type[WBPARSER_REAL_LEN(type)];		 ///<media- type:webpage,video,audio,event,magic,vote
		WBParseCHAR title[WBPARSER_REAL_LEN(name)];		 ///<media- title
		WBParseCHAR flash[WBPARSER_REAL_LEN(url)];		 ///<media- flash
		//
		struct MediaExternItem
		{
			// extern information
			WBParseCHAR ext_from[32]; 
			WBParseCHAR ext_shorturl[WBPARSER_REAL_LEN(url)];	 ///<media- shorturl
			WBParseCHAR ext_description[WBPARSER_REAL_LEN(text)];///<media- description
			WBParseCHAR ext_lastmodified[ WBPARSER_REAL_LEN(created_time) ];///<media- lastmodify

			// extern - music 
			WBParseCHAR ext_audio_artist[WBPARSER_REAL_LEN(name)];
			WBParseCHAR ext_audio_album[WBPARSER_REAL_LEN(name)];
			WBParseCHAR ext_audio_appkey[256];

			// extern - event
			WBParseCHAR ext_event_id[WBPARSER_REAL_LEN(id)];

			// extern - vote
			WBParseCHAR ext_vote_uid[WBPARSER_REAL_LEN(id)];

		} media_ext;
	};

	struct t_wbParse_Media
	{
		t_wbParse_Header ph_; ///< 解析头

		int cmd;
		WBParseCHAR parentid[WBPARSER_REAL_LEN(id)];///<父微博ID（包含这条微博的父微博id)
		WBParseCHAR id[WBPARSER_REAL_LEN(id)];///< 当前微博ID

		t_wbParse_media_item mitem;
	};


#ifdef _MSC_VER
	const __int64 llMAX = 0xFFFFFFFFFFFFFFFFi64;
#else
	const long long  llMAX = 0xFFFFFFFFFFFFFFFFLL;
#endif

	/** 简单的 */
	struct t_wbParser_MMId
	{
		enum eType
		{
			EType_Unknow = -1,

			/** 我的评论 */
			EType_CommentsToMe = 0,

			/** 我发出去的 */
			EType_CommentsByMe,

			/** 我发与收到 */
			EType_CommentsTimeline,

			/** @我的 */
			EType_AtMe,

			/** 首页 */
			EType_Homes,

			/** 私信 */
			EType_DirectMsgs,

			/** 粉丝*/
			EType_Follower,

			EType_End,
		};
		struct IdValue
		{

#ifdef _MSC_VER
			__int64 maxId;
			__int64 minId;
#else
			long long maxId;
			long long minId;
#endif

		};

		struct IdValue _idVal[EType_End];
	};



    struct t_wbParse_m_short_room
    {
        WBParseINT     id;
        WBParseCHAR    name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR    regionName[WBPARSER_REAL_LEN(regionName)];
    };

    struct t_wbParse_m_short_rooms
    {
        WBParseINT                          count;
        struct t_wbParse_m_short_room *     items;
    };

    struct t_wbParse_m_room_ids
    {
        WBParseINT                       count;
        WBParseINT *                     items;
    };

    struct t_wbParse_m_short_meeting
    {
        WBParseINT   id;
        WBParseCHAR  subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  organizerMoid[WBPARSER_REAL_LEN(moId)];
        WBParseINT   status;
        WBParseINT   isVideoMeeting;
        WBParseINT   feedbackStatus;
        WBParseINT   regularId;

        struct t_wbParse_m_room_ids      room_ids;
    };

    struct t_wbParse_m_short_meetings
    {
        WBParseINT                           count;
        struct t_wbParse_m_short_meeting *   items;
    };


    struct t_wbParse_m_meeting_list
    {
        struct  t_wbParse_m_short_rooms      related_rooms;
        struct  t_wbParse_m_short_meetings   meetings;
    };



    struct t_wbParse_m_person
    {
        WBParseCHAR            moId[WBPARSER_REAL_LEN(moId)];
        WBParseCHAR            email[WBPARSER_REAL_LEN(email)];
        WBParseCHAR            name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR            participantNo[WBPARSER_REAL_LEN(participantNo)];

        WBParseINT             isParticipant;
        WBParseINT             participantType;
    };

    struct t_wbParse_m_persons
    {
        WBParseINT                           count;
        struct t_wbParse_m_person *          items;
    };


    struct  t_wbParse_m_vMeeting
    {
        WBParseCHAR         e164[WBPARSER_REAL_LEN(e164)];
        WBParseCHAR         templateId[WBPARSER_REAL_LEN(template_id)];
        WBParseCHAR         templateName[WBPARSER_REAL_LEN(template_name)];
        WBParseCHAR         password[WBPARSER_REAL_LEN(common)];
        WBParseINT          shortNO;
        WBParseINT          state;
        WBParseINT          isAutoCall;
    };



    struct t_wbParse_m_meeting_info
    {
        WBParseINT   id;
        WBParseCHAR  subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   status;


        WBParseCHAR  organizerMoid[WBPARSER_REAL_LEN(moId)];
        WBParseCHAR  organizer[WBPARSER_REAL_LEN(organizer)];
        WBParseCHAR  organizerMail[WBPARSER_REAL_LEN(organizerMail)];
        WBParseCHAR  phone[WBPARSER_REAL_LEN(phone)];
        WBParseCHAR  mobile[WBPARSER_REAL_LEN(mobile)];
        WBParseCHAR  brief[WBPARSER_REAL_LEN(m_brief)];

        WBParseINT   isVideo;
        // WBParseCHAR  template_id[WBPARSER_REAL_LEN(template_id)];
        // WBParseCHAR  template_name[WBPARSER_REAL_LEN(template_name)];
        // WBParseINT   isAutoCall;
        // WBParseCHAR  e164[WBPARSER_REAL_LEN(e164)];

        t_wbParse_m_vMeeting    vMeeting;

        struct t_wbParse_m_short_rooms  rooms;
        struct t_wbParse_m_persons      participants;
    };

    struct t_wbParse_m_meeting_ids
    {
        WBParseINT                       count;
        WBParseINT *                     items;
    };

    struct t_wbParse_m_regular_info
    {
        WBParseINT   id;
        WBParseCHAR  subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   status;


        WBParseCHAR  organizer[WBPARSER_REAL_LEN(organizer)];
        WBParseCHAR  organizerMail[WBPARSER_REAL_LEN(organizerMail)];
        WBParseCHAR  phone[WBPARSER_REAL_LEN(phone)];
        WBParseCHAR  mobile[WBPARSER_REAL_LEN(mobile)];
        WBParseCHAR  brief[WBPARSER_REAL_LEN(m_brief)];

        WBParseINT   isVideo;
        // WBParseCHAR  template_id[WBPARSER_REAL_LEN(template_id)];
        // WBParseCHAR  template_name[WBPARSER_REAL_LEN(template_name)];
        // WBParseINT   isAutoCall;

        // struct t_wbParse_m_short_rooms  rooms;
        // struct t_wbParse_m_persons      participants;

        WBParseCHAR  startDate[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endDate[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   repeatType;
        WBParseINT   frequency;
        WBParseCHAR  weekDays[WBPARSER_REAL_LEN(common)];

        struct t_wbParse_m_meeting_ids      meeting_ids;
    };



    struct t_wbParse_m_room
    {
        WBParseINT   id;
        WBParseCHAR  name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR  regionName[WBPARSER_REAL_LEN(regionName)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   peopleAdmit;
        WBParseINT   deviceMask;
    };

    struct t_wbParse_m_rooms
    {
        WBParseINT                    count;
        struct t_wbParse_m_room *     items;
    };

    struct t_wbParse_m_room_list
    {
        t_wbParse_m_rooms             rooms;
        WBParseINT                    total;
    };

    struct t_wbParse_m_normal_room_list
    {
        int                                 start;
        int                                 count;

        struct t_wbParse_m_short_rooms      rooms;
    };

    struct t_wbParse_m_room_info
    {
        WBParseINT   id;
        WBParseCHAR  name[WBPARSER_REAL_LEN(name)];
        WBParseINT   peopleAdmit;
        WBParseINT   area;
        WBParseCHAR  phone[WBPARSER_REAL_LEN(phone)];
        WBParseINT   hasVideo;
        WBParseCHAR  e164[WBPARSER_REAL_LEN(e164)];
        WBParseCHAR  deviceAdmin[WBPARSER_REAL_LEN(name)];
        WBParseCHAR  adminPhone[WBPARSER_REAL_LEN(phone)];
        WBParseCHAR  remark[WBPARSER_REAL_LEN(remark)];
        WBParseINT   deviceMask;
    };

    struct t_wbParse_m_room_status
    {
        WBParseINT   id;
        WBParseCHAR  name[WBPARSER_REAL_LEN(name)];
        WBParseINT   status;
    };


    //struct t_wbParse_m_common_item
    //{
    //    WBParseCHAR   id[WBPARSER_REAL_LEN(id)];
    //    WBParseCHAR   name[WBPARSER_REAL_LEN(name)];
    //};

    //struct t_wbParse_m_common_items
    //{
    //    WBParseINT                        count;
    //    struct t_wbParse_m_common_item *  items;
    //};


    struct t_wbParse_m_template_info
    {
        WBParseCHAR  id[WBPARSER_REAL_LEN(id)];
        WBParseCHAR  name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR  resolution[WBPARSER_REAL_LEN(common)];
        WBParseCHAR  bandwith[WBPARSER_REAL_LEN(common)];

        struct t_wbParse_m_persons    persons;
    };

    struct t_wbParse_m_template
    {
        WBParseCHAR   id[WBPARSER_REAL_LEN(id)];
        WBParseCHAR   name[WBPARSER_REAL_LEN(name)];
    };

    struct t_wbParse_m_templates
    {
        WBParseINT                        count;
        struct t_wbParse_m_template *     items;
    };

    struct t_wbParse_m_templateList
    {
        t_wbParse_m_templates             templates;
    };


    // 注意： content的内容比较复杂， 随着messgaeType而变化
    struct t_wbParse_m_notify
    {
        WBParseINT     id;
        WBParseCHAR    receiverMoId[WBPARSER_REAL_LEN(moId)];
        WBParseINT     messgaeType;
        WBParseCHAR    companyMoId[WBPARSER_REAL_LEN(companyMoId)];
        WBParseCHAR    createTime[WBPARSER_REAL_LEN(datetime)];
        void *         content;
    };

    struct t_wbParse_m_notifys
    {
        WBParseINT                         count;
        struct t_wbParse_m_notify *        items;
    };




    // content
    // 适用于messgaeType为102, 103
    struct t_wbParse_m_notify_content_meeting
    {
        WBParseINT   id;
        WBParseCHAR  subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR  startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR  endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseINT   isVideoMeeting;
        WBParseCHAR  phone[WBPARSER_REAL_LEN(phone)];
        WBParseCHAR  mobilePhone[WBPARSER_REAL_LEN(mobile)];

        WBParseCHAR  organiger[WBPARSER_REAL_LEN(organizer)];
        WBParseCHAR  organigerMoid[WBPARSER_REAL_LEN(moId)];

        WBParseCHAR  beforeSubject[WBPARSER_REAL_LEN(subject)];

        struct t_wbParse_m_short_rooms      rooms;
    };


    // 适用于messgaeType为104、108、109、115、116、117、118、119
    struct t_wbParse_m_notify_content_meeting_id
    {
        WBParseINT                      meetingId;

        WBParseCHAR                     subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR                     startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR                     endTime[WBPARSER_REAL_LEN(datetime)];

        WBParseCHAR                     organigerMoid[WBPARSER_REAL_LEN(moId)];
    };

    // 适用于messageType为105、106,107
    struct t_wbParse_m_notify_content_regular_id
    {
        WBParseINT                      regularId;

        WBParseCHAR                     organigerMoid[WBPARSER_REAL_LEN(moId)];
    };

    // 适用于messageType为110
    struct t_wbParse_m_notify_content_cancel_room
    {
        WBParseINT                       meetingId;

        WBParseCHAR                     subject[WBPARSER_REAL_LEN(subject)];
        WBParseCHAR                     startTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR                     endTime[WBPARSER_REAL_LEN(datetime)];
        WBParseCHAR                     organigerMoid[WBPARSER_REAL_LEN(moId)];

        struct t_wbParse_m_short_rooms   rooms;
    };

    // 适用于messageType为111
    struct t_wbParse_m_notify_content_deadline_changed
    {
        WBParseINT                       headline;
    };

    // 适用于messageType为112
    struct t_wbParse_m_notify_content_user_feedback
    {
        WBParseINT                       meetingId;
        struct t_wbParse_m_person        participant;

        WBParseINT                       reasonId;
        WBParseINT                       isVideoMeeting;
    };

    // 适用于messageType为113, 114
    struct t_wbParse_m_notify_content_room_changed
    {
        WBParseINT                       roomId;
    };


    struct t_wbParse_app_notify
    {
        WBParseINT                       amount;
        WBParseCHAR                      messageType[WBPARSER_REAL_LEN(common)];
    };


    struct t_wbParse_app_notifys
    {
        WBParseINT                          count;
        struct t_wbParse_app_notify *       items;
    };



    struct t_wbParse_monitor_device
    {
        WBParseCHAR                         id[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         name[WBPARSER_REAL_LEN(name)];
        WBParseCHAR                         deviceId[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         deviceGUID[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         e164[WBPARSER_REAL_LEN(e164)];
        WBParseCHAR                         deviceType[WBPARSER_REAL_LEN(common)];
        WBParseCHAR                         description[WBPARSER_REAL_LEN(remark)];
    };

    struct t_wbParse_monitor_devices
    {
        WBParseINT                          count;
        struct t_wbParse_monitor_device *   items;
    };

    struct t_wbParse_monitor_group
    {
        WBParseCHAR                         id[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         groupId[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         groupName[WBPARSER_REAL_LEN(name)];
        WBParseCHAR                         parentId[WBPARSER_REAL_LEN(id)];
        WBParseCHAR                         newGroup[WBPARSER_REAL_LEN(common)];
        WBParseCHAR                         leaf[WBPARSER_REAL_LEN(common)];

        struct t_wbParse_monitor_devices    devices;
    };

    struct t_wbParse_monitor_groups
    {
        WBParseINT                          count;
        struct t_wbParse_monitor_group *   items;
    };
}

#endif //__WB_PARSE_STRUCT_H__
