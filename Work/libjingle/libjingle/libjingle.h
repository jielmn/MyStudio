/*****************************************************************************
模块名      : 
文件名      : libjingle.h
相关文件    : 
文件实现功能: 提供致邻libjingle封装后的库，简化libjingle的使用
作者        : 
版本        : Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期          版本        修改人      修改内容
2011-09-27		
******************************************************************************/

#ifndef _LIB_JINGLE_HEADER_2011_09_27_
#define _LIB_JINGLE_HEADER_2011_09_27_

//#include <string>
//#include <vector>

#ifdef WIN32
#ifdef LIBJINGLE_EXPORTS
#define LIBJINGLE_API __declspec(dllexport)
#else
#define LIBJINGLE_API __declspec(dllimport)
#endif
#else
#define LIBJINGLE_API 
#endif

#ifndef BOOL
#define BOOL   int
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define  _LIBJINGLE_VERSION_            "1.0.0.0"

namespace LibJingleEx
{
	// 提供接口的通用返回值
	enum ELibjingleError
	{
		LIBJINGLE_ERR_OK                       = 0,          // 成功--0
		LIBJINGLE_ERR_ALREADY_INITED,                        // 已经初始化--1
		LIBJINGLE_ERR_NOT_INITED,                            // 没有初始化--2
		LIBJINGLE_ERR_INVALID_ARGUMENT,                      // 无效参数--3
		LIBJINGLE_ERR_INVALID_JID,                           // 无效的Jid格式--4
		LIBJINGLE_ERR_NOT_ENOUGH_MEM,                        // 没有内存--5
        LIBJINGLE_ERR_FAILED_TO_INIT_SSL,                    // 初始化ssl失败--6
        LIBJINGLE_ERR_TOO_LONG_STRING,                       // 字符串太长--7
        LIBJINGLE_ERR_ALREADY_EXISTS,                        // 已经存在，不需添加--8
        LIBJINGLE_ERR_NO_SUCH_FRIEND,                        // 没有找到好友--9
        LIBJINGLE_ERR_NO_SUCH_GROUP,                         // 没有找到组--10
        LIBJINGLE_ERR_NOT_LOGIN,                             // 还没有登录--11
        LIBJINGLE_ERR_NOT_CHATROOM_SERVICE,                  // 没有chatroom服务--12
        LIBJINGLE_ERR_ROOM_NOT_READY,                        // 房间没有准备好--13
        LIBJINGLE_ERR_INVITEE_ALREADY_IN_ROOM,               // 群里已存在此联系人--14
        LIBJINGLE_ERR_NOT_OWNER,                             // --15
        LIBJINGLE_ERR_NO_SUCH_PARTICIPANT,                   // --16
        LIBJINGLE_ERR_INVALID_ROOM,                          // --非法的群ID--17
        LIBJINGLE_ERR_NOT_FOUND,                             // --18
        LIBJINGLE_ERR_INVALID_PRESENCE,                      // 非法的状态--19
        LIBJINGLE_ERR_NO_FILE_SHARE_ABILITY,                 // 自己没有文件传输能力--20
        LIBJINGLE_ERR_PEER_NO_FILE_SHARE_ABILITY,            // 对方没有文件传输能力--21
        LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY,            // 私有数据没有准备好--22
        LIBJINGLE_ERR_DMS_NO_SUCH_PARENT_GROUP,              // 父组不存在--23
        LIBJINGLE_ERR_DMS_REACH_MAX_SUB_GROUP_COUNT,         // 子组数目已经最大化--24
        LIBJINGLE_ERR_DMS_REACH_MAX_SUB_MEMBER_COUNT,        // 组成员数目已经最大化--25
        LIBJINGLE_ERR_DMS_NO_SUCH_GROUP,                     // 组不存在--26
        LIBJINGLE_ERR_DMS_HAS_SUB_GROUP,                     // 子组已存在--27
        LIBJINGLE_ERR_DMS_NOT_ENOUGH_BUFF,                   // 缓存满--28
        LIBJINGLE_ERR_DMS_FRIEND_ALREADY_IN_THE_GROUP,       // 组内已存在此联系人--29
        LIBJINGLE_ERR_DMS_CANNOT_ADD_SELF,                   // 不能添加自己为组成员--30
        LIBJINGLE_ERR_DMS_NO_SUCH_FRIEND,                    // 不存在此联系人--31
        LIBJINGLE_ERR_DMS_MOVE_METHOD_FORBIDDEN,             // 非法操作--32
        LIBJINGLE_ERR_DMS_FORBID_ADD_SUBGROUP,               // 禁止在“未分组”添加子组--33
        LIBJINGLE_ERR_NO_PUBSUB_SERVICE,
        LIBJINGLE_ERR_DMS_REACH_MAX_TOTOL_MEMBERS_COUNT,    // 达到最大成员数
        LIBJINGLE_ERR_NOT_PERSISTENT_ROOM,                  // 不是持久性房间
        LIBJINGLE_ERR_PARAM_BUFF_FULL,                      // 参数buff已满（请再次调用） 
        LIBJINGLE_ERR_CANNOT_INVITE_SELF,                   // 不能邀请自己到聊天室
        LIBJINGLE_ERR_INVALID_HANDLE,                       // 无效句柄( hApp, 多人聊天句柄， 文件传输句柄 )
        LIBJINGLE_ERR_DISCONNECTED,                         // 链接已断开
        LIBJINGLE_ERR_REACH_MAX_TEMP_CHATROOM_LIMIT,        // 创建非视频聊天室(临时的或固定的上限)
        LIBJINGLE_ERR_REACH_MAX_PERSISTENT_CHATROOM_LIMIT,  // 创建非视频聊天室(临时的或固定的上限)
        LIBJINGLE_ERR_CHATROOM_ROSTER_NOT_READY,            // 还没有获取聊天室的成员列表
        LIBJINGLE_ERR_CHATROOM_ROSTER_IS_FULL,              // 聊天室的成员个数已满(192个)
        LIBJINGLE_ERR_FAILED_TO_OPEN_FILE,                  // 打开文件失败
        LIBJINGLE_ERR_BUSY,                                 // 正忙
        LIBJINGLE_ERR_NO_SUCH_CHATROOM,                     // 没有这样的房间
        LIBJINGLE_ERR_NOT_ALLOWED,                          // 不允许
        LIBJINGLE_ERR_CHAR_STATE_NOT_CHANGED,               // 输入状态没有改变
        LIBJINGLE_ERR_CONFIG_NOT_READY,                     // 私有数据中的配置还没有获取到
        LIBJINGLE_ERR_NOT_GET_SERVER_TIME,                  // 还没有获取到服务器时间
        LIBJINGLE_ERR_CHAT_MSG_NO_RESPONSE,                 // 发送聊天消息后没有回复是否发送成功
        LIBJINGLE_ERR_PARENT_GROUP_NOT_SUPPORTED,           // ROSTER不支持父组
        LIBJINGLE_ERR_ROSTER_GROUP_EXISTS,                  // ROSTER组已经存在
        LIBJINGLE_ERR_NO_SUCH_ROSTER_GROUP,                 // ROSTER组不存在
        LIBJINGLE_ERR_ROSTER_GROUP_TO_BE_DELETED,           // 将要被删除的ROSTER组
        LIBJINGLE_ERR_ROSTER_CONTACT_EXISTS,                // ROSTER里已经存在该联系人
        LIBJINGLE_ERR_ROSTER_NO_CONTACT,                    // ROSTER里不存在该联系人
        LIBJINGLE_ERR_ROSTER_NOT_READY,                     // 还没有获取到roster数据
        LIBJINGLE_ERR_ROSTER_UNSCRIBED_CONTACT,             // 没有订阅状态的联系人
        LIBJINGLE_ERR_CANT_MODIFY_SPECIAL_GROUP,            // 不能修改“未分组”
        LIBJINGLE_ERR_ROSTER_GROUP_NAME_CONFLICT,           // 联系人组名冲突
        
		LIBJINGLE_ERR_ERROR                    = -1
	};

	enum ELibjingleState
	{
		STATE_NONE = 0,          // 没有用
		STATE_START,             // 启动
		STATE_OPENING,           // 正在连接服务器
		STATE_OPEN,              // 连接上服务器
		STATE_CLOSED             // 关闭
	};

    enum ELibjingleStateSubCode
    {
        STATE_SUBCODE_NONE      = 0,
        STATE_SUBCODE_UNAUTHORIED,      // 用户名/密码错误
        STATE_SUBCODE_CONFLICT,         // 同一账号(资源名也相同)从其他地方登录
        STATE_SUBCODE_DISCONECTED       // 网络断开（被动关闭连接）
    };

	enum EPresence
	{
		PRESENCE_NONE     = 0,    // 不可用
		PRESENCE_OFFLINE  = 1,    // 下线(不会用到)
		PRESENCE_XA       = 2,    // 远远离开
		PRESENCE_AWAY     = 3,    // 离开
		PRESENCE_DND      = 4,    // 不要打扰
		PRESENCE_ONLINE   = 5,    // 在线
		PRESENCE_CHAT     = 6     // 可以聊天
	};

    // 用来操作后续接口的句柄
    typedef void * HANDLE;

	const unsigned long MAX_NODE_LEN       =   64;   // JID的node最大长度
	const unsigned long MAX_DOMAIN_LEN     =   32;   // JID的domain最大长度
	const unsigned long MAX_RESOURCE_LEN   =   64;   // JID的resource最大长度
    // JID结构体(字符串表现形式 node@domain\resource )
	typedef struct tagJid
	{
		char    sNode[MAX_NODE_LEN];          // node
		char    sDomain[MAX_DOMAIN_LEN];      // domain
		char    sResource[MAX_RESOURCE_LEN];  // resource

        tagJid()
        {
            memset( this, 0, sizeof(*this) );
        }
	}TJid, *PTJid;


	const unsigned long MAX_PRESENCE_DESCRIPTION = 32;   // 出现状态描述字符串最大长度

    // 出席状态
	typedef struct tagPresence
	{
		TJid        tJid;                    // JID
		EPresence   ePresence;               // 出席状态
		char        sDescription[MAX_PRESENCE_DESCRIPTION]; // 文字描述

        BOOL        bFileShare;             // 文件传输能力
        BOOL        bAudio;                 // 音频能力
        BOOL        bVideo;                 // 视频能力
        BOOL        bCall;                  // 是否连上GK

        tagPresence()
        {
            memset( this, 0, sizeof(*this) );
        }
	}TPresence, *PTPresence;

    // roster(好友列表)的格式如下：
    // 
    //    group1
    //     |   +------ friend1
    //     |   |
    //     |   +------ friend2
    //     |
    //    group2
    //         +------ friend3
    //         |
    //         +------ friend4
    //
    // 组的深度只有一级
    //
	const unsigned long MAX_MEMBER_NAME_LEN     = 32;    // 好友名字最大长度
    const unsigned long MAX_MEMBER_BITRATE_LEN  = 32;    // 呼叫码率

#define SUB_STATE_NONE                    0x00
#define SUB_STATE_SUBSCRIBE               0x01        // 被别人订阅了状态(和下面的一起写反了)
#define SUB_STATE_SUBSCRIBED              0x02        // 订阅了别人的状态

#define ASK_STATE_NONE                    0
#define ASK_STATE_SUBSCRIBE               1           // 正在请求订阅某人的状态

	typedef struct tagMember
	{
		TJid        tJid;                         // 好友JID
		char        szName[MAX_MEMBER_NAME_LEN];  // 好友名称
		int         nSubscription;                // 订阅的状态(参考上面的值)
        int         nAsk;                         //
		void *      pData;                        // 暂时没用

        char        szBitRate[MAX_MEMBER_BITRATE_LEN];    //呼叫码率
        int         nType;

        unsigned long  dwId;                           // id

		tagMember * pNext;

        tagMember()
        {
            memset( this, 0, sizeof(*this) );
        }

        //tagMember & operator = ( const tagMember & obj )
        //{
        //    memcpy( &tJid, &obj.tJid, sizeof(tJid) );
        //    memcpy( szName, obj.szName,MAX_MEMBER_NAME_LEN );
        //    nSubscription = obj.nSubscription;
        //    nAsk          = obj.nAsk;
        //    pData         = obj.pData;
        //    memcpy( szBitRate, obj.szBitRate, MAX_MEMBER_BITRATE_LEN );
        //    nType         = obj.nType;

        //    return *this;
        //}
	}TMember, *PTMember;

	const unsigned long  MAX_GROUP_NAME_LEN = 64+4;     // 组名最大长度
	typedef struct tagGroup
	{
        unsigned long   dwGroupId;
		char       szName[MAX_GROUP_NAME_LEN];        // 组的名称
		PTMember   pChild;                            // 组下第一个好友
        unsigned long     bToBeDeleted;               // 将被删除
		tagGroup * pNext;                             // 下一个组

        tagGroup()
        {
            memset( this, 0, sizeof(*this) );
        }

        //tagGroup & operator = ( const tagGroup & obj )
        //{
        //    memcpy( szName, obj.szName, MAX_GROUP_NAME_LEN );
        //    return *this;
        //}
	}TGroup, *PTGroup;


    typedef struct tagMemberItem
    {
        char        szName[MAX_MEMBER_NAME_LEN];  // 好友名称
        int         nSubscription;                // 订阅的状态(参考上面的值)
        int         nAsk;                         //
        void *      pData;                        // 暂时没用

        char        szBitRate[MAX_MEMBER_BITRATE_LEN];    //呼叫码率
        int         nType;

        tagMemberItem()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TMemberItem, *PTMemberItem;


#define LIBJINGLE_CALLBACK





















	// libjingle事件的回调函数
    // *******************************************************
    // *******************************************************
    // 重要！
    //     1. 回调函数不要调用接口函数(会死锁)
    //     2. 回调函数不要有阻塞操作
    // 通常主线程调用API，回调函数都在辅线程里，回调里可以Post一个消息到主线程

    // 登录的回调
	typedef void (* LIBJINGLE_CALLBACK POnLoginResult)( HANDLE hApp, ELibjingleState eState, ELibjingleStateSubCode eSubCode, void * pUserData );
    // 收到好友列表
	typedef void (* LIBJINGLE_CALLBACK POnRosterInfo)( HANDLE hApp, const PTGroup pFirstGroup );
    // 某个好友改变了属性
	typedef void (* LIBJINGLE_CALLBACK POnRosterChange)( HANDLE hApp, 
                                                         const char * szGroupName, 
                                                         const PTJid ptJid, 
                                                         const PTMemberItem ptChange );
    // 新增了某个好友
    typedef void (* LIBJINGLE_CALLBACK POnRosterAdd )( HANDLE hApp, 
                                                       const char * szGroupName, 
                                                       const PTJid ptJid, 
                                                       const PTMemberItem ptChange );
    // 失去了某个好友
    typedef void (* LIBJINGLE_CALLBACK POnRosterDel )( HANDLE hApp, 
                                                       const char * szGroupName, 
                                                       const PTJid ptJid );
    // 好友出席状态改变
	typedef void (* LIBJINGLE_CALLBACK POnRosterPresenceChange)( HANDLE hApp, 
                                                                 const PTPresence pPresence );
    // 收到消息
	typedef void (* LIBJINGLE_CALLBACK POnChatMessage)( HANDLE hApp, 
                                                        const PTJid pJid, const char * szMessage );
    // 别人请求订阅你的状态
    typedef void (* LIBJINGLE_CALLBACK POnSubscribe)( HANDLE hApp, const PTJid pJid );


    /*=====================================  群聊回调  =============================================*/
    // 是否有chatroom服务
    typedef void ( *LIBJINGLE_CALLBACK POnChatRoomService )( HANDLE hApp, BOOL bAvailable );
    // 房间的句柄
    typedef void * HROOM;
    // 创建的房间是否成功，nErrorCode为0时，表示创建成功；非0，失败
    typedef void (* LIBJINGLE_CALLBACK POnCreateUniqueRoom )( HANDLE hApp, HROOM hRoom, int nErrorCode );
    // 被邀请到某个房间
    typedef void (* LIBJINGLE_CALLBACK POnInvited )( HANDLE hApp, HROOM hRoom, PTJid ptInviterJid, 
                                                     BOOL bPersistent, const char * szRoomName ) ;
    // 与会者和房间的关系
    enum EAffiliation
    {
        AFFILIATION_NONE     = 0,    // 房间非所有者
        AFFILIATION_OWNER    = 1,    // 房间所有者
        AFFILIATION_MEMBER   = 2,    // 房间成员
        AFFILIATION_ADMIN    = 3,    // 房间admin
        AFFILIATION_UNKNOWN  = -1
    };

    enum EChatRoomPreseFlag
    {
        CHATROOM_PRESENCE_FLAG_LEAVE,           // 离开房间
        CHATROOM_PRESENCE_FLAG_FIRST_ENTER,     // 第一次进入
        CHATROOM_PRESENCE_FLAG_CHANGE,          // presence 改变
        CHATROOM_PRESENCE_FLAG_KICKED           // 被踢出
    };

    typedef void (* LIBJINGLE_CALLBACK POnParticipantPresenceChange )( HROOM hRoom,
                                                  const PTJid ptJid,                               // 与会者的jid
                                                  EPresence * peShow, const char * szDescription,
                                                  EAffiliation eAffiliation ,                       // 与会者和房间的关系
                                                  BOOL  bSelf,                                     // Jid是否自己
                                                  int   nFlag,                                     // 是否首次进入，是否被踢出
                                                  HANDLE       hApp // 退出房间后，该房间内存销毁，所以传递一个hApp参数过来
                                                  );
    // 被授予新的权限(一般是房间所有者权限)
    typedef void (* LIBJINGLE_CALLBACK POnGrant )( HANDLE hApp, HROOM hRoom, EAffiliation eOldAffiliation, EAffiliation eNewAffiliation );
    // 房间销毁 
    typedef void (* LIBJINGLE_CALLBACK POnDestroyRoom )( HROOM hRoom, HANDLE hApp );  // 事后会内存销毁，所以传递一个hApp参数过来
    // 收到房间消息
    typedef void (* LIBJINGLE_CALLBACK POnRoomMessage)( HANDLE hApp, HROOM hRoom, const PTJid pJid, const char * szMessage, const char * szMessageId );    

    /*===================================== END 群聊回调  =============================================*/


    /*=====================================  文件传输回调  =============================================*/
    // 暂时设定一次Session只能发送一个文件或文件夹
    const unsigned long MAX_MANIFEST_SIZE = 1;
    const unsigned long MAX_FILENAME_SIZE = 256;

    typedef struct tagFileShareManifest
    {
        enum Type { T_FILE, T_IMAGE, T_FOLDER };
        enum      { SIZE_UNKNOWN = -1 };
        typedef struct tagItem
        {
            tagItem()
            {
                memset( this, 0, sizeof(*this) );
            }
            Type          m_eTyp;
            char          m_szName[MAX_FILENAME_SIZE];
            unsigned long m_dwSize;
            unsigned long m_dwWidth;
            unsigned long m_dwHeight;
        }Item, *PItem;
        BOOL IsEmpty()const { return (0 == m_dwSize); }
        unsigned long GetSize() const{ return m_dwSize; }
        const Item & GetItem(unsigned long index) const{ return m_atItems[index]; }
        LIBJINGLE_API BOOL AddFile(   const char * szName,   unsigned long dwSize);
        LIBJINGLE_API BOOL AddImage(  const char * szName,   unsigned long dwSize,
            unsigned long dwWidth, unsigned long dwHeight);
        LIBJINGLE_API BOOL AddFolder( const char * szName,   unsigned long dwSize);
        LIBJINGLE_API unsigned long GetItemCount (Type t) const;
        unsigned long GetFileCount()const{ return GetItemCount(T_FILE); }
        unsigned long GetImageCount()const{ return GetItemCount(T_IMAGE); }
        unsigned long GetFolderCount()const{ return GetItemCount(T_FOLDER); }
        LIBJINGLE_API unsigned long GetTotalSize()const;

        tagFileShareManifest()
        {
            m_dwSize = 0;
        }
    private:
        Item           m_atItems[MAX_MANIFEST_SIZE];
        unsigned long  m_dwSize;
    }FileShareManifest, *PFileShareManifest;

    // 操作文件传输的句柄
    typedef void * HSession;

    typedef void (* LIBJINGLE_CALLBACK POnFileOffer)( HANDLE hApp, HSession hSession, const PTJid pJid, const FileShareManifest * pManifest, BOOL bSender );
    typedef void (* LIBJINGLE_CALLBACK POnFileTransfer)( HANDLE hApp, HSession hSession );
    typedef void (* LIBJINGLE_CALLBACK POnFileComplete)( HANDLE hApp, HSession hSession, const char * szCompletePath );
    typedef void (* LIBJINGLE_CALLBACK POnFileCancel)( HANDLE hApp, HSession hSession, BOOL bLocal );
    typedef void (* LIBJINGLE_CALLBACK POnFileFailure)( HANDLE hApp, HSession hSession, int nErrorCode );
    typedef void (* LIBJINGLE_CALLBACK POnFileProgress)( HANDLE hApp, HSession hSession, int nPercent, 
        const char * szCurFileName, unsigned long dwTransferSize, unsigned long dwTransferRate );

    /*===================================== END 文件传输回调  =============================================*/

    enum   EImageType
    {
        IMAGE_TYPE_BITMAP,
        IMAGE_TYPE_JPG,
        IMAGE_TYPE_GIF,
        IMAGE_TYPE_PNG,
        IMAGE_TYPE_UNKNOWN
    };

    const unsigned long   VCARD_MAX_NICK_LEN               =   32;
    const unsigned long   VCARD_MAX_HEAD_PORTRAIT_SIZE     =   4096;
    typedef struct tagVCard
    {
        char      m_achNickName[VCARD_MAX_NICK_LEN];         // 用户昵称
        BOOL      m_bCustomHeadPortrait;                     // 是否使用自定义图像
        int       m_nSysHeadPortraitIndex;                   // 使用系统图像的索引
        EImageType       m_eCustomHeadPortraitType;          // 自定义头像类型
        unsigned char    m_abyCumtomHeadPortrait[VCARD_MAX_HEAD_PORTRAIT_SIZE];    // 自定义头像
        unsigned long    m_dwCustomHeadPortraitSize;
        tagVCard()
        {
            memset( this, 0, sizeof(*this) );
        }
        void SetCustomHeadPortrait( EImageType eType, const unsigned char * pbyData, unsigned long dwSize )
        {
            if ( eType >= IMAGE_TYPE_UNKNOWN || dwSize > VCARD_MAX_HEAD_PORTRAIT_SIZE )
            {
                return;
            }
            m_eCustomHeadPortraitType = eType;
            m_dwCustomHeadPortraitSize = dwSize;
            memcpy( m_abyCumtomHeadPortrait, pbyData, dwSize );
        }
    }TVCard, *PTVCard;

    typedef void (* LIBJINGLE_CALLBACK POnVCardInfo)( HANDLE hApp, const TJid * ptJid, 
                                                      const TVCard * ptVCard, BOOL bSelf );


	// 回调函数放在一个类里
	typedef struct tagLibJingleEvent
	{
		POnLoginResult          pOnLoginResult;                      // 登录回调
		POnRosterInfo           pOnRosterInfo;                       // 好友名单回调，
                                                                     // 如果要长久使用回调的参数，请进行复制
		POnRosterChange         pOnRosterChange;
        POnRosterAdd            pOnRosterAdd;
        POnRosterDel            pOnRosterDel;
		POnRosterPresenceChange pOnRosterPresenceChange;             // 好友状态改变回调
		POnChatMessage          pOnMessage;                          // 接收到聊天消息
        POnSubscribe            pOnSubscribe;                        // 收到状态订阅

        POnChatRoomService      pOnChatRoomService;                  // 是否有聊天室功能
        POnCreateUniqueRoom     pOnCreateUniqueRoom;                 // 创建房间的回调
        POnInvited              pOnInvited;                          // 被邀请进入了房间
        POnParticipantPresenceChange pOnParticipantPresenceChange;   // 某聊天室的与会者信息
        POnGrant                pOnGrant;                            // 被授予聊天室所有者
        POnDestroyRoom          pOnDestroyRoom;
        POnRoomMessage          pOnRoomMessage;                      // 收到聊天室群发信息

        POnFileOffer            pOnFileOffer;                         // 收到文件传输offer
        POnFileTransfer         pOnFileTransfer;                      // 传输开始
        POnFileComplete         pOnFileComplete;                      // 传输完毕
        POnFileCancel           pOnFileCancel;                        // 传输中止
        POnFileFailure          pOnFileFailure;                       // 传输失败
        POnFileProgress         pOnFileProgress;                      // 传输进度

        POnVCardInfo            pOnVCardInfo;

        tagLibJingleEvent()
        {
            memset( this, 0, sizeof(*this) );
        }
	}TLibJingleEvent, *PTLibJingleEvent;



    const unsigned long   MAX_GROUPCHAT_ROOM_NAEM_LEN        =   64 + 4;
    const unsigned long   MAX_GROUPCHAT_ROOM_RESERVERD_LEN   =   32;

    typedef struct tagRoomConfigure
    {
        char    szRoomName[MAX_GROUPCHAT_ROOM_NAEM_LEN];
        char    szReserverd[MAX_GROUPCHAT_ROOM_RESERVERD_LEN];
        tagRoomConfigure()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TRoomConfigure, *PTRoomConfigure;


    
    const unsigned long  MAX_KEDA_WEIBO_TYPE_LEN       = 64;
    const unsigned long  MAX_KEDA_WEIBO_DATA_LEN       = 64;
    const unsigned long  MAX_KEDA_WEIBO_DATA_ITEM_CNT  = 16;
    const unsigned long  MAX_KEDA_WEIBO_KEY_LEN        = 64;

    typedef struct tagKedaWeiboMsg
    {
        char             szType[MAX_KEDA_WEIBO_TYPE_LEN];
        char             szData[MAX_KEDA_WEIBO_DATA_ITEM_CNT][MAX_KEDA_WEIBO_DATA_LEN]; // 数据
        unsigned long    dwCnt;                                                         // 多少数据

        char             szKey [MAX_KEDA_WEIBO_DATA_ITEM_CNT][MAX_KEDA_WEIBO_KEY_LEN];  // key
        tagKedaWeiboMsg()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TKedaWeiboMsg, *PTKedaWeiboMsg;

    
    typedef struct tagRoomRosterItem
    {
        LibJingleEx::TJid                m_tJid;                     // 房间成员的jid
        LibJingleEx::EAffiliation        m_eAlliliation;             // 房间成员的权限
        BOOL                             m_bSelf;                    // 是否同一账号

        tagRoomRosterItem()
        {
            m_eAlliliation = LibJingleEx::AFFILIATION_NONE;
            m_bSelf = FALSE;
        }
    }TRoomRosterItem, *PTRoomRosterItem;

    const unsigned long MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT            =  20;

    enum EFileShareError
    {
        FILE_SHARE_ERR_NO_SPACE          = 1,                            // 磁盘空间满
        FILE_SHARE_ERR_DISCONNECTED,                                     // 传输网络断开
        FILE_SHARE_ERR_NOT_FOUND,                                        // 文件没有找到
    };

    const unsigned long MAX_SID_LEN = 32;


    enum EMsgId
    {                                     /***********              数据格式              ****************/
        ON_STATE                    = 1,  // ELibjingleState
        ON_ROSTER_INFO,                   // PTGroup
        ON_ROSTER_CHANGE,                 // char [MAX_GROUP_NAME_LEN] + TJid + TMemberItem
        ON_ROSTER_ADD,                    // char [MAX_GROUP_NAME_LEN] + TJid + TMemberItem
        ON_ROSTER_DEL,                    // char [MAX_GROUP_NAME_LEN] + TJid
        ON_FRIEND_PRESENCE,               // TPresence + BOOL(是否同一账号) + EPresence(最大状态) + BOOL(是否第一次) + BOOL(音频能力) + BOOL(视频能力) + BOOL(是否来自status chatroom) + BOOL(和上一次的状态比较是否有变换) + bDelay + bLoginBeforeMe
        ON_CHAT_MESSAGE,                  // TJid + BOOL(是否离线) + 不定长字符串(包括结束符号0)
        ON_QUERY_SUBSCRIBED,              // TJid

        ON_GROUP_CHAT_ABILITY,            // BOOL
        ON_CREATE_UNIQUE_CHATROOM,        // HROOM + int + char array( room id, length = 64 ) + char array( service name, 64 )
        ON_INVITED_TO_CHATROOM,           // HROOM + TJid(inviter) + BOOL( is persistent? ) + int(type) + char array( LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN， room name ) + roomid + service name
        ON_PARTICIPANT_PRESENCE,          // HROOM + TJid + EPresence * + const char * + EAffiliation + BOOL + EChatRoomPreseFlag + MaxPresence + char array( room id, length = 64 ) + char array( service name, 64 )
        ON_GRANT,                         // HROOM + EAffiliation
        ON_DESTROY_CHATROOM,              // HROOM + char array( room id, length = 64 ) + char array( service name, 64 )
        ON_GROUPCHAT_MESSAGE,             // HROOM + TJid + BOOL(是否离线消息) + char array(消息id) + TDmsChatContent + char array( room id, length = 64 ) + char array( service name, 64 )

        ON_FILE_OFFER,
        ON_FILE_TRANSFER,
        ON_FILE_COMPLETE,
        ON_FILE_CANCEL,
        ON_FILE_FAILURE,
        ON_FILE_PROGRESS,

        ON_VCARD_INFO,                     // TJid + TVCard + BOOL

        ON_NOTIFY_ADD_DMS_GROUP,           // TDmsGroup
        ON_NOTIFY_DEL_DMS_GROUP,           // unsigned long(group id )
        ON_NOTIFY_MODIFY_DMS_GROUP,        // TDmsGroup
        ON_NOTIFY_MOVE_DMS_GROUP,

        ON_NOTIFY_ADD_DMS_MEMBER,          // TDmsMember
        ON_NOTIFY_DEL_DMS_MEMBER,
        ON_NOTIFY_MODIFY_DMS_MEMBER,
        ON_NOTIFY_MOVE_DMS_MEMBER,

        ON_PICTURE_MSG,                     // id(32 char) + TJid + flag( &1 :begin, &2 :end )
                                           // + total size(unsigned long, valid when begin)
                                           // + buf size( unsigned long ) + buf

        ON_PUBSUB_CREATE_NODE_RESULT,     // unsigned long(Context Id) + BOOL(TRUE: success, FALSE: fail)
        ON_PUBSUB_DELETE_NODE_RESULT,     // unsigned long(Context Id) + BOOL(TRUE: success, FALSE: fail)
        ON_PUBSUB_SUBSCRIBE_NODE_RESULT,  // unsigned long(Context Id) + BOOL(TRUE: success, FALSE: fail)

        ON_KEDA_WEIBO_MSG_NOTIFY,         // TKedaWeiboMsg

        ON_ENTER_PERSISTENT_ROOM,         // HROOM + EAffiliation + char array( room name, length LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN ) + BOOL(bPersistent)
                                          // + char array( room id, length = 64 )
        ON_ENTER_PERSISTENT_ROOM_FOR_TT_ANDROID,      
                                          // HROOM + char array( room name, length LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN )
                                          // + char array( room id, length = 64 )

        ON_ROOM_CONFIG_MSG,                // 获取房间配置(通常是房间名称)
                                          // char array(LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN)

        ON_ACCEPT_INVITE_RESULT,          // 接受房间邀请的结果( HROOM + BOOL(TRUE, 接受邀请成功，FALSE，房间已经不存在，接受失败) )

        
        ON_MEMBERS_DATA_READY,            // 联系人数据可用
        ON_NOTIFY_SECURITY,               // 通知同一账号在其他地方登录

        ON_DECLINE_INVITING,             // 因聊天室数目限制，邀请被拒绝通知
                                         // char array( room id, length = 64 ) + char array( service name, 64 ) 
                                         // + char array( LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN， room name ) + BOOL(是否固定群) + TJid(拒绝者)

        ON_OFFLINE_MSG_OVERFLOW,         // TJid( 发送给谁 )

        ON_ROOM_ROSTER_ADDED,            // 聊天室的成员列表增加了N个成员  HROOM + room id + unsigend long ( N ) + TRoomRosterItem array ( N 个 )
        ON_ROOM_ROSTER_DELETED,          // 聊天室的成员列表减少了N个成员  HROOM + room id + unsigend long ( N ) + TRoomRosterItem array ( N 个 )


        ON_CHATROOM_PIC_OPEN,            // 聊天室图片传输开始  HROOM + TJid + SessionId( MAX_SID_LEN ) + DWORD( size ) + char array( MAX_FILENAME_SIZE, picture name ) + room_id + room_service
        ON_CHATROOM_PIC_DATA,            // 聊天室图片传输数据  HROOM + TJid + SessionId                    + DWORD( progress ) + room_id + room_service
        ON_CHATROOM_PIC_CLOSE,           // 聊天室图片传输结束  HROOM + TJid + SessionId + pic final path( MAX_PATH_SIZE size ) + room_id + room_service
        ON_CHATROOM_PIC_FAIL,            // 聊天室图片传输数据失败 HROOM + TJid + SessionId + errorCode + room_id + room_service


        ON_CHATROOM_ADD_MEMBER_NOTIFY,   // 聊天室增加成员通知   HROOM + TJid  + room_id + room_service       
                                         // ON_ROOM_ROSTER_ADDED不能区分是再次上线获取roster的还是当前管理员邀请获取的


        ON_PERSISTENT_CHATROOMS_NOTIFY,      // 通知上一次登录中保存的固定聊天室信息  N + ( TJid( 聊天室jid )  + char array ( LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN ) ) * N
        ON_PERSISTENT_CHATROOMS_NOTIFY_FIN,  // 通知上一次登录中保存的固定聊天室信息结束


        ON_ENTER_PERSISTENT_ROOM_FAIL,   // char array( room name, length LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN ) + room id + service name

        ON_CHAT_STATE,                  // TJid + ECharState

        ON_CUSTOM_MESSAGE,              // TJid + LibJingleEx::TCustomMsg

        ON_CUSTOM_CONFIG_DATA_READY,    // 
        ON_CUSTOM_CONFIG_CHANGE,        // TCumstomConfigItem + Old value ( char array[szValue[MAX_CUSTOM_CONFIG_VALUE_LEN]] )


        ON_P2P_CHAT_MESSAGE_ACK,        // unsigned long(context id) + BOOL
        ON_GROUP_CHAT_MESSAGE_ACK,      // HROOM + unsigned long(context id) + BOOL + room id + chat room service

        ON_NOTHING
    };

    // 回调函数
    typedef void (* LIBJINGLE_CALLBACK POnMsg )( EMsgId eMsgId, HANDLE hApp,     void * pUserData, 
                                                 const unsigned char * pbyMsg,   unsigned long dwMsgLen );


    const unsigned long MAX_MESSAGE_DATA_LENGTH  = 10000;

    class CMsgData
    {
    public:
        LIBJINGLE_API CMsgData( const unsigned char * pbyData = 0, unsigned long dwDataLen = 0 );

        LIBJINGLE_API BOOL Append( const unsigned char * pbyData, unsigned long dwDataLen ); // 写入数据到末尾
        LIBJINGLE_API void Clear();

        LIBJINGLE_API BOOL Read( unsigned char * pbyData, unsigned long dwDataSize = -1 );        // 从当前读位置开始读取数据
        LIBJINGLE_API void ResetReadPos();                                                   // 把读重置为0

        LIBJINGLE_API unsigned long         GetMsgLength() const;
        LIBJINGLE_API const unsigned char * GetMsgData() const;

        LIBJINGLE_API unsigned long   GetReadPos() const;

    private:
        unsigned char    m_abyData[MAX_MESSAGE_DATA_LENGTH];            // 数据
        unsigned long    m_dwDataLen;                                   // 数据长度

        unsigned long    m_dwRPos;                                      // 当前读的位置
    };


    enum EDeclineReason
    {
        DECLINE_REASON_UNKNOWN,                                       // 未知原因
        DECLINE_REASON_MAX_PERSISTENT_ROOM_COUNT,                     // 达到固定群最大数
        DECLINE_REASON_MAX_TEMPORARY_ROOM_COUNT,                      // 达到临时群最大数
        DECLINE_REASON_FROM_WEIBO                                     // 仅微博在线时的拒绝
    };














    const unsigned long MAX_HOST_NAME_LEN       =            32;
	const unsigned long MAX_PASSWORD_LEN        =            64;
    const unsigned long  MAX_PATH_SIZE          =            256;
    const unsigned long  MAX_ADDR_LEN           =            32;
    const unsigned long  MAX_ENTERPRISE_NAME_LEN    =        64;


    // 登录时使用的结构体
	typedef struct tagLoginSettings
	{
		char    sUserName[MAX_NODE_LEN];                     // 用户名
		char    sDomain[MAX_DOMAIN_LEN];                     // 域名
		char    sPassword[MAX_PASSWORD_LEN];                 // 密码
		char    sHost[MAX_HOST_NAME_LEN];                    // 服务器主机名
        char    sResource[MAX_RESOURCE_LEN];                 // 资源( truelink, truetouch等字符串 )
		int     bUseTls;                                     // 是否使用TLS
        int     bUseCompression;                             // 是否使用zlib压缩
        unsigned short wPort;                                // 服务器端口(如果为0，认为取默认值5222)
        char    reserved0[2];
        char    sEnterpriseName[MAX_ENTERPRISE_NAME_LEN];

        // 文件传输相关
        BOOL               bFileShareEnable;                        // 是否启用文件传输
        char               szStunAddr[MAX_ADDR_LEN];                // stun server ip address
        unsigned short     wStunPort;                               // stun port
        char               reserverd1[2];
        char               szRelayAddr[MAX_ADDR_LEN];               // relay ip address
        char               szDefaultSaveDir[MAX_PATH_SIZE];         // default save directory
        // end 文件传输相关


        BOOL               m_bAudio;                                // 是否有音频呼叫能力
        BOOL               m_bVideo;                                // 是否有视频呼叫能力
        BOOL               m_bCall;
        EPresence          m_ePres;


        char              szGroupChatPicSaveDir[MAX_PATH_SIZE];      // 聊天室图片保存路径

        BOOL              bDelayLogin;                               // 是否延迟登录

        BOOL              bSs5Proxy;                                 // 是否用socks5代理
        char              szSs5Host[MAX_HOST_NAME_LEN];              // ss5主机名或ip地址
        unsigned short    wSs5Port;                                  // ss5服务器端口(如果为0，认为用缺省的1080)
        char              reserved2[2];
        char              szSs5ProxyUsername[MAX_NODE_LEN];          // ss5用户名
        char              szSs5ProxyPassword[MAX_PASSWORD_LEN];      // ss5密码

        char              szConfigPath[MAX_PATH_SIZE];               // 保存xmpp配置文件的路径

        tagLoginSettings()
        {
            memset( this, 0, sizeof(*this) );
            m_ePres = PRESENCE_ONLINE;
        }
	}TLoginSettings, *PTLoginSettings;


    // 以下API不要在回调函数里调用

    /********************************************************/
    // 函数名称：Init
    // 功能：    初始化封装库(注册事件回调函数)
    // 参数：    tEvent的成员不能为空
    // 返回值：  0，成功
    /********************************************************/
    LIBJINGLE_API ELibjingleError Init( const PTLibJingleEvent ptEvent );
    LIBJINGLE_API ELibjingleError Init( POnMsg  pOnMsg );

    /********************************************************/
    // 函数名称：Deinit
    // 功能：    去初始化
    // 返回值：  0，成功
    /********************************************************/
    LIBJINGLE_API ELibjingleError Deinit(  );

    /********************************************************/
    // 函数名称：Login
    // 功能：    登录到XMPP服务器
    // 说明：    返回值为0时，发送登录的消息给XMPP服务器成功。
    //           回调函数pOnLoginResult会给出登录的状态。OPEN 时为成功登录
    // 参数：    ptLoginSettings不能为空
    /********************************************************/
	LIBJINGLE_API ELibjingleError Login( HANDLE *  phApp, PTLoginSettings ptLoginSettings, void * pUserData = 0 );

    LIBJINGLE_API ELibjingleError SetReady( HANDLE hApp );


    /********************************************************/
    // 函数名称：Logout
    // 功能：    登出XMPP服务器
    // 说明：    调用后底层TCP链会断开。不考虑pOnLogoutResult回调
    /********************************************************/
	LIBJINGLE_API ELibjingleError Logout( HANDLE hApp );


    const unsigned long  MAX_DMS_FONT_BUF_SIZE   =  64;
    const unsigned long  MAX_DMS_TIME_BUF_SIZE   =  8;
    const unsigned long  MAX_DMS_HEADER_SIZE     =  64   + 4;
    const unsigned long  MAX_DMS_CONTENT_SIZE    =  2048 + 4;
    const unsigned long  MAX_DMS_SEND_PIC_COUNT  =  4;

    typedef struct  tagTDmsChatPicInfo 
    {
        unsigned long   m_dwIndex;                      // 文件索引，对应于插入richedit中的dwUser
        char            m_achFileName[MAX_PATH_SIZE];   // 图片文件名称，格式：GUID_发送端帐号
    public:
        tagTDmsChatPicInfo()
        {
            memset(this, 0, sizeof(*this) );
        }
    }TDmsChatPicInfo, *PTDmsChatPicInfo;


    const unsigned long  MAX_DMS_FONT_FACE_SIZE  =  32;

    typedef struct tagTDmsFont
    {
        long           lfHeight; 
        long           lfWidth; 
        long           lfEscapement; 
        long           lfOrientation; 
        long           lfWeight; 
        unsigned char  lfItalic; 
        unsigned char  lfUnderline; 
        unsigned char  lfStrikeOut; 
        unsigned char  lfCharSet; 
        unsigned char  lfOutPrecision; 
        unsigned char  lfClipPrecision; 
        unsigned char  lfQuality; 
        unsigned char  lfPitchAndFamily; 
        char           lfFaceName[MAX_DMS_FONT_FACE_SIZE]; 

        tagTDmsFont()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TDmsFont, *PTDmsFont;

    typedef struct tagTDmsChatContent
    {
        // unsigned char    m_abyFontBuf[MAX_DMS_FONT_BUF_SIZE];         // 字体
        TDmsFont         m_tFont;
        unsigned long    m_dwColor;                                   // 聊天内容的颜色
        // unsigned char	 m_abyTimeBuf[MAX_DMS_TIME_BUF_SIZE];         // 聊天的时间
        unsigned long    m_dwTime;
        char             m_szMsgHeader[MAX_DMS_HEADER_SIZE];          
        char             m_szContent[MAX_DMS_CONTENT_SIZE];           
        unsigned long    m_dwChatType;
        unsigned long    m_dwPicCount;
        TDmsChatPicInfo  m_atPicFileInfoTable[MAX_DMS_SEND_PIC_COUNT]; // 发送图片列表，一次最多只能发送MAX_SEND_PIC_COUNT个图片
    public:
        tagTDmsChatContent()
        {
            memset(this, 0, sizeof(*this) );
        }
    }TDmsChatContent, *PTDmsChatContent;


    enum  ECharStatus
    {
        CHAR_STATUS_ACTIVE     = 0,
        CHAR_STATUS_COMPOSING,
        CHAR_STATUS_PAUSED
    };


    /********************************************************/
    // 函数名称：SendMsg
    // 功能：   发送消息给XMPP服务器上的一个用户
    // 参数：   pJid， 对方JID， 不能为空
    //          szMsg  发送的字符串，不能为空
    // 返回值： 0，发送成功
    /********************************************************/
    const unsigned long MAX_MSG_LENGTH  = 4096;
	LIBJINGLE_API ELibjingleError SendMsg( HANDLE hApp, const PTJid pJid, const char * szMsg, BOOL bUtf8Encode = TRUE );


    LIBJINGLE_API ELibjingleError SendMsg( HANDLE hApp, const PTJid pJid, const TDmsChatContent * ptMsg, unsigned long * pdwSendTime = 0, unsigned long dwContextId = 0 );


    LIBJINGLE_API ELibjingleError SendPicture( HANDLE hApp, const PTJid pJid, 
                                               const unsigned char * pbyPicture, unsigned long dwSize );


    LIBJINGLE_API ELibjingleError SendCharState( HANDLE hApp, const PTJid pJid, ECharStatus eCharState );


    /********************************************************/
    // 函数名称：AddFriend
    // 功能：    添加好友
    // 参数：    pJid，要添加的好友的JID，不能为空
    //           szGroup， 要添加到的组，可以为空(默认为friend)
    //           szNickName，别名，可以为空(取jid的node值)
    // 返回值：  0，发送添加好友消息成功
    // 说明：    组名最大长度为MAX_GROUP_NAME_LEN，
    //           别名最大长度为MAX_MEMBER_NAME_LEN
    //           触发回调OnRosterAdd
    //                 或OnRosterChange
    /********************************************************/
    LIBJINGLE_API ELibjingleError AddFriend( HANDLE hApp, const PTJid pJid, const char * szGroup = 0, const char * szNickName = 0,
        const char * szBirtate = 0, unsigned long dwType = 0 );


    LIBJINGLE_API ELibjingleError MoveFriend( HANDLE hApp, const PTJid pJid, 
                                              const char * szGroupSrc, const char * szGroupDst );

    /********************************************************/
    // 函数名称：AcceptFriend
    // 功能：    是否接受别人添加你为好友
    // 参数：    pJid，要添加你为好友的别人的JID，不能为空
    //           bAccept， 是否接受
    // 返回值：  0，发送是否意愿消息成功
    // 说明：    回调函数pOnSubscribe会传递一个pJid，用这个pJid作为参数
    /********************************************************/
    LIBJINGLE_API ELibjingleError AcceptFriend( HANDLE hApp, const PTJid pJid, BOOL bAccept = TRUE );


    /********************************************************/
    // 函数名称：DelFriend
    // 功能：    从某个组里删除好友，或删除所有组的该好友(szGroup == 0)
    // 参数：    pJid，好友的bare JID
    //           szGroup， 组名
    // 返回值：  0，发送删除消息成功
    // 说明：    触发回调OnRosterDel
    /********************************************************/
    LIBJINGLE_API ELibjingleError DelFriend( HANDLE hApp, const PTJid pJid, const char * szGroup = 0,
                                             BOOL  bUnsubscribed = TRUE );

    /********************************************************/
    // 函数名称：SetFriendNick
    // 功能：    修改好友的别名
    // 参数：    pJid，好友的bare JID
    //           szNick， 新的别名
    // 返回值：  0，发送消息成功
    // 说明：    触发回调OnRosterChange
    /********************************************************/
	LIBJINGLE_API ELibjingleError SetFriendNick( HANDLE hApp, const PTJid pJid, const char * szNick, const char * szBitrate = 0 );


    enum ESetStatusSwitchIndex
    {
        SET_STATUS_SWITCH_INDEX_PRESENCE = 0,
        SET_STATUS_SWITCH_INDEX_CALL
    };

    /********************************************************/
    // 函数名称：SetStatus
    // 功能：    发布自己的状态。好友会收到发布的状态
    // 参数：    e，自己的新状态
    // 返回值：  0，发送消息成功
    /********************************************************/
	LIBJINGLE_API ELibjingleError SetStatus( HANDLE hApp, EPresence e, const char * szDescription = 0, BOOL bCall = TRUE, unsigned long dwSwitch = -1 );

    /********************************************************/
    // 函数名称：Subscribe
    // 功能：    请求对好友的关注
    /********************************************************/
    LIBJINGLE_API ELibjingleError Subscribe( HANDLE hApp, const PTJid pJid );

    /********************************************************/
    // 函数名称：Unsubscribed
    // 功能：    取消自己被好友关注
    /********************************************************/
    LIBJINGLE_API ELibjingleError Unsubscribed( HANDLE hApp, const PTJid pJid );

    /********************************************************/
    // 函数名称：Unsubscribe
    // 功能：    取消对好友的关注
    /********************************************************/
    LIBJINGLE_API ELibjingleError Unsubscribe( HANDLE hApp, const PTJid pJid );


    /********************************************************/
    // 函数名称：GetPresence
    // 功能：    用来打印presence的文字描述
    /********************************************************/
    LIBJINGLE_API const char * GetPresence( EPresence e );

    /********************************************************/
    // 函数名称：GetSubscription
    // 功能：    用来打印subscription的文字描述
    /********************************************************/
    LIBJINGLE_API const char * GetSubscription( int nSubscription );

    /********************************************************/
    // 函数名称：GetAsk
    // 功能：    用来打印ask的文字描述
    /********************************************************/
    LIBJINGLE_API const char * GetAsk( int nAsk );



    const unsigned long MAX_CHATROOM_ID_LEN                = 64;
    const unsigned long MAX_SERVICE_NAME_LEN               = 64;


    /*=====================================  群聊  =============================================*/

    /********************************************************/
    // 函数名称：CreateUniqueRoom
    // 功能：    在conference服务上创建一个有唯一号的房间
    // 返回值：  0，发送消息成功
    // 说明：    对应的回调 pOnCreateUniqueRoom
    /********************************************************/
    LIBJINGLE_API ELibjingleError CreateUniqueRoom( HANDLE hApp, HROOM * phRoom, const char * szName = 0, BOOL bPersistent = FALSE );

    LIBJINGLE_API ELibjingleError CreateOrEnterRoom( HANDLE hApp, HROOM * phRoom, const char * szId, const char * szName = 0 );

    LIBJINGLE_API ELibjingleError EnterPersistentRoom( HANDLE hApp, PTJid  ptRoomJid );

    /********************************************************/
    // 函数名称：InviteParticipant
    // 功能：    邀请与会者(要求自己为房间所有者)
    // 返回值：  0，发送消息成功
    /********************************************************/
    LIBJINGLE_API ELibjingleError InviteParticipant( HROOM hRoom, const PTJid ptJid );

    /********************************************************/
    // 函数名称：AcceptInvite
    // 功能：    是否接受参加聊天室的邀请
    // 返回值：  0，发送消息成功
    /********************************************************/
    LIBJINGLE_API ELibjingleError AcceptInvite( HROOM hRoom, BOOL bAccepted );


    LIBJINGLE_API ELibjingleError GetRoomConfigure( HROOM hRoom );


    /********************************************************/
    // 函数名称：GrantOwner
    // 功能：    授予别人为所有者(首先要求自己为房间的所有者)
    // 返回值：  0，发送消息成功
    // 说明：    别人获得所有者后，自己会失去所有者权限
    //           (1个房间只能有1个所有者)
    /********************************************************/
    LIBJINGLE_API ELibjingleError GrantOwner( HROOM hRoom, const PTJid ptJid );

    /********************************************************/
    // 函数名称：QuitRoom
    // 功能：    离开房间
    // 返回值：  0，发送消息成功
    // 说明：    
    /********************************************************/
    LIBJINGLE_API ELibjingleError QuitRoom( HROOM hRoom );

    /********************************************************/
    // 函数名称：DestroyRoom
    // 功能：    销毁房间(首先要求自己是房间的所有者)
    // 返回值：  0，发送消息成功
    // 说明：    
    /********************************************************/
    LIBJINGLE_API ELibjingleError DestroyRoom( HROOM hRoom );

    /********************************************************/
    // 函数名称：SendRoomMessage
    // 功能：    在房间发送广播
    // 返回值：  0，发送消息成功
    // 说明：    
    /********************************************************/
    LIBJINGLE_API ELibjingleError SendRoomMessage( HROOM hRoom, const char * szMessage, BOOL bUtf8Encode = TRUE );

    LIBJINGLE_API ELibjingleError SendRoomMessage( HROOM hRoom, const TDmsChatContent * ptMsg, unsigned long * pdwSendTime = 0, unsigned long dwContextId = 0 );

    
    /********************************************************/
    // 函数名称：KickRoomMember
    // 功能：    踢出聊天室成员(相同裸jid的都被踢出)
    // 返回值：  0，发送消息成功
    // 说明：    
    /********************************************************/
    LIBJINGLE_API ELibjingleError KickRoomMember( HROOM hRoom, const TJid * ptJid = 0 );


    // 设置群聊消息已读
    // szLastMessageId 为设置的已读的最后一条消息Id
    LIBJINGLE_API ELibjingleError SetGroupchatMsgRead( HROOM hRoom, const char * szLastReadMessageId = 0 );

    // 改聊天室名称
    LIBJINGLE_API ELibjingleError SetRoomName( HROOM hRoom, const char * szName );


    const unsigned long  MAX_PICTURE_DATA_SIZE = 0x400000;          // 4M字节
    // 在聊天室里发送图片
    LIBJINGLE_API ELibjingleError SendRoomPicture( HROOM hRoom, const char * szPicFilePathName, unsigned long dwPicSize );



    // 通知邀请者自己拒绝邀请
    // nReason =    LIBJINGLE_ERR_REACH_MAX_TEMP_CHATROOM_LIMIT
    //           或 LIBJINGLE_ERR_REACH_MAX_PERSISTENT_CHATROOM_LIMIT
    LIBJINGLE_API ELibjingleError NotifyInviterDeclineMsg( HANDLE hApp,  const char * szRoomId, const char * szService, 
                                                           PTJid ptInviter, int nReason, const char * szRoomName = 0 );



    
    typedef struct tagChatRoomId
    {
        char     szRoomId[MAX_CHATROOM_ID_LEN];
        char     szService[MAX_SERVICE_NAME_LEN];

        tagChatRoomId()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TChatRoomId, *PTChatRoomId;


    // 屏蔽聊天室消息
    LIBJINGLE_API ELibjingleError ScreenChatroomsMsg( HANDLE hApp, const TChatRoomId * atIds, unsigned long dwCnt );

    // 取消屏蔽聊天室消息
    LIBJINGLE_API ELibjingleError UnScreenChatroomsMsg( HANDLE hApp, const TChatRoomId * atIds, unsigned long dwCnt );

    LIBJINGLE_API ELibjingleError IsScreenedChatroom( HANDLE hApp, const TChatRoomId * pIds, int * pbIsScreened );

    // 获取聊天室的所有成员(非在线的)
    LIBJINGLE_API ELibjingleError GetChatRoomMembers( HROOM hRoom );


    /********************************************************/
    // 函数名称：GetRoomUniqueId
    // 功能：    获取conference服务提供的唯一房间的id
    // 返回值：  0，无效的房间或不可用
    //           非0，房间的id（字符串以NULL结束）
    /********************************************************/
    // const char * GetRoomUniqueId( HROOM hRoom );

    /********************************************************/
    // 函数名称：HANDLE
    // 功能：    获取room所在的应用句柄
    /********************************************************/
    LIBJINGLE_API HANDLE GetHApp( HROOM hRoom );

    LIBJINGLE_API ELibjingleError GetRoomId( HROOM hRoom, PTChatRoomId ptRoomId );

    LIBJINGLE_API HROOM GetRoomHandle( HANDLE hApp, const TChatRoomId * ptRoomId );

    /********************************************************/
    // 函数名称：GetAffiliation
    // 功能：    获取affiliation的字符串描述
    /********************************************************/
    LIBJINGLE_API const char * GetAffiliation( EAffiliation e );
    /*=====================================  END 群聊  ===========================================*/


    /********************************************************/
    // 函数名称：GetSelfJid
    // 功能：    获取自己的full jid
    /********************************************************/
    LIBJINGLE_API ELibjingleError GetSelfJid( HANDLE hApp, PTJid ptJid );


    /********************************************************/
    // 函数名称：GetUserData
    // 功能：    获取初始化时的参数pData
    /********************************************************/
    LIBJINGLE_API void * GetUserData( HANDLE hApp );


    /********************************************************/
    // 函数名称：GetRoster
    // 功能：    获取好友列表
    /********************************************************/
    LIBJINGLE_API PTGroup GetRoster( HANDLE hApp );


    /********************************************************/
    // 函数名称：GetFriendPresence
    // 功能：    获取好友状态
    /********************************************************/
    LIBJINGLE_API EPresence GetFriendPresence( HANDLE hApp, const PTJid ptJid );

    // 说明：aPres数组元素个数必须和aszRes相同，即dwResCnt
    LIBJINGLE_API ELibjingleError  GetFriendPresence( HANDLE hApp, char aszRes[][MAX_RESOURCE_LEN], 
                                                        unsigned long dwResCnt, const PTJid ptBareJid, 
                                                        EPresence aPres[], EPresence * pHighestPres );

    /********************************************************/
    // 函数名称：IfHasFriend
    // 功能：    roster中是否有好友
    /********************************************************/
    LIBJINGLE_API BOOL IfHasFriend( HANDLE hApp, const PTJid ptJid );

    
    /********************************************************/
    // 函数名称：GetFriendAllInfo
    // 功能：    获取某好友的所有不同resouce，和对应的数据
    /********************************************************/
    typedef struct tagFriendInfo
    {
        char                    sResource[MAX_RESOURCE_LEN];  // resource
        LibJingleEx::EPresence  ePres;
        BOOL                    bFileShareAbility;
        tagFriendInfo()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TFriendInfo, *PTFriendInfo;
    
    LIBJINGLE_API PTFriendInfo GetFriendAllInfo( HANDLE hApp, const PTJid ptBareJid, PTFriendInfo ptInfo, unsigned long * pdwSize );


    // 字符串jid和结构体jid转换
    LIBJINGLE_API ELibjingleError StrJid2TJid( const char * szJid, LibJingleEx::TJid & tJid  );
    LIBJINGLE_API ELibjingleError TJid2StrJid( const LibJingleEx::TJid & tJid, char * szJid, unsigned int dwSize );

   
    // 获取房间所有的成员(除自己外)
    // pdwStartIndex   IN OUT
    //                 输入： 为请求的开始地址
    //                 输出： 为下次请求的开始地址
    LIBJINGLE_API ELibjingleError GetAllRoomMembers( HROOM hRoom, PTJid ptMembers, unsigned long * pdwSize, 
                                                     unsigned long * pdwStartIndex = 0 );

    /************************************* debug use **************************************************/

    // xmpp消息
    typedef void (* LIBJINGLE_CALLBACK POnXmppMessage)( const char * pbyData, unsigned long dwSize,
                                                        BOOL bInput );
    // 内部的日志
    typedef void (* LIBJINGLE_CALLBACK POnLogInfo)( const char * szLogMessage );

    // xmpp消息和内部日志都属于调试范围
    // 设置调试用的回调
    LIBJINGLE_API void SetDebugInfoHandler( POnXmppMessage pOnXmppMessage, POnLogInfo pOnLogMessage );

    enum EDebugInfoIndex
    {
        DEBUG_INFO_INDEX_GROUP     =  0,
        DEBUG_INFO_INDEX_MEMBER_GROUP,
        DEBUG_INFO_INDEX_CHATROOM,
        DEBUG_INFO_INDEX_DMS,
        DEBUG_INFO_INDEX_CONFIG,
        DEBUG_INFO_INDEX_GROUP0
    };
    // 打印和某个app相关的调试信息，输出可从上述的pOnLogMessage获得
    LIBJINGLE_API void PrintDebugInfo( HANDLE hApp, unsigned long dwSwitch = -1 );

    LIBJINGLE_API void PrintAllApps( );

    // 发送任意的 Xmpp stanza，需要先登录
    LIBJINGLE_API ELibjingleError SendXmppMessage( HANDLE hApp, const char * szMessage );

    /********************************** end debug use **************************************************/




    /***************************** 文件传输 **************************/

    
    /********************************************************/
    // 函数名称：CreateSession
    // 功能：    创建一个session用来发送文件
    // 参数：    hApp，之前创建的app句柄
    //           pSessioin，输出参数，Session句柄
    //           ptJid，目的jid( full jid, resouce不能为空 )
    //           pManifest，要发送的文件和文件夹
    //           szLocalDir，设置默认的路径，用来发送文件时的参考路径
    // 说明： session结束时自动销毁之前分配的内存，所以没有destroysession接口
    /********************************************************/
    LIBJINGLE_API ELibjingleError  CreateSession( HANDLE hApp,        HSession * pSession, 
                                                  const PTJid ptJid,  const FileShareManifest * pManifest,
                                                  const char * szLocalDir = 0 );

    /********************************************************/
    // 函数名称：AcceptSession
    // 功能：    接受文件传输请求
    /********************************************************/
    LIBJINGLE_API ELibjingleError  AcceptSession( HSession hSession, const char * szSavePath = 0 );


    /********************************************************/
    // 函数名称：DeclineSession
    // 功能：    拒绝接受文件传输请求
    /********************************************************/
    LIBJINGLE_API ELibjingleError  DeclineSession( HSession hSession );


    /********************************************************/
    // 函数名称：CancelSession
    // 功能：    中断文件传输
    /********************************************************/
    LIBJINGLE_API ELibjingleError  CancelSession( HSession hSession );


    /********************************************************/
    // 函数名称：SetFileSavePath
    // 功能：    设置文件传输的保存路径
    /********************************************************/
    LIBJINGLE_API ELibjingleError  SetFileSavePath( HANDLE hApp, const char * szFilePath );


    /********************************************************/
    // 函数名称：SetFileShareStreamBitRate
    // 功能：    设置文件传输的保存路径
    /********************************************************/
    LIBJINGLE_API void SetFileShareStreamBitRate( unsigned long dwKbps );




    LIBJINGLE_API unsigned long GetDirSize( const char * szDirectory);

    LIBJINGLE_API unsigned long GetFileSize( const char * szFile );

    /***************************** 文件传输结束 **************************/


    /****************** VCARD *************/
    // 当ptJid为0时，获取自己的vcard。当不为0时，获取其他人的vcard
    LIBJINGLE_API   ELibjingleError  GetVCard( HANDLE hApp, const TJid * ptJid = 0 );
    // 设置自己的vcard
    LIBJINGLE_API   ELibjingleError  SetVCard( HANDLE hApp, const TVCard * ptVCard );
    /***************** END **************/


    /************** 设置密码 *************/
    LIBJINGLE_API   ELibjingleError  SetPassword( HANDLE hApp, const char * szPassword = 0 );
    /*************************************/




    const unsigned int MAX_DMS_SUB_GROUPS_IN_A_GROUP           =  128;
    const unsigned int MAX_DMS_MEMBERS_IN_A_GROUP              =  128;
    // const unsigned int MAX_DMS_TOTAL_MEMBERS_COUNT             =  1024;
    const unsigned int MAX_DMS_TOTAL_MEMBERS_COUNT             =  256;


    // 顶级组的父级组id为0
    // 组的id从1开始
    const unsigned long  MAX_DMS_GROUP_NAME_LEN  = 64 + 4;
    typedef struct tagDmsGroup
    {
        unsigned long   dwGroupId;
        unsigned long   dwParentId;
        char            szGroupName[MAX_DMS_GROUP_NAME_LEN];

        tagDmsGroup()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TDmsGroup, *PTDmsGroup;

    LIBJINGLE_API  ELibjingleError  DmsAddGroup( HANDLE hApp, PTDmsGroup    ptDmsGroup );
    LIBJINGLE_API  ELibjingleError  DmsDelGroup( HANDLE hApp, unsigned long dwGroupId, BOOL bMemberReserverd = FALSE );
    LIBJINGLE_API  ELibjingleError  DmsModifyGroup( HANDLE hApp, PTDmsGroup    ptDmsGroup );
    LIBJINGLE_API  ELibjingleError  DmsMoveGroup  ( HANDLE hApp, unsigned long dwGroupId, 
                                                 unsigned long dwNewParentGroupId );

    LIBJINGLE_API  ELibjingleError  DmsQuerySubGroups( HANDLE hApp, unsigned long dwParentGroupId, 
                                                    PTDmsGroup    ptDmsGroups,
                                                    unsigned long * pdwGroupsSize );

    LIBJINGLE_API  ELibjingleError  DmsQueryGroupDepth( HANDLE hApp, unsigned long dwGroupId, 
                                                        unsigned long * pdwGroupsDepth );


    const unsigned long MAX_DMS_MEMBER_NAME_LEN          =  64 + 4;
    const unsigned long MAX_JID_LEN                      =  64;
    const unsigned long MAX_DMS_MEMBER_RESERVERD_BUF_LEN          =  32;
    const unsigned long MAX_DMS_MEMBER_RESERVERD_BUF_CNT          =  4;

    enum EDmsMemberType 
    {
        DMS_MEMBER_TYPE_JID,        // jid类型
        DMS_MEMBER_TYPE_PHONE,		// 电话类型	
        DMS_MEMBER_TYPE_FXO	        // FXO电话类型	
    };

    typedef struct tagDmsMember
    {
        char               szJid[MAX_JID_LEN];

        unsigned  long     dwMemberId;
        unsigned  long     dwGroupId;
        char               szName[MAX_DMS_MEMBER_NAME_LEN];
        EDmsMemberType     eType;
        char               szReserved[MAX_DMS_MEMBER_RESERVERD_BUF_CNT][MAX_DMS_MEMBER_RESERVERD_BUF_LEN];

        tagDmsMember()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TDmsMember, *PTDmsMember;
    
    LIBJINGLE_API  ELibjingleError  DmsAddMember( HANDLE hApp, PTDmsMember     ptDmsMember );
    LIBJINGLE_API  ELibjingleError  DmsDelMember( HANDLE hApp, unsigned long   dwMemberId );
    // 注意：只能修改名称，不能修改jid或类型
    LIBJINGLE_API  ELibjingleError  DmsModifyMember( HANDLE hApp, PTDmsMember  ptDmsMember );
    LIBJINGLE_API  ELibjingleError  DmsMoveMember  ( HANDLE hApp, unsigned long dwMemberId, 
                                                  unsigned long dwNewGroupId );

    LIBJINGLE_API  ELibjingleError  DmsQueryMembers (  HANDLE hApp,    unsigned long dwParentGroupId, 
                                                       PTDmsMember     ptDmsMembers,
                                                       unsigned long * pdwMembersSize,
                                                       unsigned long * pdwStartIndex = 0,
                                                       unsigned long * pdwEndIndex   = 0 );

    LIBJINGLE_API ELibjingleError DmsGetPresenceByGroup( HANDLE hApp, TPresence * ptPresence,
                                                         unsigned long * pdwSize, unsigned long dwGroupId, 
                                                         unsigned long * pdwStartIndex = 0 );


    const unsigned int MAX_DMS_MEMBER_STATE_CNT = 6;   // 例如： 有tl, tp, td 和 ts4种终端类型

    // dms成员在每种终端上的状态
    typedef struct tagDmsMemberState
    {
        char               szJid[MAX_JID_LEN];
        EPresence          eMaxPres;                          // 在多处终端上的最高状态
        EPresence          ePres[MAX_DMS_MEMBER_STATE_CNT];   // (和输入参数对应的)各终端上的状态

        BOOL               bMaxAudio;                         // 既有音频能力，又连上GK
        BOOL               bMaxVideo;                         // 既有视频能力，又连上GK

        BOOL               bAudio[MAX_DMS_MEMBER_STATE_CNT];   // 音频能力(硬件)
        BOOL               bVideo[MAX_DMS_MEMBER_STATE_CNT];   // 视频能力(硬件)
        BOOL               bCall[MAX_DMS_MEMBER_STATE_CNT];    // 呼叫能力(连上GK否)

        tagDmsMemberState()
        {
            memset( szJid, 0, sizeof(szJid) );
            eMaxPres = PRESENCE_NONE;

            bMaxAudio = FALSE;
            bMaxVideo = FALSE; 

            unsigned long i;
            for ( i = 0; i < MAX_DMS_MEMBER_STATE_CNT; i++ )
            {
                ePres[i] = PRESENCE_NONE;
                bAudio[i] = FALSE;
                bVideo[i] = FALSE;
                bCall[i] = FALSE;
            }
        }
    }TDmsMemberState, *PTDmsMemberState;

    // 参数aszRes为查询哪些resource状态, dwResCnt为数组aszRes的元素个数
    LIBJINGLE_API ELibjingleError DmsGetPresenceByGroup( HANDLE hApp, 
                                                         const char aszRes[][MAX_RESOURCE_LEN], unsigned long dwResCnt,
                                                         PTDmsMemberState  ptMemberState,       unsigned long * pdwSize, 
                                                         unsigned long dwGroupId, 
                                                         unsigned long * pdwStartIndex = 0 );

    // 说明：aPres数组元素个数必须和aszRes相同，即dwResCnt
    LIBJINGLE_API ELibjingleError  GetMemberCapabilities( HANDLE hApp, char aszRes[][MAX_RESOURCE_LEN], 
                                                          unsigned long dwResCnt, const PTJid ptBareJid, 
                                                          PTDmsMemberState ptMemberState );


    LIBJINGLE_API  ELibjingleError  DmsQueryMember (  HANDLE hApp, unsigned long dwMemberId, PTDmsMember ptDmsMember );



    /*****************************************************************************/
    // 功能: 把字节流转换成base64格式               
    /*****************************************************************************/
    LIBJINGLE_API  int EncodeBase64( char * szBase64, unsigned long & dwSize, 
                                     const            unsigned char * pbySrc, 
                                     unsigned long dwSrcLength );

    LIBJINGLE_API  int DecodeBase64( unsigned char * pbyDest, unsigned long * pdwSize, 
                                                  const char * szBase64 );



    ////////////////////////// PUBSUB ////////////////////////////////////////
    const unsigned long MAX_PUBSUB_NODE_NAME_LEN                      =  32;

    typedef struct tagPubsubParam
    {
        char    szNodeName[MAX_PUBSUB_NODE_NAME_LEN];
        tagPubsubParam()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TPubsubParam, *PTPubsubParam;

    // 创建
    LIBJINGLE_API  ELibjingleError  CreatePubsubNode( HANDLE hApp, const TPubsubParam * ptParam, 
                                                      unsigned long dwContextId );

    // 订阅
    LIBJINGLE_API  ELibjingleError  SubscribePubsubNode( HANDLE hApp, const char * szPubsubNodeName,
                                                         unsigned long dwContextId );

    // 删除
    LIBJINGLE_API  ELibjingleError  DeletePubsubNode( HANDLE hApp, const char * szPubsubNodeName,
                                                      unsigned long dwContextId   );
    ///////////////////////// END /////////////////////////////////////////////////////



    LIBJINGLE_API  void  SetTempPath( const char * szTempPath );

    LIBJINGLE_API  ELibjingleError  SendPing( HANDLE hApp );


    LIBJINGLE_API  BOOL test(HANDLE hApp);

    LIBJINGLE_API  const char * GetCompileTime( );
    LIBJINGLE_API  const char * GetVersion( );




    LIBJINGLE_API  ELibjingleError  TryEnterAllPersistentChatrooms( HANDLE hApp );



    const unsigned long MAX_CUSTOM_PARAMS_CNT     = 4;
    const unsigned long MAX_CUSTOM_PARAM_BUF_LEN  = 4096;

    typedef struct tagCustomMsg
    {
        int     nMsgType;                                      // 类型
        BOOL    bOffline;                                      // 可否为离线消息

        int     nParams[MAX_CUSTOM_PARAMS_CNT];                // int 参数
        char    achBuf[MAX_CUSTOM_PARAM_BUF_LEN];              // buffer 参数

        tagCustomMsg()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TCustomMsg, *PTCustomMsg;

    LIBJINGLE_API  ELibjingleError  SendCustomMsg( HANDLE hApp, const PTJid pJid, const TCustomMsg * ptCustomMsg );


    const unsigned long MAX_CUSTOM_CONFIG_KEY_LEN    =    32;
    const unsigned long MAX_CUSTOM_CONFIG_VALUE_LEN  =    4096;

    typedef struct tagCustomConfigItem
    {
        char      szKey[MAX_CUSTOM_CONFIG_KEY_LEN];
        char      szValue[MAX_CUSTOM_CONFIG_VALUE_LEN];

        tagCustomConfigItem()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TCustomConfigItem, *PTCustomConfigItem;


    LIBJINGLE_API  ELibjingleError  SetCustomCfg( HANDLE hApp, const TCustomConfigItem * achCfgItems, unsigned long dwCnt );
    LIBJINGLE_API  ELibjingleError  GetCustomCfg( HANDLE hApp, TCustomConfigItem * ptItem );


    LIBJINGLE_API extern const char *  SCREENED_CHATROOMS_KEY;



    LIBJINGLE_API void SetTrueSocialName( const char * szJid, const char * szTureSocialName, BOOL bUtf8 = FALSE );


#ifdef _LINUX_
    // void PrintMemInfo();
#endif

};

#endif
