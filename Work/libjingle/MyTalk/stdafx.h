// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include "libjingle/libjingle.h"

using namespace LibJingleEx;




#define MAX_ACCOUNT_NUMBER             6

#include <string>
#include <vector>
#include <algorithm>

#include "libjingle/libjingle.h"
#include "MyConfigFile.h"

extern LibJingleEx::TLoginSettings     g_tAccounts[MAX_ACCOUNT_NUMBER];
extern LibJingleEx::HANDLE             g_hXmppApp[MAX_ACCOUNT_NUMBER];

extern MyConfigFile                    g_Config;

#define CONFIG_FILE                    "account.txt"

extern std::string                     GetFullJid( const std::string & sNode, 
                                                   const std::string & sDomain,
                                                   const std::string & sResource );

extern std::string                     GetBareJid( const std::string & sNode, 
                                                  const std::string & sDomain );

#define CONFIG_USERNAME     "username"
#define CONFIG_DOMAIN       "domain"
#define CONFIG_PASSWORD     "password"
#define CONFIG_HOST         "host"
#define CONFIG_RESOURCE     "resource"
#define CONFIG_USE_TLS      "use tls"

#define CONFIG_STUN_ADDR    "stun addr"
#define CONFIG_RELAY_ADDR   "relay addr"
#define CONFIG_STUN_PORT    "stun port"
#define CONFIG_SAVE_PATH    "save path"

void OnXmppMessage( const char * pbyData, unsigned long dwSize, BOOL bInput );
void OnLogInfo( const char * szLogMessage );

void SetXmppConsole( HWND hWnd );



#define UM_XMPP_MSG       ( WM_USER + 100 )
#define UM_DEBUG_MSG       ( WM_USER + 101 )

#define UM_ON_SUBSCRIBE_CLOSE ( WM_USER + 102 )

#define UM_STATE          (WM_USER + 1001)
#define UM_ROSTER         (WM_USER + 1002)
#define UM_CHAT_MSG       (WM_USER + 1003)
#define UM_PRESENCE       (WM_USER + 1004)
#define UM_ROSTER_ADD     (WM_USER + 1005)
#define UM_ROSTER_DEL     (WM_USER + 1006)
#define UM_ROSTER_CHANGE  (WM_USER + 1007)
#define UM_ON_SUBSCRIBE   (WM_USER + 1008)
#define UM_CREATE_CHATROOM_SUCCESS       (WM_USER + 1009)
#define UM_ON_INVITE                     (WM_USER + 1010)
#define UM_CHATROOM_PRESENCE             (WM_USER + 1011)
#define UM_CHATROOM_MESSAGE              (WM_USER + 1012)
#define UM_ON_CHATROOM_DESTROY           (WM_USER + 1013)
#define UM_ON_CHATROOM_ROSTER_ADD        (WM_USER + 1014)
#define UM_ON_CHATROOM_ROSTER_DEL        (WM_USER + 1015)
#define UM_ON_NOTIFY_PERSISTENT_CHATROOM       (WM_USER + 1016)
#define UM_ON_NOTIFY_CHAR_STATE                (WM_USER + 1017)


#define UM_ON_FILE_OFFER           (WM_USER + 2001)
#define UM_ON_FILE_PROGRESS        (WM_USER + 2002)
#define UM_ON_FILE_COMPLETE        (WM_USER + 2003)
#define UM_ON_FILE_CANCEL          (WM_USER + 2004)
#define UM_ON_FILE_FAILURE         (WM_USER + 2005)
#define UM_ON_FILE_TRANSFER        (WM_USER + 2006)
//#define UM_STATE          (WM_USER + 1001)

#define UM_CONFERENCE_SERVER   (WM_USER + 1100)

#define UM_CLOSE_PROGRESS      (WM_USER + 3001)

#define UM_ON_VCARD_INFO       (WM_USER + 4001)

#define UM_ON_DMS_ADD_GROUP       (WM_USER + 5001)
#define UM_ON_DMS_DEL_GROUP       (WM_USER + 5002)
#define UM_ON_DMS_MODIFY_GROUP    (WM_USER + 5003)
#define UM_ON_DMS_ADD_MEMBER       (WM_USER + 5004)
#define UM_ON_DMS_DEL_MEMBER       (WM_USER + 5005)
#define UM_ON_DMS_MODIFY_MEMBER    (WM_USER + 5006)
#define UM_ON_DMS_MOVE_MEMBER      (WM_USER + 5007)

#define UM_ON_PICTURE              (WM_USER + 6000)

extern FILE *             g_fp;
extern CRITICAL_SECTION g_tLock;

extern BOOL _IfJidEqual( const LibJingleEx::TJid & t1, const LibJingleEx::TJid & t2 );

typedef struct tagOnSubscribeMsg
{
    LibJingleEx::HANDLE   m_hApp;
    LibJingleEx::PTJid    m_ptJid;

    tagOnSubscribeMsg()
    {
        m_hApp  = 0;
        m_ptJid = 0;
    }
}TOnSubscribeMsg, *PTOnSubscribeMsg;

typedef std::vector<PTOnSubscribeMsg> vOnSubscribeMsg;

typedef struct tagParticipant
{
    LibJingleEx::TJid          m_tJid;
    LibJingleEx::EPresence     m_ePres;
    LibJingleEx::EAffiliation  m_eAffi;

    tagParticipant()
    {
        m_ePres  =  LibJingleEx::PRESENCE_NONE;
        m_eAffi  =  LibJingleEx::AFFILIATION_NONE;
    }
}TParticipant, *PTParticipant;

typedef std::vector<PTParticipant> vParticipant;

typedef struct tagChatRoom
{
    LibJingleEx::HROOM    m_hRoom;
    vParticipant          m_vInvited;
    vParticipant          m_vJoined;
    CWnd *                m_ChatRoom;

    tagChatRoom()
    {
        m_hRoom = 0;
        m_ChatRoom = 0;
    }

    ~tagChatRoom()
    {
        if ( m_ChatRoom )
        {
            m_ChatRoom->DestroyWindow();
            delete m_ChatRoom;
        }

        vParticipant::iterator it;

        for ( it = m_vInvited.begin(); it != m_vInvited.end(); it++ )
        {
            delete *it;
        }
        m_vInvited.clear();

        for ( it = m_vJoined.begin(); it != m_vJoined.end(); it++ )
        {
            delete *it;
        }
        m_vJoined.clear();
    }

    void Update( const LibJingleEx::PTJid ptJid, LibJingleEx::EPresence * pPresence, 
                 LibJingleEx::EAffiliation * peAffi )
    {
        vParticipant::iterator it;
        PTParticipant p = 0;
        for ( it = m_vJoined.begin(); it != m_vJoined.end(); it++ )
        {
            p = *it;
            if ( _IfJidEqual(*ptJid, p->m_tJid) )
            {
                break;
            }
        }

        if ( 0 == p )
        {
            p = new TParticipant;
            m_vJoined.push_back( p );
        }

        if ( pPresence )
        {
            p->m_ePres = *pPresence;
        }

        if ( peAffi )
        {
            p->m_eAffi = *peAffi;
        }
    }
}TChatRoom, *PTChatRoom;


typedef std::vector<PTChatRoom>   vChatRoom;

extern vChatRoom g_vChatRooms[MAX_ACCOUNT_NUMBER];

extern BOOL      g_bConferenceAvailable[MAX_ACCOUNT_NUMBER];

class FindChatRoomObj
{
public:
    FindChatRoomObj( LibJingleEx::HROOM hRoom ) : m_hRoom ( hRoom ){}

    bool operator() ( const PTChatRoom p )
    {
        if ( p->m_hRoom == m_hRoom )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    LibJingleEx::HROOM    m_hRoom;
};

const char * GetPresence( EPresence e );
const char * GetAffiliation( EAffiliation e );



HRESULT    LoadFromBuffer( const BYTE * pBuf, DWORD dwSize, IPicture * & pPicture );
void       Draw2Dc(  IPicture * & pPicture,  CDC * pDc, CRect * pRect = 0 );
int        GetDataFromPicture( BYTE * pbyData, DWORD & dwSize,  IPicture * & pPicture, CRect * pRect = 0 );