// MyTalkDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <direct.h>
#include "MyTalk.h"
#include "ChatDlg.h"
#include "PictureDlg.h"
#include "OnSubscribeDlg.h"
#include "ChatRoomDlg.h"
#include "ProgressDlg.h"
#include "EmptyDlg.h"
#include "FileTranferDlg.h"
#include "MyTalkDlg.h"
#include "MyConfigFile.h"
#include "XmppDlg.h"
#include "AddFriendDlg.h"
#include "SetNickDlg.h"
#include "CreateChatRoomDlg.h"
#include "SelAccountDlg.h"
#include "FileTranferDlg.h"
#include "TestDmgGroupDlg.h"
#include "VCardSetDlg.h"
#include "PasswordDlg.h"

extern CXmppDlg * g_pXmppDlg;
extern CMyTalkDlg * g_pMainDlg;


#pragma warning(disable: 4996)
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_MENU_COUNT                100

CTestDmgGroupDlg *  g_pTestDlg = 0;

int GetTreeDepth( CTreeCtrl & tree, HTREEITEM hItem )
{
    int nDepth = 0;

    HTREEITEM hParent = tree.GetParentItem( hItem );
    while( hParent )
    {
        nDepth++;
        hParent = tree.GetParentItem( hParent );
    }

    return nDepth;
}

HTREEITEM GetTreeTop( CTreeCtrl & tree, HTREEITEM hItem )
{
    HTREEITEM hLast   = 0;
    HTREEITEM hParent = tree.GetParentItem( hItem );

    while( hParent )
    {
        hLast = hParent;
        hParent = tree.GetParentItem( hParent );
    }

    return hLast;
}

class FindChatDlgObj
{
public:
    FindChatDlgObj( LibJingleEx::PTJid ptJid ) : m_ptJid(ptJid)
    {
        
    }

    bool operator () ( CChatDlg * pChatDlg )
    {
        if ( _IfJidEqual( *m_ptJid, pChatDlg->m_tJid )  )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    LibJingleEx::PTJid   m_ptJid;
};

class FindPictureDlgObj
{
public:
    FindPictureDlgObj( LibJingleEx::PTJid ptJid ) : m_ptJid(ptJid)
    {

    }

    bool operator () ( CPictureDlg * pDlg )
    {
        if ( _IfJidEqual( *m_ptJid, pDlg->m_tJid )  )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    LibJingleEx::PTJid   m_ptJid;
};


LibJingleEx::PTMember GetMemberInfo( LibJingleEx::HANDLE hApp, LibJingleEx::PTJid ptJid )
{
    LibJingleEx::PTGroup pGroup = LibJingleEx::GetRoster( hApp );

    while( pGroup )
    {
        LibJingleEx::PTMember pMember = pGroup->pChild;
        while( pMember )
        {
            if ( _IfJidEqual( pMember->tJid, *ptJid ) )
            {
                return pMember;
            }
            pMember = pMember->pNext;
        }
        pGroup = pGroup->pNext;
    }

    return 0;
}

void FindFriendInTree( CTreeCtrl & tree, int nIndex, LibJingleEx::PTJid ptJid, std::vector<HTREEITEM> & v )
{
    HTREEITEM hItem = tree.GetRootItem();
    while( hItem )
    {
        int nTmp = tree.GetItemData(hItem) - 1;
        if ( nTmp == nIndex )
        {
            break;
        }
        hItem = tree.GetNextSiblingItem(hItem);
    }

    if ( 0 == hItem )
    {
        return;
    }

    HTREEITEM hGroup = tree.GetChildItem( hItem );
    while( hGroup )
    {
        HTREEITEM hFriend = tree.GetChildItem(hGroup);
        while( hFriend )
        {
            LibJingleEx::PTMember p = (LibJingleEx::PTMember)tree.GetItemData(hFriend);
            if ( _IfJidEqual(p->tJid, *ptJid) )
            {
                v.push_back(hFriend);
                break;
            }
            hFriend = tree.GetNextSiblingItem(hFriend);
        }

        hGroup = tree.GetNextSiblingItem(hGroup);
    }



}

void DeleteItemInTree( CTreeCtrl & tree, HTREEITEM hItem )
{
    int nDepth = GetTreeDepth( tree, hItem );

    if ( 2 == nDepth )
    {
        LibJingleEx::PTMember p = (LibJingleEx::PTMember)tree.GetItemData(hItem);
        delete p;
        tree.DeleteItem( hItem );
    }
    else if ( 1 == nDepth )
    {
        HTREEITEM hChild = tree.GetChildItem(hItem );
        while( hChild )
        {
            LibJingleEx::PTMember p = (LibJingleEx::PTMember)tree.GetItemData(hChild);
            delete p;
            hChild = tree.GetNextSiblingItem( hChild );
        }
        tree.DeleteItem(hItem);
    }
    else
    {
        ASSERT( 0 == nDepth );

        HTREEITEM hGroup = tree.GetChildItem(hItem );
        while( hGroup )
        {
            HTREEITEM hChild = tree.GetChildItem(hGroup);
            while( hChild )
            {
                LibJingleEx::PTMember p = (LibJingleEx::PTMember)tree.GetItemData(hChild);
                delete p;
                hChild = tree.GetNextSiblingItem(hChild);
            }
            hGroup = tree.GetNextSiblingItem( hGroup);
        }
        tree.DeleteItem(hItem);
    }
}

int GetImgIndex( LibJingleEx::EPresence e )
{
    switch( e )
    {
        case LibJingleEx::PRESENCE_NONE:
            return 0;
            break;

        case LibJingleEx::PRESENCE_OFFLINE:
            return 0;
            break;

        case LibJingleEx::PRESENCE_XA:
            return 1;
            break;

        case LibJingleEx::PRESENCE_AWAY:
            return 2;
            break;

        case LibJingleEx::PRESENCE_DND:
            return 3;
            break;

        case LibJingleEx::PRESENCE_ONLINE:
            return 4;
            break;

        case LibJingleEx::PRESENCE_CHAT:
            return 5;
            break;

        default:
            ASSERT(0);
            return 0;
            break;
        
    }
    return 0;
}


class FindFileTranferDlgObj
{
public:
    FindFileTranferDlgObj( LibJingleEx::PTJid ptPeerJid ) : m_ptPeerJid( ptPeerJid ){}

    bool operator() ( CFileTranferDlg * pDlg )
    {
        if ( _IfJidEqual( pDlg->m_tPeerJid, *m_ptPeerJid ) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
private:
    LibJingleEx::PTJid m_ptPeerJid;
};



/******************************** 回调 **********************************/

void LIBJINGLE_CALLBACK OnLoginResult ( LibJingleEx::HANDLE hApp, LibJingleEx::ELibjingleState eState, 
                                        LibJingleEx::ELibjingleStateSubCode eSubCode, void * pUserData )
{
    void ** pTmp = new void*[3];

    pTmp[0] = (void *)eState;
    pTmp[1] = (void *)eSubCode;
    pTmp[2] = pUserData;

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_STATE, (WPARAM)hApp, (LPARAM)pTmp  );
}

void LIBJINGLE_CALLBACK OnRosterInfo( LibJingleEx::HANDLE hApp, const LibJingleEx::PTGroup pFirstGroup )
{
    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ROSTER, (WPARAM)hApp, 0  );
}

void LIBJINGLE_CALLBACK OnRosterChange( LibJingleEx::HANDLE hApp, const char * szGroupName, 
                                       const LibJingleEx::PTJid ptJid, 
                                       const LibJingleEx::PTMemberItem ptChange)
{
    void ** pTmp = new void*[3];

    char * pNewGroupName = new char[strlen(szGroupName)+1];
    strcpy( pNewGroupName, szGroupName );

    LibJingleEx::PTJid pNewJid = new LibJingleEx::TJid;
    memcpy( pNewJid, ptJid, sizeof(LibJingleEx::TJid) );

    LibJingleEx::PTMemberItem pNewChange = new LibJingleEx::TMemberItem;
    memcpy( pNewChange, ptChange, sizeof(LibJingleEx::TMemberItem) );

    pTmp[0] = pNewGroupName;
    pTmp[1] = pNewJid;
    pTmp[2] = pNewChange;

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ROSTER_CHANGE, (WPARAM)hApp, (LPARAM)pTmp );
}

void LIBJINGLE_CALLBACK OnRosterAdd( LibJingleEx::HANDLE hApp, const char * szGroupName, 
                                    const LibJingleEx::PTJid ptJid, 
                                    const LibJingleEx::PTMemberItem ptChange)
{
    void ** pTmp = new void*[3];
    
    char * pNewGroupName = new char[strlen(szGroupName)+1];
    strcpy( pNewGroupName, szGroupName );

    LibJingleEx::PTJid pNewJid = new LibJingleEx::TJid;
    memcpy( pNewJid, ptJid, sizeof(LibJingleEx::TJid) );

    LibJingleEx::PTMemberItem pNewChange = new LibJingleEx::TMemberItem;
    memcpy( pNewChange, ptChange, sizeof(LibJingleEx::TMemberItem) );

    pTmp[0] = pNewGroupName;
    pTmp[1] = pNewJid;
    pTmp[2] = pNewChange;

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ROSTER_ADD, (WPARAM)hApp, (LPARAM)pTmp );
}

void OnRosterDel( LibJingleEx::HANDLE hApp, const char * szGroupName, 
                 const LibJingleEx::PTJid ptJid )
{
    void ** pTmp = new void*[2];

    char * pNewGroupName = new char[strlen(szGroupName)+1];
    strcpy( pNewGroupName, szGroupName );

    LibJingleEx::PTJid pNewJid = new LibJingleEx::TJid;
    memcpy( pNewJid, ptJid, sizeof(LibJingleEx::TJid) );

    pTmp[0] = pNewGroupName;
    pTmp[1] = pNewJid;

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ROSTER_DEL, (WPARAM)hApp, (LPARAM)pTmp );
}

void LIBJINGLE_CALLBACK OnRosterPresenceChange( LibJingleEx::HANDLE hApp, LibJingleEx::PTPresence pPresence )
{
    LibJingleEx::PTPresence pNew = new LibJingleEx::TPresence;
    memcpy( pNew, pPresence, sizeof(LibJingleEx::TPresence) );

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_PRESENCE, (WPARAM)hApp, (LPARAM)pNew  );
}

void LIBJINGLE_CALLBACK OnMessage( LibJingleEx::HANDLE hApp, const LibJingleEx::PTJid pJid, const char * szMessage )
{
    LibJingleEx::PTJid pNewJid = new LibJingleEx::TJid;
    memcpy( pNewJid, pJid, sizeof(LibJingleEx::TJid) );

    int nLen = strlen( szMessage );
    char * pNewMsg = new char[nLen +1];
    strcpy( pNewMsg, szMessage );

    void ** pTmp = new void *[2];
    pTmp[0] = pNewJid;
    pTmp[1] = pNewMsg;

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_CHAT_MSG, (WPARAM)hApp, (LPARAM)pTmp );
}

void LIBJINGLE_CALLBACK OnSubscribe( LibJingleEx::HANDLE hApp, const LibJingleEx::PTJid pJid )
{
    LibJingleEx::PTJid ptNewJid = new LibJingleEx::TJid;
    memcpy( ptNewJid, pJid, sizeof(LibJingleEx::TJid) );

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_SUBSCRIBE, (WPARAM)hApp, (LPARAM)ptNewJid );

}

void LIBJINGLE_CALLBACK OnChatRoomService(LibJingleEx::HANDLE hApp, BOOL bAvailable)
{
    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_CONFERENCE_SERVER, (WPARAM)hApp, (LPARAM)bAvailable );
}

void LIBJINGLE_CALLBACK OnCreateUniqueRoom( LibJingleEx::HANDLE hApp, HROOM hRoom, int nErrorCode)
{
    if ( nErrorCode != 0 && nErrorCode != 1 )
    {
        ::MessageBox( 0,  "创建聊天室失败！", "", 0 );
        return;
    }

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_CREATE_CHATROOM_SUCCESS, (WPARAM)hRoom, 0 );
}

void LIBJINGLE_CALLBACK OnInvited( LibJingleEx::HANDLE hApp, LibJingleEx::HROOM hRoom, LibJingleEx::PTJid ptInviterJid,  BOOL bPersistent, const char * szRoomName )
{
    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_INVITE, (WPARAM)hRoom, 0 );
}

void LIBJINGLE_CALLBACK OnParticipantPresenceChange(LibJingleEx::HROOM hRoom, const LibJingleEx::PTJid ptJid,  
                                                    LibJingleEx::EPresence * peShow,    const char * szDescription,
                                                    LibJingleEx::EAffiliation eAffiliation ,
                                                    BOOL  bSelf,                                     
                                                    int   nFlag,
                                                    LibJingleEx::HANDLE hApp )
{
    void ** ppTmp = new void *[7];

    LibJingleEx::PTJid  pNewJid = new LibJingleEx::TJid;
    memcpy( pNewJid, ptJid, sizeof(LibJingleEx::TJid) );
    ppTmp[0] = pNewJid;

    ppTmp[1] = 0;
    ppTmp[2] = 0;

    LibJingleEx::EPresence * pNewPres = 0;
    if ( peShow )
    {
        pNewPres = new LibJingleEx::EPresence;
        *pNewPres = *peShow;
        
        char * pNewDes = 0;
        if ( szDescription )
        {
            pNewDes = new char[strlen(szDescription) + 1 ];
            strcpy( pNewDes, szDescription );
        }
        ppTmp[2] = pNewDes;
    }
    ppTmp[1] = pNewPres;

    ppTmp[3] = (void *)eAffiliation;
    ppTmp[4] = hApp;

    ppTmp[5] = (void *)bSelf;
    ppTmp[6] = (void *)nFlag;


    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_CHATROOM_PRESENCE, (WPARAM)hRoom, (LPARAM)ppTmp );
    
}

void LIBJINGLE_CALLBACK OnGrant( LibJingleEx::HANDLE hApp, LibJingleEx::HROOM hRoom, LibJingleEx::EAffiliation eOldAffiliation, LibJingleEx::EAffiliation eNewAffiliation )
{

}

void LIBJINGLE_CALLBACK OnDestroyRoom ( LibJingleEx::HROOM hRoom, LibJingleEx::HANDLE hApp )
{
    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_CHATROOM_DESTROY, (WPARAM)hApp, (LPARAM)hRoom );
}

void LIBJINGLE_CALLBACK OnRoomMessage( LibJingleEx::HANDLE hApp, LibJingleEx::HROOM hRoom, const LibJingleEx::PTJid ptJid,  const char * szMessage, const char * szMsgId )
{
    void ** ppTmp = new void *[3];

    LibJingleEx::PTJid pNewJid = new LibJingleEx::TJid;
    memcpy( pNewJid, ptJid, sizeof(LibJingleEx::TJid) );

    char * pNewMsg = new char[ strlen(szMessage) + 1  ];
    strcpy( pNewMsg, szMessage );

    char * pNewMsgId = new char[ strlen(szMsgId) + 1 ];
    strcpy( pNewMsgId, szMsgId );

    ppTmp[0] = pNewJid;
    ppTmp[1] = pNewMsg;
    ppTmp[2] = pNewMsgId;

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_CHATROOM_MESSAGE, (WPARAM)hRoom, (LPARAM)ppTmp );
}

void OnFileOffer( LibJingleEx::HANDLE hApp, LibJingleEx::HSession hSession, const LibJingleEx::PTJid pJid, 
                  const LibJingleEx::FileShareManifest * pManifest, BOOL bSender )
{
    LibJingleEx::PTJid pNewJid = new LibJingleEx::TJid;
    memcpy( pNewJid, pJid, sizeof(LibJingleEx::TJid) );

    LibJingleEx::PFileShareManifest pNewManifest = new LibJingleEx::FileShareManifest;
    memcpy( pNewManifest, pManifest, sizeof(LibJingleEx::FileShareManifest) );

    void ** pTmp = new void *[4];
    pTmp[0] = hSession;
    pTmp[1] = pNewJid;
    pTmp[2] = pNewManifest;
    pTmp[3] = (void *)bSender;

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_FILE_OFFER, (WPARAM)hApp, (LPARAM)pTmp ); 
}

void OnFileTransfer( LibJingleEx::HANDLE hApp, LibJingleEx::HSession hSession )
{
    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_FILE_TRANSFER, (WPARAM)hApp, (LPARAM)hSession );
}

void OnFileComplete( LibJingleEx::HANDLE hApp, LibJingleEx::HSession hSession, const char * szCompletePath )
{
    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_FILE_COMPLETE, (WPARAM)hApp, (LPARAM)hSession );
}

void OnFileCancel( LibJingleEx::HANDLE hApp, LibJingleEx::HSession hSession, BOOL bLocal )
{
    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_FILE_CANCEL, (WPARAM)hApp, (LPARAM)hSession );
}

void OnFileFailure( LibJingleEx::HANDLE hApp, LibJingleEx::HSession hSession, int nErrorCode )
{
    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_FILE_FAILURE, (WPARAM)hApp, (LPARAM)hSession );
}

void OnFileProgress( LibJingleEx::HANDLE hApp, LibJingleEx::HSession hSession, int nPercent, const char * szCurFileName,
                     unsigned long dwTransferSize, unsigned long dwTransferRate )
{
    DWORD dwLen = strlen( szCurFileName );
    char * pFileName = new char[dwLen + 1];
    strcpy( pFileName, szCurFileName );

    void ** pTmp = new void *[5];
    pTmp[0] = (void *)hSession;
    pTmp[1] = (void *)nPercent;
    pTmp[2] = (void *)pFileName;
    pTmp[3] = (void *)dwTransferSize;
    pTmp[4] = (void *)dwTransferRate;

    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_FILE_PROGRESS, (WPARAM)hApp, (LPARAM)pTmp ); 
}

void OnVCardInfo( LibJingleEx::HANDLE hApp, const LibJingleEx::TJid * ptJid, 
                  const TVCard * ptVCard, BOOL bSelf )
{
    LibJingleEx::TJid * pNewJid = new LibJingleEx::TJid;
    memcpy( pNewJid, ptJid, sizeof(LibJingleEx::TJid) );

    LibJingleEx::TVCard * ptNewVcard = new LibJingleEx::TVCard;
    memcpy( ptNewVcard, ptVCard, sizeof(LibJingleEx::TVCard) );

    void ** pTmp = new void*[3];
    pTmp[0] = pNewJid;
    pTmp[1] = ptNewVcard;
    pTmp[2] = (void *)bSelf;
    
    ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_VCARD_INFO, (WPARAM)hApp, (LPARAM)pTmp ); 
}


void OnXmppMessage ( LibJingleEx::EMsgId eMsgId, LibJingleEx::HANDLE hApp, void * pUserData, 
                     const unsigned char * pMsg, unsigned long dwMsgLen )
{
    switch( eMsgId )
    {
    case LibJingleEx::ON_STATE:
        {
            LibJingleEx::CMsgData msgData( pMsg, dwMsgLen );

            LibJingleEx::ELibjingleState eState;
            LibJingleEx::ELibjingleStateSubCode eSubCode;

            msgData.Read( (unsigned char *)&eState,   sizeof(eState) );
            msgData.Read( (unsigned char *)&eSubCode,  sizeof(eSubCode) );

            OnLoginResult( hApp, eState, eSubCode, pUserData );
        }
        break;
    
    case LibJingleEx::ON_ROSTER_INFO:
        {
            LibJingleEx::PTGroup pGroup = *(LibJingleEx::PTGroup *)pMsg;
            OnRosterInfo( hApp, pGroup );
        }
        break;

    case LibJingleEx::ON_ROSTER_CHANGE:
        {
            LibJingleEx::CMsgData msgData( pMsg, dwMsgLen );

            char szGroupName[LibJingleEx::MAX_GROUP_NAME_LEN];
            LibJingleEx::TJid         tJid;
            LibJingleEx::TMemberItem  tItem;

            msgData.Read( (unsigned char *)szGroupName, LibJingleEx::MAX_GROUP_NAME_LEN );
            msgData.Read( (unsigned char *)&tJid,       sizeof(tJid) );
            msgData.Read( (unsigned char *)&tItem,      sizeof(tItem) );

            ::OnRosterChange( hApp, szGroupName, &tJid, &tItem );
        }
        break;

    case LibJingleEx::ON_ROSTER_ADD:
        {
            LibJingleEx::CMsgData msgData( pMsg, dwMsgLen );

            char szGroupName[LibJingleEx::MAX_GROUP_NAME_LEN];
            LibJingleEx::TJid         tJid;
            LibJingleEx::TMemberItem  tItem;

            msgData.Read( (unsigned char *)szGroupName, LibJingleEx::MAX_GROUP_NAME_LEN );
            msgData.Read( (unsigned char *)&tJid,       sizeof(tJid) );
            msgData.Read( (unsigned char *)&tItem,      sizeof(tItem) );

            ::OnRosterAdd( hApp, szGroupName, &tJid, &tItem );
        }
        break;

    case LibJingleEx::ON_ROSTER_DEL:
        {
            LibJingleEx::CMsgData msgData( pMsg, dwMsgLen );

            char szGroupName[LibJingleEx::MAX_GROUP_NAME_LEN];
            LibJingleEx::TJid         tJid;

            msgData.Read( (unsigned char *)szGroupName, LibJingleEx::MAX_GROUP_NAME_LEN );
            msgData.Read( (unsigned char *)&tJid,       sizeof(tJid) );

            ::OnRosterDel( hApp, szGroupName, &tJid );
        }
        break;

    case LibJingleEx::ON_FRIEND_PRESENCE:
        {
            LibJingleEx::CMsgData msgData( pMsg, dwMsgLen );

            LibJingleEx::TPresence tPresence;
            BOOL bSelf;
            LibJingleEx::EPresence ePres_max;
            BOOL bAdded;

            BOOL  bMaxAudio = FALSE;;
            BOOL  bMaxVideo = FALSE;
            BOOL  bFromStatusChatroom = FALSE;

            BOOL  bChanged = FALSE;

            msgData.Read( (unsigned char *)&tPresence,   sizeof(tPresence) );
            msgData.Read( (unsigned char *)&bSelf,       sizeof(bSelf) ); 
            msgData.Read( (unsigned char *)&ePres_max,   sizeof(ePres_max) ); 
            msgData.Read( (unsigned char *)&bAdded,      sizeof(bAdded) ); 
            msgData.Read( (unsigned char *)&bMaxAudio,      sizeof(bMaxAudio) ); 
            msgData.Read( (unsigned char *)&bMaxVideo,      sizeof(bMaxVideo) ); 
            msgData.Read( (unsigned char *)&bFromStatusChatroom,      sizeof(bFromStatusChatroom) ); 
            msgData.Read( (unsigned char *)&bChanged,                 sizeof(bChanged) ); 


            ::OnRosterPresenceChange( hApp, &tPresence );
        }
        break;

    case LibJingleEx::ON_CHAT_MESSAGE:
        {
            LibJingleEx::CMsgData msgData( pMsg, dwMsgLen );

            LibJingleEx::TJid   tJid;
            BOOL                bOffLine = FALSE;
            LibJingleEx::TDmsChatContent tMsg;

            msgData.Read( (unsigned char *)&tJid,             sizeof(tJid) );
            msgData.Read( (unsigned char *)&bOffLine,         sizeof(bOffLine) );
            msgData.Read( (unsigned char *)&tMsg,             sizeof(tMsg) );

            ::OnMessage( hApp, &tJid, tMsg.m_szContent );
        }
        break;

    case LibJingleEx::ON_QUERY_SUBSCRIBED:
        {
            LibJingleEx::TJid   tJid;
            memcpy( &tJid, pMsg, sizeof(tJid) );

            ::OnSubscribe( hApp, &tJid );
        }
        break;

    case LibJingleEx::ON_GROUP_CHAT_ABILITY:
        {
            BOOL bValue = *(BOOL *)pMsg;
            ::OnChatRoomService( hApp, bValue );
        }
        break;

    case LibJingleEx::ON_CREATE_UNIQUE_CHATROOM:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HROOM  hRoom;
            int                 nError;
            char                szRoomId[64];
            char                szServiceName[64];

            msgData.Read( (unsigned char *)&hRoom,  sizeof(hRoom) );
            msgData.Read( (unsigned char *)&nError, sizeof(int) );
            msgData.Read( (unsigned char *)szRoomId, sizeof(szRoomId) );
            msgData.Read( (unsigned char *)szServiceName, sizeof(szServiceName) );


            ::OnCreateUniqueRoom( hApp, hRoom, nError );
        }
        break;

    case LibJingleEx::ON_INVITED_TO_CHATROOM:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HROOM  hRoom;
            LibJingleEx::TJid   tInviterJid;
            BOOL bPersistent = FALSE;
            int  nType = 0;
            char szRoomName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN];
            char szRoomId[64];
            char szService[64];

            msgData.Read( (unsigned char *)&hRoom,        sizeof(hRoom) );
            msgData.Read( (unsigned char *)&tInviterJid,  sizeof(tInviterJid) );
            msgData.Read( (unsigned char *)&bPersistent,  sizeof(bPersistent) );
            msgData.Read( (unsigned char *)&nType,        sizeof(nType) );
            msgData.Read( (unsigned char *)szRoomName,    sizeof(szRoomName) );
            msgData.Read( (unsigned char *)szRoomId,      sizeof(szRoomId) );
            msgData.Read( (unsigned char *)szService,     sizeof(szService) );

            ::OnInvited( hApp, hRoom, &tInviterJid, bPersistent, szRoomName );
        }
        break;

    case LibJingleEx::ON_PARTICIPANT_PRESENCE:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HROOM  hRoom;
            LibJingleEx::TJid   tJid;
            LibJingleEx::EPresence * pPresence = 0;
            const char * pDescription = 0;
            LibJingleEx::EAffiliation tAffiliation;
            BOOL bSelf;
            int nFlag;
            LibJingleEx::EPresence  eMax = LibJingleEx::PRESENCE_NONE;
            char                szRoomId[64];
            char                szServiceName[64];

            msgData.Read( (unsigned char *)&hRoom,          sizeof(hRoom) );
            msgData.Read( (unsigned char *)&tJid,           sizeof(tJid) );
            msgData.Read( (unsigned char *)&pPresence,      sizeof(pPresence) );
            msgData.Read( (unsigned char *)&pDescription,   sizeof(pDescription) );
            msgData.Read( (unsigned char *)&tAffiliation,   sizeof(tAffiliation) );
            msgData.Read( (unsigned char *)&bSelf,          sizeof(bSelf) );
            msgData.Read( (unsigned char *)&nFlag,          sizeof(nFlag) );
            msgData.Read( (unsigned char *)&eMax,           sizeof(eMax) );
            msgData.Read( (unsigned char *)szRoomId, sizeof(szRoomId) );
            msgData.Read( (unsigned char *)szServiceName, sizeof(szServiceName) );

            ::OnParticipantPresenceChange( hRoom, &tJid, pPresence, pDescription, tAffiliation, bSelf, nFlag, hApp );
        }
        break;

    case LibJingleEx::ON_GRANT:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HROOM  hRoom;
            LibJingleEx::EAffiliation tOldAffiliation;
            LibJingleEx::EAffiliation tNewAffiliation;

            msgData.Read( (unsigned char *)&hRoom,             sizeof(hRoom) );
            msgData.Read( (unsigned char *)&tOldAffiliation,   sizeof(tOldAffiliation) );
            msgData.Read( (unsigned char *)&tNewAffiliation,   sizeof(tNewAffiliation) );

            ::OnGrant( hApp, hRoom, tOldAffiliation, tNewAffiliation );
        }
        break;
    
    case LibJingleEx::ON_DESTROY_CHATROOM:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HROOM  hRoom;
            char                szRoomId[64];
            char                szServiceName[64];

            msgData.Read( (unsigned char *)&hRoom,             sizeof(hRoom) );
            msgData.Read( (unsigned char *)szRoomId, sizeof(szRoomId) );
            msgData.Read( (unsigned char *)szServiceName, sizeof(szServiceName) );

            ::OnDestroyRoom( hRoom, hApp );
        }
        break;

    case LibJingleEx::ON_GROUPCHAT_MESSAGE:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HROOM  hRoom;
            LibJingleEx::TJid   tJid;
            BOOL                bOffLine = FALSE;
            LibJingleEx::TDmsChatContent tMsg;
            char                szMsgId[32];
            char                szRoomId[64];
            char                szServiceName[64];

            msgData.Read( (unsigned char *)&hRoom,          sizeof(hRoom) );
            msgData.Read( (unsigned char *)&tJid,           sizeof(tJid) );
            msgData.Read( (unsigned char *)&bOffLine,       sizeof(bOffLine) );
            msgData.Read( (unsigned char *)szMsgId,         sizeof(szMsgId) );
            msgData.Read( (unsigned char *)&tMsg,           sizeof(tMsg) );
            msgData.Read( (unsigned char *)szRoomId, sizeof(szRoomId) );
            msgData.Read( (unsigned char *)szServiceName, sizeof(szServiceName) );

            ::OnRoomMessage( hApp, hRoom, &tJid, tMsg.m_szContent, szMsgId );
        }
        break;

    case LibJingleEx::ON_FILE_OFFER:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HSession   hSession;
            LibJingleEx::TJid       tJid;
            LibJingleEx::FileShareManifest tManifest;
            BOOL bSender;

            msgData.Read( (unsigned char *)&hSession,          sizeof(hSession) );
            msgData.Read( (unsigned char *)&tJid,              sizeof(tJid) );
            msgData.Read( (unsigned char *)&tManifest,         sizeof(tManifest) );
            msgData.Read( (unsigned char *)&bSender,           sizeof(bSender) );

            ::OnFileOffer( hApp, hSession, &tJid, &tManifest, bSender );
        }
        break;

    case LibJingleEx::ON_FILE_TRANSFER:
        {
            LibJingleEx::HSession   hSession;
            memcpy( &hSession, pMsg, sizeof(hSession) );

            ::OnFileTransfer( hApp, hSession );
        }
        break;

    case LibJingleEx::ON_FILE_COMPLETE:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HSession   hSession;
            char                    szCompletePath[256] = {0};

            msgData.Read( (unsigned char *)&hSession,          sizeof(hSession) );
            msgData.Read( (unsigned char *)szCompletePath                       );

            ::OnFileComplete( hApp, hSession, szCompletePath );
        }
        break;

    case LibJingleEx::ON_FILE_CANCEL:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HSession   hSession;
            BOOL bLocal;

            msgData.Read( (unsigned char *)&hSession,          sizeof(hSession) );
            msgData.Read( (unsigned char *)&bLocal,            sizeof(bLocal) );

            ::OnFileCancel( hApp, hSession, bLocal );
        }
        break;

    case LibJingleEx::ON_FILE_FAILURE:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HSession   hSession;
            int nError = 0;

            msgData.Read( (unsigned char *)&hSession,          sizeof(hSession) );
            msgData.Read( (unsigned char *)&nError,            sizeof(nError) );

            ::OnFileFailure( hApp, hSession, nError );
        }
        break;

    case LibJingleEx::ON_FILE_PROGRESS:
        {
            LibJingleEx::CMsgData  msgData( pMsg, dwMsgLen );

            LibJingleEx::HSession   hSession;
            int  nPercent;
            char szFileName[LibJingleEx::MAX_FILENAME_SIZE];
            DWORD                   dwTransferSize = 0;
            DWORD                   dwRate = 0;

            msgData.Read( (unsigned char *)&hSession,          sizeof(hSession) );
            msgData.Read( (unsigned char *)&nPercent,          sizeof(nPercent) );
            msgData.Read( (unsigned char *)&dwTransferSize,    sizeof(dwTransferSize) );
            msgData.Read( (unsigned char *)&dwRate,            sizeof(dwRate) );
            msgData.Read( (unsigned char *)&szFileName );

            ::OnFileProgress( hApp, hSession, nPercent, szFileName, dwTransferSize, dwRate );
        }
        break;

    case LibJingleEx::ON_VCARD_INFO:
        {

        }
        break;

    case LibJingleEx::ON_NOTIFY_ADD_DMS_GROUP:
        {
            LibJingleEx::TDmsGroup *    ptDmsGroup = new LibJingleEx::TDmsGroup;

            ASSERT( dwMsgLen == sizeof(LibJingleEx::TDmsGroup) );
            memcpy( ptDmsGroup, pMsg, sizeof(LibJingleEx::TDmsGroup) );

            if ( g_pTestDlg )
            {
                ::PostMessage( g_pTestDlg->m_hWnd, UM_ON_DMS_ADD_GROUP, (WPARAM)hApp, (LPARAM)ptDmsGroup );
            }
            else
            {
                delete ptDmsGroup;
            }
            
        }
        break;

    case LibJingleEx::ON_NOTIFY_DEL_DMS_GROUP:
        {
            ASSERT( dwMsgLen == sizeof(DWORD) );
            DWORD dwId = *(DWORD *)pMsg;

            if ( g_pTestDlg )
            {
                ::PostMessage( g_pTestDlg->m_hWnd, UM_ON_DMS_DEL_GROUP, (WPARAM)hApp, (LPARAM)dwId );
            }
        }
        break;

    case LibJingleEx::ON_NOTIFY_MODIFY_DMS_GROUP:
        {
            LibJingleEx::TDmsGroup *    ptDmsGroup = new LibJingleEx::TDmsGroup;

            ASSERT( dwMsgLen == sizeof(LibJingleEx::TDmsGroup) );
            memcpy( ptDmsGroup, pMsg, sizeof(LibJingleEx::TDmsGroup) );

            if ( g_pTestDlg )
            {
                ::PostMessage( g_pTestDlg->m_hWnd, UM_ON_DMS_MODIFY_GROUP, (WPARAM)hApp, (LPARAM)ptDmsGroup );
            }
            else
            {
                delete ptDmsGroup;
            }
        }
        break;

    case LibJingleEx::ON_NOTIFY_ADD_DMS_MEMBER:
        {
            LibJingleEx::TDmsMember * ptMember = new LibJingleEx::TDmsMember;

            ASSERT( dwMsgLen == sizeof(LibJingleEx::TDmsMember) );
            memcpy( ptMember, pMsg, sizeof(LibJingleEx::TDmsMember) );

            if ( g_pTestDlg )
            {
                ::PostMessage( g_pTestDlg->m_hWnd, UM_ON_DMS_ADD_MEMBER, (WPARAM)hApp, (LPARAM)ptMember );
            }
            else
            {
                delete ptMember;
            }
        }
        break;

    case LibJingleEx::ON_NOTIFY_DEL_DMS_MEMBER:
        {
            DWORD   dwId  = 0;

            ASSERT( dwMsgLen == sizeof(DWORD) );
            memcpy( &dwId, pMsg, sizeof(DWORD) );

            if ( g_pTestDlg )
            {
                ::PostMessage( g_pTestDlg->m_hWnd, UM_ON_DMS_DEL_MEMBER, (WPARAM)hApp, (LPARAM)dwId );
            }
        }
        break;

    case LibJingleEx::ON_NOTIFY_MODIFY_DMS_MEMBER:
        {
            LibJingleEx::TDmsMember * ptMember = new LibJingleEx::TDmsMember;

            ASSERT( dwMsgLen == sizeof(LibJingleEx::TDmsMember) );
            memcpy( ptMember, pMsg, sizeof(LibJingleEx::TDmsMember) );

            if ( g_pTestDlg )
            {
                ::PostMessage( g_pTestDlg->m_hWnd, UM_ON_DMS_MODIFY_MEMBER, (WPARAM)hApp, (LPARAM)ptMember );
            }
            else
            {
                delete ptMember;
            }
        }
        break;

    case LibJingleEx::ON_NOTIFY_MOVE_DMS_MEMBER:
        {
            LibJingleEx::CMsgData   cMsg(pMsg, dwMsgLen);

            DWORD dwOldGroupId = 0;
            LibJingleEx::TDmsMember * ptMember = new LibJingleEx::TDmsMember;

            cMsg.Read( (unsigned char *)&dwOldGroupId,  sizeof(dwOldGroupId) );
            cMsg.Read( (unsigned char *)ptMember,       sizeof(LibJingleEx::TDmsMember) );

            //ASSERT( dwMsgLen == sizeof(LibJingleEx::TDmsMember) );
            //memcpy( ptMember, pMsg, sizeof(LibJingleEx::TDmsMember) );

            if ( g_pTestDlg )
            {
                ::PostMessage( g_pTestDlg->m_hWnd, UM_ON_DMS_MOVE_MEMBER, (WPARAM)hApp, (LPARAM)ptMember );
            }
            else
            {
                delete ptMember;
            }
        }
        break;

    case LibJingleEx::ON_PICTURE_MSG:
        {
            LibJingleEx::CMsgData  cMsg( pMsg, dwMsgLen );

            char * pId               = new char[32];
            LibJingleEx::PTJid ptJid = new LibJingleEx::TJid;
            int nFlag                = 0;
            DWORD dwTotalSize        = 0;
            DWORD dwBufSize          = 0;
            
            cMsg.Read( (unsigned char *)pId,          32 );
            cMsg.Read( (unsigned char *)ptJid,        sizeof(LibJingleEx::TJid) );
            cMsg.Read( (unsigned char *)&nFlag,       sizeof(int) );
            cMsg.Read( (unsigned char *)&dwTotalSize, sizeof(DWORD) );
            cMsg.Read( (unsigned char *)&dwBufSize,   sizeof(DWORD) );

            BYTE * pBuf = new BYTE[dwBufSize];
            cMsg.Read( (unsigned char *)pBuf,   dwBufSize );

            void ** ppTmp = new void *[6];
            ppTmp[0] = (void *)pId;
            ppTmp[1] = (void *)ptJid;
            ppTmp[2] = (void *)nFlag;
            ppTmp[3] = (void *)dwTotalSize;
            ppTmp[4] = (void *)dwBufSize;
            ppTmp[5] = (void *)pBuf;

            ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_PICTURE, (WPARAM)hApp, (LPARAM)ppTmp );
        }
        break;

    case LibJingleEx::ON_PUBSUB_CREATE_NODE_RESULT:
        {
            DWORD   dwContextId = 0;
            BOOL    bSuccess    = FALSE;
            LibJingleEx::CMsgData  cMsg( pMsg, dwMsgLen );

            cMsg.Read( (unsigned char *)&dwContextId,    sizeof(dwContextId) );
            cMsg.Read( (unsigned char *)&bSuccess,       sizeof(bSuccess) );
        }
        break;

    case LibJingleEx::ON_PUBSUB_DELETE_NODE_RESULT:
        {
            DWORD   dwContextId = 0;
            BOOL    bSuccess    = FALSE;
            LibJingleEx::CMsgData  cMsg( pMsg, dwMsgLen );

            cMsg.Read( (unsigned char *)&dwContextId,    sizeof(dwContextId) );
            cMsg.Read( (unsigned char *)&bSuccess,       sizeof(bSuccess) );
        }
        break;

    case LibJingleEx::ON_PUBSUB_SUBSCRIBE_NODE_RESULT:
        {
            DWORD   dwContextId = 0;
            BOOL    bSuccess    = FALSE;
            LibJingleEx::CMsgData  cMsg( pMsg, dwMsgLen );

            cMsg.Read( (unsigned char *)&dwContextId,    sizeof(dwContextId) );
            cMsg.Read( (unsigned char *)&bSuccess,       sizeof(bSuccess) );
        }
        break;

    case LibJingleEx::ON_ROOM_ROSTER_ADDED:
        {
            LibJingleEx::CMsgData * pNewMsg = new LibJingleEx::CMsgData( pMsg, dwMsgLen );
            ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_CHATROOM_ROSTER_ADD, (WPARAM)hApp, (LPARAM)pNewMsg );
        }
        break;

    case LibJingleEx::ON_ROOM_ROSTER_DELETED:
        {
            LibJingleEx::CMsgData * pNewMsg = new LibJingleEx::CMsgData(pMsg, dwMsgLen);
            ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_CHATROOM_ROSTER_DEL, (WPARAM)hApp, (LPARAM)pNewMsg );
        }
        break;

    case LibJingleEx::ON_PERSISTENT_CHATROOMS_NOTIFY:
        {
            DWORD  dwCnt = 0;
            memcpy( &dwCnt, pMsg, sizeof(DWORD) );

            LibJingleEx::CMsgData  cMsg( pMsg + sizeof(DWORD), dwMsgLen - sizeof(DWORD) );

            DWORD i;
            for ( i = 0; i < dwCnt; i++ )
            {
                LibJingleEx::TJid   tRoomJid;
                char                szName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN];
                
                cMsg.Read( (unsigned char *)&tRoomJid,   sizeof(tRoomJid) );
                cMsg.Read( (unsigned char *)szName,      LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN );
            }

            unsigned char * pData = new unsigned char[dwMsgLen+sizeof(DWORD)];
            unsigned char * p = pData;

            memcpy( p, &dwMsgLen, sizeof(DWORD) );
            p += sizeof(DWORD);

            memcpy( p, pMsg, dwMsgLen );
            p += dwMsgLen;

            ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_NOTIFY_PERSISTENT_CHATROOM, (WPARAM)hApp, (LPARAM)pData );
        }
        break;

    case LibJingleEx::ON_CHAT_STATE:
        {
            LibJingleEx::CMsgData msgData( pMsg, dwMsgLen );

            LibJingleEx::TJid        tJid;
            LibJingleEx::ECharStatus e;
            
            msgData.Read( (unsigned char *)&tJid,             sizeof(tJid) );
            msgData.Read( (unsigned char *)&e,                sizeof(e) );


            unsigned char * pData = new unsigned char[dwMsgLen];
            memcpy( pData, pMsg, dwMsgLen );

            
            ::PostMessage( g_pMainDlg->GetSafeHwnd(), UM_ON_NOTIFY_CHAR_STATE, (WPARAM)hApp, (LPARAM)pData );
        }
        break;

    case LibJingleEx::ON_CUSTOM_MESSAGE:
        {
            LibJingleEx::CMsgData msgData( pMsg, dwMsgLen );

            LibJingleEx::TJid        tJid;
            LibJingleEx::TCustomMsg  tMsg;

            msgData.Read( (unsigned char *)&tJid,             sizeof(tJid) );
            msgData.Read( (unsigned char *)&tMsg,             sizeof(tMsg) );

            int a = 100;

        }
        break;

    case LibJingleEx::ON_CUSTOM_CONFIG_CHANGE:
        {
            LibJingleEx::CMsgData msgData( pMsg, dwMsgLen );

            LibJingleEx::TCustomConfigItem        tItem;
            char                                  szOld[LibJingleEx::MAX_CUSTOM_CONFIG_VALUE_LEN];

            msgData.Read( (unsigned char *)&tItem,             sizeof(tItem) );
            msgData.Read( (unsigned char *)&szOld,             sizeof(szOld) );

            int a = 100;
        }                                      
        break;

    default:
        // ::MessageBox( 0, "Unknown message", "", 0 );
        break;
    }
}



/******************************** 回调 结束 **********************************/


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMyTalkDlg 对话框




CMyTalkDlg::CMyTalkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyTalkDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    int i;
    for ( i = 0; i < MAX_ACCOUNT_NUMBER; i++ )
    {
        m_nStarting[i] = 0;
    }
}

void CMyTalkDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_STATUS, m_comboStatus);
    DDX_Control(pDX, IDC_TREE_ROSTER, m_treeRoster);
}

BEGIN_MESSAGE_MAP(CMyTalkDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_COMMAND(ID_ACCOUNT, OnAccount)
    ON_COMMAND(ID_XMPP_CONSOLE, OnXmppConsole)
    ON_COMMAND(ID_ADD_FRIEND, OnAddFriend)
    ON_COMMAND(ID_DEL_FRIEND, OnDelFriend)
    ON_COMMAND(ID_DEBUG_INFO, OnDebugInfo)
    ON_COMMAND(ID_SET_NICK, OnSetNick )
    ON_COMMAND(ID_SUBSCRIBE, OnSendSubscribe )
    ON_COMMAND(ID_UNSUBSCRIBE, OnSendUnsubscribe )
    ON_COMMAND(ID_UNSUBSCRIBED, OnSendUnsubscribed )
    ON_COMMAND(ID_CREATE_CHATROOM, OnCreateChatRoom )
    ON_COMMAND(ID_FILE_TRANSFER, OnFileTransfer )
    ON_COMMAND(ID_VCARD_SET, OnVCardSet )
    ON_COMMAND(ID_GET_VCARD_INFO, OnVCardGet )
    ON_COMMAND(ID_PASSWORD, OnSetPassword )
    ON_COMMAND(ID_SEND_PICTURE, OnSendPicture )
    ON_WM_TIMER()

    ON_WM_ACTIVATE()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_MESSAGE( UM_STATE, OnState )
    ON_MESSAGE( UM_ROSTER, OnRoster )
    ON_MESSAGE( UM_CHAT_MSG, OnChatMsg )
    ON_MESSAGE( UM_PRESENCE, OnPresence )
    ON_MESSAGE( UM_ROSTER_ADD, OnRosterAdd )
    ON_MESSAGE( UM_ROSTER_DEL, OnRosterDel )
    ON_MESSAGE( UM_ROSTER_CHANGE, OnRosterChange )
    ON_MESSAGE( UM_ON_SUBSCRIBE, OnSubscribe )
    ON_MESSAGE( UM_ON_SUBSCRIBE_CLOSE, OnSubscribeClose )
    ON_MESSAGE( UM_CONFERENCE_SERVER, OnConferenceServer )
    ON_MESSAGE( UM_CREATE_CHATROOM_SUCCESS, OnCreateChatRoomSuccess )
    ON_MESSAGE( UM_ON_INVITE, OnInvited )
    ON_MESSAGE( UM_CHATROOM_PRESENCE, OnChatRoomPresence )
    ON_MESSAGE( UM_CHATROOM_MESSAGE, OnChatRoomMessage )
    ON_MESSAGE( UM_ON_CHATROOM_DESTROY, OnChatRoomDestroy )
    ON_MESSAGE( UM_ON_FILE_OFFER, OnFileOffer )
    ON_MESSAGE( UM_ON_FILE_PROGRESS, OnFileProgress )
    ON_MESSAGE( UM_ON_FILE_COMPLETE, OnFileComplete )
    ON_MESSAGE( UM_ON_FILE_CANCEL, OnFileCancel )
    ON_MESSAGE( UM_ON_FILE_FAILURE, OnFileFailure )
    ON_MESSAGE( UM_ON_FILE_TRANSFER, OnFileTransfer )
    ON_MESSAGE( UM_ON_VCARD_INFO,    OnVCardInfo )
    ON_MESSAGE( UM_ON_PICTURE,       OnPictureMsg )
    ON_MESSAGE( UM_ON_CHATROOM_ROSTER_ADD, OnChatRoomRosterAdd )
    ON_MESSAGE( UM_ON_CHATROOM_ROSTER_DEL, OnChatRoomRosterDel )
    ON_MESSAGE( UM_ON_NOTIFY_PERSISTENT_CHATROOM, OnNotifyPersistentRooms )
    ON_MESSAGE( UM_ON_NOTIFY_CHAR_STATE, OnNotifyCharState )
    
    
    
	//}}AFX_MSG_MAP
    ON_CBN_SELCHANGE(IDC_COMBO_STATUS, &CMyTalkDlg::OnCbnSelchangeComboStatus)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_ROSTER, &CMyTalkDlg::OnNMDblclkTreeRoster)
    ON_NOTIFY(NM_RCLICK, IDC_TREE_ROSTER, &CMyTalkDlg::OnNMRClickTreeRoster)



    // Message map entry
    ON_NOTIFY(TVN_GETINFOTIP, IDC_TREE_ROSTER, &CMyTalkDlg::OnTvnGetInfoTip)

END_MESSAGE_MAP()

// Handler
void CMyTalkDlg::OnTvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;

    LPNMTVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);

    // Set your text here...
    if ( pNMHDR->hwndFrom == m_treeRoster.m_hWnd )
    {
        CPoint pt;
        GetCursorPos( &pt );
        m_treeRoster.ScreenToClient( &pt);

        TVHITTESTINFO t;
        t.pt = pt;
        m_treeRoster.HitTest( &t );

        if ( t.hItem )
        {
            HTREEITEM hItem = t.hItem;

            int nDepth = GetTreeDepth( m_treeRoster, hItem );
            if ( 2 == nDepth )
            {
                char buf[1024];

                LibJingleEx::PTMember pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData(hItem);
                _snprintf( buf, 1024, "Jid          : %s \n"
                                      "Subscription : %s \n"
                                      "Ask          : %s ", 
                    GetBareJid( pMember->tJid.sNode, pMember->tJid.sDomain ).c_str(),
                    LibJingleEx::GetSubscription(pMember->nSubscription),
                    LibJingleEx::GetAsk(pMember->nAsk) );

                pGetInfoTip->pszText = buf;    
            }
            else
            {
                pGetInfoTip->pszText = 0;
            }
        }
        
    }
    else
    {
        pGetInfoTip->pszText = 0;
    }
    

    *pResult = 0;
}

BOOL CMyTalkDlg::PreTranslateMessage(MSG* pMsg) 
{
    if ( pMsg->hwnd == m_treeRoster.m_hWnd )
    {
        if ( pMsg->message == WM_MOUSEMOVE )
        {
            m_tipRoster.RelayEvent( pMsg );
        }
    }
    return CDialog::PreTranslateInput(pMsg);
}

// CMyTalkDlg 消息处理程序

BOOL CMyTalkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

    LibJingleEx::TLibJingleEvent events;

    events.pOnLoginResult                  =  0; //OnLoginResult;
    events.pOnMessage                      =  0; //OnMessage;
    events.pOnRosterChange                 =  0; //::OnRosterChange;
    events.pOnRosterInfo                   =  0; //OnRosterInfo;
    events.pOnRosterPresenceChange         =  0; //OnRosterPresenceChange;
    events.pOnSubscribe                    =  0; //::OnSubscribe;
    events.pOnRosterAdd                    =  0; //::OnRosterAdd;
    events.pOnRosterDel                    =  0; //::OnRosterDel;

    events.pOnChatRoomService              =  0; //OnChatRoomService;
    events.pOnCreateUniqueRoom             =  0; //OnCreateUniqueRoom;
    events.pOnInvited                      =  0; //::OnInvited;
    events.pOnParticipantPresenceChange    =  0; //OnParticipantPresenceChange;
    events.pOnGrant                        =  0; //OnGrant;
    events.pOnRoomMessage                  =  0; //OnRoomMessage;
    events.pOnDestroyRoom                  =  0; //OnDestroyRoom;

    events.pOnFileOffer                    = 0; //::OnFileOffer;
    events.pOnFileTransfer                 = 0; //::OnFileTransfer;
    events.pOnFileComplete                 = 0; //::OnFileComplete;
    events.pOnFileCancel                   = 0; //::OnFileCancel;
    events.pOnFileFailure                  = 0; //::OnFileFailure;
    events.pOnFileProgress                 = 0; //::OnFileProgress;

    events.pOnVCardInfo                    = ::OnVCardInfo;

    LibJingleEx::ELibjingleError e = LibJingleEx::Init( &events );        
    if ( 0 != e )
    {
        MessageBox( "Failed to LibJingleEx::Init()! " );
        return TRUE;
    }

    e = LibJingleEx::Init( ::OnXmppMessage );        
    if ( 0 != e )
    {
        MessageBox( "Failed to LibJingleEx::Init2()! " );
        return TRUE;
    }

    

    std::vector<int>  vValidAccounts;

    try
    {
        char path[512] = {0};
        GetModuleFileName( 0, path, 512 );
        
        char * p = strrchr( path, '\\' );
        if ( p )
        {
            *p = '\0';
        }

        std::string sPath = path;
        sPath += '\\';
        sPath += CONFIG_FILE;


        g_Config.LoadFile( sPath );

        int i;
        for ( i = 0; i < MAX_ACCOUNT_NUMBER; i++ )
        {
            std::string sTmp;
            std::stringstream s;

            s << (i+1);

            sTmp  = CONFIG_USERNAME;
            sTmp += s.str();
            strcpy( g_tAccounts[i].sUserName,  g_Config[sTmp].c_str() );

            sTmp  = CONFIG_DOMAIN;
            sTmp += s.str();
            strcpy( g_tAccounts[i].sDomain,    g_Config[sTmp].c_str() );

            sTmp  = CONFIG_PASSWORD;
            sTmp += s.str();
            strcpy( g_tAccounts[i].sPassword,  g_Config[sTmp].c_str() );

            sTmp  = CONFIG_HOST;
            sTmp += s.str();
            strcpy( g_tAccounts[i].sHost,      g_Config[sTmp].c_str() );

            sTmp  = CONFIG_RESOURCE;
            sTmp += s.str();
            strcpy( g_tAccounts[i].sResource,  g_Config[sTmp].c_str() );

            sTmp  = CONFIG_USE_TLS;
            sTmp += s.str();
            g_tAccounts[i].bUseTls = g_Config.readInt(sTmp);

            
            sTmp  = CONFIG_STUN_ADDR;
            sTmp += s.str();
            strcpy( g_tAccounts[i].szStunAddr,  g_Config[sTmp].c_str() );

            sTmp  = CONFIG_RELAY_ADDR;
            sTmp += s.str();
            strcpy( g_tAccounts[i].szRelayAddr,  g_Config[sTmp].c_str() );

            sTmp  = CONFIG_SAVE_PATH;
            sTmp += s.str();
            strcpy( g_tAccounts[i].szDefaultSaveDir,  g_Config[sTmp].c_str() );

            sTmp  = CONFIG_STUN_PORT;
            sTmp += s.str();
            g_tAccounts[i].wStunPort = g_Config.readInt(sTmp);

            g_tAccounts[i].bFileShareEnable = TRUE;

            sTmp  = "enterprise";
            sTmp += s.str();

            if ( g_Config[sTmp].length() > 0  )
            {
                strcpy( g_tAccounts[i].sEnterpriseName, g_Config[sTmp].c_str() );
            }
            else
            {
                strcpy( g_tAccounts[i].sEnterpriseName, "kedacom" );
            }

            strcpy( g_tAccounts[i].szGroupChatPicSaveDir, "d:\\temp" );

            g_tAccounts[i].bSs5Proxy = FALSE;
            strcpy( g_tAccounts[i].szSs5Host, "192.168.1.99" );
            g_tAccounts[i].wSs5Port = 1080;
            strcpy( g_tAccounts[i].szSs5ProxyUsername, "admin" );
            strcpy( g_tAccounts[i].szSs5ProxyPassword, "123456" );

            if (    '\0' != g_tAccounts[i].sUserName[0] && '\0' != g_tAccounts[i].sDomain
                 && '\0' != g_tAccounts[i].sHost )
            {
                vValidAccounts.push_back( i );
            }
        }
    }
    catch ( MyConfigFile::file_not_found_exception e )
    {
        
    }

    // 有有效的账号
    if ( vValidAccounts.size() > 0  )
    {
        m_SubMenu.CreatePopupMenu();

        std::vector<int>::iterator it;
        for ( it = vValidAccounts.begin(); it != vValidAccounts.end(); it++)
        {
            int      nIndex = *it;
            UINT_PTR nId    = ID_START_ACCOUNT+MAX_MENU_COUNT+nIndex;

            m_SubMenu.AppendMenu( MF_STRING, nId, 
                                GetFullJid( g_tAccounts[nIndex].sUserName,
                                            g_tAccounts[nIndex].sDomain,
                                            g_tAccounts[nIndex].sResource ).c_str() );
        }

        GetMenu()->GetSubMenu(1)->ModifyMenu( 1, MF_BYPOSITION|MF_POPUP, 
                                              (UINT_PTR)m_SubMenu.m_hMenu, "启用账号" );
    }

    // m_comboStatus.InsertString( 0, "下线" );
    m_comboStatus.InsertString( 0, "远远离开" );
    m_comboStatus.InsertString( 1, "离开" );
    m_comboStatus.InsertString( 2, "不要打扰" );
    m_comboStatus.InsertString( 3, "在线" );
    m_comboStatus.InsertString( 4, "可以聊天" );

    m_comboStatus.SetCurSel( 3 );


    m_ImgList.Create( 16, 16, ILC_COLOR24, 16, 0 );

    CBitmap bm[6];

    bm[0].LoadBitmap(IDB_OFF);
    m_ImgList.Add( &bm[0], RGB(0, 0, 0) );

    bm[1].LoadBitmap(IDB_XA);
    m_ImgList.Add( &bm[1], RGB(0, 0, 0) );

    bm[2].LoadBitmap(IDB_AWAY);
    m_ImgList.Add( &bm[2], RGB(0, 0, 0) );

    bm[3].LoadBitmap(IDB_BUSY);
    m_ImgList.Add( &bm[3], RGB(0, 0, 0) );

    bm[4].LoadBitmap(IDB_AVAILABLE);
    m_ImgList.Add( &bm[4], RGB(0, 0, 0) );

    m_ImgList.Add( &bm[4], RGB(0, 0, 0) );                         // 第6个

    bm[5].LoadBitmap(IDB_FOLDER);
    m_ImgList.Add( &bm[5], RGB(0, 0, 0) );                         // 第7个

    m_treeRoster.SetImageList( &m_ImgList, TVSIL_NORMAL );


    m_OnSubscribeDlg.Create( IDD_ON_SUBSCRIBE, this );
    // m_OnSubscribeDlg.ShowWindow( SW_SHOW );




    this->EnableToolTips( TRUE );
    m_treeRoster.EnableToolTips( TRUE );

    if ( m_tipRoster.Create( this ) )
    {
        if ( m_tipRoster.AddTool( &m_treeRoster, LPSTR_TEXTCALLBACK, CRect(0,0,10000,10000), IDC_TREE_ROSTER ) )
        {
            m_treeRoster.SetToolTips(&m_tipRoster);    
            m_tipRoster.SetDelayTime(1000);
        }   
        else
        {
            MessageBox("Failed to AddTool! ");
        }
    }
    else
    {
        MessageBox("Failed to create tip control! ");
    }


    // SetTimer( 1, 10000, 0 );

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

BOOL CMyTalkDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
    WORD wId = LOWORD(wParam);
    // 启动账号
    if (     wId >= ID_START_ACCOUNT + MAX_MENU_COUNT 
          && wId <  ID_START_ACCOUNT + MAX_MENU_COUNT + MAX_ACCOUNT_NUMBER )
    {
        OnStartAccount( wId - (ID_START_ACCOUNT + MAX_MENU_COUNT)  );
        return TRUE;
    }
    else if (    wId >= ID_START_ACCOUNT + MAX_MENU_COUNT + MAX_ACCOUNT_NUMBER
              && wId <  ID_START_ACCOUNT + MAX_MENU_COUNT + MAX_ACCOUNT_NUMBER * 2 )
    {
        OnStopAccount( wId - (ID_START_ACCOUNT + MAX_MENU_COUNT + MAX_ACCOUNT_NUMBER) );
        return TRUE;
    }
    else if ( ID_TEST == wId )
    {
        OnTest();
    }
    else if ( ID_TEST_MISC == wId )
    {
        OnTestMisc();
    }


    return CDialog::OnCommand(wParam,lParam);
}

void CMyTalkDlg::OnStartAccount( int nIndex )
{
    LibJingleEx::ELibjingleError e;

    // ASSERT( 0 == g_hXmppApp[nIndex] );
    // g_tAccounts[nIndex].bUseCompression = TRUE;
    strcpy( g_tAccounts[nIndex].szConfigPath,           "E:\\KDVP_daily\\b0-thirdparty\\libjingle-0.4.0\\talk" );
    strcpy( g_tAccounts[nIndex].szGroupChatPicSaveDir,  "E:\\KDVP_daily\\b0-thirdparty\\libjingle-0.4.0\\talk" );

    //strcpy( g_tAccounts[nIndex].sUserName, "93a363b9-d3fa-4bbe-bdc6-0c030b97ea1" );
    //strcpy( g_tAccounts[nIndex].sDomain  , "A#B" );

    e = LibJingleEx::Login( &g_hXmppApp[nIndex], &g_tAccounts[nIndex], (void *)nIndex );
    
    if ( 0 != e )
    {
        MessageBox( "Failed to LibJingleEx::Login()! " );
        return;
    }

    // 正在连接中又重连
    if ( 1 == m_nStarting[nIndex] )
    {
        // 记录重连的状态
        m_nStarting[nIndex] = 2;
    }
    else
    {
        // 状态为正在重连
        m_nStarting[nIndex] = 1;
    }
}

void CMyTalkDlg::OnStopAccount(  int nIndex )
{
    if ( 0 == g_hXmppApp[nIndex] )
    {
        CString s;
        s.Format( "ERROR OnStopAccount( nIndex = %d) ", nIndex  );

        MessageBox(s );
        return;
    }

    LibJingleEx::Logout( g_hXmppApp[nIndex] );
    g_hXmppApp[nIndex] = 0;
}

void CMyTalkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyTalkDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyTalkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMyTalkDlg::OnAccount()
{
    m_AccountDlg.DoModal();

    int i;
    std::vector<int> v; 
    for ( i = 0; i < MAX_ACCOUNT_NUMBER; i++ )
    {

        if (    '\0' != g_tAccounts[i].sUserName[0] && '\0' != g_tAccounts[i].sDomain
            &&  '\0' != g_tAccounts[i].sHost )
        {
            v.push_back( i );
        }
    }

    CMenu * pAccountMenu = GetMenu()->GetSubMenu(1);
    UINT nCnt = pAccountMenu->GetMenuItemCount();

    for ( i = 3; i < (int)nCnt; i++ )
    {
        CMenu * pMenu = pAccountMenu->GetSubMenu(i);

        std::vector<int>::iterator it;
        for ( it = v.begin(); it != v.end(); it++ )
        {
            if ( pMenu->GetMenuItemID( 0 ) - (ID_START_ACCOUNT + MAX_MENU_COUNT + MAX_ACCOUNT_NUMBER) == (*it)  )
            {
                v.erase( it );
                break;
            }
        }
    }

    // 有有效的账号
    if ( v.size() > 0  )
    {
        if ( ::IsMenu( m_SubMenu.m_hMenu ) )
        {
            m_SubMenu.DestroyMenu();
        }

        m_SubMenu.CreatePopupMenu();
        
        std::vector<int>::iterator it;
        for ( it = v.begin(); it != v.end(); it++)
        {
            int      nIndex = *it;
            UINT_PTR nId    = ID_START_ACCOUNT+MAX_MENU_COUNT+nIndex;

            m_SubMenu.AppendMenu( MF_STRING, nId, 
                GetFullJid( g_tAccounts[nIndex].sUserName,
                g_tAccounts[nIndex].sDomain,
                g_tAccounts[nIndex].sResource ).c_str() );
        }

        pAccountMenu->ModifyMenu( 1, MF_BYPOSITION|MF_POPUP, 
            (UINT_PTR)m_SubMenu.m_hMenu, "启用账号" );
    }
}

void CMyTalkDlg::OnXmppConsole()
{
    if ( 0 != g_pXmppDlg )
    {
        if ( g_pXmppDlg->IsWindowVisible() )
        {
            return;
        }
        else
        {
            g_pXmppDlg->DestroyWindow();
            delete g_pXmppDlg;
            g_pXmppDlg = 0;
        }
    }

    g_pXmppDlg = new CXmppDlg;
    g_pXmppDlg->Create( IDD_XMPP_MSG, GetDesktopWindow() );

    SetXmppConsole( g_pXmppDlg->m_hWnd );

    g_pXmppDlg->ShowWindow( SW_SHOW );
}

void CMyTalkDlg::OnAddFriend()
{
    CAddFriendDlg dlg;

    std::vector<int> & v = dlg.m_vInt;

    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        int nIndex = m_treeRoster.GetItemData( hItem ) - 1;
        v.push_back( nIndex );
        hItem = m_treeRoster.GetNextSiblingItem( hItem );
    }

    if ( 0 == v.size() )
    {
        MessageBox("没有账号登录");
        return;
    }

    dlg.DoModal();
}

void CMyTalkDlg::OnDelFriend()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    if ( 0 == hItem )
    {
        return;
    }

    int n = GetTreeDepth( m_treeRoster, hItem );

    if ( 2 == n )
    {
        if (  IDYES != MessageBox("确定删除好友吗？", "删除好友", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION )  )
        {
            return;
        }

        HTREEITEM hGroup = m_treeRoster.GetParentItem( hItem );
        ASSERT( hGroup );

        CString strGroupName = m_treeRoster.GetItemText(hGroup);

        HTREEITEM hRoot  = m_treeRoster.GetParentItem( hGroup );
        ASSERT( hRoot );

        int nIndex = m_treeRoster.GetItemData(hRoot) - 1;

        LibJingleEx::PTMember  pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData( hItem );

        LibJingleEx::ELibjingleError e = LibJingleEx::DelFriend( g_hXmppApp[nIndex], &pMember->tJid, strGroupName, FALSE  );
        if ( 0 != e )
        {
            MessageBox("Failed to LibJingleEx::DelFriend()! ");
        }
    }
}

void CMyTalkDlg::OnDebugInfo()
{
    CSelAccountDlg dlg;
    
    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        int nIndex = m_treeRoster.GetItemData( hItem ) - 1;
        dlg.m_v.push_back( nIndex );
    
        hItem = m_treeRoster.GetNextSiblingItem(hItem);
    }

    if ( IDOK == dlg.DoModal() )
    {
        LibJingleEx::PrintDebugInfo( g_hXmppApp[dlg.m_nIndex] );
    }    
}

void CMyTalkDlg::OnSetNick()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    if ( 0 == hItem )
    {
        return;
    }

    int n = GetTreeDepth( m_treeRoster, hItem );

    if ( 2 == n )
    {
        HTREEITEM hGroup = m_treeRoster.GetParentItem( hItem );
        ASSERT( hGroup );

        HTREEITEM hRoot  = m_treeRoster.GetParentItem( hGroup );
        ASSERT( hRoot );

        int nIndex = m_treeRoster.GetItemData(hRoot) - 1;

        LibJingleEx::PTMember  pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData( hItem );

        CSetNickDlg dlg;
        dlg.m_nIndex = nIndex;
        memcpy( &dlg.m_tJid, &pMember->tJid, sizeof(LibJingleEx::TJid) ); 
        dlg.m_sOriNick = pMember->szName;
        dlg.DoModal();
    }
}

void CMyTalkDlg::OnSendSubscribe()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    int nDepth = GetTreeDepth(m_treeRoster, hItem);

    if ( 2 == nDepth )
    {
        HTREEITEM hGroup = m_treeRoster.GetParentItem(hItem);
        ASSERT( hGroup );

        HTREEITEM hRoot = m_treeRoster.GetParentItem(hGroup);
        ASSERT( hRoot );

        int nIndex = m_treeRoster.GetItemData(hRoot) - 1;

        LibJingleEx::PTMember pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData(hItem);
    
        LibJingleEx::ELibjingleError e = LibJingleEx::Subscribe( g_hXmppApp[nIndex], &pMember->tJid );

        if ( 0 != e )
        {
            MessageBox( "Failed to LibJingleEx::Subscribe() !" );
        }
        
    }
}

void CMyTalkDlg::OnSendUnsubscribe()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    int nDepth = GetTreeDepth(m_treeRoster, hItem);

    if ( 2 == nDepth )
    {
        if ( IDYES != MessageBox("确定要取消关注吗？", "取消关注", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) )
        {
            return;
        }

        HTREEITEM hGroup = m_treeRoster.GetParentItem(hItem);
        ASSERT( hGroup );

        HTREEITEM hRoot = m_treeRoster.GetParentItem(hGroup);
        ASSERT( hRoot );

        int nIndex = m_treeRoster.GetItemData(hRoot) - 1;

        LibJingleEx::PTMember pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData(hItem);

        LibJingleEx::ELibjingleError e = LibJingleEx::Unsubscribe( g_hXmppApp[nIndex], &pMember->tJid );

        if ( 0 != e )
        {
            MessageBox( "Failed to LibJingleEx::Unsubscribe() !" );
        }

    }
}

void CMyTalkDlg::OnSendUnsubscribed()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    int nDepth = GetTreeDepth(m_treeRoster, hItem);

    if ( 2 == nDepth )
    {
        if ( IDYES != MessageBox("确定要取消被关注吗？您的好友将无法查看您的状态更新", "取消被关注", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) )
        {
            return;
        }

        HTREEITEM hGroup = m_treeRoster.GetParentItem(hItem);
        ASSERT( hGroup );

        HTREEITEM hRoot = m_treeRoster.GetParentItem(hGroup);
        ASSERT( hRoot );

        int nIndex = m_treeRoster.GetItemData(hRoot) - 1;

        LibJingleEx::PTMember pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData(hItem);

        LibJingleEx::ELibjingleError e = LibJingleEx::Unsubscribed( g_hXmppApp[nIndex], &pMember->tJid );

        if ( 0 != e )
        {
            MessageBox( "Failed to LibJingleEx::Unsubscribed() !" );
        }

    }
}

void CMyTalkDlg::OnCreateChatRoom()
{
    CCreateChatRoomDlg dlg;

    std::vector<int> & v = dlg.m_vInt;

    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        int nIndex = m_treeRoster.GetItemData( hItem ) - 1;
        if ( g_bConferenceAvailable[nIndex] )
        {
            v.push_back( nIndex );
        }
        hItem = m_treeRoster.GetNextSiblingItem( hItem );
    }

    if ( 0 == v.size() )
    {
        MessageBox("没有账号登录或登录的账号没有Conference服务！");
        return;
    }

    dlg.DoModal();
}

void CMyTalkDlg::OnActivate( UINT nState, CWnd* pWndOther,BOOL bMinimized )
{
    CDialog::OnActivate( nState, pWndOther, bMinimized );

}

void CMyTalkDlg::OnDestroy()
{
    int i;
    for ( i = 0; i < MAX_ACCOUNT_NUMBER; i++ )
    {
        if ( g_hXmppApp[i] )
        {
            LibJingleEx::Logout( g_hXmppApp[i] );
            g_hXmppApp[i] = 0;
        }

        vChatDlg::iterator it;
        vChatDlg & v = m_vChatDlgs[i];

        for ( it = v.begin(); it != v.end(); it++ )
        {
            CChatDlg * pChatDlg = *it;

            pChatDlg->DestroyWindow();
            delete pChatDlg;
        }

        v.clear();

        {
            vPictureDlg::iterator it;
            vPictureDlg & v = m_vPictureDlgs[i];

            for ( it = v.begin(); it != v.end(); it++ )
            {
                CPictureDlg * pPictureDlg = *it;

                pPictureDlg->DestroyWindow();
                delete pPictureDlg;
            }

            v.clear();
        }

        vChatRoom::iterator it_cr;
        vChatRoom & vcr = g_vChatRooms[i];

        for ( it_cr = vcr.begin(); it_cr != vcr.end(); it_cr++ )
        {
            PTChatRoom p = *it_cr;
            delete p;
        }
        vcr.clear();

        vFileTranferDlg & f = m_vFileTranferDlgs[i];
        vFileTranferDlg::iterator ifv;
        for ( ifv = f.begin(); ifv != f.end(); ifv++ )
        {
            CFileTranferDlg * p = *ifv;
            p->DestroyWindow();
            delete p;
        }
        f.clear();
    }

    if ( g_pXmppDlg )
    {
        g_pXmppDlg->DestroyWindow();
        delete g_pXmppDlg;
        g_pXmppDlg = 0;
    }

    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        HTREEITEM hTmp = hItem;
        hItem = m_treeRoster.GetNextSiblingItem(hItem);
        DeleteItemInTree( m_treeRoster, hTmp );
    }

    vOnSubscribeMsg::iterator it_s;
    for ( it_s = m_vOnSubscribeMsg.begin(); it_s != m_vOnSubscribeMsg.end(); it_s++ )
    {
        PTOnSubscribeMsg p = *it_s;

        delete p->m_ptJid;
        delete p;
    }
    m_vOnSubscribeMsg.clear();

    m_OnSubscribeDlg.DestroyWindow();

    if ( 0 != g_pTestDlg )
    {
        g_pTestDlg->DestroyWindow();
        delete g_pTestDlg;
        g_pTestDlg = 0;
    }

    CDialog::OnDestroy();
}

void CMyTalkDlg::OnSize(UINT nType, int cx, int cy)
{
    if ( ::IsWindow( m_comboStatus.m_hWnd ) )
    {
        if ( !m_OnSubscribeDlg.IsWindowVisible() )
        {
            CRect r1( 0, 0, cx,cy - 30 );
            m_treeRoster.MoveWindow( r1 );

            CRect r2;
            r2.left   = r1.left;
            r2.right  = r1.right;
            r2.top    = r1.bottom + 5;
            r2.bottom = r2.top + 20;

            m_comboStatus.MoveWindow( r2 );
        }
        else
        {
            CRect r1( 0, 0, cx,cy - 90 );
            m_treeRoster.MoveWindow( r1 );

            CRect r2;
            r2.left   = r1.left + 2;
            r2.right  = r1.right - 2;
            r2.top    = r1.bottom + 3;
            r2.bottom = r2.top + 60;

            m_OnSubscribeDlg.MoveWindow( r2 );

            CRect r3;
            r3.left    = r1.left;
            r3.right   = r1.right;
            r3.top     = r2.bottom + 3;
            r3.bottom  = r3.top + 20;

            m_comboStatus.MoveWindow( r3 );
        }
    }
    CDialog::OnSize(nType, cx, cy );
}

LRESULT CMyTalkDlg::OnState( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp            = (LibJingleEx::HANDLE)wParam;

    void ** pTmp = (void **)lParam;

    LibJingleEx::ELibjingleState eState = (LibJingleEx::ELibjingleState)(int)pTmp[0];
    LibJingleEx::ELibjingleStateSubCode eSubCode = (LibJingleEx::ELibjingleStateSubCode)(int)pTmp[1];
    int nIndex = (int)pTmp[2];

    delete[] pTmp;

    ASSERT( nIndex < MAX_ACCOUNT_NUMBER );

    LibJingleEx::TLoginSettings & account = g_tAccounts[nIndex];

    // 登录
    if ( LibJingleEx::STATE_OPEN == eState )
    {
        // 改变记录状态
        m_nStarting[nIndex] = 0;


        int nSel = m_comboStatus.GetCurSel();
        ASSERT( nSel <= 5 );

        LibJingleEx::EPresence status = ( LibJingleEx::EPresence )(nSel+2);

        LibJingleEx::ELibjingleError e = LibJingleEx::SetStatus( hApp, status );
        if ( 0 != e )
        {
            MessageBox("failed to set status!");
        }

        ////////////////////////////// 界面

        HTREEITEM hItem = m_treeRoster.InsertItem( GetFullJid( account.sUserName, account.sDomain, account.sResource ).c_str(), 
                                 nSel+1, nSel+1, TVI_ROOT  );
        m_treeRoster.SetItemData( hItem, (DWORD_PTR)(nIndex+1) );
        
        CMenu * pAccountMenu = GetMenu()->GetSubMenu(1);

        if ( pAccountMenu->GetMenuItemCount() <= 2 )
        {
            pAccountMenu->AppendMenu( MF_SEPARATOR );
        }

        if ( !::IsMenu( m_AccountMenu[nIndex].m_hMenu ) )
        {
            m_AccountMenu[nIndex].CreatePopupMenu();

            UINT_PTR nId = ID_START_ACCOUNT + (MAX_MENU_COUNT + MAX_ACCOUNT_NUMBER)
                + nIndex;

            m_AccountMenu[nIndex].AppendMenu( MF_STRING, nId, "禁用" );
        }

        pAccountMenu->AppendMenu( MF_POPUP, (UINT_PTR)m_AccountMenu[nIndex].m_hMenu, 
            GetFullJid( account.sUserName, account.sDomain, account.sResource ).c_str()  );

        m_SubMenu.RemoveMenu( ID_START_ACCOUNT + MAX_MENU_COUNT + nIndex, MF_BYCOMMAND );

        e = LibJingleEx::SetReady( hApp );
        if ( 0 != e )
        {
            MessageBox("Failed to LibJingleEx::SetReady()! \n");
        }
    }
    // 登出
    else if ( LibJingleEx::STATE_CLOSED == eState )
    {
        // 如果是被动断开连接( 非Logout )
        //if ( LibJingleEx::STATE_SUBCODE_NONE != eSubCode )
        {
            LibJingleEx::Logout( hApp );
        }

        HTREEITEM hItem = m_treeRoster.GetRootItem();
        while( hItem )
        {
            int nTmp = m_treeRoster.GetItemData( hItem );
            if ( nTmp == nIndex + 1 )
            {
                DeleteItemInTree( m_treeRoster, hItem );
                break;
            }
            hItem = m_treeRoster.GetNextSiblingItem( hItem );
        }

        vOnSubscribeMsg::iterator it_s;
        vOnSubscribeMsg & vs = m_vOnSubscribeMsg;

        for ( it_s = vs.begin(); it_s != vs.end();)
        {
            PTOnSubscribeMsg p = *it_s;
            if ( p->m_hApp == hApp )
            {
                delete p->m_ptJid;
                delete p;

                it_s = vs.erase( it_s );
            }
            else
            {
                it_s++;
            }
        }

        if ( m_OnSubscribeDlg.IsWindowVisible() && m_OnSubscribeDlg.m_hApp == hApp )
        {
            if ( vs.size() > 0 )
            {
                it_s = vs.begin();
                PTOnSubscribeMsg p = *it_s;

                m_OnSubscribeDlg.m_hApp = p->m_hApp;
                memcpy( &m_OnSubscribeDlg.m_tJid, p->m_ptJid, sizeof(LibJingleEx::TJid) );
                m_OnSubscribeDlg.UpdateMsg();

            }
            else
            {
                CRect rectClient;
                this->GetClientRect( rectClient );

                int cx = rectClient.Width();
                int cy = rectClient.Height();

                CRect r1( 0, 0, cx,cy - 30 );
                m_treeRoster.MoveWindow( r1 );

                CRect r2;
                r2.left   = r1.left;
                r2.right  = r1.right;
                r2.top    = r1.bottom + 5;
                r2.bottom = r2.top + 20;

                m_comboStatus.MoveWindow( r2 );

                m_OnSubscribeDlg.ShowWindow( SW_HIDE );
            }
        }
        

        /////////////////  销毁对话框
        vChatDlg & v = m_vChatDlgs[nIndex];
        vChatDlg::iterator it;
        for ( it = v.begin(); it != v.end(); it++ )
        {
            CChatDlg * pChatDlg = *it;

            pChatDlg->DestroyWindow();
            delete pChatDlg;
        }
        v.clear();
        /////////////////  结束 销毁对话框

        {
            /////////////////  销毁图片框
            vPictureDlg & v = m_vPictureDlgs[nIndex];
            vPictureDlg::iterator it;
            for ( it = v.begin(); it != v.end(); it++ )
            {
                CPictureDlg * pDlg = *it;

                pDlg->DestroyWindow();
                delete pDlg;
            }
            v.clear();
            /////////////////  结束 销毁图片框
        }

        g_bConferenceAvailable[nIndex] = FALSE;


        

        vChatRoom::iterator it_cr;
        vChatRoom & vcr = g_vChatRooms[nIndex];

        for ( it_cr = vcr.begin(); it_cr != vcr.end(); it_cr++ )
        {
            PTChatRoom p = *it_cr;
            delete p;
        }
        vcr.clear();


        vFileTranferDlg & f = m_vFileTranferDlgs[nIndex];
        vFileTranferDlg::iterator ifv;
        for ( ifv = f.begin(); ifv != f.end(); ifv++ )
        {
            CFileTranferDlg * p = *ifv;
            p->DestroyWindow();
            delete p;
        }
        f.clear();



        ////////////////////////////// 界面
        CMenu * pAccountMenu = GetMenu()->GetSubMenu(1);

        UINT nCnt = pAccountMenu->GetMenuItemCount();
        UINT i;

        BOOL bFindMenu = FALSE;
        for ( i = 3; i < nCnt; i++ )
        {
            CMenu * pMenu = pAccountMenu->GetSubMenu(i);

            if ( pMenu )
            {
                if ( pMenu->GetMenuItemID( 0 ) ==  ID_START_ACCOUNT + MAX_MENU_COUNT + MAX_ACCOUNT_NUMBER + nIndex )
                {
                    pMenu->DestroyMenu();
                    pAccountMenu->DeleteMenu( i, MF_BYPOSITION );
                    bFindMenu = TRUE;
                    break;
                }
            }
        }

        if ( 3 == pAccountMenu->GetMenuItemCount() )
        {
            pAccountMenu->DeleteMenu( 2, MF_BYPOSITION );
        }

        if ( '\0' != account.sUserName[0] && '\0' != account.sDomain[nIndex] )
        {
            if ( bFindMenu )
            {
                if ( !::IsMenu(m_SubMenu.m_hMenu) )
                {
                    m_SubMenu.CreatePopupMenu();
                    pAccountMenu->ModifyMenu( 1, MF_BYPOSITION|MF_POPUP, 
                        (UINT_PTR)m_SubMenu.m_hMenu, "启用账号" );
                }

                m_SubMenu.AppendMenu( MF_STRING, ID_START_ACCOUNT + MAX_MENU_COUNT + nIndex, 
                    GetFullJid( account.sUserName, account.sDomain, account.sResource ).c_str() );
            }
        }

        // 如果正在登陆（一直没有登录成功）
        if ( 1 == m_nStarting[nIndex] )
        {
            CString sTmp;
            sTmp.Format("%s登录服务器失败！", 
                       GetFullJid( account.sUserName, account.sDomain, account.sResource ).c_str() );
            MessageBox(sTmp);

            m_nStarting[nIndex] = 0;
        }
        else if ( 2 == m_nStarting[nIndex] )
        {
            m_nStarting[nIndex] = 1;
        }
        else if ( 0 == m_nStarting[nIndex] )
        {
            if ( 0 != eSubCode )
            {
                CString sTmp;
                sTmp.Format( "error: %d", eSubCode );
                // MessageBox( sTmp );
            }
        }


        


        // 重连
        //if ( LibJingleEx::STATE_SUBCODE_NONE != eSubCode )
        //{
        //    LibJingleEx::Login( &g_hXmppApp[nIndex], &g_tAccounts[nIndex], (void *)nIndex  );
        //}



    }
    return 0;
}
void CMyTalkDlg::OnCbnSelchangeComboStatus()
{
    // TODO: 在此添加控件通知处理程序代码
    int nSel = m_comboStatus.GetCurSel();

    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        m_treeRoster.SetItemImage( hItem, nSel+1, nSel+1);

        int nIndex = ((DWORD_PTR)m_treeRoster.GetItemData( hItem ) - 1);
        LibJingleEx::ELibjingleError e = LibJingleEx::SetStatus( g_hXmppApp[nIndex], LibJingleEx::EPresence(nSel+2) );
        if ( 0 != e )
        {
            MessageBox("failed to set status!");
        }

        hItem = m_treeRoster.GetNextSiblingItem(hItem);
    }
}

LRESULT CMyTalkDlg::OnRoster( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE   hApp        = (LibJingleEx::HANDLE)wParam;
    int nIndex                        = (int)LibJingleEx::GetUserData(hApp);
    LibJingleEx::PTGroup  pGroup      = LibJingleEx::GetRoster( hApp );

    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        int nTmp = (DWORD_PTR)m_treeRoster.GetItemData(hItem) - 1;
        if ( nTmp == nIndex )
        {
            break;
        }
        hItem = m_treeRoster.GetNextSiblingItem( hItem );
    }

    // 找到
    if ( hItem )
    {
        while( pGroup )
        {
            HTREEITEM hGroup = m_treeRoster.GetChildItem(hItem);
            while(hGroup)
            {
                CString s = m_treeRoster.GetItemText(hGroup);
                // 找到组
                if ( s == pGroup->szName )
                {
                    break;
                }
                hGroup = m_treeRoster.GetNextSiblingItem(hGroup);
            }

            // 没找到
            if ( 0 == hGroup )
            {
                hGroup = m_treeRoster.InsertItem( pGroup->szName, 6, 6, hItem );
            }

            ASSERT( 0 == m_treeRoster.GetChildItem(hGroup) );

            LibJingleEx::PTMember pMember = pGroup->pChild;
            while( pMember )
            {
                HTREEITEM hMember = 0;
                LibJingleEx::EPresence ePres = LibJingleEx::GetFriendPresence(hApp, &pMember->tJid);
                int nTmp = GetImgIndex( ePres );

                if ( '\0' != pMember->szName[0])
                {
                    hMember = m_treeRoster.InsertItem( pMember->szName, nTmp, nTmp, hGroup );
                }
                else
                {
                    hMember = m_treeRoster.InsertItem( GetFullJid(pMember->tJid.sNode, pMember->tJid.sDomain, pMember->tJid.sResource).c_str(), nTmp, nTmp, hGroup );
                }

                LibJingleEx::PTMember pNew = new LibJingleEx::TMember;
                memcpy( pNew, pMember, sizeof(LibJingleEx::TMember) );

                m_treeRoster.SetItemData( hMember, (DWORD_PTR)pNew );

                pMember = pMember->pNext;
            }

            m_treeRoster.Expand( hGroup, TVE_EXPAND );

            pGroup = pGroup->pNext;
        }

        m_treeRoster.Expand( hItem, TVE_EXPAND );
    }
    // 没有找到
    else
    {
        ASSERT( 0 );
    }

    

    return 0;
}

void CMyTalkDlg::OnNMDblclkTreeRoster(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    if ( 0 == hItem )
    {
        return;
    }

    int n = GetTreeDepth( m_treeRoster, hItem );
    if ( 2 == n )
    {
        HTREEITEM hRoot = GetTreeTop( m_treeRoster, hItem );
        ASSERT( hRoot );

        int nIndex = m_treeRoster.GetItemData( hRoot ) - 1;
        ASSERT( nIndex < MAX_ACCOUNT_NUMBER );

        vChatDlg & v = m_vChatDlgs[nIndex];

        LibJingleEx::PTMember pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData( hItem );

        LibJingleEx::PTJid ptJid = &pMember->tJid;

        vChatDlg::iterator it = find_if( v.begin(), v.end(), FindChatDlgObj( ptJid ) );

        CChatDlg * pChatDlg = 0;
        // 没有
        if ( it == v.end() )
        {
            pChatDlg = new CChatDlg;
            memcpy( &pChatDlg->m_tJid, ptJid, sizeof(LibJingleEx::TJid) );
            pChatDlg->m_sNick = pMember->szName;
            pChatDlg->m_nIndex = nIndex;

            pChatDlg->Create( IDD_CHAT, GetDesktopWindow() );
            
            v.push_back( pChatDlg );
        }
        // 有
        else
        {
            pChatDlg = *it;
        }

        pChatDlg->ShowWindow( SW_SHOW );
    }
}

LRESULT CMyTalkDlg::OnChatMsg( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp = (LibJingleEx::HANDLE)wParam;
    void ** pTmp             = (void **)lParam;

    LibJingleEx::PTJid ptJid = (LibJingleEx::PTJid)pTmp[0];
    char *             pMsg  = (char*)pTmp[1];
    delete[] pTmp;

    int nIndex = (int)LibJingleEx::GetUserData(hApp);
    ASSERT( nIndex < MAX_ACCOUNT_NUMBER );

    ptJid->sResource[0] = '\0';

    LibJingleEx::PTMember pMember = GetMemberInfo( hApp, ptJid );
    
    vChatDlg & v = m_vChatDlgs[nIndex];
    vChatDlg::iterator it = find_if( v.begin(), v.end(), FindChatDlgObj( ptJid ) );

    CChatDlg * pChatDlg = 0;
    // 没有
    if ( it == v.end() )
    {
        pChatDlg = new CChatDlg;
        memcpy( &pChatDlg->m_tJid, ptJid, sizeof(LibJingleEx::TJid) );
        if ( pMember )
        {
            pChatDlg->m_sNick = pMember->szName;
        }
        pChatDlg->m_nIndex = nIndex;

        pChatDlg->Create( IDD_CHAT, GetDesktopWindow() );
        v.push_back( pChatDlg );
    }
    // 有
    else
    {
        pChatDlg = *it;
    }

    pChatDlg->OnChatMessage( pMsg );
    pChatDlg->ShowWindow( SW_SHOW );

    delete[] pMsg;
    delete ptJid;
    return 0;
}

LRESULT CMyTalkDlg::OnPresence( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp          = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::PTPresence pPresence = (LibJingleEx::PTPresence)lParam;

    int nIndex = (int)LibJingleEx::GetUserData(hApp);
    ASSERT( nIndex < MAX_ACCOUNT_NUMBER );

    LibJingleEx::EPresence ePres = LibJingleEx::GetFriendPresence(hApp, &pPresence->tJid);


    LibJingleEx::TJid tTmpJid;
    memcpy( &tTmpJid, &pPresence->tJid, sizeof(LibJingleEx::TJid) );

    pPresence->tJid.sResource[0] = '\0';

    std::vector<HTREEITEM> v;
    FindFriendInTree( m_treeRoster, nIndex, &pPresence->tJid, v );

    std::vector<HTREEITEM>::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        HTREEITEM hItem = *it;

        int nTmp = GetImgIndex(ePres);
        m_treeRoster.SetItemImage( hItem, nTmp, nTmp );
    }

    LibJingleEx::TJid tSelfJid;
    LibJingleEx::GetSelfJid(hApp, &tSelfJid);

    // 如果是自己
    if ( _IfJidEqual(tSelfJid, tTmpJid) )
    {
        if ( pPresence->ePresence != LibJingleEx::PRESENCE_NONE )
        {
            // 更新chatroom
            vChatRoom & v = g_vChatRooms[nIndex];
            vChatRoom::iterator it;
            for ( it = v.begin(); it != v.end(); it++ )
            {
                PTChatRoom p = *it;
                
                p->Update( &tSelfJid, &pPresence->ePresence, 0 );
                
                ASSERT( p->m_ChatRoom );
                CChatRoomDlg * pDlg = (CChatRoomDlg *)p->m_ChatRoom;
                
                pDlg->UpdateParticipant( &tSelfJid, &pPresence->ePresence, 0 );
            }
        }
    }
    else
    {
        if ( pPresence->ePresence == LibJingleEx::PRESENCE_NONE )
        {
            vFileTranferDlg & f = m_vFileTranferDlgs[nIndex];
            vFileTranferDlg::iterator ifv;

            for ( ifv = f.begin(); ifv != f.end(); ifv++ )
            {
                CFileTranferDlg * p = *ifv;
                if ( _IfJidEqual( p->m_tPeerJid, tTmpJid )  )
                {
                    p->DestroyWindow();
                    delete p;
                    f.erase( ifv );
                    break;
                }
            }
        }
    }
    delete pPresence;
    return 0;
}

LRESULT CMyTalkDlg::OnRosterAdd( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp          = (LibJingleEx::HANDLE)wParam;
    void ** pTmp                      = (void **)lParam;

    char * pGroupName                 = (char*)pTmp[0];
    LibJingleEx::PTJid pJid           = (LibJingleEx::PTJid)pTmp[1];
    LibJingleEx::PTMemberItem pChange = (LibJingleEx::PTMemberItem)pTmp[2];

    int nIndex = (int)LibJingleEx::GetUserData(hApp);
    ASSERT( nIndex < MAX_ACCOUNT_NUMBER );

    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        int nTmp = m_treeRoster.GetItemData(hItem) - 1;
        if ( nTmp == nIndex )
        {
            break;
        }
        hItem = m_treeRoster.GetNextSiblingItem(hItem);
    }

    ASSERT( 0 != hItem );

    HTREEITEM hGroup = m_treeRoster.GetChildItem( hItem );
    while( hGroup )
    {
        CString s = m_treeRoster.GetItemText(hGroup );
        if ( s == pGroupName )
        {
            break;
        }
        hGroup = m_treeRoster.GetNextSiblingItem( hGroup );
    }

    // 没有就添加
    if ( 0 == hGroup )
    {
        hGroup = m_treeRoster.InsertItem( pGroupName, 6,6, hItem  );
    }
    
    CString sNick;
    if ( '\0' != pChange->szName[0] )
    {
        sNick = pChange->szName;
    }
    else
    {
        sNick = GetFullJid( pJid->sNode, pJid->sDomain, pJid->sResource ).c_str();
    }

    LibJingleEx::EPresence ePres = LibJingleEx::GetFriendPresence( hApp, pJid );
    int nImage = GetImgIndex( ePres );

    HTREEITEM hFriend = m_treeRoster.InsertItem( sNick, nImage, nImage, hGroup );
    LibJingleEx::PTMember pMember = new LibJingleEx::TMember;
    memcpy( &pMember->tJid, pJid, sizeof(LibJingleEx::TJid) );
    strcpy( pMember->szName, pChange->szName );
    pMember->nSubscription = pChange->nSubscription;
    pMember->nAsk          = pChange->nAsk;
    m_treeRoster.SetItemData( hFriend, (DWORD_PTR)pMember );

    m_treeRoster.Expand( hItem,  TVE_EXPAND );
    m_treeRoster.Expand( hGroup, TVE_EXPAND );


    delete[] pTmp;
    delete[] pGroupName;
    delete pJid;
    delete pChange;

    return 0;
}

LRESULT CMyTalkDlg::OnRosterDel( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp          = (LibJingleEx::HANDLE)wParam;
    void ** pTmp                      = (void **)lParam;

    char * pGroupName                 = (char*)pTmp[0];
    LibJingleEx::PTJid pJid           = (LibJingleEx::PTJid)pTmp[1];

    int nIndex = (int)LibJingleEx::GetUserData(hApp);
    ASSERT( nIndex < MAX_ACCOUNT_NUMBER );

    HTREEITEM hItem = m_treeRoster.GetRootItem();

    while( hItem )
    {
        int nTmp = m_treeRoster.GetItemData(hItem) - 1;
        if ( nTmp == nIndex )
        {
            break;
        }
        hItem = m_treeRoster.GetNextSiblingItem(hItem);
    }

    ASSERT( 0 != hItem );

    HTREEITEM hGroup = m_treeRoster.GetChildItem( hItem );
    while( hGroup )
    {
        CString s = m_treeRoster.GetItemText(hGroup );
        if ( s == pGroupName )
        {
            break;
        }
        hGroup = m_treeRoster.GetNextSiblingItem( hGroup );
    }

    ASSERT( hGroup );

    HTREEITEM hFriend = m_treeRoster.GetChildItem( hGroup );
    while( hFriend )
    {
        LibJingleEx::PTMember pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData( hFriend );
        if ( _IfJidEqual( pMember->tJid, *pJid ) )
        {
            break;
        }
        hFriend = m_treeRoster.GetNextSiblingItem( hFriend );
    }

    ASSERT( hFriend );

    LibJingleEx::PTMember pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData(hFriend);
    delete pMember;
    m_treeRoster.DeleteItem(hFriend);

    HTREEITEM hTmp = m_treeRoster.GetChildItem( hGroup );
    if ( 0 == hTmp )
    {
        m_treeRoster.DeleteItem( hGroup );
    }
    


    delete[] pTmp;
    delete[] pGroupName;
    delete pJid;

    return 0;
}

LRESULT CMyTalkDlg::OnRosterChange( WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp          = (LibJingleEx::HANDLE)wParam;
    void ** pTmp                      = (void **)lParam;

    char * pGroupName                 = (char*)pTmp[0];
    LibJingleEx::PTJid pJid           = (LibJingleEx::PTJid)pTmp[1];
    LibJingleEx::PTMemberItem pChange = (LibJingleEx::PTMemberItem)pTmp[2];

    int nIndex = (int)LibJingleEx::GetUserData(hApp);
    ASSERT( nIndex < MAX_ACCOUNT_NUMBER );

    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        int nTmp = m_treeRoster.GetItemData(hItem) - 1;
        if ( nTmp == nIndex )
        {
            break;
        }
        hItem = m_treeRoster.GetNextSiblingItem(hItem);
    }

    ASSERT( 0 != hItem );

    HTREEITEM hGroup = m_treeRoster.GetChildItem( hItem );
    while( hGroup )
    {
        CString s = m_treeRoster.GetItemText(hGroup );
        if ( s == pGroupName )
        {
            break;
        }
        hGroup = m_treeRoster.GetNextSiblingItem( hGroup );
    }
    ASSERT( hGroup );

    LibJingleEx::PTMember pMember = 0;
    HTREEITEM hFriend = m_treeRoster.GetChildItem( hGroup );
    while ( hFriend )
    {
        pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData(hFriend);
        if ( _IfJidEqual(pMember->tJid, *pJid) )
        {
            break;
        }
        hFriend = m_treeRoster.GetNextSiblingItem( hFriend );
    }
    ASSERT( hFriend && pMember );

    strcpy( pMember->szName, pChange->szName );
    pMember->nSubscription = pChange->nSubscription;
    pMember->nAsk          = pChange->nAsk;

    CString sNick;
    if ( '\0' != pChange->szName[0] )
    {
        sNick = pChange->szName;
    }
    else
    {
        sNick = GetFullJid( pJid->sNode, pJid->sDomain, pJid->sResource ).c_str();
    }

    LibJingleEx::EPresence ePres = LibJingleEx::GetFriendPresence( hApp, pJid );
    int nImage = GetImgIndex( ePres );

    m_treeRoster.SetItemText( hFriend, sNick );
    m_treeRoster.SetItemImage( hFriend, nImage, nImage );


    delete[] pTmp;
    delete[] pGroupName;
    delete pJid;
    delete pChange;

    return 0;
}

void CMyTalkDlg::OnNMRClickTreeRoster(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;

    CPoint   pt;
    CPoint   pt_client;

    GetCursorPos( &pt);

    pt_client = pt;
    m_treeRoster.ScreenToClient( &pt_client );

    TVHITTESTINFO t;
    t.pt = pt_client;
    m_treeRoster.HitTest( &t );

    if ( t.hItem )
    {
        HTREEITEM hItem = t.hItem;

        int nDepth = GetTreeDepth( m_treeRoster, hItem );
        if ( 2 == nDepth )
        {
            m_treeRoster.SelectItem( hItem );

            CMenu   menu;
            menu.LoadMenu(IDR_MENU_FRIEND);
            CMenu   *pSubMenu   =   menu.GetSubMenu(0);
            pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,   pt.y,   this); 
        }
        else if ( 0 == nDepth )
        {
            m_treeRoster.SelectItem( hItem );

            CMenu   menu;
            menu.LoadMenu(IDR_MENU_SELF);
            CMenu   *pSubMenu   =   menu.GetSubMenu(0);
            pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,   pt.y,   this); 
        }
    }

    //int n = GetTreeDepth( m_treeRoster, hItem );

    //if ( 2 == n )
    //{
    //    CMenu   menu;
    //    menu.LoadMenu(IDR_MENU_FRIEND);
    //    CPoint   pt;
    //    GetCursorPos(&pt);
    //    CMenu   *pSubMenu   =   menu.GetSubMenu(0);
    //    pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,pt.x,   pt.y,   this); 

    //}
}

LRESULT CMyTalkDlg::OnSubscribe(WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp  = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::PTJid  ptJid = (LibJingleEx::PTJid)lParam;

    PTOnSubscribeMsg p = new TOnSubscribeMsg;
    p->m_hApp  = hApp;
    p->m_ptJid = ptJid;

    m_vOnSubscribeMsg.push_back( p );

    // 如果OnSubscribeDlg没有显示
    if ( !m_OnSubscribeDlg.IsWindowVisible() )
    {
        ASSERT( 1 == m_vOnSubscribeMsg.size() );

        m_OnSubscribeDlg.m_hApp = hApp;
        memcpy( &m_OnSubscribeDlg.m_tJid, ptJid, sizeof(LibJingleEx::TJid) );

        m_OnSubscribeDlg.UpdateMsg();

        CRect rectClient;
        this->GetClientRect(rectClient);
        int cx = rectClient.Width();
        int cy = rectClient.Height();

        CRect r1( 0, 0, cx,cy - 90 );
        m_treeRoster.MoveWindow( r1 );

        CRect r2;
        r2.left   = r1.left + 2;
        r2.right  = r1.right - 2;
        r2.top    = r1.bottom + 3;
        r2.bottom = r2.top + 60;

        m_OnSubscribeDlg.MoveWindow( r2 );

        CRect r3;
        r3.left    = r1.left;
        r3.right   = r1.right;
        r3.top     = r2.bottom + 3;
        r3.bottom  = r3.top + 20;

        m_comboStatus.MoveWindow( r3 );

        m_OnSubscribeDlg.ShowWindow( SW_SHOW );
    }

    return 0;
}

LRESULT CMyTalkDlg::OnSubscribeClose(WPARAM wParam, LPARAM lParam )
{
    vOnSubscribeMsg::iterator it = m_vOnSubscribeMsg.begin();
    ASSERT( it != m_vOnSubscribeMsg.end() );

    PTOnSubscribeMsg p = *it;
    delete p->m_ptJid;
    delete p;
    m_vOnSubscribeMsg.erase( it );

    
    it = m_vOnSubscribeMsg.begin();
    if ( it != m_vOnSubscribeMsg.end() )
    {
        p = *it;

        m_OnSubscribeDlg.m_hApp = p->m_hApp;
        memcpy( &m_OnSubscribeDlg.m_tJid, p->m_ptJid, sizeof(LibJingleEx::TJid) );

        m_OnSubscribeDlg.UpdateMsg();
    }
    else
    {
        CRect rectClient;
        this->GetClientRect( rectClient );

        int cx = rectClient.Width();
        int cy = rectClient.Height();

        CRect r1( 0, 0, cx,cy - 30 );
        m_treeRoster.MoveWindow( r1 );

        CRect r2;
        r2.left   = r1.left;
        r2.right  = r1.right;
        r2.top    = r1.bottom + 5;
        r2.bottom = r2.top + 20;

        m_comboStatus.MoveWindow( r2 );

        m_OnSubscribeDlg.ShowWindow( SW_HIDE );
    }
    return 0;
}

LRESULT CMyTalkDlg::OnConferenceServer(WPARAM wParam, LPARAM lParam )
{
    LibJingleEx::HANDLE hApp = (LibJingleEx::HANDLE)wParam;
    BOOL bAvailable          = ( BOOL )lParam;

    int nIndex = (int)LibJingleEx::GetUserData(hApp);
    if ( !g_bConferenceAvailable[nIndex] && bAvailable )
    {
        g_bConferenceAvailable[nIndex] = TRUE;
    }
    return 0;
}

LRESULT CMyTalkDlg::OnCreateChatRoomSuccess(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HROOM hRoom  = (LibJingleEx::HROOM)wParam;
    LibJingleEx::HANDLE hApp  = LibJingleEx::GetHApp(hRoom);

    int nIndex = (int)LibJingleEx::GetUserData(hApp);

    LibJingleEx::TJid tSelfJid;
    LibJingleEx::GetSelfJid(hApp,&tSelfJid);

    CChatRoomDlg * pDlg = new CChatRoomDlg;
    memcpy( &pDlg->m_tSelfJid, &tSelfJid, sizeof(LibJingleEx::TJid) );
    
    vChatRoom & v = g_vChatRooms[nIndex];
    vChatRoom::iterator it = find_if ( v.begin(), v.end(), FindChatRoomObj(hRoom)  );

    PTChatRoom p = 0;
    if ( it != v.end() )
    {
        p = *it;
    }
    else
    {
        p = new TChatRoom;
        p->m_hRoom = hRoom;
        v.push_back( p );
    }

    //PTChatRoom p = *it;
    p->m_ChatRoom = pDlg;

    pDlg->m_pChatRoom = p;
    pDlg->Create( IDD_CHATROOM, GetDesktopWindow() );

    pDlg->m_lstParticipant.InsertItem( 0, GetFullJid(tSelfJid.sNode, tSelfJid.sDomain, tSelfJid.sResource).c_str() );
    int nSel = m_comboStatus.GetCurSel();
    LibJingleEx::EPresence ePres = LibJingleEx::EPresence(nSel+2);
    pDlg->m_lstParticipant.SetItemText(0, 1, ::GetPresence(ePres) );
    pDlg->m_lstParticipant.SetItemText(0, 2, ::GetAffiliation(LibJingleEx::AFFILIATION_OWNER));
    pDlg->ShowWindow( SW_SHOW );


    vParticipant::iterator it_p;
    for ( it_p = p->m_vInvited.begin(); it_p != p->m_vInvited.end(); it_p++ )
    {
        PTParticipant pParticipant = *it_p;

        LibJingleEx::ELibjingleError e = LibJingleEx::InviteParticipant( hRoom, &pParticipant->m_tJid );
        if ( 0 != e )
        {
            CString s;
            s.Format( "Failed to LibJingleEx::InviteParticipant! ( invite = %s, error = %d )", 
                GetFullJid(pParticipant->m_tJid.sNode, pParticipant->m_tJid.sDomain, pParticipant->m_tJid.sResource).c_str(), e );

            MessageBox( s );
        }

        delete pParticipant;
    }
    p->m_vInvited.clear();

    return 0;
}

LRESULT CMyTalkDlg::OnInvited(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HROOM hRoom = (LibJingleEx::HROOM)wParam;

    LibJingleEx::ELibjingleError e = LibJingleEx::AcceptInvite( hRoom, TRUE );
    if ( 0 != e )
    {
        MessageBox("Failed to LibJingleEx::AcceptInvite()");
    }

    return 0;
}

LRESULT CMyTalkDlg::OnChatRoomPresence(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HROOM  hRoom = (LibJingleEx::HROOM)wParam;
    void ** ppTmp             = (void **)lParam;

    LibJingleEx::PTJid  ptJid       = (LibJingleEx::PTJid)ppTmp[0];
    LibJingleEx::EPresence * pPres  = (LibJingleEx::EPresence *)ppTmp[1];
    char * szDescription            = (char *)ppTmp[2];
    LibJingleEx::EAffiliation eAffi = (LibJingleEx::EAffiliation)(int)ppTmp[3];
    LibJingleEx::HANDLE  hApp       = (LibJingleEx::HANDLE)ppTmp[4];
    int nFlag                       = (int)ppTmp[6];

    BOOL bKick = ( LibJingleEx::CHATROOM_PRESENCE_FLAG_KICKED == nFlag ? TRUE : FALSE );

    delete[] ppTmp;

    // to do
    int nIndex = (int)LibJingleEx::GetUserData(hApp);

    vChatRoom & v = g_vChatRooms[nIndex];
    vChatRoom::iterator it = find_if ( v.begin(), v.end(), FindChatRoomObj(hRoom)  );
    
    PTChatRoom p = 0;

    // 没有找到
    if ( it == v.end() )
    {
        p = new TChatRoom;

        p->m_hRoom = hRoom;

        CChatRoomDlg * pDlg = new CChatRoomDlg;

        LibJingleEx::TJid tSelfJid;
        LibJingleEx::GetSelfJid(hApp,&tSelfJid);
        memcpy( &pDlg->m_tSelfJid, &tSelfJid, sizeof(LibJingleEx::TJid) );

        pDlg->Create( IDD_CHATROOM, GetDesktopWindow() );
        pDlg->m_pChatRoom = p;

        p->m_ChatRoom = pDlg;
        p->Update( ptJid, pPres, &eAffi );
        pDlg->UpdateParticipant( ptJid, pPres, &eAffi );
        pDlg->ShowWindow( SW_SHOW );

        v.push_back( p );
    }
    // 找到
    else
    {
        p = *it;

        ASSERT( p->m_ChatRoom );
        CChatRoomDlg * pDlg = (CChatRoomDlg *)p->m_ChatRoom;

        LibJingleEx::TJid tSelfJid;
        LibJingleEx::ELibjingleError e =  LibJingleEx::GetSelfJid(hApp, &tSelfJid);
        if ( 0 != e )
        {
            MessageBox("Failed to LibJingleEx::GetSelfJid() !");
            return 0;
        }

        // 是自己
        if ( _IfJidEqual(tSelfJid, *ptJid) )
        {
            BOOL bQuit = FALSE;

            // 有状态
            if ( pPres )
            {
                // 下线(退出房间)
                if ( *pPres == LibJingleEx::PRESENCE_NONE  )
                {
                    delete p;
                    v.erase( it );
                    bQuit = TRUE;
                }
            }
            
            if ( !bQuit )
            {
                p->Update( ptJid, pPres, &eAffi );
                pDlg->UpdateParticipant( ptJid, pPres, &eAffi );
            }
        }
        // 不是自己
        else
        {
            p->Update( ptJid, pPres, &eAffi );
            pDlg->UpdateParticipant( ptJid, pPres, &eAffi, bKick );
        }
    }



    // end to to

    delete ptJid;

    if ( pPres )
    {
        delete pPres;
    }

    if ( szDescription )
    {
        delete[] szDescription;
    }
    return 0;
}

LRESULT CMyTalkDlg::OnChatRoomMessage(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HROOM hRoom = (LibJingleEx::HROOM)wParam;
    void ** ppTmp            = (void **)lParam;

    LibJingleEx::PTJid  ptJid   = (LibJingleEx::PTJid)ppTmp[0];
    char *              szMsg   = (char *)ppTmp[1];
    char *              szMsgId = (char *)ppTmp[2];


    LibJingleEx::HANDLE hApp = LibJingleEx::GetHApp(hRoom );
    int nIndex = (int)LibJingleEx::GetUserData(hApp);

    vChatRoom & v = g_vChatRooms[nIndex];
    
    vChatRoom::iterator it = find_if( v.begin(), v.end(), FindChatRoomObj(hRoom) );
    ASSERT( it != v.end() );

    PTChatRoom p = *it;
    CChatRoomDlg * pDlg = (CChatRoomDlg *)p->m_ChatRoom;

    pDlg->AddMessage(ptJid, szMsg);

    if ( !pDlg->IsWindowVisible() )
    {
        pDlg->ShowWindow( SW_SHOW );
    }

    // LibJingleEx::SetGroupchatMsgRead( hRoom );

    delete[] ppTmp;
    delete ptJid;
    delete[] szMsg;
    delete[] szMsgId;

    return 0;
}

LRESULT CMyTalkDlg::OnChatRoomDestroy(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE hApp = ( LibJingleEx::HANDLE )wParam;
    LibJingleEx::HROOM  hRoom = (LibJingleEx::HROOM)lParam;

    int nIndex = (int)LibJingleEx::GetUserData(hApp);

    vChatRoom & v = g_vChatRooms[nIndex];
    vChatRoom::iterator it = find_if( v.begin(), v.end(), FindChatRoomObj(hRoom) );
    ASSERT( it != v.end() );

    PTChatRoom p = *it;
    
    delete p;
    v.erase( it );


    return 0;
}



LRESULT CMyTalkDlg::OnChatRoomRosterAdd(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE     hApp = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::CMsgData * pMsg = (LibJingleEx::CMsgData *)lParam;

    int nIndex = (int)LibJingleEx::GetUserData(hApp);

    LibJingleEx::HROOM hRoom = 0;
    DWORD              dwCnt = 0;
    LibJingleEx::TRoomRosterItem  atItems[LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT];
    char szRoomId[64];
    char szRoomService[64];

    pMsg->Read( (unsigned char *)&hRoom,    sizeof(hRoom) );
    pMsg->Read( (unsigned char *)szRoomId,         sizeof(szRoomId) );
    pMsg->Read( (unsigned char *)szRoomService,    sizeof(szRoomService) );

    pMsg->Read( (unsigned char *)&dwCnt,    sizeof(dwCnt) );
    pMsg->Read( (unsigned char *)atItems,   sizeof(LibJingleEx::TRoomRosterItem) * dwCnt );


    vChatRoom & v = g_vChatRooms[nIndex];
    vChatRoom::iterator it = find_if ( v.begin(), v.end(), FindChatRoomObj(hRoom)  );

    PTChatRoom p = 0;

    // 没有找到
    if ( it == v.end() )
    {
        return 0;
    }
    // 找到
    else
    {
        p = *it;

        ASSERT( p->m_ChatRoom );
        CChatRoomDlg * pDlg = (CChatRoomDlg *)p->m_ChatRoom;

        DWORD i;
        for ( i = 0; i < dwCnt; i++ )
        {
            pDlg->UpdateRosterItem( &atItems[i], TRUE );
        }
    }


    delete pMsg;
    return 0;
}


LRESULT CMyTalkDlg::OnChatRoomRosterDel(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE     hApp = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::CMsgData * pMsg = (LibJingleEx::CMsgData *)lParam;

    int nIndex = (int)LibJingleEx::GetUserData(hApp);

    LibJingleEx::HROOM hRoom = 0;
    DWORD              dwCnt = 0;
    LibJingleEx::TRoomRosterItem  atItems[LibJingleEx::MAX_ROOM_ROSTER_ITEMS_NOTIFY_COUNT];
    char szRoomId[64];
    char szRoomService[64];

    pMsg->Read( (unsigned char *)&hRoom,           sizeof(hRoom) );
    pMsg->Read( (unsigned char *)szRoomId,         sizeof(szRoomId) );
    pMsg->Read( (unsigned char *)szRoomService,    sizeof(szRoomService) );

    pMsg->Read( (unsigned char *)&dwCnt,    sizeof(dwCnt) );
    pMsg->Read( (unsigned char *)atItems,   sizeof(LibJingleEx::TRoomRosterItem) * dwCnt );


    vChatRoom & v = g_vChatRooms[nIndex];
    vChatRoom::iterator it = find_if ( v.begin(), v.end(), FindChatRoomObj(hRoom)  );

    PTChatRoom p = 0;

    // 没有找到
    if ( it == v.end() )
    {
        return 0;
    }
    // 找到
    else
    {
        p = *it;

        ASSERT( p->m_ChatRoom );
        CChatRoomDlg * pDlg = (CChatRoomDlg *)p->m_ChatRoom;

        DWORD i;
        for ( i = 0; i < dwCnt; i++ )
        {
            pDlg->UpdateRosterItem( &atItems[i], FALSE );
        }
    }


    delete pMsg;
    return 0;
}



void CMyTalkDlg::OnFileTransfer()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    if ( 0 == hItem )
    {
        return;
    }

    int n = GetTreeDepth( m_treeRoster, hItem );
    if ( 2 == n )
    {
        HTREEITEM hRoot = GetTreeTop( m_treeRoster, hItem );
        ASSERT( hRoot );

        int nIndex = m_treeRoster.GetItemData( hRoot ) - 1;
        ASSERT( nIndex < MAX_ACCOUNT_NUMBER );

        LibJingleEx::PTMember pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData( hItem );

        LibJingleEx::TFriendInfo atInfo[8];
        unsigned long dwSize = 8;
        LibJingleEx::PTFriendInfo pRet = LibJingleEx::GetFriendAllInfo( g_hXmppApp[nIndex], &pMember->tJid, atInfo, &dwSize  );
        if ( 0 == pRet || 0 == dwSize )
        {
            MessageBox("好友没有在线!");
            return;
        }

        DWORD i;
        for ( i = 0; i < dwSize; i++ )
        {
            if ( atInfo[i].bFileShareAbility )
            {
                break;
            }
        }

        if ( i >= dwSize )
        {
            MessageBox("好友没有文件传输能力!");
            return;
        }

        LibJingleEx::TJid tPeerJid;
        memcpy( &tPeerJid, &pMember->tJid, sizeof(tPeerJid) );
        strcpy( tPeerJid.sResource, atInfo[i].sResource );

        LibJingleEx::PTJid ptPeerJid = &tPeerJid;        

        vFileTranferDlg & v = m_vFileTranferDlgs[nIndex];
        vFileTranferDlg::iterator it = find_if( v.begin(), v.end(), FindFileTranferDlgObj(ptPeerJid) );

        CFileTranferDlg * pDlg = 0;
        if ( it == v.end() )
        {
            pDlg = new CFileTranferDlg;

            pDlg->m_nIndex = nIndex;
            LibJingleEx::GetSelfJid( g_hXmppApp[nIndex], &pDlg->m_tSelfJid );
            memcpy( &pDlg->m_tPeerJid, ptPeerJid, sizeof(LibJingleEx::TJid) );

            pDlg->Create(IDD_FILE_TRANSFER, GetDesktopWindow());

            v.push_back( pDlg );
        }
        else
        {
            pDlg = *it;
        }

        if ( !pDlg->IsWindowVisible() )
        {
            pDlg->ShowWindow( SW_SHOW );
        }
    }
}


LRESULT CMyTalkDlg::OnFileOffer(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE   hApp     = (LibJingleEx::HANDLE)wParam;
    
    void ** pTmp = (void **)lParam;

    LibJingleEx::HSession hSession = (LibJingleEx::HSession)pTmp[0];
    LibJingleEx::PTJid ptJid = (LibJingleEx::PTJid)pTmp[1];
    LibJingleEx::PFileShareManifest pManifest = (LibJingleEx::PFileShareManifest)pTmp[2];
    BOOL bSender = (BOOL)pTmp[3];
    delete[] pTmp;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    LibJingleEx::PTJid ptPeerJid = ptJid;

    vFileTranferDlg & v = m_vFileTranferDlgs[nIndex];
    vFileTranferDlg::iterator it = find_if( v.begin(), v.end(), FindFileTranferDlgObj(ptPeerJid) );

    CFileTranferDlg * pDlg = 0;
    if ( it == v.end() )
    {
        pDlg = new CFileTranferDlg;

        pDlg->m_nIndex = nIndex;
        LibJingleEx::GetSelfJid( g_hXmppApp[nIndex], &pDlg->m_tSelfJid );
        memcpy( &pDlg->m_tPeerJid, ptPeerJid, sizeof(LibJingleEx::TJid) );

        pDlg->Create(IDD_FILE_TRANSFER, GetDesktopWindow());
        v.push_back( pDlg );
    }
    else
    {
        pDlg = *it;
    }

    pDlg->Add( hSession, pManifest, bSender );

    if ( !pDlg->IsWindowVisible() )
    {
        pDlg->ShowWindow( SW_SHOW );
    }


    delete ptJid;
    delete pManifest;
    return 0;
}

LRESULT CMyTalkDlg::OnFileProgress(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE   hApp     = (LibJingleEx::HANDLE)wParam;

    void ** pTmp = (void **)lParam;
    LibJingleEx::HSession hSession = (LibJingleEx::HSession)pTmp[0];
    DWORD dwPercent                = (DWORD)pTmp[1];
    char * szFileName              = (char *)pTmp[2];
    DWORD dwTransferSize           = (DWORD)pTmp[3];
    DWORD dwTransferRate           = (DWORD)pTmp[4];
    delete[] pTmp;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );



    vFileTranferDlg & v = m_vFileTranferDlgs[nIndex];
    vFileTranferDlg::iterator it;

    CProgressDlg * pProgressDlg = 0;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        CFileTranferDlg * pDlg = *it;

        pProgressDlg = pDlg->IsContain( hSession, FALSE );

        if ( pProgressDlg )
        {
            break;
        }
    }

    ASSERT( it != v.end() && pProgressDlg );    
    pProgressDlg->UpdateProgress( dwPercent, szFileName, dwTransferSize, dwTransferRate );


    delete[] szFileName;
    return 0;
}

LRESULT CMyTalkDlg::OnFileComplete(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE hApp = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::HSession hSession = (LibJingleEx::HSession)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    vFileTranferDlg & v = m_vFileTranferDlgs[nIndex];
    vFileTranferDlg::iterator it;

    CProgressDlg * pProgressDlg = 0;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        CFileTranferDlg * pDlg = *it;
        pProgressDlg = pDlg->IsContain( hSession, FALSE );

        if ( pProgressDlg )
        {
            break;
        }
    }

    ASSERT( it != v.end() && pProgressDlg );    

    pProgressDlg->OnComplete();

    return 0;
}

LRESULT CMyTalkDlg::OnFileCancel(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE hApp = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::HSession hSession = (LibJingleEx::HSession)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    vFileTranferDlg & v = m_vFileTranferDlgs[nIndex];
    vFileTranferDlg::iterator it;

    CProgressDlg * pProgressDlg = 0;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        CFileTranferDlg * pDlg = *it;
        pProgressDlg = pDlg->IsContain( hSession, FALSE );

        if ( pProgressDlg )
        {
            break;
        }
    }

    // !CAUSION
    // ASSERT( it != v.end() && pProgressDlg );    
    if ( pProgressDlg )
    {
        pProgressDlg->OnFileCancel();
    }

    return 0;
}

LRESULT CMyTalkDlg::OnFileFailure(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE hApp = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::HSession hSession = (LibJingleEx::HSession)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    vFileTranferDlg & v = m_vFileTranferDlgs[nIndex];
    vFileTranferDlg::iterator it;

    CProgressDlg * pProgressDlg = 0;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        CFileTranferDlg * pDlg = *it;
        pProgressDlg = pDlg->IsContain( hSession, FALSE );

        if ( pProgressDlg )
        {
            break;
        }
    }

    ASSERT( it != v.end() && pProgressDlg );    

    pProgressDlg->OnFailure();

    return 0;
}

LRESULT CMyTalkDlg::OnFileTransfer(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE hApp = (LibJingleEx::HANDLE)wParam;
    LibJingleEx::HSession hSession = (LibJingleEx::HSession)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );


    vFileTranferDlg & v = m_vFileTranferDlgs[nIndex];
    vFileTranferDlg::iterator it;

    CProgressDlg * pProgressDlg = 0;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        CFileTranferDlg * pDlg = *it;
        pProgressDlg = pDlg->IsContain( hSession, FALSE );

        if ( pProgressDlg )
        {
            break;
        }
    }

    ASSERT( it != v.end() && pProgressDlg );    

    pProgressDlg->OnTransfer();


    return 0;
}

void CMyTalkDlg::OnVCardSet()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    ASSERT( 0 != hItem );

    int nIndex = 0;
    nIndex = (int)m_treeRoster.GetItemData( hItem ) - 1;

    LibJingleEx::ELibjingleError eRet = LibJingleEx::GetVCard( g_hXmppApp[nIndex] );

    if ( 0 != eRet )
    {
        MessageBox("Failed to LibJingleEx::GetVCard() !");
        return;
    }
}

LRESULT CMyTalkDlg::OnVCardInfo(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE hApp = (LibJingleEx::HANDLE)wParam;
    void ** pTmp             = (void **)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    LibJingleEx::PTJid   ptJid     =   (LibJingleEx::PTJid)pTmp[0];
    LibJingleEx::PTVCard ptVCard   =   (LibJingleEx::PTVCard)pTmp[1];
    BOOL  bSelf = (BOOL)pTmp[2];

    if ( bSelf )
    {
        CVCardSetDlg    dlg;

        dlg.m_nIndex   = nIndex;
        memcpy( &dlg.m_tJid, ptJid, sizeof(LibJingleEx::TJid) );
        memcpy( &dlg.m_tVCard, ptVCard, sizeof(LibJingleEx::TVCard) );
        dlg.m_bSelf    = TRUE;

        dlg.DoModal();
    }
    else
    {
        CVCardSetDlg    dlg;

        dlg.m_nIndex   = nIndex;
        memcpy( &dlg.m_tJid, ptJid, sizeof(LibJingleEx::TJid) );
        memcpy( &dlg.m_tVCard, ptVCard, sizeof(LibJingleEx::TVCard) );
        dlg.m_bSelf    = FALSE;

        dlg.DoModal();
    }

    delete ptJid;
    delete ptVCard;
    delete[] pTmp;
    return 0;
}

void CMyTalkDlg::OnVCardGet()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    ASSERT( 0 != hItem );

    HTREEITEM hRoot = ::GetTreeTop( m_treeRoster, hItem );

    int nIndex = (int)m_treeRoster.GetItemData(hRoot) - 1;
    LibJingleEx::PTMember pItem = (LibJingleEx::PTMember)m_treeRoster.GetItemData( hItem );

    LibJingleEx::TJid tJid;
    memcpy( &tJid, &pItem->tJid, sizeof(tJid) );
    tJid.sResource[0] = '\0';

    LibJingleEx::ELibjingleError eRet = LibJingleEx::GetVCard( g_hXmppApp[nIndex], &tJid );
    if ( 0 != eRet )
    {
        MessageBox( " Failed to LibJingleEx::GetVCard()" );
        return;
    }
}

void CMyTalkDlg::OnSetPassword()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    ASSERT( 0 != hItem );

    int nIndex = 0;
    nIndex = (int)m_treeRoster.GetItemData( hItem ) - 1;

    CPasswordDlg dlg;
    dlg.m_nIndex = nIndex;

    dlg.DoModal();
}

void CMyTalkDlg::OnSendPicture()
{
    HTREEITEM hItem = m_treeRoster.GetSelectedItem();
    if ( 0 == hItem )
    {
        return;
    }

    HTREEITEM hRoot = GetTreeTop( m_treeRoster, hItem );
    ASSERT( hRoot );

    int nIndex = m_treeRoster.GetItemData( hRoot ) - 1;
    ASSERT( nIndex < MAX_ACCOUNT_NUMBER );

    vPictureDlg & v = m_vPictureDlgs[nIndex];

    LibJingleEx::PTMember pMember = (LibJingleEx::PTMember)m_treeRoster.GetItemData( hItem );
    LibJingleEx::PTJid ptJid = &pMember->tJid;
    vPictureDlg::iterator it = find_if( v.begin(), v.end(), FindPictureDlgObj( ptJid ) );

    CPictureDlg * pDlg = 0;
    // 没有
    if ( it == v.end() )
    {
        pDlg = new CPictureDlg;
        memcpy( &pDlg->m_tJid, ptJid, sizeof(LibJingleEx::TJid) );
        pDlg->m_sNick = pMember->szName;
        pDlg->m_nIndex = nIndex;

        pDlg->Create( IDD_PICTURE, GetDesktopWindow() );

        v.push_back( pDlg );
    }
    // 有
    else
    {
        pDlg = *it;
    }

    pDlg->ShowWindow( SW_SHOW );
}

LRESULT CMyTalkDlg::OnPictureMsg(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE hApp = (LibJingleEx::HANDLE)wParam;
    void ** pTmp             = (void **)lParam;

    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    char * pId                         =   (char *)pTmp[0];
    LibJingleEx::PTJid   ptJid         =   (LibJingleEx::PTJid)pTmp[1];
    int                  nFlag         =   (DWORD)pTmp[2];
    DWORD                dwTotalSize   =   (DWORD)pTmp[3];
    DWORD                dwBufSize     =   (DWORD)pTmp[4];
    BYTE *               pBuf          =   (BYTE *)pTmp[5];


    vPictureDlg & v = m_vPictureDlgs[nIndex];

    vPictureDlg::iterator it = find_if( v.begin(), v.end(), FindPictureDlgObj( ptJid ) );

    CPictureDlg * pDlg = 0;
    // 没有
    if ( it == v.end() )
    {
        pDlg = new CPictureDlg;
        memcpy( &pDlg->m_tJid, ptJid, sizeof(LibJingleEx::TJid) );
        pDlg->m_nIndex = nIndex;

        pDlg->Create( IDD_PICTURE, GetDesktopWindow() );

        v.push_back( pDlg );
    }
    // 有
    else
    {
        pDlg = *it;
    }

    pDlg->UpdatePicture( pBuf, dwBufSize );
    pDlg->ShowWindow( SW_SHOW );

    //static FILE * fp = 0;

    //if ( nFlag & 1 )
    //{
    //    fp = fopen("d:\\abc", "wb");
    //}

    //fwrite( pBuf, 1, dwBufSize, fp );

    //if ( nFlag & 2 )
    //{
    //    fclose( fp );
    //}
    

    delete[]   pId;
    delete     ptJid;
    delete[]   pBuf;
    delete[]   pTmp;
    return 0;
}

LRESULT CMyTalkDlg::OnNotifyPersistentRooms(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE  hApp = (LibJingleEx::HANDLE)wParam;
    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    unsigned char * pData = (unsigned char *)lParam;
    unsigned char * p = pData;
    
    DWORD dwMsgLen = 0;
    memcpy( &dwMsgLen, pData, sizeof(DWORD) );
    pData += sizeof(DWORD);


    DWORD  dwCnt = 0;
    memcpy( &dwCnt, pData, sizeof(DWORD) );

    LibJingleEx::CMsgData  cMsg( pData + sizeof(DWORD), dwMsgLen - sizeof(DWORD) );

    DWORD i;
    for ( i = 0; i < dwCnt; i++ )
    {
        LibJingleEx::TJid   tRoomJid;
        char                szName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN];

        cMsg.Read( (unsigned char *)&tRoomJid,   sizeof(tRoomJid) );
        cMsg.Read( (unsigned char *)szName,      LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN );

        LibJingleEx::ELibjingleError eRet = LibJingleEx::EnterPersistentRoom( hApp, &tRoomJid );
        if ( 0 != eRet )
        {
            MessageBox( "Failed to LibJingleEx::EnterPersistentRoom()!" );
        }
    }



    delete[] p;

    return 0;
}


LRESULT CMyTalkDlg::OnNotifyCharState(WPARAM wParam, LPARAM lParam)
{
    LibJingleEx::HANDLE  hApp = (LibJingleEx::HANDLE)wParam;
    int nIndex = (int)LibJingleEx::GetUserData( hApp );

    unsigned char * pData = (unsigned char *)lParam;
    unsigned char * p = pData;

    LibJingleEx::TJid        tJid;
    LibJingleEx::ECharStatus e;

    memcpy( &tJid, p, sizeof(LibJingleEx::TJid) );
    p += sizeof(LibJingleEx::TJid);

    memcpy( &e, p, sizeof(LibJingleEx::ECharStatus) );
    p += sizeof(LibJingleEx::ECharStatus);

    delete[] pData;




    tJid.sResource[0] = '\0';

    vChatDlg & v = m_vChatDlgs[nIndex];
    vChatDlg::iterator it = find_if( v.begin(), v.end(), FindChatDlgObj( &tJid ) );

    CChatDlg * pChatDlg = 0;
    // 没有
    if ( it == v.end() )
    {
        pChatDlg = new CChatDlg;
        memcpy( &pChatDlg->m_tJid, &tJid, sizeof(LibJingleEx::TJid) );
        pChatDlg->m_nIndex = nIndex;

        pChatDlg->Create( IDD_CHAT, GetDesktopWindow() );
        v.push_back( pChatDlg );
    }
    // 有
    else
    {
        pChatDlg = *it;
    }

    pChatDlg->OnChatState( e );
    pChatDlg->ShowWindow( SW_SHOW );


    return 0;
}


void CMyTalkDlg::OnTimer(UINT_PTR nIDEvent)
{
    int i;
    for  ( i = 0; i < MAX_ACCOUNT_NUMBER; i++ )
    {
        if ( g_hXmppApp[i] )
        {
            LibJingleEx::SendPing( g_hXmppApp[i] );
        }
    }
}























void CMyTalkDlg::OnTest()
{
    CSelAccountDlg dlg;

    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        int nIndex = m_treeRoster.GetItemData( hItem ) - 1;
        dlg.m_v.push_back( nIndex );

        hItem = m_treeRoster.GetNextSiblingItem(hItem);
    }    
    
    if ( IDOK == dlg.DoModal() )
    {
        if ( 0 != g_pTestDlg )
        {
            g_pTestDlg->DestroyWindow();
            delete g_pTestDlg;
            g_pTestDlg = 0;
        }

        g_pTestDlg = new CTestDmgGroupDlg;
        g_pTestDlg->m_nIndex = dlg.m_nIndex;
        g_pTestDlg->Create( CTestDmgGroupDlg::IDD, GetDesktopWindow() );
        g_pTestDlg->ShowWindow( SW_SHOW );
    }
}

DWORD WINAPI test_thrd( LPVOID lpThreadParameter )
{

    LibJingleEx::SetTrueSocialName( "wolf@qq.com", "狼人" );
    LibJingleEx::SetTrueSocialName( "bird@qq.com", "菜鸟" );
    LibJingleEx::SetTrueSocialName( "test1@qq.com", "测试1" );

    return 0;
}

void CMyTalkDlg::OnTestMisc()
{
    CSelAccountDlg dlg;

    HTREEITEM hItem = m_treeRoster.GetRootItem();
    while( hItem )
    {
        int nIndex = m_treeRoster.GetItemData( hItem ) - 1;
        dlg.m_v.push_back( nIndex );

        hItem = m_treeRoster.GetNextSiblingItem(hItem);
    }    

    if ( IDOK == dlg.DoModal() )
    {
        //LibJingleEx::ELibjingleError eRet = LibJingleEx::TryEnterAllPersistentChatrooms( g_hXmppApp[dlg.m_nIndex] );   

        //if ( 0 != eRet )
        //{
        //    MessageBox("Failed to LibJingleEx::TryEnterAllPersistentChatrooms() ! \n");
        //}


        //LibJingleEx::TJid   tJid;
        //LibJingleEx::StrJid2TJid( "lion@msn.test", tJid );

        //LibJingleEx::TCustomMsg  tMsg;
        //tMsg.nMsgType = 100;
        //tMsg.nParams[0] = 1;
        //tMsg.nParams[1] = 2;
        //tMsg.nParams[2] = 3;
        //tMsg.nParams[3] = 4;
        //// strcpy( tMsg.achBuf, "test" );

        //LibJingleEx::ELibjingleError eRet = LibJingleEx::SendCustomMsg( g_hXmppApp[dlg.m_nIndex], &tJid, &tMsg );
        //if ( 0 != eRet )
        //{
        //    MessageBox("Failed to LibJingleEx::SendCustomMsg() ! \n");
        //}


        //LibJingleEx::TCustomConfigItem  t[4];

        //strcpy( t[0].szKey,   "1" );
        //strcpy( t[0].szValue, "1000" );

        //strcpy( t[1].szKey,   "2" );
        //strcpy( t[1].szValue, "2000" );

        //strcpy( t[2].szKey,   "你好" );
        //strcpy( t[2].szValue, "3000" );

        //strcpy( t[3].szKey,   LibJingleEx::SCREENED_CHATROOMS_KEY );
        //strcpy( t[3].szValue, "abc xyz abc" );

        //

        //LibJingleEx::ELibjingleError eRet = LibJingleEx::SetCustomCfg( g_hXmppApp[dlg.m_nIndex], t, 4 );
        //if ( 0 != eRet )
        //{
        //    MessageBox("Failed to LibJingleEx::SetCustomCfg() ! \n");
        //}

        //LibJingleEx::TCustomConfigItem t;
        //strcpy( t.szKey,   "3" );

        //LibJingleEx::ELibjingleError eRet = LibJingleEx::GetCustomCfg( g_hXmppApp[dlg.m_nIndex], &t );
        //if ( 0 != eRet )
        //{
        //    MessageBox("Failed to LibJingleEx::GetCustomCfg() ! \n");
        //}

        //LibJingleEx::TChatRoomId id;
        //strcpy( id.szRoomId, "e2b3d6c9b47f9d9165272d556ff267ea3b76ce1f" );
        //strcpy( id.szService, "conference.msn.test" );

        //LibJingleEx::HROOM hRoom = LibJingleEx::GetRoomHandle( g_hXmppApp[dlg.m_nIndex], &id );
        //if ( 0 == hRoom )
        //{
        //    MessageBox("Failed to LibJingleEx::GetRoomHandle()!");
        //    return;
        //}

        //TDmsChatContent t;
        //strcpy( t.m_szContent, "123test" );

        //LibJingleEx::ELibjingleError eRet =  LibJingleEx::SendRoomMessage( hRoom, &t );
        //if ( 0 != eRet )
        //{
        //    MessageBox("Failed to LibJingleEx::SendRoomMessage()!");
        //}

        //LibJingleEx::HROOM hRoom = 0;
        //LibJingleEx::ELibjingleError eRet =  LibJingleEx::CreateOrEnterRoom( g_hXmppApp[dlg.m_nIndex], &hRoom, "0512#111234", "test1" );
        //if ( 0 != eRet )
        //{
        //    MessageBox("Failed to LibJingleEx::CreateOrEnterRoom()!");
        //}
    }
    
    CreateThread( 0, 0, test_thrd, 0, 0,  0 );
}
