/*****************************************************************************
ģ����      : 
�ļ���      : libjingle.h
����ļ�    : 
�ļ�ʵ�ֹ���: �ṩ����libjingle��װ��Ŀ⣬��libjingle��ʹ��
����        : 
�汾        : Copyright(C) 2004-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
�޸ļ�¼:
��  ��          �汾        �޸���      �޸�����
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
	// �ṩ�ӿڵ�ͨ�÷���ֵ
	enum ELibjingleError
	{
		LIBJINGLE_ERR_OK                       = 0,          // �ɹ�--0
		LIBJINGLE_ERR_ALREADY_INITED,                        // �Ѿ���ʼ��--1
		LIBJINGLE_ERR_NOT_INITED,                            // û�г�ʼ��--2
		LIBJINGLE_ERR_INVALID_ARGUMENT,                      // ��Ч����--3
		LIBJINGLE_ERR_INVALID_JID,                           // ��Ч��Jid��ʽ--4
		LIBJINGLE_ERR_NOT_ENOUGH_MEM,                        // û���ڴ�--5
        LIBJINGLE_ERR_FAILED_TO_INIT_SSL,                    // ��ʼ��sslʧ��--6
        LIBJINGLE_ERR_TOO_LONG_STRING,                       // �ַ���̫��--7
        LIBJINGLE_ERR_ALREADY_EXISTS,                        // �Ѿ����ڣ��������--8
        LIBJINGLE_ERR_NO_SUCH_FRIEND,                        // û���ҵ�����--9
        LIBJINGLE_ERR_NO_SUCH_GROUP,                         // û���ҵ���--10
        LIBJINGLE_ERR_NOT_LOGIN,                             // ��û�е�¼--11
        LIBJINGLE_ERR_NOT_CHATROOM_SERVICE,                  // û��chatroom����--12
        LIBJINGLE_ERR_ROOM_NOT_READY,                        // ����û��׼����--13
        LIBJINGLE_ERR_INVITEE_ALREADY_IN_ROOM,               // Ⱥ���Ѵ��ڴ���ϵ��--14
        LIBJINGLE_ERR_NOT_OWNER,                             // --15
        LIBJINGLE_ERR_NO_SUCH_PARTICIPANT,                   // --16
        LIBJINGLE_ERR_INVALID_ROOM,                          // --�Ƿ���ȺID--17
        LIBJINGLE_ERR_NOT_FOUND,                             // --18
        LIBJINGLE_ERR_INVALID_PRESENCE,                      // �Ƿ���״̬--19
        LIBJINGLE_ERR_NO_FILE_SHARE_ABILITY,                 // �Լ�û���ļ���������--20
        LIBJINGLE_ERR_PEER_NO_FILE_SHARE_ABILITY,            // �Է�û���ļ���������--21
        LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY,            // ˽������û��׼����--22
        LIBJINGLE_ERR_DMS_NO_SUCH_PARENT_GROUP,              // ���鲻����--23
        LIBJINGLE_ERR_DMS_REACH_MAX_SUB_GROUP_COUNT,         // ������Ŀ�Ѿ����--24
        LIBJINGLE_ERR_DMS_REACH_MAX_SUB_MEMBER_COUNT,        // ���Ա��Ŀ�Ѿ����--25
        LIBJINGLE_ERR_DMS_NO_SUCH_GROUP,                     // �鲻����--26
        LIBJINGLE_ERR_DMS_HAS_SUB_GROUP,                     // �����Ѵ���--27
        LIBJINGLE_ERR_DMS_NOT_ENOUGH_BUFF,                   // ������--28
        LIBJINGLE_ERR_DMS_FRIEND_ALREADY_IN_THE_GROUP,       // �����Ѵ��ڴ���ϵ��--29
        LIBJINGLE_ERR_DMS_CANNOT_ADD_SELF,                   // ��������Լ�Ϊ���Ա--30
        LIBJINGLE_ERR_DMS_NO_SUCH_FRIEND,                    // �����ڴ���ϵ��--31
        LIBJINGLE_ERR_DMS_MOVE_METHOD_FORBIDDEN,             // �Ƿ�����--32
        LIBJINGLE_ERR_DMS_FORBID_ADD_SUBGROUP,               // ��ֹ�ڡ�δ���顱�������--33
        LIBJINGLE_ERR_NO_PUBSUB_SERVICE,
        LIBJINGLE_ERR_DMS_REACH_MAX_TOTOL_MEMBERS_COUNT,    // �ﵽ����Ա��
        LIBJINGLE_ERR_NOT_PERSISTENT_ROOM,                  // ���ǳ־��Է���
        LIBJINGLE_ERR_PARAM_BUFF_FULL,                      // ����buff���������ٴε��ã� 
        LIBJINGLE_ERR_CANNOT_INVITE_SELF,                   // ���������Լ���������
        LIBJINGLE_ERR_INVALID_HANDLE,                       // ��Ч���( hApp, ������������ �ļ������� )
        LIBJINGLE_ERR_DISCONNECTED,                         // �����ѶϿ�
        LIBJINGLE_ERR_REACH_MAX_TEMP_CHATROOM_LIMIT,        // ��������Ƶ������(��ʱ�Ļ�̶�������)
        LIBJINGLE_ERR_REACH_MAX_PERSISTENT_CHATROOM_LIMIT,  // ��������Ƶ������(��ʱ�Ļ�̶�������)
        LIBJINGLE_ERR_CHATROOM_ROSTER_NOT_READY,            // ��û�л�ȡ�����ҵĳ�Ա�б�
        LIBJINGLE_ERR_CHATROOM_ROSTER_IS_FULL,              // �����ҵĳ�Ա��������(192��)
        LIBJINGLE_ERR_FAILED_TO_OPEN_FILE,                  // ���ļ�ʧ��
        LIBJINGLE_ERR_BUSY,                                 // ��æ
        LIBJINGLE_ERR_NO_SUCH_CHATROOM,                     // û�������ķ���
        LIBJINGLE_ERR_NOT_ALLOWED,                          // ������
        LIBJINGLE_ERR_CHAR_STATE_NOT_CHANGED,               // ����״̬û�иı�
        LIBJINGLE_ERR_CONFIG_NOT_READY,                     // ˽�������е����û�û�л�ȡ��
        LIBJINGLE_ERR_NOT_GET_SERVER_TIME,                  // ��û�л�ȡ��������ʱ��
        LIBJINGLE_ERR_CHAT_MSG_NO_RESPONSE,                 // ����������Ϣ��û�лظ��Ƿ��ͳɹ�
        LIBJINGLE_ERR_PARENT_GROUP_NOT_SUPPORTED,           // ROSTER��֧�ָ���
        LIBJINGLE_ERR_ROSTER_GROUP_EXISTS,                  // ROSTER���Ѿ�����
        LIBJINGLE_ERR_NO_SUCH_ROSTER_GROUP,                 // ROSTER�鲻����
        LIBJINGLE_ERR_ROSTER_GROUP_TO_BE_DELETED,           // ��Ҫ��ɾ����ROSTER��
        LIBJINGLE_ERR_ROSTER_CONTACT_EXISTS,                // ROSTER���Ѿ����ڸ���ϵ��
        LIBJINGLE_ERR_ROSTER_NO_CONTACT,                    // ROSTER�ﲻ���ڸ���ϵ��
        LIBJINGLE_ERR_ROSTER_NOT_READY,                     // ��û�л�ȡ��roster����
        LIBJINGLE_ERR_ROSTER_UNSCRIBED_CONTACT,             // û�ж���״̬����ϵ��
        LIBJINGLE_ERR_CANT_MODIFY_SPECIAL_GROUP,            // �����޸ġ�δ���顱
        LIBJINGLE_ERR_ROSTER_GROUP_NAME_CONFLICT,           // ��ϵ��������ͻ
        
		LIBJINGLE_ERR_ERROR                    = -1
	};

	enum ELibjingleState
	{
		STATE_NONE = 0,          // û����
		STATE_START,             // ����
		STATE_OPENING,           // �������ӷ�����
		STATE_OPEN,              // �����Ϸ�����
		STATE_CLOSED             // �ر�
	};

    enum ELibjingleStateSubCode
    {
        STATE_SUBCODE_NONE      = 0,
        STATE_SUBCODE_UNAUTHORIED,      // �û���/�������
        STATE_SUBCODE_CONFLICT,         // ͬһ�˺�(��Դ��Ҳ��ͬ)�������ط���¼
        STATE_SUBCODE_DISCONECTED       // ����Ͽ��������ر����ӣ�
    };

	enum EPresence
	{
		PRESENCE_NONE     = 0,    // ������
		PRESENCE_OFFLINE  = 1,    // ����(�����õ�)
		PRESENCE_XA       = 2,    // ԶԶ�뿪
		PRESENCE_AWAY     = 3,    // �뿪
		PRESENCE_DND      = 4,    // ��Ҫ����
		PRESENCE_ONLINE   = 5,    // ����
		PRESENCE_CHAT     = 6     // ��������
	};

    // �������������ӿڵľ��
    typedef void * HANDLE;

	const unsigned long MAX_NODE_LEN       =   64;   // JID��node��󳤶�
	const unsigned long MAX_DOMAIN_LEN     =   32;   // JID��domain��󳤶�
	const unsigned long MAX_RESOURCE_LEN   =   64;   // JID��resource��󳤶�
    // JID�ṹ��(�ַ���������ʽ node@domain\resource )
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


	const unsigned long MAX_PRESENCE_DESCRIPTION = 32;   // ����״̬�����ַ�����󳤶�

    // ��ϯ״̬
	typedef struct tagPresence
	{
		TJid        tJid;                    // JID
		EPresence   ePresence;               // ��ϯ״̬
		char        sDescription[MAX_PRESENCE_DESCRIPTION]; // ��������

        BOOL        bFileShare;             // �ļ���������
        BOOL        bAudio;                 // ��Ƶ����
        BOOL        bVideo;                 // ��Ƶ����
        BOOL        bCall;                  // �Ƿ�����GK

        tagPresence()
        {
            memset( this, 0, sizeof(*this) );
        }
	}TPresence, *PTPresence;

    // roster(�����б�)�ĸ�ʽ���£�
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
    // ������ֻ��һ��
    //
	const unsigned long MAX_MEMBER_NAME_LEN     = 32;    // ����������󳤶�
    const unsigned long MAX_MEMBER_BITRATE_LEN  = 32;    // ��������

#define SUB_STATE_NONE                    0x00
#define SUB_STATE_SUBSCRIBE               0x01        // �����˶�����״̬(�������һ��д����)
#define SUB_STATE_SUBSCRIBED              0x02        // �����˱��˵�״̬

#define ASK_STATE_NONE                    0
#define ASK_STATE_SUBSCRIBE               1           // ����������ĳ�˵�״̬

	typedef struct tagMember
	{
		TJid        tJid;                         // ����JID
		char        szName[MAX_MEMBER_NAME_LEN];  // ��������
		int         nSubscription;                // ���ĵ�״̬(�ο������ֵ)
        int         nAsk;                         //
		void *      pData;                        // ��ʱû��

        char        szBitRate[MAX_MEMBER_BITRATE_LEN];    //��������
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

	const unsigned long  MAX_GROUP_NAME_LEN = 64+4;     // ������󳤶�
	typedef struct tagGroup
	{
        unsigned long   dwGroupId;
		char       szName[MAX_GROUP_NAME_LEN];        // �������
		PTMember   pChild;                            // ���µ�һ������
        unsigned long     bToBeDeleted;               // ����ɾ��
		tagGroup * pNext;                             // ��һ����

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
        char        szName[MAX_MEMBER_NAME_LEN];  // ��������
        int         nSubscription;                // ���ĵ�״̬(�ο������ֵ)
        int         nAsk;                         //
        void *      pData;                        // ��ʱû��

        char        szBitRate[MAX_MEMBER_BITRATE_LEN];    //��������
        int         nType;

        tagMemberItem()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TMemberItem, *PTMemberItem;


#define LIBJINGLE_CALLBACK





















	// libjingle�¼��Ļص�����
    // *******************************************************
    // *******************************************************
    // ��Ҫ��
    //     1. �ص�������Ҫ���ýӿں���(������)
    //     2. �ص�������Ҫ����������
    // ͨ�����̵߳���API���ص��������ڸ��߳���ص������Postһ����Ϣ�����߳�

    // ��¼�Ļص�
	typedef void (* LIBJINGLE_CALLBACK POnLoginResult)( HANDLE hApp, ELibjingleState eState, ELibjingleStateSubCode eSubCode, void * pUserData );
    // �յ������б�
	typedef void (* LIBJINGLE_CALLBACK POnRosterInfo)( HANDLE hApp, const PTGroup pFirstGroup );
    // ĳ�����Ѹı�������
	typedef void (* LIBJINGLE_CALLBACK POnRosterChange)( HANDLE hApp, 
                                                         const char * szGroupName, 
                                                         const PTJid ptJid, 
                                                         const PTMemberItem ptChange );
    // ������ĳ������
    typedef void (* LIBJINGLE_CALLBACK POnRosterAdd )( HANDLE hApp, 
                                                       const char * szGroupName, 
                                                       const PTJid ptJid, 
                                                       const PTMemberItem ptChange );
    // ʧȥ��ĳ������
    typedef void (* LIBJINGLE_CALLBACK POnRosterDel )( HANDLE hApp, 
                                                       const char * szGroupName, 
                                                       const PTJid ptJid );
    // ���ѳ�ϯ״̬�ı�
	typedef void (* LIBJINGLE_CALLBACK POnRosterPresenceChange)( HANDLE hApp, 
                                                                 const PTPresence pPresence );
    // �յ���Ϣ
	typedef void (* LIBJINGLE_CALLBACK POnChatMessage)( HANDLE hApp, 
                                                        const PTJid pJid, const char * szMessage );
    // �������������״̬
    typedef void (* LIBJINGLE_CALLBACK POnSubscribe)( HANDLE hApp, const PTJid pJid );


    /*=====================================  Ⱥ�Ļص�  =============================================*/
    // �Ƿ���chatroom����
    typedef void ( *LIBJINGLE_CALLBACK POnChatRoomService )( HANDLE hApp, BOOL bAvailable );
    // ����ľ��
    typedef void * HROOM;
    // �����ķ����Ƿ�ɹ���nErrorCodeΪ0ʱ����ʾ�����ɹ�����0��ʧ��
    typedef void (* LIBJINGLE_CALLBACK POnCreateUniqueRoom )( HANDLE hApp, HROOM hRoom, int nErrorCode );
    // �����뵽ĳ������
    typedef void (* LIBJINGLE_CALLBACK POnInvited )( HANDLE hApp, HROOM hRoom, PTJid ptInviterJid, 
                                                     BOOL bPersistent, const char * szRoomName ) ;
    // ����ߺͷ���Ĺ�ϵ
    enum EAffiliation
    {
        AFFILIATION_NONE     = 0,    // �����������
        AFFILIATION_OWNER    = 1,    // ����������
        AFFILIATION_MEMBER   = 2,    // �����Ա
        AFFILIATION_ADMIN    = 3,    // ����admin
        AFFILIATION_UNKNOWN  = -1
    };

    enum EChatRoomPreseFlag
    {
        CHATROOM_PRESENCE_FLAG_LEAVE,           // �뿪����
        CHATROOM_PRESENCE_FLAG_FIRST_ENTER,     // ��һ�ν���
        CHATROOM_PRESENCE_FLAG_CHANGE,          // presence �ı�
        CHATROOM_PRESENCE_FLAG_KICKED           // ���߳�
    };

    typedef void (* LIBJINGLE_CALLBACK POnParticipantPresenceChange )( HROOM hRoom,
                                                  const PTJid ptJid,                               // ����ߵ�jid
                                                  EPresence * peShow, const char * szDescription,
                                                  EAffiliation eAffiliation ,                       // ����ߺͷ���Ĺ�ϵ
                                                  BOOL  bSelf,                                     // Jid�Ƿ��Լ�
                                                  int   nFlag,                                     // �Ƿ��״ν��룬�Ƿ��߳�
                                                  HANDLE       hApp // �˳�����󣬸÷����ڴ����٣����Դ���һ��hApp��������
                                                  );
    // �������µ�Ȩ��(һ���Ƿ���������Ȩ��)
    typedef void (* LIBJINGLE_CALLBACK POnGrant )( HANDLE hApp, HROOM hRoom, EAffiliation eOldAffiliation, EAffiliation eNewAffiliation );
    // �������� 
    typedef void (* LIBJINGLE_CALLBACK POnDestroyRoom )( HROOM hRoom, HANDLE hApp );  // �º���ڴ����٣����Դ���һ��hApp��������
    // �յ�������Ϣ
    typedef void (* LIBJINGLE_CALLBACK POnRoomMessage)( HANDLE hApp, HROOM hRoom, const PTJid pJid, const char * szMessage, const char * szMessageId );    

    /*===================================== END Ⱥ�Ļص�  =============================================*/


    /*=====================================  �ļ�����ص�  =============================================*/
    // ��ʱ�趨һ��Sessionֻ�ܷ���һ���ļ����ļ���
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

    // �����ļ�����ľ��
    typedef void * HSession;

    typedef void (* LIBJINGLE_CALLBACK POnFileOffer)( HANDLE hApp, HSession hSession, const PTJid pJid, const FileShareManifest * pManifest, BOOL bSender );
    typedef void (* LIBJINGLE_CALLBACK POnFileTransfer)( HANDLE hApp, HSession hSession );
    typedef void (* LIBJINGLE_CALLBACK POnFileComplete)( HANDLE hApp, HSession hSession, const char * szCompletePath );
    typedef void (* LIBJINGLE_CALLBACK POnFileCancel)( HANDLE hApp, HSession hSession, BOOL bLocal );
    typedef void (* LIBJINGLE_CALLBACK POnFileFailure)( HANDLE hApp, HSession hSession, int nErrorCode );
    typedef void (* LIBJINGLE_CALLBACK POnFileProgress)( HANDLE hApp, HSession hSession, int nPercent, 
        const char * szCurFileName, unsigned long dwTransferSize, unsigned long dwTransferRate );

    /*===================================== END �ļ�����ص�  =============================================*/

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
        char      m_achNickName[VCARD_MAX_NICK_LEN];         // �û��ǳ�
        BOOL      m_bCustomHeadPortrait;                     // �Ƿ�ʹ���Զ���ͼ��
        int       m_nSysHeadPortraitIndex;                   // ʹ��ϵͳͼ�������
        EImageType       m_eCustomHeadPortraitType;          // �Զ���ͷ������
        unsigned char    m_abyCumtomHeadPortrait[VCARD_MAX_HEAD_PORTRAIT_SIZE];    // �Զ���ͷ��
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


	// �ص���������һ������
	typedef struct tagLibJingleEvent
	{
		POnLoginResult          pOnLoginResult;                      // ��¼�ص�
		POnRosterInfo           pOnRosterInfo;                       // ���������ص���
                                                                     // ���Ҫ����ʹ�ûص��Ĳ���������и���
		POnRosterChange         pOnRosterChange;
        POnRosterAdd            pOnRosterAdd;
        POnRosterDel            pOnRosterDel;
		POnRosterPresenceChange pOnRosterPresenceChange;             // ����״̬�ı�ص�
		POnChatMessage          pOnMessage;                          // ���յ�������Ϣ
        POnSubscribe            pOnSubscribe;                        // �յ�״̬����

        POnChatRoomService      pOnChatRoomService;                  // �Ƿ��������ҹ���
        POnCreateUniqueRoom     pOnCreateUniqueRoom;                 // ��������Ļص�
        POnInvited              pOnInvited;                          // ����������˷���
        POnParticipantPresenceChange pOnParticipantPresenceChange;   // ĳ�����ҵ��������Ϣ
        POnGrant                pOnGrant;                            // ������������������
        POnDestroyRoom          pOnDestroyRoom;
        POnRoomMessage          pOnRoomMessage;                      // �յ�������Ⱥ����Ϣ

        POnFileOffer            pOnFileOffer;                         // �յ��ļ�����offer
        POnFileTransfer         pOnFileTransfer;                      // ���俪ʼ
        POnFileComplete         pOnFileComplete;                      // �������
        POnFileCancel           pOnFileCancel;                        // ������ֹ
        POnFileFailure          pOnFileFailure;                       // ����ʧ��
        POnFileProgress         pOnFileProgress;                      // �������

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
        char             szData[MAX_KEDA_WEIBO_DATA_ITEM_CNT][MAX_KEDA_WEIBO_DATA_LEN]; // ����
        unsigned long    dwCnt;                                                         // ��������

        char             szKey [MAX_KEDA_WEIBO_DATA_ITEM_CNT][MAX_KEDA_WEIBO_KEY_LEN];  // key
        tagKedaWeiboMsg()
        {
            memset( this, 0, sizeof(*this) );
        }
    }TKedaWeiboMsg, *PTKedaWeiboMsg;

    
    typedef struct tagRoomRosterItem
    {
        LibJingleEx::TJid                m_tJid;                     // �����Ա��jid
        LibJingleEx::EAffiliation        m_eAlliliation;             // �����Ա��Ȩ��
        BOOL                             m_bSelf;                    // �Ƿ�ͬһ�˺�

        tagRoomRosterItem()
        {
            m_eAlliliation = LibJingleEx::AFFILIATION_NONE;
            m_bSelf = FALSE;
        }
    }TRoomRosterItem, *PTRoomRosterItem;

    const unsigned long MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT            =  20;

    enum EFileShareError
    {
        FILE_SHARE_ERR_NO_SPACE          = 1,                            // ���̿ռ���
        FILE_SHARE_ERR_DISCONNECTED,                                     // ��������Ͽ�
        FILE_SHARE_ERR_NOT_FOUND,                                        // �ļ�û���ҵ�
    };

    const unsigned long MAX_SID_LEN = 32;


    enum EMsgId
    {                                     /***********              ���ݸ�ʽ              ****************/
        ON_STATE                    = 1,  // ELibjingleState
        ON_ROSTER_INFO,                   // PTGroup
        ON_ROSTER_CHANGE,                 // char [MAX_GROUP_NAME_LEN] + TJid + TMemberItem
        ON_ROSTER_ADD,                    // char [MAX_GROUP_NAME_LEN] + TJid + TMemberItem
        ON_ROSTER_DEL,                    // char [MAX_GROUP_NAME_LEN] + TJid
        ON_FRIEND_PRESENCE,               // TPresence + BOOL(�Ƿ�ͬһ�˺�) + EPresence(���״̬) + BOOL(�Ƿ��һ��) + BOOL(��Ƶ����) + BOOL(��Ƶ����) + BOOL(�Ƿ�����status chatroom) + BOOL(����һ�ε�״̬�Ƚ��Ƿ��б任) + bDelay + bLoginBeforeMe
        ON_CHAT_MESSAGE,                  // TJid + BOOL(�Ƿ�����) + �������ַ���(������������0)
        ON_QUERY_SUBSCRIBED,              // TJid

        ON_GROUP_CHAT_ABILITY,            // BOOL
        ON_CREATE_UNIQUE_CHATROOM,        // HROOM + int + char array( room id, length = 64 ) + char array( service name, 64 )
        ON_INVITED_TO_CHATROOM,           // HROOM + TJid(inviter) + BOOL( is persistent? ) + int(type) + char array( LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN�� room name ) + roomid + service name
        ON_PARTICIPANT_PRESENCE,          // HROOM + TJid + EPresence * + const char * + EAffiliation + BOOL + EChatRoomPreseFlag + MaxPresence + char array( room id, length = 64 ) + char array( service name, 64 )
        ON_GRANT,                         // HROOM + EAffiliation
        ON_DESTROY_CHATROOM,              // HROOM + char array( room id, length = 64 ) + char array( service name, 64 )
        ON_GROUPCHAT_MESSAGE,             // HROOM + TJid + BOOL(�Ƿ�������Ϣ) + char array(��Ϣid) + TDmsChatContent + char array( room id, length = 64 ) + char array( service name, 64 )

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

        ON_ROOM_CONFIG_MSG,                // ��ȡ��������(ͨ���Ƿ�������)
                                          // char array(LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN)

        ON_ACCEPT_INVITE_RESULT,          // ���ܷ�������Ľ��( HROOM + BOOL(TRUE, ��������ɹ���FALSE�������Ѿ������ڣ�����ʧ��) )

        
        ON_MEMBERS_DATA_READY,            // ��ϵ�����ݿ���
        ON_NOTIFY_SECURITY,               // ֪ͨͬһ�˺��������ط���¼

        ON_DECLINE_INVITING,             // ����������Ŀ���ƣ����뱻�ܾ�֪ͨ
                                         // char array( room id, length = 64 ) + char array( service name, 64 ) 
                                         // + char array( LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN�� room name ) + BOOL(�Ƿ�̶�Ⱥ) + TJid(�ܾ���)

        ON_OFFLINE_MSG_OVERFLOW,         // TJid( ���͸�˭ )

        ON_ROOM_ROSTER_ADDED,            // �����ҵĳ�Ա�б�������N����Ա  HROOM + room id + unsigend long ( N ) + TRoomRosterItem array ( N �� )
        ON_ROOM_ROSTER_DELETED,          // �����ҵĳ�Ա�б������N����Ա  HROOM + room id + unsigend long ( N ) + TRoomRosterItem array ( N �� )


        ON_CHATROOM_PIC_OPEN,            // ������ͼƬ���俪ʼ  HROOM + TJid + SessionId( MAX_SID_LEN ) + DWORD( size ) + char array( MAX_FILENAME_SIZE, picture name ) + room_id + room_service
        ON_CHATROOM_PIC_DATA,            // ������ͼƬ��������  HROOM + TJid + SessionId                    + DWORD( progress ) + room_id + room_service
        ON_CHATROOM_PIC_CLOSE,           // ������ͼƬ�������  HROOM + TJid + SessionId + pic final path( MAX_PATH_SIZE size ) + room_id + room_service
        ON_CHATROOM_PIC_FAIL,            // ������ͼƬ��������ʧ�� HROOM + TJid + SessionId + errorCode + room_id + room_service


        ON_CHATROOM_ADD_MEMBER_NOTIFY,   // ���������ӳ�Ա֪ͨ   HROOM + TJid  + room_id + room_service       
                                         // ON_ROOM_ROSTER_ADDED�����������ٴ����߻�ȡroster�Ļ��ǵ�ǰ����Ա�����ȡ��


        ON_PERSISTENT_CHATROOMS_NOTIFY,      // ֪ͨ��һ�ε�¼�б���Ĺ̶���������Ϣ  N + ( TJid( ������jid )  + char array ( LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN ) ) * N
        ON_PERSISTENT_CHATROOMS_NOTIFY_FIN,  // ֪ͨ��һ�ε�¼�б���Ĺ̶���������Ϣ����


        ON_ENTER_PERSISTENT_ROOM_FAIL,   // char array( room name, length LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN ) + room id + service name

        ON_CHAT_STATE,                  // TJid + ECharState

        ON_CUSTOM_MESSAGE,              // TJid + LibJingleEx::TCustomMsg

        ON_CUSTOM_CONFIG_DATA_READY,    // 
        ON_CUSTOM_CONFIG_CHANGE,        // TCumstomConfigItem + Old value ( char array[szValue[MAX_CUSTOM_CONFIG_VALUE_LEN]] )


        ON_P2P_CHAT_MESSAGE_ACK,        // unsigned long(context id) + BOOL
        ON_GROUP_CHAT_MESSAGE_ACK,      // HROOM + unsigned long(context id) + BOOL + room id + chat room service

        ON_NOTHING
    };

    // �ص�����
    typedef void (* LIBJINGLE_CALLBACK POnMsg )( EMsgId eMsgId, HANDLE hApp,     void * pUserData, 
                                                 const unsigned char * pbyMsg,   unsigned long dwMsgLen );


    const unsigned long MAX_MESSAGE_DATA_LENGTH  = 10000;

    class CMsgData
    {
    public:
        LIBJINGLE_API CMsgData( const unsigned char * pbyData = 0, unsigned long dwDataLen = 0 );

        LIBJINGLE_API BOOL Append( const unsigned char * pbyData, unsigned long dwDataLen ); // д�����ݵ�ĩβ
        LIBJINGLE_API void Clear();

        LIBJINGLE_API BOOL Read( unsigned char * pbyData, unsigned long dwDataSize = -1 );        // �ӵ�ǰ��λ�ÿ�ʼ��ȡ����
        LIBJINGLE_API void ResetReadPos();                                                   // �Ѷ�����Ϊ0

        LIBJINGLE_API unsigned long         GetMsgLength() const;
        LIBJINGLE_API const unsigned char * GetMsgData() const;

        LIBJINGLE_API unsigned long   GetReadPos() const;

    private:
        unsigned char    m_abyData[MAX_MESSAGE_DATA_LENGTH];            // ����
        unsigned long    m_dwDataLen;                                   // ���ݳ���

        unsigned long    m_dwRPos;                                      // ��ǰ����λ��
    };


    enum EDeclineReason
    {
        DECLINE_REASON_UNKNOWN,                                       // δ֪ԭ��
        DECLINE_REASON_MAX_PERSISTENT_ROOM_COUNT,                     // �ﵽ�̶�Ⱥ�����
        DECLINE_REASON_MAX_TEMPORARY_ROOM_COUNT,                      // �ﵽ��ʱȺ�����
        DECLINE_REASON_FROM_WEIBO                                     // ��΢������ʱ�ľܾ�
    };














    const unsigned long MAX_HOST_NAME_LEN       =            32;
	const unsigned long MAX_PASSWORD_LEN        =            64;
    const unsigned long  MAX_PATH_SIZE          =            256;
    const unsigned long  MAX_ADDR_LEN           =            32;
    const unsigned long  MAX_ENTERPRISE_NAME_LEN    =        64;


    // ��¼ʱʹ�õĽṹ��
	typedef struct tagLoginSettings
	{
		char    sUserName[MAX_NODE_LEN];                     // �û���
		char    sDomain[MAX_DOMAIN_LEN];                     // ����
		char    sPassword[MAX_PASSWORD_LEN];                 // ����
		char    sHost[MAX_HOST_NAME_LEN];                    // ������������
        char    sResource[MAX_RESOURCE_LEN];                 // ��Դ( truelink, truetouch���ַ��� )
		int     bUseTls;                                     // �Ƿ�ʹ��TLS
        int     bUseCompression;                             // �Ƿ�ʹ��zlibѹ��
        unsigned short wPort;                                // �������˿�(���Ϊ0����ΪȡĬ��ֵ5222)
        char    reserved0[2];
        char    sEnterpriseName[MAX_ENTERPRISE_NAME_LEN];

        // �ļ��������
        BOOL               bFileShareEnable;                        // �Ƿ������ļ�����
        char               szStunAddr[MAX_ADDR_LEN];                // stun server ip address
        unsigned short     wStunPort;                               // stun port
        char               reserverd1[2];
        char               szRelayAddr[MAX_ADDR_LEN];               // relay ip address
        char               szDefaultSaveDir[MAX_PATH_SIZE];         // default save directory
        // end �ļ��������


        BOOL               m_bAudio;                                // �Ƿ�����Ƶ��������
        BOOL               m_bVideo;                                // �Ƿ�����Ƶ��������
        BOOL               m_bCall;
        EPresence          m_ePres;


        char              szGroupChatPicSaveDir[MAX_PATH_SIZE];      // ������ͼƬ����·��

        BOOL              bDelayLogin;                               // �Ƿ��ӳٵ�¼

        BOOL              bSs5Proxy;                                 // �Ƿ���socks5����
        char              szSs5Host[MAX_HOST_NAME_LEN];              // ss5��������ip��ַ
        unsigned short    wSs5Port;                                  // ss5�������˿�(���Ϊ0����Ϊ��ȱʡ��1080)
        char              reserved2[2];
        char              szSs5ProxyUsername[MAX_NODE_LEN];          // ss5�û���
        char              szSs5ProxyPassword[MAX_PASSWORD_LEN];      // ss5����

        char              szConfigPath[MAX_PATH_SIZE];               // ����xmpp�����ļ���·��

        tagLoginSettings()
        {
            memset( this, 0, sizeof(*this) );
            m_ePres = PRESENCE_ONLINE;
        }
	}TLoginSettings, *PTLoginSettings;


    // ����API��Ҫ�ڻص����������

    /********************************************************/
    // �������ƣ�Init
    // ���ܣ�    ��ʼ����װ��(ע���¼��ص�����)
    // ������    tEvent�ĳ�Ա����Ϊ��
    // ����ֵ��  0���ɹ�
    /********************************************************/
    LIBJINGLE_API ELibjingleError Init( const PTLibJingleEvent ptEvent );
    LIBJINGLE_API ELibjingleError Init( POnMsg  pOnMsg );

    /********************************************************/
    // �������ƣ�Deinit
    // ���ܣ�    ȥ��ʼ��
    // ����ֵ��  0���ɹ�
    /********************************************************/
    LIBJINGLE_API ELibjingleError Deinit(  );

    /********************************************************/
    // �������ƣ�Login
    // ���ܣ�    ��¼��XMPP������
    // ˵����    ����ֵΪ0ʱ�����͵�¼����Ϣ��XMPP�������ɹ���
    //           �ص�����pOnLoginResult�������¼��״̬��OPEN ʱΪ�ɹ���¼
    // ������    ptLoginSettings����Ϊ��
    /********************************************************/
	LIBJINGLE_API ELibjingleError Login( HANDLE *  phApp, PTLoginSettings ptLoginSettings, void * pUserData = 0 );

    LIBJINGLE_API ELibjingleError SetReady( HANDLE hApp );


    /********************************************************/
    // �������ƣ�Logout
    // ���ܣ�    �ǳ�XMPP������
    // ˵����    ���ú�ײ�TCP����Ͽ���������pOnLogoutResult�ص�
    /********************************************************/
	LIBJINGLE_API ELibjingleError Logout( HANDLE hApp );


    const unsigned long  MAX_DMS_FONT_BUF_SIZE   =  64;
    const unsigned long  MAX_DMS_TIME_BUF_SIZE   =  8;
    const unsigned long  MAX_DMS_HEADER_SIZE     =  64   + 4;
    const unsigned long  MAX_DMS_CONTENT_SIZE    =  2048 + 4;
    const unsigned long  MAX_DMS_SEND_PIC_COUNT  =  4;

    typedef struct  tagTDmsChatPicInfo 
    {
        unsigned long   m_dwIndex;                      // �ļ���������Ӧ�ڲ���richedit�е�dwUser
        char            m_achFileName[MAX_PATH_SIZE];   // ͼƬ�ļ����ƣ���ʽ��GUID_���Ͷ��ʺ�
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
        // unsigned char    m_abyFontBuf[MAX_DMS_FONT_BUF_SIZE];         // ����
        TDmsFont         m_tFont;
        unsigned long    m_dwColor;                                   // �������ݵ���ɫ
        // unsigned char	 m_abyTimeBuf[MAX_DMS_TIME_BUF_SIZE];         // �����ʱ��
        unsigned long    m_dwTime;
        char             m_szMsgHeader[MAX_DMS_HEADER_SIZE];          
        char             m_szContent[MAX_DMS_CONTENT_SIZE];           
        unsigned long    m_dwChatType;
        unsigned long    m_dwPicCount;
        TDmsChatPicInfo  m_atPicFileInfoTable[MAX_DMS_SEND_PIC_COUNT]; // ����ͼƬ�б�һ�����ֻ�ܷ���MAX_SEND_PIC_COUNT��ͼƬ
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
    // �������ƣ�SendMsg
    // ���ܣ�   ������Ϣ��XMPP�������ϵ�һ���û�
    // ������   pJid�� �Է�JID�� ����Ϊ��
    //          szMsg  ���͵��ַ���������Ϊ��
    // ����ֵ�� 0�����ͳɹ�
    /********************************************************/
    const unsigned long MAX_MSG_LENGTH  = 4096;
	LIBJINGLE_API ELibjingleError SendMsg( HANDLE hApp, const PTJid pJid, const char * szMsg, BOOL bUtf8Encode = TRUE );


    LIBJINGLE_API ELibjingleError SendMsg( HANDLE hApp, const PTJid pJid, const TDmsChatContent * ptMsg, unsigned long * pdwSendTime = 0, unsigned long dwContextId = 0 );


    LIBJINGLE_API ELibjingleError SendPicture( HANDLE hApp, const PTJid pJid, 
                                               const unsigned char * pbyPicture, unsigned long dwSize );


    LIBJINGLE_API ELibjingleError SendCharState( HANDLE hApp, const PTJid pJid, ECharStatus eCharState );


    /********************************************************/
    // �������ƣ�AddFriend
    // ���ܣ�    ��Ӻ���
    // ������    pJid��Ҫ��ӵĺ��ѵ�JID������Ϊ��
    //           szGroup�� Ҫ��ӵ����飬����Ϊ��(Ĭ��Ϊfriend)
    //           szNickName������������Ϊ��(ȡjid��nodeֵ)
    // ����ֵ��  0��������Ӻ�����Ϣ�ɹ�
    // ˵����    ������󳤶�ΪMAX_GROUP_NAME_LEN��
    //           ������󳤶�ΪMAX_MEMBER_NAME_LEN
    //           �����ص�OnRosterAdd
    //                 ��OnRosterChange
    /********************************************************/
    LIBJINGLE_API ELibjingleError AddFriend( HANDLE hApp, const PTJid pJid, const char * szGroup = 0, const char * szNickName = 0,
        const char * szBirtate = 0, unsigned long dwType = 0 );


    LIBJINGLE_API ELibjingleError MoveFriend( HANDLE hApp, const PTJid pJid, 
                                              const char * szGroupSrc, const char * szGroupDst );

    /********************************************************/
    // �������ƣ�AcceptFriend
    // ���ܣ�    �Ƿ���ܱ��������Ϊ����
    // ������    pJid��Ҫ�����Ϊ���ѵı��˵�JID������Ϊ��
    //           bAccept�� �Ƿ����
    // ����ֵ��  0�������Ƿ���Ը��Ϣ�ɹ�
    // ˵����    �ص�����pOnSubscribe�ᴫ��һ��pJid�������pJid��Ϊ����
    /********************************************************/
    LIBJINGLE_API ELibjingleError AcceptFriend( HANDLE hApp, const PTJid pJid, BOOL bAccept = TRUE );


    /********************************************************/
    // �������ƣ�DelFriend
    // ���ܣ�    ��ĳ������ɾ�����ѣ���ɾ��������ĸú���(szGroup == 0)
    // ������    pJid�����ѵ�bare JID
    //           szGroup�� ����
    // ����ֵ��  0������ɾ����Ϣ�ɹ�
    // ˵����    �����ص�OnRosterDel
    /********************************************************/
    LIBJINGLE_API ELibjingleError DelFriend( HANDLE hApp, const PTJid pJid, const char * szGroup = 0,
                                             BOOL  bUnsubscribed = TRUE );

    /********************************************************/
    // �������ƣ�SetFriendNick
    // ���ܣ�    �޸ĺ��ѵı���
    // ������    pJid�����ѵ�bare JID
    //           szNick�� �µı���
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    �����ص�OnRosterChange
    /********************************************************/
	LIBJINGLE_API ELibjingleError SetFriendNick( HANDLE hApp, const PTJid pJid, const char * szNick, const char * szBitrate = 0 );


    enum ESetStatusSwitchIndex
    {
        SET_STATUS_SWITCH_INDEX_PRESENCE = 0,
        SET_STATUS_SWITCH_INDEX_CALL
    };

    /********************************************************/
    // �������ƣ�SetStatus
    // ���ܣ�    �����Լ���״̬�����ѻ��յ�������״̬
    // ������    e���Լ�����״̬
    // ����ֵ��  0��������Ϣ�ɹ�
    /********************************************************/
	LIBJINGLE_API ELibjingleError SetStatus( HANDLE hApp, EPresence e, const char * szDescription = 0, BOOL bCall = TRUE, unsigned long dwSwitch = -1 );

    /********************************************************/
    // �������ƣ�Subscribe
    // ���ܣ�    ����Ժ��ѵĹ�ע
    /********************************************************/
    LIBJINGLE_API ELibjingleError Subscribe( HANDLE hApp, const PTJid pJid );

    /********************************************************/
    // �������ƣ�Unsubscribed
    // ���ܣ�    ȡ���Լ������ѹ�ע
    /********************************************************/
    LIBJINGLE_API ELibjingleError Unsubscribed( HANDLE hApp, const PTJid pJid );

    /********************************************************/
    // �������ƣ�Unsubscribe
    // ���ܣ�    ȡ���Ժ��ѵĹ�ע
    /********************************************************/
    LIBJINGLE_API ELibjingleError Unsubscribe( HANDLE hApp, const PTJid pJid );


    /********************************************************/
    // �������ƣ�GetPresence
    // ���ܣ�    ������ӡpresence����������
    /********************************************************/
    LIBJINGLE_API const char * GetPresence( EPresence e );

    /********************************************************/
    // �������ƣ�GetSubscription
    // ���ܣ�    ������ӡsubscription����������
    /********************************************************/
    LIBJINGLE_API const char * GetSubscription( int nSubscription );

    /********************************************************/
    // �������ƣ�GetAsk
    // ���ܣ�    ������ӡask����������
    /********************************************************/
    LIBJINGLE_API const char * GetAsk( int nAsk );



    const unsigned long MAX_CHATROOM_ID_LEN                = 64;
    const unsigned long MAX_SERVICE_NAME_LEN               = 64;


    /*=====================================  Ⱥ��  =============================================*/

    /********************************************************/
    // �������ƣ�CreateUniqueRoom
    // ���ܣ�    ��conference�����ϴ���һ����Ψһ�ŵķ���
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    ��Ӧ�Ļص� pOnCreateUniqueRoom
    /********************************************************/
    LIBJINGLE_API ELibjingleError CreateUniqueRoom( HANDLE hApp, HROOM * phRoom, const char * szName = 0, BOOL bPersistent = FALSE );

    LIBJINGLE_API ELibjingleError CreateOrEnterRoom( HANDLE hApp, HROOM * phRoom, const char * szId, const char * szName = 0 );

    LIBJINGLE_API ELibjingleError EnterPersistentRoom( HANDLE hApp, PTJid  ptRoomJid );

    /********************************************************/
    // �������ƣ�InviteParticipant
    // ���ܣ�    ���������(Ҫ���Լ�Ϊ����������)
    // ����ֵ��  0��������Ϣ�ɹ�
    /********************************************************/
    LIBJINGLE_API ELibjingleError InviteParticipant( HROOM hRoom, const PTJid ptJid );

    /********************************************************/
    // �������ƣ�AcceptInvite
    // ���ܣ�    �Ƿ���ܲμ������ҵ�����
    // ����ֵ��  0��������Ϣ�ɹ�
    /********************************************************/
    LIBJINGLE_API ELibjingleError AcceptInvite( HROOM hRoom, BOOL bAccepted );


    LIBJINGLE_API ELibjingleError GetRoomConfigure( HROOM hRoom );


    /********************************************************/
    // �������ƣ�GrantOwner
    // ���ܣ�    �������Ϊ������(����Ҫ���Լ�Ϊ�����������)
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    ���˻�������ߺ��Լ���ʧȥ������Ȩ��
    //           (1������ֻ����1��������)
    /********************************************************/
    LIBJINGLE_API ELibjingleError GrantOwner( HROOM hRoom, const PTJid ptJid );

    /********************************************************/
    // �������ƣ�QuitRoom
    // ���ܣ�    �뿪����
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    
    /********************************************************/
    LIBJINGLE_API ELibjingleError QuitRoom( HROOM hRoom );

    /********************************************************/
    // �������ƣ�DestroyRoom
    // ���ܣ�    ���ٷ���(����Ҫ���Լ��Ƿ����������)
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    
    /********************************************************/
    LIBJINGLE_API ELibjingleError DestroyRoom( HROOM hRoom );

    /********************************************************/
    // �������ƣ�SendRoomMessage
    // ���ܣ�    �ڷ��䷢�͹㲥
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    
    /********************************************************/
    LIBJINGLE_API ELibjingleError SendRoomMessage( HROOM hRoom, const char * szMessage, BOOL bUtf8Encode = TRUE );

    LIBJINGLE_API ELibjingleError SendRoomMessage( HROOM hRoom, const TDmsChatContent * ptMsg, unsigned long * pdwSendTime = 0, unsigned long dwContextId = 0 );

    
    /********************************************************/
    // �������ƣ�KickRoomMember
    // ���ܣ�    �߳������ҳ�Ա(��ͬ��jid�Ķ����߳�)
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    
    /********************************************************/
    LIBJINGLE_API ELibjingleError KickRoomMember( HROOM hRoom, const TJid * ptJid = 0 );


    // ����Ⱥ����Ϣ�Ѷ�
    // szLastMessageId Ϊ���õ��Ѷ������һ����ϢId
    LIBJINGLE_API ELibjingleError SetGroupchatMsgRead( HROOM hRoom, const char * szLastReadMessageId = 0 );

    // ������������
    LIBJINGLE_API ELibjingleError SetRoomName( HROOM hRoom, const char * szName );


    const unsigned long  MAX_PICTURE_DATA_SIZE = 0x400000;          // 4M�ֽ�
    // ���������﷢��ͼƬ
    LIBJINGLE_API ELibjingleError SendRoomPicture( HROOM hRoom, const char * szPicFilePathName, unsigned long dwPicSize );



    // ֪ͨ�������Լ��ܾ�����
    // nReason =    LIBJINGLE_ERR_REACH_MAX_TEMP_CHATROOM_LIMIT
    //           �� LIBJINGLE_ERR_REACH_MAX_PERSISTENT_CHATROOM_LIMIT
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


    // ������������Ϣ
    LIBJINGLE_API ELibjingleError ScreenChatroomsMsg( HANDLE hApp, const TChatRoomId * atIds, unsigned long dwCnt );

    // ȡ��������������Ϣ
    LIBJINGLE_API ELibjingleError UnScreenChatroomsMsg( HANDLE hApp, const TChatRoomId * atIds, unsigned long dwCnt );

    LIBJINGLE_API ELibjingleError IsScreenedChatroom( HANDLE hApp, const TChatRoomId * pIds, int * pbIsScreened );

    // ��ȡ�����ҵ����г�Ա(�����ߵ�)
    LIBJINGLE_API ELibjingleError GetChatRoomMembers( HROOM hRoom );


    /********************************************************/
    // �������ƣ�GetRoomUniqueId
    // ���ܣ�    ��ȡconference�����ṩ��Ψһ�����id
    // ����ֵ��  0����Ч�ķ���򲻿���
    //           ��0�������id���ַ�����NULL������
    /********************************************************/
    // const char * GetRoomUniqueId( HROOM hRoom );

    /********************************************************/
    // �������ƣ�HANDLE
    // ���ܣ�    ��ȡroom���ڵ�Ӧ�þ��
    /********************************************************/
    LIBJINGLE_API HANDLE GetHApp( HROOM hRoom );

    LIBJINGLE_API ELibjingleError GetRoomId( HROOM hRoom, PTChatRoomId ptRoomId );

    LIBJINGLE_API HROOM GetRoomHandle( HANDLE hApp, const TChatRoomId * ptRoomId );

    /********************************************************/
    // �������ƣ�GetAffiliation
    // ���ܣ�    ��ȡaffiliation���ַ�������
    /********************************************************/
    LIBJINGLE_API const char * GetAffiliation( EAffiliation e );
    /*=====================================  END Ⱥ��  ===========================================*/


    /********************************************************/
    // �������ƣ�GetSelfJid
    // ���ܣ�    ��ȡ�Լ���full jid
    /********************************************************/
    LIBJINGLE_API ELibjingleError GetSelfJid( HANDLE hApp, PTJid ptJid );


    /********************************************************/
    // �������ƣ�GetUserData
    // ���ܣ�    ��ȡ��ʼ��ʱ�Ĳ���pData
    /********************************************************/
    LIBJINGLE_API void * GetUserData( HANDLE hApp );


    /********************************************************/
    // �������ƣ�GetRoster
    // ���ܣ�    ��ȡ�����б�
    /********************************************************/
    LIBJINGLE_API PTGroup GetRoster( HANDLE hApp );


    /********************************************************/
    // �������ƣ�GetFriendPresence
    // ���ܣ�    ��ȡ����״̬
    /********************************************************/
    LIBJINGLE_API EPresence GetFriendPresence( HANDLE hApp, const PTJid ptJid );

    // ˵����aPres����Ԫ�ظ��������aszRes��ͬ����dwResCnt
    LIBJINGLE_API ELibjingleError  GetFriendPresence( HANDLE hApp, char aszRes[][MAX_RESOURCE_LEN], 
                                                        unsigned long dwResCnt, const PTJid ptBareJid, 
                                                        EPresence aPres[], EPresence * pHighestPres );

    /********************************************************/
    // �������ƣ�IfHasFriend
    // ���ܣ�    roster���Ƿ��к���
    /********************************************************/
    LIBJINGLE_API BOOL IfHasFriend( HANDLE hApp, const PTJid ptJid );

    
    /********************************************************/
    // �������ƣ�GetFriendAllInfo
    // ���ܣ�    ��ȡĳ���ѵ����в�ͬresouce���Ͷ�Ӧ������
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


    // �ַ���jid�ͽṹ��jidת��
    LIBJINGLE_API ELibjingleError StrJid2TJid( const char * szJid, LibJingleEx::TJid & tJid  );
    LIBJINGLE_API ELibjingleError TJid2StrJid( const LibJingleEx::TJid & tJid, char * szJid, unsigned int dwSize );

   
    // ��ȡ�������еĳ�Ա(���Լ���)
    // pdwStartIndex   IN OUT
    //                 ���룺 Ϊ����Ŀ�ʼ��ַ
    //                 ����� Ϊ�´�����Ŀ�ʼ��ַ
    LIBJINGLE_API ELibjingleError GetAllRoomMembers( HROOM hRoom, PTJid ptMembers, unsigned long * pdwSize, 
                                                     unsigned long * pdwStartIndex = 0 );

    /************************************* debug use **************************************************/

    // xmpp��Ϣ
    typedef void (* LIBJINGLE_CALLBACK POnXmppMessage)( const char * pbyData, unsigned long dwSize,
                                                        BOOL bInput );
    // �ڲ�����־
    typedef void (* LIBJINGLE_CALLBACK POnLogInfo)( const char * szLogMessage );

    // xmpp��Ϣ���ڲ���־�����ڵ��Է�Χ
    // ���õ����õĻص�
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
    // ��ӡ��ĳ��app��صĵ�����Ϣ������ɴ�������pOnLogMessage���
    LIBJINGLE_API void PrintDebugInfo( HANDLE hApp, unsigned long dwSwitch = -1 );

    LIBJINGLE_API void PrintAllApps( );

    // ��������� Xmpp stanza����Ҫ�ȵ�¼
    LIBJINGLE_API ELibjingleError SendXmppMessage( HANDLE hApp, const char * szMessage );

    /********************************** end debug use **************************************************/




    /***************************** �ļ����� **************************/

    
    /********************************************************/
    // �������ƣ�CreateSession
    // ���ܣ�    ����һ��session���������ļ�
    // ������    hApp��֮ǰ������app���
    //           pSessioin�����������Session���
    //           ptJid��Ŀ��jid( full jid, resouce����Ϊ�� )
    //           pManifest��Ҫ���͵��ļ����ļ���
    //           szLocalDir������Ĭ�ϵ�·�������������ļ�ʱ�Ĳο�·��
    // ˵���� session����ʱ�Զ�����֮ǰ������ڴ棬����û��destroysession�ӿ�
    /********************************************************/
    LIBJINGLE_API ELibjingleError  CreateSession( HANDLE hApp,        HSession * pSession, 
                                                  const PTJid ptJid,  const FileShareManifest * pManifest,
                                                  const char * szLocalDir = 0 );

    /********************************************************/
    // �������ƣ�AcceptSession
    // ���ܣ�    �����ļ���������
    /********************************************************/
    LIBJINGLE_API ELibjingleError  AcceptSession( HSession hSession, const char * szSavePath = 0 );


    /********************************************************/
    // �������ƣ�DeclineSession
    // ���ܣ�    �ܾ������ļ���������
    /********************************************************/
    LIBJINGLE_API ELibjingleError  DeclineSession( HSession hSession );


    /********************************************************/
    // �������ƣ�CancelSession
    // ���ܣ�    �ж��ļ�����
    /********************************************************/
    LIBJINGLE_API ELibjingleError  CancelSession( HSession hSession );


    /********************************************************/
    // �������ƣ�SetFileSavePath
    // ���ܣ�    �����ļ�����ı���·��
    /********************************************************/
    LIBJINGLE_API ELibjingleError  SetFileSavePath( HANDLE hApp, const char * szFilePath );


    /********************************************************/
    // �������ƣ�SetFileShareStreamBitRate
    // ���ܣ�    �����ļ�����ı���·��
    /********************************************************/
    LIBJINGLE_API void SetFileShareStreamBitRate( unsigned long dwKbps );




    LIBJINGLE_API unsigned long GetDirSize( const char * szDirectory);

    LIBJINGLE_API unsigned long GetFileSize( const char * szFile );

    /***************************** �ļ�������� **************************/


    /****************** VCARD *************/
    // ��ptJidΪ0ʱ����ȡ�Լ���vcard������Ϊ0ʱ����ȡ�����˵�vcard
    LIBJINGLE_API   ELibjingleError  GetVCard( HANDLE hApp, const TJid * ptJid = 0 );
    // �����Լ���vcard
    LIBJINGLE_API   ELibjingleError  SetVCard( HANDLE hApp, const TVCard * ptVCard );
    /***************** END **************/


    /************** �������� *************/
    LIBJINGLE_API   ELibjingleError  SetPassword( HANDLE hApp, const char * szPassword = 0 );
    /*************************************/




    const unsigned int MAX_DMS_SUB_GROUPS_IN_A_GROUP           =  128;
    const unsigned int MAX_DMS_MEMBERS_IN_A_GROUP              =  128;
    // const unsigned int MAX_DMS_TOTAL_MEMBERS_COUNT             =  1024;
    const unsigned int MAX_DMS_TOTAL_MEMBERS_COUNT             =  256;


    // ������ĸ�����idΪ0
    // ���id��1��ʼ
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
        DMS_MEMBER_TYPE_JID,        // jid����
        DMS_MEMBER_TYPE_PHONE,		// �绰����	
        DMS_MEMBER_TYPE_FXO	        // FXO�绰����	
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
    // ע�⣺ֻ���޸����ƣ������޸�jid������
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


    const unsigned int MAX_DMS_MEMBER_STATE_CNT = 6;   // ���磺 ��tl, tp, td �� ts4���ն�����

    // dms��Ա��ÿ���ն��ϵ�״̬
    typedef struct tagDmsMemberState
    {
        char               szJid[MAX_JID_LEN];
        EPresence          eMaxPres;                          // �ڶദ�ն��ϵ����״̬
        EPresence          ePres[MAX_DMS_MEMBER_STATE_CNT];   // (�����������Ӧ��)���ն��ϵ�״̬

        BOOL               bMaxAudio;                         // ������Ƶ������������GK
        BOOL               bMaxVideo;                         // ������Ƶ������������GK

        BOOL               bAudio[MAX_DMS_MEMBER_STATE_CNT];   // ��Ƶ����(Ӳ��)
        BOOL               bVideo[MAX_DMS_MEMBER_STATE_CNT];   // ��Ƶ����(Ӳ��)
        BOOL               bCall[MAX_DMS_MEMBER_STATE_CNT];    // ��������(����GK��)

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

    // ����aszResΪ��ѯ��Щresource״̬, dwResCntΪ����aszRes��Ԫ�ظ���
    LIBJINGLE_API ELibjingleError DmsGetPresenceByGroup( HANDLE hApp, 
                                                         const char aszRes[][MAX_RESOURCE_LEN], unsigned long dwResCnt,
                                                         PTDmsMemberState  ptMemberState,       unsigned long * pdwSize, 
                                                         unsigned long dwGroupId, 
                                                         unsigned long * pdwStartIndex = 0 );

    // ˵����aPres����Ԫ�ظ��������aszRes��ͬ����dwResCnt
    LIBJINGLE_API ELibjingleError  GetMemberCapabilities( HANDLE hApp, char aszRes[][MAX_RESOURCE_LEN], 
                                                          unsigned long dwResCnt, const PTJid ptBareJid, 
                                                          PTDmsMemberState ptMemberState );


    LIBJINGLE_API  ELibjingleError  DmsQueryMember (  HANDLE hApp, unsigned long dwMemberId, PTDmsMember ptDmsMember );



    /*****************************************************************************/
    // ����: ���ֽ���ת����base64��ʽ               
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

    // ����
    LIBJINGLE_API  ELibjingleError  CreatePubsubNode( HANDLE hApp, const TPubsubParam * ptParam, 
                                                      unsigned long dwContextId );

    // ����
    LIBJINGLE_API  ELibjingleError  SubscribePubsubNode( HANDLE hApp, const char * szPubsubNodeName,
                                                         unsigned long dwContextId );

    // ɾ��
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
        int     nMsgType;                                      // ����
        BOOL    bOffline;                                      // �ɷ�Ϊ������Ϣ

        int     nParams[MAX_CUSTOM_PARAMS_CNT];                // int ����
        char    achBuf[MAX_CUSTOM_PARAM_BUF_LEN];              // buffer ����

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
