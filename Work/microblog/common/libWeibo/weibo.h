
#ifndef __weibo_SDK_H__
#define __weibo_SDK_H__


#ifdef  __cplusplus
extern "C" {
#endif


#if (defined(WIN32) || defined(_WIN32) ) && !defined(WEIBO_STATICLIB)

	#if defined(BUILDING_LIBWEIBO)
		#define WEIBO_API  __declspec(dllexport)
	#else
		#define WEIBO_API  __declspec(dllimport)
	#endif

#else  // 其他平台的，或者是静态链接
	
	#define WEIBO_API

#endif



#ifndef BUILD_CHARACTER_SET

	typedef char WBChar;

#else

	#if defined(UNICODE) || defined(_UNICODE)
		typedef wchar_t WBChar;
	#else
		typedef char WBChar;
	#endif

#endif // BUILD_CHARACTER_SET



#define WB_MAX_LEN_DEFINE(name , val)  const unsigned int WB_##name##_LEN = val
#define WB_USE_LEN(name) WB_##name##_LEN
#define WB_REAL_LEN(name) WB_##name##_LEN + 1

	WB_MAX_LEN_DEFINE(OPT_VALUE, 64);  // 通用参数

	WB_MAX_LEN_DEFINE(UID, 64);  // 登陆用户名
	WB_MAX_LEN_DEFINE(PWD, 64);  // 登陆密码

	WB_MAX_LEN_DEFINE(OAUTH_TOKEN, 64);        // 由1024改为64
	WB_MAX_LEN_DEFINE(OAUTH_TOKEN_SECRET, 64); // 由2048改为64

	WB_MAX_LEN_DEFINE(WB_INFO, 420);       // 一条微博的标准长度为140个汉字280字节
	WB_MAX_LEN_DEFINE(URL, 1024);          // URL长度
#ifndef _ANDROID_
	WB_MAX_LEN_DEFINE(MULTI_USER, 1024);  // 批量查询账号详细信息使用
#else
	WB_MAX_LEN_DEFINE(MULTI_USER, 16383);   // 批量查询账号详细信息使用
#endif
	WB_MAX_LEN_DEFINE(MAX_FILE_PATH, 512); // 最长文件绝对路径
	WB_MAX_LEN_DEFINE(PM_INFO, 900);       // 每条私信的内容长度不超过300个汉字
	WB_MAX_LEN_DEFINE(BRIEF, 210);         // 一句话简介暂时定为70个汉字
	WB_MAX_LEN_DEFINE(USER_IDS, 256);      // 用户ID集合

    WB_MAX_LEN_DEFINE(POST, 2048);
	WB_MAX_LEN_DEFINE(IP, 30);

    WB_MAX_LEN_DEFINE(OPT_DATE, 30);
    WB_MAX_LEN_DEFINE(SUBJECT, 120);
    WB_MAX_LEN_DEFINE(PHONE, 30);
    WB_MAX_LEN_DEFINE(SHORT_UID, 32);
    WB_MAX_LEN_DEFINE(M_BRIEF, 1000);
    WB_MAX_LEN_DEFINE(TEMPLATE_ID, 20);


	typedef enum
	{
		E_FORMAT_JSON = 0,
		E_FORMAT_XML,
	}WEIBORequestFormat;


#define WEIBO_OPTION(name) WEIBO_OPT_##name
#define WEIBO_OPTION_DEFINE(name , val) WEIBO_OPTION(name) = val

	typedef enum
	{
		WEIBO_OPTION_DEFINE(UNK, 0),
		WEIBO_OPTION(BASE),

		// 2012-01-04 add by sunfei 重新整理所有KEDA API
		WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN),     //82 获得Oauth的accountToken(平台API)            * 2
		WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN),     //82 获得Oauth的accountToken(微博API)            * 3
		WEIBO_OPTION(KEDA_API_P_LOGIN),                //83 登录平台API                                 * 4
		WEIBO_OPTION(KEDA_API_W_LOGIN),                //83 登录微博API                                 * 5
		WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS),     //84 获得当前登录用户及关注用户的微博            * 6
		WEIBO_OPTION(KEDA_API_GET_USER_DATA),          //85 获得用户发布的微博数据（友讯）              * 7
		WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA),      //86 获得用户发布的微博数据（全部）              * 8
		WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY),  //87 获得一条主题以及回复                        * 9
		WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH),        //88 发布一条微博                                * 10
		WEIBO_OPTION(KEDA_API_MESSAGE_REPLY),          //89 回复一条微博                                * 11
		WEIBO_OPTION(KEDA_API_GET_USER_INFO),          //90 获得用户信息                                * 12
		// add end

		// 2012-02-21 add by sunfei 增加v1.0接口 第一部分
		WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA),  //91 获得公司广场的微博数据                    * 13
		WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA),          //92 获得群组的微博数据                        * 14
		WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA),        //93 获得@当前用户的微博数据                   * 15
		WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA),         //94 获得回复我的微博数据                      * 16
		WEIBO_OPTION(KEDA_API_GET_LIKES_DATA),           //95 获得我赞过的微博数据                      * 17
		WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA),       //96 获得我收藏的微博数据                      * 18
		WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA),          //97 获得某个话题的微博数据                    * 19
		// add end

		// 2012-02-27 add by sunfei 增加v1.0接口 第二部分
		WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY),   //98 获得公司人脉                              * 20
		WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION), //99 获得我的关注列表                          * 21
		WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS), //100 获得我的粉丝列表                         * 22
		WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS),   //101 获得我的好友列表                         * 23

		WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE),      //102 添加关注                                   * 24
		WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY),     //103 取消关注                                   * 25
		WEIBO_OPTION(KEDA_API_FAVORITES_CREATE),       //104 添加收藏                                   * 26
		WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY),      //105 取消收藏                                   * 27
		WEIBO_OPTION(KEDA_API_TOPIC_CREATE),           //106 添加话题                                   * 28
		WEIBO_OPTION(KEDA_API_TOPIC_DESTROY),          //107 取消话题                                   * 29
		WEIBO_OPTION(KEDA_API_LIKE_CREATE),            //108 添加like                                   * 30
		WEIBO_OPTION(KEDA_API_LIKE_DESTROY),           //109 取消like                                   * 31

		WEIBO_OPTION(KEDA_API_GET_GROUP_USER),         //110 获取我的群组列表                           * 32
		WEIBO_OPTION(KEDA_API_GET_GROUP_ALL),          //111 获得所有的群组列表                         * 33
		WEIBO_OPTION(KEDA_API_GROUP_CREATE),           //112 添加群操作                                 * 34
		WEIBO_OPTION(KEDA_API_GROUP_JOIN),             //113 加入群操作                                 * 35
		WEIBO_OPTION(KEDA_API_GROUP_QUIT),             //114 退出群操作                                 * 36
    
		WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST),  //115 查看我的信息列表                         * 37
		WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER),  //116 查看我与某人的私信的聊天记录             * 38
		WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND),      //117 发送私信到某人                           * 39
		WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE),    //118 删除私信                                 * 40
		// add end

		// 2012-03-06 add by sunfei 增加v1.0接口 第三部分
		WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT), //119 验证终端账号                            * 41
		WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO),        //120 查询账号详细信息                        * 42
		WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO),     //121 修改账号详细信息                        * 43
		WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD),        //122 修改账号密码                            * 44
		// add end

		// 2012-03-15 add by sunfei 增加v1.0接口 第四部分
		WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL),           //123 获得公司组织架构和所有人员信息       * 45
		WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY),       //124 获取公司组织架构信息                 * 46
		WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER),          //125 获取部门下人员信息(不包含子部门)     * 47
		WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER),       //126 获取部门下人员信息(包含子部门人员)   * 48
		WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT),  //127 在组织架构中搜索部门信息             * 49
		WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER),        //128 在组织架构中搜索人员信息             * 50
		// add end
		WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID),   //129 获取部门下的员工的在线状态           * 51
		WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID),  //130 获取组织里用户的在线状态             * 52

		WEIBO_OPTION(KEDA_API_GET_PORTRAIT),                 //131 获取头像                             * 53
		WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO),      //132 批量查询账号详细信息                 * 54
		WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT),             //133 修改用户头像                         * 55

		WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION),       //134 获取组织架构版本信息                 * 56
		WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD),               //135 带图片和附件的微博发布               * 57

		WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD),        //136 删除自己的微博                       * 58
		WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY),         //137 删除自己的回复                       * 59
		WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL),               //138 获取@、赞、粉丝等初始消息            * 60

		WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT),             //139 保持用户心跳(平台API)                * 61
		WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT),             //139 保持用户心跳(微博API)                * 62

		WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL),       //140 获得用户信息(通过email)              * 63

		WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO),        //141 获得当前登录用户信息                 * 64
		WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL),              //142 获取长篇大论的url                    * 65
		WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF),              //143 修改个人介绍                         * 66
		WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR),      //144 按姓名模糊查找本企业联系人           * 67
		WEIBO_OPTION(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL),   //145 忘记密码                             * 68
		WEIBO_OPTION(KEDA_API_AMS_GET_LIKES_ME_DATA),        //146 获取赞过我的微博数据                 * 69

		WEIBO_OPTION(KEDA_API_AMS_SET_IS_LIKE_QUERY),        //147 设置是否被模糊搜索到                 * 70
		WEIBO_OPTION(KEDA_API_AMS_SET_AUTH_MODE),            //148 设置添加联系人认证模式               * 71
		WEIBO_OPTION(KEDA_API_AMS_CONDITION_QUERY),          //149 条件查询企业联系人                   * 72


        // 会议管理
        WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN),         // 150 获取account token                   * 73
        WEIBO_OPTION(KEDA_MEETING_LOGIN),                    // 151 登录会议平台API                     * 74
        WEIBO_OPTION(KEDA_MEETING_KEEP_HEARTBEAT),           // 152 保持用户心跳(平台API)               * 75

        WEIBO_OPTION(KEDA_MEETING_MEETING_LIST),             // 153 会议列表                            * 76
        WEIBO_OPTION(KEDA_MEETING_MEETING_INFO),             // 154 会议详细信息                        * 77
        WEIBO_OPTION(KEDA_MEETING_BOOKING_INFO),             // 155 会议室预定信息                      * 78
        WEIBO_OPTION(KEDA_MEETING_MEETING_LIST_OF_PERSON),  // 156 某人的会议信息                       * 79

        WEIBO_OPTION(KEDA_MEETING_ADD_MEETING),              // 157 添加会议                            * 80
        WEIBO_OPTION(KEDA_MEETING_MODIFY_MEETING),           // 158 修改会议                            * 81
        WEIBO_OPTION(KEDA_MEETING_DELETE_MEETING),           // 159 删除会议                            * 82
        WEIBO_OPTION(KEDA_MEETING_START_MEETING),            // 160 开始会议                            * 83
        WEIBO_OPTION(KEDA_MEETING_END_MEETING),              // 161 结束会议                            * 84
        WEIBO_OPTION(KEDA_MEETING_FEED),                     // 162 会议反馈                            * 85

        WEIBO_OPTION(KEDA_MEETING_QUERY_FREE_ROOMS),                     // 163 查询会议室              * 86
        WEIBO_OPTION(KEDA_MEETING_RECOMMENDED_V_ROOM),                     // 164 推荐的虚拟会议室      * 87
        WEIBO_OPTION(KEDA_MEETING_NORMAL_ROOMS),                     // 165 推荐的虚拟会议室            * 88
        WEIBO_OPTION(KEDA_MEETING_ROOM_INFO),                        // 166 查询会议室信息              * 89
        WEIBO_OPTION(KEDA_MEETING_ROOM_STATUS),                        // 167 查询会议室状态            * 90
        WEIBO_OPTION(KEDA_MEETING_ROOM_LIST_BY_NAME),                  // 168 根据名称查询会议室        * 91
        WEIBO_OPTION(KEDA_MEETING_QUERY_ROOMS),                       // 169 查询会议室                 * 92

        WEIBO_OPTION(KEDA_MEETING_TEMPLATE_LIST),                       // 170 查询会议模板             * 93
        WEIBO_OPTION(KEDA_MEETING_TEMPLATE_INFO),                       // 171                          * 94
        WEIBO_OPTION(KEDA_MEETING_NOTIFY_ADD),                              // 172                          * 95

        WEIBO_OPTION(KEDA_MEETING_DEADLINE),                            // 173                          * 96
        WEIBO_OPTION(KEDA_MEETING_LOCK_ROOM),                            // 174                         * 97

        WEIBO_OPTION(KEDA_MEETING_LIST_NOTIFY_BY_MAX),                   // 175                         * 98
        WEIBO_OPTION(KEDA_MEETING_GET_NOTIFY_BY_ID),                     // 176                         * 99

        WEIBO_OPTION(KEDA_MEETING_DEL_BOOKED_ROOM),                      //                             * 101
        WEIBO_OPTION(KEDA_MEETING_IF_EXIST_LOCK_BY_FORMKEY),             //                             * 102

        WEIBO_OPTION(KEDA_MEETING_GET_REGULAR),                          //                             * 103

        
		WEIBO_OPTION(LAST),
	}WEIBOoption;

	typedef void (*fun_wb_status_callback)(int state, int errCode, void* pUserdata);
	typedef void (*fun_wb_body_callback)(unsigned long httpCode, const char* body, unsigned long len, void* pUserdata);
	typedef void (*fun_wb_header_callback)(unsigned long httpcode, const char* header, unsigned long len, void* pUsrData);

	struct t_wb_oauth
	{
		char oauth_token_[ WB_REAL_LEN(OAUTH_TOKEN) ];
		char oauth_token_secret_[ WB_REAL_LEN(OAUTH_TOKEN_SECRET) ];

		int  format_; ///< json/xml
		int  cookie_; ///< 是否cookie方式

		fun_wb_status_callback fstatus_cb;
		fun_wb_body_callback   fbody_cb;
		fun_wb_header_callback fheader_cb;
	};

	/** 基结构（方便程序中进行转化） */
	struct t_wb_base
	{
		t_wb_oauth wbauth_;
	};

 
	/*	数据请求,自定义的 */
	struct t_wb_custom
	{
		t_wb_oauth wbauth_;

		WBChar szUrl_[WB_REAL_LEN(URL)];
		WBChar szPost_[WB_REAL_LEN(POST)]; ///< 参数s
		int    method_;                    ///< 请求方法 POST/GET
	};


///////////////////////////////////////////////////////////////////////

	//1. 获得Oauth的accountToken
	struct t_wb_keda_get_accounttoken
	{
		t_wb_oauth wbauth_;

// 		WBChar ip[ WB_REAL_LEN(IP) ];
	};

	//2. 用户登录
	struct t_wb_keda_login
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];  ///< 用户名
		WBChar password[ WB_REAL_LEN(PWD) ];  ///< 用户密码
	};

	//3. 获得当前登录用户及关注用户的微博
	struct t_wb_keda_get_me_and_friends
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//4. 获得用户发布的微博数据（友讯）
	struct t_wb_keda_get_user_data
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];  ///< 用户的UID

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//5. 获得用户发布的微博数据（全部）
	struct t_wb_keda_get_user_all_data 
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];  ///< 用户的UID

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//6. 获得一条主题以及回复
	struct t_wb_keda_get_subject_and_reply
	{
		t_wb_oauth wbauth_;

		long thread_id;        ///< 主题ID

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//7. 发布一条微博
	struct t_wb_keda_message_publish
	{
		t_wb_oauth wbauth_;

		long group_id;                          ///< 发布到群组(可选)
		WBChar status[WB_REAL_LEN(WB_INFO)];    ///< 发布微博内容，信息内容不能超过140个汉字
	};

	//8. 回复一条微博
	struct t_wb_keda_message_reply 
	{
		t_wb_oauth wbauth_;

		long thread_id;                        ///< 需要评论的微博ID（主题ID）
		long reply_id;                         ///< 回复别人的消息ID（主题对应的feedId，或者回复对应的feedId）
		WBChar comment[WB_REAL_LEN(WB_INFO)];  ///< 回复微博内容，信息内容不超过140个汉字
	};

	//9. 获得用户信息
	struct t_wb_keda_get_user_info 
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];  ///< 需要查询的用户uid
	};

	// update end



	//2012-02-21 add by sunfei 实现v1.0接口 第一部分

	//1.获得公司广场的微博数据
	struct t_wb_keda_get_company_public_data
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//2.获得群组的微博数据
	struct t_wb_keda_get_groups_data
	{
		t_wb_oauth wbauth_;

		long group_id;         ///< 群组的ID

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//3.获得@当前用户的微博数据
	struct t_wb_keda_get_mentions_data
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//4.获得回复我的微博数据
	struct t_wb_keda_get_replies_data
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//5.获得我赞过的微博数据
	struct t_wb_keda_get_likes_data
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];  ///< 用户uid

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//6.获得我收藏的微博数据
	struct t_wb_keda_get_favorites_data
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};

	//7.获得某个话题的微博数据
	struct t_wb_keda_get_topics_data
	{
		t_wb_oauth wbauth_;

		long topic_id;         ///< 话题ID

		long since_id;         ///< 若指定此参数，则返回ID比since_id大的微博（即比since_id时间晚的微博），默认为0(可选)
		long max_id;           ///< 若指定此参数，则返回ID小于或等于max_id的微博，默认为0(可选)
		int count;             ///< 单页返回的记录条数，默认为20(可选)
		int reply_count;       ///< 为每条微博加载的回复数，默认为0(可选)
		int reply_style;       ///< 回复排序样式：0代表升序，从第一个回复开始；1表示降序，从最后一个开始。默认为0(可选)
	};
	// add end



	//2012-02-27 add by sunfei 实现v1.0接口 第二部分

	//8.获得公司人脉
	struct t_wb_keda_get_friendship_company
	{
		t_wb_oauth wbauth_;

		int current_page;                      ///< 需要获得的当前页面，不输入为第一页(可选)
		int page_size;                         ///< 每页的显示条数，默认为20(可选)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< 查询的参数(可选)
	};

	//9.获得我的关注列表
	struct t_wb_keda_get_friendship_attention
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];        ///< 用户uid

		int current_page;                      ///< 需要获得的当前页面，不输入为第一页(可选)
		int page_size;                         ///< 每页的显示条数，默认为20(可选)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< 查询的参数(可选)
	};

	//10.获得我的粉丝列表
	struct t_wb_keda_get_friendship_followers
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];        ///< 用户uid

		int current_page;                      ///< 需要获得的当前页面，不输入为第一页(可选)
		int page_size;                         ///< 每页的显示条数，默认为20(可选)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< 查询的参数(可选)
	};

	//11.获得我的好友列表
	struct t_wb_keda_get_friendship_friends
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];        ///< 用户uid

		int current_page;                      ///< 需要获得的当前页面，不输入为第一页(可选)
		int page_size;                         ///< 每页的显示条数，默认为20(可选)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< 查询的参数(可选)
	};

	//12.添加关注 
	struct t_wb_keda_friendship_create
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];       ///< 关注用户的uid
	};

	//13.取消关注
	struct t_wb_keda_friendship_destroy
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];       ///< 关注用户的uid
	};

	//14.添加收藏
	struct t_wb_keda_favorites_create
	{
		t_wb_oauth wbauth_;

		long thread_id;                       ///< 收藏的主题ID
	};

	//15.取消收藏
	struct t_wb_keda_favorites_destroy
	{
		t_wb_oauth wbauth_;

		long thread_id;                       ///< 收藏的主题ID
	};

	//16.添加话题
	struct t_wb_keda_topic_create
	{
		t_wb_oauth wbauth_;

		long thread_id;                                ///< 话题的主题ID
		WBChar topic_name[ WB_REAL_LEN(OPT_VALUE) ];   ///< 话题的主题name
	};

	//17.取消话题
	struct t_wb_keda_topic_destroy
	{
		t_wb_oauth wbauth_;

		long thread_id;    ///< 话题的主题ID
		long topic_id;     ///< 话题的ID
	};

	//18.添加like
	struct t_wb_keda_like_create
	{
		t_wb_oauth wbauth_;

		long thread_id;    ///< 赞的主题ID
		long feed_id;      ///< 赞的feed_id
	};

	//19.取消like
	struct t_wb_keda_like_destroy
	{
		t_wb_oauth wbauth_;

		long thread_id;    ///< 赞的主题ID
		long feed_id;      ///< 赞的feed_id
	};



	//20.获取我的群组列表
	struct t_wb_keda_get_group_user
	{
		t_wb_oauth wbauth_;

		int current_page;                      ///< 需要获得的当前页面，不输入为第一页(可选)
		int page_size;                         ///< 每页的显示条数，默认为20(可选)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< 查询的参数(可选)
	};

	//21.获得所有的群组列表
	struct t_wb_keda_get_group_all
	{
		t_wb_oauth wbauth_;

		int current_page;                      ///< 需要获得的当前页面，不输入为第一页(可选)
		int page_size;                         ///< 每页的显示条数，默认为20(可选)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< 查询的参数(可选)
	};

	//22.添加群操作
	struct t_wb_keda_group_create
	{
		t_wb_oauth wbauth_;

		WBChar group_name[ WB_REAL_LEN(OPT_VALUE) ];    ///< 组名称
		WBChar group_briefly[ WB_REAL_LEN(OPT_VALUE) ]; ///< 组介绍
	};

	//23.加入群操作
	struct t_wb_keda_group_join
	{
		t_wb_oauth wbauth_;

		WBChar guid[ WB_REAL_LEN(OPT_VALUE) ]; ///< 组的uid
	};

	//24.退出群操作
	struct t_wb_keda_group_quit
	{
		t_wb_oauth wbauth_;

		WBChar guid[ WB_REAL_LEN(OPT_VALUE) ]; ///< 组的uid
	};


	//25.查看我的信息列表
	struct t_wb_keda_get_privatemessage_list
	{
		t_wb_oauth wbauth_;

		int current_page;                     ///< 需要获得的当前页面，不输入为第一页(可选)
		int page_size;                        ///< 每页的显示条数，默认为20(可选)
	};

	//26.查看我与某人的私信的聊天记录 
	struct t_wb_keda_get_privatemessage_user
	{
		t_wb_oauth wbauth_;

		int current_page;                     ///< 需要获得的当前页面，不输入为第一页(可选)
		int page_size;                        ///< 每页的显示条数，默认为20(可选)
		WBChar uid[ WB_REAL_LEN(UID) ];       ///< 用户UID如"sunfei"
	};

	//27.发送私信到某人
	struct t_wb_keda_privatemessage_send
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];       ///< 用户UID如"sunfei"
		WBChar body[ WB_REAL_LEN(PM_INFO) ];  ///< 私信内容，不超过300字
	};

	//28.删除私信
	struct t_wb_keda_privatemessage_delete
	{
		t_wb_oauth wbauth_;

		long id;   ///< 私信ID
	};

	// add end




	// 2012-03-06 add by sunfei 增加v1.0接口 第三部分

	//11.验证终端账号
	struct t_wb_keda_ams_validateClientAccount
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];         ///< 账号，可以是E164号码或邮箱
		WBChar password[ WB_REAL_LEN(PWD) ];         ///< 密码
		WBChar deviceType[ WB_REAL_LEN(OPT_VALUE) ]; ///< 登录的设备类型
		WBChar apiHost[ WB_REAL_LEN(IP) ];           ///< 连接的api地址
	};

	//12.查询账号详细信息
	struct t_wb_keda_ams_getAccountInfo
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];         ///< 账号，可以是E164号码或邮箱
	};

    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_MOBILE      0x01
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_EXT         0x02
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_SEAT        0x04
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_POSITION    0x08
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_BRIEF       0x10
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_SEX         0x20
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_NAME        0x40

	//13.修改账号详细信息
	struct t_wb_keda_ams_updateAccountInfo
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];         ///< 账号，可以是E164号码或邮箱
        int    bitMask;

		WBChar mobileNum[ WB_REAL_LEN(OPT_VALUE) ];  ///< 联系电话(可选)
		WBChar extNum[ WB_REAL_LEN(OPT_VALUE) ];     ///< 分机(可选)
		WBChar seat[ WB_REAL_LEN(OPT_VALUE) ];       ///< 座位号(可选)
		WBChar position[ WB_REAL_LEN(OPT_VALUE) ];   ///< 职位(可选)
		WBChar brief[ WB_REAL_LEN(BRIEF) ];          // 一句话简介(可选)
        int    sex;                                  // 性别(可选)
        WBChar name[ WB_REAL_LEN(OPT_VALUE) ];       // 姓名(可选)
	};

	//14.修改账号密码
	struct t_wb_keda_ams_updatePassword
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];  ///< 账号，可以是E164号码或邮箱
		WBChar oldPassword[ WB_REAL_LEN(PWD) ];  ///< 原密码
		WBChar newPassword[ WB_REAL_LEN(PWD) ];  ///< 新密码
	};

	// add end




	// 2012-03-15 add by sunfei 增加v1.0接口 第四部分

	//1.获得公司组织架构和所有人员信息
	struct t_wb_keda_get_department_all
	{
		t_wb_oauth wbauth_;
	};

	//2..获取公司组织架构信息
	struct t_wb_keda_get_department_company
	{
		t_wb_oauth wbauth_;

		int department_id;   ///< 企业ID(可选)
	};

	//3.获取部门下人员信息(不包含子部门)
	struct t_wb_keda_get_department_user
	{
		t_wb_oauth wbauth_;

		int department_id;   ///< 企业ID
	};

	//4.获取部门下人员信息(包含子部门人员)
	struct t_wb_keda_get_department_allUser
	{
		t_wb_oauth wbauth_;

		int department_id;   ///< 企业ID
	};

	//5.在组织架构中搜索部门信息
	struct t_wb_keda_department_searchDepartment
	{
		t_wb_oauth wbauth_;

		WBChar department_name[ WB_REAL_LEN(OPT_VALUE) ];  ///< 需要查询的企业名称
	};

	//6.在组织架构中搜索人员信息
	struct t_wb_keda_department_searchUser
	{
		t_wb_oauth wbauth_;

		WBChar user_name[ WB_REAL_LEN(OPT_VALUE) ];  ///< 需要查询的用户名字
	};

	// add end


	// 2012-04-05 add by sunfei 增加v1.0接口 第四部分 查询状态相关

	//7.获取部门下的员工的在线状态
	struct t_wb_keda_department_online_by_depId
	{
		t_wb_oauth wbauth_;

		int department_id;      ///< 企业ID
	};

	//8.获取组织里用户的在线状态
	struct t_wb_keda_department_online_by_userId
	{
		t_wb_oauth wbauth_;

		WBChar user_ids[ WB_REAL_LEN(USER_IDS) ];  ///< 用户ID集合如"1,3,5,7"
	};

	//9.获取头像
	struct t_wb_keda_get_portrait
	{
		t_wb_oauth wbauth_;

		WBChar portrait32[ WB_REAL_LEN(URL) ];
		WBChar portrait40[ WB_REAL_LEN(URL) ];
		WBChar portrait64[ WB_REAL_LEN(URL) ];
		WBChar portrait128[ WB_REAL_LEN(URL) ];
		WBChar portrait256[ WB_REAL_LEN(URL) ];
	};

	//10.批量查询账号详细信息
	struct t_wb_keda_ams_batchGetAccountInfo
	{
		t_wb_oauth wbauth_;

		WBChar usernames[ WB_REAL_LEN(MULTI_USER) ];  ///< 账号，多个用分号连接，可以是JID、E164号码或邮箱
	};

	//11.修改用户头像
	struct t_wb_keda_confirmPortrait
	{
		t_wb_oauth wbauth_;

		int x1;              ///< 图像中需要截取的起点位置的x坐标
		int y1;              ///< 图像中需要截取的起点位置的y坐标
		int width;           ///< 截取图像的宽度（注：1，width和height必须一样；2，width + x1 <= 原图的width）
		int height;          ///< 截取头像的高度（heigth + y1 <= 原图的height）

		WBChar filepath[ WB_REAL_LEN(MAX_FILE_PATH) ];   ///< 上传头像文件绝对路径
	};

	//12.获取组织架构版本信息
	struct t_wb_keda_department_get_version
	{
		t_wb_oauth wbauth_;
	};

	//13.带图片和附件的微博发布
	struct t_wb_keda_message_upload
	{
		t_wb_oauth wbauth_;

		long group_id;  ///< 发布到的群组ID(可选)

		WBChar szwbInfo_[WB_REAL_LEN(WB_INFO)];         ///< 微博正文，内容不超过140个汉字

		int fileCount;  ///< 上传的文件数目
		WBChar filepath[10][ WB_REAL_LEN(MAX_FILE_PATH) ];   ///< 上传的附件或者图片，可以是多个
	};

	//14.删除自己的微博
	struct t_wb_keda_message_delete_thread
	{
		t_wb_oauth wbauth_;

		long feed_id;      ///< 需要删除主题消息对应的ID
	};

	//15.删除自己的回复
	struct t_wb_keda_message_delete_reply
	{
		t_wb_oauth wbauth_;

		long feed_id;      ///< 删除回复消息对应的ID
	};

	//16.获取@、赞、粉丝等初始消息
	struct t_wb_keda_notify_get_all
	{
		t_wb_oauth wbauth_;
	};

	//17.保持用户心跳
	struct t_wb_keda_keep_heartbeat
	{
		t_wb_oauth wbauth_;
	};

	//18. 获得用户信息(通过email)
	struct t_wb_keda_get_user_info_by_email 
	{
		t_wb_oauth wbauth_;

		WBChar email[ WB_REAL_LEN(OPT_VALUE) ];
	};

	//19.获得当前登录用户信息 
	struct t_wb_keda_get_current_user_info 
	{
		t_wb_oauth wbauth_;
	};

	//20.获取长篇大论的url 
	struct t_wb_keda_get_article_url 
	{
		t_wb_oauth wbauth_;

		long thread_id;               ///< 长篇大论对应的微博消息主题ID
	};

	//21.修改个人介绍 
	struct t_wb_keda_ams_updateBrief 
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ]; ///< 修改者账号，可以是E164号码或邮箱
		WBChar brief[ WB_REAL_LEN(BRIEF) ];  ///< 一句话简介(若该字段对应的参数未传或者字段值为空则清空简介)
	};

	//22.按姓名模糊查找本企业联系人
	struct t_wb_keda_ams_list_account_by_str 
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];  ///< 查询者账号，可以是E164号码或邮箱
		WBChar name[ WB_REAL_LEN(UID) ];      ///< 模糊匹配的字符串，例如：“孙”
		int startRow;                         ///< 起始行，例如：0
		int pageSize;                         ///< 返回条数，例如：10
	};

	//23.忘记密码
	struct t_wb_keda_ams_password_reset_by_mail 
	{
		t_wb_oauth wbauth_;

		WBChar mail[ WB_REAL_LEN(OPT_VALUE) ];
	};

	//24.获取赞过我的微博数据
	struct t_wb_keda_ams_get_likes_me_data 
	{
		t_wb_oauth wbauth_;

		int current_page;                      ///< 需要获得的当前页面，不输入为第一页(可选)
		int page_size;                         ///< 每页的显示条数，默认为20(可选)
	};



	//25.设置是否被模糊搜索到
	struct t_wb_keda_ams_set_is_like_query 
	{
		t_wb_oauth wbauth_;

		WBChar jid[ WB_REAL_LEN(OPT_VALUE) ];        ///< 账号对应的jid
		int isLikeQuery;                       ///< 是否被模糊搜索到(0:不可以；1:可以)
	};

	//26.设置添加联系人认证模式
	struct t_wb_keda_ams_set_auth_mode 
	{
		t_wb_oauth wbauth_;

		WBChar jid[ WB_REAL_LEN(OPT_VALUE) ];        ///< 账号对应的jid
		int authMode;                          ///< 添加联系人认证模式：(0:无需认证；1:待认证；2:拒绝接受)
	};

	//27.条件查询企业联系人
	struct t_wb_keda_ams_condition_query 
	{
		t_wb_oauth wbauth_;

		int inEnt;                               ///< 查询企业范围：0表示企业外，1表示企业内
		
		WBChar email[ WB_REAL_LEN(OPT_VALUE) ];        ///< 用户账号emai
		WBChar e164[ WB_REAL_LEN(OPT_VALUE) ];         ///< 用户账号e164
		WBChar name[ WB_REAL_LEN(OPT_VALUE) ];         ///< 用户名
		WBChar mobilePhone[ WB_REAL_LEN(OPT_VALUE) ];  ///< 用户手机(企业外不需要)

		int startRow;                            ///< 起始，例如：0（表示从第1条开始），10（表示从第11条开始），依次类推
		int pageSize;                            ///< 返回条数，例如：10
	};
	
	

	// add end


    ////// 会议管理

    // 获取会议列表
    struct t_wb_keda_m_meeting_list
    {
        t_wb_oauth wbauth_;

        WBChar startDay[ WB_REAL_LEN(OPT_DATE) ];       
        WBChar endDay[ WB_REAL_LEN(OPT_DATE) ];    
        int    isConfirm;
        int    start;
        int    count;
        int    order;
    };

    // 获取会议详细信息
    struct t_wb_keda_m_meeting_info
    {
        t_wb_oauth wbauth_;

        int        id;
    };

    // 获取会议室预订的会议信息
    struct t_wb_keda_m_booking_info
    {
        t_wb_oauth wbauth_;

        int        meetingRoomId;
        WBChar     startDay[ WB_REAL_LEN(OPT_DATE) ];       
        WBChar     endDay[ WB_REAL_LEN(OPT_DATE) ]; 
        int        start;
        int        count;
        int        order;
    };

    // 获取某人的会议信息
    struct t_wb_keda_m_meeting_list_of_person
    {
        t_wb_oauth wbauth_;

        WBChar     moId[WB_REAL_LEN(UID)];
        WBChar     startDay[ WB_REAL_LEN(OPT_DATE) ];       
        WBChar     endDay[ WB_REAL_LEN(OPT_DATE) ]; 
        int        start;
        int        count;
        int        order;
    };

    
#define  MEETING_NOTIFY_TYPE_WEIBO     0x01
#define  MEETING_NOTIFY_TYPE_EMAIL     0x02

    // 创建/修改会议
    struct t_wb_keda_m_set_meeting
    {
        t_wb_oauth wbauth_;

        int        meetingId;
        WBChar     subject[WB_REAL_LEN(SUBJECT)];
        WBChar     day[WB_REAL_LEN(OPT_DATE)];
        WBChar     startShortTime[WB_REAL_LEN(OPT_DATE)];
        WBChar     endShortTime[WB_REAL_LEN(OPT_DATE)];
        int        rooms_cnt;
        int        rooms[20];
        WBChar     phone[WB_REAL_LEN(PHONE)];
        WBChar     mobile[WB_REAL_LEN(PHONE)];
        int        participant_cnt;
        WBChar     participants[100][WB_REAL_LEN(UID)];
        WBChar     brief[WB_REAL_LEN(M_BRIEF)];
        int        isVideoMeeting;
        int        isAutoCall;
        WBChar     template_id[WB_REAL_LEN(TEMPLATE_ID)];
        int        vroom_id;
        int        notify_type;
        WBChar     meetingFormKey[WB_REAL_LEN(OPT_VALUE)];
    };

    // 删除会议/开始会议/结束会议
    struct t_wb_keda_m_meeting_id
    {
        t_wb_oauth wbauth_;

        int        meetingId;
    };

#define    JOIN_TYPE_HARDWARE     1
#define    JOIN_TYPE_SOFTWARE     2
#define    JOIN_TYPE_PHONE        3

    // 会议反馈
    struct t_wb_keda_m_meeting_feed
    {
        t_wb_oauth wbauth_;

        int        meetingId;
        int        accept;
        int        joinType;
        WBChar     participateNO[WB_REAL_LEN(OPT_VALUE)];

        int        reasonId;
    };

#define DEVICE_ID_VIDEO_TERMINAL       0x01
#define DEVICE_ID_PROJECTOR            0x02
#define DEVICE_ID_AIR_CONDITIONING     0x04
#define DEVICE_ID_WHITE_BORD           0x08

    // 会议室查询
    struct t_wb_keda_m_query_rooms
    {
        t_wb_oauth wbauth_;

        int        rooms_cnt;
        int        rooms[20];

        WBChar     startTime[ WB_REAL_LEN(OPT_DATE) ];       
        WBChar     endTime[ WB_REAL_LEN(OPT_DATE) ]; 
        int        start;
        int        count;
        int        order;
        int        peopleAdmit;
        int        deviceIdMask;

        WBChar     name[WB_REAL_LEN(OPT_VALUE)];
    };


    // 根据id查询会议室
    struct t_wb_keda_m_id
    {
        t_wb_oauth wbauth_;

        int        id;
    };

    // 通用查询
    struct t_wb_keda_m_common_query
    {
        t_wb_oauth wbauth_;

        int        start;
        int        count;
        int        order;
    };

    struct t_wb_keda_m_common_id
    {
        t_wb_oauth wbauth_;

        WBChar     id[WB_REAL_LEN(OPT_VALUE)];
    };

    struct t_wb_keda_m_notify
    {
        t_wb_oauth wbauth_;

        int        id;
        int        notifyMask;
        int        titleType;
    };



    // 会议室锁定
    struct t_wb_keda_m_lock_room
    {
        t_wb_oauth wbauth_;

        int        room_id;
        WBChar     startTime[ WB_REAL_LEN(OPT_DATE) ];       
        WBChar     endTime[ WB_REAL_LEN(OPT_DATE) ]; 
    };


    struct t_wb_keda_m_list_notify_by_max 
    {
        t_wb_oauth wbauth_;

        int        notifyId;
        int        messageType;
    };


    struct t_wb_keda_m_get_notify_by_id 
    {
        t_wb_oauth wbauth_;

        int        notifyId;
    };


    struct t_wb_keda_m_if_exist_lock_by_formkey 
    {
        t_wb_oauth wbauth_;

        WBChar     formKey[ WB_REAL_LEN(OPT_VALUE) ]; 
    };

///////////////////////////////////////////


	/** loWeibo_get_url()使用 */
	struct t_wb_URI
	{
		char uri_[ WB_REAL_LEN(URL) ];
		char post_[ WB_REAL_LEN(POST) ];
	};

	struct t_wb_REQ_EX
	{
		int format_;
		int cookie_;

		char appkey_[256];
		char secret_[256];

		char dest_ip[64];
		char accountToken[256];

		char cookies[4][512];

		void *pThreadPool;
	};

	typedef void (*fun_req_destroy)(void* pUserdata);

	// 微博请求对象
	typedef void WEIBORequest;	

	typedef enum
	{
		ERR_createhttpobject_nil = -100,
		ERR_checkvalid_fail,
		ERR_url_set_fail,
		ERR_requestParam_nil,
		ERR_reqSessionParam_nil,
		ERR_createhttpSession_nil,
		ERR_Fail = -1,
		ERR_OK = 0,
	}WEIBOError;

	typedef enum
	{
		WEIBO_OPTION_DEFINE(REQ_UNK,0),
		WEIBO_OPTION(REQ_DESTROY_FUNCTION), ///< 当请求被消毁时，通过回调函数告开发者

		WEIBO_OPTION(REQ_SENDDATA), ///< 发送的数据
		WEIBO_OPTION(REQ_USERDATA), ///< 开发者自定义数据,如果设置了它,它会随着回调函数返回给开发者使用
		WEIBO_OPTION(REQ_PROXYDATA),
		WEIBO_OPTION(REQ_RESULT_DATAFORMAT),
		WEIBO_OPTION(REQ_COOKIE),

		WEIBO_OPTION(REQ_APPKEY),
		WEIBO_OPTION(REQ_SECRETKEY),

		WEIBO_OPTION(REQ_ACCOUNTTOKEN),

		WEIBO_OPTION(REQ_COOKIE_1),
		WEIBO_OPTION(REQ_COOKIE_2),
        WEIBO_OPTION(REQ_COOKIE_3),
        WEIBO_OPTION(REQ_COOKIE_4),
        WEIBO_OPTION(REQ_COOKIE_ALL),

		WEIBO_OPTION(REQ_THREAD_POOL_HANDLE),

		WEIBO_OPTION(REQ_DEST_IP),

	    WEIBO_OPTION(REQ_LAST),
	}WEIBOReqoption;

	WEIBO_API void loWeibo_oauth_copy(struct t_wb_oauth* pdest, const struct t_wb_oauth* psrc);

	WEIBO_API WEIBORequest* loWeibo_request_create(void);
	WEIBO_API int  loWeibo_request_destroy(WEIBORequest* request);

	WEIBO_API int  loWeibo_request_setopt(WEIBORequest* request, WEIBOReqoption option, ...);

	WEIBO_API int  loWeibo_request_start(WEIBORequest* request, bool bAsyn);
	WEIBO_API void loWeibo_request_wait(WEIBORequest* request);
	WEIBO_API int  loWeibo_request_stop(WEIBORequest* request);

    WEIBO_API void *  loWeibo_request_get_sendData(WEIBORequest* request);

	/** 
	 * @brief 直接生成URL信息，内部只提供一个算法把所需要的信息组织在一起，方便调用
	 *
	 * @param ［out] outURI,返回生成URL信息
	 * @param  [in] option 参考  WEIBOoption
	 * @param  [in] t_wb   根据 WEIBOoption值对应的不同结构对象
	 * @param  [in] req_ex 一些附加的认证条件内容
	 *
	 * @return 0 ,成功
	 */
	WEIBO_API int  loWeibo_get_url(struct t_wb_URI* outURI/*[out]*/ , WEIBOoption option , const void* t_wb , const struct t_wb_REQ_EX* req_ex);



    typedef struct tagWeiboProxyParam
    {
        int                 nProxyType;               // 0: 不使用proxy;  1. sock5 proxy, 2: http proxy

        char                szProxy[256];             // proxy 地址
        unsigned short      wProxyPort;               // proxy 端口
        char                reserverd0[2];
        char                szUsername[256];          // 用户名
        char                szPassword[256];          // 密码
    }TWeiboProxyParam, *PTWeiboProxyParam;



    typedef  struct  tagRestCookie
    {
        char                szCookieKey[64];
        char                szCookieValue[256];
    }TRestCookie, *PTRestCookie;


    typedef  struct  tagRestPathCookies
    {
        char             szPath[64];
        TRestCookie      cookies[4];
    }TRestPathCookies, *PTRestPathCookies;



    typedef void (*WeiboCallback)(   int nError, unsigned long dwCode, const char * szData, unsigned long dwDataLen, 
                                     const char * szHeader, unsigned long dwHeaderLen,  void * context );

#define  WEIBO_METHOD_GET    0
#define  WEIBO_METHOD_POST   1

    WEIBO_API void loWeibo_init();
    WEIBO_API int  loWeibo_request_startex( const char * szUrl, const char * szParam, int method, 
                               WeiboCallback pfnCallback, void * context, TRestPathCookies * cookies, unsigned long dwCnt );


    WEIBO_API int  loWeiboUrlEncode   ( char * szParams, unsigned long dwSize, const char * szKey, const char * szValue, int bUtf8 );
    WEIBO_API int  loWeiboUrlEncodeInt( char * szParams, unsigned long dwSize, const char * szKey, int  nValue, int bUtf8 );


#ifdef  __cplusplus
}
#endif



/**
 * @exmaple:
 *   
 *  WEIBORequest* pWBReq = loWeibo_request_create();
 *  if( pWBReq )
 *  {
 	   struct t_wb_oauth_request_token   oauth_reqtoken;
	   
	   // 使用json方式
	   loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_JSON);

	   // 请求时，发送的请求包内容
	   loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(OAUTH_REQUEST_TOKEN) , &oauth_reqtoken );

	   // 开始请求
	   loWeibo_request_start(pWBReq , false);

	   // 消毁
       loWeibo_request_destroy(pWBReq);

 *  }
 *
 *
*/


#endif /* __weibo_SDK_H__ */
