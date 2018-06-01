#ifndef _LIB_JINGLE_INNER_HEADER_
#define _LIB_JINGLE_INNER_HEADER_

#include <vector>
#include <list>
#include <map>
#include <algorithm>

#ifdef SUPPORT_ICONV
#ifndef _ANDROID_
#include <iconv.h>
#else
#include "iconv.h"
#endif
#endif

#include "talk/base/sigslot.h"
#ifdef WIN32
#include "talk/base/win32.h"
#endif

#include "talk/session/fileshare/fileshare.h"
#include "talk/p2p/client/httpportallocator.h"
#include "talk/base/stream.h"

#include "libjingle/libjingle.h"




#define  ROSTER_SWITCH                       1

#define  ROSTER_VERSION_MAX_LENGTH           64
#define  ROSTER_DATA_FILE_NAME               "roster.xml"
#define  ROSTER_CUSTOM_ATTR                  1                            // roster ��ϵ������bitrate��type����
// #define  DMS_CONTACTS_SWITCH                 1                            // ʹ����ǰ��dms API�����Ժ��roster API

#if defined(WIN32)
#define  DMS_CONTACTS_SWITCH                 0
#else
#define  DMS_CONTACTS_SWITCH                 0
#endif

#define  RESERVERD_BITRATE_INDEX             1                            // ��ǰdms����bitrate��reserverd�������

#define  CHATROOM_SUPPORT_MEMBER_AFFILIATION  0                           // �°汾֧��member��ȡ������roster���Լ�member�߳��Լ�


#define  GENERATE_NUMERIC_VERSION(a,b,c,d)   ((((char)(a))<<24)|(((char)(b))<<16)|(((char)(c))<<8)|((char)(d)))
#define  TERMINAL_VERSION                    GENERATE_NUMERIC_VERSION(2,5,2,0)


#define  SINCE_STAMP                         "1970-01-01T00:00:00Z"

#define  LOGIN_AFTER_TIME_DIFF               5000                         // ��Ϊ�յ�5����״̬�ı���ˣ��Ǻ��¼��


#define  OPERATION_TYPE_SYNC_CHATROOM        "sync chatroom"

#define  SYNC_CHATROOM_FLAG                  0


// ��λ������
unsigned long  GetCurTick();



// ����洢��ϵ�˵�˽�����ݿ����
#define  DMS_MEMBERS_BLOCK_CNT         10

// �������μӵ������Ҹ���( 1��Ԥ������Ƶ���飬 63���������������ң����ص�״̬�����Ҳ����� )
// ������������Ҫ����max_conferences_cnt = MAX_PERSISTENT_CHATROOM_CNT + MAX_TEMP_CHATROOM_CNT + 1( ��Ƶ���� ) + 1 ( ���ص�״̬���� )
#define  MAX_PERSISTENT_CHATROOM_CNT              32
#define  MAX_TEMP_CHATROOM_CNT                    32

#define  IGNORE_PRESENCE_PREFIX                   "wb_"

#define  IM_DEBUG                                 1

#define  SEND_PICTURE_TIMEOUT                    40000


extern LibJingleEx::TLibJingleEvent  g_tEvents;
extern LibJingleEx::POnMsg           g_pfnOnMessage;

#ifndef WIN32
typedef unsigned short WCHAR;
typedef unsigned long  DWORD;
#endif


#ifdef WIN32
typedef CRITICAL_SECTION JLockType;
#else
typedef pthread_mutex_t  JLockType;
#endif

void JInitLock  ( JLockType * pLock );
void JDeinitLock( JLockType * pLock );
void JLock( JLockType * pLock );
void JUnlock( JLockType * pLock );



char *   strtrim( char * szStr );
char *  _Str2Lower( char * szStr );

char    _TOLOWER( char ch );
int     _STRICMP( const char * s1, const char * s2 );



unsigned long           GetPresenceWeight( LibJingleEx::EPresence e );
LibJingleEx::EPresence  GetPresenceFromWeight( unsigned long w );


/***********************************************************/
// ��������һ��jid��Ӧ�������ļ���
// XMPP������jidΪnode����֯�����Ľṹ
//  jid1
//  |   + --- group1
//  |   |
//  |   + --- group2
//  |
//  jid2
//      + --- group3
//      |
//      + --- group4
//  
typedef std::vector<const char *> vCStrings;

typedef struct tagMemberStatus
{
    char                    m_szResource[LibJingleEx::MAX_RESOURCE_LEN];
    LibJingleEx::EPresence  m_ePresence;
    BOOL                    m_bFileShareAvailable;
    BOOL                    m_bAudio;
    BOOL                    m_bVideo;
    BOOL                    m_bCall;

    BOOL                    m_bFromStatusRoom;                             // �Ƿ�����״̬������
    // һ��״̬����Ϊ������״̬�����ҵģ��Ժ��״̬����ֻ����״̬���������͸ı�
    // (�������ܺ���״̬����)
    DWORD                   m_dwLastTick;
    DWORD                   m_dwVersion;

    tagMemberStatus()
    {
        memset( m_szResource, 0, sizeof(m_szResource) );
        m_ePresence = LibJingleEx::PRESENCE_NONE;
        m_bFileShareAvailable = FALSE;
        m_bAudio = FALSE;
        m_bVideo = FALSE;
        m_bCall  = FALSE;

        m_bFromStatusRoom = FALSE;
        m_dwLastTick = 0;
        m_dwVersion  = 0;
    }
}TMemberStatus, *PTMemberStatus;

typedef std::vector<PTMemberStatus> vMemberStatus;


#define  MAX_TURE_SOCIAL_NAME_LEN      16

typedef struct tagTMemberGroup
{
    LibJingleEx::TJid         m_tJid;
    LibJingleEx::TMemberItem  m_tData;
    vMemberStatus             m_Status;
    vCStrings                 m_Groups;
    char                      m_szTrueSocialName[MAX_TURE_SOCIAL_NAME_LEN];

    LibJingleEx::ECharStatus  m_eFromCharStatus;
    LibJingleEx::ECharStatus  m_eToCharStatus;

    tagTMemberGroup()
    {
        m_eFromCharStatus = LibJingleEx::CHAR_STATUS_ACTIVE;
        m_eToCharStatus   = LibJingleEx::CHAR_STATUS_ACTIVE;
        memset( m_szTrueSocialName, 0, sizeof(m_szTrueSocialName) );
    }

    ~tagTMemberGroup()
    {
        vMemberStatus::iterator it;
        for ( it = m_Status.begin(); it != m_Status.end(); it++ )
        {
            delete *it;
        }
        m_Status.clear();
    }
}TMemberGroup, *PTMemberGroup;

typedef std::vector<PTMemberGroup> vMemberGroups;
// ���϶�����������Ľṹͼ
/***********************************************************/



// �ṩ�̰߳�ȫ����
class cFuncLock
{
public:
    cFuncLock( JLockType * pLock )
    {
        m_pLock = pLock;
        JLock( m_pLock );
    }

    ~cFuncLock()
    {
        JUnlock( m_pLock );
    }

private:
    JLockType * m_pLock;
};

#define MAX_BUFF_SIZE        8192

/********************************************************************************/
// �ַ�ת������
// �ַ�ת�� UTF-8 ---> ANSI
char * Utf8ToAnsi( const char * szUtf8, char * ansi, unsigned int dwSize );


// �ַ�ת�� ANSI ---> UTF-8
char * AnsiToUtf8( const char * szAnsi, char * utf8, unsigned int dwSize );

/********************************************************************************/

// ˵������ʱ���̲߳���ȫ��
char * GetJid( const LibJingleEx::TJid & tJid );


char * GetBareJid( char * szBareJid, unsigned long dwSize, const char * szFullJid );


#define MAX_RAND_ID_LENGTH             32

// ���������Id(����ȷ��Ψһ�ԣ����ǲ�����ͬid�ĸ��ʺ�С)
char * GenerateRandomId( char * buff, unsigned int dwSize, const char * prefix = "green" );


#define SAFE_DELETE_PTR(p)   do { delete p;   p=0; } while ( 0 )
#define SAFE_DELETE_ARR(p)   do { delete[] p; p=0; } while ( 0 )

void OnDebugInfo( const char * szFormat, ... );

#ifdef    NO_LOG_MODULE

#ifdef WIN32
#define MYLOG(f,...)      
#define MYLOG_(f,...) 
#else
#define MYLOG(f...)     
#define MYLOG_(f...)
#endif

#else // NO_LOG_MODULE

#ifdef WIN32
#define MYLOG(f,...)           OnDebugInfo( f, __VA_ARGS__ )
#define MYLOG_(f,...)          OnDebugInfo( f, __VA_ARGS__ )
#else
#define MYLOG(f...)            OnDebugInfo( f )
#define MYLOG_(f...)           OnDebugInfo( f )
#endif

#endif // end NO_LOG_MODULE

BOOL IfJidEqual( const LibJingleEx::TJid & t1, const LibJingleEx::TJid & t2 );
BOOL IfBareJidEqual( const LibJingleEx::TJid & t1, const LibJingleEx::TJid & t2 );

extern void ClearFirstGroup(LibJingleEx::PTGroup & pFirstGroup);
extern void ClearMemberGroups( vMemberGroups & v );

// ����jid��MemberGroups����MemberGroup
class FindMemberGroupObj
{
public:
    FindMemberGroupObj( const LibJingleEx::PTJid ptJid );
    bool operator()( const PTMemberGroup p );
private:
    LibJingleEx::TJid   m_tJid;
};

// ����������MemberGroup->m_Groups������
class FindStringObj
{
public:
    FindStringObj( const char * s );
    bool operator()( const char * s );
private:
    const char * m_str;
};


BOOL IfMemberEqual( const LibJingleEx::PTMember p1, const LibJingleEx::PTMember p2 );




/* ====================  Ⱥ�� =========================*/

#define MAX_UNIQUE_ROOM_ID_LENGTH     LibJingleEx::MAX_NODE_LEN

// ��������ȫ�ֵ�Ψһ����id
typedef char UniqueRoomId[MAX_UNIQUE_ROOM_ID_LENGTH];

typedef struct tagRoomMember
{
    LibJingleEx::TJid                m_tJid;                     // �����Ա��jid
    char                             m_szNick[LibJingleEx::MAX_RESOURCE_LEN];  // �ڷ����е��ǳ�
    LibJingleEx::EAffiliation        m_eAlliliation;             // �����Ա��Ȩ��
    BOOL                             m_bSelf;                    // �Ƿ��Լ�
    LibJingleEx::EPresence           m_ePres;                    // ״̬

    tagRoomMember()
    {
        memset( m_szNick, 0, sizeof(m_szNick) );
        m_eAlliliation = LibJingleEx::AFFILIATION_NONE;
        m_bSelf = FALSE;
        m_ePres = LibJingleEx::PRESENCE_NONE;
    }
}TRoomMember, *PTRoomMember;

typedef std::vector<PTRoomMember> vRoomMember;

class FindRoomMemberObj
{
public:
    FindRoomMemberObj( const LibJingleEx::PTJid ptJid );
    bool operator ()(PTRoomMember pMember);

private:
    const LibJingleEx::PTJid m_ptJid;
};

class FindRoomMemberByBareJidObj
{
public:
    FindRoomMemberByBareJidObj( const LibJingleEx::PTJid ptBareJid );
    bool operator ()(PTRoomMember pMember);

private:
    LibJingleEx::TJid    m_tBareJid;
};

class FindRoomMemberByNickObj
{
public:
    FindRoomMemberByNickObj( const char * szNick );
    bool operator ()(PTRoomMember pMember);

private:
    const char * m_szNick;
};

typedef std::vector<LibJingleEx::PTRoomRosterItem> vRoomRoster;


class FindRoomRosterItem
{
public:
    FindRoomRosterItem( const LibJingleEx::PTJid  ptBareJid );
    bool operator ()(LibJingleEx::PTRoomRosterItem pItem);

private:
    LibJingleEx::TJid     m_tJid;
};

class FindBareJidObj
{
public:
    FindBareJidObj( const LibJingleEx::PTJid  ptBareJid );
    bool operator ()(LibJingleEx::PTJid  ptJid);

private:
    LibJingleEx::TJid     m_tJid;
};

class FindFullJidObj
{
public:
    FindFullJidObj( const LibJingleEx::PTJid  ptJid );
    FindFullJidObj( const std::string & jid );
    bool operator ()(LibJingleEx::PTJid  ptJid);

private:
    LibJingleEx::TJid     m_tJid;
};


typedef struct tagDeclineItem
{
    char                szRoomId[MAX_UNIQUE_ROOM_ID_LENGTH];
    char                szService[LibJingleEx::MAX_SERVICE_NAME_LEN];
    LibJingleEx::TJid   tDeclineJid;
    tagDeclineItem()
    {
        memset( this, 0, sizeof(*this) );
    }
}TDeclineItem, *PTDeclineItem;


class FindDeclineItemObj
{
public:
    FindDeclineItemObj( PTDeclineItem pItem );
    bool operator() ( PTDeclineItem p );

private:
    TDeclineItem   m_tItem;
};



class CLibJinglexApp;

enum RoomState
{
    ROOM_STATE_UNVAILABLE = 0,          // ��ʼ״̬
    ROOM_STATE_ON_QUERY_ID,             // ��������Ψһid
    ROOM_STATE_ON_PRESENCE,             // ����ͨ��presence���󴴽�
    ROOM_STATE_NOTIFIED_CREATE,         // ֪ͨ���Ѿ�������Ϣ(���ڴ�����Ƶ����ʱ)

    ROOM_STATE_ON_CONFIG,               // ���������޸�ȱʡ����
    ROOM_STATE_AVAILABLE,               // ����״̬
    ROOM_STATE_ON_INVITING,             // ����������
    ROOM_STATE_ON_ACCEPTING,            // ���ڽ�������
    ROOM_STATE_ENTERING_PERSISTENT,     // ���ڽ���־��Է���
    
    ROOM_STATE_HOLD_PERSISTENT,         // ��¼�ɹ��󣬲��������������飬������Ȩ�����ϲ�

    ROOM_STATE_TRYING_PERSISTENT,        // ��ͼ����̶�Ⱥ(�ǹ̶�Ⱥ)
};

const char * GetRoomState( RoomState e );

#define MAX_GROUPCHAT_SERVICE_NAME_LEN             64

#define ROOM_TYPE_NORMAL                           0
#define ROOM_TYPE_VISUAL                           1
#define ROOM_TYPE_SPECIAL                          2

#define SPECIAL_ROOM_MAX_USERS_CNT                 10000

#define MAX_GROUPCHAT_MESSAGE_ID_LEN               32    

// �����ҵ���ʷ��¼�����20��
#define  MAX_MUC_HISTORY_STANZAS                     20

class  CHistoryRoom
{
public:
    CHistoryRoom()
    {
        memset( m_aszMessageIds, 0, sizeof(m_aszMessageIds) );
        m_dwBeginPos = 0;
        m_dwEndPos   = 0;    // END - POS == TOTAL COUNT
    }

    void AddMessageId( const char * szMessageId )
    {
        assert( m_dwEndPos <= MAX_MUC_HISTORY_STANZAS );

        strcpy( m_aszMessageIds[m_dwEndPos], szMessageId );
        m_dwEndPos++;

        // ��������������ޣ�����end posΪ0
        if ( m_dwEndPos > MAX_MUC_HISTORY_STANZAS )
        {
            m_dwEndPos = 0;
        }

        // ���begin == end���ƶ�begin pos
        if ( m_dwEndPos == m_dwBeginPos )
        {
            m_dwBeginPos++;

            // ���begin pos�������ޣ�����begin posΪ0
            if ( m_dwBeginPos > MAX_MUC_HISTORY_STANZAS )
            {
                m_dwBeginPos = 0;
            }
        }
    }

    unsigned long GetCount()
    {
        if ( m_dwEndPos < m_dwBeginPos )
        {
            return m_dwEndPos + MAX_MUC_HISTORY_STANZAS+1 - m_dwBeginPos;
        }
        else
        {
            return m_dwEndPos - m_dwBeginPos;
        }
    }

    // ���ش�begin pos���𣬵ڼ���λ��
    // -1Ϊû���ҵ�
    unsigned long FindIndex( const char * szMessageId )
    {
        unsigned long dwCnt = GetCount();

        unsigned long i;
        for ( i = 0; i < dwCnt; i++ )
        {
            unsigned long dwIndex = m_dwBeginPos + i;
            if ( dwIndex > MAX_MUC_HISTORY_STANZAS )
            {
                dwIndex -= (MAX_MUC_HISTORY_STANZAS + 1);
            }

            // �ҵ���
            if ( 0 == strcmp( m_aszMessageIds[dwIndex], szMessageId ) )
            {
                return i;
            }
        }

        // �Ҳ���
        return (unsigned long)-1;
    }

    // dwIndexΪ��begin pos����ĵڼ���λ��
    char * operator[] ( unsigned long dwIndex )
    {
        unsigned long dwCnt = GetCount();
        if ( dwIndex >= dwCnt )
        {
            return 0;
        }

        dwIndex += m_dwBeginPos;
        if ( dwIndex > MAX_MUC_HISTORY_STANZAS )
        {
            dwIndex -= (MAX_MUC_HISTORY_STANZAS + 1);
        }

        return m_aszMessageIds[dwIndex];
    }

    void Clear()
    {
        m_dwBeginPos = 0;
        m_dwEndPos   = 0;
    }

    // ���begin pos��λ��
    unsigned long GetBeginPos()
    {
        return 0;
    }

    // ���begin pos��λ��
    unsigned long GetEndPos()
    {
        if ( m_dwEndPos < m_dwBeginPos )
        {
            return m_dwEndPos + (MAX_MUC_HISTORY_STANZAS+1) - m_dwBeginPos;
        }
        else
        {
            return m_dwEndPos - m_dwBeginPos;
        }
    }

private:
    char            m_aszMessageIds[MAX_MUC_HISTORY_STANZAS+1][MAX_GROUPCHAT_MESSAGE_ID_LEN];
    unsigned long   m_dwBeginPos;
    unsigned long   m_dwEndPos;
};


// ԭ��Ŀ�����ڴ洢iq��id������Ҳ���ڴ洢message��id
typedef struct tagIqId
{
    unsigned long     dwType;
    char              szIqId[MAX_RAND_ID_LENGTH];
    unsigned long     dwContextId;
    unsigned long     dwSubType;
    void *            pRoom;
    unsigned long     dwTick;

    tagIqId()
    {
        memset( this, 0, sizeof(*this) );
    }
}TIqId, *PTIqId;

enum EIqType
{
    IQ_TYPE_PUBSUB          = 1,
    IQ_TYPE_PRIVATE,
    IQ_TYPE_PING,
    IQ_TYPE_ROSTER,
    IQ_TYPE_P2P_MESSAGE,
    IQ_TYPE_GROUPCHAT_MESSAGE,
};

enum EPubsubSubType
{
    PUBSUB_SUBTYPE_CREATE_NODE = 1,
    PUBSUB_SUBTYPE_DELETE_NODE,
    PUBSUB_SUBTYPE_SUBSCRIBE_NODE
};

enum RosterSubType
{
    ROSTER_GET   =  1
};


class cPicSession
{
public:
    cPicSession()
    {
        memset( m_szNick,    0,    sizeof(m_szNick) );
        memset( m_szSid,     0,    sizeof(m_szSid) );
        memset( m_szPicName, 0,    sizeof(m_szPicName) );
        memset( m_szTempPath, 0, sizeof(m_szTempPath) );

        m_dwPicSize = 0;
        m_dwWiteBytes = 0;
    }

    ~cPicSession()
    {
        m_stream.Close();
    }

public:
    char                   m_szNick[LibJingleEx::MAX_RESOURCE_LEN];                     // ����Ⱥ��ͼƬ�ĳ�Ա��Nick
    char                   m_szSid[LibJingleEx::MAX_SID_LEN];                           // sid����nick����Ψһȷ��session
    char                   m_szPicName[LibJingleEx::MAX_FILENAME_SIZE];                 // ͼƬ��
    char                   m_szTempPath[LibJingleEx::MAX_PATH_SIZE];
    unsigned long          m_dwPicSize;                                                 // ͼƬ��С
    unsigned long          m_dwWiteBytes;                                               // ��д���С(���ڼ������)
    talk_base::FileStream  m_stream;
};

class FindRoomPicSessionObj
{
public:
    FindRoomPicSessionObj( const char * szNick, const char * szSid );
    bool operator () ( cPicSession * p );

private:
    char          m_szNick[LibJingleEx::MAX_RESOURCE_LEN];
    char          m_szSid[MAX_RAND_ID_LENGTH];            
};



void ClearVector( vRoomMember & v );
void ClearVector( vRoomRoster & v );
void ClearVector( std::vector<LibJingleEx::PTJid> & v );
void ClearVector( std::vector< TIqId * > & v );
void ClearVector( std::vector< cPicSession * > & v );
void ClearVector( std::vector<PTDeclineItem> & v );




// ����ģ��vs2008�ɹ�
#ifdef WIN32 
// ���item��ָ���vector
//template <class T>
//void ClearVector( std::vector<T> & v )
//{
//    std::vector<T>::iterator it;
//    for ( it = v.begin(); it != v.end(); it++ )
//    {
//        delete *it;
//    }
//    v.clear();
//}
#endif


typedef  struct  tagCachedGroupchatMsg
{
    LibJingleEx::TJid              tMemberJid;
    BOOL                           bOffline;
    char                           szMsgId[MAX_RAND_ID_LENGTH];
    LibJingleEx::TDmsChatContent   tMsg;

    tagCachedGroupchatMsg()
    {
        memset( this, 0, sizeof(*this) );
    }
}TCachedGroupchatMsg, *PTCachedGroupchatMsg;


typedef  std::vector<PTCachedGroupchatMsg>   vCachedGrouchatMsgs;


void ClearVector( vCachedGrouchatMsgs & v );



typedef struct tagRoomChatMsgItem
{
    char              szMsgId[MAX_RAND_ID_LENGTH];
    unsigned long     dwContextId;
    unsigned long     dwTick;

    tagRoomChatMsgItem()
    {
        memset( this, 0, sizeof(*this) );
    }
}TRoomChatMsgItem, *PTRoomChatMsgItem;

void ClearVector( std::vector<PTRoomChatMsgItem> & v );


typedef struct tagTRoom
{
    CLibJinglexApp *                   m_pParent;                       // App���
    RoomState                          m_eState;                        // �μ�����ö��ֵ
    UniqueRoomId                       m_szRoomId;                      // ����id
    char                               m_szGroupChatServiceName[MAX_GROUPCHAT_SERVICE_NAME_LEN];// �������(conference.kedacom.com) ��ʱ����OnInvite�����
    char                               m_szQueryId[MAX_RAND_ID_LENGTH]; // �����������Ψһidʱ������id������Ψһid����ʱ����
    char                               m_szLastRosterQueryId[MAX_RAND_ID_LENGTH]; // ������������Ա�б�����һ���ѯ�����ò�ѯ��ʱ�����ϲ�֪ͨroster items

    LibJingleEx::EAffiliation          m_eAffiliation;                  // �Լ��ڷ����е�Ȩ��(OWNER,NONE);
    char                               m_szNick[LibJingleEx::MAX_RESOURCE_LEN]; // �Լ��ı���
    char                               m_szName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN];
    BOOL                               m_bPersistent;
    int                                m_nType;                        // ROOM_TYPE_NORMAL�� ��ͨ��ʱ����
    // ROOM_TYPE_VISUAL�� ��Ƶ������ʱ����
    BOOL                               m_bUnique;
    BOOL                               m_bNeverIn;                     // �����ٽ��룬������֪�´ε�¼���ٽ���־���������

    BOOL                               m_bNofityRoomCreated;           // ���߽����ϴμ�����Ĺ̶�Ⱥ�������Ƿ�֪ͨ�ϲ�̶�Ⱥ�����Ѿ�������
    vRoomMember                        m_vMembers;
    CHistoryRoom                       m_LatestMsgIds;                // ��¼���µ�20����Ϣid(ÿ������һ����Ϣʱ����)
    BOOL                               m_bNoOfflineMsg;               // �յ���������뷿��ʱ������ʾ������Ϣ
    BOOL                               m_bGetMembers;                 // �Ƿ��ѻ�ȡ��Ա�б�(����)
    BOOL                               m_hasGotMembers;               // �Ƿ��ѻ�ȡ��Ա�б�(�õ�)
    vRoomRoster                        m_vRoster;                     // �����Ա�б�

    vRoomRoster                        m_vTmpRoster;                  // �����Ա�б�(��ʱ�����ں;��е�roster�Ƚ�)

    BOOL                               m_bGetSubject;                 // �Ƿ�ȡ�÷����subject��subject��¼�˷�������id
    // (ֻ��ȡ��subject�󣬲�������roster items���õ�roster items֮�����ϱ����ϲ�Ӧ��)
    std::vector<LibJingleEx::PTJid>    m_vQueryQuitMembers;           // ��������������˳��ĳ�Ա��jid����bare jid
    char                               m_szLastSubjectDelayMsgId[MAX_RAND_ID_LENGTH];   // ���µ�delay subject������Ϣid

    std::vector< cPicSession * >       m_vPicSession;


#if  IM_DEBUG
    unsigned long                      m_dwDelayMsgCnt;               // ���������ҵ������������Ѷ���Ϣ
#endif

    BOOL                               m_bReceivedSelfPresence;       // �Ƿ��յ��Լ���presence

    BOOL                               m_bFirst;                      // �Ƿ��ǵ�һ�ν��룬����HOLDING_PERSISTENT״̬��������


    // BOOL                               m_bNotifyMsgReady;             // ֪ͨ�ϲ�������Ϣready flag
    vCachedGrouchatMsgs                m_vDelayedMsgs;                // �����delay������Ϣ(Ҳ�п���һЩ��delay��Ϣ)
    // char                               m_szLastDelayedMsgId[MAX_RAND_ID_LENGTH];   // ���һ�ε�delay��Ϣid�������ӳ���Ϣ����ʱ�������idû�䣬��Ϊ�����һ��delay��Ϣ

    BOOL                               m_bQuitOnSelfOwn;              // �����˳�����(�б�����ʱ�Ƿ�����߳��Լ�)

    std::vector<PTRoomChatMsgItem>     m_vSendMsgs;                   // ������Ƿ��ͳɹ�����Ϣ

    tagTRoom( CLibJinglexApp * pParent )
    {
        m_pParent = pParent;
        m_eState  = ROOM_STATE_UNVAILABLE;

        memset( m_szRoomId, 0, sizeof(m_szRoomId) );
        memset( m_szQueryId, 0, sizeof(m_szQueryId) );
        memset( m_szLastRosterQueryId, 0, sizeof(m_szLastRosterQueryId) );

        memset( m_szGroupChatServiceName, 0, sizeof(m_szGroupChatServiceName) );
        m_eAffiliation = LibJingleEx::AFFILIATION_NONE;
        memset( m_szNick, 0, sizeof(m_szNick) );
        memset( m_szName, 0, sizeof(m_szName) );
        m_bPersistent = FALSE;
        m_nType       = ROOM_TYPE_NORMAL;
        m_bUnique     = FALSE;
        m_bNeverIn    = FALSE;
        m_bNofityRoomCreated = FALSE;
        m_bNoOfflineMsg = FALSE;
        m_bGetMembers   = FALSE;
        m_bGetSubject   = FALSE;

        memset( m_szLastSubjectDelayMsgId, 0, sizeof(m_szLastSubjectDelayMsgId) );

#if  IM_DEBUG
        m_dwDelayMsgCnt = 0;
#endif
        m_bReceivedSelfPresence = FALSE;
        m_bFirst                = FALSE;

        // m_bNotifyMsgReady       = FALSE;
        // memset( m_szLastDelayedMsgId, 0, sizeof(m_szLastDelayedMsgId) );

        m_bQuitOnSelfOwn = FALSE;
        m_hasGotMembers  = FALSE;
    }

    ~tagTRoom()
    {
        ClearVector( m_vMembers );
        ClearVector( m_vRoster );
        ClearVector( m_vQueryQuitMembers );
        ClearVector( m_vPicSession );
        ClearVector( m_vDelayedMsgs );
        ClearVector( m_vTmpRoster );
        ClearVector( m_vSendMsgs );
    }

    BOOL  EraseRosterItem( LibJingleEx::PTJid  ptBareJid, LibJingleEx::PTRoomRosterItem pOutputItem = 0 )
    {
        vRoomRoster::iterator it = find_if( m_vRoster.begin(), m_vRoster.end(), FindRoomRosterItem( ptBareJid) );
        if ( it != m_vRoster.end() )
        {
            if ( pOutputItem )
            {
                memcpy( pOutputItem, *it, sizeof(LibJingleEx::TRoomRosterItem) );
            }

            delete *it;
            m_vRoster.erase( it );
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    BOOL  AddRosterItem( LibJingleEx::PTRoomRosterItem  ptItem )
    {
        vRoomRoster::iterator it = find_if( m_vRoster.begin(), m_vRoster.end(), FindRoomRosterItem( &ptItem->m_tJid ) );
        if ( it != m_vRoster.end() )
        {
            return FALSE;
        }
        else
        {
            LibJingleEx::PTRoomRosterItem  pNewItem = new LibJingleEx::TRoomRosterItem;
            memcpy( pNewItem, ptItem, sizeof(LibJingleEx::TRoomRosterItem) );
            pNewItem->m_tJid.sResource[0] = '\0';
            m_vRoster.push_back( pNewItem );
            return TRUE;
        }
    }

    BOOL  GetFirstOwner( LibJingleEx::PTJid  ptOwnerJid )
    {
        vRoomRoster::iterator it;
        for ( it = m_vRoster.begin(); it != m_vRoster.end(); it++ )
        {
            LibJingleEx::PTRoomRosterItem  pItem = *it;
            if ( pItem->m_eAlliliation == LibJingleEx::AFFILIATION_OWNER )
            {
                memcpy( ptOwnerJid, &pItem->m_tJid, sizeof(LibJingleEx::TJid) );
                return TRUE;
            }
        }

        return FALSE;
    }

    // �����˳��ĳ�Ա֮���ÿո�ֿ�
    void ParseSubject( const std::string & subject )
    {
        std::string::size_type pos_start = 0;
        std::string::size_type pos_end   = 0;
        std::string  sBareJid;

        pos_end = subject.find( ' ', pos_start );
        while( std::string::npos != pos_end )
        {
            sBareJid = subject.substr( pos_start, ( pos_end - pos_start ) );

            LibJingleEx::PTJid  ptJid = new LibJingleEx::TJid;
            LibJingleEx::StrJid2TJid( sBareJid.c_str(), *ptJid );
            m_vQueryQuitMembers.push_back( ptJid );

            pos_start = pos_end + 1;
            pos_end = subject.find( ' ', pos_start );
        }

        sBareJid = subject.substr( pos_start );
        if ( sBareJid.length() > 0 )
        {
            LibJingleEx::PTJid  ptJid = new LibJingleEx::TJid;
            LibJingleEx::StrJid2TJid( sBareJid.c_str(), *ptJid );
            m_vQueryQuitMembers.push_back( ptJid );
        }
    }

    LibJingleEx::EPresence  GetMaxPresence( const LibJingleEx::PTJid  ptMemberJid )
    {
        LibJingleEx::EPresence  eMax = LibJingleEx::PRESENCE_NONE;
        unsigned long dwMaxPresWeight = GetPresenceWeight( eMax );

        vRoomMember::iterator it;
        for ( it = m_vMembers.begin(); it != m_vMembers.end(); it++ )
        {
            PTRoomMember ptMember = *it;

            if ( IfBareJidEqual( ptMember->m_tJid,  *ptMemberJid ) )
            {
                unsigned long dwPresWeight = GetPresenceWeight( ptMember->m_ePres );
                if ( dwPresWeight > dwMaxPresWeight )
                {
                    dwMaxPresWeight = dwPresWeight;
                }
            }
        }

        return GetPresenceFromWeight( dwMaxPresWeight );
    }

    void  SetMemberPresence( const LibJingleEx::PTJid  ptMemberJid,  LibJingleEx::EPresence ePres )
    {
        vRoomMember::iterator it;
        for ( it = m_vMembers.begin(); it != m_vMembers.end(); it++ )
        {
            PTRoomMember ptMember = *it;

            if ( IfJidEqual( ptMember->m_tJid,  *ptMemberJid ) )
            {
                ptMember->m_ePres = ePres;
            }
        }
    }

    bool AddIqId( const TIqId *  ptIqId )
    {
        std::vector<PTRoomChatMsgItem>::iterator it;
        for ( it = m_vSendMsgs.begin(); it != m_vSendMsgs.end(); it++ )
        {
            PTRoomChatMsgItem p = *it;
            if ( 0 == strcmp( p->szMsgId, ptIqId->szIqId ) )
            {
                return false;
            }
        }

        PTRoomChatMsgItem ptNewIqId = new TRoomChatMsgItem;
        strcpy( ptNewIqId->szMsgId, ptIqId->szIqId );
        ptNewIqId->dwContextId = ptIqId->dwContextId;
        ptNewIqId->dwTick = GetCurTick();

        m_vSendMsgs.push_back( ptNewIqId );
        return true;
    }

    bool IfExistIqId( const char * szIq, bool bDelete, PTRoomChatMsgItem ptItem = 0 )
    {
        std::vector< PTRoomChatMsgItem >::iterator it;
        for ( it = m_vSendMsgs.begin(); it != m_vSendMsgs.end(); it++ )
        {
            PTRoomChatMsgItem p = *it;
            if ( 0 == strcmp( p->szMsgId, szIq ) )
            {
                if ( bDelete )
                {
                    if ( ptItem )
                    {
                        memcpy( ptItem, p, sizeof(TRoomChatMsgItem) );
                    }
                    delete p;
                    m_vSendMsgs.erase( it );
                }
                return true;
            }
        }
        return false;
    }

}TRoom, *PTRoom;

typedef std::vector<PTRoom> vRoom;

extern void ClearRooms( vRoom & v );

class FindRoomByQueryId
{
public:
    FindRoomByQueryId( const char * szQueryId );
    bool operator ()(PTRoom pRoom);

private:
    const char * m_szQueryId;
};

class FindRoomByRoomId
{
public:
    FindRoomByRoomId( const char * szRoomId );
    bool operator ()(PTRoom pRoom);

private:
    const char * m_szRoomId;
};

class FindRoomByFullId
{
public:
    FindRoomByFullId( const char * szRoomId, const char * szServiceName );
    bool operator ()(PTRoom pRoom);

private:
    const char * m_szRoomId;
    const char * m_szServiceName;
};


/* ====================  END Ⱥ�� =========================*/



#define MAX_SESSION_ID_LEN  16
#define MAX_FULL_JID_LEN    128

/* ========================== �ļ����� =====================================*/

class FileShareClient : public sigslot::has_slots<>
{
public:
    FileShareClient( );
    ~FileShareClient();

    cricket::FileShareSession *  m_pSession;
    void *                       m_hApp;
    unsigned long                m_dwNo;
    size_t                       m_dwProgress;


    LibJingleEx::FileShareManifest   m_Manifest;
    LibJingleEx::TJid                m_PeerJid;
    char                             m_szLocalDir[LibJingleEx::MAX_PATH_SIZE];
    BOOL                             m_IsSender;
    BOOL                             m_bEof;
    talk_base::StreamInterface *     m_stream;

    char                             m_szSessionId[MAX_SESSION_ID_LEN];
    char                             m_szTempFilePath[LibJingleEx::MAX_PATH_SIZE];
    char                             m_szSaveAsName[256];


    // ��ͨ����������תʱ��¼��ǰ�ļ���
    char                     m_szTransferName[LibJingleEx::MAX_FILENAME_SIZE];
    StreamCounter*           m_counter;




    void OnSessionState(cricket::FileShareState state) ;
    void OnUpdateProgress(cricket::FileShareSession *sess) ;
    void OnUpdateProgress( unsigned long dwTotalSize, const char * szCurFileName ) ;
    void OnResampleImage(std::string path, int width, int height, talk_base::HttpTransaction *trans) ;

    // ����������ת�ļ�����ʱ����Ҫ��ȡ��ǰ�ļ����ƺʹ�����ֽ�
    void OnNextEntry(const std::string& name, size_t size);
    void OnUpdateBytes(size_t count);

    static unsigned long  s_dwNo;

    unsigned   long                     m_dwLastTimeTick;             // ��λ����
    unsigned   long                     m_dwLastSize;
    unsigned   long                     m_dwDeltaTick;
    unsigned   long                     m_dwDeltaSize;


    // ��ʱֱ�Ӵ�pumpɾ��ʱ��������post��pump��
    BOOL                                m_bPost2Pump;

    // ��ʱ�Է����ߣ��Լ�����cancel��ȴҪ֪ͨ�ϲ��ǶԷ�cancel
    BOOL                                m_bPeerOffline;
};

typedef std::vector< FileShareClient * > vFileShareClient;

/* ========================== �ļ��������  =====================================*/


typedef std::vector< LibJingleEx::PTDmsGroup >  vDmsGroup;
typedef std::vector< LibJingleEx::PTDmsMember > vDmsMember;



typedef  struct  tagPersistentRoom
{
    UniqueRoomId  m_szRoomId;                                               // ����id
    char          m_szGroupChatServiceName[MAX_GROUPCHAT_SERVICE_NAME_LEN]; // �������(conference.kedacom.com) 
    BOOL          m_bNotFound;                                              // �Ƿ��ǵ�¼���ֲ����ڵ�
    BOOL          m_bFirst;                                                 // �Ƿ��ǵ�һ�ν���(������web�����ָʾ��һ�ν���)

    tagPersistentRoom()
    {
        memset( this, 0, sizeof(*this) );
    }
}TPersistentRoom, *PTPersistentRoom;

typedef std::vector< PTPersistentRoom >   vPersistentRoom;

void ClearVector( vPersistentRoom & v );

class FindPersistentRoom
{
public:
    FindPersistentRoom( const char * szRoomId, const char * szMucServiceJid )
    {
        m_szRoomId       = szRoomId;
        m_szMucServiceId = szMucServiceJid;
    }

    bool operator () ( PTPersistentRoom p )
    {
        if (    0 == strcmp( p->m_szRoomId, m_szRoomId ) 
            && 0 == strcmp( p->m_szGroupChatServiceName, m_szMucServiceId )  )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    const char * m_szRoomId;
    const char * m_szMucServiceId;
};




class FindRoomIdObj
{
public:
    FindRoomIdObj( const char * szRoomId, const char * szMucServiceJid )
    {
        m_szRoomId       = szRoomId;
        m_szMucServiceId = szMucServiceJid;
    }

    bool operator () ( LibJingleEx::PTChatRoomId p )
    {
        if (    0 == strcmp( p->szRoomId, m_szRoomId ) 
            && 0 == strcmp( p->szService, m_szMucServiceId )  )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    const char * m_szRoomId;
    const char * m_szMucServiceId;
};

typedef std::vector<LibJingleEx::PTChatRoomId>   vChatRoomIds;
void ClearVector( vChatRoomIds & v );



class CRoomReadMsgs
{
public:
    CRoomReadMsgs( const char * szRoomId, const char * szService )
    {
        strcpy( m_szRoomId, szRoomId );
        strcpy( m_szGroupChatServiceName, szService );
    }

    ~CRoomReadMsgs()
    {

    }

public:
    UniqueRoomId                       m_szRoomId;                                              // ����id
    char                               m_szGroupChatServiceName[MAX_GROUPCHAT_SERVICE_NAME_LEN];// �������(conference.kedacom.com) ��ʱ����OnInvite�����
    CHistoryRoom                       m_room;
};


class FindHistoryRoom
{
public:
    FindHistoryRoom( const char * szRoomId, const char * szService )
    {
        m_szRoomId  = szRoomId;
        m_szService = szService;
    }

    bool operator() ( CRoomReadMsgs * p )
    {
        if ( 0 == strcmp(p->m_szRoomId, m_szRoomId) && 0 == strcmp(p->m_szGroupChatServiceName, m_szService) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    const char * m_szRoomId;
    const char * m_szService;
};

// ������ʱ�����յ�������������
typedef struct tagInvit2ChatRoomMsg
{
    LibJingleEx::TJid      m_tInviterJid;                     // �����ߵ�bare jid
    BOOL                   m_bPersistent;                     // �Ƿ�̶�Ⱥ
    int                    m_nType;                           // 
    char                   m_szName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN];       // ������
    UniqueRoomId           m_szRoomId;                                               // ����id
    char                   m_szGroupChatServiceName[MAX_GROUPCHAT_SERVICE_NAME_LEN]; // �������(conference.kedacom.com) 
    tagInvit2ChatRoomMsg()
    {
        memset( this, 0, sizeof(*this) );
    }
}TInvit2ChatRoomMsg, *PTInvit2ChatRoomMsg;


typedef struct tagQueryQuitRoomItem
{
    LibJingleEx::TJid     tMemberJid;
    UniqueRoomId          szRoomId;                                                     // ����id
    char                  szGroupChatServiceName[MAX_GROUPCHAT_SERVICE_NAME_LEN];       // �������(conference.kedacom.com)
    tagQueryQuitRoomItem()
    {
        memset( this, 0, sizeof(*this) );
    }
}TQueryQuitRoomItem, *PTQueryQuitRoomItem;


typedef  std::vector<char *>  vQueryIdsList;
void ClearVector( vQueryIdsList & v );

class FindQueryIdObj
{
public:
    FindQueryIdObj( const char * id )
    {
        m_szQueryId = id;
    }

    bool operator() ( const char * id )
    {
        if ( 0 == strcmp(m_szQueryId, id) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    const char *  m_szQueryId;
};


typedef struct  tagUnscribedContact
{
    LibJingleEx::TJid      tJid;
    DWORD                  dwFailCnt;                              // û�л��ping result�Ĵ���

    tagUnscribedContact()
    {
        dwFailCnt = 0;
    }
}TUnscribedContact, *PTUnscribedContact;

void ClearVector( std::vector<PTUnscribedContact> & v );


class FindUnscribedContactObj
{
public:
    FindUnscribedContactObj( const LibJingleEx::TJid * ptJid );
    bool operator() ( PTUnscribedContact p);


private:
    LibJingleEx::TJid      tJid;
};

class cKeyValue
{
public:
    std::string     sKey;
    std::string     sValue;
};


void  ClearList( std::list<cKeyValue *> & lst );

class  FindConfigObj
{
public:
    FindConfigObj( const char * szKey )
    {
        m_szKey = szKey;
    }

    bool operator() ( cKeyValue * pItem )
    {
        if ( 0 == _STRICMP( m_szKey, pItem->sKey.c_str() ) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    const char * m_szKey;
};


class  CCustomConfig
{
public:
    CCustomConfig(){}
    ~CCustomConfig(){ ClearList(m_lstConfigItems); }

    const char * operator [] ( const char * szKey ) const;
    void  SetConfig(const char * szKey, const char * szValue);

    const cKeyValue * GetFirstItem();
    const cKeyValue * GetNext();

private:
    std::list<cKeyValue *>     m_lstConfigItems;                           // �Զ���������
    std::list<cKeyValue *>::iterator m_it;
};


typedef struct tagSendRoomPictureParam
{
    PTRoom              m_pRoom;
    char                m_szPictureFilePath[LibJingleEx::MAX_PATH_SIZE];
    unsigned long       m_dwPictureSize;
    FILE *              m_fp;
    int                 m_state;
    unsigned long       m_dwSequence;
    char                m_szSid[MAX_RAND_ID_LENGTH];

    tagSendRoomPictureParam()
    {
        memset( this, 0, sizeof(*this) );
    }

    ~tagSendRoomPictureParam()
    {
        if ( m_fp )
        {
            fclose( m_fp );
            m_fp = 0;
        }
    }
}TSendRoomPictureParam, *PTSendRoomPictureParam;


void  ClearVector( std::vector<TSendRoomPictureParam *> & v );

     
/************************************************************/
// ��ΪҪ����ͬʱ��¼����û������԰�֮ǰ��ȫ��(��̬)����
// ��װ���������ĵ�ַ��ΪHandle
/************************************************************/
class XmppThread;

class CLibJinglexApp
{
public:
    CLibJinglexApp();
    ~CLibJinglexApp();

    // ����������߳̽���XMPP����
    XmppThread *                  m_pXmppThread;
    // ����Ϊ���ڵ�Ľṹ
    LibJingleEx::PTGroup          m_pFirstGroup;
    // ��jidΪ��ڵ�Ľṹ
    vMemberGroups                 m_MemberGroups;
    // �Լ�����ķ���
    vRoom                         m_Rooms;

    JLockType                     m_lock;

    JLockType                     m_external_lock;

    void *                        m_pUserData;

    char                          m_szStunAddr[LibJingleEx::MAX_ADDR_LEN];
    unsigned short                m_wStunPort;
    char                          m_reserverd0[2];
    char                          m_szRelayAddr[LibJingleEx::MAX_ADDR_LEN];

    char                          m_szSaveFileDir[LibJingleEx::MAX_PATH_SIZE];

    BOOL                          m_bFileShareAvailable;
    BOOL                          m_bNotifySecurity;           // �Ƿ���ʾ�û��������ط���¼��

    BOOL                          m_bAudio;                    // �Ƿ�����Ƶ��������
    BOOL                          m_bVideo;                    // �Ƿ�����Ƶ��������
    LibJingleEx::EPresence        m_ePres;
    BOOL                          m_bCall;


    vDmsGroup   m_vDmsGroups;
    vDmsMember  m_vDmsMembers[DMS_MEMBERS_BLOCK_CNT];

    unsigned  long                m_dwDmsMaxGroupId;
    unsigned  long                m_dwDmsMaxMemberId;

    BOOL                          m_bGetDmsGroupResult;
    BOOL                          m_bGetDmsMemberResult;

    BOOL                          m_bPersistentRoomsChanged;                          // �̶�Ⱥ�������Ƿ�ı�
    BOOL                          m_bPrivatePersistentRoomsCntConfirmed;              // ��¼���Ƿ��Ѿ�ȷ������Щ�̶�Ⱥ����
    vQueryIdsList                 m_vQueryPersistentRoomIds;

    BOOL                          m_bPersistentRoomsResponsed;                        // �Ƿ�ȡ�÷������Ļ�Ӧ


    std::vector<PTInvit2ChatRoomMsg>   m_vInviteMsgs;


    // ��¼�־���������
    vPersistentRoom               m_vPersistentRooms;

    std::vector< CRoomReadMsgs *> m_vRoomReadMsgs;
    BOOL                          m_bRoomReadMsgsChanged;                                          // �������Ѷ���Ϣ�Ƿ�ı�
    BOOL                          m_bGetRoomReadMsgs;                                              // �Ƿ�����ʷ�Ѷ���Ϣ
    buzz::XmlElement *            m_chatroom_history;                                              // Ⱥ����ʷ��¼
    unsigned long                 m_dwChatroomHistoryVersion;                                      // Ⱥ����ʷ�汾��

    // ��¼��ҵ����
    char                          m_szEnterpriseName[LibJingleEx::MAX_ENTERPRISE_NAME_LEN];
    char                          m_szGroupChatPicSaveDir[LibJingleEx::MAX_PATH_SIZE];              // ������ͼƬĬ�ϱ���·��


    std::vector<PTDeclineItem>    m_vDeclineItems;

    unsigned long                 m_dwPingTimeoutCnt;


    BOOL                          m_bTryEnterAllPersistentRooms;                                     // �Ƿ����ڽ������еĹ̶�Ⱥ
    vQueryIdsList                 m_vQueryAllPersistentRoomIds;
    std::vector<PTPersistentRoom> m_vTryingPersistents;


    BOOL                          m_bUpperAppReady;                                                  // �ϲ�Ӧ���Ƿ�׼����


    vChatRoomIds                  m_vScreenedRooms;                                                  // ������Ϣ��������

    std::vector<LibJingleEx::PTJid>   m_vQueryStatusContact;                                         // ���Լ�����״̬����ϵ��(bare jid)

    std::vector<PTUnscribedContact>   m_vUnscribedContacts;                                          // ���Լ�֪ͨ״̬����ϵ��


    CCustomConfig                     m_cConfig;                                                     // �Զ���������
    BOOL                              m_bConfigAvailable;                                            // �Ƿ��Ѿ��ӷ�������ȡ������

    BOOL                              m_bGetServerTime;                                              // �Ƿ��ȡ��������ʱ��
    DWORD                             m_dwTimeDiff;                                                  // �������ͱ���ʱ���ֵ
    DWORD                             m_dwQueryTick;                                                 // �����������ʱ�ı���tick
    char                              m_szQueryTimeId[MAX_RAND_ID_LENGTH];                           // �����������ʱ��iq id

    BOOL                              m_bSendStatus;

    char                              m_szConfigPath[LibJingleEx::MAX_PATH_SIZE];                   // ����д���õ�Ŀ¼
    buzz::XmlElement *                m_roster_root;                                                // ��roster�����ļ���ȡ������
    BOOL                              m_bRosterChanged;
    DWORD                             m_dwMaxGroupId;                                               // ������id
    DWORD                             m_dwMaxMemberId;                                              // ������Աid

    BOOL                              m_bGetRosterData;
    char                              m_szRosterVer[ROSTER_VERSION_MAX_LENGTH];

    DWORD                             m_dwLoginTick;


    std::vector<TSendRoomPictureParam *>   m_vSendRoomPictureParams;
    char                                   m_szLastSendPictureId[MAX_RAND_ID_LENGTH];
    unsigned long                          m_dwLastSendPictureTick;
    


    // ����jidɾ���������µ�����ͬjid�ĺ���
    void RemoveFriend( const LibJingleEx::PTJid ptJid );
    // ��ӡ������������
    void PrintDebugInfo( unsigned long dwSwitchs = -1 );

    void SavePrivateData( char * szLastQueryId, unsigned long dwSize );

    void Clear();

    void EraseHistoryRoom( const char * szRoomId, const char * szService )
    {
        std::vector< CRoomReadMsgs *>::iterator it_find = 
            find_if( m_vRoomReadMsgs.begin(), m_vRoomReadMsgs.end(), FindHistoryRoom(szRoomId, szService) );
        if ( it_find != m_vRoomReadMsgs.end() )
        {
            delete *it_find;
            m_vRoomReadMsgs.erase( it_find );
        }
    }

    BOOL IfExistRoom( PTRoom pRoom )
    {
        vRoom::iterator it;
        for ( it = m_Rooms.begin(); it != m_Rooms.end(); it++ )
        {
            PTRoom p = *it;
            if ( p == pRoom )
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    PTRoom  FindRoom( const LibJingleEx::TChatRoomId * pRoomId )
    {
        vRoom::iterator it;
        for ( it = m_Rooms.begin(); it != m_Rooms.end(); it++ )
        {
            PTRoom p = *it;

            if (   0 == strcmp(p->m_szGroupChatServiceName, pRoomId->szService) 
                && 0 == strcmp(p->m_szRoomId, pRoomId->szRoomId) )
            {
                return p;
            }
        }

        return 0;
    }

    BOOL IfExistFileShareSession( FileShareClient * pSession );


    // �����е�private block����һ�����ʵ�block���µ�dms ��Ա
    // �����ĸ�private block���˸ó�Ա
    unsigned long  AddDmsMember2Block(  LibJingleEx::PTDmsMember ptNewMember, unsigned long & dwBits );

    // �����е�private blocks���ҵ�ĳ��id�ĳ�Ա
    // ����ֵΪ0��ʾû���ҵ�
    LibJingleEx::PTDmsMember  GetDmsMember( unsigned long dwMemberId, unsigned long * pdwIndex = 0 );

    // ɾ��dms ��Ա
    // ����ֵ��ʾ�ĸ�˽�п�ɾ���˸ó�Ա��-1��ʾû�������Ա
    unsigned long DeleteDmsMember( unsigned long dwMemberId );

    // ��ȡ����jidΪxx��dms��Ա��id
    void  GetDmsMemberIdsByJid( const LibJingleEx::TJid * ptJid, std::vector<unsigned long> & vIds );


    // �޸�����jid��ptMember�е�jid��ͬ�ĳ�Ա( ��id��jid���ԣ���������Ϊһ�� )
    // ����ֵ��λ���أ� ��Щblock���޸�
    unsigned long ModifyDmsMemberByJid( LibJingleEx::PTDmsMember ptMember );

    // �Ƿ���ĳ��ָ�������ڴ���ָ��jid�ĳ�Ա
    BOOL IfExistDmsMemberInTheGroup( unsigned long dwGroupId, const char * szJid );

    // �Ƿ����ָ��jid�ĳ�Ա
    BOOL IfExistDmsMember(const char * szJid);

    // ���������� ROOM_TYPE_NORMAL, ROOM_TYPE_VISUAL
    // �� dwRoomType = ROOM_TYPE_NORMALʱ�� bPersistent����Ч
    unsigned long  GetChatRoomsCnt( unsigned long dwRoomType, BOOL bPersistent );

    // aszRes��ʾ����Щresource��Ӧ��״̬
    // ����ֵTRUE��ʾ�ҵ���ptDmsMemberState->szJid��������ϵ��
    // ˵����ptDmsMemberState->szJid����ص�resource�������������resource��Ӧ��״̬���������
    BOOL  GetDmsMemberState( const char aszRes[][LibJingleEx::MAX_RESOURCE_LEN], unsigned long dwResCnt,
        LibJingleEx::PTDmsMemberState  ptDmsMemberState );

    // �������������������Ϣ
    void  HandleDelayInvite2ChatRoomMsgs();

    void  NotifyHoldPersistentRoom();

    void  EnterEnterpriseStatusRoom();
    void  DenyEnterpriseStatusIn();

    void  ScreenChatroomMsgs();

    void  HandleScreenedChatroomCfg();

    BOOL  GetRosterData( char * szRosterVersion, DWORD dwRoseterVersionSize );

    void  SaveRoster2File();

    LibJingleEx::PTGroup AddGroup ( const char * szGroupName, BOOL * pbAdded );

    LibJingleEx::PTGroup FindGroup( unsigned long dwGroupId );

    unsigned long  GetFriendsCount();

    LibJingleEx::PTGroup FindGroupByName( const char * szName );

    BOOL  RemoveGroup( LibJingleEx::PTGroup pGroup );

    BOOL AddMember( LibJingleEx::PTGroup & pGroup, LibJingleEx::PTMember pMember, LibJingleEx::PTMember pOld = 0, 
                    BOOL * pbUiChanged = 0, BOOL * pbDmsAdded = 0, BOOL * pbDmsDeleted = 0 );

    LibJingleEx::PTMember FindMember( unsigned long dwMemberId, LibJingleEx::PTGroup & pGroup );

    // ֪ͨ�ϲ����Ⱥ��
    void  NotifyBufferedChatRoomMsgs( PTRoom pRoom );
    void  NotifyAllBufferedChatroomMsgs();

    // �ӱ��ض�ȡ��������ʷ��¼
    BOOL  GetChatRoomHistoryFromLocal( unsigned long & dwVersion );
    void  SaveChatRoomHistory2Local();
    void  ParseChatRoomHistory( const buzz::XmlElement * history );

    const char *  FindTrueSocialName( LibJingleEx::TJid *  ptJid );

    void  SendRoomPictureNextStep( const char * szGroupChatMessageId );
};


class FindDmsGroupObj
{
public:
    FindDmsGroupObj( unsigned long dwId ):m_dwId(dwId){}
    bool operator() ( LibJingleEx::PTDmsGroup pDmsGroup )
    {
        if ( pDmsGroup->dwGroupId == m_dwId )
        {
            return true;
        }
        return false;
    }
private:
    unsigned long     m_dwId;
};

class FindDmsMemberObj
{
public:
    FindDmsMemberObj( unsigned long dwId ):m_dwId(dwId){}
    bool operator() ( LibJingleEx::PTDmsMember pDmsMember )
    {
        if ( pDmsMember->dwMemberId == m_dwId )
        {
            return true;
        }
        return false;
    }
private:
    unsigned long     m_dwId;
};

typedef std::vector<CLibJinglexApp *> vApp;
extern  vApp g_Apps;

#define LOCK_INDEX_ANSI        0
#define LOCK_INDEX_UTF         1
#define LOCK_INDEX_JID         2

#define LOCK_INDEX_XML_PARSER  3
#define LOCK_INDEX_SSL         4
#define LOCK_INDEX_XML_PRINTER 5
#define LOCK_INDEX_NETWORKER_MANAGER     6
#define LOCK_INDEX_FILE_SHARE_SLEEP_TIME 7
#define LOCK_INDEX_APP_VECTOR            8

#define GLOBAL_LOCK_COUNT                9

// ����IOSϵͳ����ʱĿ¼
#define MAX_TMP_PATH_LEN        256


#define MAX_COMPILE_TIME_LEN    64

// ȫ�ֵ�Ψһʵ��
class CGlobalData
{
public:
    ~CGlobalData();
    static CGlobalData * GetInstance();

    // �������Ƿ��ʼ��
    BOOL      m_bInited;
    JLockType m_arrLocks[GLOBAL_LOCK_COUNT];

    char      m_szTmpPath[MAX_TMP_PATH_LEN];

    unsigned long   m_dwMaxTotalRate;
    unsigned long   m_dwFileSessionCnt;
    unsigned long   m_dwMaxDmsMembersPerBlock;     // ÿ��˽�����ݿ���������


    char            m_szCompileTime[MAX_COMPILE_TIME_LEN];


#ifdef SUPPORT_ICONV
    iconv_t         m_cd_ansi_to_utf8;
    iconv_t         m_cd_utf8_to_ansi;
#endif


    std::vector< CLibJinglexApp * >     m_vApps;

    long           m_nBias;                       // �룬��ʱ�����㣬ƫ�����


    void AddApps( CLibJinglexApp * app )
    {
        cFuncLock  cLock( &m_arrLocks[LOCK_INDEX_APP_VECTOR] );

        m_vApps.push_back( app );
    }

    void EraseApp( CLibJinglexApp * app )
    {
        cFuncLock  cLock( &m_arrLocks[LOCK_INDEX_APP_VECTOR] );

        std::vector< CLibJinglexApp * >::iterator it;
        for ( it = m_vApps.begin(); it != m_vApps.end(); it++ )
        {
            CLibJinglexApp * p = *it;
            if ( p == app )
            {
                m_vApps.erase( it );
                return;
            }
        }
    }

    BOOL IfExistApp( CLibJinglexApp * app )
    {
        cFuncLock  cLock( &m_arrLocks[LOCK_INDEX_APP_VECTOR] );

        std::vector< CLibJinglexApp * >::iterator it;
        for ( it = m_vApps.begin(); it != m_vApps.end(); it++ )
        {
            CLibJinglexApp * p = *it;
            if ( p == app )
            {
                return TRUE;
            }
        }

        return FALSE;
    }


    BOOL IfExistRoom( PTRoom  pRoom )
    {
        cFuncLock  cLock( &m_arrLocks[LOCK_INDEX_APP_VECTOR] );

        std::vector< CLibJinglexApp * >::iterator it;
        for ( it = m_vApps.begin(); it != m_vApps.end(); it++ )
        {
            CLibJinglexApp * p = *it;
            cFuncLock  cLock_app( &p->m_lock );

            if ( p->IfExistRoom(pRoom) )
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    PTRoom  FindRoom( CLibJinglexApp * app, const LibJingleEx::TChatRoomId * pRoomId )
    {
        cFuncLock  cLock( &m_arrLocks[LOCK_INDEX_APP_VECTOR] );

        std::vector< CLibJinglexApp * >::iterator it;
        for ( it = m_vApps.begin(); it != m_vApps.end(); it++ )
        {
            CLibJinglexApp * p = *it;

            if ( p == app )
            {
                cFuncLock  cLock_app( &p->m_lock );

                return p->FindRoom(pRoomId);
            }
        }

        return 0;
    }

    BOOL IfExistFileShareSession( FileShareClient * pSession )
    {
        cFuncLock  cLock( &m_arrLocks[LOCK_INDEX_APP_VECTOR] );

        std::vector< CLibJinglexApp * >::iterator it;
        for ( it = m_vApps.begin(); it != m_vApps.end(); it++ )
        {
            CLibJinglexApp * p = *it;
            cFuncLock  cLock_app( &p->m_lock );

            if ( p->IfExistFileShareSession(pSession) )
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    void PrintAllApps()
    {
        cFuncLock  cLock( &m_arrLocks[LOCK_INDEX_APP_VECTOR] );

        std::vector< CLibJinglexApp * >::iterator it;
        int i = 0;
        std::string s;
        s = "=========== ALL APPS ============= \r\n";
        for ( it = m_vApps.begin(), i = 0; it != m_vApps.end(); it++, i++ )
        {
            CLibJinglexApp * p = *it;

            char buf[256];
            sprintf( buf, "%d th app: 0x%p \r\n", i+1, p );
            s += buf;
        }
        if ( 0 == i )
        {
            s += "    no account handle \r\n";
        }
        s += "================================== \r\n";
        MYLOG( "%s", s.c_str() );
    }



    // unsigned long  m_dwFileShareSleepTime;  // ���������ļ���������� 

private:
    CGlobalData();    
    static CGlobalData * s_pInstance;
};

#ifdef WIN32
#define SNPRINTF(s,n,f,...)           _snprintf(s,n,f,__VA_ARGS__ )
#else
#define SNPRINTF(s,n,f...)             snprintf( s, n, f )
#endif

extern LibJingleEx::POnXmppMessage    g_pOnXmppMessage;
extern LibJingleEx::POnLogInfo        g_pOnLogMessage;


void FileShareManifestConvert( cricket::FileShareManifest & m,   const LibJingleEx::FileShareManifest & n );
void FileShareManifestConvert( LibJingleEx::FileShareManifest & m, const cricket::FileShareManifest & n);



enum EFileShareSubType
{
    FILE_SHARE_SUB_TYPE_START,
    FILE_SHARE_SUB_TYPE_END,
    FILE_SHARE_SUB_TYPE_CANCEL,
    FILE_SHARE_SUB_TYPE_FAIL,
    FILE_SHARE_SUB_TYPE_TRANSFER
};

EFileShareSubType GetFileShareSubType( const char * szSubType );

typedef struct tagFileShareMsg
{
    EFileShareSubType     eSubType;
    char                  szPeerJid[MAX_FULL_JID_LEN];
    char                  szSessionId[MAX_SESSION_ID_LEN];
    int                   nError;
    tagFileShareMsg()
    {
        memset( this, 0, sizeof(*this) );
    }
}TFileShareMsg, *PTFileShareMsg;


#define  MSG_STOP_FILE_SHARE                        3101
#define  MSG_P2P_FAIL                               3102
#define  MSG_NEXT_P2P_TRANSFER                      3103
#define  MSG_P2P_END                                3104

#define  MSG_SUBJECT_DELAY                          4000
#define  MSG_CHATROOM_MSGS_DELAY_CHECK              4001
#define  MSG_GET_CHATROOM_MEMBERS_DELAY             4002


#define  INTERVAL_SUBJECT_DELAY                     1000
#define  INTERVAL_CHATROOM_MSGS_CHECK_DELAY         1500
#define  INTERVAL_GET_CHATROOM_MEMBERS_DELAY        1500


#define  MAX_BYTE_PER_TRANSFER             (1024*16)        // ͨ����������ת�ļ���ÿ������ֽ���(����base64�����ͳ�����MAX_BYTE_PER_TRANSFER)

#define  MIN_BYTE_RATE                     ( 8 * 1024 ) // ��С�ļ���������



/*****************************************************************************/
// ����: ���ֽ���ת����base64��ʽ               
/*****************************************************************************/
int EncodeBase64( char * szBase64,              unsigned long & dwSize, 
                 const unsigned char * pbySrc, unsigned long   dwSrcLength );


/*****************************************************************************/
// ����: ��base64��ʽת�����ֽ���           
// ˵��: pdwSize���������������pbyDest�Ĵ�С
/*****************************************************************************/
int DecodeBase64( unsigned char * pbyDest, unsigned long * pdwSize, 
                 const char * szBase64 );


#define VCARD_KEYWORD_NAME_CUSTOM_PORTRAIT_FLAG  "custom portrait"
#define VCARD_KEYWORD_NAME_SYS_PORTRAIT_INDEX    "portrait index"


void  LibjingleLogError( const char * szFileName, unsigned long dwLine, const char * szFormat, ... );

#ifdef WIN32
#define LIBJINGLE_LOG_ERROR( f, ... ) LibjingleLogError(__FILE__, __LINE__, f,  __VA_ARGS__)
#else
#define LIBJINGLE_LOG_ERROR( f... ) LibjingleLogError(__FILE__, __LINE__, f )
#endif



#define IMAGE_TYPE_BITMAP_STR               "image/bmp"
#define IMAGE_TYPE_JPG_STR                  "image/jpg"
#define IMAGE_TYPE_GIF_STR                  "image/gif"
#define IMAGE_TYPE_PNG_STR                  "image/png"
#define IMAGE_TYPE_UNKNOWN_STR              "image/unknown"

#define  SPECIAL_GROUP_SN                     1
#define  SPECIAL_GROUP_NAME                   "δ������ϵ��"





#define  MAGIC_PREFIX                  "kedacom.com"
#define  MAGIC_PREFIX_LEN              11
#define  INVITE_PRESISTENT_BUF_SIZE    2
#define  INVITE_ROOMTYPE_BUF_SIZE      2


void     SavePersistentRoom( CLibJinglexApp * app );


BOOL     ParseXmlBuf( buzz::XmlElement * root, const char * szXmlBuf, unsigned long deSize );

void     PrintXmlElement( const buzz::XmlElement * root );


// ��ȡ�洢dms��Ա��˽�����ݿ������(�����ռ�)
// dwIndex ��0��ʼ ( ������1, ��2, ��3.... )
char *   GetDmsMembersBlockName( char * szBuf, unsigned long dwSize, unsigned long dwIndex );

// ���������������ռ��ȡindex
unsigned long GetDmsMembersBlockIndex( const char * szNamespace );

// ����dms group˽�����ݵ�������
void  SaveDmsGroup2Server ( CLibJinglexApp * pAppObj );
// ������Щdms member˽�����ݿ鵽������
// ����dwBlockBitsΪλ���أ����λ��־��һ��˽��block
void  SaveDmsMember2Server( CLibJinglexApp * pAppObj, unsigned long dwBlockBits );


/*******************************************************************************/
// ����: �鿴����(char|unsigned char|short|unsigned short|int|unsigned int��
//       �Ƿ�������ĳλ��λ��������0��ʼ��
// ����ֵ:                      // -1, ��������
//          1, ������
//          0, û������
/*******************************************************************************/
template <class T>
int IfHasBit(T num, unsigned char index)
{
    //if ( index >= sizeof(T) * 8 )
    //{
    //    return -1;
    //}

    if ( 0 == ( num & ((T)1<<index) ) )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/*******************************************************************************/
// ����ĳλ��ֵ                                                        
/*******************************************************************************/
template <class T>
void SetBit(T& num, unsigned char index, bool bSet = true)
{
    if ( index >= sizeof(T) * 8 )
    {
        return;
    }

    if ( bSet )
    {
        num |= (((T)0x01)<<(index));
    }
    else
    {
        num &= (~(((T)0x01)<<(index)));
    }
}





#define STRNCPY( dst, src, len ) \
    do{ strncpy(dst,src,(len) -1); dst[(len)-1] = '\0'; }while(0)



bool IsXmlSpace(int ch);

bool ListContainsToken(const std::string & list, const std::string & token);

bool IsIgnoredResource( const std::string & resource );

LibJingleEx::EAffiliation StrAffi2Int( const char * strAffi );
LibJingleEx::EPresence StrShow2Int( const char * show );


// ��bool��BOOL
#define  GET_BOOL( b )        ( (b) ? TRUE:FALSE )

void  GetRoomRoster( void * client, PTRoom pRoom );


// ����������Ա��
#define CHATROOM_MAX_MEMBERS_COUNT       192




const unsigned long MSG_LOGIN = 1;
const unsigned long MSG_DISCONNECT = 2;
const unsigned long MSG_AFTER_LOGIN = 3;
const unsigned long MSG_SHARE_FILES = 4;
const unsigned long MSG_ACCEPT_FILES = 5;
const unsigned long MSG_DECLINE_FILES = 6;
const unsigned long MSG_CANCEL_FILES = 7;
const unsigned long MSG_DISCONNECT2 = 8;
const unsigned long MSG_GET_VCARD = 9;
const unsigned long MSG_SEND_STANZA = 10;
const unsigned long MSG_SEND_RAW    = 11;
const unsigned long MSG_SEND_STANZA_EX = 12;
const unsigned long MSG_DELETE_ROOM_MEMORY = 13;
const unsigned long MSG_LOGOUT_TIMEOUT = 14;
const unsigned long MSG_PING_TIMEOUT = 15;
const unsigned long MSG_INNER_LOOP = 16;
const unsigned long MSG_UPPER_READY = 17;
const unsigned long MSG_NOTIFY_UNSCRIBED_STATUS = 18;

const unsigned long MSG_PING = 19;
const unsigned long MSG_SURE_SEND_STATUS = 20;
const unsigned long MSG_GET_ROOM_MEMBERS  = 21;
const unsigned long MSG_RE_ENTER_CHATROOM = 22;

const unsigned long MSG_SET_TRUE_SOCIAL_NAME = 23;
const unsigned long MSG_SEND_ROOM_PICTURE    = 24;


#define  LOGOUT_TIMEOUT          1000
#define  PING_TIMEOUT            30000
#define  INNER_LOOP_TIMEOUT      60000
#define  NOTIFY_UNSCRIBED_STATUS_INTERVAL  60000
#define  SURE_SEND_STATUS_DELAY  1000

#define  MAX_UNSCRIBED_CONTACT_PING_FAIL_COUNT   3



struct DeleteRoomMemoryMsg : public talk_base::MessageData
{
    DeleteRoomMemoryMsg( PTRoom  pRoom )
    {
        m_pRoom = pRoom;
    }

    ~DeleteRoomMemoryMsg()
    {
    }

    PTRoom     m_pRoom;
};


struct CommonChatroomMsg : public talk_base::MessageData
{
    CommonChatroomMsg( PTRoom  pRoom )
    {
        m_pRoom = pRoom;
    }

    ~CommonChatroomMsg()
    {
    }
    PTRoom     m_pRoom;
};


struct SubjectDelayMsg : public talk_base::MessageData
{
    SubjectDelayMsg()
    {
        memset( szRoomId, 0, sizeof(szRoomId) );
        memset( szGroupChatServiceName, 0, sizeof(szGroupChatServiceName) );
        memset( szSubjectMsgId, 0, sizeof(szSubjectMsgId) );
    }

    ~SubjectDelayMsg()
    {

    }

    UniqueRoomId             szRoomId;                                                   // ����id
    char                     szGroupChatServiceName[MAX_GROUPCHAT_SERVICE_NAME_LEN];     // service
    char                     szSubjectMsgId[MAX_RAND_ID_LENGTH];
};

struct PingTimeoutMsg : public talk_base::MessageData
{
    PingTimeoutMsg( const PTIqId  ptIqId )
    {
        memcpy( &m_tIqId, ptIqId,  sizeof(TIqId) );
    }

    ~PingTimeoutMsg()
    {

    }

    TIqId      m_tIqId;
};


struct ChatroomMsgsCheckMsg : public talk_base::MessageData
{
    ChatroomMsgsCheckMsg()
    {
        memset( szRoomId, 0, sizeof(szRoomId) );
        memset( szGroupChatServiceName, 0, sizeof(szGroupChatServiceName) );
        memset( szDelayedMsgId, 0, sizeof(szDelayedMsgId) );
    }

    ~ChatroomMsgsCheckMsg()
    {

    }

    UniqueRoomId             szRoomId;                                                   // ����id
    char                     szGroupChatServiceName[MAX_GROUPCHAT_SERVICE_NAME_LEN];     // service
    char                     szDelayedMsgId[MAX_RAND_ID_LENGTH];
};




struct ChatRoomMsg : public talk_base::MessageData
{
    ChatRoomMsg( )
    {
        memset( m_szRoomId,                 0,   sizeof(m_szRoomId) );
        memset( m_szGroupChatServiceName,   0,   sizeof(m_szGroupChatServiceName) );
        memset( m_szName,                   0,   sizeof(m_szName) );
    }

    ~ChatRoomMsg()
    {

    }

    UniqueRoomId             m_szRoomId;                                                   // ����id
    char                     m_szGroupChatServiceName[MAX_GROUPCHAT_SERVICE_NAME_LEN];     // service
    char                     m_szName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN];
    int                      m_nType;
    BOOL                     m_bPersistent;
};


struct  SetTrueSocialNameMsg : public talk_base::MessageData
{
    SetTrueSocialNameMsg( const char * szJid, const char * szName )
    {
        strncpy( m_szJid,            szJid,   LibJingleEx::MAX_JID_LEN );
        strncpy( m_szTrueSocialName, szName,  MAX_TURE_SOCIAL_NAME_LEN );
    }

    char                     m_szTrueSocialName[MAX_TURE_SOCIAL_NAME_LEN];
    char                     m_szJid[LibJingleEx::MAX_JID_LEN];
};


struct  SendRoomPictureMsg : public talk_base::MessageData
{
    SendRoomPictureMsg( PTRoom pRoom, const char * szPicFilePathName, unsigned long dwPicSize )
    {
        m_pRoom = pRoom;
        strncpy( m_szPicturePath, szPicFilePathName, sizeof(m_szPicturePath) );
        m_dwPictureSize = dwPicSize;
    }

    PTRoom                   m_pRoom;
    char                     m_szPicturePath[LibJingleEx::MAX_PATH_SIZE];
    unsigned long            m_dwPictureSize;
};






// ����ֵ��TRUE�� ������һ�������� FALSE�� û������
BOOL GetAbilityString( std::string & ability, CLibJinglexApp * app );

// ȡ��presence���е�show�ַ���
const char * GetPresShowString( LibJingleEx::EPresence e );


// �����·������ȡ�ļ���
char * GetFileName( char * szDest, unsigned long dwSize, const char * szFilePath );

// szResourceҲ�������ҳ�Ա�� nick name, �Ⱦ�����nick name��ȡ��Ա��jid��������У�ֱ�Ӱ�nick name����full jid 
void GetMemberJidFromChatRoomResource( PTRoom pRoom, const char * szResource, LibJingleEx::PTJid  ptMemberJid );

void DeleteChatroomMember( PTRoom pRoom,  LibJingleEx::PTJid  ptMemberJid, void * client );


#define DECLINE_PERSISTENT_ROOM "persistent chatroom count limited"
#define DECLINE_TEMPORARY_ROOM  "temp chatroom count limited"
#define DECLINE_FROM_WEIBO      "weibo declined"

#define MAX_PING_TIME_OUT_COUNT  3


#define CHAT_MESSAGE_VERSION    "1.1"


buzz::XmlElement *  ConstructChatBody( const LibJingleEx::TDmsChatContent * ptMsg, CLibJinglexApp * pAppObj, unsigned long * pdwSendTime );
void ParseChatMessage( const buzz::XmlElement * body, LibJingleEx::TDmsChatContent & tMsg );


buzz::XmlElement * BuildCostomMsgBody( const LibJingleEx::TCustomMsg * ptCustomeMsg );
void ParseCustomMsg( const buzz::XmlElement * body, LibJingleEx::TCustomMsg & tCustomMsg );


#define  MINIMIZATION( a, b )  ( (a) < (b) ? (a) : (b) )


#define  MIN_SAVE_PERSISTENT_CHATROOM_COUNT               32


#define  MIN_RELOGIN_DELAY_TIME                           5                // seconds
#define  MAX_RELOGIN_DELAY_TIME                           30               // seconds


#define  UPDATE_FILE_TRANSFER_PROGRESS_INTERVAL           1000


#define  SS5_PROXY_VERSION                                '\x05'
#define  SS5_PROXY_AUTH_METHOD_NO_AUTH                    '\x00'
#define  SS5_PROXY_AUTH_METHOD_USER_PWD                   '\x02'

#define  SS5_PROXY_CONNECT_CMD                            '\x01'
#define  SS5_PROXY_RSV                                    '\x00'
#define  SS5_PROXY_IPV4_TYPE                              '\x01'


unsigned char *   GetSs5ProxyUserPwdMethodQueryString( unsigned char * buf, unsigned long & dwSize, const char * szUser, const char * szPwd );

unsigned char *   GetSs5ProxyConnectQueryString( unsigned char * buf, unsigned long & dwSize, unsigned long dwDestIp, unsigned short int wDestPort );


unsigned long    GetUnavailablePersistentRoomCnt( vPersistentRoom & v );


const char *     GetCharStateString(LibJingleEx::ECharStatus e);
const char *     GetBoolString( BOOL b );



#define    DELAYED_TERMINATE_TIME                          1000


LibJingleEx::EPresence   GetMaxPresence( vMemberStatus & s, BOOL * pbMaxAudio = 0, BOOL * pbMaxVideo = 0 );

#define    INSTANT_MESSAGE_TYPE                            "headline"

#define    PREFIX_PING                                      "ping_"


char *  MyGetVersion( char * buf, unsigned long dwSize, unsigned long dwVersion );
unsigned long  ConvertVersion(const char * szVersion);

#define  JINGLE_MIN(a, b)                                   ((a)<(b)?(a):(b))






// ���豣���xml�����ļ����Ǽ򵥵�xml�﷨(ʹ�ÿհ������ռ䣬BodyText�����س�����, ��ֻ���ı���Ƕ����node��ϵ����)
// xmlԪ�ػ����Ե�name��valueֵ������̫��(ת��GBK���ܳ���8195�ֽ�)
buzz::XmlElement * ParseXmlFile( const char * szFilePath );
int                WriteXmlFile( buzz::XmlElement * rootElement, const char * szFilePath );

void StringReplace( std::string & strBase, const std::string & strSrc, const std::string & strDes );

const buzz::XmlElement * GetFirstNamedElementByLocalPart( const buzz::XmlElement * root, const char * szLocalPart );


#define  SEND_CHAT_MSG_FAILURE_TIMEOUT                                  ( INNER_LOOP_TIMEOUT - 1000 )   // ����� INNER_LOOP_TIMEOUT С


#if !DMS_CONTACTS_SWITCH
int  _DmsDelMember( CLibJinglexApp *  pAppObj,  unsigned long dwMemberId );
int  _DmsAddMember( CLibJinglexApp *  pAppObj,  LibJingleEx::PTDmsMember  ptDmsMemberSrc );
int  _DmsModifyMember( CLibJinglexApp *  pAppObj, LibJingleEx::PTDmsMember  ptDmsMember );
#endif


BOOL   IfMemberUiChanged( const LibJingleEx::TMember * pOld , const LibJingleEx::TMember * pNew );
BOOL   IfUnscribedContact( const LibJingleEx::TMember * pMember );


FILE * XmppOpenFile   ( const char * szFile, const char * szMode );


#endif


