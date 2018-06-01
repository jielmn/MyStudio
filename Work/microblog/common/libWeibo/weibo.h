
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

#else  // ����ƽ̨�ģ������Ǿ�̬����
	
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

	WB_MAX_LEN_DEFINE(OPT_VALUE, 64);  // ͨ�ò���

	WB_MAX_LEN_DEFINE(UID, 64);  // ��½�û���
	WB_MAX_LEN_DEFINE(PWD, 64);  // ��½����

	WB_MAX_LEN_DEFINE(OAUTH_TOKEN, 64);        // ��1024��Ϊ64
	WB_MAX_LEN_DEFINE(OAUTH_TOKEN_SECRET, 64); // ��2048��Ϊ64

	WB_MAX_LEN_DEFINE(WB_INFO, 420);       // һ��΢���ı�׼����Ϊ140������280�ֽ�
	WB_MAX_LEN_DEFINE(URL, 1024);          // URL����
#ifndef _ANDROID_
	WB_MAX_LEN_DEFINE(MULTI_USER, 1024);  // ������ѯ�˺���ϸ��Ϣʹ��
#else
	WB_MAX_LEN_DEFINE(MULTI_USER, 16383);   // ������ѯ�˺���ϸ��Ϣʹ��
#endif
	WB_MAX_LEN_DEFINE(MAX_FILE_PATH, 512); // ��ļ�����·��
	WB_MAX_LEN_DEFINE(PM_INFO, 900);       // ÿ��˽�ŵ����ݳ��Ȳ�����300������
	WB_MAX_LEN_DEFINE(BRIEF, 210);         // һ�仰�����ʱ��Ϊ70������
	WB_MAX_LEN_DEFINE(USER_IDS, 256);      // �û�ID����

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

		// 2012-01-04 add by sunfei ������������KEDA API
		WEIBO_OPTION(KEDA_API_P_GET_ACCOUNTTOKEN),     //82 ���Oauth��accountToken(ƽ̨API)            * 2
		WEIBO_OPTION(KEDA_API_W_GET_ACCOUNTTOKEN),     //82 ���Oauth��accountToken(΢��API)            * 3
		WEIBO_OPTION(KEDA_API_P_LOGIN),                //83 ��¼ƽ̨API                                 * 4
		WEIBO_OPTION(KEDA_API_W_LOGIN),                //83 ��¼΢��API                                 * 5
		WEIBO_OPTION(KEDA_API_GET_ME_AND_FRIENDS),     //84 ��õ�ǰ��¼�û�����ע�û���΢��            * 6
		WEIBO_OPTION(KEDA_API_GET_USER_DATA),          //85 ����û�������΢�����ݣ���Ѷ��              * 7
		WEIBO_OPTION(KEDA_API_GET_USER_ALL_DATA),      //86 ����û�������΢�����ݣ�ȫ����              * 8
		WEIBO_OPTION(KEDA_API_GET_SUBJECT_AND_REPLY),  //87 ���һ�������Լ��ظ�                        * 9
		WEIBO_OPTION(KEDA_API_MESSAGE_PUBLISH),        //88 ����һ��΢��                                * 10
		WEIBO_OPTION(KEDA_API_MESSAGE_REPLY),          //89 �ظ�һ��΢��                                * 11
		WEIBO_OPTION(KEDA_API_GET_USER_INFO),          //90 ����û���Ϣ                                * 12
		// add end

		// 2012-02-21 add by sunfei ����v1.0�ӿ� ��һ����
		WEIBO_OPTION(KEDA_API_GET_COMPANY_PUBLIC_DATA),  //91 ��ù�˾�㳡��΢������                    * 13
		WEIBO_OPTION(KEDA_API_GET_GROUPS_DATA),          //92 ���Ⱥ���΢������                        * 14
		WEIBO_OPTION(KEDA_API_GET_MENTIONS_DATA),        //93 ���@��ǰ�û���΢������                   * 15
		WEIBO_OPTION(KEDA_API_GET_REPLIES_DATA),         //94 ��ûظ��ҵ�΢������                      * 16
		WEIBO_OPTION(KEDA_API_GET_LIKES_DATA),           //95 ������޹���΢������                      * 17
		WEIBO_OPTION(KEDA_API_GET_FAVORITES_DATA),       //96 ������ղص�΢������                      * 18
		WEIBO_OPTION(KEDA_API_GET_TOPICS_DATA),          //97 ���ĳ�������΢������                    * 19
		// add end

		// 2012-02-27 add by sunfei ����v1.0�ӿ� �ڶ�����
		WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_COMPANY),   //98 ��ù�˾����                              * 20
		WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_ATTENTION), //99 ����ҵĹ�ע�б�                          * 21
		WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FOLLOWERS), //100 ����ҵķ�˿�б�                         * 22
		WEIBO_OPTION(KEDA_API_GET_FRIENDSHIP_FRIENDS),   //101 ����ҵĺ����б�                         * 23

		WEIBO_OPTION(KEDA_API_FRIENDSHIP_CREATE),      //102 ��ӹ�ע                                   * 24
		WEIBO_OPTION(KEDA_API_FRIENDSHIP_DESTROY),     //103 ȡ����ע                                   * 25
		WEIBO_OPTION(KEDA_API_FAVORITES_CREATE),       //104 ����ղ�                                   * 26
		WEIBO_OPTION(KEDA_API_FAVORITES_DESTROY),      //105 ȡ���ղ�                                   * 27
		WEIBO_OPTION(KEDA_API_TOPIC_CREATE),           //106 ��ӻ���                                   * 28
		WEIBO_OPTION(KEDA_API_TOPIC_DESTROY),          //107 ȡ������                                   * 29
		WEIBO_OPTION(KEDA_API_LIKE_CREATE),            //108 ���like                                   * 30
		WEIBO_OPTION(KEDA_API_LIKE_DESTROY),           //109 ȡ��like                                   * 31

		WEIBO_OPTION(KEDA_API_GET_GROUP_USER),         //110 ��ȡ�ҵ�Ⱥ���б�                           * 32
		WEIBO_OPTION(KEDA_API_GET_GROUP_ALL),          //111 ������е�Ⱥ���б�                         * 33
		WEIBO_OPTION(KEDA_API_GROUP_CREATE),           //112 ���Ⱥ����                                 * 34
		WEIBO_OPTION(KEDA_API_GROUP_JOIN),             //113 ����Ⱥ����                                 * 35
		WEIBO_OPTION(KEDA_API_GROUP_QUIT),             //114 �˳�Ⱥ����                                 * 36
    
		WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_LIST),  //115 �鿴�ҵ���Ϣ�б�                         * 37
		WEIBO_OPTION(KEDA_API_GET_PRIVATEMESSAGE_USER),  //116 �鿴����ĳ�˵�˽�ŵ������¼             * 38
		WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_SEND),      //117 ����˽�ŵ�ĳ��                           * 39
		WEIBO_OPTION(KEDA_API_PRIVATEMESSAGE_DELETE),    //118 ɾ��˽��                                 * 40
		// add end

		// 2012-03-06 add by sunfei ����v1.0�ӿ� ��������
		WEIBO_OPTION(KEDA_API_AMS_VALIDATECLIENTACCOUNT), //119 ��֤�ն��˺�                            * 41
		WEIBO_OPTION(KEDA_API_AMS_GETACCOUNTINFO),        //120 ��ѯ�˺���ϸ��Ϣ                        * 42
		WEIBO_OPTION(KEDA_API_AMS_UPDATEACCOUNTINFO),     //121 �޸��˺���ϸ��Ϣ                        * 43
		WEIBO_OPTION(KEDA_API_AMS_UPDATEPASSWORD),        //122 �޸��˺�����                            * 44
		// add end

		// 2012-03-15 add by sunfei ����v1.0�ӿ� ���Ĳ���
		WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALL),           //123 ��ù�˾��֯�ܹ���������Ա��Ϣ       * 45
		WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_COMPANY),       //124 ��ȡ��˾��֯�ܹ���Ϣ                 * 46
		WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_USER),          //125 ��ȡ��������Ա��Ϣ(�������Ӳ���)     * 47
		WEIBO_OPTION(KEDA_API_GET_DEPARTMENT_ALLUSER),       //126 ��ȡ��������Ա��Ϣ(�����Ӳ�����Ա)   * 48
		WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHDEPARTMENT),  //127 ����֯�ܹ�������������Ϣ             * 49
		WEIBO_OPTION(KEDA_API_DEPARTMENT_SEARCHUSER),        //128 ����֯�ܹ���������Ա��Ϣ             * 50
		// add end
		WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_DEPID),   //129 ��ȡ�����µ�Ա��������״̬           * 51
		WEIBO_OPTION(KEDA_API_DEPARTMENT_ONLINE_BY_USERID),  //130 ��ȡ��֯���û�������״̬             * 52

		WEIBO_OPTION(KEDA_API_GET_PORTRAIT),                 //131 ��ȡͷ��                             * 53
		WEIBO_OPTION(KEDA_API_AMS_BATCHGETACCOUNTINFO),      //132 ������ѯ�˺���ϸ��Ϣ                 * 54
		WEIBO_OPTION(KEDA_API_CONFIRM_PORTRAIT),             //133 �޸��û�ͷ��                         * 55

		WEIBO_OPTION(KEDA_API_DEPARTMENT_GET_VERSION),       //134 ��ȡ��֯�ܹ��汾��Ϣ                 * 56
		WEIBO_OPTION(KEDA_API_MESSAGE_UPLOAD),               //135 ��ͼƬ�͸�����΢������               * 57

		WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_THREAD),        //136 ɾ���Լ���΢��                       * 58
		WEIBO_OPTION(KEDA_API_MESSAGE_DELETE_REPLY),         //137 ɾ���Լ��Ļظ�                       * 59
		WEIBO_OPTION(KEDA_API_NOTIFY_GET_ALL),               //138 ��ȡ@���ޡ���˿�ȳ�ʼ��Ϣ            * 60

		WEIBO_OPTION(KEDA_API_P_KEEP_HEARTBEAT),             //139 �����û�����(ƽ̨API)                * 61
		WEIBO_OPTION(KEDA_API_W_KEEP_HEARTBEAT),             //139 �����û�����(΢��API)                * 62

		WEIBO_OPTION(KEDA_API_GET_USER_INFO_BY_EMAIL),       //140 ����û���Ϣ(ͨ��email)              * 63

		WEIBO_OPTION(KEDA_API_GET_CURRENT_USER_INFO),        //141 ��õ�ǰ��¼�û���Ϣ                 * 64
		WEIBO_OPTION(KEDA_API_GET_ARTICLE_URL),              //142 ��ȡ��ƪ���۵�url                    * 65
		WEIBO_OPTION(KEDA_API_AMS_UPDATEBRIEF),              //143 �޸ĸ��˽���                         * 66
		WEIBO_OPTION(KEDA_API_AMS_LIST_ACCOUNT_BY_STR),      //144 ������ģ�����ұ���ҵ��ϵ��           * 67
		WEIBO_OPTION(KEDA_API_AMS_PASSWORD_RESET_BY_MAIL),   //145 ��������                             * 68
		WEIBO_OPTION(KEDA_API_AMS_GET_LIKES_ME_DATA),        //146 ��ȡ�޹��ҵ�΢������                 * 69

		WEIBO_OPTION(KEDA_API_AMS_SET_IS_LIKE_QUERY),        //147 �����Ƿ�ģ��������                 * 70
		WEIBO_OPTION(KEDA_API_AMS_SET_AUTH_MODE),            //148 ���������ϵ����֤ģʽ               * 71
		WEIBO_OPTION(KEDA_API_AMS_CONDITION_QUERY),          //149 ������ѯ��ҵ��ϵ��                   * 72


        // �������
        WEIBO_OPTION(KEDA_MEETING_GET_ACCOUNTTOKEN),         // 150 ��ȡaccount token                   * 73
        WEIBO_OPTION(KEDA_MEETING_LOGIN),                    // 151 ��¼����ƽ̨API                     * 74
        WEIBO_OPTION(KEDA_MEETING_KEEP_HEARTBEAT),           // 152 �����û�����(ƽ̨API)               * 75

        WEIBO_OPTION(KEDA_MEETING_MEETING_LIST),             // 153 �����б�                            * 76
        WEIBO_OPTION(KEDA_MEETING_MEETING_INFO),             // 154 ������ϸ��Ϣ                        * 77
        WEIBO_OPTION(KEDA_MEETING_BOOKING_INFO),             // 155 ������Ԥ����Ϣ                      * 78
        WEIBO_OPTION(KEDA_MEETING_MEETING_LIST_OF_PERSON),  // 156 ĳ�˵Ļ�����Ϣ                       * 79

        WEIBO_OPTION(KEDA_MEETING_ADD_MEETING),              // 157 ��ӻ���                            * 80
        WEIBO_OPTION(KEDA_MEETING_MODIFY_MEETING),           // 158 �޸Ļ���                            * 81
        WEIBO_OPTION(KEDA_MEETING_DELETE_MEETING),           // 159 ɾ������                            * 82
        WEIBO_OPTION(KEDA_MEETING_START_MEETING),            // 160 ��ʼ����                            * 83
        WEIBO_OPTION(KEDA_MEETING_END_MEETING),              // 161 ��������                            * 84
        WEIBO_OPTION(KEDA_MEETING_FEED),                     // 162 ���鷴��                            * 85

        WEIBO_OPTION(KEDA_MEETING_QUERY_FREE_ROOMS),                     // 163 ��ѯ������              * 86
        WEIBO_OPTION(KEDA_MEETING_RECOMMENDED_V_ROOM),                     // 164 �Ƽ������������      * 87
        WEIBO_OPTION(KEDA_MEETING_NORMAL_ROOMS),                     // 165 �Ƽ������������            * 88
        WEIBO_OPTION(KEDA_MEETING_ROOM_INFO),                        // 166 ��ѯ��������Ϣ              * 89
        WEIBO_OPTION(KEDA_MEETING_ROOM_STATUS),                        // 167 ��ѯ������״̬            * 90
        WEIBO_OPTION(KEDA_MEETING_ROOM_LIST_BY_NAME),                  // 168 �������Ʋ�ѯ������        * 91
        WEIBO_OPTION(KEDA_MEETING_QUERY_ROOMS),                       // 169 ��ѯ������                 * 92

        WEIBO_OPTION(KEDA_MEETING_TEMPLATE_LIST),                       // 170 ��ѯ����ģ��             * 93
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
		int  cookie_; ///< �Ƿ�cookie��ʽ

		fun_wb_status_callback fstatus_cb;
		fun_wb_body_callback   fbody_cb;
		fun_wb_header_callback fheader_cb;
	};

	/** ���ṹ����������н���ת���� */
	struct t_wb_base
	{
		t_wb_oauth wbauth_;
	};

 
	/*	��������,�Զ���� */
	struct t_wb_custom
	{
		t_wb_oauth wbauth_;

		WBChar szUrl_[WB_REAL_LEN(URL)];
		WBChar szPost_[WB_REAL_LEN(POST)]; ///< ����s
		int    method_;                    ///< ���󷽷� POST/GET
	};


///////////////////////////////////////////////////////////////////////

	//1. ���Oauth��accountToken
	struct t_wb_keda_get_accounttoken
	{
		t_wb_oauth wbauth_;

// 		WBChar ip[ WB_REAL_LEN(IP) ];
	};

	//2. �û���¼
	struct t_wb_keda_login
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];  ///< �û���
		WBChar password[ WB_REAL_LEN(PWD) ];  ///< �û�����
	};

	//3. ��õ�ǰ��¼�û�����ע�û���΢��
	struct t_wb_keda_get_me_and_friends
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//4. ����û�������΢�����ݣ���Ѷ��
	struct t_wb_keda_get_user_data
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];  ///< �û���UID

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//5. ����û�������΢�����ݣ�ȫ����
	struct t_wb_keda_get_user_all_data 
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];  ///< �û���UID

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//6. ���һ�������Լ��ظ�
	struct t_wb_keda_get_subject_and_reply
	{
		t_wb_oauth wbauth_;

		long thread_id;        ///< ����ID

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//7. ����һ��΢��
	struct t_wb_keda_message_publish
	{
		t_wb_oauth wbauth_;

		long group_id;                          ///< ������Ⱥ��(��ѡ)
		WBChar status[WB_REAL_LEN(WB_INFO)];    ///< ����΢�����ݣ���Ϣ���ݲ��ܳ���140������
	};

	//8. �ظ�һ��΢��
	struct t_wb_keda_message_reply 
	{
		t_wb_oauth wbauth_;

		long thread_id;                        ///< ��Ҫ���۵�΢��ID������ID��
		long reply_id;                         ///< �ظ����˵���ϢID�������Ӧ��feedId�����߻ظ���Ӧ��feedId��
		WBChar comment[WB_REAL_LEN(WB_INFO)];  ///< �ظ�΢�����ݣ���Ϣ���ݲ�����140������
	};

	//9. ����û���Ϣ
	struct t_wb_keda_get_user_info 
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];  ///< ��Ҫ��ѯ���û�uid
	};

	// update end



	//2012-02-21 add by sunfei ʵ��v1.0�ӿ� ��һ����

	//1.��ù�˾�㳡��΢������
	struct t_wb_keda_get_company_public_data
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//2.���Ⱥ���΢������
	struct t_wb_keda_get_groups_data
	{
		t_wb_oauth wbauth_;

		long group_id;         ///< Ⱥ���ID

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//3.���@��ǰ�û���΢������
	struct t_wb_keda_get_mentions_data
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//4.��ûظ��ҵ�΢������
	struct t_wb_keda_get_replies_data
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//5.������޹���΢������
	struct t_wb_keda_get_likes_data
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];  ///< �û�uid

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//6.������ղص�΢������
	struct t_wb_keda_get_favorites_data
	{
		t_wb_oauth wbauth_;

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};

	//7.���ĳ�������΢������
	struct t_wb_keda_get_topics_data
	{
		t_wb_oauth wbauth_;

		long topic_id;         ///< ����ID

		long since_id;         ///< ��ָ���˲������򷵻�ID��since_id���΢��������since_idʱ�����΢������Ĭ��Ϊ0(��ѡ)
		long max_id;           ///< ��ָ���˲������򷵻�IDС�ڻ����max_id��΢����Ĭ��Ϊ0(��ѡ)
		int count;             ///< ��ҳ���صļ�¼������Ĭ��Ϊ20(��ѡ)
		int reply_count;       ///< Ϊÿ��΢�����صĻظ�����Ĭ��Ϊ0(��ѡ)
		int reply_style;       ///< �ظ�������ʽ��0�������򣬴ӵ�һ���ظ���ʼ��1��ʾ���򣬴����һ����ʼ��Ĭ��Ϊ0(��ѡ)
	};
	// add end



	//2012-02-27 add by sunfei ʵ��v1.0�ӿ� �ڶ�����

	//8.��ù�˾����
	struct t_wb_keda_get_friendship_company
	{
		t_wb_oauth wbauth_;

		int current_page;                      ///< ��Ҫ��õĵ�ǰҳ�棬������Ϊ��һҳ(��ѡ)
		int page_size;                         ///< ÿҳ����ʾ������Ĭ��Ϊ20(��ѡ)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< ��ѯ�Ĳ���(��ѡ)
	};

	//9.����ҵĹ�ע�б�
	struct t_wb_keda_get_friendship_attention
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];        ///< �û�uid

		int current_page;                      ///< ��Ҫ��õĵ�ǰҳ�棬������Ϊ��һҳ(��ѡ)
		int page_size;                         ///< ÿҳ����ʾ������Ĭ��Ϊ20(��ѡ)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< ��ѯ�Ĳ���(��ѡ)
	};

	//10.����ҵķ�˿�б�
	struct t_wb_keda_get_friendship_followers
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];        ///< �û�uid

		int current_page;                      ///< ��Ҫ��õĵ�ǰҳ�棬������Ϊ��һҳ(��ѡ)
		int page_size;                         ///< ÿҳ����ʾ������Ĭ��Ϊ20(��ѡ)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< ��ѯ�Ĳ���(��ѡ)
	};

	//11.����ҵĺ����б�
	struct t_wb_keda_get_friendship_friends
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];        ///< �û�uid

		int current_page;                      ///< ��Ҫ��õĵ�ǰҳ�棬������Ϊ��һҳ(��ѡ)
		int page_size;                         ///< ÿҳ����ʾ������Ĭ��Ϊ20(��ѡ)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< ��ѯ�Ĳ���(��ѡ)
	};

	//12.��ӹ�ע 
	struct t_wb_keda_friendship_create
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];       ///< ��ע�û���uid
	};

	//13.ȡ����ע
	struct t_wb_keda_friendship_destroy
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];       ///< ��ע�û���uid
	};

	//14.����ղ�
	struct t_wb_keda_favorites_create
	{
		t_wb_oauth wbauth_;

		long thread_id;                       ///< �ղص�����ID
	};

	//15.ȡ���ղ�
	struct t_wb_keda_favorites_destroy
	{
		t_wb_oauth wbauth_;

		long thread_id;                       ///< �ղص�����ID
	};

	//16.��ӻ���
	struct t_wb_keda_topic_create
	{
		t_wb_oauth wbauth_;

		long thread_id;                                ///< ���������ID
		WBChar topic_name[ WB_REAL_LEN(OPT_VALUE) ];   ///< ���������name
	};

	//17.ȡ������
	struct t_wb_keda_topic_destroy
	{
		t_wb_oauth wbauth_;

		long thread_id;    ///< ���������ID
		long topic_id;     ///< �����ID
	};

	//18.���like
	struct t_wb_keda_like_create
	{
		t_wb_oauth wbauth_;

		long thread_id;    ///< �޵�����ID
		long feed_id;      ///< �޵�feed_id
	};

	//19.ȡ��like
	struct t_wb_keda_like_destroy
	{
		t_wb_oauth wbauth_;

		long thread_id;    ///< �޵�����ID
		long feed_id;      ///< �޵�feed_id
	};



	//20.��ȡ�ҵ�Ⱥ���б�
	struct t_wb_keda_get_group_user
	{
		t_wb_oauth wbauth_;

		int current_page;                      ///< ��Ҫ��õĵ�ǰҳ�棬������Ϊ��һҳ(��ѡ)
		int page_size;                         ///< ÿҳ����ʾ������Ĭ��Ϊ20(��ѡ)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< ��ѯ�Ĳ���(��ѡ)
	};

	//21.������е�Ⱥ���б�
	struct t_wb_keda_get_group_all
	{
		t_wb_oauth wbauth_;

		int current_page;                      ///< ��Ҫ��õĵ�ǰҳ�棬������Ϊ��һҳ(��ѡ)
		int page_size;                         ///< ÿҳ����ʾ������Ĭ��Ϊ20(��ѡ)
		WBChar key[ WB_REAL_LEN(OPT_VALUE) ];  ///< ��ѯ�Ĳ���(��ѡ)
	};

	//22.���Ⱥ����
	struct t_wb_keda_group_create
	{
		t_wb_oauth wbauth_;

		WBChar group_name[ WB_REAL_LEN(OPT_VALUE) ];    ///< ������
		WBChar group_briefly[ WB_REAL_LEN(OPT_VALUE) ]; ///< �����
	};

	//23.����Ⱥ����
	struct t_wb_keda_group_join
	{
		t_wb_oauth wbauth_;

		WBChar guid[ WB_REAL_LEN(OPT_VALUE) ]; ///< ���uid
	};

	//24.�˳�Ⱥ����
	struct t_wb_keda_group_quit
	{
		t_wb_oauth wbauth_;

		WBChar guid[ WB_REAL_LEN(OPT_VALUE) ]; ///< ���uid
	};


	//25.�鿴�ҵ���Ϣ�б�
	struct t_wb_keda_get_privatemessage_list
	{
		t_wb_oauth wbauth_;

		int current_page;                     ///< ��Ҫ��õĵ�ǰҳ�棬������Ϊ��һҳ(��ѡ)
		int page_size;                        ///< ÿҳ����ʾ������Ĭ��Ϊ20(��ѡ)
	};

	//26.�鿴����ĳ�˵�˽�ŵ������¼ 
	struct t_wb_keda_get_privatemessage_user
	{
		t_wb_oauth wbauth_;

		int current_page;                     ///< ��Ҫ��õĵ�ǰҳ�棬������Ϊ��һҳ(��ѡ)
		int page_size;                        ///< ÿҳ����ʾ������Ĭ��Ϊ20(��ѡ)
		WBChar uid[ WB_REAL_LEN(UID) ];       ///< �û�UID��"sunfei"
	};

	//27.����˽�ŵ�ĳ��
	struct t_wb_keda_privatemessage_send
	{
		t_wb_oauth wbauth_;

		WBChar uid[ WB_REAL_LEN(UID) ];       ///< �û�UID��"sunfei"
		WBChar body[ WB_REAL_LEN(PM_INFO) ];  ///< ˽�����ݣ�������300��
	};

	//28.ɾ��˽��
	struct t_wb_keda_privatemessage_delete
	{
		t_wb_oauth wbauth_;

		long id;   ///< ˽��ID
	};

	// add end




	// 2012-03-06 add by sunfei ����v1.0�ӿ� ��������

	//11.��֤�ն��˺�
	struct t_wb_keda_ams_validateClientAccount
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];         ///< �˺ţ�������E164���������
		WBChar password[ WB_REAL_LEN(PWD) ];         ///< ����
		WBChar deviceType[ WB_REAL_LEN(OPT_VALUE) ]; ///< ��¼���豸����
		WBChar apiHost[ WB_REAL_LEN(IP) ];           ///< ���ӵ�api��ַ
	};

	//12.��ѯ�˺���ϸ��Ϣ
	struct t_wb_keda_ams_getAccountInfo
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];         ///< �˺ţ�������E164���������
	};

    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_MOBILE      0x01
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_EXT         0x02
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_SEAT        0x04
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_POSITION    0x08
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_BRIEF       0x10
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_SEX         0x20
    #define UPDATE_ACCOUNT_INFO_BIT_INDEX_NAME        0x40

	//13.�޸��˺���ϸ��Ϣ
	struct t_wb_keda_ams_updateAccountInfo
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];         ///< �˺ţ�������E164���������
        int    bitMask;

		WBChar mobileNum[ WB_REAL_LEN(OPT_VALUE) ];  ///< ��ϵ�绰(��ѡ)
		WBChar extNum[ WB_REAL_LEN(OPT_VALUE) ];     ///< �ֻ�(��ѡ)
		WBChar seat[ WB_REAL_LEN(OPT_VALUE) ];       ///< ��λ��(��ѡ)
		WBChar position[ WB_REAL_LEN(OPT_VALUE) ];   ///< ְλ(��ѡ)
		WBChar brief[ WB_REAL_LEN(BRIEF) ];          // һ�仰���(��ѡ)
        int    sex;                                  // �Ա�(��ѡ)
        WBChar name[ WB_REAL_LEN(OPT_VALUE) ];       // ����(��ѡ)
	};

	//14.�޸��˺�����
	struct t_wb_keda_ams_updatePassword
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];  ///< �˺ţ�������E164���������
		WBChar oldPassword[ WB_REAL_LEN(PWD) ];  ///< ԭ����
		WBChar newPassword[ WB_REAL_LEN(PWD) ];  ///< ������
	};

	// add end




	// 2012-03-15 add by sunfei ����v1.0�ӿ� ���Ĳ���

	//1.��ù�˾��֯�ܹ���������Ա��Ϣ
	struct t_wb_keda_get_department_all
	{
		t_wb_oauth wbauth_;
	};

	//2..��ȡ��˾��֯�ܹ���Ϣ
	struct t_wb_keda_get_department_company
	{
		t_wb_oauth wbauth_;

		int department_id;   ///< ��ҵID(��ѡ)
	};

	//3.��ȡ��������Ա��Ϣ(�������Ӳ���)
	struct t_wb_keda_get_department_user
	{
		t_wb_oauth wbauth_;

		int department_id;   ///< ��ҵID
	};

	//4.��ȡ��������Ա��Ϣ(�����Ӳ�����Ա)
	struct t_wb_keda_get_department_allUser
	{
		t_wb_oauth wbauth_;

		int department_id;   ///< ��ҵID
	};

	//5.����֯�ܹ�������������Ϣ
	struct t_wb_keda_department_searchDepartment
	{
		t_wb_oauth wbauth_;

		WBChar department_name[ WB_REAL_LEN(OPT_VALUE) ];  ///< ��Ҫ��ѯ����ҵ����
	};

	//6.����֯�ܹ���������Ա��Ϣ
	struct t_wb_keda_department_searchUser
	{
		t_wb_oauth wbauth_;

		WBChar user_name[ WB_REAL_LEN(OPT_VALUE) ];  ///< ��Ҫ��ѯ���û�����
	};

	// add end


	// 2012-04-05 add by sunfei ����v1.0�ӿ� ���Ĳ��� ��ѯ״̬���

	//7.��ȡ�����µ�Ա��������״̬
	struct t_wb_keda_department_online_by_depId
	{
		t_wb_oauth wbauth_;

		int department_id;      ///< ��ҵID
	};

	//8.��ȡ��֯���û�������״̬
	struct t_wb_keda_department_online_by_userId
	{
		t_wb_oauth wbauth_;

		WBChar user_ids[ WB_REAL_LEN(USER_IDS) ];  ///< �û�ID������"1,3,5,7"
	};

	//9.��ȡͷ��
	struct t_wb_keda_get_portrait
	{
		t_wb_oauth wbauth_;

		WBChar portrait32[ WB_REAL_LEN(URL) ];
		WBChar portrait40[ WB_REAL_LEN(URL) ];
		WBChar portrait64[ WB_REAL_LEN(URL) ];
		WBChar portrait128[ WB_REAL_LEN(URL) ];
		WBChar portrait256[ WB_REAL_LEN(URL) ];
	};

	//10.������ѯ�˺���ϸ��Ϣ
	struct t_wb_keda_ams_batchGetAccountInfo
	{
		t_wb_oauth wbauth_;

		WBChar usernames[ WB_REAL_LEN(MULTI_USER) ];  ///< �˺ţ�����÷ֺ����ӣ�������JID��E164���������
	};

	//11.�޸��û�ͷ��
	struct t_wb_keda_confirmPortrait
	{
		t_wb_oauth wbauth_;

		int x1;              ///< ͼ������Ҫ��ȡ�����λ�õ�x����
		int y1;              ///< ͼ������Ҫ��ȡ�����λ�õ�y����
		int width;           ///< ��ȡͼ��Ŀ�ȣ�ע��1��width��height����һ����2��width + x1 <= ԭͼ��width��
		int height;          ///< ��ȡͷ��ĸ߶ȣ�heigth + y1 <= ԭͼ��height��

		WBChar filepath[ WB_REAL_LEN(MAX_FILE_PATH) ];   ///< �ϴ�ͷ���ļ�����·��
	};

	//12.��ȡ��֯�ܹ��汾��Ϣ
	struct t_wb_keda_department_get_version
	{
		t_wb_oauth wbauth_;
	};

	//13.��ͼƬ�͸�����΢������
	struct t_wb_keda_message_upload
	{
		t_wb_oauth wbauth_;

		long group_id;  ///< ��������Ⱥ��ID(��ѡ)

		WBChar szwbInfo_[WB_REAL_LEN(WB_INFO)];         ///< ΢�����ģ����ݲ�����140������

		int fileCount;  ///< �ϴ����ļ���Ŀ
		WBChar filepath[10][ WB_REAL_LEN(MAX_FILE_PATH) ];   ///< �ϴ��ĸ�������ͼƬ�������Ƕ��
	};

	//14.ɾ���Լ���΢��
	struct t_wb_keda_message_delete_thread
	{
		t_wb_oauth wbauth_;

		long feed_id;      ///< ��Ҫɾ��������Ϣ��Ӧ��ID
	};

	//15.ɾ���Լ��Ļظ�
	struct t_wb_keda_message_delete_reply
	{
		t_wb_oauth wbauth_;

		long feed_id;      ///< ɾ���ظ���Ϣ��Ӧ��ID
	};

	//16.��ȡ@���ޡ���˿�ȳ�ʼ��Ϣ
	struct t_wb_keda_notify_get_all
	{
		t_wb_oauth wbauth_;
	};

	//17.�����û�����
	struct t_wb_keda_keep_heartbeat
	{
		t_wb_oauth wbauth_;
	};

	//18. ����û���Ϣ(ͨ��email)
	struct t_wb_keda_get_user_info_by_email 
	{
		t_wb_oauth wbauth_;

		WBChar email[ WB_REAL_LEN(OPT_VALUE) ];
	};

	//19.��õ�ǰ��¼�û���Ϣ 
	struct t_wb_keda_get_current_user_info 
	{
		t_wb_oauth wbauth_;
	};

	//20.��ȡ��ƪ���۵�url 
	struct t_wb_keda_get_article_url 
	{
		t_wb_oauth wbauth_;

		long thread_id;               ///< ��ƪ���۶�Ӧ��΢����Ϣ����ID
	};

	//21.�޸ĸ��˽��� 
	struct t_wb_keda_ams_updateBrief 
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ]; ///< �޸����˺ţ�������E164���������
		WBChar brief[ WB_REAL_LEN(BRIEF) ];  ///< һ�仰���(�����ֶζ�Ӧ�Ĳ���δ�������ֶ�ֵΪ������ռ��)
	};

	//22.������ģ�����ұ���ҵ��ϵ��
	struct t_wb_keda_ams_list_account_by_str 
	{
		t_wb_oauth wbauth_;

		WBChar username[ WB_REAL_LEN(UID) ];  ///< ��ѯ���˺ţ�������E164���������
		WBChar name[ WB_REAL_LEN(UID) ];      ///< ģ��ƥ����ַ��������磺���
		int startRow;                         ///< ��ʼ�У����磺0
		int pageSize;                         ///< �������������磺10
	};

	//23.��������
	struct t_wb_keda_ams_password_reset_by_mail 
	{
		t_wb_oauth wbauth_;

		WBChar mail[ WB_REAL_LEN(OPT_VALUE) ];
	};

	//24.��ȡ�޹��ҵ�΢������
	struct t_wb_keda_ams_get_likes_me_data 
	{
		t_wb_oauth wbauth_;

		int current_page;                      ///< ��Ҫ��õĵ�ǰҳ�棬������Ϊ��һҳ(��ѡ)
		int page_size;                         ///< ÿҳ����ʾ������Ĭ��Ϊ20(��ѡ)
	};



	//25.�����Ƿ�ģ��������
	struct t_wb_keda_ams_set_is_like_query 
	{
		t_wb_oauth wbauth_;

		WBChar jid[ WB_REAL_LEN(OPT_VALUE) ];        ///< �˺Ŷ�Ӧ��jid
		int isLikeQuery;                       ///< �Ƿ�ģ��������(0:�����ԣ�1:����)
	};

	//26.���������ϵ����֤ģʽ
	struct t_wb_keda_ams_set_auth_mode 
	{
		t_wb_oauth wbauth_;

		WBChar jid[ WB_REAL_LEN(OPT_VALUE) ];        ///< �˺Ŷ�Ӧ��jid
		int authMode;                          ///< �����ϵ����֤ģʽ��(0:������֤��1:����֤��2:�ܾ�����)
	};

	//27.������ѯ��ҵ��ϵ��
	struct t_wb_keda_ams_condition_query 
	{
		t_wb_oauth wbauth_;

		int inEnt;                               ///< ��ѯ��ҵ��Χ��0��ʾ��ҵ�⣬1��ʾ��ҵ��
		
		WBChar email[ WB_REAL_LEN(OPT_VALUE) ];        ///< �û��˺�emai
		WBChar e164[ WB_REAL_LEN(OPT_VALUE) ];         ///< �û��˺�e164
		WBChar name[ WB_REAL_LEN(OPT_VALUE) ];         ///< �û���
		WBChar mobilePhone[ WB_REAL_LEN(OPT_VALUE) ];  ///< �û��ֻ�(��ҵ�ⲻ��Ҫ)

		int startRow;                            ///< ��ʼ�����磺0����ʾ�ӵ�1����ʼ����10����ʾ�ӵ�11����ʼ������������
		int pageSize;                            ///< �������������磺10
	};
	
	

	// add end


    ////// �������

    // ��ȡ�����б�
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

    // ��ȡ������ϸ��Ϣ
    struct t_wb_keda_m_meeting_info
    {
        t_wb_oauth wbauth_;

        int        id;
    };

    // ��ȡ������Ԥ���Ļ�����Ϣ
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

    // ��ȡĳ�˵Ļ�����Ϣ
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

    // ����/�޸Ļ���
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

    // ɾ������/��ʼ����/��������
    struct t_wb_keda_m_meeting_id
    {
        t_wb_oauth wbauth_;

        int        meetingId;
    };

#define    JOIN_TYPE_HARDWARE     1
#define    JOIN_TYPE_SOFTWARE     2
#define    JOIN_TYPE_PHONE        3

    // ���鷴��
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

    // �����Ҳ�ѯ
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


    // ����id��ѯ������
    struct t_wb_keda_m_id
    {
        t_wb_oauth wbauth_;

        int        id;
    };

    // ͨ�ò�ѯ
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



    // ����������
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


	/** loWeibo_get_url()ʹ�� */
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

	// ΢���������
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
		WEIBO_OPTION(REQ_DESTROY_FUNCTION), ///< ����������ʱ��ͨ���ص������濪����

		WEIBO_OPTION(REQ_SENDDATA), ///< ���͵�����
		WEIBO_OPTION(REQ_USERDATA), ///< �������Զ�������,�����������,�������Żص��������ظ�������ʹ��
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
	 * @brief ֱ������URL��Ϣ���ڲ�ֻ�ṩһ���㷨������Ҫ����Ϣ��֯��һ�𣬷������
	 *
	 * @param ��out] outURI,��������URL��Ϣ
	 * @param  [in] option �ο�  WEIBOoption
	 * @param  [in] t_wb   ���� WEIBOoptionֵ��Ӧ�Ĳ�ͬ�ṹ����
	 * @param  [in] req_ex һЩ���ӵ���֤��������
	 *
	 * @return 0 ,�ɹ�
	 */
	WEIBO_API int  loWeibo_get_url(struct t_wb_URI* outURI/*[out]*/ , WEIBOoption option , const void* t_wb , const struct t_wb_REQ_EX* req_ex);



    typedef struct tagWeiboProxyParam
    {
        int                 nProxyType;               // 0: ��ʹ��proxy;  1. sock5 proxy, 2: http proxy

        char                szProxy[256];             // proxy ��ַ
        unsigned short      wProxyPort;               // proxy �˿�
        char                reserverd0[2];
        char                szUsername[256];          // �û���
        char                szPassword[256];          // ����
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
	   
	   // ʹ��json��ʽ
	   loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_RESULT_DATAFORMAT) , E_FORMAT_JSON);

	   // ����ʱ�����͵����������
	   loWeibo_request_setopt(pWBReq , WEIBO_OPTION(REQ_SENDDATA) ,  WEIBO_OPTION(OAUTH_REQUEST_TOKEN) , &oauth_reqtoken );

	   // ��ʼ����
	   loWeibo_request_start(pWBReq , false);

	   // ����
       loWeibo_request_destroy(pWBReq);

 *  }
 *
 *
*/


#endif /* __weibo_SDK_H__ */
