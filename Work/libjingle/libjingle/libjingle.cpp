
#define  VLD_ON                  0

#if VLD_ON
#ifdef WIN32
#include "vld.h"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __IOS__
#include <malloc.h>
#else
#include <malloc/malloc.h>
#endif
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <expat.h>


#ifdef WIN32
#include <direct.h>
#else
#include <sys/times.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#endif

#ifdef SUPPORT_ICONV
#include <iconv.h>
#endif

#define TEST          0




#include "talk/base/ssladapter.h"
#include "talk/base/fileutils.h"
#include "talk/base/pathutils.h"

#include "talk/examples/login/xmppthread.h"
#include "talk/xmllite/xmlelement.h"
#include "talk/xmpp/constants.h"
#include "talk/p2p/base/sessionmanager.h"
#include "talk/base/httpcommon.h"


#include "talk/base/stream.h"



#include "libjingle/libjingle_inner.h"
#include "libjingle/strutils.h"

#if defined(_MSC_VER) && _MSC_VER >= 1600
#include <iterator>
#endif


LibJingleEx::TLibJingleEvent  g_tEvents;
LibJingleEx::POnMsg           g_pfnOnMessage = 0;


#ifndef  DWORD
#define  DWORD  unsigned long
#endif

#define  XMPP_PORT                             5222
#define  DEFAULT_GROUP_NAME                    "friend"

LibJingleEx::POnXmppMessage    g_pOnXmppMessage = 0;
LibJingleEx::POnLogInfo        g_pOnLogMessage  = 0;

vApp g_Apps;


#define  DMS_MAX_REAL_GROUP_NAME_LEN                   32


LIBJINGLE_API const char *  LibJingleEx::SCREENED_CHATROOMS_KEY = "_screened_chatrooms_";


#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {

        }
        break;
    case DLL_THREAD_ATTACH:
        {

        }
        break;
    case DLL_THREAD_DETACH:
        {

        }
        break;
    case DLL_PROCESS_DETACH:
        {

        }
        break;
    }
    return TRUE;
}
#endif

#ifndef  WIN32
void sig_handler( int sig )
{

}
#endif



/**************************** Libjinglex  ********************************/
namespace LibJingleEx
{

    LIBJINGLE_API ELibjingleError Init( const PTLibJingleEvent ptEvent )
    {
        if ( 0 == ptEvent )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        const TLibJingleEvent & tEvent = *ptEvent;

        CGlobalData * pGlobal = CGlobalData::GetInstance();

        g_tEvents = tEvent;

#ifndef WIN32
        // ��׽PIPE�ź�
        // signal( SIGPIPE, ::sig_handler);
#endif

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError Init( POnMsg  pOnMsg )
    {
        if ( 0 == pOnMsg )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        CGlobalData * pGlobal = CGlobalData::GetInstance();

        g_pfnOnMessage = pOnMsg;

#ifndef WIN32
        // ��׽PIPE�ź�
        signal( SIGPIPE, ::sig_handler);
#endif

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError Deinit( )
    {
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        delete pGlobal;
        return LIBJINGLE_ERR_OK;
    }

    // ��¼
    LIBJINGLE_API ELibjingleError Login( HANDLE *  phApp, PTLoginSettings ptLoginSettings, void * pUserData /* = 0 */ )
    {
        // ��������ȷ
        if ( 0 == phApp || 0 == ptLoginSettings )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        CLibJinglexApp * pAppObj = new CLibJinglexApp;

        CGlobalData * pGlobal = CGlobalData::GetInstance();
        pGlobal->AddApps( pAppObj );

        // ����ص�����
        pAppObj->m_pUserData = pUserData;

        if ( ptLoginSettings->szDefaultSaveDir[0] != '\0' )
        {
            strcpy( pAppObj->m_szSaveFileDir, ptLoginSettings->szDefaultSaveDir );
        }
        else
        {
            getcwd( pAppObj->m_szSaveFileDir, LibJingleEx::MAX_PATH_SIZE );
        }

        strcpy( pAppObj->m_szStunAddr, ptLoginSettings->szStunAddr );
        pAppObj->m_wStunPort = ptLoginSettings->wStunPort;
        strcpy( pAppObj->m_szRelayAddr, ptLoginSettings->szRelayAddr );

        // ������ͼƬĬ�ϱ���·��
        strcpy( pAppObj->m_szGroupChatPicSaveDir, ptLoginSettings->szGroupChatPicSaveDir );

        *phApp = pAppObj;

        buzz::Jid                            jid;
        buzz::XmppClientSettings             xcs;
        talk_base::InsecureCryptStringImpl   pass;
        std::string                          strTmp;

        strTmp  = ptLoginSettings->sUserName;
        strTmp += '@';
        strTmp += ptLoginSettings->sDomain;
        if ( '\0' != ptLoginSettings->sResource[0] )
        {
            strTmp += '/';
            strTmp += ptLoginSettings->sResource;
        }

        jid = buzz::Jid( strTmp );
        if ( !jid.IsValid() || jid.node() == "" )
        {
            return LIBJINGLE_ERR_INVALID_JID;
        }

        pass.password() = ptLoginSettings->sPassword;

        xcs.set_user(jid.node());
        xcs.set_host(jid.domain());
        xcs.set_resource( jid.resource() );
        xcs.set_use_tls( ptLoginSettings->bUseTls ? true : false );
        xcs.set_pass( talk_base::CryptString(pass) );
        xcs.set_use_compression( ptLoginSettings->bUseCompression ? true : false );


        // ��ʱ������tls��compressionһ��ʹ��
        if ( xcs.use_compression() )
        {
            xcs.set_use_tls( false );
        }


        strTmp = ptLoginSettings->sHost;
        // ��ʱ�趨xmpp�������˿�Ϊ5222
        if ( 0 == ptLoginSettings->wPort )
        {
            xcs.set_server( talk_base::SocketAddress(strTmp, XMPP_PORT) );
        }
        else
        {
            xcs.set_server( talk_base::SocketAddress(strTmp, ptLoginSettings->wPort) );
        }

        pAppObj->m_bFileShareAvailable = ptLoginSettings->bFileShareEnable;	
        strcpy( pAppObj->m_szEnterpriseName, ptLoginSettings->sEnterpriseName );

        pAppObj->m_bAudio = ptLoginSettings->m_bAudio;
        pAppObj->m_bVideo = ptLoginSettings->m_bVideo; 


        pAppObj->m_bCall  = ptLoginSettings->m_bCall;
        if ( PRESENCE_NONE != ptLoginSettings->m_ePres && PRESENCE_OFFLINE != ptLoginSettings->m_ePres )
        {
            pAppObj->m_ePres  = ptLoginSettings->m_ePres;
        }

        
        xcs.set_delay_login( ptLoginSettings->bDelayLogin ? true : false );

        if ( ptLoginSettings->bSs5Proxy )
        {
            // proxy type
            xcs.set_proxy( talk_base::PROXY_SOCKS5 );


            // proxy server
            if ( 0 == ptLoginSettings->wSs5Port )
            {
                ptLoginSettings->wSs5Port = 1080;
            }
            strTmp = ptLoginSettings->szSs5Host;
            xcs.set_proxy_server( talk_base::SocketAddress( strTmp, ptLoginSettings->wSs5Port)  );



            if ( '\0' != ptLoginSettings->szSs5ProxyUsername[0] )
            {
                xcs.set_use_proxy_auth( true );
            }
            
            // proxy user
            xcs.set_proxy_user( ptLoginSettings->szSs5ProxyUsername );

            // proxy password
            talk_base::InsecureCryptStringImpl   ss5pass;
            ss5pass.password() = ptLoginSettings->szSs5ProxyPassword;
            xcs.set_proxy_pass( talk_base::CryptString(ss5pass) );
        }

        strcpy( pAppObj->m_szConfigPath, ptLoginSettings->szConfigPath );


        pAppObj->m_pXmppThread->Login(xcs);
        pAppObj->m_pXmppThread->Start();

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError SetReady( HANDLE  hApp )
    {
        if ( 0 == hApp )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        // ����Ѿ������ˣ�����
        if ( pAppObj->m_bUpperAppReady )
        {
            return LIBJINGLE_ERR_OK;
        }

        pAppObj->m_pXmppThread->UpperReady();

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError Logout( HANDLE hApp )
    {
        if ( 0 == hApp )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        cFuncLock  cLock( &pGlobal->m_arrLocks[LOCK_INDEX_APP_VECTOR] );

        std::vector< CLibJinglexApp * >::iterator it;
        for ( it = pGlobal->m_vApps.begin(); it != pGlobal->m_vApps.end(); it++ )
        {
            CLibJinglexApp * p = *it;
            if ( p == pAppObj )
            {
                break;
            }
        }

        // ���û�ҵ�
        if ( it == pGlobal->m_vApps.end() )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����


        // �̰߳�ȫ
        JLock( &pAppObj->m_external_lock );

        if ( pAppObj->m_pXmppThread )
        {
            pAppObj->m_pXmppThread->Disconnect();
            // pAppObj->m_pXmppThread->Quit();
            pAppObj->m_pXmppThread->Join();

            pAppObj->Clear();

            delete pAppObj->m_pXmppThread;
            pAppObj->m_pXmppThread = 0;
        }

        JUnlock( &pAppObj->m_external_lock );

        // ����state change����
        pGlobal->m_vApps.erase( it );
        delete pAppObj;

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError SendMsg( HANDLE hApp, const PTJid pJid, const char * szMsg, BOOL bUtf8Encode /* = TRUE */ )
    {
        if ( 0 == hApp || 0 == pJid || 0 == szMsg )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        unsigned long dwMsgLen = strlen( szMsg );
        if ( dwMsgLen >= MAX_MSG_LENGTH )
        {
            return LIBJINGLE_ERR_TOO_LONG_STRING;
        }

        // ����stanza
        std::string str;
        char buf[MAX_BUFF_SIZE];

        buzz::XmlElement * msg  = new buzz::XmlElement(buzz::QN_MESSAGE );
        msg->SetAttr( buzz::QN_TYPE, "chat" );

        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        msg->SetAttr( buzz::QN_ID,   randid );

        str  = pJid->sNode;
        str += '@';
        str += pJid->sDomain;
        msg->SetAttr( buzz::QN_TO,  AnsiToUtf8( str.c_str(), buf, MAX_BUFF_SIZE ) );

        // buzz::XmlElement * body = new buzz::XmlElement(buzz::QN_BODY);
        buzz::XmlElement * body = new buzz::XmlElement(buzz::QN_XBODY, true);
        msg->AddElement( body );

        body->SetAttr( buzz::QN_VER, "1.0" );

        if ( bUtf8Encode )
        {
            char szUtf8[MAX_MSG_LENGTH*2];
            AnsiToUtf8( szMsg, szUtf8, MAX_MSG_LENGTH*2 );
            body->SetBodyText(szUtf8);
        }
        else
        {
            body->SetBodyText(szMsg);
        }

        // char state: active
        buzz::XmlElement * chatstate = new buzz::XmlElement(buzz::QN_CHAR_ACTIVE, true);
        msg->AddElement(chatstate);


        // ���浱ǰ�Լ�����ϵ�˵�״̬Ϊactive
        vMemberGroups & vContacts = pAppObj->m_MemberGroups;
        vMemberGroups::iterator it_contact = find_if( vContacts.begin(), vContacts.end(), FindMemberGroupObj( pJid ) ) ;

        PTMemberGroup pContact = 0;
        // ���û�о����
        if ( it_contact == vContacts.end() )
        {
            pContact = new TMemberGroup;
            memcpy( &pContact->m_tJid, pJid, sizeof(LibJingleEx::TJid) );
            pContact->m_tJid.sResource[0] = '\0';

            vContacts.push_back( pContact );
        }
        else
        {
            pContact = *it_contact;
        }
        pContact->m_eToCharStatus = LibJingleEx::CHAR_STATUS_ACTIVE;
        // 
        
        pAppObj->m_pXmppThread->SendStanza( msg );

        return LIBJINGLE_ERR_OK;
    }



    LIBJINGLE_API ELibjingleError SendMsg( HANDLE hApp, const PTJid pJid, const TDmsChatContent * ptMsg, 
                                           unsigned long * pdwSendTime /*= 0*/, unsigned long dwContextId /*= 0*/ )
    {
        if ( 0 == hApp || 0 == pJid || 0 == ptMsg )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        if ( !pAppObj->m_bGetServerTime )
        {
            return LIBJINGLE_ERR_NOT_GET_SERVER_TIME;
        }



        // ����stanza
        std::string str;
        char buf[MAX_BUFF_SIZE];

        buzz::XmlElement * msg  = new buzz::XmlElement(buzz::QN_MESSAGE);
        msg->SetAttr( buzz::QN_TYPE, "chat" );

        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        msg->SetAttr( buzz::QN_ID,   randid );

        str  = pJid->sNode;
        str += '@';
        str += pJid->sDomain;
        msg->SetAttr( buzz::QN_TO,  AnsiToUtf8( str.c_str(), buf, MAX_BUFF_SIZE ) );


        msg->AddElement( ConstructChatBody( ptMsg, pAppObj, pdwSendTime ) );  

        // char state: active
        buzz::XmlElement * chatstate = new buzz::XmlElement(buzz::QN_CHAR_ACTIVE, true);
        msg->AddElement(chatstate);

        // receipts
        buzz::XmlElement * receipts_request = new buzz::XmlElement( buzz::QN_RECEIPT_REQUEST, true );
        msg->AddElement( receipts_request );


        // ���浱ǰ�Լ�����ϵ�˵�״̬Ϊactive
        vMemberGroups & vContacts = pAppObj->m_MemberGroups;
        vMemberGroups::iterator it_contact = find_if( vContacts.begin(), vContacts.end(), FindMemberGroupObj( pJid ) ) ;

        PTMemberGroup pContact = 0;
        // ���û�о����
        if ( it_contact == vContacts.end() )
        {
            pContact = new TMemberGroup;
            memcpy( &pContact->m_tJid, pJid, sizeof(LibJingleEx::TJid) );
            pContact->m_tJid.sResource[0] = '\0';

            vContacts.push_back( pContact );
        }
        else
        {
            pContact = *it_contact;
        }
        pContact->m_eToCharStatus = LibJingleEx::CHAR_STATUS_ACTIVE;
        // 

        PTIqId  ptIqId = new TIqId;
        ptIqId->dwType = IQ_TYPE_P2P_MESSAGE;
        strcpy( ptIqId->szIqId, randid );
        ptIqId->dwContextId = dwContextId;
        ptIqId->dwSubType   = 0;
        ptIqId->dwTick      = GetCurTick();

        // pAppObj->m_pXmppThread->SendStanza( msg );
        pAppObj->m_pXmppThread->SendStanza( ptIqId,  msg );

        return LIBJINGLE_ERR_OK;
    }


    LIBJINGLE_API ELibjingleError SendCharState( HANDLE hApp, const PTJid pJid, ECharStatus eCharState )
    {
        // ֻ����composing��paused״̬
        if ( 0 == hApp || 0 == pJid || eCharState > CHAR_STATUS_PAUSED || CHAR_STATUS_ACTIVE == eCharState )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }



        // ���char state�Ƿ�ı�
        vMemberGroups & vContacts = pAppObj->m_MemberGroups;
        vMemberGroups::iterator it_contact = find_if( vContacts.begin(), vContacts.end(), FindMemberGroupObj( pJid ) ) ;

        PTMemberGroup pContact = 0;

        // ���û�о����
        if ( it_contact == vContacts.end() )
        {
            pContact = new TMemberGroup;
            memcpy( &pContact->m_tJid, pJid, sizeof(LibJingleEx::TJid) );
            pContact->m_tJid.sResource[0] = '\0';

            vContacts.push_back( pContact );
        }
        else
        {
            pContact = *it_contact;
        }


        // û�иı�
        if ( pContact->m_eToCharStatus == eCharState )
        {
            return LIBJINGLE_ERR_CHAR_STATE_NOT_CHANGED;
        }
        // 





        // ����stanza
        std::string str;
        char buf[MAX_BUFF_SIZE];

        buzz::XmlElement * msg  = new buzz::XmlElement(buzz::QN_MESSAGE);
        //msg->SetAttr( buzz::QN_TYPE, "chat" );
        msg->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );
        

        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        msg->SetAttr( buzz::QN_ID,   randid );

        str  = pJid->sNode;
        str += '@';
        str += pJid->sDomain;
        msg->SetAttr( buzz::QN_TO,  AnsiToUtf8( str.c_str(), buf, MAX_BUFF_SIZE ) );



        // char state
        buzz::XmlElement * chatstate = 0;
        if ( CHAR_STATUS_COMPOSING == eCharState )
        {
            chatstate = new buzz::XmlElement(buzz::QN_CHAR_COMPOSING, true);
        }
        else if ( CHAR_STATUS_PAUSED == eCharState )
        {
            chatstate = new buzz::XmlElement(buzz::QN_CHAR_PAUSED, true);
        }

        assert( chatstate );
        msg->AddElement( chatstate );


        assert ( pContact );
        // ���µ�ǰ�Լ�����ϵ�˵�״̬
        pContact->m_eToCharStatus = eCharState;

        pAppObj->m_pXmppThread->SendStanza( msg );

        return LIBJINGLE_ERR_OK;
    }


    LIBJINGLE_API ELibjingleError SendPicture( HANDLE hApp, const PTJid pJid, 
        const unsigned char * pbyPicture, unsigned long dwSize )
    {
        if ( 0 == hApp || 0 == pJid || 0 == pbyPicture || 0 == dwSize )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }


        unsigned long dwMultiple = dwSize / MAX_MSG_LENGTH;
        unsigned long dwResidue  = dwSize % MAX_MSG_LENGTH;
        if ( dwResidue > 0 )
        {
            dwMultiple++;
        }

        char buf[MAX_BUFF_SIZE];
        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );

        // �ֿ��η��ͳ�ȥ
        unsigned long dwIndex;
        for ( dwIndex = 0; dwIndex < dwMultiple; dwIndex++ )
        {
            // ����stanza
            std::string str;

            buzz::XmlElement * msg  = new buzz::XmlElement(buzz::QN_MESSAGE);
            // ����type
            msg->SetAttr( buzz::QN_TYPE, "picture" );
            // ����id
            msg->SetAttr( buzz::QN_ID,   randid );    

            str  = pJid->sNode;
            str += '@';
            str += pJid->sDomain;
            msg->SetAttr( buzz::QN_TO,  AnsiToUtf8( str.c_str(), buf, MAX_BUFF_SIZE ) );

            // buzz::XmlElement * body = new buzz::XmlElement(buzz::QN_BODY);
            buzz::XmlElement * body = new buzz::XmlElement(buzz::QN_XBODY);
            msg->AddElement( body );

            unsigned long dwBase64Size = MAX_MSG_LENGTH * 2;
            char          szBase64[MAX_MSG_LENGTH * 2];
            // δ�����һ��
            if ( dwIndex < dwMultiple - 1 )
            {
                EncodeBase64( szBase64, dwBase64Size, pbyPicture + MAX_MSG_LENGTH * dwIndex, MAX_MSG_LENGTH );
            }
            // ���һ��
            else
            {
                EncodeBase64( szBase64, dwBase64Size, pbyPicture + MAX_MSG_LENGTH * dwIndex, dwResidue );
                body->SetAttr( buzz::QName(true, "", "end" ), "true" );
            }

            body->SetBodyText( szBase64 );

            if ( 0 == dwIndex )
            {
                sprintf( buf, "%u", dwSize );
                body->SetAttr( buzz::QName(true, "", "size" ), buf );
            }

            pAppObj->m_pXmppThread->SendStanza( msg );
        }

        return LIBJINGLE_ERR_OK;
    }







    LIBJINGLE_API ELibjingleError MoveFriend( HANDLE hApp, const PTJid pJid, 
        const char * szGroupSrc, const char * szGroupDst )
    {
        if ( 0 == hApp || 0 == pJid || 0 == szGroupSrc || 0 == szGroupDst )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }   

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }


        cFuncLock cLock( &pAppObj->m_lock );
        vMemberGroups::iterator it_find = find_if(  pAppObj->m_MemberGroups.begin(), 
            pAppObj->m_MemberGroups.end(),
            FindMemberGroupObj(pJid) );
        // ������������jid����
        if ( it_find == pAppObj->m_MemberGroups.end() )
        {
            return LIBJINGLE_ERR_NO_SUCH_FRIEND;
        }

        PTMemberGroup p = *it_find;

        std::vector<const char*> vGroups;
        // ����һ��vGroups
        std::copy( p->m_Groups.begin(), p->m_Groups.end(), back_inserter(vGroups) );

        // �鿴����Դ��
        std::vector<const char*>::iterator it_str = 
            find_if( vGroups.begin(), vGroups.end(), FindStringObj(szGroupSrc) );

        // ���û��Դ��
        if ( vGroups.end() == it_str )
        {
            return LIBJINGLE_ERR_NO_SUCH_GROUP;    
        }

        // ���Դ���Ŀ����ͬ���������ƶ�
        if ( 0 == strcmp(szGroupSrc, szGroupDst) )
        {
            return LIBJINGLE_ERR_OK;
        }

        // ��vGroupsɾ��Դ��
        vGroups.erase( it_str );

        // ����Ŀ����
        vGroups.push_back( szGroupDst );


        // send stanza
        std::string str;
        char buf[MAX_BUFF_SIZE];

        buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);
        iq->SetAttr( buzz::QN_TYPE, "set" );

        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        iq->SetAttr( buzz::QN_ID,   randid );

        buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_ROSTER_QUERY, true );
        iq->AddElement( query );

        buzz::XmlElement * item  = new buzz::XmlElement( buzz::QN_ROSTER_ITEM );
        query->AddElement( item );

        str  = pJid->sNode;
        if ( str.length() > 0 )
        {
            str += '@';
        }
        str += pJid->sDomain;

        item->SetAttr( buzz::QN_JID,  AnsiToUtf8( str.c_str(), buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_NAME, AnsiToUtf8( p->m_tData.szName, buf, MAX_BUFF_SIZE ) );

#if !DMS_CONTACTS_SWITCH && ROSTER_CUSTOM_ATTR
        if ( '\0' != p->m_tData.szBitRate[0] )
        {
            buzz::XmlElement * bitrate = new buzz::XmlElement( buzz::QN_KEDA_ROSTER_BITRATE, true );
            item->AddElement( bitrate );
            bitrate->SetAttr( buzz::QN_VALUE, AnsiToUtf8(p->m_tData.szBitRate, buf, MAX_BUFF_SIZE)  );
        }
        
        if( p->m_tData.nType > 0 )
        {
            buzz::XmlElement * type_ele = new buzz::XmlElement( buzz::QN_KEDA_ROSTER_TYPE, true );
            item->AddElement( type_ele );

            SNPRINTF( buf, MAX_BUFF_SIZE, "%d", p->m_tData.nType );
            type_ele->SetAttr( buzz::QN_VALUE, buf );
        }
#endif

        for ( it_str = vGroups.begin(); it_str != vGroups.end(); it_str++ )
        {
            buzz::XmlElement * group  = new buzz::XmlElement( buzz::QN_ROSTER_GROUP );
            item->AddElement( group );

            char szUtf8[MAX_BUFF_SIZE];
            AnsiToUtf8( *it_str, szUtf8, MAX_BUFF_SIZE );

            group->AddText( szUtf8 );
        }

        buzz::XmppClient * client = pAppObj->m_pXmppThread->client();
        // client->SendStanza( iq );
        // delete iq;
        pAppObj->m_pXmppThread->SendStanza( iq );

        return LIBJINGLE_ERR_OK;
    }




    LIBJINGLE_API ELibjingleError AddFriend(  HANDLE hApp, const PTJid pJid, 
        const char * szGroup /* = 0 */, 
        const char * szNickName /* = 0 */,
        const char * szBirtate /* = 0 */, 
        unsigned long dwType /* = 0 */ )
    {
        if ( 0 == hApp || 0 == pJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        unsigned long dwLen = 0;
        if ( 0 != szGroup )
        {
            dwLen = strlen( szGroup );
            if ( dwLen >= MAX_GROUP_NAME_LEN )
            {
                return LIBJINGLE_ERR_TOO_LONG_STRING;
            }
        }
        else
        {
            szGroup = DEFAULT_GROUP_NAME;
        }

        if ( 0 != szNickName )
        {
            dwLen = strlen( szNickName );
            if ( dwLen >= MAX_MEMBER_NAME_LEN )
            {
                return LIBJINGLE_ERR_TOO_LONG_STRING;
            }
        }
        else
        {
            if ( 0 == strlen(pJid->sNode) )
            {
                szNickName = pJid->sDomain;
            }
            else
            {
                szNickName = pJid->sNode;
            }
        }

        int  nSubscription = 0;
        std::vector<const char*> vGroups;

        cFuncLock cLock( &pAppObj->m_lock );
        vMemberGroups::iterator it_find = find_if( pAppObj->m_MemberGroups.begin(), 
            pAppObj->m_MemberGroups.end(),
            FindMemberGroupObj(pJid) );
        // �Ѿ����ڣ�����ӵ��µ���
        if ( it_find != pAppObj->m_MemberGroups.end() )
        {
            PTMemberGroup p = *it_find;

            std::vector<const char*>::iterator it_str = 
                find_if( p->m_Groups.begin(), p->m_Groups.end(), FindStringObj(szGroup) );
            // �Ѿ�����ͬ������ͺ�����
            if ( it_str != p->m_Groups.end() )
            {
                return LIBJINGLE_ERR_ALREADY_EXISTS;
            }
            else
            {
                std::copy( p->m_Groups.begin(), p->m_Groups.end(), back_inserter(vGroups) );
            }

            nSubscription = p->m_tData.nSubscription;
        }

        vGroups.push_back( szGroup );

        // send stanza
        std::string str;
        char buf[MAX_BUFF_SIZE];

        buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);
        iq->SetAttr( buzz::QN_TYPE, "set" );

        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        iq->SetAttr( buzz::QN_ID,   randid );

        buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_ROSTER_QUERY, true );
        iq->AddElement( query );

        buzz::XmlElement * item  = new buzz::XmlElement( buzz::QN_ROSTER_ITEM );
        query->AddElement( item );

        str  = pJid->sNode;
        if ( str.length() > 0 )
        {
            str += '@';
        }
        str += pJid->sDomain;

        item->SetAttr( buzz::QN_JID,  AnsiToUtf8( str.c_str(), buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_NAME, AnsiToUtf8( szNickName, buf, MAX_BUFF_SIZE ) );

        std::vector<const char*>::iterator it_str;
        for ( it_str = vGroups.begin(); it_str != vGroups.end(); it_str++ )
        {
#if DMS_CONTACTS_SWITCH
            if ( '\0' == *(*it_str) )
            {
                continue;
            }
#endif

            buzz::XmlElement * group  = new buzz::XmlElement( buzz::QN_ROSTER_GROUP );
            item->AddElement( group );

            char szUtf8[MAX_BUFF_SIZE];
            AnsiToUtf8( *it_str, szUtf8, MAX_BUFF_SIZE );

            group->AddText( szUtf8 );
        }

        if ( szBirtate )
        {
            buzz::XmlElement * bitrate = new buzz::XmlElement( buzz::QN_KEDA_ROSTER_BITRATE, true );
            item->AddElement( bitrate );
            bitrate->SetAttr( buzz::QN_VALUE, AnsiToUtf8(szBirtate, buf, MAX_BUFF_SIZE) );
        }

        if ( dwType > 0 )
        {
            buzz::XmlElement * type_ele = new buzz::XmlElement( buzz::QN_KEDA_ROSTER_TYPE, true );
            item->AddElement( type_ele );

            SNPRINTF( buf, MAX_BUFF_SIZE, "%u", dwType );
            type_ele->SetAttr( buzz::QN_VALUE, buf );
        }

        buzz::XmppClient * client = pAppObj->m_pXmppThread->client();
        pAppObj->m_pXmppThread->SendStanza( iq );


        if ( DMS_MEMBER_TYPE_JID == dwType && !(nSubscription & SUB_STATE_SUBSCRIBED)  )
        {
            // ����״̬
            buzz::XmlElement * presence = new buzz::XmlElement( buzz::QN_PRESENCE );
            presence->SetAttr( buzz::QN_TO,   AnsiToUtf8( str.c_str(), buf, MAX_BUFF_SIZE ) );
            presence->SetAttr( buzz::QN_TYPE, "subscribe" );
            pAppObj->m_pXmppThread->SendStanza( presence );
            // ���Ͷ���״̬����
        }

#if !DMS_CONTACTS_SWITCH
        LibJingleEx::TDmsMember   tDmsMember;
        tDmsMember.eType = (EDmsMemberType)dwType;
        LibJingleEx::TJid2StrJid( *pJid, tDmsMember.szJid, sizeof(tDmsMember.szJid) );
        tDmsMember.dwGroupId = SPECIAL_GROUP_SN;
        if ( szNickName )
        {
            strncpy( tDmsMember.szName, szNickName, sizeof(tDmsMember.szName) );
        }
        if ( szBirtate )
        {
            strncpy( tDmsMember.szReserved[RESERVERD_BITRATE_INDEX], szBirtate, sizeof(tDmsMember.szReserved[RESERVERD_BITRATE_INDEX]) );
        }
        _DmsAddMember( pAppObj, &tDmsMember );
#endif

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError AcceptFriend( HANDLE hApp, const PTJid pJid, BOOL bAccept /* = TRUE */ )
    {
        if ( 0 == hApp || 0 == pJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        char buf[MAX_BUFF_SIZE];

        // ����
        if ( bAccept )
        {
            buzz::XmlElement * presence = new buzz::XmlElement( buzz::QN_PRESENCE );

            char szJid[256];
            LibJingleEx::TJid2StrJid( *pJid, szJid, 256 );

            presence->SetAttr( buzz::QN_TO,   AnsiToUtf8( szJid, buf, MAX_BUFF_SIZE ) );
            presence->SetAttr( buzz::QN_TYPE, "subscribed" );

            buzz::XmppClient * client = pAppObj->m_pXmppThread->client();
            // client->SendStanza( presence );
            // delete presence;
            pAppObj->m_pXmppThread->SendStanza( presence );


            // ���Լ�����״̬�ķǹ�ע��ϵ�ˣ�ȡ��֮
            std::vector<LibJingleEx::PTJid> & v = pAppObj->m_vQueryStatusContact;
            std::vector<LibJingleEx::PTJid>::iterator it = find_if( v.begin(), v.end(), FindBareJidObj(pJid) );
            if ( it != v.end() )
            {
                delete *it;
                v.erase( it );
            }

        }
        // �ܾ�
        else
        {
            std::string sTemplate = "<presence to='%s' type='unsubscribed'/>";

            char buf_tmp[4096];
            SNPRINTF( buf_tmp, 4096, sTemplate.c_str(), AnsiToUtf8( GetJid( *pJid ), buf, MAX_BUFF_SIZE ) );

            buzz::XmppClient * client = pAppObj->m_pXmppThread->client();
            // client->SendRaw( buf );
            pAppObj->m_pXmppThread->SendRaw( buf_tmp );

        }
        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError DelFriend( HANDLE hApp, const PTJid pJid, const char * szGroup /* = 0 */,
        BOOL  bUnsubscribed /* = TRUE */ )
    {
        if ( 0 == hApp || 0 == pJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );


        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );
        vMemberGroups::iterator it_find = find_if( pAppObj->m_MemberGroups.begin(), 
            pAppObj->m_MemberGroups.end(), FindMemberGroupObj(pJid) );
        // ������
        if ( it_find == pAppObj->m_MemberGroups.end() )
        {
            return LIBJINGLE_ERR_NO_SUCH_FRIEND;
        }

        PTMemberGroup p = *it_find;

        char szId[MAX_RAND_ID_LENGTH];
        char buf[MAX_BUFF_SIZE];

        if ( szGroup )
        {
            std::vector<const char*>::iterator it_str = find_if( p->m_Groups.begin(), p->m_Groups.end(),
                FindStringObj(szGroup) );
            if ( it_str == p->m_Groups.end() )
            {
                return LIBJINGLE_ERR_NO_SUCH_FRIEND;
            }

            if ( p->m_Groups.size() > 1 )
            {
                buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
                iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
                iq->SetAttr( buzz::QN_ID, GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

                buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_ROSTER_QUERY, true );
                iq->AddElement( query );

                buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ROSTER_ITEM, true );
                query->AddElement( item );

                item->SetAttr( buzz::QN_JID,  AnsiToUtf8( GetJid(p->m_tJid), buf, MAX_BUFF_SIZE ) );
                item->SetAttr( buzz::QN_NAME, AnsiToUtf8( p->m_tData.szName, buf, MAX_BUFF_SIZE ) );

#if !DMS_CONTACTS_SWITCH && ROSTER_CUSTOM_ATTR
                buzz::XmlElement * bitrate = new buzz::XmlElement( buzz::QN_KEDA_ROSTER_BITRATE, true );
                item->AddElement( bitrate );
                bitrate->SetAttr( buzz::QN_VALUE, AnsiToUtf8(p->m_tData.szBitRate, buf, MAX_BUFF_SIZE)  );
#endif

                std::vector<const char*>::iterator it;
                for ( it = p->m_Groups.begin(); it != p->m_Groups.end(); it++ )
                {
                    if ( it == it_str )
                    {
                        continue;
                    }

                    buzz::XmlElement * group = new buzz::XmlElement( buzz::QN_ROSTER_GROUP, true );
                    item->AddElement( group );
                    group->AddText( AnsiToUtf8( *it, buf, MAX_BUFF_SIZE ) );
                }

                pAppObj->m_pXmppThread->SendStanza( iq );
            }
            else
            {
                buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
                iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
                iq->SetAttr( buzz::QN_ID, GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

                buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_ROSTER_QUERY, true );
                iq->AddElement( query );

                buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ROSTER_ITEM, true );
                query->AddElement( item );

                item->SetAttr( buzz::QN_JID,  AnsiToUtf8( GetJid(p->m_tJid), buf, MAX_BUFF_SIZE ) );
                item->SetAttr( buzz::QN_NAME, AnsiToUtf8( p->m_tData.szName, buf, MAX_BUFF_SIZE ) );

#if !DMS_CONTACTS_SWITCH && ROSTER_CUSTOM_ATTR
                buzz::XmlElement * bitrate = new buzz::XmlElement( buzz::QN_KEDA_ROSTER_BITRATE, true );
                item->AddElement( bitrate );
                bitrate->SetAttr( buzz::QN_VALUE, AnsiToUtf8(p->m_tData.szBitRate, buf, MAX_BUFF_SIZE)  );
#endif

                if ( bUnsubscribed )
                {
                    item->SetAttr( buzz::QN_SUBSCRIPTION, "remove" );
                }
                // ���û�ж����Լ���״̬
                else if ( !(p->m_tData.nSubscription & SUB_STATE_SUBSCRIBE) ) 
                {
                    item->SetAttr( buzz::QN_SUBSCRIPTION, "remove" );
                    // ����Ҫ��unscribe��
                    bUnsubscribed = TRUE;
                }

                pAppObj->m_pXmppThread->SendStanza( iq );

                if ( !bUnsubscribed )
                {
                    Unsubscribe( hApp, pJid );
                }

            }
        }
        else
        {
            buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
            iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
            iq->SetAttr( buzz::QN_ID, GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

            buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_ROSTER_QUERY, true );
            iq->AddElement( query );

            buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ROSTER_ITEM, true );
            query->AddElement( item );

            item->SetAttr( buzz::QN_JID, AnsiToUtf8( GetJid(p->m_tJid), buf, MAX_BUFF_SIZE ) );
            if ( bUnsubscribed )
            {
                item->SetAttr( buzz::QN_SUBSCRIPTION, "remove" );
            }
            // ���û�ж����Լ���״̬
            else if ( !(p->m_tData.nSubscription & SUB_STATE_SUBSCRIBE) ) 
            {
                item->SetAttr( buzz::QN_SUBSCRIPTION, "remove" );
                // ����Ҫ��unscribe��
                bUnsubscribed = TRUE;
            }

            pAppObj->m_pXmppThread->SendStanza( iq );

            if ( !bUnsubscribed )
            {
                Unsubscribe( hApp, pJid );
            }
        }

#if !DMS_CONTACTS_SWITCH
        // ɾ��private��ϵ��
        if ( bUnsubscribed )
        {
            std::vector<unsigned long>  vIds;
            pAppObj->GetDmsMemberIdsByJid( pJid, vIds );

            std::vector<unsigned long> ::iterator  it_id;
            for ( it_id = vIds.begin(); it_id != vIds.end(); it_id++ )
            {
                _DmsDelMember( pAppObj, *it_id );
            }
        }
#endif

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError SetFriendNick( HANDLE hApp, const PTJid pJid, const char * szNick, const char * szBitrate /*= 0*/ )
    {
        if ( 0 == hApp || 0 == pJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        if ( 0 == szNick )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );


        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );
        vMemberGroups::iterator it_find = find_if( pAppObj->m_MemberGroups.begin(), 
            pAppObj->m_MemberGroups.end(),FindMemberGroupObj(pJid) );
        // ������
        if ( it_find == pAppObj->m_MemberGroups.end() )
        {
            return LIBJINGLE_ERR_NO_SUCH_FRIEND;
        }

        PTMemberGroup p = *it_find;

        assert( p->m_Groups.size() > 0 );

        char szId[MAX_RAND_ID_LENGTH];
        char buf[MAX_BUFF_SIZE];

        buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
        iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
        iq->SetAttr( buzz::QN_ID, GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

        buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_ROSTER_QUERY, true );
        iq->AddElement( query );

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ROSTER_ITEM, true );
        query->AddElement( item );

        item->SetAttr( buzz::QN_JID,  AnsiToUtf8( GetJid(p->m_tJid), buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_NAME, AnsiToUtf8( szNick, buf, MAX_BUFF_SIZE ) );

        std::vector<const char*>::iterator it;
        for ( it = p->m_Groups.begin(); it != p->m_Groups.end(); it++ )
        {
            buzz::XmlElement * group = new buzz::XmlElement( buzz::QN_ROSTER_GROUP, true );
            item->AddElement( group );
            group->AddText( AnsiToUtf8( *it, buf, MAX_BUFF_SIZE ) );
        }

#if !DMS_CONTACTS_SWITCH
        if ( szBitrate )
        {
            buzz::XmlElement * bitrate = new buzz::XmlElement(buzz::QN_KEDA_ROSTER_BITRATE, true );
            item->AddElement( bitrate );
            bitrate->SetAttr( buzz::QN_VALUE, AnsiToUtf8(szBitrate, buf, MAX_BUFF_SIZE) );
        }

        if ( p->m_tData.nType > 0  )
        {
            buzz::XmlElement * type_ele = new buzz::XmlElement( buzz::QN_KEDA_ROSTER_TYPE, true );
            item->AddElement( type_ele );

            SNPRINTF( buf, MAX_BUFF_SIZE, "%d", p->m_tData.nType );
            type_ele->SetAttr( buzz::QN_VALUE, buf );
        }
#endif

        
        pAppObj->m_pXmppThread->SendStanza( iq );


#if !DMS_CONTACTS_SWITCH
        std::vector<unsigned long> vIds;
        pAppObj->GetDmsMemberIdsByJid( &p->m_tJid, vIds );

        if ( vIds.size() > 0 )
        {
            LibJingleEx::TDmsMember  tDmsMember;
            tDmsMember.dwMemberId = vIds[0];
            tDmsMember.eType      = (EDmsMemberType)p->m_tData.nType;
            LibJingleEx::TJid2StrJid( p->m_tJid, tDmsMember.szJid, sizeof(tDmsMember.szJid) );
            strncpy( tDmsMember.szName, szNick, sizeof(tDmsMember.szName) );
            if ( szBitrate )
            {
                strncpy( tDmsMember.szReserved[RESERVERD_BITRATE_INDEX], szBitrate, LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
            }
            _DmsModifyMember( pAppObj, &tDmsMember );
        }
#endif

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError SetStatus( HANDLE hApp, EPresence e, const char * szDescription /*= 0*/, BOOL bCall /*= TRUE*/, unsigned long dwSwitch /*= -1*/ )
    {
        if ( 0 == hApp )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        if ( !IfHasBit( dwSwitch, SET_STATUS_SWITCH_INDEX_PRESENCE ) && !IfHasBit( dwSwitch, SET_STATUS_SWITCH_INDEX_CALL ) )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ���presence switch��Ч
        if ( IfHasBit( dwSwitch, SET_STATUS_SWITCH_INDEX_PRESENCE ) )
        {
            if ( e > PRESENCE_CHAT )
            {
                return LIBJINGLE_ERR_INVALID_ARGUMENT;
            }

            if ( PRESENCE_NONE == e || PRESENCE_OFFLINE == e )
            {
                return LIBJINGLE_ERR_INVALID_ARGUMENT;
            }
        }


        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( 0 == pGlobal )
        {
            return LIBJINGLE_ERR_ERROR;
        }

        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );


        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }


        // ���status�Ƿ�ı�, ����ı�ŷ��ͳ�ȥ
        BOOL  bChanged = FALSE;

        if ( IfHasBit( dwSwitch, SET_STATUS_SWITCH_INDEX_PRESENCE ) )
        {
            if ( pAppObj->m_ePres != e )
            {
                pAppObj->m_ePres = e;
                bChanged = TRUE;
            }
        }

        if ( IfHasBit( dwSwitch, SET_STATUS_SWITCH_INDEX_CALL ) )
        {
            if ( pAppObj->m_bCall != bCall )
            {
                pAppObj->m_bCall = bCall;
                bChanged = TRUE;
            }
        }

        // ���û�иı䣬����
        if ( !bChanged )
        {
            return LIBJINGLE_ERR_OK;
        }



        char buf[MAX_BUFF_SIZE];

        buzz::XmlElement * presence = new buzz::XmlElement(buzz::QN_PRESENCE);

        const char * szPresence = GetPresShowString( pAppObj->m_ePres );
        if ( szPresence )
        {
            buzz::XmlElement * show   = new buzz::XmlElement(buzz::QN_SHOW );
            presence->AddElement( show );
            show->SetBodyText( szPresence );
        }

        if ( szDescription )
        {
            buzz::XmlElement * status     = new buzz::XmlElement(buzz::QN_STATUS);
            presence->AddElement( status );
            status->SetBodyText( AnsiToUtf8( szDescription, buf, MAX_BUFF_SIZE ) );
        }

        buzz::XmlElement * priority = new buzz::XmlElement( buzz::QN_PRIORITY );
        presence->AddElement( priority );
        priority->SetBodyText("1");

        std::string   ability;
        if ( GetAbilityString( ability, pAppObj ) )
        {
            presence->AddElement( new buzz::XmlElement( buzz::QN_CAPS_C, true) );
            presence->AddAttr( buzz::QN_NODE, "http://www.google.com/xmpp/client/caps", 1);
            MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
            presence->AddAttr( buzz::QN_VER,  buf, 1);
            presence->AddAttr( buzz::QN_EXT, ability, 1);
        }  


        vRoom::iterator it_room;

        // �������䷢��״̬
        for ( it_room = pAppObj->m_Rooms.begin(); it_room != pAppObj->m_Rooms.end(); it_room++ )
        {
            PTRoom pRoom = *it_room;

#ifdef SUPPORT_STATUS_ROOM
            if (  (ROOM_STATE_AVAILABLE == pRoom->m_eState) ||  (ROOM_TYPE_VISUAL == pRoom->m_nType) )
#else
            if ( (ROOM_STATE_AVAILABLE == pRoom->m_eState) || ( ROOM_TYPE_SPECIAL == pRoom->m_nType ) || ( ROOM_TYPE_VISUAL == pRoom->m_nType ) )
#endif
            {
                buzz::XmlElement * room_presence = new buzz::XmlElement( *presence );

                std::string sTmp = pRoom->m_szRoomId;
                sTmp += "@";
                sTmp += pRoom->m_szGroupChatServiceName;

                room_presence->SetAttr( buzz::QN_TO, AnsiToUtf8( sTmp.c_str(), buf, MAX_BUFF_SIZE ) );

                buzz::XmlElement * x = new buzz::XmlElement( buzz::QN_MUC_X, true );
                room_presence->AddElement( x );

                pAppObj->m_pXmppThread->SendStanza( room_presence );
            }
        }

        std::vector<LibJingleEx::PTJid>::iterator it_c;
        for ( it_c = pAppObj->m_vQueryStatusContact.begin(); it_c != pAppObj->m_vQueryStatusContact.end(); it_c++ )
        {
            LibJingleEx::PTJid ptJid = *it_c;

            buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
            LibJingleEx::TJid2StrJid( *ptJid, buf, MAX_BUFF_SIZE );
            message->SetAttr( buzz::QN_TO,   buf );
            message->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );

            buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
            message->AddElement( operation );
            operation->SetAttr( buzz::QN_TYPE, "info status" );

            buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
            operation->AddElement( item );

            if ( szPresence )
            {
                item->SetAttr( buzz::QN_SHOW, szPresence );
            }

            if ( ability.length() > 0  )
            {
                item->SetAttr( buzz::QN_EXT, ability );
            }

            pAppObj->m_pXmppThread->SendStanza( message );
        }
        

        pAppObj->m_pXmppThread->SendStanza( presence );
        pAppObj->m_bSendStatus = TRUE;

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�Subscribe
    // ���ܣ�    ����Ժ��ѵĹ�ע
    /********************************************************/
    LIBJINGLE_API ELibjingleError Subscribe( HANDLE hApp, const PTJid pJid )
    {
        if ( 0 == hApp || 0 == pJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );


        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        std::string sTemplate = "<presence to='%s' type='subscribe'/>";

        LibJingleEx::TJid tJid;
        memcpy( &tJid, pJid, sizeof(LibJingleEx::TJid) );
        tJid.sResource[0] = '\0';

        char buf[MAX_BUFF_SIZE];
        char buf_tmp[MAX_BUFF_SIZE];
        SNPRINTF( buf, MAX_BUFF_SIZE, sTemplate.c_str(), AnsiToUtf8( GetJid( tJid ), buf_tmp, MAX_BUFF_SIZE ) );
        // client->SendRaw( buf );
        pAppObj->m_pXmppThread->SendRaw( buf );



        std::vector<PTUnscribedContact> & v = pAppObj->m_vUnscribedContacts;
        std::vector<PTUnscribedContact>::iterator it;
        for ( it = v.begin(); it != v.end(); )
        {
            PTUnscribedContact p = *it;
            // ���bare jid��ͬ��unscribed ��ϵ�˲�����ͼping֮
            if ( IfBareJidEqual( p->tJid, *pJid ) )
            {
                delete p;
                it = v.erase( it );
            }
            else
            {
                it++;
            }
        }

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�Unsubscribed
    // ���ܣ�    ȡ���Լ������ѹ�ע
    /********************************************************/
    LIBJINGLE_API ELibjingleError Unsubscribed( HANDLE hApp, const PTJid pJid )
    {
        if ( 0 == hApp || 0 == pJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );


        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        std::string sTemplate = "<presence to='%s' type='unsubscribed'/>";

        LibJingleEx::TJid tJid;
        memcpy( &tJid, pJid, sizeof(LibJingleEx::TJid) );
        tJid.sResource[0] = '\0';

        char buf[MAX_BUFF_SIZE];
        char buf_tmp[MAX_BUFF_SIZE];
        SNPRINTF( buf, MAX_BUFF_SIZE, sTemplate.c_str(), AnsiToUtf8( GetJid( tJid ), buf_tmp, MAX_BUFF_SIZE ) );
        // client->SendRaw( buf );
        pAppObj->m_pXmppThread->SendRaw( buf );

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�Unsubscribe
    // ���ܣ�    ȡ���Ժ��ѵĹ�ע
    /********************************************************/
    LIBJINGLE_API ELibjingleError Unsubscribe( HANDLE hApp, const PTJid pJid )
    {
        if ( 0 == hApp || 0 == pJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );


        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        std::string sTemplate = "<presence to='%s' type='unsubscribe'/>";

        LibJingleEx::TJid tJid;
        memcpy( &tJid, pJid, sizeof(LibJingleEx::TJid) );
        tJid.sResource[0] = '\0';

        char buf[MAX_BUFF_SIZE];
        char buf_tmp[MAX_BUFF_SIZE];
        SNPRINTF( buf, MAX_BUFF_SIZE, sTemplate.c_str(), AnsiToUtf8( GetJid( tJid ), buf_tmp, MAX_BUFF_SIZE ) );
        // client->SendRaw( buf );
        pAppObj->m_pXmppThread->SendRaw( buf );

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�GetPresence
    // ���ܣ�    ������ӡpresence����������
    /********************************************************/
    LIBJINGLE_API const char * GetPresence( EPresence e )
    {
        switch ( e )
        {
        case PRESENCE_NONE:
            return "NONE";
            break;

        case PRESENCE_OFFLINE:
            return "OFFLINE";
            break;

        case PRESENCE_XA:
            return "FAR AWAY";
            break;

        case PRESENCE_AWAY:
            return "AWAY";
            break;

        case PRESENCE_DND:
            return "DON'T BOTHER";
            break;

        case PRESENCE_ONLINE:
            return "ONLINE";
            break;

        case PRESENCE_CHAT:
            return "CHAT";
            break;

        default:
            return "";
            break;
        }
    }

    LIBJINGLE_API const char * GetSubscription( int s )
    {
        switch( s )
        {
        case SUB_STATE_SUBSCRIBE:
            return "from";

        case SUB_STATE_SUBSCRIBED:
            return "to";

        case (SUB_STATE_SUBSCRIBE | SUB_STATE_SUBSCRIBED):
            return "both";

        case 0:
            return "none";

        default:
            return "unknown";
        }
    }

    LIBJINGLE_API const char * GetAsk( int nAsk )
    {
        switch( nAsk )
        {
        case ASK_STATE_NONE:
            return "none";

        case ASK_STATE_SUBSCRIBE:
            return "subscribe";

        default:
            return "unknown";
        }
    }



    /*=====================================  Ⱥ��  =============================================*/


    /********************************************************/
    // �������ƣ�CreateUniqueRoom
    // ���ܣ�    ��conference�����ϴ���һ����Ψһ�ŵķ���
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    ��Ӧ�Ļص� pOnCreateUniqueRoom
    /********************************************************/
    LIBJINGLE_API ELibjingleError CreateUniqueRoom( HANDLE hApp, HROOM * phRoom, const char * szName /*= 0*/, 
        BOOL bPersistent /*= FALSE*/ )
    {
        if ( 0 == hApp || 0 == phRoom )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        unsigned long dwMaxRoomCnt = ( bPersistent ? MAX_PERSISTENT_CHATROOM_CNT : MAX_TEMP_CHATROOM_CNT );
        unsigned long dwRoomCnt = pAppObj->GetChatRoomsCnt( ROOM_TYPE_NORMAL, bPersistent );
        // ������˴�����������
        if ( dwRoomCnt >= dwMaxRoomCnt )
        {
            return ( bPersistent ? LIBJINGLE_ERR_REACH_MAX_PERSISTENT_CHATROOM_LIMIT : LIBJINGLE_ERR_REACH_MAX_TEMP_CHATROOM_LIMIT );
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );
        if ( 0 == client->ConferenceJid().length() )
        {
            return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        }

        buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
        iq->SetAttr( buzz::QN_TYPE, buzz::STR_GET );

        char szQueryId[MAX_RAND_ID_LENGTH];
        GenerateRandomId( szQueryId, MAX_RAND_ID_LENGTH );

        iq->SetAttr( buzz::QN_ID, szQueryId );    
        iq->SetAttr( buzz::QN_TO, client->ConferenceJid() );

        buzz::XmlElement * unique = new buzz::XmlElement( buzz::QN_MUC_UNIQUE, true );
        iq->AddElement( unique );

        // client->SendStanza(iq);
        // delete iq;
        pAppObj->m_pXmppThread->SendStanza( iq );

        PTRoom pRoom = new TRoom(pAppObj);
        strcpy( pRoom->m_szQueryId, szQueryId );
        strcpy( pRoom->m_szGroupChatServiceName, client->ConferenceJid().c_str() );
        pRoom->m_bPersistent = bPersistent;
        pRoom->m_nType       = ROOM_TYPE_NORMAL ;
        pRoom->m_eState = ROOM_STATE_ON_QUERY_ID;          // ����unique id״̬

        // ��������������
        if ( szName )
        {
            strncpy( pRoom->m_szName, szName, sizeof( pRoom->m_szName ) );
        }

        pRoom->m_bUnique = TRUE;

        pAppObj->m_Rooms.push_back( pRoom );
        *phRoom = pRoom;

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API ELibjingleError CreateOrEnterRoom( HANDLE hApp, HROOM * phRoom, const char * szId, 
        const char * szName /* = 0 */ )
    {
        if ( 0 == hApp || 0 == phRoom || 0 == szId )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );
        if ( 0 == client->ConferenceJid().length() )
        {
            return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        }



        // ����Ѿ������ˣ��ͷ���OK
        vRoom::iterator  it = find_if( pAppObj->m_Rooms.begin(), pAppObj->m_Rooms.end(), 
                                       FindRoomByFullId( szId, client->ConferenceJid().c_str() ) );
        if ( it != pAppObj->m_Rooms.end() )
        {
            *phRoom = *it;
            return LIBJINGLE_ERR_OK;
        }





        // ��������뷿��
        buzz::XmlElement * presence = new buzz::XmlElement( buzz::QN_PRESENCE );

        char buf[MAX_BUFF_SIZE];
        std::string sTmp;
        sTmp  = szId;
        sTmp += '@';
        sTmp += client->ConferenceJid();
        sTmp += '/';
        // sTmp += client->jid().BareJid().Str();
        sTmp += client->jid().Str();

        presence->SetAttr( buzz::QN_TO, AnsiToUtf8( sTmp.c_str(), buf, MAX_BUFF_SIZE ) );

        const char * szPresence = GetPresShowString( pAppObj->m_ePres );
        if ( szPresence )
        {
            buzz::XmlElement * show   = new buzz::XmlElement(buzz::QN_SHOW );
            presence->AddElement( show );
            show->SetBodyText( szPresence );
        }

        buzz::XmlElement * priority = new buzz::XmlElement( buzz::QN_PRIORITY );
        presence->AddElement( priority );
        priority->SetBodyText("1");

        std::string  ability;
        if ( GetAbilityString( ability, pAppObj ) )
        {
            presence->AddElement( new buzz::XmlElement( buzz::QN_CAPS_C, true) );
            presence->AddAttr( buzz::QN_NODE, "http://www.google.com/xmpp/client/caps", 1);
            MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
            presence->AddAttr( buzz::QN_VER,  buf, 1);
            presence->AddAttr( buzz::QN_EXT, ability, 1);
        }

        buzz::XmlElement * x = new buzz::XmlElement( buzz::QN_MUC_X, true );
        presence->AddElement( x );

        // ����ÿ�����ȡ��20��stanza
        buzz::XmlElement * history = new buzz::XmlElement( buzz::QN_HISTORY, true );
        x->AddElement( history );

        //char szMaxStanzas[256] = { 0 };
        //SNPRINTF( szMaxStanzas, 256, "%u", MAX_MUC_HISTORY_STANZAS );
        //history->SetAttr( buzz::QN_MAX_STANZAS, szMaxStanzas );
        history->SetAttr( buzz::QN_MAX_STANZAS, "0" );
        // history->SetAttr( buzz::QN_SINCE, SINCE_STAMP );
        // END ����ÿ�����ȡ��20��stanza


        pAppObj->m_pXmppThread->SendStanza( presence );
        // ���� ��������뷿��


        PTRoom pRoom = new TRoom(pAppObj);
        strcpy( pRoom->m_szRoomId, szId );
        strcpy( pRoom->m_szGroupChatServiceName, client->ConferenceJid().c_str() );
        pRoom->m_bPersistent = FALSE;
        pRoom->m_nType       = ROOM_TYPE_VISUAL;
        pRoom->m_eState      = ROOM_STATE_ON_PRESENCE;

        // ��������������
        if ( szName )
        {
            strncpy( pRoom->m_szName, szName, sizeof( pRoom->m_szName ) );
        }


        pAppObj->m_Rooms.push_back( pRoom );
        *phRoom = pRoom;


        return LIBJINGLE_ERR_OK;
    }


    LIBJINGLE_API ELibjingleError EnterPersistentRoom( HANDLE hApp,  PTJid  ptRoomJid )
    {
        if ( 0 == hApp || 0 == ptRoomJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );
        if ( 0 == client->ConferenceJid().length() )
        {
            return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        }

        
        vRoom::iterator  it = find_if( pAppObj->m_Rooms.begin(), pAppObj->m_Rooms.end(), FindRoomByFullId(ptRoomJid->sNode, ptRoomJid->sDomain) );
        if ( it == pAppObj->m_Rooms.end() )
        {
            return LIBJINGLE_ERR_NO_SUCH_CHATROOM;
        }

        PTRoom pRoom = *it;
        if ( ROOM_STATE_HOLD_PERSISTENT != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_NOT_ALLOWED;
        }



        char buf[MAX_BUFF_SIZE];

        buzz::XmlElement * presence = new buzz::XmlElement( buzz::QN_PRESENCE );

        std::string sTmp;

        sTmp  = pRoom->m_szRoomId;
        sTmp += '@';
        sTmp += pRoom->m_szGroupChatServiceName;
        sTmp += '/';
        sTmp += client->jid().Str();

        presence->SetAttr( buzz::QN_TO, AnsiToUtf8( sTmp.c_str(), buf, MAX_BUFF_SIZE ) );

        const char * szPresence = GetPresShowString( pAppObj->m_ePres );
        if ( szPresence )
        {
           buzz::XmlElement * show   = new buzz::XmlElement(buzz::QN_SHOW );
           presence->AddElement( show );
           show->SetBodyText( szPresence );
        }

        buzz::XmlElement * priority = new buzz::XmlElement( buzz::QN_PRIORITY );
        presence->AddElement( priority );
        priority->SetBodyText("1");

        std::string  ability;
        if ( GetAbilityString( ability, pAppObj ) )
        {
           presence->AddElement( new buzz::XmlElement( buzz::QN_CAPS_C, true) );
           presence->AddAttr( buzz::QN_NODE, "http://www.google.com/xmpp/client/caps", 1);
           MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
           presence->AddAttr( buzz::QN_VER,  buf, 1);
           presence->AddAttr( buzz::QN_EXT, ability, 1);
        }

        buzz::XmlElement * x = new buzz::XmlElement( buzz::QN_MUC_X, true );
        presence->AddElement( x );


        buzz::XmlElement * history = new buzz::XmlElement( buzz::QN_HISTORY, true );
        x->AddElement( history );

        if ( pRoom->m_bFirst )
        {
            history->SetAttr( buzz::QN_MAX_STANZAS, "0" );
        }
        else
        {
            // ����ÿ�����ȡ��20��stanza
            char szMaxStanzas[256] = { 0 };
            SNPRINTF( szMaxStanzas, 256, "%u", MAX_MUC_HISTORY_STANZAS );
            history->SetAttr( buzz::QN_MAX_STANZAS, szMaxStanzas );
            // END ����ÿ�����ȡ��20��stanza
            history->SetAttr( buzz::QN_SINCE, SINCE_STAMP );
        }        

        pAppObj->m_pXmppThread->SendStanza( presence );
        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�InviteParticipant
    // ���ܣ�    ���������
    // ����ֵ��  0��������Ϣ�ɹ�
    /********************************************************/
    LIBJINGLE_API ELibjingleError InviteParticipant( HROOM hRoom, const PTJid ptJid )
    {
        if ( 0 == hRoom || 0 == ptJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        //if ( 0 == client->ConferenceJid().length() )
        //{
        //    return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        //}

        // ���仹û��׼����
        if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_ROOM_NOT_READY;
        }

        // ֻ�������߲�������
        if ( AFFILIATION_OWNER != pRoom->m_eAffiliation )
        {
            return LIBJINGLE_ERR_NOT_OWNER;
        }

        // ����������Լ�
        if ( client->jid().node() == ptJid->sNode && client->jid().domain() == ptJid->sDomain )
        {
            return LIBJINGLE_ERR_CANNOT_INVITE_SELF;
        }


        vRoomMember::iterator it_m;
        it_m = find_if ( pRoom->m_vMembers.begin(), pRoom->m_vMembers.end(), FindRoomMemberByBareJidObj(ptJid) );
        if ( it_m != pRoom->m_vMembers.end() )
        {
            return LIBJINGLE_ERR_INVITEE_ALREADY_IN_ROOM;
        }

        if ( ROOM_TYPE_NORMAL == pRoom->m_nType )
        {
            // �����û�л�ȡroster items��������������
            if ( !pRoom->m_bGetMembers )
            {
                return LIBJINGLE_ERR_CHATROOM_ROSTER_NOT_READY;
            }

            if ( pRoom->m_vRoster.size() >= CHATROOM_MAX_MEMBERS_COUNT )
            {
                return LIBJINGLE_ERR_CHATROOM_ROSTER_IS_FULL;
            }
        }

        std::string sTmp;
        char szFullUtf8RoomId[256] = { 0 };
        char szUtf8Jid[256] = { 0 };

        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );

        sTmp  = pRoom->m_szRoomId;
        sTmp += '@';
        sTmp += client->ConferenceJid();

        AnsiToUtf8( sTmp.c_str(), szFullUtf8RoomId, 256  );

        message->SetAttr( buzz::QN_TO, szFullUtf8RoomId );

        buzz::XmlElement * x = new buzz::XmlElement( buzz::QN_MUC_USER_X, true);
        message->AddElement( x );

        buzz::XmlElement * invite = new buzz::XmlElement( buzz::QName(true,"","invite"), true );
        x->AddElement(invite);

        AnsiToUtf8( GetJid(*ptJid), szUtf8Jid, 256  );
        invite->SetAttr(buzz::QN_TO, szUtf8Jid );

        buzz::XmlElement * reason = new buzz::XmlElement( buzz::QN_REASON, true );
        invite->AddElement( reason );

        char  buf_name[MAX_BUFF_SIZE] = { 0 };
        unsigned long dwSize = MAX_BUFF_SIZE;
        EncodeBase64( buf_name, dwSize, (unsigned char *)pRoom->m_szName, LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN );

        char  buf_persistent[INVITE_PRESISTENT_BUF_SIZE] = { 0 };
        if ( pRoom->m_bPersistent )
        {
            strcpy( buf_persistent, "1" );
        }
        else
        {
            strcpy( buf_persistent, "0" );
        }

        char  buf_room_type[INVITE_ROOMTYPE_BUF_SIZE] = { 0 };
        SNPRINTF( buf_room_type, INVITE_ROOMTYPE_BUF_SIZE, "%d", pRoom->m_nType );


        // ����"kedacom.com���"
        char  buf_all[MAX_BUFF_SIZE] = { 0 };
        SNPRINTF( buf_all, MAX_BUFF_SIZE, "%s%s%s%s", MAGIC_PREFIX, buf_persistent, buf_room_type, buf_name );

        reason->SetBodyText( buf_all );

        // client->SendStanza(message);
        // delete message;
        pAppObj->m_pXmppThread->SendStanza( message );


        char szRandId[32];
        buzz::XmlElement * iq = 0;
        buzz::XmlElement * query = 0;
        buzz::XmlElement * item = 0;

#if !CHATROOM_SUPPORT_MEMBER_AFFILIATION
        // ����ǳ־��Է��䣬������owner�����ó�ԱaffiliationΪadmin
        GenerateRandomId( szRandId, 32 );

        iq = new buzz::XmlElement( buzz::QN_IQ );
        iq->SetAttr( buzz::QN_ID,   szRandId );
        iq->SetAttr( buzz::QN_TO,   szFullUtf8RoomId );
        iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );

        query = new buzz::XmlElement( buzz::QN_MUC_ADMIN_QUERY, true );
        iq->AddElement( query );

        item = new buzz::XmlElement( buzz::QN_ITEM, true );
        query->AddElement( item );
        item->SetAttr( buzz::QN_MUC_AFFILIATION, "admin" );
        item->SetAttr( buzz::QN_JID, szUtf8Jid );

        pAppObj->m_pXmppThread->SendStanza( iq );
        // END(����ǳ־��Է��䣬������owner�����ó�ԱaffiliationΪadmin)
#endif


        // ����㲥�����˳�Ա
        message = new buzz::XmlElement( buzz::QN_MESSAGE );
        GenerateRandomId( szRandId, 32 );
        message->SetAttr( buzz::QN_ID, szRandId );
        message->SetAttr( buzz::QN_TO, szFullUtf8RoomId );
        message->SetAttr( buzz::QN_TYPE, "groupchat" );

        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );
        operation->SetAttr( buzz::QN_TYPE, "chatroom add member" );

        item = new buzz::XmlElement( buzz::QN_ITEM );
        operation->AddElement( item );

        item->SetAttr( buzz::QN_JID, szUtf8Jid );
#if !CHATROOM_SUPPORT_MEMBER_AFFILIATION
        item->SetAttr( buzz::QN_MUC_AFFILIATION, "admin" );
#else
        item->SetAttr( buzz::QN_MUC_AFFILIATION, "member" );
#endif

        pAppObj->m_pXmppThread->SendStanza( message );
        // END( ����㲥�����˳�Ա )


        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�AcceptInvite
    // ���ܣ�    �Ƿ���ܲμ������ҵ�����
    // ����ֵ��  0��������Ϣ�ɹ�
    /********************************************************/
    LIBJINGLE_API ELibjingleError AcceptInvite( HROOM hRoom, BOOL bAccepted )
    {
        if ( 0 == hRoom )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        //if ( 0 == client->ConferenceJid().length() )
        //{
        //    return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        //}

        // �������״̬����ROOM_STATE_ON_INVITING������Ϊ����һ���ظ�������
        // ����LIBJINGLE_ERR_INVITEE_ALREADY_IN_ROOM
        if ( ROOM_STATE_ON_INVITING != pRoom->m_eState )
        {
            assert( ROOM_STATE_UNVAILABLE != pRoom->m_eState );
            return LIBJINGLE_ERR_INVITEE_ALREADY_IN_ROOM;
        }

        // ����Ǿܾ�( ���ٴ����ķ����ڴ� )
        if ( !bAccepted )
        {
            pAppObj->m_pXmppThread->DeleteRoom( pRoom );
            return LibJingleEx::LIBJINGLE_ERR_OK;
        }

        // ��ע��ʱ�ĺ͹̶�����������Ŀ�޶�( ����ĺ����صĲ����� )
        if ( ROOM_TYPE_NORMAL == pRoom->m_nType )
        {
            unsigned long dwMaxRoomCnt = ( pRoom->m_bPersistent ? MAX_PERSISTENT_CHATROOM_CNT : MAX_TEMP_CHATROOM_CNT );
            unsigned long dwRoomCnt    = pAppObj->GetChatRoomsCnt( ROOM_TYPE_NORMAL, pRoom->m_bPersistent );

            // ������˴�����������
            if ( dwRoomCnt >= dwMaxRoomCnt )
            {
                return ( pRoom->m_bPersistent ? LIBJINGLE_ERR_REACH_MAX_PERSISTENT_CHATROOM_LIMIT : LIBJINGLE_ERR_REACH_MAX_TEMP_CHATROOM_LIMIT );
            }
        }


        std::string sTmp;
        char buf[MAX_BUFF_SIZE];

        if ( bAccepted )
        {
            // ��������
            buzz::XmlElement * presence = new buzz::XmlElement( buzz::QN_PRESENCE );

            sTmp  = pRoom->m_szRoomId;
            sTmp += '@';
            sTmp += pRoom->m_szGroupChatServiceName;
            sTmp += '/';
            sTmp += client->jid().Str();

            presence->SetAttr( buzz::QN_TO, AnsiToUtf8( sTmp.c_str(), buf, MAX_BUFF_SIZE ) );

            const char * szPresence = GetPresShowString( pAppObj->m_ePres );
            if ( szPresence )
            {
                buzz::XmlElement * show   = new buzz::XmlElement(buzz::QN_SHOW );
                presence->AddElement( show );
                show->SetBodyText( szPresence );
            }

            buzz::XmlElement * priority = new buzz::XmlElement( buzz::QN_PRIORITY );
            presence->AddElement( priority );
            priority->SetBodyText("1");

            std::string  ability;
            if ( GetAbilityString( ability, pAppObj ) )
            {
                presence->AddElement( new buzz::XmlElement( buzz::QN_CAPS_C, true) );
                presence->AddAttr( buzz::QN_NODE, "http://www.google.com/xmpp/client/caps", 1);
                MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
                presence->AddAttr( buzz::QN_VER,  buf, 1);
                presence->AddAttr( buzz::QN_EXT, ability, 1);
            }

            buzz::XmlElement * x = new buzz::XmlElement( buzz::QN_MUC_X, true );
            presence->AddElement( x );

            // ����ÿ�����ȡ��20��stanza
            buzz::XmlElement * history = new buzz::XmlElement( buzz::QN_HISTORY, true );
            x->AddElement( history );

            char szMaxStanzas[256] = { 0 };
            SNPRINTF( szMaxStanzas, 256, "%u", MAX_MUC_HISTORY_STANZAS );
            history->SetAttr( buzz::QN_MAX_STANZAS, szMaxStanzas );
            // END ����ÿ�����ȡ��20��stanza
            history->SetAttr( buzz::QN_SINCE, SINCE_STAMP );

            pAppObj->m_pXmppThread->SendStanza( presence );
            //�������� ����

            pRoom->m_eState = ROOM_STATE_ON_ACCEPTING;

            // ��һ���뷿�䣬������������Ϣ
            pRoom->m_bNoOfflineMsg = TRUE;
        }

        return LIBJINGLE_ERR_OK;
    }


    LIBJINGLE_API ELibjingleError GetRoomConfigure( HROOM hRoom )
    {
        if ( 0 == hRoom )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );

        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        iq->SetAttr( buzz::QN_ID, randid );

        char buf[256];
        char buf_tmp[MAX_BUFF_SIZE];
        SNPRINTF( buf, 256, "%s@%s", pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
        iq->SetAttr( buzz::QN_TO, AnsiToUtf8( buf, buf_tmp, MAX_BUFF_SIZE ) );

        iq->SetAttr( buzz::QN_TYPE, "get" );

        buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_DISCO_INFO_QUERY, true );
        iq->AddElement( query );

        pAppObj->m_pXmppThread->SendStanza( iq );

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�GrantOwner
    // ���ܣ�    �������Ϊ������(����Ҫ���Լ�Ϊ�����������)
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    ���˻�������ߺ��Լ���ʧȥ������Ȩ��
    //           (1������ֻ����1��������)
    /********************************************************/
    LIBJINGLE_API ELibjingleError GrantOwner( HROOM hRoom, const PTJid ptJid )
    {
        if ( 0 == hRoom || 0 == ptJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        //if ( 0 == client->ConferenceJid().length() )
        //{
        //    return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        //}

        // ���仹û��׼����
        if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_ROOM_NOT_READY;
        }

        // �㲻��owner
        if ( AFFILIATION_OWNER != pRoom->m_eAffiliation )
        {
            return LIBJINGLE_ERR_NOT_OWNER;
        }

        vRoomMember::iterator it_m;

        vRoomMember & v = pRoom->m_vMembers;

        for ( it_m = v.begin(); it_m != v.end(); it_m++ )
        {
            PTRoomMember p = *it_m;

            if ( 0 == strcmp( p->m_tJid.sNode, ptJid->sNode ) && 0 == strcmp( p->m_tJid.sDomain, ptJid->sDomain ) )
            {
                break;
            }
        }

        // û���ҵ��ò�����
        if ( it_m == pRoom->m_vMembers.end() )
        {
            return LIBJINGLE_ERR_NO_SUCH_PARTICIPANT;
        }



        std::string sTemplate = "<iq id='%s' to='%s@%s' type='set'>"
            "<query xmlns='http://jabber.org/protocol/muc#admin'>"
            "<item affiliation='owner' jid='%s'/>"
            "</query>"
            "</iq>";

        char buf[4096];
        char buf_tmp[MAX_BUFF_SIZE];
        char szId[MAX_RAND_ID_LENGTH];
        SNPRINTF( buf, 4096,sTemplate.c_str(), GenerateRandomId(szId,MAX_RAND_ID_LENGTH), pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName,
            AnsiToUtf8( GetJid( *ptJid ), buf_tmp, MAX_BUFF_SIZE ) );

        // client->SendRaw( buf );
        pAppObj->m_pXmppThread->SendRaw( buf );

        sTemplate = "<iq id='%s' to='%s@%s' type='set'>"
            "<query xmlns='http://jabber.org/protocol/muc#admin'>"
            "<item affiliation='none' jid='%s'/>"
            "</query>"
            "</iq>";
        SNPRINTF( buf, 4096, sTemplate.c_str(), GenerateRandomId(szId, MAX_RAND_ID_LENGTH), pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName,
            AnsiToUtf8( client->jid().Str().c_str(), buf_tmp, MAX_BUFF_SIZE ) );

        // client->SendRaw( buf );
        pAppObj->m_pXmppThread->SendRaw( buf );

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�QuitRoom
    // ���ܣ�    �뿪����
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    
    /********************************************************/
    LIBJINGLE_API ELibjingleError QuitRoom( HROOM hRoom )
    {
        if ( 0 == hRoom )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        //if ( 0 == client->ConferenceJid().length() )
        //{
        //    return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        //}

        // ���仹û��׼����
        if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_ROOM_NOT_READY;
        }

        std::string sTemplate = "<presence to='%s@%s' type='unavailable'/>";

        char buf[4096];
        SNPRINTF( buf, 4096,sTemplate.c_str(), pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );

        // client->SendRaw( buf );
        pAppObj->m_pXmppThread->SendRaw( buf );

        pRoom->m_bQuitOnSelfOwn = TRUE;

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�DestroyRoom
    // ���ܣ�    ���ٷ���(����Ҫ���Լ��Ƿ����������)
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    
    /********************************************************/
    LIBJINGLE_API ELibjingleError DestroyRoom( HROOM hRoom )
    {
        if ( 0 == hRoom )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        //if ( 0 == client->ConferenceJid().length() )
        //{
        //    return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        //}

        // ���仹û��׼����
        if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_ROOM_NOT_READY;
        }

        if ( AFFILIATION_OWNER != pRoom->m_eAffiliation )
        {
            return LIBJINGLE_ERR_NOT_OWNER;
        }

        std::string sTemplate = "<iq id='%s' to='%s@%s' type='set'>"
            "<query xmlns='http://jabber.org/protocol/muc#owner'>"
            "<destroy jid='%s@%s'/>"
            "</query>"
            "</iq>";

        char buf[4096];
        char szId[MAX_RAND_ID_LENGTH];
        SNPRINTF( buf, 4096,sTemplate.c_str(), GenerateRandomId(szId,MAX_RAND_ID_LENGTH), pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName,
            pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );

        // client->SendRaw( buf );
        pAppObj->m_pXmppThread->SendRaw( buf );

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�SendRoomMessage
    // ���ܣ�    �ڷ��䷢�͹㲥
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    
    /********************************************************/
    LIBJINGLE_API ELibjingleError SendRoomMessage( HROOM hRoom, const char * szMessage, BOOL bUtf8Encode /* = TRUE */ )
    {
        if ( 0 == hRoom || 0 == szMessage )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        //if ( 0 == client->ConferenceJid().length() )
        //{
        //    return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        //}

        // ���仹û��׼����
        if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_ROOM_NOT_READY;
        }

        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );

        char szId[MAX_RAND_ID_LENGTH];
        char buf[MAX_BUFF_SIZE];

        message->SetAttr( buzz::QN_TYPE, "groupchat" );
        message->SetAttr( buzz::QN_ID,   GenerateRandomId(szId,MAX_RAND_ID_LENGTH,0) );

        std::string sTmp;
        sTmp  = pRoom->m_szRoomId;
        sTmp += '@';
        sTmp += pRoom->m_szGroupChatServiceName;

        message->SetAttr( buzz::QN_TO, sTmp );

        // buzz::XmlElement * body = new buzz::XmlElement( buzz::QN_BODY );
        buzz::XmlElement * body = new buzz::XmlElement( buzz::QN_XBODY, true );
        message->AddElement( body );

        body->SetAttr( buzz::QN_VER, "1.0" );

        if ( bUtf8Encode )
        {
            body->SetBodyText( AnsiToUtf8( szMessage, buf, MAX_BUFF_SIZE ) );
        }
        else
        {
            body->SetBodyText( szMessage );
        }


        // client->SendStanza( message );
        // delete message;
        pAppObj->m_pXmppThread->SendStanza( message );

        return LIBJINGLE_ERR_OK;
    }





    LIBJINGLE_API ELibjingleError SendRoomMessage( HROOM hRoom, const TDmsChatContent * ptMsg, unsigned long * pdwSendTime /*= 0*/, 
                                                   unsigned long dwContextId /*= 0*/ )
    {
        if ( 0 == hRoom || 0 == ptMsg )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        //if ( 0 == client->ConferenceJid().length() )
        //{
        //    return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        //}

        // ���仹û��׼����
        if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_ROOM_NOT_READY;
        }

        if ( !pAppObj->m_bGetServerTime )
        {
            return LIBJINGLE_ERR_NOT_GET_SERVER_TIME;
        }


        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );

        char szId[MAX_RAND_ID_LENGTH];

        message->SetAttr( buzz::QN_TYPE, "groupchat" );
        message->SetAttr( buzz::QN_ID,   GenerateRandomId(szId,MAX_RAND_ID_LENGTH,0) );

        std::string sTmp;
        sTmp  = pRoom->m_szRoomId;
        sTmp += '@';
        sTmp += pRoom->m_szGroupChatServiceName;

        message->SetAttr( buzz::QN_TO, sTmp );

        // buzz::XmlElement * body = new buzz::XmlElement( buzz::QN_BODY );
        message->AddElement( ConstructChatBody( ptMsg, pAppObj, pdwSendTime ) );

        LibJingleEx::TChatRoomId  tRoomId;
        LibJingleEx::GetRoomId( hRoom, &tRoomId );
        BOOL  bScreened = FALSE;
        LibJingleEx::IsScreenedChatroom( (LibJingleEx::HANDLE)pAppObj, &tRoomId, &bScreened );

        // ���������
        if ( bScreened )
        {
            pAppObj->m_pXmppThread->SendStanza( message );
        }
        // ���û������
        else
        {
            PTIqId  ptIqId = new TIqId;
            ptIqId->dwType = IQ_TYPE_GROUPCHAT_MESSAGE;
            strcpy( ptIqId->szIqId, szId );
            ptIqId->dwContextId = dwContextId;
            ptIqId->dwSubType   = 0;
            ptIqId->pRoom       = pRoom;
            ptIqId->dwTick      = GetCurTick();

            pAppObj->m_pXmppThread->SendStanza( ptIqId, message );
        }
        
        return LIBJINGLE_ERR_OK;
    }



    /********************************************************/
    // �������ƣ�KickRoomMember
    // ���ܣ�    �߳������ҳ�Ա(��ͬ��jid�Ķ����߳�)
    // ����ֵ��  0��������Ϣ�ɹ�
    // ˵����    
    /********************************************************/
    LIBJINGLE_API ELibjingleError KickRoomMember( HROOM hRoom, const TJid * ptJid /* = 0 */ )
    {
        if ( 0 == hRoom )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();
        if ( 0 == client )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        // ���仹û��׼����
        if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_ROOM_NOT_READY;
        }



        // �鿴�Ƿ����Լ�
        BOOL  bKickSelf = FALSE;

        if ( 0 == ptJid )
        {
            bKickSelf = TRUE;
        }
        else 
        {
            if ( client->jid().node() == ptJid->sNode && client->jid().domain() == ptJid->sDomain )
            {
                bKickSelf = TRUE;
            }
        }
        // END �鿴�Ƿ����Լ�

        char szId[MAX_RAND_ID_LENGTH];
        char buf[MAX_BUFF_SIZE];

        std::string  room_id;
        room_id += pRoom->m_szRoomId;
        room_id += "@";
        room_id += pRoom->m_szGroupChatServiceName;

        std::string member_jid;
        std::string owner_jid;

        member_jid  = ptJid->sNode;
        member_jid += "@";
        member_jid += ptJid->sDomain;

        if ( bKickSelf )
        {
            // ���Լ�����

            // �㲻��owner
            if ( AFFILIATION_OWNER != pRoom->m_eAffiliation )
            {

#if CHATROOM_SUPPORT_MEMBER_AFFILIATION
                if ( AFFILIATION_MEMBER == pRoom->m_eAffiliation )
                {
                    // �����߳���stanza
                    std::string sTemplate = "<iq id='%s' to='%s' type='set'>"
                        "<query xmlns='http://jabber.org/protocol/muc#admin'>"
                        "<item affiliation='none' jid='%s' >"
                        "</item>"
                        "</query>"
                        "</iq>";
                    SNPRINTF( buf, MAX_BUFF_SIZE, sTemplate.c_str(), GenerateRandomId(szId,MAX_RAND_ID_LENGTH), room_id.c_str(), member_jid.c_str() );
                    pAppObj->m_pXmppThread->SendRaw( buf );
                    return LIBJINGLE_ERR_OK;
                }
#endif
                // ����ǳ־��Է���
                // if ( pRoom->m_bPersistent )
                {
                    // ׼��������¼
                    pRoom->m_bNeverIn = TRUE;
                    // END

                    // �ȼ���Ƿ��Ѿ������������
                    std::vector<LibJingleEx::PTJid>::iterator  ix;
                    for ( ix = pRoom->m_vQueryQuitMembers.begin(); ix != pRoom->m_vQueryQuitMembers.end(); ix++ )
                    {
                        LibJingleEx::PTJid  ptJid = *ix;
                        // ����Ѿ������˳���������
                        if ( client->jid().node() == ptJid->sNode && client->jid().domain() == ptJid->sDomain  )
                        {
                            return LibJingleEx::QuitRoom( hRoom );
                        }
                    }


                    // ��֪�������Լ������˳�
                    buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );

                    GenerateRandomId( szId, MAX_RAND_ID_LENGTH, 0 );
                    message->SetAttr( buzz::QN_ID, szId );

                    message->SetAttr( buzz::QN_TO, room_id );
                    message->SetAttr( buzz::QN_TYPE, "groupchat" );

                    buzz::XmlElement * subject = new buzz::XmlElement( buzz::QN_SUBJECT );
                    message->AddElement( subject );


                    std::string  subject_body;

                    for ( ix = pRoom->m_vQueryQuitMembers.begin(); ix != pRoom->m_vQueryQuitMembers.end(); ix++ )
                    {
                        LibJingleEx::PTJid  ptJid = *ix;

                        if ( subject_body.length() > 0 )
                        {
                            subject_body += " ";
                        }
                        subject_body += GetJid( *ptJid );
                    }

                    if ( subject_body.length() > 0 )
                    {
                        subject_body += " ";
                    }
                    subject_body += client->jid().BareJid().Str();

                    subject->SetBodyText( subject_body );

                    pAppObj->m_pXmppThread->SendStanza( message );
                    // END(��֪�������Լ������˳�)
                }
                return LibJingleEx::QuitRoom( hRoom );
            }
            // ����owner
            else
            {
                // ֱ�����ٷ���
                return LibJingleEx::DestroyRoom( hRoom );
            }
            // END

            return LIBJINGLE_ERR_OK;
        }
        else
        {
            // �������˴���

            // �㲻��owner
            if ( AFFILIATION_OWNER != pRoom->m_eAffiliation )
            {
                return LIBJINGLE_ERR_NOT_OWNER;
            }

            // �����߳���stanza
            std::string sTemplate = "<iq id='%s' to='%s' type='set'>"
                "<query xmlns='http://jabber.org/protocol/muc#admin'>"
                "<item affiliation='none' jid='%s' >"
                "</item>"
                "</query>"
                "</iq>";


            SNPRINTF( buf, MAX_BUFF_SIZE, sTemplate.c_str(), GenerateRandomId(szId,MAX_RAND_ID_LENGTH), room_id.c_str(), member_jid.c_str() );

            pAppObj->m_pXmppThread->SendRaw( buf );


            // ����㲥ɾ���˳�Ա
            buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );

            GenerateRandomId( szId, MAX_RAND_ID_LENGTH );
            message->SetAttr( buzz::QN_ID, szId );
            message->SetAttr( buzz::QN_TO, room_id );
            message->SetAttr( buzz::QN_TYPE, "groupchat" );

            buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
            message->AddElement( operation );
            operation->SetAttr( buzz::QN_TYPE, "chatroom delete member" );

            buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ITEM );
            operation->AddElement( item );

            item->SetAttr( buzz::QN_JID, member_jid );

            pAppObj->m_pXmppThread->SendStanza( message );
            // END( ����㲥ɾ���˳�Ա )

            return LIBJINGLE_ERR_OK;
            // END
        }

    }


    // ����Ⱥ����Ϣ�Ѷ�
    LIBJINGLE_API ELibjingleError SetGroupchatMsgRead( HROOM hRoom, const char * szLastReadMessageId /* = 0 */ )
    {
        //if ( 0 == hRoom )
        //{
        //    return LIBJINGLE_ERR_INVALID_ARGUMENT;
        //}

        //PTRoom pRoom = (PTRoom)hRoom;

        //// �����
        //CGlobalData * pGlobal = CGlobalData::GetInstance();
        //if ( !pGlobal->IfExistRoom(pRoom) )
        //{
        //    return LIBJINGLE_ERR_INVALID_HANDLE;
        //}
        //// END �����

        //if ( 0 == pRoom->m_pParent )
        //{
        //    return LIBJINGLE_ERR_INVALID_ROOM;
        //}

        //// ת��
        //CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        //// �̰߳�ȫ
        //cFuncLock cLock_( &pAppObj->m_external_lock );

        //// û�е�¼
        //if ( 0 == pAppObj->m_pXmppThread )
        //{
        //    return LIBJINGLE_ERR_NOT_LOGIN;
        //}

        //if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        //{
        //    return LIBJINGLE_ERR_DISCONNECTED;
        //}

        //buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        //cFuncLock cLock( &pAppObj->m_lock );

        ////if ( 0 == client->ConferenceJid().length() )
        ////{
        ////    return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        ////}

        //// ���仹û��׼����
        //if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        //{
        //    return LIBJINGLE_ERR_ROOM_NOT_READY;
        //}

        //// ������ǳ־��Է���
        //if ( !pRoom->m_bPersistent )
        //{
        //    return LIBJINGLE_ERR_NOT_PERSISTENT_ROOM;
        //}


        //unsigned long dwEndIndex = 0;

        //// �ӵ�ǰ�����������Ϣ��ʼ����Ϊ�Ѷ�
        //if ( 0 == szLastReadMessageId )
        //{
        //    dwEndIndex = pRoom->m_LatestMsgIds.GetEndPos();
        //}
        //// ��szLastReadMessageId֮ǰ����Ϣ�����Ϊ�Ѷ�
        //else
        //{
        //    dwEndIndex = pRoom->m_LatestMsgIds.FindIndex( szLastReadMessageId );
        //    // ����ҵ��ˣ��ƶ�pos������λ��
        //    if ( -1 != dwEndIndex )
        //    {
        //        dwEndIndex++;
        //    }
        //    else
        //    {
        //        dwEndIndex = pRoom->m_LatestMsgIds.GetEndPos();
        //    }
        //}

        //unsigned long  dwCnt = dwEndIndex - pRoom->m_LatestMsgIds.GetBeginPos();
        //// ���û�п�����ӵ��Ѷ���ʷ��¼���
        //if ( 0 == dwCnt )
        //{
        //    return LIBJINGLE_ERR_OK;
        //}

        //std::vector< CRoomReadMsgs *>::iterator it_find;
        //std::vector< CRoomReadMsgs *> & w = pAppObj->m_vRoomReadMsgs;

        //CRoomReadMsgs * pHistoryRoom = 0;
        //it_find = find_if( w.begin(), w.end(), FindHistoryRoom(pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName) );
        //// ���û���ҵ�
        //if ( it_find == w.end() )
        //{
        //    pHistoryRoom = new CRoomReadMsgs( pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName );
        //    pAppObj->m_vRoomReadMsgs.push_back( pHistoryRoom );
        //}
        //else
        //{
        //    pHistoryRoom = *it_find;
        //}

        //pHistoryRoom->m_room.Clear();



        //// �����µ���Ϣid����it_end�������20�������Ƶ��Ѷ���ʷ��¼��
        //unsigned long i;
        //for ( i = pRoom->m_LatestMsgIds.GetBeginPos(); i != dwEndIndex; i++ )
        //{
        //    pHistoryRoom->m_room.AddMessageId( pRoom->m_LatestMsgIds[i] );
        //    pAppObj->m_bRoomReadMsgsChanged = TRUE;
        //    pAppObj->m_dwChatroomHistoryVersion++;
        //    pAppObj->SaveChatRoomHistory2Local();
        //}
        //// END �����µ���Ϣ����it_end����20�������Ƶ�m_vLastMsgIds



        //char buf[MAX_BUFF_SIZE];

        //// ͬ��dms��������ͬresource���Լ�
        //buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
        //message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
        //message->SetAttr( buzz::QN_TYPE, "dms" );

        //buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        //message->AddElement( operation );
        //operation->SetAttr( buzz::QN_TYPE, "set_chatroom_read_messages" );

        //buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ITEM, true );
        //operation->AddElement( item );

        //item->SetAttr( buzz::QN_ID,               pRoom->m_szRoomId );
        //item->SetAttr( buzz::QN_CHATROOM_SERVICE, pRoom->m_szGroupChatServiceName );

        //if ( szLastReadMessageId )
        //{
        //    item->SetBodyText( szLastReadMessageId );
        //}

        //pAppObj->m_pXmppThread->SendStanza( message );
        // ����

        return LIBJINGLE_ERR_OK;
    }


    LIBJINGLE_API ELibjingleError SetRoomName( HROOM hRoom, const char * szName )
    {
        if ( 0 == hRoom || 0 == szName )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        //if ( 0 == client->ConferenceJid().length() )
        //{
        //    return LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
        //}

        // ���仹û��׼����
        if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_ROOM_NOT_READY;
        }

        if ( AFFILIATION_OWNER != pRoom->m_eAffiliation )
        {
            return LIBJINGLE_ERR_NOT_OWNER;
        }



        char szId[MAX_RAND_ID_LENGTH];
        char buf[MAX_BUFF_SIZE];
        std::string sTmp;

        // ���÷���Ĭ������Ϊpublic to all
        sTmp  = pRoom->m_szRoomId;
        sTmp += '@';
        sTmp += pRoom->m_szGroupChatServiceName;

        buzz::XmlElement * new_iq = new buzz::XmlElement( buzz::QN_IQ );
        new_iq->SetAttr( buzz::QN_TYPE, "set" );
        GenerateRandomId(szId,MAX_RAND_ID_LENGTH);
        new_iq->SetAttr( buzz::QN_ID, szId );
        new_iq->SetAttr( buzz::QN_TO, AnsiToUtf8( sTmp.c_str(), buf, MAX_BUFF_SIZE ) );

        buzz::XmlElement * new_query = new buzz::XmlElement( buzz::QN_MUC_QUERY_OWNER, true );
        new_iq->AddElement( new_query );

        buzz::XmlElement * new_x = new buzz::XmlElement( buzz::QName(true, "jabber:x:data", "x"), true );
        new_query->AddElement( new_x );
        new_x->SetAttr( buzz::QN_TYPE, "submit" );

        buzz::XmlElement * filed = new buzz::XmlElement( buzz::QName(true, "", "field" ), true );
        new_x->AddElement( filed );
        filed->SetAttr( buzz::QName(true, "", "var"), "FORM_TYPE" );

        buzz::XmlElement * new_value = new buzz::XmlElement( buzz::QN_VALUE  );
        filed->AddElement( new_value );
        new_value->SetBodyText("http://jabber.org/protocol/muc#roomconfig");


        // �޸ķ�����
        filed = new buzz::XmlElement( buzz::QName(true, "", "field" ), true );
        new_x->AddElement( filed );
        filed->SetAttr( buzz::QName(true, "", "var"), "muc#roomconfig_roomname" );
        filed->SetAttr( buzz::QN_TYPE,  "text-single" );
        filed->SetAttr( buzz::QN_LABEL, "Room title" );

        new_value = new buzz::XmlElement( buzz::QN_VALUE );
        filed->AddElement( new_value );
        strncpy( pRoom->m_szName, szName, LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN - 1 );
        pRoom->m_szName[LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN - 1] = '\0';
        new_value->SetBodyText( AnsiToUtf8(pRoom->m_szName, buf, MAX_BUFF_SIZE) );
        // ���� �޸ķ�����

        pAppObj->m_pXmppThread->SendStanza( new_iq );




        // ֪ͨ�����Ա �������޸�
        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );

        message->SetAttr( buzz::QN_TYPE, "groupchat" );
        message->SetAttr( buzz::QN_ID,   GenerateRandomId(szId,MAX_RAND_ID_LENGTH,0) );
        message->SetAttr( buzz::QN_TO,   AnsiToUtf8( sTmp.c_str(), buf, MAX_BUFF_SIZE ) );


        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );
        operation->SetAttr( buzz::QN_TYPE, "change room name" );

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ITEM );
        operation->AddElement( item );

        item->SetAttr( buzz::QN_ID,               AnsiToUtf8( pRoom->m_szRoomId, buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_CHATROOM_SERVICE, AnsiToUtf8( pRoom->m_szGroupChatServiceName, buf, MAX_BUFF_SIZE ) );
        item->SetBodyText( AnsiToUtf8( pRoom->m_szName, buf, MAX_BUFF_SIZE ) );

        pAppObj->m_pXmppThread->SendStanza( message );
        // END( ֪ͨ�����Ա �������޸� )


        return LIBJINGLE_ERR_OK;
    }



    // ���������﷢��ͼƬ
    LIBJINGLE_API ELibjingleError SendRoomPicture( HROOM hRoom, const char * szPicFilePathName, unsigned long dwPicSize )
    {
        if ( 0 == hRoom || 0 == szPicFilePathName || 0 == dwPicSize || dwPicSize > MAX_PICTURE_DATA_SIZE )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );


        // ���仹û��׼����
        if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        {
            return LIBJINGLE_ERR_ROOM_NOT_READY;
        }


        pAppObj->m_pXmppThread->SendRoomPicture( pRoom, szPicFilePathName, dwPicSize );


        return LIBJINGLE_ERR_OK;
    }




    // ֪ͨ�������Լ��ܾ�����
    // nReason =    LIBJINGLE_ERR_REACH_MAX_TEMP_CHATROOM_LIMIT
    //           �� LIBJINGLE_ERR_REACH_MAX_PERSISTENT_CHATROOM_LIMIT
    LIBJINGLE_API ELibjingleError NotifyInviterDeclineMsg( HANDLE hApp, const char * szRoomId, const char * szService, 
        PTJid ptInviter, int nReason, const char * szRoomName /*= 0*/ )
    {
        if (   0 == szRoomId || 0 == szService || 0 == ptInviter 
            || ( (LIBJINGLE_ERR_REACH_MAX_TEMP_CHATROOM_LIMIT != nReason) && (LIBJINGLE_ERR_REACH_MAX_PERSISTENT_CHATROOM_LIMIT != nReason) ) )
        {
            LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }



        // ����stanza
        std::string str;
        char buf[MAX_BUFF_SIZE];

        str = ptInviter->sNode;
        str += "@";
        str += ptInviter->sDomain;

        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
        message->SetAttr( buzz::QN_TO,   AnsiToUtf8( str.c_str(), buf, MAX_BUFF_SIZE ) );
        message->SetAttr( buzz::QN_TYPE, "inner" );

        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );
        operation->SetAttr( buzz::QN_TYPE, "notify inviter decline" );

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ITEM );
        operation->AddElement( item );

        item->SetAttr( buzz::QN_ID,                AnsiToUtf8( szRoomId,  buf, MAX_BUFF_SIZE )  );
        item->SetAttr( buzz::QN_CHATROOM_SERVICE,  AnsiToUtf8( szService, buf, MAX_BUFF_SIZE )  );

        if ( LIBJINGLE_ERR_REACH_MAX_TEMP_CHATROOM_LIMIT == nReason )
        {
            str = DECLINE_TEMPORARY_ROOM;
        }
        else
        {
            str = DECLINE_PERSISTENT_ROOM;
        }
        item->SetAttr( buzz::QN_REASON, str.c_str() );

        if ( szRoomName )
        {
            item->SetAttr( buzz::QN_NAME,  AnsiToUtf8( szRoomName, buf, MAX_BUFF_SIZE )  );
        }


        pAppObj->m_pXmppThread->SendStanza( message );

        return LIBJINGLE_ERR_OK;
    }


    // ������������Ϣ
    LIBJINGLE_API ELibjingleError ScreenChatroomsMsg( HANDLE hApp, const TChatRoomId * atIds, unsigned long dwCnt )
    {
        if (   0 == hApp || 0 == atIds || 0 == dwCnt )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        unsigned long i;
        for ( i = 0; i < dwCnt; i++ )
        {
            if ( '\0' == atIds[i].szRoomId[0] || '\0' == atIds[i].szService[0] )
            {
                return LIBJINGLE_ERR_INVALID_ARGUMENT;
            }
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }


        
        vChatRoomIds & v = pAppObj->m_vScreenedRooms;
        vChatRoomIds::iterator it;

        for ( i = 0; i < dwCnt; i++ )
        {
            it = find_if( v.begin(), v.end(), FindRoomIdObj(atIds[i].szRoomId, atIds[i].szService) );
            // �����������
            if ( it == v.end() )
            {
                LibJingleEx::PTChatRoomId  ptRoomId = new LibJingleEx::TChatRoomId;
                strcpy( ptRoomId->szRoomId, atIds[i].szRoomId );
                strcpy( ptRoomId->szService, atIds[i].szService );
                v.push_back( ptRoomId );
            }
        }

        pAppObj->ScreenChatroomMsgs();

        return LIBJINGLE_ERR_OK;
    }

    // ȡ��������������Ϣ
    LIBJINGLE_API ELibjingleError UnScreenChatroomsMsg( HANDLE hApp, const TChatRoomId * atIds, unsigned long dwCnt )
    {
        if (   0 == hApp || 0 == atIds || 0 == dwCnt )
        {
            LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        unsigned long i;
        for ( i = 0; i < dwCnt; i++ )
        {
            if ( '\0' == atIds[i].szRoomId[0] || '\0' == atIds[i].szService[0] )
            {
                LIBJINGLE_ERR_INVALID_ARGUMENT;
            }
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }



        vChatRoomIds & v = pAppObj->m_vScreenedRooms;
        vChatRoomIds::iterator it;

        for ( i = 0; i < dwCnt; i++ )
        {
            it = find_if( v.begin(), v.end(), FindRoomIdObj(atIds[i].szRoomId, atIds[i].szService) );
            // ������ɾ��
            if ( it != v.end() )
            {
                delete *it;
                v.erase( it );
            }
        }

        pAppObj->ScreenChatroomMsgs();

        return LIBJINGLE_ERR_OK;
    }


    LIBJINGLE_API ELibjingleError IsScreenedChatroom( HANDLE hApp, const TChatRoomId * pIds, int * pbIsScreened )
    {
        if (   0 == hApp || 0 == pIds || '\0' == pIds->szRoomId[0] || '\0' == pIds->szService[0] || 0 == pbIsScreened )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }



        vChatRoomIds & v = pAppObj->m_vScreenedRooms;
        vChatRoomIds::iterator it = find_if( v.begin(), v.end(), FindRoomIdObj( pIds->szRoomId, pIds->szService ) );
        if ( it == v.end() )
        {
            *pbIsScreened = FALSE;
        }
        else
        {
            *pbIsScreened = TRUE;
        }

        return LIBJINGLE_ERR_OK;
    }


    // ��ȡ�����ҵ����г�Ա(�����ߵ�)
    LIBJINGLE_API ELibjingleError GetChatRoomMembers( HROOM hRoom )
    {
        if ( 0 == hRoom )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        //// ���仹û��׼����
        //if ( ROOM_STATE_AVAILABLE != pRoom->m_eState )
        //{
        //    return LIBJINGLE_ERR_ROOM_NOT_READY;
        //}

        pAppObj->m_pXmppThread->GetChatRoomMembers( (PTRoom)hRoom );

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�GetRoomUniqueId
    // ���ܣ�    ��ȡconference�����ṩ��Ψһ�����id
    // ����ֵ��  0����Ч�ķ���򲻿���
    //           ��0�������id���ַ�����NULL������
    /********************************************************/
    //const char * GetRoomUniqueId( HROOM hRoom )
    //{
    //    if ( 0 == hRoom )
    //    {
    //        return 0;
    //    }
    //
    //    PTRoom pRoom = (PTRoom)hRoom;
    //    assert( pRoom->m_pParent );
    //
    //    if ( '\0' == pRoom->m_szRoomId[0] )
    //    {
    //        return 0;
    //    }
    //
    //    return pRoom->m_szRoomId;
    //}

    /********************************************************/
    // �������ƣ�GetAffiliation
    // ���ܣ�    ��ȡaffiliation���ַ�������
    /********************************************************/
    LIBJINGLE_API const char * GetAffiliation( EAffiliation e )
    {
        switch( e )
        {
        case AFFILIATION_OWNER:
            return "owner";
            break;
        case AFFILIATION_NONE:
            return "none";
            break;
        case AFFILIATION_MEMBER:
            return "member";
            break;
        case AFFILIATION_ADMIN:
            return "admin";
        default:
            return "unknown";
            break;
        }
    }

    /********************************************************/
    // �������ƣ�HANDLE
    // ���ܣ�    ��ȡroom���ڵ�Ӧ��
    /********************************************************/
    LIBJINGLE_API HANDLE GetHApp( HROOM hRoom )
    {
        if ( 0 == hRoom )
        {
            return 0;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return 0;
        }
        // END �����

        assert( pRoom->m_pParent );

        return pRoom->m_pParent;
    }

    LIBJINGLE_API ELibjingleError GetRoomId( HROOM hRoom, PTChatRoomId ptRoomId )
    {
        if ( 0 == hRoom || 0 == ptRoomId )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_NO_SUCH_CHATROOM;
        }
        // END �����

        
        strncpy( ptRoomId->szRoomId,  pRoom->m_szRoomId, sizeof(ptRoomId->szRoomId) );
        strncpy( ptRoomId->szService, pRoom->m_szGroupChatServiceName, sizeof(ptRoomId->szService) );
        

        return LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API HROOM GetRoomHandle( HANDLE hApp, const TChatRoomId * ptRoomId )
    {
        if ( 0 == hApp || 0 == ptRoomId )
        {
            return 0;
        }

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        return pGlobal->FindRoom( (CLibJinglexApp *)hApp,  ptRoomId );
    }

    /*=====================================  END Ⱥ��  ===========================================*/


    LIBJINGLE_API ELibjingleError GetSelfJid( HANDLE hApp, PTJid ptJid )
    {
        if ( 0 == hApp || 0 == ptJid )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        // �̰߳�ȫ
        cFuncLock cLock( &pAppObj->m_external_lock );

        cFuncLock cLock_( &pAppObj->m_lock );

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        StrJid2TJid( client->jid().Str().c_str(), *ptJid );

        return LIBJINGLE_ERR_OK;
    }


    /********************************************************/
    // �������ƣ�GetUserData
    // ���ܣ�    ��ȡ��ʼ��ʱ�Ĳ���pData
    /********************************************************/
    LIBJINGLE_API void * GetUserData( HANDLE hApp )
    {
        if ( 0 == hApp )
        {
            return 0;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return 0;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        return pAppObj->m_pUserData;
    }

    /********************************************************/
    // �������ƣ�GetRoster
    // ���ܣ�    ��ȡ�����б�
    /********************************************************/
    LIBJINGLE_API PTGroup GetRoster( HANDLE hApp )
    {
        if ( 0 == hApp )
        {
            return 0;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return 0;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        return pAppObj->m_pFirstGroup;
    }

    /********************************************************/
    // �������ƣ�GetFriendPresence
    // ���ܣ�    ��ȡ����״̬
    /********************************************************/
    LIBJINGLE_API EPresence GetFriendPresence( HANDLE hApp, const PTJid ptJid )
    {
        EPresence ePres = PRESENCE_NONE;

        if ( 0 == hApp || 0 == ptJid )
        {
            return ePres;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return ePres;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock( &pAppObj->m_external_lock );

        cFuncLock cLock_( &pAppObj->m_lock );

        vMemberGroups & v = pAppObj->m_MemberGroups;

        LibJingleEx::TJid tJid;
        memcpy( &tJid, ptJid, sizeof(tJid) );
        tJid.sResource[0] = '\0';

        vMemberGroups::iterator it_find = find_if( v.begin(), v.end(), FindMemberGroupObj(&tJid) );
        if ( it_find == v.end() )
        {
            return ePres;
        }

        PTMemberGroup p = *it_find;
        vMemberStatus & s = p->m_Status;

        // ��ȡȨ��
        unsigned long dwMaxWeight = GetPresenceWeight( ePres );

        vMemberStatus::iterator it;
        for ( it = s.begin(); it != s.end(); it++ )
        {
            PTMemberStatus pStatus = *it;
            unsigned long dwWeight = GetPresenceWeight( pStatus->m_ePresence );

            if ( dwWeight > dwMaxWeight )
            {
                dwMaxWeight = dwWeight;
            }
        }
        // ����Ȩ�����»�ȡpresence
        ePres = GetPresenceFromWeight( dwMaxWeight );

        return ePres;

    }


    // ˵����aPres����Ԫ�ظ��������aszRes��ͬ����dwResCnt
    LIBJINGLE_API ELibjingleError  GetFriendPresence( HANDLE hApp, char aszRes[][MAX_RESOURCE_LEN], 
        unsigned long dwResCnt, const PTJid ptJid, 
        EPresence aPres[], EPresence * pHighestPres )
    {
        if ( 0 == hApp || 0 == ptJid || 0 == aszRes || 0 == dwResCnt || 0 == aPres || 0 == pHighestPres )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock( &pAppObj->m_external_lock );
        cFuncLock cLock_( &pAppObj->m_lock );


        // ��ȡBareJid
        LibJingleEx::TJid tJid;
        memcpy( &tJid, ptJid, sizeof(tJid) );
        tJid.sResource[0] = '\0';


        vMemberGroups & v = pAppObj->m_MemberGroups;
        vMemberGroups::iterator it_find = find_if( v.begin(), v.end(), FindMemberGroupObj(&tJid) );
        if ( it_find == v.end() )
        {
            return LibJingleEx::LIBJINGLE_ERR_NO_SUCH_FRIEND;
        }

        PTMemberGroup p = *it_find;
        vMemberStatus & s = p->m_Status;
        vMemberStatus::iterator it;

        *pHighestPres = PRESENCE_NONE;

        unsigned long i;
        for ( i = 0; i < dwResCnt; i++ )
        {
            aPres[i] = PRESENCE_NONE;
        }

        unsigned long dwMaxWeight = GetPresenceWeight( *pHighestPres );

        for ( it = s.begin(); it != s.end(); it++ )
        {
            PTMemberStatus pStatus = *it;
            unsigned long dwWeight = GetPresenceWeight( pStatus->m_ePresence );

            // �ҵ����״̬
            if ( dwWeight > dwMaxWeight )
            {
                dwMaxWeight = dwWeight;
            }

            for ( i = 0; i < dwResCnt; i++ )
            {
                // ����ҵ���Ӧ����Դ
                if ( 0 == strcmp(aszRes[i], pStatus->m_szResource) )
                {
                    aPres[i] = pStatus->m_ePresence;
                }
            }
        }
        *pHighestPres = GetPresenceFromWeight( dwMaxWeight );

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }



    LIBJINGLE_API BOOL IfHasFriend( HANDLE hApp, const PTJid ptJid )
    {
        if ( 0 == hApp || 0 == ptJid )
        {
            return FALSE;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock( &pAppObj->m_external_lock );

        cFuncLock cLock_( &pAppObj->m_lock );

        LibJingleEx::TJid tJid;
        memcpy( &tJid, ptJid, sizeof(tJid) );
        tJid.sResource[0] = '\0';

        vMemberGroups & v = pAppObj->m_MemberGroups;

        vMemberGroups::iterator it_find = find_if( v.begin(), v.end(), FindMemberGroupObj(&tJid) );
        if ( it_find == v.end() )
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    /********************************************************/
    // �������ƣ�GetFriendAllInfo
    // ���ܣ�    ��ȡĳ���ѵ����в�ͬresouce���Ͷ�Ӧ������
    /********************************************************/
    LIBJINGLE_API PTFriendInfo GetFriendAllInfo( HANDLE hApp, const PTJid ptBareJid, PTFriendInfo ptInfo, 
        unsigned long * pdwSize )
    {
        if ( 0 == ptInfo || 0 == hApp || 0 == ptBareJid || 0 == pdwSize )
        {
            return 0;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return 0;
        }
        // END �����


        // �̰߳�ȫ
        cFuncLock cLock( &pAppObj->m_external_lock );

        cFuncLock cLock_( &pAppObj->m_lock );



        LibJingleEx::TJid tJid;
        memcpy( &tJid, ptBareJid, sizeof(TJid) );
        tJid.sResource[0] = '\0';

        vMemberGroups & v = pAppObj->m_MemberGroups;
        vMemberGroups::iterator it =  find_if( v.begin(), v.end(), FindMemberGroupObj(&tJid) );

        if ( it == v.end() )
        {
            return 0;
        }

        PTMemberGroup p = *it;

        vMemberStatus::iterator it_status;
        vMemberStatus & s = p->m_Status;

        DWORD dwSize = *pdwSize;
        DWORD i;

        for ( it_status = s.begin(), i = 0; it_status != s.end() && i < dwSize; it_status++, i++ )
        {
            PTMemberStatus q = *it_status;

            strcpy( ptInfo[i].sResource, q->m_szResource );
            ptInfo[i].ePres = q->m_ePresence;
            ptInfo[i].bFileShareAbility = q->m_bFileShareAvailable;
        }

        *pdwSize = i;
        return ptInfo;
    }

    // ��ȥ�Լ�
    // ��������У�ȫΪ0��jid���Լ�
    LIBJINGLE_API ELibjingleError GetAllRoomMembers( HROOM hRoom, PTJid ptMembers, unsigned long * pdwSize, 
        unsigned long * pdwStartIndex /* = 0 */ )
    {
        if ( 0 == hRoom || 0 == ptMembers || 0 == pdwSize )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        PTRoom pRoom = (PTRoom)hRoom;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistRoom(pRoom) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        if ( 0 == pRoom->m_pParent )
        {
            return LIBJINGLE_ERR_INVALID_ROOM;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pRoom->m_pParent;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );


        vRoomMember & v = pRoom->m_vMembers;

        if ( 0 == pdwStartIndex )
        {
            if ( pRoom->m_vMembers.size() > *pdwSize )
            {
                return LibJingleEx::LIBJINGLE_ERR_NOT_ENOUGH_MEM;
            }

            unsigned long i;
            for ( i = 0; i < v.size() ; i++  )
            {
                if ( v[i]->m_bSelf )
                {
                    memset( &ptMembers[i], 0, sizeof(TJid) );
                }
                else
                {
                    memcpy( &ptMembers[i], &v[i]->m_tJid, sizeof(TJid) );
                }
            }
            // ������ȥ������
            *pdwSize       = i;

            return LibJingleEx::LIBJINGLE_ERR_OK;
        }
        else
        {
            unsigned long dwStartPos = *pdwStartIndex;

            unsigned long i;
            for ( i = 0; (i < *pdwSize) && (i + dwStartPos < v.size() ); i++  )
            {
                if ( v[i+dwStartPos]->m_bSelf )
                {
                    memset( &ptMembers[i], 0, sizeof(TJid) );
                }   
                else
                {
                    memcpy( &ptMembers[i], &v[i+dwStartPos]->m_tJid, sizeof(TJid) );
                }
            }
            // ���¼��� end pos
            *pdwStartIndex = i + dwStartPos;
            // ������ȥ������
            *pdwSize       = i;


            // һ�����ڵ��������
            if ( *pdwStartIndex >= v.size() )
            {
                return LibJingleEx::LIBJINGLE_ERR_OK;
            }
            else
            {
                return LibJingleEx::LIBJINGLE_ERR_PARAM_BUFF_FULL;
            }
        }
    }


    LIBJINGLE_API void PrintDebugInfo( HANDLE hApp, unsigned long dwSwitch /*= -1*/ )
    {
        if ( 0 == hApp )
        {
            return;
        }

        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return;
        }
        // END �����

        cFuncLock cLock( &pAppObj->m_lock );
        pAppObj->PrintDebugInfo( dwSwitch );
    }

    LIBJINGLE_API void PrintAllApps( )
    {
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        pGlobal->PrintAllApps();
    }

    // xmpp��Ϣ���ڲ���־�����ڵ��Է�Χ
    // ���õ����õĻص�
    LIBJINGLE_API void SetDebugInfoHandler( POnXmppMessage pOnXmppMessage, POnLogInfo pOnLogMessage )
    {
        g_pOnXmppMessage = pOnXmppMessage;
        g_pOnLogMessage  = pOnLogMessage;
    }

    LIBJINGLE_API ELibjingleError SendXmppMessage( HANDLE hApp, const char * szMessage )
    {
        if ( 0 == hApp || 0 == szMessage )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        // client->SendRaw( szMessage );
        pAppObj->m_pXmppThread->SendRaw( szMessage );

        return LIBJINGLE_ERR_OK;
    }


    ///////////////////////////////////////////////////////////////////////////////
    // FileShareManifest
    ///////////////////////////////////////////////////////////////////////////////

    LIBJINGLE_API BOOL FileShareManifest::AddFile(   const char * szName,   unsigned long dwSize)
    {
        if ( m_dwSize == MAX_MANIFEST_SIZE )
        {
            return FALSE;
        }

        assert( m_dwSize < MAX_MANIFEST_SIZE );

        Item & itm = m_atItems[m_dwSize];

        itm.m_eTyp = T_FILE;
        strncpy( itm.m_szName, szName, MAX_FILENAME_SIZE );
        itm.m_dwSize = dwSize;

        m_dwSize++;
        return TRUE;
    }

    LIBJINGLE_API BOOL FileShareManifest::AddImage(  const char * szName,   unsigned long dwSize,
        unsigned long dwWidth, unsigned long dwHeight)
    {
        if ( m_dwSize == MAX_MANIFEST_SIZE )
        {
            return FALSE;
        }

        assert( m_dwSize < MAX_MANIFEST_SIZE );

        Item & itm = m_atItems[m_dwSize];

        itm.m_eTyp = T_IMAGE;
        strncpy( itm.m_szName, szName, MAX_FILENAME_SIZE );
        itm.m_dwSize = dwSize;
        itm.m_dwWidth = dwWidth;
        itm.m_dwHeight = dwHeight;

        m_dwSize++;
        return TRUE;
    }

    LIBJINGLE_API BOOL FileShareManifest::AddFolder( const char * szName,   unsigned long dwSize)
    {
        if ( m_dwSize == MAX_MANIFEST_SIZE )
        {
            return FALSE;
        }

        assert( m_dwSize < MAX_MANIFEST_SIZE );

        Item & itm = m_atItems[m_dwSize];

        itm.m_eTyp = T_FOLDER;
        strncpy( itm.m_szName, szName, MAX_FILENAME_SIZE );
        itm.m_dwSize = dwSize;

        m_dwSize++;
        return TRUE;
    }

    LIBJINGLE_API unsigned long FileShareManifest::GetItemCount(Type t) const
    {
        DWORD i;
        DWORD cnt = 0;
        for ( i = 0; i < m_dwSize; i++ )
        {
            const Item & itm = m_atItems[i];
            if ( itm.m_eTyp == t )
            {
                cnt++;
            }
        }
        return cnt;
    }

    LIBJINGLE_API unsigned long FileShareManifest::GetTotalSize() const
    {
        DWORD i;
        DWORD dwTotalSize = 0;
        for ( i = 0; i < m_dwSize; i++ )
        {
            dwTotalSize += m_atItems[i].m_dwSize;
        }
        return dwTotalSize;
    }





}// end of libjingleEx namespace

















void OnDebugInfo( const char * szFormat, ... )
{
    if ( g_pOnLogMessage )
    {
        //ʱ���
        time_t now;
        struct tm t;

        char szTimeBuff[64];
        time(&now);
        if ( now < 0 )
        {
            now = 0;
        }

        struct tm * pTime = localtime(&now);
        if ( 0 == pTime )
        {
            memset( &t, 0, sizeof(t) );
        }
        else
        {
            t = *pTime;
        }

        sprintf( szTimeBuff, "[%04u-%02u-%02u %02u:%02u:%02u] ", 
                 t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                 t.tm_hour, t.tm_min, t.tm_sec );

        va_list ap;
        va_start( ap, szFormat );
        char buf[MAX_BUFF_SIZE] = { 0 };
        vsnprintf( buf, MAX_BUFF_SIZE - 1, szFormat, ap );
        va_end(ap);

        std::string sTmp = szTimeBuff;
        sTmp += buf;

        g_pOnLogMessage( sTmp.c_str() );
    }
}

char * Utf8ToAnsi( const char * szUtf8, char * ansi, unsigned int dwSize )
{

    CGlobalData * pGlobal = CGlobalData::GetInstance();
    assert( pGlobal->m_bInited );
    cFuncLock cLock( &pGlobal->m_arrLocks[LOCK_INDEX_ANSI] );
    int nLen = 0;

#ifdef WIN32	
    WCHAR wszStr[MAX_BUFF_SIZE] = {0};
    int nCount = MultiByteToWideChar( CP_UTF8, 0, szUtf8, strlen(szUtf8), wszStr, MAX_BUFF_SIZE - 1 );
    nLen = WideCharToMultiByte( CP_ACP, 0, wszStr, nCount, ansi, dwSize - 1, 0, 0 );
    ansi[nLen] = '\0';
    return ansi;

#elif defined (SUPPORT_ICONV)
    char * pUtf8      = (char *)szUtf8;
    size_t dwUtf8Len  = strlen(szUtf8);

    size_t dwTmpSize  = dwSize;
    char * pTmp       = ansi;

    int nRet = iconv( pGlobal->m_cd_utf8_to_ansi, (char **)&pUtf8, &dwUtf8Len, &pTmp, &dwTmpSize );
    while( -1 == nRet && dwUtf8Len > 0 )
    {
        if ( dwTmpSize > 1 )
        {
            *pTmp = '?';
            pTmp++;
            dwTmpSize--;
        }
        else
        {
            break;
        }

        if ( dwUtf8Len > 3 )
        {
            pUtf8     += 3;	
            dwUtf8Len -= 3;
        }
        else
        {
            break;
        }

        nRet = iconv( pGlobal->m_cd_utf8_to_ansi, (char **)&pUtf8, &dwUtf8Len, &pTmp, &dwTmpSize );
    }

    if ( dwTmpSize > 0 )
    {
        *pTmp = '\0';
    }
    return ansi;

#else
    // memset( ansi, 0, dwSize );
    UINT16 wStr[MAX_BUFF_SIZE];
    int nCount = UTF2Uni( szUtf8, wStr );
    nLen = StrUnicode2Gb( wStr, nCount, ansi, dwSize );
    ansi[nLen] = '\0';
    return ansi;

#endif	

}



extern int chr2utf(char *str, BYTE* bmpStr);
char * AnsiToUtf8( const char * szAnsi, char * utf8, unsigned int dwSize )
{
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    assert( pGlobal->m_bInited );
    cFuncLock cLock( &pGlobal->m_arrLocks[LOCK_INDEX_UTF] );

    int nLen = 0;
#ifdef WIN32
    WCHAR wszStr[MAX_BUFF_SIZE];
    int nCount = MultiByteToWideChar( CP_ACP, 0, szAnsi, strlen(szAnsi), wszStr, MAX_BUFF_SIZE - 1 );
    nLen = WideCharToMultiByte( CP_UTF8, 0, wszStr, nCount, utf8, dwSize - 1, 0, 0 );
    utf8[nLen] = '\0';
    return utf8;

#elif defined( SUPPORT_ICONV)
    size_t dwAnsiLen = strlen(szAnsi);
    size_t dwTmpSize = dwSize;
    char * pTmp      = utf8;

    int nRet = iconv( pGlobal->m_cd_ansi_to_utf8, (char **)&szAnsi, &dwAnsiLen, &pTmp, &dwTmpSize );
    if ( -1 != nRet )
    {
        if ( dwTmpSize > 0 )
        {
            *pTmp = '\0';
        }
    }
    return utf8;

#else

    // memset( utf8, 0, dwSize );
    nLen = chr2utf( (char *)szAnsi, (BYTE *)utf8 );
    utf8[nLen] = '\0';
    return utf8;
#endif
}




LIBJINGLE_API LibJingleEx::ELibjingleError LibJingleEx::StrJid2TJid( const char * szJid, LibJingleEx::TJid & jid  )
{
    //	memset( &jid, 0, sizeof(jid) );

    const char * pAt    = strchr( szJid, '@' );

    unsigned long dwLen = 0;

    if ( 0 == pAt )
    {
        dwLen = strlen( szJid );
        if ( dwLen >= LibJingleEx::MAX_DOMAIN_LEN )
        {
            return LibJingleEx::LIBJINGLE_ERR_NOT_ENOUGH_MEM;
        }
        strcpy( jid.sDomain, szJid );
    }
    else
    {
        unsigned long dwNodeLen = pAt - szJid;
        if ( dwNodeLen >= LibJingleEx::MAX_NODE_LEN )
        {
            return LibJingleEx::LIBJINGLE_ERR_NOT_ENOUGH_MEM;
        }

        strcpy( jid.sNode, szJid );
        jid.sNode[dwNodeLen] = '\0';

        const char * pSlash = strchr( szJid, '/' );
        if ( 0 != pSlash )
        {
            if( pAt >= pSlash )
            {
                return LIBJINGLE_ERR_INVALID_JID;
            }

            unsigned long dwDomainLen = pSlash - pAt - 1;
            if ( LibJingleEx::MAX_DOMAIN_LEN <= dwDomainLen )
            {
                return LibJingleEx::LIBJINGLE_ERR_NOT_ENOUGH_MEM;
            }

            strncpy( jid.sDomain, pAt + 1, dwDomainLen );
            jid.sDomain[dwDomainLen] = '\0';

            unsigned long dwResLen = strlen(szJid) - ( pSlash - szJid ) - 1;
            if ( LibJingleEx::MAX_RESOURCE_LEN <= dwResLen )
            {
                return LibJingleEx::LIBJINGLE_ERR_NOT_ENOUGH_MEM;
            }

            strcpy( jid.sResource, pSlash + 1 );
        }
        else
        {
            unsigned long dwDomainLen = strlen(szJid) - ( pAt - szJid ) - 1;
            if ( LibJingleEx::MAX_DOMAIN_LEN <= dwDomainLen )
            {
                return LibJingleEx::LIBJINGLE_ERR_NOT_ENOUGH_MEM;
            }

            strcpy( jid.sDomain, pAt + 1 );
        }
    }

    return LibJingleEx::LIBJINGLE_ERR_OK;
}

LIBJINGLE_API LibJingleEx::ELibjingleError LibJingleEx::TJid2StrJid( const LibJingleEx::TJid & tJid, char * szJid, unsigned int dwSize )
{
#ifdef WIN32
    if ( '\0' == tJid.sResource[0] )
    {
        if ( '\0' == tJid.sNode[0] )
        {
            _snprintf( szJid, dwSize - 1, "%s", tJid.sDomain );   
        }
        else
        {
            _snprintf( szJid, dwSize - 1, "%s@%s", tJid.sNode, tJid.sDomain );
        }
    }
    else
    {
        _snprintf( szJid, dwSize - 1, "%s@%s/%s", tJid.sNode, tJid.sDomain, tJid.sResource );
    }
#else
    if ( '\0' == tJid.sResource[0] )
    {
        if ( '\0' == tJid.sNode[0] )
        {
            snprintf( szJid, dwSize - 1, "%s", tJid.sDomain );
        }
        else
        {
            snprintf( szJid, dwSize - 1, "%s@%s", tJid.sNode, tJid.sDomain );
        }
    }
    else
    {
        snprintf( szJid, dwSize - 1, "%s@%s/%s", tJid.sNode, tJid.sDomain, tJid.sResource );
    }
#endif
    return LibJingleEx::LIBJINGLE_ERR_OK;
}

char * GetJid( const LibJingleEx::TJid & tJid )
{
    static char szJid[256];

    CGlobalData * pGlobal = CGlobalData::GetInstance();
    assert( pGlobal->m_bInited );
    cFuncLock cLock( &pGlobal->m_arrLocks[LOCK_INDEX_JID] );

    LibJingleEx::TJid2StrJid( tJid, szJid, 256 );
    return szJid;
}

char * GetBareJid( char * szBareJid, unsigned long dwSize, const char * szFullJid )
{
    assert( szBareJid && szFullJid && dwSize > 0 );

    LibJingleEx::TJid tJid;
    LibJingleEx::StrJid2TJid( szFullJid, tJid  );

    tJid.sResource[0] = '\0';

    LibJingleEx::TJid2StrJid( tJid, szBareJid, dwSize );

    return szBareJid;
}

char * GenerateRandomId( char * buff, unsigned int dwSize, const char * prefix )
{
    assert( dwSize >= MAX_RAND_ID_LENGTH );

    // const char * prefix = "green";
    int prefix_len = 0;
    if ( prefix )
    {
        prefix_len = strlen( prefix );
        strcpy( buff, prefix );
    }


    // ������ֵĸ���
    const int RAND_DIGITAL_CNT = 8;

    int i;
    for ( i = 0; i < RAND_DIGITAL_CNT; i++ )
    {
        int r = rand() % 36;
        char c = 0;

        if ( r <= 9 )
        {
            c = '0' + r;	
        }
        else
        {
            c = ( r - 10 ) + 'a';
        }

        buff[prefix_len+i] = c;
    }

    buff[prefix_len+i] = '\0';
    return buff;
}


void JInitLock  ( JLockType * pLock )
{
    assert( pLock );
#ifdef WIN32
    InitializeCriticalSection( pLock );
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );

    pthread_mutex_init( pLock, &attr );

    pthread_mutexattr_destroy( &attr );
#endif
}

void JDeinitLock( JLockType * pLock )
{
    assert( pLock );
#ifdef WIN32
    DeleteCriticalSection( pLock );
#else
    pthread_mutex_destroy( pLock );
#endif
}

void JLock( JLockType * pLock )
{
    assert( pLock );
#ifdef WIN32
    EnterCriticalSection( pLock );
#else
    pthread_mutex_lock( pLock );
#endif
}

void JUnlock( JLockType * pLock )
{
    assert( pLock );
#ifdef WIN32
    LeaveCriticalSection( pLock );
#else
    pthread_mutex_unlock( pLock );
#endif
}


BOOL IfJidEqual( const LibJingleEx::TJid & t1, const LibJingleEx::TJid & t2 )
{
    if (     0 == strcmp( t1.sDomain, t2.sDomain ) && 0 == strcmp( t1.sNode, t2.sNode )
        && 0 == strcmp( t1.sResource, t2.sResource )
        )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL IfBareJidEqual( const LibJingleEx::TJid & t1, const LibJingleEx::TJid & t2 )
{
    if ( 0 == strcmp( t1.sDomain, t2.sDomain ) && 0 == strcmp( t1.sNode, t2.sNode ) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void ClearFirstGroup(LibJingleEx::PTGroup & pFirstGroup)
{
    LibJingleEx::PTGroup pCurGroup  = pFirstGroup;

    while( pCurGroup )
    {
        LibJingleEx::PTGroup pNextGroup = pCurGroup->pNext;

        LibJingleEx::PTMember pCurMember = pCurGroup->pChild;
        while( pCurMember )
        {
            LibJingleEx::PTMember pNextMember = pCurMember->pNext;

            delete pCurMember;
            pCurMember = pNextMember;
        }

        delete pCurGroup;
        pCurGroup = pNextGroup;
    }

    pFirstGroup = 0;
}

void ClearMemberGroups( vMemberGroups & v )
{
    vMemberGroups::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        PTMemberGroup p = *it;

        // û�и����������ڴ�
        p->m_Groups.clear();

        delete p;
    }

    v.clear();
}


FindMemberGroupObj::FindMemberGroupObj( const LibJingleEx::PTJid ptJid )
{
    memcpy( &m_tJid, ptJid, sizeof(LibJingleEx::TJid) );
}

bool FindMemberGroupObj::operator()( const PTMemberGroup p )
{
    // return IfJidEqual( m_tJid, p->m_tJid ) ? true : false;
    return IfBareJidEqual( m_tJid, p->m_tJid ) ? true : false;
}

FindStringObj::FindStringObj( const char * s )
{
    m_str = s;
}

bool FindStringObj::operator()( const char * s )
{
    if ( 0 == m_str )
    {
        return false;
    }

    return ( 0 == strcmp(m_str, s) ) ? true : false;
}


BOOL IfMemberEqual( const LibJingleEx::PTMember p1, const LibJingleEx::PTMember p2 )
{
    if ( p1->nSubscription != p2->nSubscription )
    {
        return FALSE;
    }

    if ( p1->nAsk != p2->nAsk )
    {
        return FALSE;
    }

    //if ( p1->pData != p2->pData )
    //{
    //    return FALSE;
    //}

    if ( 0 != strcmp( p1->szName, p2->szName ) )
    {
        return FALSE;
    }

    return TRUE;
}

void ClearRooms( vRoom & v )
{
    vRoom::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

FindRoomMemberObj::FindRoomMemberObj( const LibJingleEx::PTJid ptJid ) : m_ptJid(ptJid)
{

}

bool FindRoomMemberObj::operator ()(PTRoomMember pMember)
{
    if ( IfJidEqual( *m_ptJid, pMember->m_tJid ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


FindRoomMemberByBareJidObj::FindRoomMemberByBareJidObj( const LibJingleEx::PTJid ptBareJid )
{
    memcpy( &m_tBareJid, ptBareJid, sizeof(LibJingleEx::TJid) );
    m_tBareJid.sResource[0] = '\0';
}

bool FindRoomMemberByBareJidObj::operator ()(PTRoomMember pMember)
{
    if ( IfBareJidEqual( m_tBareJid, pMember->m_tJid ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


FindRoomMemberByNickObj::FindRoomMemberByNickObj( const char * szNick ) : m_szNick(szNick)
{

}

bool FindRoomMemberByNickObj::operator ()(PTRoomMember pMember)
{
    if ( 0 == strcmp( pMember->m_szNick, m_szNick ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


FindRoomByQueryId::FindRoomByQueryId( const char * szQueryId ) : m_szQueryId( szQueryId )
{

}

bool FindRoomByQueryId::operator()(PTRoom pRoom)
{
    if ( 0 == strcmp( pRoom->m_szQueryId, m_szQueryId ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

FindRoomByRoomId::FindRoomByRoomId( const char * szRoomId ) : m_szRoomId ( szRoomId )
{

}


bool FindRoomByRoomId::operator()(PTRoom pRoom)
{
    if ( 0 == strcmp( pRoom->m_szRoomId, m_szRoomId ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

FindRoomByFullId::FindRoomByFullId( const char * szRoomId, const char * szServiceName )
: m_szRoomId(szRoomId), m_szServiceName(szServiceName)
{

}

bool FindRoomByFullId::operator ()(PTRoom pRoom)
{
    if (    0 == strcmp(pRoom->m_szRoomId,m_szRoomId) 
        && 0 == strcmp(m_szServiceName, pRoom->m_szGroupChatServiceName ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}



/***********************     CLibJinglexApp     ***********************/

CLibJinglexApp::CLibJinglexApp()
{
    // ����������߳̽���XMPP����
    m_pXmppThread = new XmppThread(this);

    // ����Ϊ���ڵ�Ľṹ
    m_pFirstGroup = 0;

    JInitLock( &m_lock );

    JInitLock( &m_external_lock );

    m_pUserData = 0;

    m_szStunAddr[0]  = '\0';
    m_wStunPort      = 0;
    memset(  m_reserverd0, 0, sizeof( m_reserverd0) );
    m_szRelayAddr[0] = '\0';

    memset( m_szSaveFileDir, 0, sizeof(m_szSaveFileDir) );

    m_bFileShareAvailable = TRUE;

    m_dwDmsMaxGroupId   = 0;
    m_dwDmsMaxMemberId  = 0;

    m_bGetDmsGroupResult  = FALSE;
    m_bGetDmsMemberResult = FALSE;

    memset( m_szEnterpriseName, 0, sizeof(m_szEnterpriseName) );

    m_bNotifySecurity = FALSE;

    m_bPrivatePersistentRoomsCntConfirmed = FALSE;

    m_bAudio = FALSE;
    m_bVideo = FALSE;

    m_ePres = LibJingleEx::PRESENCE_NONE;
    m_bCall = FALSE;

    memset( m_szGroupChatPicSaveDir, 0, sizeof(m_szGroupChatPicSaveDir) );

    m_bRoomReadMsgsChanged = FALSE;
    m_bGetRoomReadMsgs     = FALSE;
    m_chatroom_history     = 0;
    m_dwChatroomHistoryVersion = 0;

    m_dwPingTimeoutCnt = 0;
    m_bPersistentRoomsChanged = FALSE;
    m_bTryEnterAllPersistentRooms = FALSE;

    m_bUpperAppReady = FALSE;

    m_bConfigAvailable   = FALSE;

    m_bPersistentRoomsResponsed = FALSE;


    m_bGetServerTime = FALSE;
    m_dwTimeDiff     = 0;
    m_dwQueryTick    = 0;
    memset( m_szQueryTimeId, 0, sizeof(m_szQueryTimeId) );


    m_bSendStatus = FALSE;

    memset( m_szConfigPath,    0,  sizeof(m_szConfigPath) );
    m_roster_root = 0;
    m_bRosterChanged = FALSE;

    m_dwMaxGroupId   = 0;
    m_dwMaxMemberId  = 0;
    m_bGetRosterData = FALSE;
    memset( m_szRosterVer, 0, sizeof(m_szRosterVer) );

    m_dwLoginTick = 0;

    memset( m_szLastSendPictureId, 0, sizeof(m_szLastSendPictureId) );
    m_dwLastSendPictureTick = 0;
}

CLibJinglexApp::~CLibJinglexApp()
{
    if ( 0 != m_pXmppThread )
    {
        // �ȴ����߳̽���
        m_pXmppThread->Disconnect();
        m_pXmppThread->Join();

        delete m_pXmppThread;
        m_pXmppThread = 0;
    }

    Clear();

    JDeinitLock( &m_lock );

    JDeinitLock( &m_external_lock );
}


// ����jidɾ���������µ�����ͬjid�ĺ���
void CLibJinglexApp::RemoveFriend( const LibJingleEx::PTJid ptJid )
{
    LibJingleEx::PTGroup pGroup = m_pFirstGroup;
    LibJingleEx::PTGroup pLastGroup = 0;
    while( pGroup )
    {
        LibJingleEx::PTMember pMember = pGroup->pChild;
        LibJingleEx::PTMember pLastMember = 0;
        while( pMember )
        {
            if ( IfJidEqual( *ptJid, pMember->tJid ) )
            {
                if ( 0 == pLastMember )
                {
                    pGroup->pChild = pMember->pNext;
                }
                else
                {
                    pLastMember->pNext = pMember->pNext;
                }

                if ( g_tEvents.pOnRosterDel )
                {
                    g_tEvents.pOnRosterDel( this, pGroup->szName, &pMember->tJid );
                }
                else if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData msgData;
                    msgData.Append( (unsigned char *)pGroup->szName, sizeof(pGroup->szName) );
                    msgData.Append( (unsigned char *)&pMember->tJid, sizeof(pMember->tJid) );

                    g_pfnOnMessage( LibJingleEx::ON_ROSTER_DEL, this, this->m_pUserData, 
                        msgData.GetMsgData(), msgData.GetMsgLength() );
                }

#if !DMS_CONTACTS_SWITCH
                // �������û�ж����������ϵ��
                if ( !IfUnscribedContact(pMember) )
                {
                    // ֪ͨ�ϲ�Ӧ��
                    if ( g_pfnOnMessage )
                    {
                        g_pfnOnMessage(  LibJingleEx::ON_NOTIFY_DEL_DMS_MEMBER, this, m_pUserData, 
                                        (unsigned char *)&pMember->dwId,  sizeof(pMember->dwId) );
                    }
                }
#endif

                delete pMember;
                break;
            }
            pLastMember = pMember;
            pMember = pMember->pNext;
        }

#if !DMS_CONTACTS_SWITCH
        if ( pGroup->bToBeDeleted && 0 == pGroup->pChild )
        {
            // ֪ͨ��ɾ��
            if ( g_pfnOnMessage )
            {
                g_pfnOnMessage( LibJingleEx::ON_NOTIFY_DEL_DMS_GROUP, this, m_pUserData, 
                                (unsigned char *)&pGroup->dwGroupId,  sizeof(pGroup->dwGroupId) );
            }
            // END(֪ͨ��ɾ��)
            
            if ( pLastGroup )
            {
                pLastGroup->pNext = pGroup->pNext;
            }
            else
            {
                m_pFirstGroup = pGroup->pNext;
            }

            LibJingleEx::PTGroup pTmpGroup = pGroup;
            pGroup = pGroup->pNext;

            delete pTmpGroup;
            continue;
        }
#endif              

        pLastGroup = pGroup;
        pGroup = pGroup->pNext;
    }

    // ɾ��MemberGroup item���ڵ�����
    vMemberGroups::iterator it_find = find_if( m_MemberGroups.begin(),
        m_MemberGroups.end(), FindMemberGroupObj( ptJid ) ) ;

    if ( it_find != m_MemberGroups.end() )
    {
        PTMemberGroup p = *it_find;
        p->m_Groups.clear();
        memset( &p->m_tData, 0, sizeof(p->m_tData) );
    }

}

const char * GetRoomState( RoomState e )
{
    switch( e )
    {
    case ROOM_STATE_UNVAILABLE:
        return "unavailable";

    case ROOM_STATE_ON_QUERY_ID:
        return "on query id";

    case ROOM_STATE_ON_PRESENCE:
        return "on presence";

    case ROOM_STATE_ON_CONFIG:
        return "on config";

    case ROOM_STATE_AVAILABLE:
        return "available";

    case ROOM_STATE_ON_INVITING:
        return "on inviting";

    case ROOM_STATE_ON_ACCEPTING:
        return "on accept inviting";

    case ROOM_STATE_NOTIFIED_CREATE:
        return "notified created";

    case ROOM_STATE_ENTERING_PERSISTENT:
        return "entering persistent room";

    case ROOM_STATE_HOLD_PERSISTENT:
        return "holding persistent room";

    default:
        return "unknown";
    }
}

void CLibJinglexApp::PrintDebugInfo( unsigned long dwSwitchs /*= -1*/ )
{
    std::string   sDebug;
    char buf[MAX_BUFF_SIZE];

    sDebug += "============= DEBUG INFO =================== \r\n";

    SNPRINTF( buf, MAX_BUFF_SIZE, "READY FLAG = %s \r\n\r\n", GetBoolString(m_bUpperAppReady)  );
    sDebug += buf;

    if ( IfHasBit( dwSwitchs, LibJingleEx::DEBUG_INFO_INDEX_GROUP ) )
    {
        LibJingleEx::PTGroup pGroup = m_pFirstGroup;
        int nGroupNo = 0;
        while( pGroup )
        {
            SNPRINTF( buf, MAX_BUFF_SIZE, "== %dth GROUP: %s, %lu \r\n", nGroupNo + 1, pGroup->szName, pGroup->dwGroupId );
            sDebug += buf;

            int nMemberNo = 0;
            LibJingleEx::PTMember pMember = pGroup->pChild;
            while( pMember )
            {
                BOOL  bIgnored = IfUnscribedContact( pMember );
                const char * szTsName = FindTrueSocialName( &pMember->tJid );

                if ( bIgnored )
                {
                    if ( 0 == szTsName )
                    {
                        SNPRINTF( buf, MAX_BUFF_SIZE, 
                            "        %dth member : \r\n"
                            "            Name: %s  \r\n"
                            "            JID: %s   \r\n"
                            "            Subscription: %s \r\n"
                            "            Ask: %d  (IGNORED) \r\n", 
                            nMemberNo+1, pMember->szName, GetJid( pMember->tJid ), 
                            LibJingleEx::GetSubscription(pMember->nSubscription), pMember->nAsk );
                    }
                    else
                    {
                        SNPRINTF( buf, MAX_BUFF_SIZE, 
                            "        %dth member : \r\n"
                            "            Name: %s  \r\n"
                            "            JID: %s(%s)   \r\n"
                            "            Subscription: %s \r\n"
                            "            Ask: %d  (IGNORED) \r\n", 
                            nMemberNo+1, pMember->szName,  GetJid( pMember->tJid ), szTsName,
                            LibJingleEx::GetSubscription(pMember->nSubscription), pMember->nAsk );
                    }
                }
                else
                {
                    if ( 0 == szTsName )
                    {
                        SNPRINTF( buf, MAX_BUFF_SIZE, 
                            "        %dth member : \r\n"
                            "            Name: %s  \r\n"
                            "            JID: %s   \r\n"
                            "            Subscription: %s \r\n"
                            "            Ask: %d  \r\n", 
                            nMemberNo+1, pMember->szName, GetJid( pMember->tJid ), 
                            LibJingleEx::GetSubscription(pMember->nSubscription), pMember->nAsk );
                    }
                    else
                    {
                        SNPRINTF( buf, MAX_BUFF_SIZE, 
                            "        %dth member : \r\n"
                            "            Name: %s  \r\n"
                            "            JID: %s(%s)   \r\n"
                            "            Subscription: %s \r\n"
                            "            Ask: %d  \r\n", 
                            nMemberNo+1, pMember->szName, GetJid( pMember->tJid ), szTsName,
                            LibJingleEx::GetSubscription(pMember->nSubscription), pMember->nAsk );
                    }
                }


                sDebug += buf;
                sDebug += "\r\n";

                pMember = pMember->pNext;
                nMemberNo++;
            }
            pGroup = pGroup->pNext;
            nGroupNo++;
        }

        sDebug += "\r\n";
    }


    if ( IfHasBit( dwSwitchs, LibJingleEx::DEBUG_INFO_INDEX_GROUP0 ) )
    {
        LibJingleEx::PTGroup pGroup = m_pFirstGroup;
        int nGroupNo = 0;
        while( pGroup )
        {
            SNPRINTF( buf, MAX_BUFF_SIZE, "== %dth GROUP: %s, %lu \r\n", nGroupNo + 1, pGroup->szName, pGroup->dwGroupId );
            sDebug += buf;

            int nMemberNo = 0;
            LibJingleEx::PTMember pMember = pGroup->pChild;
            while( pMember )
            {
                BOOL  bIgnored = IfUnscribedContact( pMember );
                const char * szTsName = FindTrueSocialName( &pMember->tJid );

                // ������ԣ�����ʾ
                if ( bIgnored )
                {
                    pMember = pMember->pNext;
                    continue;
                }


                if ( 0 == szTsName )
                {
                    SNPRINTF( buf, MAX_BUFF_SIZE, 
                        "        %dth member : \r\n"
                        "            Name: %s  \r\n"
                        "            JID: %s   \r\n"
                        "            Subscription: %s \r\n"
                        "            Ask: %d  \r\n", 
                        nMemberNo+1, pMember->szName, GetJid( pMember->tJid ), 
                        LibJingleEx::GetSubscription(pMember->nSubscription), pMember->nAsk );
                }
                else
                {
                    SNPRINTF( buf, MAX_BUFF_SIZE, 
                        "        %dth member : \r\n"
                        "            Name: %s  \r\n"
                        "            JID: %s(%s)   \r\n"
                        "            Subscription: %s \r\n"
                        "            Ask: %d  \r\n", 
                        nMemberNo+1, pMember->szName, GetJid( pMember->tJid ), szTsName,
                        LibJingleEx::GetSubscription(pMember->nSubscription), pMember->nAsk );
                }


                sDebug += buf;
                sDebug += "\r\n";

                pMember = pMember->pNext;
                nMemberNo++;
            }
            pGroup = pGroup->pNext;
            nGroupNo++;
        }

        sDebug += "\r\n";
    }

    if ( IfHasBit( dwSwitchs, LibJingleEx::DEBUG_INFO_INDEX_MEMBER_GROUP ) )
    {
        vMemberGroups::iterator it;
        int nNo = 0;
        for ( it = m_MemberGroups.begin(), nNo = 0; it != m_MemberGroups.end(); it++, nNo++ )
        {
            PTMemberGroup p = *it;

            if ( '\0' == p->m_szTrueSocialName[0] )
            {
                SNPRINTF( buf, MAX_BUFF_SIZE, "== %dth MemberGroup: %s, from char state = %s, to char state = %s \r\n", nNo + 1, GetJid(p->m_tJid), GetCharStateString(p->m_eFromCharStatus), GetCharStateString(p->m_eToCharStatus) );
            }
            else
            {
                SNPRINTF( buf, MAX_BUFF_SIZE, "== %dth MemberGroup: %s(%s), from char state = %s, to char state = %s \r\n", nNo + 1, GetJid(p->m_tJid), p->m_szTrueSocialName,  GetCharStateString(p->m_eFromCharStatus), GetCharStateString(p->m_eToCharStatus) );
            }
            sDebug += buf;

            int m;
            vMemberStatus::iterator it_status;
            vMemberStatus & s = p->m_Status;

            for ( it_status = s.begin(), m = 0; it_status != s.end(); it_status++, m++ )
            {
                PTMemberStatus pStatus = *it_status;
                char szVersion[128] = { 0 };
                MyGetVersion( szVersion, 128, pStatus->m_dwVersion );

                SNPRINTF( buf, MAX_BUFF_SIZE, "        %dth STATUS, resource = %s, status = %s, file ability = %s, audio ability = %s, video ability = %s, call ability = %s, version = %s \r\n ", m+1, 
                    pStatus->m_szResource, GetPresence(pStatus->m_ePresence), GetBoolString(pStatus->m_bFileShareAvailable), GetBoolString(pStatus->m_bAudio), GetBoolString(pStatus->m_bVideo), GetBoolString(pStatus->m_bCall), szVersion );
                sDebug += buf;
            }

            sDebug += "\r\n";
        }

        sDebug += "\r\n";
    }


    if ( IfHasBit( dwSwitchs, LibJingleEx::DEBUG_INFO_INDEX_CHATROOM ) )
    {
        vRoom::iterator it_room;
        int nNo = 0;
        int nPersistentNo = 0;
        int nTemporaryNo  = 0;

        for ( it_room = m_Rooms.begin(), nNo = 0; it_room != m_Rooms.end(); it_room++, nNo++ )
        {
            PTRoom pRoom = *it_room;

            SNPRINTF( buf, MAX_BUFF_SIZE, "== %dth chat room: \r\n"
                "        room id = %s@%s \r\n"
                "        room name = %s \r\n"
                "        state = %s  \r\n"
                "        affiliation = %s \r\n", nNo+1, 
                pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, pRoom->m_szName, GetRoomState(pRoom->m_eState), 
                GetAffiliation(pRoom->m_eAffiliation) );
            sDebug += buf;

            if ( ROOM_TYPE_SPECIAL == pRoom->m_nType )
            {
                SNPRINTF( buf, MAX_BUFF_SIZE, "        status push room \r\n" );
                sDebug += buf;
            }
            else if ( ROOM_TYPE_VISUAL == pRoom->m_nType )
            {
                SNPRINTF( buf, MAX_BUFF_SIZE, "        h323 meeting room \r\n" );
                sDebug += buf;
            }
            else if ( ROOM_TYPE_NORMAL == pRoom->m_nType )
            {
                if ( pRoom->m_bPersistent )
                {
                    SNPRINTF( buf, MAX_BUFF_SIZE, "        persistent room(%dth) \r\n", nPersistentNo + 1 );
                    nPersistentNo++;
                }
                else
                {
                    SNPRINTF( buf, MAX_BUFF_SIZE, "        temporary room(%dth) \r\n", nTemporaryNo + 1 );
                    nTemporaryNo++;
                }
                sDebug += buf;
            }

#if  IM_DEBUG
            SNPRINTF( buf, MAX_BUFF_SIZE, "        push delay msg count = %u \r\n", pRoom->m_dwDelayMsgCnt );
            sDebug += buf;
#endif

            vRoomMember::iterator it_m;
            vRoomRoster::iterator it_r;

            vRoomRoster vTmpRoster;
            std::copy( pRoom->m_vRoster.begin(), pRoom->m_vRoster.end(), back_inserter(vTmpRoster) );


            // ��jid�ĺϲ�Ϊһ����
            std::vector<LibJingleEx::PTJid> vDistinctMembers;
            std::vector<LibJingleEx::PTJid>::iterator it_sub;


            for ( it_m = pRoom->m_vMembers.begin(); it_m != pRoom->m_vMembers.end(); it_m++ )
            {
                PTRoomMember p = *it_m;
                
                for( it_sub = vDistinctMembers.begin();  it_sub  != vDistinctMembers.end(); it_sub++ )
                {
                    LibJingleEx::PTJid ptMemberJid = *it_sub;

                    // �ҵ�ͬ����jid��
                    if ( IfBareJidEqual(*ptMemberJid, p->m_tJid) )
                    {
                        break;
                    }
                }

                // ��������������jid
                if ( it_sub == vDistinctMembers.end() )
                {
                    LibJingleEx::PTJid  ptMemberJid = new LibJingleEx::TJid;
                    memcpy( ptMemberJid, &p->m_tJid, sizeof(LibJingleEx::TJid) );
                    ptMemberJid->sResource[0] = '\0';

                    vDistinctMembers.push_back( ptMemberJid );
                }
            }

            char blank_buf[MAX_BUFF_SIZE];
            int i = 0;

            for( it_sub = vDistinctMembers.begin(), i = 0;  it_sub  != vDistinctMembers.end(); it_sub++, i++ )
            {
                LibJingleEx::PTJid ptMemberJid = *it_sub;

                const char * szTsName = FindTrueSocialName( ptMemberJid );

                if ( 0 == szTsName )
                {
                    SNPRINTF( buf, MAX_BUFF_SIZE, "            %dth Member: %s:  ", i+1, GetJid(*ptMemberJid) );
                }
                else
                {
                    SNPRINTF( buf, MAX_BUFF_SIZE, "            %dth Member: %s(%s):  ", i+1, GetJid(*ptMemberJid), szTsName );
                }
                sDebug += buf;

                unsigned long dwLength = strlen( buf );

                memset( blank_buf, ' ', dwLength );
                blank_buf[dwLength] = '\0';

                
                int j = 0;
                for ( it_m = pRoom->m_vMembers.begin(); it_m != pRoom->m_vMembers.end(); it_m++ )
                {
                    PTRoomMember p = *it_m;

                    if ( IfBareJidEqual( *ptMemberJid, p->m_tJid) )
                    {
                        if ( j > 0 )
                        {
                            sDebug += blank_buf;
                        }

                        SNPRINTF( buf, MAX_BUFF_SIZE, "%s, %s, %s \r\n", p->m_tJid.sResource, GetAffiliation(p->m_eAlliliation), GetPresence(p->m_ePres)  );
                        sDebug += buf;

                        j++;
                    }
                }

                it_r = find_if( vTmpRoster.begin(), vTmpRoster.end(), FindRoomRosterItem( ptMemberJid ) );
                if ( vTmpRoster.end() != it_r )
                {
                    vTmpRoster.erase( it_r );
                }

                delete ptMemberJid;
            }

            vDistinctMembers.clear();
            

            // ��Ա�б�(����ʾ�����ߵ�)
            for ( it_r = vTmpRoster.begin(); it_r != vTmpRoster.end(); it_r++, i++ )
            {
                LibJingleEx::PTRoomRosterItem p = *it_r;

                // �鿴���Ƿ������˳�
                bool bQueryQuit = false;
                std::vector<LibJingleEx::PTJid>::iterator  ix;

                for ( ix = pRoom->m_vQueryQuitMembers.begin(); ix != pRoom->m_vQueryQuitMembers.end(); ix++ )
                {
                    LibJingleEx::PTJid ptJid = *ix;
                    if ( IfBareJidEqual( *ptJid, p->m_tJid ) )
                    {
                        bQueryQuit = true;
                    }
                }
                // END(�鿴���Ƿ������˳�)

                const char * szTsName = FindTrueSocialName( &p->m_tJid );

                if ( !bQueryQuit )
                {
                    if ( 0 == szTsName)
                    {
                        SNPRINTF( buf, MAX_BUFF_SIZE, "            %dth Member: %s, %s (offline) \r\n", i+1, GetJid(p->m_tJid), GetAffiliation(p->m_eAlliliation)  );
                    }
                    else
                    {
                        SNPRINTF( buf, MAX_BUFF_SIZE, "            %dth Member: %s(%s), %s (offline) \r\n", i+1, GetJid(p->m_tJid), szTsName, GetAffiliation(p->m_eAlliliation)  );
                    }
                }
                else
                {
                    if ( 0 == szTsName )
                    {
                        SNPRINTF( buf, MAX_BUFF_SIZE, "            %dth Member: %s, %s (offline, query to quit) \r\n", i+1, GetJid(p->m_tJid), GetAffiliation(p->m_eAlliliation)  );
                    }
                    else
                    {
                        SNPRINTF( buf, MAX_BUFF_SIZE, "            %dth Member: %s(%s), %s (offline, query to quit) \r\n", i+1, GetJid(p->m_tJid), szTsName, GetAffiliation(p->m_eAlliliation)  );
                    }
                }
                sDebug += buf;

            }
            // END(��Ա�б�(����ʾ�����ߵ�))
            sDebug += "\r\n";
        }
        sDebug += "\r\n";
    }


    if ( IfHasBit( dwSwitchs, LibJingleEx::DEBUG_INFO_INDEX_DMS ) )
    {
        vDmsGroup & v = m_vDmsGroups;
        vDmsGroup::iterator it_g;
        int i = 0;

        for ( it_g = v.begin(), i = 0; it_g != v.end(); it_g++, i++ )
        {
            LibJingleEx::PTDmsGroup pGroup = *it_g;
            SNPRINTF( buf, MAX_BUFF_SIZE, "%dth DMS group[%s, sn: %u, parent id: %u ]: \r\n", i + 1, pGroup->szGroupName, pGroup->dwGroupId, pGroup->dwParentId );
            sDebug += buf;

            int k;
            for ( k = 0; k < DMS_MEMBERS_BLOCK_CNT; k++ )
            {
                vDmsMember & m = m_vDmsMembers[k];
                vDmsMember::iterator it_m;
                int j = 0;

                for ( it_m = m.begin(), j = 0; it_m != m.end(); it_m++, j++ )
                {
                    LibJingleEx::PTDmsMember pMember = *it_m;

                    if ( pMember->dwGroupId == pGroup->dwGroupId )
                    {
                        LibJingleEx::TJid  tJid;
                        LibJingleEx::StrJid2TJid( pMember->szJid, tJid );

                        const char * szTsName = FindTrueSocialName( &tJid );

                        if ( 0 == szTsName )
                        {
                            SNPRINTF( buf, MAX_BUFF_SIZE, "    %dth DMS member[%s, sn: %u, name: %s] \r\n", j + 1, pMember->szJid, pMember->dwMemberId, pMember->szName );
                        }
                        else
                        {
                            SNPRINTF( buf, MAX_BUFF_SIZE, "    %dth DMS member[%s(%s), sn: %u, name: %s] \r\n", j + 1, pMember->szJid, szTsName, pMember->dwMemberId, pMember->szName );
                        }
                        sDebug += buf;
                    }
                }
            }

            sDebug += " \r\n";
        }

        sDebug += "\r\n";
    }


    if ( IfHasBit( dwSwitchs, LibJingleEx::DEBUG_INFO_INDEX_CONFIG ) )
    {
        const cKeyValue * p = m_cConfig.GetFirstItem();
        while( p )
        {
            SNPRINTF( buf, MAX_BUFF_SIZE, " %s:[%s] \r\n",  p->sKey.c_str(), p->sValue.c_str() );
            sDebug += buf;

            p = m_cConfig.GetNext();
        }

        sDebug += "\r\n";
    }


    sDebug += "============= END OF DEBUG INFO ============= \r\n";



    if ( g_pOnLogMessage )
    {
        g_pOnLogMessage( sDebug.c_str() );
    }

}


void CLibJinglexApp::SavePrivateData( char * szLastQueryId, unsigned long dwSize )
{
    char buf[MAX_BUFF_SIZE];

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

    // iq����type����
    iq->SetAttr(buzz::QN_TYPE,buzz::STR_SET);

    // iq����id����
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->SetAttr(buzz::QN_ID, randid);

    // ���query��Ԫ��
    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
    iq->AddElement( query );

    // ���history��Ԫ��
    buzz::XmlElement * history = new buzz::XmlElement(buzz::QN_CHATROOM_HISTORY, true);
    query->AddElement( history );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%lu", m_dwChatroomHistoryVersion );
    history->SetAttr( buzz::QN_VERSION, buf );


    std::vector< CRoomReadMsgs *>::iterator it;
    for ( it = m_vRoomReadMsgs.begin(); it != m_vRoomReadMsgs.end(); it++ )
    {
        CRoomReadMsgs * p = *it;

        if ( m_bPersistentRoomsResponsed )
        {
            // �鿴�Ƿ��ж�Ӧ�̶�Ⱥ
            vPersistentRoom::iterator it_p = find_if( m_vPersistentRooms.begin(), m_vPersistentRooms.end(),
                FindPersistentRoom( p->m_szRoomId, p->m_szGroupChatServiceName ) );
            // ��������ڳ־��Է���
            if ( it_p == m_vPersistentRooms.end() )
            {
                continue;
            }
            // END
        }


        buzz::XmlElement * groupchat = new buzz::XmlElement( buzz::QN_GROUPCHAT, true );
        history->AddElement( groupchat );

        groupchat->SetAttr( buzz::QN_ID, p->m_szRoomId );
        groupchat->SetAttr( buzz::QN_CHATROOM_SERVICE, p->m_szGroupChatServiceName );

        unsigned long i;
        unsigned long cnt = p->m_room.GetCount();

        for ( i = 0; i < cnt; i++ )
        {
            char * pMsgId = p->m_room[i];
            assert( pMsgId );

            buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ITEM, true );
            groupchat->AddElement( item );

            item->SetBodyText( pMsgId );
        }
    }

    buzz::XmppClient * client = m_pXmppThread->client();
    if ( client )
    {
        client->SendStanza( iq );
    }
    delete iq;

    // ��¼���һ�ε�iq id
    strncpy( szLastQueryId, randid, dwSize );
}



void CLibJinglexApp::Clear()
{
    // ���FirstGroup
    ClearFirstGroup( m_pFirstGroup );
    // ���membergroups
    ClearMemberGroups( m_MemberGroups );
    // ���������
    ClearRooms( m_Rooms );

    std::vector< LibJingleEx::PTDmsGroup >::iterator it_g;
    for ( it_g = m_vDmsGroups.begin(); it_g != m_vDmsGroups.end(); it_g++ )
    {
        LibJingleEx::PTDmsGroup p = *it_g;
        delete p;
    }
    m_vDmsGroups.clear();


    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        std::vector< LibJingleEx::PTDmsMember >::iterator it_m;
        for ( it_m = m_vDmsMembers[i].begin(); it_m != m_vDmsMembers[i].end(); it_m++ )
        {
            LibJingleEx::PTDmsMember p = *it_m;
            delete p;
        }
        m_vDmsMembers[i].clear();
    }


    vPersistentRoom::iterator it_p;
    for ( it_p = m_vPersistentRooms.begin() ; it_p != m_vPersistentRooms.end(); it_p++ )
    {
        PTPersistentRoom p = *it_p;
        delete p;
    }
    m_vPersistentRooms.clear();

    std::vector< CRoomReadMsgs *>::iterator it_h;
    for ( it_h = m_vRoomReadMsgs.begin(); it_h != m_vRoomReadMsgs.end(); it_h++ )
    {
        CRoomReadMsgs * p = *it_h;
        delete p;
    }
    m_vRoomReadMsgs.clear();

    ClearVector( m_vDeclineItems );

    ClearVector( m_vQueryPersistentRoomIds );

    ClearVector( m_vQueryAllPersistentRoomIds );

    ClearVector( m_vTryingPersistents );

    ClearVector( m_vScreenedRooms );

    ClearVector( m_vQueryStatusContact );

    ClearVector( m_vUnscribedContacts );

    if ( m_roster_root )
    {
        delete m_roster_root;
        m_roster_root = 0;
    }

    if ( m_chatroom_history )
    {
        delete m_chatroom_history;
        m_chatroom_history = 0;
    }

    ClearVector( m_vSendRoomPictureParams );
}

BOOL CLibJinglexApp::IfExistFileShareSession( FileShareClient * pSession )
{
    if ( 0 == m_pXmppThread )
    {
        return FALSE;
    }

    XmppPump * pump = m_pXmppThread->pump();
    if ( 0 == pump )
    {
        return FALSE;
    }

    return pump->IfExistFileShareClient( pSession );
}

// �����е�private block����һ�����ʵ�block���µ�dms ��Ա
// �����ĸ�private block���˸ó�Ա
unsigned long  CLibJinglexApp::AddDmsMember2Block(  LibJingleEx::PTDmsMember ptNewMember, unsigned long & dwBits  )
{
    CGlobalData * pGlobal = CGlobalData::GetInstance();

    // ��Щ��Ҫ�޸�(�е�����Ҫ����)
    dwBits = 0;

    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT - 1; i++ )
    {
        vDmsMember & v = m_vDmsMembers[i];
        if ( v.size() < pGlobal->m_dwMaxDmsMembersPerBlock )
        {
            break;
        }
    }

    // �ȱ�������index
    unsigned long dwAddIndex = i;  


    // �޸�ͬjid��Ϊһ���ı���
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & v = m_vDmsMembers[i];

        vDmsMember::iterator it;
        for ( it = v.begin(); it != v.end(); it++ )
        {
            LibJingleEx::PTDmsMember  p = *it;

            // ����ҵ�jidһ���ģ�Ҫ�޸ı���
            if ( 0 == strcmp(p->szJid, ptNewMember->szJid) )
            {
                strcpy( p->szName, ptNewMember->szName );
                SetBit( dwBits, i );
            }
        }
    }

    SetBit( dwBits, dwAddIndex );

    // û��ѡ�������һ��block
    m_vDmsMembers[dwAddIndex].push_back( ptNewMember );
    return dwAddIndex;
}

// �����е�private blocks���ҵ�ĳ��id�ĳ�Ա
// ����ֵΪ0��ʾû���ҵ�
LibJingleEx::PTDmsMember  CLibJinglexApp::GetDmsMember( unsigned long dwMemberId, unsigned long * pdwIndex /*= 0*/ )
{
    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & vMembers = m_vDmsMembers[i];
        vDmsMember::iterator it_member;

        it_member = find_if( vMembers.begin(), vMembers.end(), FindDmsMemberObj( dwMemberId ) );
        if ( vMembers.end() != it_member )
        {
            if ( pdwIndex )
            {
                *pdwIndex = i;
            }
            return *it_member;
        }
    }
    return 0;
}

// ɾ��dms ��Ա
// ����ֵ��ʾ�ĸ�˽�п�ɾ���˸ó�Ա��-1��ʾû�������Ա
unsigned long CLibJinglexApp::DeleteDmsMember( unsigned long dwMemberId )
{
    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & vMembers = m_vDmsMembers[i];
        vDmsMember::iterator it_member;

        it_member = find_if( vMembers.begin(), vMembers.end(), FindDmsMemberObj( dwMemberId ) );
        // ����ҵ�����ɾ����
        if ( vMembers.end() != it_member )
        {
            delete *it_member;
            vMembers.erase( it_member );
            return i;
        }
    }
    // û���ҵ�
    return (unsigned long)-1;
}

void  CLibJinglexApp::GetDmsMemberIdsByJid( const LibJingleEx::TJid * ptJid, std::vector<unsigned long> & vIds )
{
    char szJid[256] = { 0 };
    LibJingleEx::TJid2StrJid( *ptJid, szJid, sizeof(szJid) );
    vIds.clear();

    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & vMembers = m_vDmsMembers[i];
        vDmsMember::iterator it_member;

        for ( it_member = vMembers.begin(); it_member != vMembers.end(); it_member++ )
        {
            LibJingleEx::PTDmsMember  pMember = *it_member;

            if ( 0 == strcmp( pMember->szJid, szJid ) )
            {
                vIds.push_back( pMember->dwMemberId );
                break;
            }
        }
    }
}

// �޸�����jid��ptMember�е�jid��ͬ�ĳ�Ա( ��id��jid���ԣ���������Ϊһ�� )
unsigned long CLibJinglexApp::ModifyDmsMemberByJid( LibJingleEx::PTDmsMember ptMember )
{
    unsigned long i;
    unsigned long dwBlockBits = 0;

    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & vMembers = m_vDmsMembers[i];
        vDmsMember::iterator it_member;

        for ( it_member = vMembers.begin();  it_member != vMembers.end(); it_member++  )
        {
            LibJingleEx::PTDmsMember  pItem = *it_member;

            // ���jid���
            if ( 0 == strcmp( pItem->szJid, ptMember->szJid ) )
            {
                SetBit( dwBlockBits, i );

                strcpy( pItem->szName, ptMember->szName );

                int j;
                for ( j = 0; j < LibJingleEx::MAX_DMS_MEMBER_RESERVERD_BUF_CNT; j++ )
                {
                    strcpy( pItem->szReserved[j], ptMember->szReserved[j] );
                }
            }
        }
    }

    return dwBlockBits;
}

// �Ƿ���ĳ��ָ�������ڴ���ָ��jid�ĳ�Ա
BOOL CLibJinglexApp::IfExistDmsMemberInTheGroup( unsigned long dwGroupId, const char * szJid )
{
    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & vMembers = m_vDmsMembers[i];
        vDmsMember::iterator it_member;

        for ( it_member = vMembers.begin();  it_member != vMembers.end(); it_member++  )
        {
            LibJingleEx::PTDmsMember  pItem = *it_member;

            // ����ҵ������ĳ�Ա
            if ( pItem->dwGroupId == dwGroupId &&  0 == strcmp(pItem->szJid, szJid) )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

// �Ƿ���ָ��jid�ĳ�Ա
BOOL CLibJinglexApp::IfExistDmsMember(const char * szJid)
{
    unsigned long i;
    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & vMembers = m_vDmsMembers[i];
        vDmsMember::iterator it_member;

        for ( it_member = vMembers.begin();  it_member != vMembers.end(); it_member++  )
        {
            LibJingleEx::PTDmsMember  pItem = *it_member;

            // ����ҵ������ĳ�Ա
            if ( 0 == strcmp(pItem->szJid, szJid) )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

// ���������� ROOM_TYPE_NORMAL, ROOM_TYPE_VISUAL
// �� dwRoomType = ROOM_TYPE_NORMALʱ�� bPersistent����Ч
unsigned long  CLibJinglexApp::GetChatRoomsCnt( unsigned long dwRoomType, BOOL bPersistent )
{
    unsigned long dwCnt = 0;
    vRoom::iterator it;

    for ( it = m_Rooms.begin(); it != m_Rooms.end(); it++ )
    {
        PTRoom  p = *it;

        // ���ڱ�����ķ������(��Ϊ���ڱ�����ķ���Ҳ���ܵ�vector)
        if ( p->m_nType == dwRoomType && ROOM_STATE_ON_INVITING != p->m_eState )
        {
            if ( ROOM_TYPE_NORMAL == dwRoomType )
            {
                if ( p->m_bPersistent == bPersistent )
                {
                    dwCnt++;
                }
            }
            else
            {
                dwCnt++;
            }
        }
    }

    return dwCnt;
}


// aszRes��ʾ����Щresource��Ӧ��״̬
// ����ֵTRUE��ʾ�ҵ���ptDmsMemberState->szJid��������ϵ��
// ˵����ptDmsMemberState->szJid����ص�resource�������������resource��Ӧ��״̬���������
BOOL  CLibJinglexApp::GetDmsMemberState( const char aszRes[][LibJingleEx::MAX_RESOURCE_LEN], unsigned long dwResCnt,
                                        LibJingleEx::PTDmsMemberState  ptDmsMemberState )
{
    // ״̬����ʼ��
    unsigned long i;
    ptDmsMemberState->eMaxPres  = LibJingleEx::PRESENCE_NONE;
    ptDmsMemberState->bMaxAudio = FALSE;
    ptDmsMemberState->bMaxVideo = FALSE;

    for ( i = 0; i < LibJingleEx::MAX_DMS_MEMBER_STATE_CNT; i++ )
    {
        ptDmsMemberState->ePres[i]  = LibJingleEx::PRESENCE_NONE;
        ptDmsMemberState->bAudio[i] = FALSE;
        ptDmsMemberState->bVideo[i] = FALSE;
        ptDmsMemberState->bCall[i]  = FALSE;
    }
    // END(״̬����ʼ��)


    LibJingleEx::TJid tBareJid;
    LibJingleEx::StrJid2TJid( ptDmsMemberState->szJid,  tBareJid );
    tBareJid.sResource[0] = '\0';

    vMemberGroups & v = m_MemberGroups;
    vMemberGroups::iterator it_find = find_if( v.begin(), v.end(), FindMemberGroupObj(&tBareJid) );
    if ( it_find == v.end() )
    {
        return FALSE;
    }

    PTMemberGroup p   = *it_find;
    vMemberStatus & s = p->m_Status;
    vMemberStatus::iterator it;

    // ����Ȩ��
    unsigned long dwMaxWeight = GetPresenceWeight( ptDmsMemberState->eMaxPres );

    for ( it = s.begin(); it != s.end(); it++ )
    {
        PTMemberStatus pStatus = *it;
        unsigned long dwWeight = GetPresenceWeight( pStatus->m_ePresence );

        // �����״̬
        if ( dwWeight > dwMaxWeight )
        {
            dwMaxWeight = dwWeight;
        }

        if ( pStatus->m_bCall )
        {
            if ( pStatus->m_bAudio )
            {
                ptDmsMemberState->bMaxAudio = TRUE;
            }

            if ( pStatus->m_bVideo )
            {
                ptDmsMemberState->bMaxVideo = TRUE;
            }
        }


        for ( i = 0; i < dwResCnt && i < LibJingleEx::MAX_DMS_MEMBER_STATE_CNT; i++ )
        {
            // ����ҵ�resource��ͬ��
            if ( 0 == strcmp( aszRes[i], pStatus->m_szResource ) )
            {
                ptDmsMemberState->ePres[i] = pStatus->m_ePresence;

                ptDmsMemberState->bAudio[i] = pStatus->m_bAudio;
                ptDmsMemberState->bVideo[i] = pStatus->m_bVideo;
                ptDmsMemberState->bCall[i]  = pStatus->m_bCall;
            }
        }
    }
    ptDmsMemberState->eMaxPres = GetPresenceFromWeight( dwMaxWeight );

    return TRUE;
}


// �������������������Ϣ
void  CLibJinglexApp::HandleDelayInvite2ChatRoomMsgs()
{
    // ���û�еõ��̶������ȷ�ϣ�����
    // ����ϲ�Ӧ��û��׼���ã�����
    if ( !m_bPrivatePersistentRoomsCntConfirmed || !m_bUpperAppReady )
    {
        return;
    }

    std::vector<PTInvit2ChatRoomMsg>::iterator it_delay;
    for ( it_delay = m_vInviteMsgs.begin(); it_delay != m_vInviteMsgs.end(); it_delay++ )
    {
        PTInvit2ChatRoomMsg  pDelayInviteMsg= *it_delay;

        PTRoom pRoom = 0;
        vRoom::iterator it_room = find_if( m_Rooms.begin(), m_Rooms.end(), 
            FindRoomByFullId( pDelayInviteMsg->m_szRoomId, pDelayInviteMsg->m_szGroupChatServiceName ) );

        // ��û���������
        if ( it_room == m_Rooms.end() )
        {
            pRoom = new TRoom(this);
            strncpy( pRoom->m_szRoomId,               pDelayInviteMsg->m_szRoomId,               MAX_UNIQUE_ROOM_ID_LENGTH );
            strncpy( pRoom->m_szGroupChatServiceName, pDelayInviteMsg->m_szGroupChatServiceName, MAX_GROUPCHAT_SERVICE_NAME_LEN );

            pRoom->m_eState = ROOM_STATE_ON_INVITING;

            pRoom->m_bPersistent = pDelayInviteMsg->m_bPersistent;
            pRoom->m_nType       = pDelayInviteMsg->m_nType;
            strcpy( pRoom->m_szName, pDelayInviteMsg->m_szName );

            m_Rooms.push_back( pRoom );
        }
        // ����������䣬��������һ�ε�����
        else
        {
            pRoom = *it_room;
        }

        // ����Ǳ�����̬����һ��֪ͨ(Ҳ���ǵ�һ��)
        if ( ROOM_STATE_ON_INVITING == pRoom->m_eState  )
        {
            if ( g_tEvents.pOnInvited )
            {
                g_tEvents.pOnInvited( this, pRoom, &pDelayInviteMsg->m_tInviterJid, pRoom->m_bPersistent, pRoom->m_szName );
            }
            else if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData   msgData;
                msgData.Append( (unsigned char *)&pRoom,                sizeof(pRoom) );
                msgData.Append( (unsigned char *)&pDelayInviteMsg->m_tInviterJid,          sizeof(pDelayInviteMsg->m_tInviterJid) );
                msgData.Append( (unsigned char *)&pRoom->m_bPersistent, sizeof(pRoom->m_bPersistent) );
                msgData.Append( (unsigned char *)&pRoom->m_nType,       sizeof(pRoom->m_nType) );
                msgData.Append( (unsigned char *)pRoom->m_szName,       sizeof(pRoom->m_szName) );
                msgData.Append( (unsigned char *)pRoom->m_szRoomId,     sizeof(pRoom->m_szRoomId) );
                msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, 
                    sizeof(pRoom->m_szGroupChatServiceName) );

                g_pfnOnMessage( LibJingleEx::ON_INVITED_TO_CHATROOM, this, m_pUserData, 
                    msgData.GetMsgData(),  msgData.GetMsgLength() );
            }
        }


        delete pDelayInviteMsg;
    }

    m_vInviteMsgs.clear();
}


void  CLibJinglexApp::NotifyHoldPersistentRoom()
{
    LibJingleEx::CMsgData    msgData;
    LibJingleEx::CMsgData    roomsData;

    vRoom::iterator  it;
    unsigned long    dwCnt      = 0;
    unsigned long   max_count =    ( LibJingleEx::MAX_MESSAGE_DATA_LENGTH - sizeof(unsigned long) ) 
                                 / ( sizeof(LibJingleEx::TJid) + sizeof(LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN) );

    for ( it = m_Rooms.begin();  it != m_Rooms.end();  it++ )
    {
        PTRoom  pRoom  = * it;

        if ( pRoom->m_eState == ROOM_STATE_HOLD_PERSISTENT )
        {
            LibJingleEx::TJid   tRoomJid;
            strncpy( tRoomJid.sNode,   pRoom->m_szRoomId,                sizeof(tRoomJid.sNode) );
            strncpy( tRoomJid.sDomain, pRoom->m_szGroupChatServiceName,  sizeof(tRoomJid.sDomain) );

            roomsData.Append( (unsigned char *)&tRoomJid,        sizeof(tRoomJid) );
            roomsData.Append( (unsigned char *)pRoom->m_szName,  LibJingleEx::MAX_GROUPCHAT_ROOM_NAEM_LEN );
            dwCnt++;

            // ��������㹻������
            if ( dwCnt >= max_count )
            {
                msgData.Append( (unsigned char *)&dwCnt,   sizeof(dwCnt) );
                msgData.Append(  roomsData.GetMsgData(),   roomsData.GetMsgLength() );

                if ( g_pfnOnMessage )
                {
                    g_pfnOnMessage( LibJingleEx::ON_PERSISTENT_CHATROOMS_NOTIFY, this, m_pUserData, msgData.GetMsgData(),  msgData.GetMsgLength() );
                }

                dwCnt = 0;
                roomsData.Clear();
                msgData.Clear();
            }
        }
    }

    if ( dwCnt > 0 )
    {
        msgData.Append( (unsigned char *)&dwCnt,   sizeof(dwCnt) );
        msgData.Append(  roomsData.GetMsgData(),   roomsData.GetMsgLength() );

        if ( g_pfnOnMessage )
        {
            g_pfnOnMessage( LibJingleEx::ON_PERSISTENT_CHATROOMS_NOTIFY, this, m_pUserData, msgData.GetMsgData(),  msgData.GetMsgLength() );
        }

        dwCnt = 0;
        roomsData.Clear();
        msgData.Clear();
    }

    // ֪ͨ���
    if ( g_pfnOnMessage )
    {
        g_pfnOnMessage( LibJingleEx::ON_PERSISTENT_CHATROOMS_NOTIFY_FIN, this, m_pUserData,  0, 0 );
    }

    
}

void  CLibJinglexApp::EnterEnterpriseStatusRoom()
{
    // ����ϲ�Ӧ��û��׼����
    if ( !m_bUpperAppReady )
    {
        return;
    }

    // ���û����ҵ��
    if ( '\0' == m_szEnterpriseName[0] )
    {
        return;
    }

    buzz::XmppClient * client = m_pXmppThread->pump()->client();
    if ( 0 == client )
    {
        return;
    }

    // ��û�л�ȡmuc jid
    if ( 0 == client->ConferenceJid().length() )
    {
        return;
    }

    char buf[MAX_BUFF_SIZE];

    buzz::XmlElement * presence = new buzz::XmlElement( buzz::QN_PRESENCE );

    std::string sTmp;
    sTmp  = m_szEnterpriseName;
    sTmp += '@';
    sTmp += client->ConferenceJid();
    sTmp += '/';
    sTmp += client->jid().Str();

    presence->SetAttr( buzz::QN_TO, AnsiToUtf8(sTmp.c_str(), buf, MAX_BUFF_SIZE) );

    const char * szPresence = GetPresShowString( m_ePres );
    if ( szPresence )
    {
        buzz::XmlElement * show   = new buzz::XmlElement(buzz::QN_SHOW );
        presence->AddElement( show );
        show->SetBodyText( szPresence );
    }

    // ���priority
    buzz::XmlElement * priority = new buzz::XmlElement( buzz::QN_PRIORITY );
    presence->AddElement( priority );
    priority->SetBodyText("1");

    std::string ability;
    if ( GetAbilityString( ability, this ) )
    {
        presence->AddElement( new buzz::XmlElement( buzz::QN_CAPS_C, true) );
        presence->AddAttr( buzz::QN_NODE, "http://www.google.com/xmpp/client/caps", 1);
        MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
        presence->AddAttr( buzz::QN_VER,  buf, 1);
        presence->AddAttr( buzz::QN_EXT, ability, 1);
    }

    buzz::XmlElement * x = new buzz::XmlElement( buzz::QN_MUC_X, true );
    presence->AddElement( x );

    // ����ÿ�����ȡ��0��stanza
    buzz::XmlElement * history = new buzz::XmlElement( buzz::QN_HISTORY, true );
    x->AddElement( history );
    history->SetAttr( buzz::QN_MAX_STANZAS, "0" );
    // END ����ÿ�����ȡ��0��stanza

    client->SendStanza( presence );
    delete presence;


    PTRoom pRoom = new TRoom(this);
    strcpy( pRoom->m_szRoomId, m_szEnterpriseName );
    strcpy( pRoom->m_szGroupChatServiceName, client->ConferenceJid().c_str() );
    pRoom->m_bPersistent = FALSE;
    pRoom->m_nType       = ROOM_TYPE_SPECIAL;
    pRoom->m_eState      = ROOM_STATE_ON_PRESENCE;
    m_Rooms.push_back( pRoom );
}

void  CLibJinglexApp::DenyEnterpriseStatusIn()
{
    buzz::XmppClient * client = m_pXmppThread->pump()->client();
    if ( 0 == client )
    {
        return;
    }

    char buf[MAX_BUFF_SIZE];

    if ( '\0' != m_szEnterpriseName[0] )
    {
        const char * szFormat = 
            "<iq type='set' id='%s'>"
            "<query xmlns='jabber:iq:privacy'>"
            "<list name='enterprise_status_room'>"
            "<item type='jid' value='%s' action='deny' order='1'>"
            "<presence-in/>"
            "</item>"
            "</list>"
            "</query>"
            "</iq>";

        std::string  sStatusRoomJid = m_szEnterpriseName;
        sStatusRoomJid += "@";
        sStatusRoomJid += client->ConferenceJid();

        char szStatusRoomJid[256];
        AnsiToUtf8( sStatusRoomJid.c_str(), szStatusRoomJid, 256 );

        char szId[MAX_RAND_ID_LENGTH];
        SNPRINTF( buf, MAX_BUFF_SIZE, szFormat, GenerateRandomId(szId, MAX_RAND_ID_LENGTH), szStatusRoomJid );

        client->SendRaw( buf );


        szFormat = 
            "<iq type='set' id='%s'>"
            "<query xmlns='jabber:iq:privacy'>"
            "<active name='enterprise_status_room'/>"
            "</query>"
            "</iq>";

        SNPRINTF( buf, MAX_BUFF_SIZE, szFormat, GenerateRandomId(szId, MAX_RAND_ID_LENGTH) );

        client->SendRaw( buf );
    }
}

void CLibJinglexApp::ScreenChatroomMsgs()
{
    buzz::XmppClient * client = m_pXmppThread->pump()->client();
    if ( 0 == client )
    {
        return;
    }

    char buf[MAX_BUFF_SIZE];
    char  szId[MAX_RAND_ID_LENGTH];

    buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
    iq->SetAttr( buzz::QN_TYPE, "set" );
    iq->SetAttr( buzz::QN_ID, GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVACY_QUERY, true);
    iq->AddElement( query );

    buzz::XmlElement * lst     = new buzz::XmlElement( buzz::QN_PRIVACY_LIST );
    query->AddElement( lst );
    lst->SetAttr( buzz::QN_NAME, client->jid().resource() );

    vChatRoomIds::iterator it;
    unsigned long i;

    for ( it = m_vScreenedRooms.begin(), i = 0; it != m_vScreenedRooms.end(); it++, i++ )
    {
        LibJingleEx::PTChatRoomId  p = *it;

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_PRIVACY_ITEM );
        lst->AddElement( item );
        item->SetAttr( buzz::QN_TYPE, "jid" );

        std::string sTmp = p->szRoomId;
        sTmp += "@";
        sTmp += p->szService;

        item->SetAttr( buzz::QN_VALUE, sTmp );
        item->SetAttr( buzz::QN_ACTION, "deny" );

        SNPRINTF( buf, MAX_BUFF_SIZE, "%u", i+1 );
        item->SetAttr( buzz::QN_ORDER, buf );

        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_PRIVACY_MESSAGE );
        item->AddElement( message );
    }

    m_pXmppThread->SendStanza( iq );



    // activate
    iq = new buzz::XmlElement( buzz::QN_IQ );
    iq->SetAttr( buzz::QN_TYPE, "set" );
    iq->SetAttr( buzz::QN_ID, GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

    query = new buzz::XmlElement(buzz::QN_PRIVACY_QUERY, true);
    iq->AddElement( query );

    buzz::XmlElement * active     = new buzz::XmlElement( buzz::QN_PRIVACY_ACTIVE );
    query->AddElement( active );
    active->SetAttr( buzz::QN_NAME, client->jid().resource() );

    m_pXmppThread->SendStanza( iq );
}


void  ParseScreenedChatrooms( std::vector<std::string *> & v,  const char * szConfig )
{
    const char * p = szConfig;

    BOOL bStart = 0;
    std::string * q = 0;

    while( '\0' != *p )
    {
        // ����ǿո�
        if ( ' ' == *p )
        {
            // һ������
            if ( bStart )
            {
                bStart = FALSE;
                assert( q );

                std::vector<std::string *>::iterator it;
                for ( it = v.begin(); it != v.end(); it++ )
                {
                    std::string * pItem = *it;

                    // ����ַ����ظ�
                    if ( *pItem == *q )
                    {
                        break;
                    }
                }

                // û���ظ����ַ���
                if ( it == v.end() )
                {
                    v.push_back( q );
                }
                else
                {
                    delete q;
                }

                q = 0;
            }
        }
        // ����ǿո�
        else
        {
            if ( !bStart )
            {
                bStart = TRUE;
                q = new std::string;
            }

            assert( q );
            *q += *p;
        }

        p++;
    }

    // һ������
    if ( bStart )
    {
        bStart = FALSE;
        assert( q );

        std::vector<std::string *>::iterator it;
        for ( it = v.begin(); it != v.end(); it++ )
        {
            std::string * pItem = *it;

            // ����ַ����ظ�
            if ( *pItem == *q )
            {
                break;
            }
        }

        // û���ظ����ַ���
        if ( it == v.end() )
        {
            v.push_back( q );
        }
        else
        {
            delete q;
        }

        q = 0;
    }
}

void  AssembleScreenedChatroomsString( std::string & str, std::vector<std::string *> & v )
{
    str = "";

    std::vector<std::string *>::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        std::string * pStr = *it;
        if ( str.length() > 0  )
        {
            str += ' ';
        }
        str += *pStr;
    }
}


void  CLibJinglexApp::HandleScreenedChatroomCfg()
{
    if ( !m_bPrivatePersistentRoomsCntConfirmed || !m_bConfigAvailable  )
    {
        return;
    }

    
    const char * pScreenedChatrooms = m_cConfig[LibJingleEx::SCREENED_CHATROOMS_KEY];

    std::vector<std::string *> v;
    ParseScreenedChatrooms( v, pScreenedChatrooms );


    BOOL  bChanged = FALSE;
    std::vector<std::string *>::iterator it;
    for ( it = v.begin(); it != v.end(); )
    {
        std::string * pStr = *it;

        LibJingleEx::TJid   tRoomId;
        LibJingleEx::StrJid2TJid( pStr->c_str(), tRoomId );


        vPersistentRoom & w = m_vPersistentRooms;
        vPersistentRoom::iterator it_p;

        // ������ڳ־��Է�����ģ�ɾ��
        for ( it_p = w.begin(); it_p != w.end(); it_p++ )
        {
            PTPersistentRoom pPersistent = *it_p;
            
            if (    0 == strcmp( pPersistent->m_szRoomId, tRoomId.sNode ) 
                 && 0 == strcmp( pPersistent->m_szGroupChatServiceName, tRoomId.sDomain ) )
            {
                break;
            }
        }

        // û�ҵ���������������id��Ч
        if ( it_p == w.end() )
        {
            delete pStr;
            it = v.erase( it );
            bChanged = TRUE;
        }
        // �ҵ��ˣ�������������id��Ч
        else
        {
            it++;
        }
    }
    

    if ( bChanged )
    {
        std::string  strScreendRooms;
        AssembleScreenedChatroomsString( strScreendRooms, v );
        m_cConfig.SetConfig( LibJingleEx::SCREENED_CHATROOMS_KEY, strScreendRooms.c_str() );
    }

    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
    

    if ( g_pfnOnMessage )
    {
        g_pfnOnMessage( LibJingleEx::ON_CUSTOM_CONFIG_DATA_READY, this, this->m_pUserData, 0, 0  );
    }
}

// �ӱ��ض�ȡ��������ʷ��¼
BOOL  CLibJinglexApp::GetChatRoomHistoryFromLocal( unsigned long & dwVersion )
{
    // ���û�����������ļ�·��
    if ( '\0' == m_szConfigPath[0] )
    {
        return FALSE;
    }

    std::string  sPath = m_szConfigPath;
    std::string  sDirSlash;
    char         chSlash;

#ifdef WIN32
    sDirSlash = "\\";
    chSlash   = '\\';
#else
    sDirSlash = "/";
    chSlash   = '/';
#endif

    if ( sPath.length() > 0 && sPath.at(sPath.length()-1) != chSlash  )
    {
        sPath += sDirSlash;
    }

    if ( 0 == m_pXmppThread->pump()->client() )
    {
        return FALSE;
    }

    sPath += m_pXmppThread->pump()->client()->jid().BareJid().Str();
    sPath += ".chatroom_history.xml";

    m_chatroom_history = ParseXmlFile( sPath.c_str() );

    if ( m_chatroom_history )
    {
        dwVersion = 0;
        sscanf( m_chatroom_history->Attr( buzz::QN_VERSION ).c_str(), "%lu", &dwVersion  );
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void  CLibJinglexApp::SaveChatRoomHistory2Local()
{
    char buf[MAX_BUFF_SIZE];

    // �������·��û������
    if ( 0 == m_szConfigPath[0] )
    {
        return;
    }

    // ���clientΪ0
    if ( 0 == m_pXmppThread->pump()->client() )
    {
        return;
    }

    if ( m_chatroom_history )
    {
        delete  m_chatroom_history;
        m_chatroom_history = 0;
    }


    m_chatroom_history = new buzz::XmlElement(buzz::QN_CHATROOM_HISTORY, true);
    SNPRINTF( buf, MAX_BUFF_SIZE, "%lu", m_dwChatroomHistoryVersion );
    m_chatroom_history->SetAttr( buzz::QN_VERSION, buf );

    std::vector< CRoomReadMsgs *>::iterator it;
    for ( it = m_vRoomReadMsgs.begin(); it != m_vRoomReadMsgs.end(); it++ )
    {
        CRoomReadMsgs * p = *it;

        // �鿴�Ƿ��ж�Ӧ�̶�Ⱥ
        vPersistentRoom::iterator it_p = find_if( m_vPersistentRooms.begin(), m_vPersistentRooms.end(),
                                                  FindPersistentRoom( p->m_szRoomId, p->m_szGroupChatServiceName ) );
        // ��������ڳ־��Է���
        if ( it_p == m_vPersistentRooms.end() )
        {
            continue;
        }
        // END


        buzz::XmlElement * groupchat = new buzz::XmlElement( buzz::QN_GROUPCHAT, true );
        m_chatroom_history->AddElement( groupchat );

        groupchat->SetAttr( buzz::QN_ID, p->m_szRoomId );
        groupchat->SetAttr( buzz::QN_CHATROOM_SERVICE, p->m_szGroupChatServiceName );

        unsigned long i;
        unsigned long cnt = p->m_room.GetCount();

        for ( i = 0; i < cnt; i++ )
        {
            char * pMsgId = p->m_room[i];
            assert( pMsgId );

            buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ITEM, true );
            groupchat->AddElement( item );

            item->SetBodyText( pMsgId );
        }
    }




    std::string  sPath = m_szConfigPath;
    std::string  sDirSlash;
    char         chSlash;

#ifdef WIN32
    sDirSlash = "\\";
    chSlash   = '\\';
#else
    sDirSlash = "/";
    chSlash   = '/';
#endif

    if ( sPath.length() > 0 && sPath.at(sPath.length()-1) != chSlash  )
    {
        sPath += sDirSlash;
    }

    sPath += m_pXmppThread->pump()->client()->jid().BareJid().Str();
    sPath += ".chatroom_history.xml";

    WriteXmlFile( m_chatroom_history, sPath.c_str() );

    delete m_chatroom_history;
    m_chatroom_history = 0;
}

void  CLibJinglexApp::ParseChatRoomHistory( const buzz::XmlElement * history )
{
    assert ( history );

    const buzz::XmlElement * groupchat= history->FirstElement();
    while( groupchat )
    {
        const std::string &   room_id      = groupchat->Attr( buzz::QN_ID );
        const std::string &   room_service = groupchat->Attr( buzz::QN_CHATROOM_SERVICE );

        CRoomReadMsgs * pHistoryRoom = 0;
        std::vector< CRoomReadMsgs *>::iterator   it_find;

        it_find = find_if( m_vRoomReadMsgs.begin(), m_vRoomReadMsgs.end(), FindHistoryRoom( room_id.c_str(), room_service.c_str()) );
        if ( it_find != m_vRoomReadMsgs.end() )
        {
            pHistoryRoom = *it_find;
            pHistoryRoom->m_room.Clear();
        }
        else
        {
            pHistoryRoom = new CRoomReadMsgs( room_id.c_str(), room_service.c_str() );
            this->m_vRoomReadMsgs.push_back( pHistoryRoom );
        }

        const buzz::XmlElement * item = groupchat->FirstElement();
        while( item )
        {
            pHistoryRoom->m_room.AddMessageId( item->BodyText().c_str() );
            item = item->NextElement();
        }

        groupchat = groupchat->NextElement();
    }
}



// ��ȡroster�����ļ�
BOOL  CLibJinglexApp::GetRosterData( char * szRosterVersion, DWORD dwRoseterVersionSize )
{
    assert(szRosterVersion);

    // ���û�����������ļ�·��
    if ( '\0' == m_szConfigPath[0] )
    {
        return FALSE;
    }

    std::string  sPath = m_szConfigPath;
    std::string  sDirSlash;
    char         chSlash;

#ifdef WIN32
    sDirSlash = "\\";
    chSlash   = '\\';
#else
    sDirSlash = "/";
    chSlash   = '/';
#endif

    if ( sPath.length() > 0 && sPath.at(sPath.length()-1) != chSlash  )
    {
        sPath += sDirSlash;
    }

    // sPath += ROSTER_DATA_FILE_NAME;
    if ( 0 == m_pXmppThread->pump()->client() )
    {
        return FALSE;
    }

    sPath += m_pXmppThread->pump()->client()->jid().BareJid().Str();
    sPath += ".xml";

    m_roster_root = ParseXmlFile( sPath.c_str() );

    if ( m_roster_root )
    {
        // ��ȡroster�汾��
        strncpy( szRosterVersion, m_roster_root->Attr( buzz::QN_VER ).c_str(), dwRoseterVersionSize );
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void  CLibJinglexApp::SaveRoster2File()
{
    char buf[MAX_BUFF_SIZE];

    // ���roster�汾��û�иı䣬���߱���roster���ݵ�·��û������
    if ( !m_bRosterChanged || 0 == m_szConfigPath[0] )
    {
        return;
    }

    // ���clientΪ0
    if ( 0 == m_pXmppThread->pump()->client() )
    {
        return;
    }

    if ( m_roster_root )
    {
        delete m_roster_root;
        m_roster_root = 0;
    }

    m_roster_root = new buzz::XmlElement( buzz::QName("", "roster") );
    m_roster_root->SetAttr( buzz::QN_VER, m_szRosterVer );

    buzz::XmlElement * groups = new buzz::XmlElement( buzz::QName("", "groups") );
    m_roster_root->AddElement( groups );


    LibJingleEx::PTGroup pGroup = m_pFirstGroup;
    while( pGroup )
    {
        if ( '\0' != pGroup->szName[0] )
        {
            buzz::XmlElement * group = new buzz::XmlElement( buzz::QName("", "group") );
            group->SetBodyText( AnsiToUtf8( pGroup->szName, buf, MAX_BUFF_SIZE) );
            //group->SetBodyText(  pGroup->szName );
            groups->AddElement( group );
        }

        pGroup = pGroup->pNext;
    }

    buzz::XmlElement * contacts = new buzz::XmlElement( buzz::QName("", "contacts") );
    m_roster_root->AddElement( contacts );


    
    vMemberGroups  v;

    pGroup = m_pFirstGroup;
    while( pGroup )
    {
        LibJingleEx::PTMember pChild = pGroup->pChild;
        while( pChild )
        {
            vMemberGroups::iterator it_find_v = find_if( v.begin(), v.end(), FindMemberGroupObj( &pChild->tJid ) ) ;
            // û���ҵ�
            if ( it_find_v == v.end() )
            {
                vMemberGroups::iterator it_find_m = find_if( m_MemberGroups.begin(), m_MemberGroups.end(), FindMemberGroupObj( &pChild->tJid ) ) ;
                if ( it_find_m != m_MemberGroups.end() )
                {
                    v.push_back( *it_find_m );
                }
            }
            pChild = pChild->pNext;
        }
        pGroup = pGroup->pNext;
    }

    //����v
    std::vector<PTMemberGroup>::iterator  it_v;
    for ( it_v = v.begin(); it_v != v.end(); it_v++ )
    {
        PTMemberGroup  pMemberGroup = *it_v;

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QName("", "item") );
        item->SetAttr( buzz::QN_SUBSCRIPTION, LibJingleEx::GetSubscription( pMemberGroup->m_tData.nSubscription ) );
        if ( 0 != pMemberGroup->m_tData.nAsk )
        {
            item->SetAttr( buzz::QN_ASK, LibJingleEx::GetAsk( pMemberGroup->m_tData.nAsk ) );
        }
        item->SetAttr( buzz::QN_NAME, AnsiToUtf8(pMemberGroup->m_tData.szName, buf, MAX_BUFF_SIZE) );
        // item->SetAttr( buzz::QN_NAME, pMemberGroup->m_tData.szName );

        char  szJid[512];
        LibJingleEx::TJid2StrJid( pMemberGroup->m_tJid,  szJid, 512 );
        item->SetAttr( buzz::QN_JID, AnsiToUtf8(szJid, buf, MAX_BUFF_SIZE) );
        // item->SetAttr( buzz::QN_JID, szJid );

        if ( pMemberGroup->m_Groups.size() > 0 )
        {
            vCStrings::iterator  it_s;
            for ( it_s = pMemberGroup->m_Groups.begin(); it_s != pMemberGroup->m_Groups.end(); it_s++ )
            {
                const char * szGroup = *it_s;

                buzz::XmlElement * group = new buzz::XmlElement( buzz::QName("", "group") );
                group->SetBodyText( AnsiToUtf8(szGroup, buf, MAX_BUFF_SIZE) );
                // group->SetBodyText( *it_s );
                item->AddElement( group );
            }
        }

        // ��������
        if ( '\0' != pMemberGroup->m_tData.szBitRate[0] )
        {
            buzz::XmlElement * bitrate = new buzz::XmlElement( buzz::QName("", "bitrate") );
            bitrate->SetAttr( buzz::QN_VALUE, AnsiToUtf8(pMemberGroup->m_tData.szBitRate, buf, MAX_BUFF_SIZE) );
            item->AddElement( bitrate );
        }

        if ( LibJingleEx::DMS_MEMBER_TYPE_JID != pMemberGroup->m_tData.nType )
        {
            buzz::XmlElement * type_ele = new buzz::XmlElement( buzz::QName("", "type") );
            SNPRINTF( buf, MAX_BUFF_SIZE, "%d", pMemberGroup->m_tData.nType );
            type_ele->SetAttr( buzz::QN_VALUE, buf );
            item->AddElement( type_ele );
        }

        contacts->AddElement( item );
    }

    std::string  sPath = m_szConfigPath;
    std::string  sDirSlash;
    char         chSlash;

#ifdef WIN32
    sDirSlash = "\\";
    chSlash   = '\\';
#else
    sDirSlash = "/";
    chSlash   = '/';
#endif

    if ( sPath.length() > 0 && sPath.at(sPath.length()-1) != chSlash  )
    {
        sPath += sDirSlash;
    }

    // sPath += ROSTER_DATA_FILE_NAME;
    sPath += m_pXmppThread->pump()->client()->jid().BareJid().Str();
    sPath += ".xml";

    WriteXmlFile( m_roster_root, sPath.c_str() );

    delete m_roster_root;
    m_roster_root = 0;

    m_bRosterChanged = FALSE;
}

LibJingleEx::PTGroup   CLibJinglexApp::AddGroup( const char * szGroupName, BOOL * pbAdded )
{
    assert( szGroupName );

    BOOL  bSpecialGroup = FALSE;

    // ����ǡ�δ������ϵ�ˡ���������������Ϊ��
    if ( 0 == strcmp( szGroupName, SPECIAL_GROUP_NAME ) || '\0' == szGroupName[0] )
    {
        szGroupName   = "";
        bSpecialGroup = TRUE;
    }

    BOOL bAdded = FALSE;

    LibJingleEx::PTGroup  pLast = 0;
    LibJingleEx::PTGroup  pCur  = m_pFirstGroup;

    while( pCur )
    {
        if ( 0 == strcmp( pCur->szName, szGroupName ) )
        {
            break;
        }

        pLast = pCur;
        pCur  = pCur->pNext;
    }

    if ( 0 == pCur )
    {
        LibJingleEx::PTGroup pNewGroup = new LibJingleEx::TGroup;
        memset( pNewGroup, 0, sizeof(LibJingleEx::TGroup) );

        strncpy( pNewGroup->szName, szGroupName, LibJingleEx::MAX_GROUP_NAME_LEN - 1 );
        pNewGroup->szName[LibJingleEx::MAX_GROUP_NAME_LEN - 1] = '\0';

        if ( bSpecialGroup )
        {
            pNewGroup->dwGroupId = SPECIAL_GROUP_SN;
        }
        else
        {
            m_dwMaxGroupId++;
            while( m_dwMaxGroupId == SPECIAL_GROUP_SN )
            {
                m_dwMaxGroupId++;
            }
            pNewGroup->dwGroupId = m_dwMaxGroupId;
        }


        if ( 0 == pLast )
        {
            m_pFirstGroup = pNewGroup;
        }
        else
        {
            pLast->pNext = pNewGroup;
        }

        bAdded = TRUE;
        if ( 0 != pbAdded )
        {
            *pbAdded = bAdded;
        }

        return pNewGroup;
    }
    else
    {
        bAdded = FALSE;
        if ( 0 != pbAdded )
        {
            *pbAdded = bAdded;
        }
        return pCur;
    }
}


LibJingleEx::PTGroup  CLibJinglexApp::FindGroup( unsigned long dwGroupId )
{
    assert( dwGroupId > 0 );

    LibJingleEx::PTGroup  pCur  = m_pFirstGroup;
    while( pCur )
    {
        if ( pCur->dwGroupId == dwGroupId )
        {
            break;
        }
        pCur  = pCur->pNext;
    }
    return pCur;
}

unsigned long  CLibJinglexApp::GetFriendsCount()
{
    unsigned long dwCnt = 0;

    LibJingleEx::PTGroup  pCur  = m_pFirstGroup;
    while( pCur )
    {
        LibJingleEx::PTMember pChild = pCur->pChild;
        while( pChild )
        {
            if ( !IfUnscribedContact(pChild) )
            {
                dwCnt++;
            }
            pChild = pChild->pNext;
        }
        
        pCur  = pCur->pNext;
    }

    return dwCnt;
}

LibJingleEx::PTGroup  CLibJinglexApp::FindGroupByName( const char * szName )
{
    assert( szName );

    LibJingleEx::PTGroup  pCur  = m_pFirstGroup;
    while( pCur )
    {
        if ( 0 == strcmp( pCur->szName, szName ) )
        {
            break;
        }
        pCur  = pCur->pNext;
    }
    return pCur;
}

BOOL  CLibJinglexApp::RemoveGroup( LibJingleEx::PTGroup pGroup )
{
    assert ( pGroup );

    LibJingleEx::PTGroup  pCur  = m_pFirstGroup;
    LibJingleEx::PTGroup  pPrev = 0;
    while( pCur )
    {
        if ( pCur == pGroup )
        {
            break;    
        }
        pPrev = pCur;
        pCur  = pCur->pNext;
    }
    
    // û���ҵ�
    if ( 0 == pCur )
    {
        return FALSE;
    }

    // ���ǰһ������NULL
    if ( 0 == pPrev )
    {
        m_pFirstGroup = pCur->pNext;
    }
    else
    {
        pPrev->pNext  = pCur->pNext;
    }

    // ɾ��pCur
    LibJingleEx::PTMember pChild = pCur->pChild;
    while( pChild )
    {
        LibJingleEx::PTMember  pTmp = pChild;
        pChild = pChild->pNext;

        delete  pTmp;
    }
    delete pCur;

    return TRUE;
}

BOOL CLibJinglexApp::AddMember( LibJingleEx::PTGroup & pGroup, LibJingleEx::PTMember pMember, 
                                LibJingleEx::PTMember pOld /*= 0*/, BOOL * pbUiChanged /*= 0*/,
                                BOOL * pbDmsAdded /*= 0*/, BOOL * pbDmsDeleted /*= 0*/ )
{
    assert( 0 == pMember->pNext );

    bool bFind = false;

    LibJingleEx::PTMember pLast = 0;
    LibJingleEx::PTMember pCur = pGroup->pChild;
    while( pCur )
    {
        // �ҵ���ͬJID��member
        if ( IfBareJidEqual( pCur->tJid, pMember->tJid ) )
        {
            bFind = true;
            break;
        }

        pLast = pCur;
        pCur = pCur->pNext;
    }

    BOOL  bNewUnscribed = IfUnscribedContact(pMember);


    // ����ҵ�ͬ��JID�ĳ�Ա���޸�һ�³�Ա���ԣ�����
    if ( bFind )
    {
        if ( pOld )
        {
            memcpy( pOld, pCur, sizeof(LibJingleEx::TMember) );
        }

        if ( pbUiChanged )
        {
            *pbUiChanged = IfMemberUiChanged( pCur, pMember );
        }

        BOOL  bOldUnscribed = IfUnscribedContact(pCur);

        if ( bOldUnscribed == bNewUnscribed )
        {
            if ( pbDmsAdded )
            {
                *pbDmsAdded = FALSE;
            }

            if ( pbDmsDeleted )
            {
                *pbDmsDeleted = FALSE;
            }
        }
        else
        {
            // ��û�ж��ĵ����ģ���������ϵ��
            if ( bOldUnscribed )
            {
                if ( pbDmsAdded )
                {
                    *pbDmsAdded = TRUE;
                }

                if ( pbDmsDeleted )
                {
                    *pbDmsDeleted = FALSE;
                }
            }
            // �Ӷ��ĵ�û�ж���
            else
            {
                if ( pbDmsAdded )
                {
                    *pbDmsAdded = FALSE;
                }

                if ( pbDmsDeleted )
                {
                    *pbDmsDeleted = TRUE;
                }
            }
        }

        pCur->nAsk          = pMember->nAsk;
        pCur->nSubscription = pMember->nSubscription;
        pCur->nType         = pMember->nType;
        pCur->pData         = pMember->pData;
        strcpy( pCur->szBitRate, pMember->szBitRate );
        strcpy( pCur->szName, pMember->szName );
        
        pMember->dwId = pCur->dwId;
        return FALSE;
    }

    m_dwMaxMemberId++;
    LibJingleEx::PTMember pNewMember = new LibJingleEx::TMember;
    memcpy( pNewMember, pMember, sizeof(LibJingleEx::TMember) );
    pNewMember->dwId = m_dwMaxMemberId;

    if ( 0 == pLast )
    {
        assert( 0 == pGroup->pChild );
        pGroup->pChild = pNewMember;
    }
    else
    {
        pLast->pNext = pNewMember;
    }

    pMember->dwId = pNewMember->dwId;

    // û�ж���
    if ( bNewUnscribed )
    {
        if ( pbDmsAdded )
        {
            *pbDmsAdded = FALSE;
        }
        if ( pbDmsDeleted )
        {
            *pbDmsDeleted = FALSE;
        }
    }
    // ������
    else
    {
        if ( pbDmsAdded )
        {
            *pbDmsAdded = TRUE;
        }
        if ( pbDmsDeleted )
        {
            *pbDmsDeleted = FALSE;
        }
    }

    return TRUE;
}

LibJingleEx::PTMember CLibJinglexApp::FindMember( unsigned long dwMemberId, LibJingleEx::PTGroup & pParentGroup )
{
    if ( 0 == dwMemberId )
    {
        return 0;
    }

    LibJingleEx::PTGroup pGroup = m_pFirstGroup;

    while( pGroup )
    {
        LibJingleEx::PTMember pChild = pGroup->pChild;
        while( pChild )
        {
            if ( pChild->dwId == dwMemberId )
            {
                pParentGroup = pGroup;
                return pChild;
            }

            pChild = pChild->pNext;
        }
        pGroup = pGroup->pNext;
    }

    return 0;
}

// ֪ͨ�ϲ����Ⱥ��
void  CLibJinglexApp::NotifyBufferedChatRoomMsgs( PTRoom pRoom )
{
    // ֪ͨ�ϲ���Ϣ
    vCachedGrouchatMsgs::iterator it;
    vCachedGrouchatMsgs & v = pRoom->m_vDelayedMsgs;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        PTCachedGroupchatMsg pCachedMsg = * it;

#if  IM_DEBUG
        pRoom->m_dwDelayMsgCnt++;
#endif

        if ( g_pfnOnMessage )
        {
          LibJingleEx::CMsgData msgData;

          msgData.Append( (unsigned char *)&pRoom,                       sizeof(pRoom) );
          msgData.Append( (unsigned char *)&pCachedMsg->tMemberJid,      sizeof(pCachedMsg->tMemberJid) );
          msgData.Append( (unsigned char *)&pCachedMsg->bOffline,        sizeof(pCachedMsg->bOffline) );
          msgData.Append( (unsigned char *)pCachedMsg->szMsgId,          sizeof(pCachedMsg->szMsgId) );
          msgData.Append( (unsigned char *)&pCachedMsg->tMsg,            sizeof(pCachedMsg->tMsg)  );

          msgData.Append( (unsigned char *)pRoom->m_szRoomId,               sizeof(pRoom->m_szRoomId)  );
          msgData.Append( (unsigned char *)pRoom->m_szGroupChatServiceName, sizeof(pRoom->m_szGroupChatServiceName)  );

          g_pfnOnMessage( LibJingleEx::ON_GROUPCHAT_MESSAGE, this, m_pUserData, 
                          msgData.GetMsgData(),  msgData.GetMsgLength() );
        }

        delete pCachedMsg;
    }

    v.clear();
    // ����( ֪ͨ�ϲ���Ϣ )
}

void  CLibJinglexApp::NotifyAllBufferedChatroomMsgs()
{
    vRoom::iterator  it;
    for ( it = m_Rooms.begin();  it != m_Rooms.end(); it++ )
    {
        NotifyBufferedChatRoomMsgs( *it );
    }
}

const char *  CLibJinglexApp::FindTrueSocialName( LibJingleEx::TJid *  ptJid )
{
    vMemberGroups::iterator it_find = find_if( m_MemberGroups.begin(), m_MemberGroups.end(), FindMemberGroupObj(ptJid) );
    if ( it_find != m_MemberGroups.end() )
    {
        PTMemberGroup  pMg = *it_find;
        if ( '\0' != pMg->m_szTrueSocialName[0] )
        {
            return pMg->m_szTrueSocialName;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

void  CLibJinglexApp::SendRoomPictureNextStep( const char * szGroupChatMessageId )
{
    std::vector<TSendRoomPictureParam *>::iterator it = m_vSendRoomPictureParams.begin();
    // û�����ݣ�����
    if ( it == m_vSendRoomPictureParams.end() )
    {
        return;
    }

    char   buf[MAX_BUFF_SIZE];
    char   szMsgId[MAX_RAND_ID_LENGTH];

    TSendRoomPictureParam * pParam = *it;

    buzz::XmppClient * client = m_pXmppThread->pump()->client();
    // ���clientΪ0
    if ( 0 == client )
    {
        return;
    }

    // ��Ϣid��ƥ��
    if ( 0 != szGroupChatMessageId && '\0' != m_szLastSendPictureId[0] && 0 != strcmp(szGroupChatMessageId, m_szLastSendPictureId) )
    {
        return;
    }

    // �����仹�ڲ���
    vRoom::iterator  it_room;
    for ( it_room = m_Rooms.begin();  it_room != m_Rooms.end(); it_room++ )
    {
        PTRoom  pRoom = *it_room;
        if ( pRoom == pParam->m_pRoom )
        {
            break;
        }
    }

    // ���û���ҵ���room
    if ( it_room == m_Rooms.end() )
    {
        delete  pParam;
        m_vSendRoomPictureParams.erase( it );

        SendRoomPictureNextStep( 0 );
        return;
    }

    std::string  sRoomId = pParam->m_pRoom->m_szRoomId;
    sRoomId += "@";
    sRoomId += pParam->m_pRoom->m_szGroupChatServiceName;

    
    unsigned long dwCurTick = GetCurTick();


    // ��ʼ�׶�
    if ( 0 == pParam->m_state )
    {
        assert( 0 == pParam->m_fp );

        pParam->m_fp = XmppOpenFile( pParam->m_szPictureFilePath, "rb" );
        // ���ļ�ʧ��
        if ( 0 == pParam->m_fp )
        {
            delete  pParam;
            m_vSendRoomPictureParams.erase( it );

            // ������һ��
            SendRoomPictureNextStep( 0 );
            return;
        }
        

        // ����ͷ
        buzz::XmlElement * message_open = new buzz::XmlElement( buzz::QN_MESSAGE );

        message_open->SetAttr( buzz::QN_TO, sRoomId );
        GenerateRandomId( szMsgId, MAX_RAND_ID_LENGTH, 0 );
        message_open->SetAttr( buzz::QN_ID, szMsgId );
        message_open->SetAttr( buzz::QN_TYPE, "groupchat" );


        buzz::XmlElement * open = new buzz::XmlElement( buzz::QN_IBB_OPEN, true );
        message_open->AddElement( open );

        SNPRINTF( buf, MAX_BUFF_SIZE, "%u", pParam->m_dwPictureSize );
        open->SetAttr( buzz::QN_BLOCK_SIZE, buf );

        GenerateRandomId( pParam->m_szSid, MAX_RAND_ID_LENGTH, 0 );
        open->SetAttr( buzz:: QN_SID, pParam->m_szSid );

        char  szPicName[LibJingleEx::MAX_FILENAME_SIZE];
        GetFileName( szPicName, LibJingleEx::MAX_FILENAME_SIZE, pParam->m_szPictureFilePath );

        open->SetAttr( buzz::QN_NAME, AnsiToUtf8( szPicName, buf, MAX_BUFF_SIZE ) );

        client->SendStanza( message_open );

        delete message_open;
        // END(����ͷ)

        pParam->m_state = 1;

        // ����msg id��tick
        strcpy( m_szLastSendPictureId, szMsgId );
        m_dwLastSendPictureTick = dwCurTick;

        return;
    }
    // ���͹�ͷ
    else if ( 1 == pParam->m_state )
    {
        assert( pParam->m_fp );

        // �����ʱ
        if ( dwCurTick - m_dwLastSendPictureTick >= SEND_PICTURE_TIMEOUT )
        {
            delete  pParam;
            m_vSendRoomPictureParams.erase( it );

            SendRoomPictureNextStep( 0 );
            return;
        }


        // ��������
        size_t         dwRead      = 0;
        unsigned long  dwEncodeLen = 0;
        char           data_buf[MAX_BYTE_PER_TRANSFER];
        char           encode_buf[MAX_BYTE_PER_TRANSFER*2];


        dwRead = fread( data_buf, 1, MAX_BYTE_PER_TRANSFER, pParam->m_fp );
        if ( dwRead > 0 )
        {
            dwEncodeLen = MAX_BYTE_PER_TRANSFER*2;
            LibJingleEx::EncodeBase64( encode_buf, dwEncodeLen , (unsigned char *)data_buf, dwRead );
            encode_buf[dwEncodeLen] = '\0';


            buzz::XmlElement * message_data = new buzz::XmlElement(buzz::QN_MESSAGE);

            message_data->SetAttr( buzz::QN_TO, sRoomId );
            GenerateRandomId( szMsgId, MAX_RAND_ID_LENGTH, 0 );
            message_data->SetAttr( buzz::QN_ID, szMsgId );
            message_data->SetAttr( buzz::QN_TYPE, "groupchat" );

            buzz::XmlElement * data = new buzz::XmlElement( buzz::QN_IBB_DATA, true );
            message_data->AddElement( data );

            SNPRINTF( buf, MAX_BUFF_SIZE, "%u", pParam->m_dwSequence);
            data->SetAttr( buzz::QN_SEQ, buf );
            pParam->m_dwSequence++;

            data->SetAttr( buzz::QN_SID, pParam->m_szSid );

            data->SetBodyText( encode_buf );

            client->SendStanza( message_data );

            delete message_data;
        }
        // һ��ͼƬ���ݷ���
        else
        {
            // ����β
            buzz::XmlElement * message_close = new buzz::XmlElement( buzz::QN_MESSAGE );

            message_close->SetAttr( buzz::QN_TO, sRoomId );
            GenerateRandomId( szMsgId, MAX_RAND_ID_LENGTH, 0 );
            message_close->SetAttr( buzz::QN_ID, szMsgId );
            message_close->SetAttr( buzz::QN_TYPE, "groupchat" );


            buzz::XmlElement * close = new buzz::XmlElement( buzz::QN_IBB_CLOSE, true );
            message_close->AddElement( close );

            close->SetAttr( buzz:: QN_SID, pParam->m_szSid );

            client->SendStanza( message_close );

            delete message_close;
            // END(����β)

            delete pParam;
            m_vSendRoomPictureParams.erase(it);
        }

        // ����msg id��tick
        strcpy( m_szLastSendPictureId, szMsgId );
        m_dwLastSendPictureTick = dwCurTick;
    }
}
/***********************  END CLibJinglexApp     ***********************/











/**********************            CGlobalData          *******************/
CGlobalData * CGlobalData::s_pInstance = 0;

CGlobalData * CGlobalData::GetInstance()
{
    if ( 0 == s_pInstance )
    {
        s_pInstance = new CGlobalData;
    }

    return s_pInstance;
}

CGlobalData::CGlobalData()
{
    m_bInited = FALSE;

    unsigned int dwCurTime = (unsigned int)time(0);
    srand( dwCurTime );

    // ��ʼ�������ֱ��������ٻ��ansi, utf �� jid�ַ���
    int i;
    for ( i = 0; i < GLOBAL_LOCK_COUNT; i++ )
    {
        JInitLock( &m_arrLocks[i] );
    }

    // ��ʼ��sslģ��
    bool bRet = talk_base::InitializeSSL ( );
    if ( !bRet )
    {
        MYLOG("Failed to InitializeSSL() \r\n");
        return;
    }

    memset( m_szTmpPath, 0, sizeof(m_szTmpPath) );

    m_dwMaxTotalRate   = 0;
    m_dwFileSessionCnt = 0;

    // m_dwFileShareSleepTime = 0;

    m_bInited = TRUE;

    m_dwMaxDmsMembersPerBlock = LibJingleEx::MAX_DMS_TOTAL_MEMBERS_COUNT / DMS_MEMBERS_BLOCK_CNT;
    m_dwMaxDmsMembersPerBlock++;

    memset( m_szCompileTime, 0, sizeof(m_szCompileTime) );
    SNPRINTF( m_szCompileTime, MAX_COMPILE_TIME_LEN,  "%s %s", __DATE__, __TIME__ );

#ifdef SUPPORT_ICONV
    m_cd_ansi_to_utf8 = iconv_open( "utf-8", "gbk"   );
    m_cd_utf8_to_ansi = iconv_open( "gbk",   "utf-8" );
#endif

#ifdef WIN32
    TIME_ZONE_INFORMATION   tzi;
    GetTimeZoneInformation(&tzi);
    m_nBias = tzi.Bias * 60;
#else
    struct timezone  tz;
    gettimeofday( 0, &tz );
    m_nBias = tz.tz_minuteswest * 60;
#endif

}

CGlobalData::~CGlobalData()
{
#ifdef SUPPORT_ICONV
    iconv_close( m_cd_ansi_to_utf8 );
    iconv_close( m_cd_utf8_to_ansi );
#endif

    talk_base::CleanupSSL();

    int i;
    for ( i = 0; i < GLOBAL_LOCK_COUNT; i++ )
    {
        JDeinitLock( &m_arrLocks[i] );
    }
}


/* ========================== �ļ����� =====================================*/

unsigned long FileShareClient::s_dwNo = 0;

FileShareClient::FileShareClient(  )
{
    s_dwNo++;

    m_dwNo     = s_dwNo;

    m_pSession = 0;
    m_hApp     = 0;

    m_dwProgress = 0;

    m_IsSender   = FALSE;
    memset( m_szLocalDir, 0, sizeof( m_szLocalDir) );

    memset( m_szSessionId, 0, sizeof(m_szSessionId) );

    m_bEof = FALSE;
    m_stream = 0;

    memset( m_szTransferName, 0, sizeof( m_szTransferName ) );
    m_counter  = 0;
    memset( m_szTempFilePath, 0, sizeof( m_szTempFilePath) );
    memset( m_szSaveAsName, 0, sizeof(m_szSaveAsName) );

    m_dwLastTimeTick  =  0;
    m_dwLastSize      =  0;
    m_dwDeltaTick     =  0;
    m_dwDeltaSize     =  0;


    // ���յ�cancel��fail��Ϣʱ���Ƿ���Ҫpost��pump
    m_bPost2Pump   = TRUE;

    m_bPeerOffline = FALSE;
}

FileShareClient::~FileShareClient()
{
    if ( m_pSession )
    {
        // ��Ҫ�� post MSG_STOP_FILE_SHARE �� pump
        m_bPost2Pump = FALSE;
        delete m_pSession;
        m_pSession = 0;
    }

    // m_counter �� m_stream�ǰ���һ���( m_counter��scope_ptr���� )
    if ( m_counter )
    {
        if ( m_stream )
        {
            m_stream->Close();
        }

        delete m_counter;
        m_counter = 0;
        m_stream = 0;
    }
}

void FileShareClient::OnSessionState(cricket::FileShareState state)
{
    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)m_hApp;
    cFuncLock cLock_( &pAppObj->m_lock );

    // talk_base::Thread *thread = talk_base::ThreadManager::CurrentThread();
    talk_base::Thread *thread = pAppObj->m_pXmppThread;
    XmppPump * pump           = pAppObj->m_pXmppThread->pump();

    switch( state )
    {
    case cricket::FS_OFFER:
        {
            // ����session����
            FileShareManifestConvert( m_Manifest, *m_pSession->manifest() );
            StrJid2TJid( m_pSession->jid().Str().c_str(), m_PeerJid );
            strncpy( m_szLocalDir, m_pSession->local_folder().c_str(), LibJingleEx::MAX_PATH_SIZE );
            m_IsSender = ( m_pSession->is_sender() ? TRUE : FALSE );
            strncpy( m_szSessionId, m_pSession->session()->id().id_str().c_str(), MAX_SESSION_ID_LEN );
            // strncpy( m_szSaveAsName, m_pSession->m_sSaveAsName.c_str(), 256 );

            if ( g_tEvents.pOnFileOffer )
            {
                LibJingleEx::TJid tJid;
                StrJid2TJid( m_pSession->jid().Str().c_str(), tJid );

                LibJingleEx::FileShareManifest m;
                FileShareManifestConvert( m, *m_pSession->manifest() );

                g_tEvents.pOnFileOffer( m_hApp, this, &tJid, &m, (m_pSession->is_sender()?TRUE:FALSE) );
            }
            else if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData   msgData;

                LibJingleEx::HSession   hSession = this;
                msgData.Append( (unsigned char *)&hSession,   sizeof(hSession)  );

                LibJingleEx::TJid tJid;
                StrJid2TJid( m_pSession->jid().Str().c_str(), tJid );
                msgData.Append( (unsigned char *)&tJid,       sizeof(tJid) );

                LibJingleEx::FileShareManifest m;
                FileShareManifestConvert( m, *m_pSession->manifest() );
                msgData.Append( (unsigned char *)&m,  sizeof(m) );

                BOOL bSend = (m_pSession->is_sender()?TRUE:FALSE);
                msgData.Append( (unsigned char *)&bSend, sizeof(bSend) );

                g_pfnOnMessage( LibJingleEx::ON_FILE_OFFER,  pAppObj, pAppObj->m_pUserData,
                    msgData.GetMsgData(),   msgData.GetMsgLength() );
            }
        }
        break;
    case cricket::FS_TRANSFER:
        {
            m_dwLastTimeTick = GetCurTick();
            m_dwLastSize     = 0;
            m_dwDeltaTick    = 0;
            m_dwDeltaSize    = 0;

            if ( g_tEvents.pOnFileTransfer )
            {
                g_tEvents.pOnFileTransfer( m_hApp, this );
            }
            else if ( g_pfnOnMessage )
            {
                LibJingleEx::HSession   hSession = this;

                g_pfnOnMessage( LibJingleEx::ON_FILE_TRANSFER,  pAppObj, pAppObj->m_pUserData,
                    (unsigned char*)&hSession,  sizeof(hSession) );
            }
        }
        break;
    case cricket::FS_COMPLETE:
        {
            CGlobalData * pGlobal = CGlobalData::GetInstance();
            pGlobal->m_dwFileSessionCnt--;

            if ( g_tEvents.pOnFileComplete )
            {
                g_tEvents.pOnFileComplete( m_hApp, this, m_pSession->m_sCompletePath.c_str() );
            }
            else if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData  msgData;

                LibJingleEx::HSession   hSession = this;
                msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );

                msgData.Append( (unsigned char *)m_pSession->m_sCompletePath.c_str(),  
                    m_pSession->m_sCompletePath.length() + 1 );

                g_pfnOnMessage( LibJingleEx::ON_FILE_COMPLETE,  pAppObj, pAppObj->m_pUserData,
                    msgData.GetMsgData(),   msgData.GetMsgLength() );
            }
        }
        if ( m_bPost2Pump )
        {
            thread->Post(pump, MSG_STOP_FILE_SHARE,( talk_base::MessageData *)this );
        }
        break;

    case cricket::FS_LOCAL_CANCEL:
        {
            CGlobalData * pGlobal = CGlobalData::GetInstance();
            pGlobal->m_dwFileSessionCnt--;

            BOOL bLocal = TRUE;
            // ����ǶԷ����ߣ���Ϊ��Զ�̷�cancel
            if ( m_bPeerOffline )
            {
                bLocal = FALSE;
            }

            if ( g_tEvents.pOnFileCancel )
            {
                g_tEvents.pOnFileCancel( m_hApp, this, bLocal );
            }
            else if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData  msgData;

                LibJingleEx::HSession   hSession = this;
                msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );

                msgData.Append( (unsigned char *)&bLocal,  sizeof(bLocal) );

                g_pfnOnMessage( LibJingleEx::ON_FILE_CANCEL,  pAppObj, pAppObj->m_pUserData,
                    msgData.GetMsgData(),   msgData.GetMsgLength() );
            }
        }
        if ( m_bPost2Pump )
        {
            thread->Post(pump, MSG_STOP_FILE_SHARE, (talk_base::MessageData *)this);
        }
        break;

    case cricket::FS_REMOTE_CANCEL:
        {
            // ֱ�ӱ��Է��ܾ���
            if ( 1 != m_pSession->m_dwCancelFlag )
            {
                assert( 2 == m_pSession->m_dwCancelFlag );

                CGlobalData * pGlobal = CGlobalData::GetInstance();
                pGlobal->m_dwFileSessionCnt--;

                
                // ������ļ�û�ҵ�������ʧ�ܴ���
                if ( talk_base::HC_NOT_FOUND == this->m_pSession->err() )
                {
                    int nError = LibJingleEx::FILE_SHARE_ERR_NOT_FOUND;
                    if ( g_tEvents.pOnFileFailure )
                    {
                        g_tEvents.pOnFileFailure( m_hApp, this, nError );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        LibJingleEx::HSession   hSession = this;
                        msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );
                        msgData.Append( (unsigned char *)&nError,  sizeof(nError) );

                        g_pfnOnMessage( LibJingleEx::ON_FILE_FAILURE,  pAppObj, pAppObj->m_pUserData,
                            msgData.GetMsgData(),   msgData.GetMsgLength() );
                    }
                }
                // �����Է��ܾ�����
                else
                {
                    if ( g_tEvents.pOnFileCancel )
                    {
                        g_tEvents.pOnFileCancel( m_hApp, this, FALSE );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        LibJingleEx::HSession   hSession = this;
                        msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );

                        BOOL bLocal = FALSE;
                        msgData.Append( (unsigned char *)&bLocal,  sizeof(bLocal) );

                        g_pfnOnMessage( LibJingleEx::ON_FILE_CANCEL,  pAppObj, pAppObj->m_pUserData,
                            msgData.GetMsgData(),   msgData.GetMsgLength() );
                    }
                }

                
                if ( m_bPost2Pump )
                {
                    thread->Post(pump, MSG_STOP_FILE_SHARE,( talk_base::MessageData *)this);
                }
            }
            else
            {
                //! assert( 0 == m_dwProgress );
                if ( 0 != m_dwProgress )
                {
                    CGlobalData * pGlobal = CGlobalData::GetInstance();
                    pGlobal->m_dwFileSessionCnt--;

                    if ( g_tEvents.pOnFileCancel )
                    {
                        g_tEvents.pOnFileCancel( m_hApp, this, FALSE );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        LibJingleEx::HSession   hSession = this;
                        msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );

                        BOOL bLocal = FALSE;
                        msgData.Append( (unsigned char *)&bLocal,  sizeof(bLocal) );

                        g_pfnOnMessage( LibJingleEx::ON_FILE_CANCEL,  pAppObj, pAppObj->m_pUserData,
                            msgData.GetMsgData(),   msgData.GetMsgLength() );
                    }

                    if ( m_bPost2Pump )
                    {
                        thread->Post(pump, MSG_STOP_FILE_SHARE,( talk_base::MessageData *)this);
                    }                        
                }
                else
                {
                    assert( 1 == m_pSession->m_dwCancelFlag );
                    thread->Post(pump, MSG_P2P_FAIL,( talk_base::MessageData *)this);
                }
            }
        }
        break;

    case cricket::FS_FAILURE:
        {
            int nError = 0;

            if ( talk_base::HE_DISCONNECTED == this->m_pSession->err() )
            {
                nError = LibJingleEx::FILE_SHARE_ERR_DISCONNECTED;
            }
            else if ( talk_base::HE_STREAM == this->m_pSession->err() )
            {
                nError = LibJingleEx::FILE_SHARE_ERR_NO_SPACE;
            }
            else if ( talk_base::HC_NOT_FOUND == this->m_pSession->err() )
            {
                nError = LibJingleEx::FILE_SHARE_ERR_NOT_FOUND;
            }

            CGlobalData * pGlobal = CGlobalData::GetInstance();
            pGlobal->m_dwFileSessionCnt--;

            if ( g_tEvents.pOnFileFailure )
            {
                g_tEvents.pOnFileFailure( m_hApp, this, nError );
            }
            else if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData  msgData;

                LibJingleEx::HSession   hSession = this;
                msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );
                msgData.Append( (unsigned char *)&nError,  sizeof(nError) );

                g_pfnOnMessage( LibJingleEx::ON_FILE_FAILURE,  pAppObj, pAppObj->m_pUserData,
                    msgData.GetMsgData(),   msgData.GetMsgLength() );
            }
        }
        if ( m_bPost2Pump )
        {
            thread->Post(pump, MSG_STOP_FILE_SHARE,( talk_base::MessageData *)this);
        }

        break;
    default:
        break;
    }
}

void  FileShareClient::OnUpdateProgress( unsigned long dwTotalSize, const char * szCurFileName ) 
{
    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)m_hApp;
    cFuncLock cLock_( &pAppObj->m_lock );

    int nPercent = 0;

    if ( dwTotalSize > 0 )
    {
        float percent = (float)m_dwProgress / dwTotalSize;
        nPercent = (int)(percent * 100);
    }
    else
    {
        nPercent = 100;
    }

    // ���㴫������
    unsigned long dwCurTick   = GetCurTick();

    unsigned long dwDeltaTick = dwCurTick - m_dwLastTimeTick;
    unsigned long dwDeltaSize = m_dwProgress - m_dwLastSize;

    m_dwLastTimeTick = dwCurTick;
    m_dwLastSize     = m_dwProgress;

    m_dwDeltaTick += dwDeltaTick;
    m_dwDeltaSize += dwDeltaSize;

    unsigned long dwRate = 0;

    // if ( m_dwDeltaTick < 100 )
    if ( m_dwDeltaTick < UPDATE_FILE_TRANSFER_PROGRESS_INTERVAL )
    {
        return;
    }

    dwRate = (unsigned long)( (float)m_dwDeltaSize * 1000 / (float)m_dwDeltaTick );

    m_dwDeltaTick = 0;
    m_dwDeltaSize = 0;
    // ���㴫�����ʽ���


    if ( g_tEvents.pOnFileProgress )
    {
        g_tEvents.pOnFileProgress( m_hApp, this, nPercent, szCurFileName, m_dwProgress, dwRate );
    }
    else if( g_pfnOnMessage )
    {
        LibJingleEx::CMsgData   msgData;

        LibJingleEx::HSession   hSession = this;
        msgData.Append( (unsigned char *)&hSession,        sizeof(hSession) );
        msgData.Append( (unsigned char *)&nPercent,        sizeof(nPercent) );
        msgData.Append( (unsigned char *)&m_dwProgress,    sizeof(m_dwProgress) );
        msgData.Append( (unsigned char *)&dwRate,          sizeof(dwRate) );
        msgData.Append( (unsigned char *)szCurFileName,    strlen(szCurFileName) + 1 );

        g_pfnOnMessage( LibJingleEx::ON_FILE_PROGRESS, pAppObj, pAppObj->m_pUserData, 
            msgData.GetMsgData(),  msgData.GetMsgLength() );
    }
}

void FileShareClient::OnUpdateProgress(cricket::FileShareSession *sess)
{
    size_t totalsize ;
    std::string itemname;

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)m_hApp;
    cFuncLock cLock_( &pAppObj->m_lock );

    if(    m_pSession->GetTotalSize(totalsize) && m_pSession->GetProgress(m_dwProgress) 
        && m_pSession->GetCurrentItemName(&itemname) )
    {
        OnUpdateProgress( totalsize, itemname.c_str() );
    }
}

void FileShareClient::OnResampleImage(std::string path, int width, int height, talk_base::HttpTransaction *trans) 
{

}

void FileShareClient::OnNextEntry(const std::string& name, size_t size)
{
    strncpy( m_szTransferName, name.c_str(), LibJingleEx::MAX_FILENAME_SIZE );
}

void FileShareClient::OnUpdateBytes(size_t count)
{
    assert( 1 == m_Manifest.GetSize() );

    size_t dwCurItemSize = m_Manifest.GetItem(0).m_dwSize;
    if ( count > dwCurItemSize )
    {
        count = dwCurItemSize;
    }

    m_dwProgress = count;
}



void FileShareManifestConvert( cricket::FileShareManifest & m, const LibJingleEx::FileShareManifest & n )
{
    DWORD i;

    for ( i = 0; i < n.GetSize(); i++ )
    {
        const LibJingleEx::FileShareManifest::Item & itm = n.GetItem(i);

        if ( itm.m_eTyp == LibJingleEx::FileShareManifest::T_FILE )
        {
            m.AddFile( itm.m_szName, itm.m_dwSize );
        }
        else if ( itm.m_eTyp == LibJingleEx::FileShareManifest::T_FOLDER )
        {
            m.AddFolder( itm.m_szName, itm.m_dwSize );
        }
        else if ( itm.m_eTyp == LibJingleEx::FileShareManifest::T_IMAGE )
        {
            m.AddImage( itm.m_szName, itm.m_dwSize, itm.m_dwWidth, itm.m_dwHeight );
        }
    }

}

void FileShareManifestConvert( LibJingleEx::FileShareManifest & m, const cricket::FileShareManifest & n)
{
    DWORD i;

    for ( i = 0; i < n.size(); i++ )
    {
        const cricket::FileShareManifest::Item & itm = n.item(i);

        if ( itm.type == cricket::FileShareManifest::T_FILE )
        {
            m.AddFile( itm.name.c_str(), itm.size );
        }
        else if ( itm.type == cricket::FileShareManifest::T_FOLDER )
        {
            m.AddFolder( itm.name.c_str(), itm.size );
        }
        else if ( itm.type == cricket::FileShareManifest::T_IMAGE )
        {
            m.AddImage( itm.name.c_str(), itm.size, itm.width, itm.height );
        }
    }
}

/* ========================== �ļ��������  =====================================*/

namespace LibJingleEx
{



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
        const char * szLocalDir /* = 0 */ )
    {
        if ( 0 == hApp || 0 == pSession || 0 == ptJid || 0 == pManifest )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        if ( '\0' == ptJid->sResource[0] || 0 == pManifest->GetSize() )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        if ( !pAppObj->m_bFileShareAvailable )
        {
            return LIBJINGLE_ERR_NO_FILE_SHARE_ABILITY;
        }


        cFuncLock cLock( &pAppObj->m_lock );

        // �鿴�Է������ļ���������
        // ���û���ҵ��Է�����ʱ��Ϊ�Է�������
        vMemberGroups & v = pAppObj->m_MemberGroups;

        LibJingleEx::TJid tJid;
        memcpy( &tJid, ptJid, sizeof(tJid) );
        tJid.sResource[0] = '\0';

        vMemberGroups::iterator it_find = find_if( v.begin(), v.end(), FindMemberGroupObj(&tJid) );
        // ����ҵ�ͬbare jid
        if ( it_find != v.end() )
        {
            PTMemberGroup p   = *it_find;
            vMemberStatus & s = p->m_Status;

            vMemberStatus::iterator it;
            for ( it = s.begin(); it != s.end(); it++ )
            {
                PTMemberStatus pStatus = *it;
                // �ҵ�ͬresource
                if ( 0 == strcmp(pStatus->m_szResource, ptJid->sResource) )
                {
                    // ����Է�û���ļ���������
                    if ( !pStatus->m_bFileShareAvailable )
                    {
                        return LIBJINGLE_ERR_PEER_NO_FILE_SHARE_ABILITY;
                    }
                    break;
                }
            }
        }

        FileShareClient * pFileShareClient = new FileShareClient;

        pAppObj->m_pXmppThread->ShareFiles( pFileShareClient, ptJid, pManifest, szLocalDir );

        *pSession = pFileShareClient;
        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�AcceptSession
    // ���ܣ�    �����ļ���������
    /********************************************************/
    LIBJINGLE_API ELibjingleError  AcceptSession( HSession hSession, const char * szSavePath /* = 0 */ )
    {
        if ( 0 == hSession )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        FileShareClient * pFileShareClient = (FileShareClient *)hSession;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistFileShareSession(pFileShareClient) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pFileShareClient->m_hApp;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        pAppObj->m_pXmppThread->AcceptFiles( pFileShareClient->m_pSession, szSavePath );

        return LIBJINGLE_ERR_OK;
    }

    /********************************************************/
    // �������ƣ�DeclineSession
    // ���ܣ�    �ܾ������ļ���������
    /********************************************************/
    LIBJINGLE_API ELibjingleError  DeclineSession( HSession hSession )
    {
        if ( 0 == hSession )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        FileShareClient * pFileShareClient = (FileShareClient *)hSession;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistFileShareSession(pFileShareClient) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pFileShareClient->m_hApp;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        pAppObj->m_pXmppThread->DeclineFiles( pFileShareClient->m_pSession );

        return LIBJINGLE_ERR_OK;
    }



    /********************************************************/
    // �������ƣ�CancelSession
    // ���ܣ�    �ж��ļ�����
    /********************************************************/
    LIBJINGLE_API ELibjingleError  CancelSession( HSession hSession )
    {
        if ( 0 == hSession )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        FileShareClient * pFileShareClient = (FileShareClient *)hSession;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistFileShareSession(pFileShareClient) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)pFileShareClient->m_hApp;

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        pAppObj->m_pXmppThread->CancelFiles( pFileShareClient );

        return LIBJINGLE_ERR_OK;
    }


    /********************************************************/
    // �������ƣ�SetFileSavePath
    // ���ܣ�    �����ļ�����ı���·��
    /********************************************************/
    LIBJINGLE_API ELibjingleError  SetFileSavePath( HANDLE hApp, const char * szFilePath )
    {
        if ( 0 == hApp || 0 == szFilePath )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *   pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        cFuncLock cLock( &pAppObj->m_lock );


        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        strncpy( pAppObj->m_szSaveFileDir, szFilePath, MAX_PATH_SIZE - 1 );


        return LIBJINGLE_ERR_OK;
    }


    /********************************************************/
    // �������ƣ�SetFileShareStreamBitRate
    // ���ܣ�    �����ļ�����ı���·��
    /********************************************************/
    LIBJINGLE_API void SetFileShareStreamBitRate( unsigned long dwKbps )
    {
        CGlobalData * pGlobal = CGlobalData::GetInstance();

        // dwKbpsΪ0ʱ����Ϊ����������
        if ( 0 == dwKbps )
        {
            pGlobal->m_dwMaxTotalRate = 0;
        }
        else
        {
            // ת��ΪB/s��λ
            unsigned long dwBps = dwKbps * 1024;

            // ��Сֵֻ��Ϊ (MIN_BYTE_RATE)�ֽ�/s
            if ( dwBps < MIN_BYTE_RATE  )
            {
                dwBps = MIN_BYTE_RATE;
            }

            pGlobal->m_dwMaxTotalRate = dwBps;
        }
    }




    LIBJINGLE_API unsigned long GetDirSize( const char * directory)
    {
        unsigned int total = 0;
        talk_base::DirectoryIterator iter;
        talk_base::Pathname path;
        path.AppendFolder(directory);
        iter.Iterate(path.pathname());
        while (iter.Next())  {
            if (iter.Name() == "." || iter.Name() == "..")
                continue;
            if (iter.IsDirectory()) {
                path.AppendPathname(iter.Name());
                total += GetDirSize(path.pathname().c_str());
            }
            else
                total += iter.FileSize();
        }
        return total;
    }



    LIBJINGLE_API unsigned long GetFileSize( const char * szFile )
    {
        size_t size = 0;

        talk_base::Filesystem::GetFileSize( std::string(szFile), &size );

        return size;
    }



    LIBJINGLE_API CMsgData::CMsgData( const unsigned char * pbyData /* = 0 */, unsigned long dwDataLen /* = 0 */ )
    {
        memset( m_abyData, 0, sizeof(m_abyData) );
        m_dwDataLen     = 0;
        m_dwRPos        = 0;

        if ( 0 == pbyData || 0 == dwDataLen)
        {
            return;
        }

        if ( dwDataLen > MAX_MESSAGE_DATA_LENGTH )
        {
            return;
        }

        memcpy( m_abyData, pbyData, dwDataLen );
        m_dwDataLen = dwDataLen;
    }

    LIBJINGLE_API BOOL CMsgData::Append( const unsigned char * pbyData, unsigned long dwDataLen )
    {
        if ( 0 == dwDataLen )
        {
            return TRUE;
        }

        if ( 0 == pbyData )
        {
            return FALSE;
        }

        if ( m_dwDataLen + dwDataLen > MAX_MESSAGE_DATA_LENGTH )
        {
            return FALSE;
        }

        memcpy( m_abyData + m_dwDataLen, pbyData, dwDataLen );
        m_dwDataLen += dwDataLen;

        return TRUE;
    }

    LIBJINGLE_API void CMsgData::Clear()
    {
        m_dwDataLen = 0;
        m_dwRPos    = 0;
    }

    LIBJINGLE_API BOOL CMsgData::Read( unsigned char * pbyData, unsigned long dwDataSize /* = -1 */ )
    {
        if ( 0 == dwDataSize )
        {
            return TRUE;
        }

        if ( 0 == pbyData )
        {
            return FALSE;
        }

        // ��ȡʣ�����������
        if ( -1 == dwDataSize )
        {
            // �Ѿ�����ĩ
            if ( m_dwRPos == m_dwDataLen )
            {
                return TRUE;
            }

            memcpy( pbyData, m_abyData + m_dwRPos, m_dwDataLen - m_dwRPos );
            m_dwRPos = m_dwDataLen;
            return TRUE;
        }
        else
        {
            if ( m_dwRPos + dwDataSize > m_dwDataLen )
            {
                return FALSE;
            }

            memcpy( pbyData, m_abyData + m_dwRPos, dwDataSize );
            m_dwRPos += dwDataSize;
        }
        return TRUE;
    }

    LIBJINGLE_API void CMsgData::ResetReadPos()
    {
        m_dwRPos = 0;
    }

    LIBJINGLE_API unsigned long  CMsgData::GetMsgLength() const
    {
        return m_dwDataLen;
    }

    LIBJINGLE_API const unsigned char * CMsgData::GetMsgData() const
    {
        return m_abyData;
    }

    LIBJINGLE_API unsigned long   CMsgData::GetReadPos() const
    {
        return m_dwRPos;
    }


    // ��ptJidΪ0ʱ����ȡ�Լ���vcard������Ϊ0ʱ����ȡ�����˵�vcard
    LIBJINGLE_API   ELibjingleError  GetVCard( HANDLE hApp, const TJid * ptJid )
    {
        if ( 0 == hApp )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        // �������߳�������
        pAppObj->m_pXmppThread->GetVCard( ptJid );


        return LIBJINGLE_ERR_OK;
    }

    // �����Լ���vcard
    LIBJINGLE_API   ELibjingleError  SetVCard( HANDLE hApp, const TVCard * ptVCard )
    {
        if ( 0 == hApp || 0 == ptVCard )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;


        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        buzz::XmppClient * client = pAppObj->m_pXmppThread->client();

        char buf[MAX_BUFF_SIZE];

        // ����stanza
        buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

        // iq����type����
        iq->AddAttr( buzz::QN_TYPE, buzz::STR_SET );

        // iq����id����
        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        iq->AddAttr( buzz::QN_ID, randid );

        buzz::XmlElement * vcard = new buzz::XmlElement( buzz::QN_VCARD, true );
        iq->AddElement( vcard );

        buzz::XmlElement * nick = new buzz::XmlElement( buzz::QN_VCARD_FN, true );
        vcard->AddElement( nick );
        nick->SetBodyText( AnsiToUtf8( ptVCard->m_achNickName, buf, MAX_BUFF_SIZE  ) );

        buzz::XmlElement * categories = new buzz::XmlElement( buzz::QN_VCARD_CATEGORIES, true );
        vcard->AddElement( categories );

        buzz::XmlElement * use_custom_head_portrait = new buzz::XmlElement( buzz::QN_VCARD_CATEGORIES_KEYWORD, true );
        categories->AddElement( use_custom_head_portrait );
        use_custom_head_portrait->SetAttr( buzz::QN_NAME, VCARD_KEYWORD_NAME_CUSTOM_PORTRAIT_FLAG );
        if ( ptVCard->m_bCustomHeadPortrait )
        {
            use_custom_head_portrait->SetBodyText( "true" );
        }
        else
        {
            use_custom_head_portrait->SetBodyText( "false" );
        }

        buzz::XmlElement * headportrait_index = new buzz::XmlElement( buzz::QN_VCARD_CATEGORIES_KEYWORD, true );
        categories->AddElement( headportrait_index );
        headportrait_index->SetAttr( buzz::QN_NAME, VCARD_KEYWORD_NAME_SYS_PORTRAIT_INDEX );
        sprintf( buf, "%d", ptVCard->m_nSysHeadPortraitIndex );
        headportrait_index->SetBodyText( buf );

        // ���ʹ���Զ����ͷ��
        if ( ptVCard->m_bCustomHeadPortrait )
        {
            buzz::XmlElement * photo = new buzz::XmlElement( buzz::QN_VCARD_PHOTO );
            vcard->AddElement( photo );

            buzz::XmlElement * image_type = new buzz::XmlElement( buzz::QN_VCARD_PHOTO_TYPE );
            photo->AddElement( image_type );

            buzz::XmlElement * image_data = new buzz::XmlElement( buzz::QN_VCARD_PHOTO_BINVAL );
            photo->AddElement( image_data );

            const char * szImageType[LibJingleEx::IMAGE_TYPE_UNKNOWN+1] = { IMAGE_TYPE_BITMAP_STR,
                IMAGE_TYPE_JPG_STR, IMAGE_TYPE_GIF_STR,IMAGE_TYPE_PNG_STR,IMAGE_TYPE_UNKNOWN_STR };

            image_type->SetBodyText( szImageType[ptVCard->m_eCustomHeadPortraitType] );

            char buf_tmp[VCARD_MAX_HEAD_PORTRAIT_SIZE * 2];
            unsigned long dwSize = VCARD_MAX_HEAD_PORTRAIT_SIZE * 2;
            int nRet = EncodeBase64( buf_tmp, dwSize, ptVCard->m_abyCumtomHeadPortrait, ptVCard->m_dwCustomHeadPortraitSize );
            assert( 0 == nRet );

            image_data->SetBodyText( buf_tmp );
        }

        // client->SendStanza( iq );
        // delete iq;
        pAppObj->m_pXmppThread->SendStanza( iq );
        // END ���� 

        return LIBJINGLE_ERR_OK;
    }


    LIBJINGLE_API   ELibjingleError  SetPassword( HANDLE hApp, const char * szPassword )
    {
        if ( 0 == hApp  )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        buzz::XmppClient * client = pAppObj->m_pXmppThread->client();

        LibJingleEx::TJid tJid;
        LibJingleEx::StrJid2TJid( client->jid().Str().c_str(), tJid );


        char buf[MAX_BUFF_SIZE];

        // ����stanza
        buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

        // iq����type����
        iq->AddAttr( buzz::QN_TYPE, buzz::STR_SET );

        // iq����id����
        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        iq->AddAttr( buzz::QN_ID, randid );

        // iq����to����
        iq->AddAttr( buzz::QN_TO, AnsiToUtf8( tJid.sDomain, buf, MAX_BUFF_SIZE ) ); 


        buzz::XmlElement * query = new buzz::XmlElement( buzz::QN_REGISTER_QUERY, true );
        iq->AddElement( query );

        buzz::XmlElement * username = new buzz::XmlElement( buzz::QN_USER_NAME, true );
        query->AddElement( username );

        buzz::XmlElement * password = new buzz::XmlElement( buzz::QN_PASSWORD, true );
        query->AddElement( password );

        username->SetBodyText( AnsiToUtf8( tJid.sNode, buf, MAX_BUFF_SIZE ) );

        if ( 0 == szPassword )
        {
            szPassword = "";
        }

        password->SetBodyText( AnsiToUtf8( szPassword, buf, MAX_BUFF_SIZE ) );

        // client->SendStanza( iq );
        // delete iq;
        pAppObj->m_pXmppThread->SendStanza( iq );
        // END ���� 

        return LIBJINGLE_ERR_OK;
    }























#if  DMS_CONTACTS_SWITCH

    LIBJINGLE_API  ELibjingleError  DmsAddGroup( HANDLE hApp, PTDmsGroup    ptDmsGroup )
    {
        if ( 0 == hApp ||  0 == ptDmsGroup )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        vDmsGroup & v = pAppObj->m_vDmsGroups;
        vDmsGroup::iterator it;


        // ����и�����
        if ( 0 != ptDmsGroup->dwParentId )
        {
            // ��ѯ���޸ø�����
            it = find_if( v.begin(), v.end(), FindDmsGroupObj( ptDmsGroup->dwParentId ) );
            // û�иø�����
            if ( it == v.end() )
            {
                return LIBJINGLE_ERR_DMS_NO_SUCH_PARENT_GROUP;
            }

            // ��ֹ��δ���顱�������
            if ( ptDmsGroup->dwParentId == SPECIAL_GROUP_SN )
            {
                return LIBJINGLE_ERR_DMS_FORBID_ADD_SUBGROUP;
            }
        }


        // �鿴���޴ﵽÿ��������ɵ��������
        unsigned long dwSiblingsCnt = 0;
        for ( it = v.begin();  it != v.end();  it++ )
        {
            LibJingleEx::PTDmsGroup  p = *it;
            // �����ͬһ�������
            if ( p->dwParentId == ptDmsGroup->dwParentId )
            {
                dwSiblingsCnt++;
            }
        }

        if ( dwSiblingsCnt >= MAX_DMS_SUB_GROUPS_IN_A_GROUP )
        {
            return LIBJINGLE_ERR_DMS_REACH_MAX_SUB_GROUP_COUNT;
        }
        // END �鿴���޴ﵽÿ��������ɵ��������



        pAppObj->m_dwDmsMaxGroupId++;

        PTDmsGroup pNewGroup = new TDmsGroup;
        memcpy( pNewGroup, ptDmsGroup, sizeof(TDmsGroup) );
        pNewGroup->dwGroupId = pAppObj->m_dwDmsMaxGroupId;
        v.push_back( pNewGroup );

        ptDmsGroup->dwGroupId = pNewGroup->dwGroupId;

        // ���͸�����������
        SaveDmsGroup2Server(pAppObj);


        char buf[MAX_BUFF_SIZE];

        // ͬ��dms��������ͬresource���Լ�
        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
        message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
        message->SetAttr( buzz::QN_TYPE, "dms" );

        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );
        operation->SetAttr( buzz::QN_TYPE, "add group" );

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
        operation->AddElement( item );

        char szId[256];
        char szParentId[256];

        sprintf( szId,         "%u", pNewGroup->dwGroupId );
        sprintf( szParentId,   "%u", pNewGroup->dwParentId );

        item->SetAttr( buzz::QN_DMS_ID, szId  );
        item->SetAttr( buzz::QN_DMS_PARENT_ID, szParentId );
        item->SetAttr( buzz::QN_DMS_NAME, AnsiToUtf8( pNewGroup->szGroupName, buf, MAX_BUFF_SIZE ) );

        // client->SendStanza( message );
        // delete message;
        pAppObj->m_pXmppThread->SendStanza( message );
        // ����


        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API  ELibjingleError  DmsDelGroup( HANDLE hApp, unsigned long dwGroupId, BOOL bMemberReserverd /*= FALSE*/ )
    {
        if ( 0 == hApp ||  0 == dwGroupId )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        vDmsGroup & v = pAppObj->m_vDmsGroups;
        vDmsGroup::iterator it, it_find;

        // ��ѯ���޸���
        it_find = find_if( v.begin(), v.end(), FindDmsGroupObj( dwGroupId ) );
        // û�и���
        if ( it_find == v.end() )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_GROUP;
        }

        // �鿴�����Ƿ�������
        for ( it = v.begin(); it != v.end(); it++ )
        {
            PTDmsGroup  p = *it;

            // ������
            if ( p->dwParentId == dwGroupId )
            {
                return LIBJINGLE_ERR_DMS_HAS_SUB_GROUP;
            }
        }

        // ɾ�������µ�������ϵ��(�򱣴浽��δ���顱)
        unsigned long i;
        // ˽�п�λ����
        unsigned long dwMemberBlockBits = 0;

        // ��δ���顱��Ӧ��Roster�е�����
        char szDstGroup[DMS_MAX_REAL_GROUP_NAME_LEN];
        SNPRINTF( szDstGroup, DMS_MAX_REAL_GROUP_NAME_LEN, "%u", SPECIAL_GROUP_SN );

        // ��Ӧ��׼Roster��������
        char szSrcGroup[DMS_MAX_REAL_GROUP_NAME_LEN];
        SNPRINTF( szSrcGroup, DMS_MAX_REAL_GROUP_NAME_LEN, "%u", dwGroupId );

        // �����Ҫ�����浽δ���飬����ʱ���������µĳ�Ա��ȷ���Ƿ���Ҫunsubscribe����
        vDmsMember   vTmp;

        for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
        {
            vDmsMember & vMembers = pAppObj->m_vDmsMembers[i];
            vDmsMember::iterator it_m;

            for ( it_m = vMembers.begin(); it_m != vMembers.end(); )
            {
                PTDmsMember pMember = *it_m;

                // �ҵ����Ա
                if ( pMember->dwGroupId == dwGroupId )
                {
                    // ����λ����
                    SetBit( dwMemberBlockBits, i );

                    TJid tJid;
                    StrJid2TJid( pMember->szJid, tJid );

                    // �����Ҫ�������Ա��δ����
                    if ( bMemberReserverd )
                    {
                        // ����ڡ�δ���顱���ҵ��ó�Ա
                        if ( pAppObj->IfExistDmsMemberInTheGroup( SPECIAL_GROUP_SN, pMember->szJid ) )
                        {
                            it_m = vMembers.erase( it_m );
                            delete pMember;
                        }
                        else
                        {
                            pMember->dwGroupId = SPECIAL_GROUP_SN;
                            it_m++;
                        }
                    }
                    // �������Ҫ�������Ա
                    else
                    {
                        // �����jid���ͣ���Ҫ֮��ȷ���Ƿ�unsubscribe��
                        if ( DMS_MEMBER_TYPE_JID == pMember->eType )
                        {
                            vTmp.push_back( pMember );
                        }
                        else
                        {
                            delete pMember;
                        }

                        it_m = vMembers.erase( it_m );
                    }
                }
                else
                {
                    it_m++;
                }
            }
        }


        // ɾ������
        delete *it_find;
        v.erase( it_find );

        // ���͸�����������
        SaveDmsGroup2Server(pAppObj);
        SaveDmsMember2Server( pAppObj, dwMemberBlockBits );


        vDmsMember::iterator it_m;
        for ( it_m = vTmp.begin(); it_m != vTmp.end(); it_m++ )
        {
            PTDmsMember pMember = *it_m;

            // ����Ѿ������ڸó�Ա�ˣ���ȡ����ע��
            if ( !pAppObj->IfExistDmsMember(pMember->szJid) )
            {
                LibJingleEx::TJid   tJid;
                LibJingleEx::StrJid2TJid( pMember->szJid, tJid );

                Unsubscribe( hApp, &tJid );
            }

            delete pMember;
        }
        vTmp.clear();


        char buf[MAX_BUFF_SIZE];
        // ͬ��dms��������ͬresource���Լ�
        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
        message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
        message->SetAttr( buzz::QN_TYPE, "dms" );

        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );

        if ( bMemberReserverd )
        {
            operation->SetAttr( buzz::QN_TYPE, "delete group members reserved" );
        }
        else
        {
            operation->SetAttr( buzz::QN_TYPE, "delete group" );
        }

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
        operation->AddElement( item );

        char szId[256];

        sprintf( szId,         "%u", dwGroupId );

        item->SetAttr( buzz::QN_DMS_ID, szId  );

        // client->SendStanza( message );
        // delete message;
        pAppObj->m_pXmppThread->SendStanza( message );
        // ����

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API  ELibjingleError  DmsModifyGroup( HANDLE hApp, PTDmsGroup    ptDmsGroup )
    {
        if ( 0 == hApp ||  0 == ptDmsGroup )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        vDmsGroup & v = pAppObj->m_vDmsGroups;
        vDmsGroup::iterator it_find;

        // ��ѯ���޸���
        it_find = find_if( v.begin(), v.end(), FindDmsGroupObj( ptDmsGroup->dwGroupId ) );
        // û�и���
        if ( it_find == v.end() )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_GROUP;
        }

        PTDmsGroup p = *it_find;

        BOOL bChanged = FALSE;

        if ( 0 != strcmp( p->szGroupName, ptDmsGroup->szGroupName ) )
        {
            bChanged = TRUE;
        }

        if ( !bChanged )
        {
            return LibJingleEx::LIBJINGLE_ERR_OK;
        }

        strcpy( p->szGroupName, ptDmsGroup->szGroupName );
        // ���͸�����������
        SaveDmsGroup2Server(pAppObj);

        char buf[MAX_BUFF_SIZE];
        // ͬ��dms��������ͬresource���Լ�
        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
        message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
        message->SetAttr( buzz::QN_TYPE, "dms" );

        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );
        operation->SetAttr( buzz::QN_TYPE, "modify group" );

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
        operation->AddElement( item );

        char szId[256];
        sprintf( szId,         "%u", ptDmsGroup->dwGroupId );

        item->SetAttr( buzz::QN_DMS_ID,   szId  );
        item->SetAttr( buzz::QN_DMS_NAME, AnsiToUtf8( ptDmsGroup->szGroupName, buf, MAX_BUFF_SIZE ) );

        // client->SendStanza( message );
        // delete message;
        pAppObj->m_pXmppThread->SendStanza( message );
        // ����

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API  ELibjingleError  DmsMoveGroup  ( HANDLE hApp, unsigned long dwGroupId, 
        unsigned long dwNewParentGroupId )
    {
        if ( 0 == hApp ||  0 == dwGroupId )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        if ( dwGroupId == dwNewParentGroupId )
        {
            return LIBJINGLE_ERR_DMS_MOVE_METHOD_FORBIDDEN;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        vDmsGroup & v = pAppObj->m_vDmsGroups;
        vDmsGroup::iterator it_find, it_new;

        // ��ѯ���޸���
        it_find = find_if( v.begin(), v.end(), FindDmsGroupObj( dwGroupId ) );
        // û�и���
        if ( it_find == v.end() )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_GROUP;
        }

        PTDmsGroup p = *it_find;

        // �����ƶ�
        if ( p->dwParentId == dwNewParentGroupId )
        {
            return LibJingleEx::LIBJINGLE_ERR_OK;
        }

        if ( dwNewParentGroupId > 0 )
        {
            it_new = find_if( v.begin(), v.end(), FindDmsGroupObj(dwNewParentGroupId) );
            if ( v.end() == it_new )
            {
                return LibJingleEx::LIBJINGLE_ERR_DMS_NO_SUCH_GROUP;
            }

            // �鿴�ƶ�������µĸ��������޸��ӹ�ϵ
            // ���ƶ��������µĸ�����ĸ��ڵ���游�ڵ�ʱ����ֹ�����ƶ�
            PTDmsGroup pNewParentGroup = *it_new;
            while ( TRUE )
            {
                if ( pNewParentGroup->dwParentId == p->dwGroupId )
                {
                    return LIBJINGLE_ERR_DMS_MOVE_METHOD_FORBIDDEN;       
                }

                if ( pNewParentGroup->dwParentId > 0  )
                {
                    it_new = find_if( v.begin(), v.end(), FindDmsGroupObj(pNewParentGroup->dwParentId) );
                    assert( v.end() != it_new );
                    pNewParentGroup = *it_new;
                }
                else
                {
                    break;
                }
            }
        }

        p->dwParentId = dwNewParentGroupId;

        // ���͸�����������
        SaveDmsGroup2Server(pAppObj);

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API  ELibjingleError  DmsQuerySubGroups( HANDLE hApp, unsigned long dwParentGroupId, 
        PTDmsGroup    ptDmsGroups,
        unsigned long * pdwGroupsSize )
    {
        if ( 0 == hApp ||  0 == ptDmsGroups || 0 == pdwGroupsSize )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        vDmsGroup & v = pAppObj->m_vDmsGroups;
        vDmsGroup::iterator it;

        unsigned long i = 0;
        for ( it = v.begin(), i = 0; it != v.end(); it++ )
        {
            PTDmsGroup p = *it;

            // �ҵ�һ��
            if ( p->dwParentId == dwParentGroupId )
            {
                // ������㹻���� 
                if ( i < *pdwGroupsSize )
                {
                    memcpy( &ptDmsGroups[i], p, sizeof(TDmsGroup) );
                    i++;
                }
                else
                {
                    return LibJingleEx::LIBJINGLE_ERR_DMS_NOT_ENOUGH_BUFF;
                }
            }
        }

        *pdwGroupsSize = i;

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }


    LIBJINGLE_API  ELibjingleError  DmsQueryGroupDepth( HANDLE hApp, unsigned long dwGroupId, 
        unsigned long * pdwGroupsDepth )
    {
        if ( 0 == hApp ||  0 == dwGroupId || 0 == pdwGroupsDepth )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        vDmsGroup & v = pAppObj->m_vDmsGroups;
        vDmsGroup::iterator it_find;

        it_find = find_if( v.begin(), v.end(), FindDmsGroupObj(dwGroupId) );
        // ���鲻����
        if ( v.end() == it_find )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_GROUP;
        }

        LibJingleEx::PTDmsGroup p = *it_find;    
        unsigned long dwDepth = 1;            // ��ȴ�1��ʼ

        while( p->dwParentId > 0  )
        {
            vDmsGroup::iterator it_parent = find_if( v.begin(), v.end(), FindDmsGroupObj( p->dwParentId ) );
            assert( v.end() != it_find );

            p = *it_parent;
            dwDepth++;
        }

        *pdwGroupsDepth = dwDepth;

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }



    LIBJINGLE_API  ELibjingleError  DmsAddMember( HANDLE hApp, PTDmsMember     ptDmsMemberSrc )
    {
        if ( 0 == hApp ||  0 == ptDmsMemberSrc || 0 == ptDmsMemberSrc->dwGroupId )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ��û�л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }


        TDmsMember   tmpDmsMember;
        memcpy( &tmpDmsMember,  ptDmsMemberSrc,  sizeof(TDmsMember) );
        _Str2Lower( tmpDmsMember.szJid );
        PTDmsMember     ptDmsMember  =  &tmpDmsMember;



        vDmsGroup & vGroups = pAppObj->m_vDmsGroups;
        vDmsGroup::iterator it_group;

        // ��ѯ������������
        it_group = find_if( vGroups.begin(), vGroups.end(), FindDmsGroupObj( ptDmsMember->dwGroupId ) );
        // û�и���
        if ( it_group == vGroups.end() )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_PARENT_GROUP;
        }

        unsigned long  i;
        // ��Ա����
        unsigned long  dwTotalsCnt = 0;
        // ͬһ���ڳ�Ա����
        unsigned long  dwSiblingsCnt = 0;

        // �Ƿ��Ѿ��������Ա�ˣ�����У�����������subscribe�ˣ�
        BOOL  bExistSuchMember = FALSE;

        for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
        {
            vDmsMember & vMembers = pAppObj->m_vDmsMembers[i];
            vDmsMember::iterator it_member;

            dwTotalsCnt += vMembers.size();    

            for ( it_member = vMembers.begin(); it_member != vMembers.end(); it_member++ )
            {
                PTDmsMember p = *it_member;

                // �����ͬһ���ڵĳ�Ա
                if ( p->dwGroupId == ptDmsMember->dwGroupId )
                {
                    dwSiblingsCnt++;

                    // ���ͬһ�����Ѿ��иó�Ա��
                    if (  0 == strcmp( p->szJid, ptDmsMember->szJid )  )
                    {
                        bExistSuchMember = TRUE;
                        return LIBJINGLE_ERR_DMS_FRIEND_ALREADY_IN_THE_GROUP;
                    }
                }
                // ����ͬһ���ڳ�Ա������jid��ͬ
                else if (  0 == strcmp( p->szJid, ptDmsMember->szJid )  )
                {
                    bExistSuchMember = TRUE;
                }
            }
        }

        // ������еĳ�Ա�Ƿ�ﵽ�����Ŀ
        if ( dwTotalsCnt >= MAX_DMS_TOTAL_MEMBERS_COUNT )
        {
            return LIBJINGLE_ERR_DMS_REACH_MAX_TOTOL_MEMBERS_COUNT;
        }
        // END ������еĳ�Ա�Ƿ�ﵽ�����Ŀ


        // ������ǡ�δ���顱���ͼ�����ڵĳ�Ա����(δ����ĳ�Ա���������)
        //if (  SPECIAL_GROUP_SN != ptDmsMember->dwGroupId && dwSiblingsCnt >= MAX_DMS_MEMBERS_IN_A_GROUP )
        //{
        //    return LIBJINGLE_ERR_DMS_REACH_MAX_SUB_MEMBER_COUNT;
        //}
        // END ��鸸����ӵ�еĳ�Ա�Ƿ�ﵽ���ֵ


        // ����Ƿ�����Լ�(�ܾ�����Լ�Ϊ����)
        if ( DMS_MEMBER_TYPE_JID == ptDmsMember->eType )
        {
            LibJingleEx::TJid    tSelfJid;
            LibJingleEx::StrJid2TJid( client->jid().BareJid().Str().c_str(), tSelfJid );

            LibJingleEx::TJid    tMemberJid;
            LibJingleEx::StrJid2TJid( ptDmsMember->szJid,  tMemberJid );

            if ( IfJidEqual( tSelfJid, tMemberJid) )
            {
                return LIBJINGLE_ERR_DMS_CANNOT_ADD_SELF;
            }
        }
        // END ����Ƿ�����Լ�(�ܾ�����Լ�Ϊ����)


        unsigned long dwBlockBits = 0;

        pAppObj->m_dwDmsMaxMemberId++;

        PTDmsMember ptNewMember = new TDmsMember;
        memcpy( ptNewMember, ptDmsMember, sizeof(TDmsMember) );
        ptNewMember->dwMemberId = pAppObj->m_dwDmsMaxMemberId;
        // vMembers.push_back( ptNewMember );
        // �ĸ��������˸��³�Ա
        unsigned long dwBlockIndex = pAppObj->AddDmsMember2Block( ptNewMember, dwBlockBits );
        ptDmsMember->dwMemberId = ptNewMember->dwMemberId;
        ptDmsMemberSrc->dwMemberId = ptDmsMember->dwMemberId;


        // ���͸�����������
        SaveDmsMember2Server( pAppObj, dwBlockBits );


        // �����jid���ͣ�������Ҫ��ע����
        if ( DMS_MEMBER_TYPE_JID == ptNewMember->eType )
        {
            // �����û��������ѣ���ע��
            if ( !bExistSuchMember )
            {
                TJid tJid;
                StrJid2TJid( ptDmsMember->szJid, tJid );

                Subscribe( hApp, &tJid );
            }
        }


        char buf[MAX_BUFF_SIZE];
        // ͬ��dms��������ͬresource���Լ�
        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
        message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
        message->SetAttr( buzz::QN_TYPE, "dms" );

        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );
        operation->SetAttr( buzz::QN_TYPE, "add member" );

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
        operation->AddElement( item );

        char szId[256];
        char szParentId[256];
        char szType[256];

        sprintf( szId,         "%u", ptDmsMember->dwMemberId );
        sprintf( szParentId,   "%u", ptDmsMember->dwGroupId );
        sprintf( szType,       "%u", ptDmsMember->eType );

        item->SetAttr( buzz::QN_DMS_ID,          szId  );
        item->SetAttr( buzz::QN_DMS_PARENT_ID,   szParentId  );
        item->SetAttr( buzz::QN_DMS_NAME,        AnsiToUtf8( ptDmsMember->szName, buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_DMS_JID,         AnsiToUtf8( ptDmsMember->szJid, buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_DMS_TYPE,        szType );

        item->SetAttr( buzz::QN_DMS_RESERVERD1,  AnsiToUtf8( ptDmsMember->szReserved[0], buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_DMS_RESERVERD2,  AnsiToUtf8( ptDmsMember->szReserved[1], buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_DMS_RESERVERD3,  AnsiToUtf8( ptDmsMember->szReserved[2], buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_DMS_RESERVERD4,  AnsiToUtf8( ptDmsMember->szReserved[3], buf, MAX_BUFF_SIZE ) );

        // client->SendStanza( message );
        // delete message;
        pAppObj->m_pXmppThread->SendStanza( message );
        // ����


        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API  ELibjingleError  DmsDelMember( HANDLE hApp, unsigned long   dwMemberId )
    {
        if ( 0 == hApp ||  0 == dwMemberId )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ��û�л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        PTDmsMember ptDmsMember = pAppObj->GetDmsMember( dwMemberId );
        if ( 0 == ptDmsMember )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_FRIEND;
        }

        // �ȱ������� 
        TDmsMember  tReserverd;
        memcpy( &tReserverd, ptDmsMember, sizeof(TDmsMember) );

        // ��Dms˽��������ɾ��
        unsigned long dwBlockIndex = pAppObj->DeleteDmsMember( dwMemberId );

        // ���͸�����������
        unsigned long dwBlockBits = 0;
        SetBit( dwBlockBits, dwBlockIndex );
        SaveDmsMember2Server( pAppObj, dwBlockBits );


        // �����jid���ͣ���Rosterɾ������
        if ( DMS_MEMBER_TYPE_JID == tReserverd.eType )
        {
            // ��ȫû����������ˣ���ȡ����ע
            if ( !pAppObj->IfExistDmsMember(tReserverd.szJid) )
            {
                TJid  tJid;
                StrJid2TJid( tReserverd.szJid, tJid );

                Unsubscribe( hApp, &tJid );
            }
        }


        char buf[MAX_BUFF_SIZE];
        // ͬ��dms��������ͬresource���Լ�
        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
        message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
        message->SetAttr( buzz::QN_TYPE, "dms" );

        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );
        operation->SetAttr( buzz::QN_TYPE, "delete member" );

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
        operation->AddElement( item );

        char szId[256];
        sprintf( szId,         "%u", dwMemberId );

        item->SetAttr( buzz::QN_DMS_ID,          szId  );

        // client->SendStanza( message );
        // delete message;
        pAppObj->m_pXmppThread->SendStanza( message );
        // ����

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    // ���� ptDmsMember->dwGroupId �� ptDmsMember->jid
    LIBJINGLE_API  ELibjingleError  DmsModifyMember( HANDLE hApp, PTDmsMember  ptDmsMember )
    {
        if ( 0 == hApp ||  0 == ptDmsMember || 0 == ptDmsMember->dwMemberId )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ��û�л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        PTDmsMember pFind = pAppObj->GetDmsMember( ptDmsMember->dwMemberId );
        if ( 0 == pFind )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_FRIEND;
        }

        TDmsMember  tTmp;
        memcpy( &tTmp, ptDmsMember, sizeof(TDmsMember) );
        strcpy( tTmp.szJid, pFind->szJid );

        // ����ͬ��jid�ĳ�Ա���޸�
        unsigned long dwBlockbits = pAppObj->ModifyDmsMemberByJid( &tTmp );

        // ���͸�����������
        SaveDmsMember2Server( pAppObj, dwBlockbits );

        char buf[MAX_BUFF_SIZE];
        // ͬ��dms��������ͬresource���Լ�
        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
        message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
        message->SetAttr( buzz::QN_TYPE, "dms" );

        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );
        operation->SetAttr( buzz::QN_TYPE, "modify member" );

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
        operation->AddElement( item );

        char szId[256];
        sprintf( szId,         "%u", ptDmsMember->dwMemberId );

        item->SetAttr( buzz::QN_DMS_ID,          szId  );
        item->SetAttr( buzz::QN_DMS_NAME,        AnsiToUtf8( ptDmsMember->szName, buf, MAX_BUFF_SIZE )  );

        item->SetAttr( buzz::QN_DMS_RESERVERD1,  AnsiToUtf8( ptDmsMember->szReserved[0], buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_DMS_RESERVERD2,  AnsiToUtf8( ptDmsMember->szReserved[1], buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_DMS_RESERVERD3,  AnsiToUtf8( ptDmsMember->szReserved[2], buf, MAX_BUFF_SIZE ) );
        item->SetAttr( buzz::QN_DMS_RESERVERD4,  AnsiToUtf8( ptDmsMember->szReserved[3], buf, MAX_BUFF_SIZE ) );

        // client->SendStanza( message );
        // delete message;
        pAppObj->m_pXmppThread->SendStanza( message );
        // ����

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API  ELibjingleError  DmsMoveMember  ( HANDLE hApp, unsigned long dwMemberId, 
        unsigned long dwNewGroupId )
    {
        if ( 0 == hApp ||  0 == dwMemberId || 0 == dwNewGroupId )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ��û�л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        unsigned long dwIndex = 0;
        PTDmsMember p = pAppObj->GetDmsMember( dwMemberId, &dwIndex );
        // ���û�иó�Ա
        if ( 0 == p )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_FRIEND;
        }

        // �������Ҫ�ƶ�
        if ( p->dwGroupId == dwNewGroupId )
        {
            return LIBJINGLE_ERR_OK;
        }

        vDmsGroup & vGroup = pAppObj->m_vDmsGroups;
        vDmsGroup::iterator it_group;

        // �鿴Ŀ�����Ƿ����
        it_group = find_if( vGroup.begin(), vGroup.end(), FindDmsGroupObj(dwNewGroupId) );
        // ���Ŀ���鲻����
        if ( vGroup.end() == it_group )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_GROUP;
        }

        // �鿴Ŀ�����Ƿ��Ѿ�����ͬ��jid���û�
        // ���Ŀ�������Ѿ���ͬ��jid�Ĵ���
        if ( pAppObj->IfExistDmsMemberInTheGroup(dwNewGroupId, p->szJid) )
        {
            return LIBJINGLE_ERR_DMS_FRIEND_ALREADY_IN_THE_GROUP;
        }

        unsigned long dwOldGroupId = p->dwGroupId;
        p->dwGroupId = dwNewGroupId;

        // ���͸�����������
        unsigned long dwBlockBits = 0;
        SetBit( dwBlockBits, dwIndex );
        SaveDmsMember2Server( pAppObj, dwBlockBits );

        // �����jid���ͣ��޸ı�׼xmpp roster
        if ( DMS_MEMBER_TYPE_JID == p->eType )
        {
            // �޸�ʵ�ʺ����б�
            TJid   tJid;
            StrJid2TJid( p->szJid, tJid );

            char szSrcGroupName[DMS_MAX_REAL_GROUP_NAME_LEN];
            char szDestGroupName[DMS_MAX_REAL_GROUP_NAME_LEN];

            sprintf( szSrcGroupName,  "%u", dwOldGroupId );
            sprintf( szDestGroupName, "%u", dwNewGroupId );

            MoveFriend( hApp, &tJid, szSrcGroupName, szDestGroupName );
        }

        char buf[MAX_BUFF_SIZE];
        // ͬ��dms��������ͬresource���Լ�
        buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
        message->SetAttr( buzz::QN_TO,   AnsiToUtf8(  client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
        message->SetAttr( buzz::QN_TYPE, "dms" );

        buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
        message->AddElement( operation );
        operation->SetAttr( buzz::QN_TYPE, "move member" );

        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
        operation->AddElement( item );

        char szId[256];
        char szNewGroupId[256];
        sprintf( szId,         "%u", p->dwMemberId );
        sprintf( szNewGroupId, "%u", p->dwGroupId );

        item->SetAttr( buzz::QN_DMS_ID,          szId  );
        item->SetAttr( buzz::QN_DMS_PARENT_ID,   szNewGroupId );

        // client->SendStanza( message );
        // delete message;
        pAppObj->m_pXmppThread->SendStanza( message );
        // ����

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    LIBJINGLE_API  ELibjingleError  DmsQueryMembers (  HANDLE hApp, unsigned long dwParentGroupId, 
        PTDmsMember     ptDmsMembers,
        unsigned long * pdwMembersSize,
        unsigned long * pdwStartIndex /*= 0*/,
        unsigned long * pdwEndIndex   /*= 0*/ )
    {
        if ( 0 == hApp ||  0 == dwParentGroupId || 0 == ptDmsMembers || 0 == pdwMembersSize )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        // ����ǰ����ٸ���Ա
        unsigned long dwStartIndex = 0;
        if ( 0 != pdwStartIndex )
        {
            dwStartIndex = *pdwStartIndex;
        }

        // ͬһ���µ�ǰ��Ա������ 
        unsigned long dwCurMemberIndex = 0;

        unsigned long i;
        for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
        {
            vDmsMember & v = pAppObj->m_vDmsMembers[i];
            vDmsMember::iterator it;

            // ����ÿ����
            for ( it = v.begin(); it != v.end(); it++ )
            {
                PTDmsMember p = *it;

                // �ҵ�һ������������
                if ( p->dwGroupId == dwParentGroupId )
                {
                    // �����û�дﵽ��Ҫ����������
                    if ( dwCurMemberIndex < dwStartIndex )
                    {

                    }
                    else
                    {
                        unsigned long k = dwCurMemberIndex - dwStartIndex;

                        // ������㹻����
                        if ( k < *pdwMembersSize )
                        {
                            memcpy( &ptDmsMembers[k], p, sizeof(TDmsMember) );
                        }
                        // ���û���㹻����
                        else
                        {
                            if ( 0 != pdwEndIndex )
                            {
                                *pdwEndIndex = dwCurMemberIndex;
                            }

                            return LibJingleEx::LIBJINGLE_ERR_DMS_NOT_ENOUGH_BUFF;
                        }
                    }

                    dwCurMemberIndex++;
                }
            }
        }

        if ( dwCurMemberIndex >= dwStartIndex )
        {
            *pdwMembersSize = dwCurMemberIndex - dwStartIndex;
        }
        else
        {
            *pdwMembersSize = 0;
        }


        return LibJingleEx::LIBJINGLE_ERR_OK;
    }


    LIBJINGLE_API ELibjingleError DmsGetPresenceByGroup( HANDLE hApp, TPresence * ptPresence,
        unsigned long * pdwSize, unsigned long dwGroupId, 
        unsigned long * pdwStartIndex /* = 0 */ )
    {
        if ( 0 == hApp ||  0 == ptPresence || 0 == pdwSize || 0 == dwGroupId )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        vDmsGroup::iterator it_group = find_if( pAppObj->m_vDmsGroups.begin(), pAppObj->m_vDmsGroups.end(),
            FindDmsGroupObj( dwGroupId )  );
        // û�и���
        if ( pAppObj->m_vDmsGroups.end() ==  it_group )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_GROUP;
        }


        vDmsMember   vTmp;
        unsigned long dwIndex = 0;

        for ( dwIndex = 0; dwIndex < DMS_MEMBERS_BLOCK_CNT; dwIndex++ )
        {
            vDmsMember & vMembers = pAppObj->m_vDmsMembers[dwIndex];
            vDmsMember::iterator it;

            for ( it = vMembers.begin(); it != vMembers.end(); it++ )
            {
                LibJingleEx::PTDmsMember p = *it;
                if ( p->dwGroupId == dwGroupId )
                {
                    vTmp.push_back( p );
                }
            }
        }


        if ( 0 == pdwStartIndex )
        {
            if ( *pdwSize < vTmp.size() )
            {
                return LibJingleEx::LIBJINGLE_ERR_NOT_ENOUGH_MEM;
            }

            unsigned long i;
            vDmsMember::iterator it;

            for ( it = vTmp.begin(), i = 0; it != vTmp.end(); it++, i++ )
            {
                LibJingleEx::PTDmsMember p = *it;

                // �����jid����
                if ( DMS_MEMBER_TYPE_JID == p->eType )
                {
                    LibJingleEx::StrJid2TJid( p->szJid,  ptPresence[i].tJid );
                    ptPresence[i].ePresence = LibJingleEx::GetFriendPresence( hApp, &ptPresence[i].tJid );
                }
                else
                {
                    ptPresence[i].ePresence = PRESENCE_OFFLINE;
                }
            }

            *pdwSize = i;
            return LibJingleEx::LIBJINGLE_ERR_OK;
        }
        else
        {
            unsigned long i;
            vDmsMember::iterator it;

            for ( i = 0, it = vTmp.begin() + *pdwStartIndex; (i < *pdwSize) && (i + *pdwStartIndex < vTmp.size()); i++, it++ )
            {
                LibJingleEx::PTDmsMember p = *it;

                // �����jid����
                if ( DMS_MEMBER_TYPE_JID == p->eType )
                {
                    LibJingleEx::StrJid2TJid( p->szJid,  ptPresence[i].tJid );
                    ptPresence[i].ePresence = LibJingleEx::GetFriendPresence( hApp, &ptPresence[i].tJid);
                }
                else
                {
                    ptPresence[i].ePresence = PRESENCE_OFFLINE;
                }
            }

            *pdwSize = i;

            *pdwStartIndex = *pdwStartIndex + i;

            if ( *pdwStartIndex >= vTmp.size() )
            {
                return LibJingleEx::LIBJINGLE_ERR_OK;
            }
            else
            {
                return LibJingleEx::LIBJINGLE_ERR_PARAM_BUFF_FULL;
            }
        }
    }

    LIBJINGLE_API ELibjingleError DmsGetPresenceByGroup( HANDLE hApp, 
        const char aszRes[][MAX_RESOURCE_LEN], unsigned long dwResCnt,
        PTDmsMemberState  ptMemberState,       unsigned long * pdwSize, 
        unsigned long   dwGroupId, 
        unsigned long * pdwStartIndex /* = 0 */ )
    {
        if ( 0 == hApp ||  0 == aszRes || 0 == dwResCnt || 0 == ptMemberState || 0 == pdwSize || 0 == dwGroupId
            || dwResCnt > MAX_DMS_MEMBER_STATE_CNT )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }


        vDmsGroup::iterator it_group = find_if( pAppObj->m_vDmsGroups.begin(), pAppObj->m_vDmsGroups.end(),
            FindDmsGroupObj( dwGroupId )  );
        // û�и���
        if ( pAppObj->m_vDmsGroups.end() ==  it_group )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_GROUP;
        }


        vDmsMember   vTmp;
        unsigned long dwIndex = 0;

        for ( dwIndex = 0; dwIndex < DMS_MEMBERS_BLOCK_CNT; dwIndex++ )
        {
            vDmsMember & vMembers = pAppObj->m_vDmsMembers[dwIndex];
            vDmsMember::iterator it;

            for ( it = vMembers.begin(); it != vMembers.end(); it++ )
            {
                LibJingleEx::PTDmsMember p = *it;
                if ( p->dwGroupId == dwGroupId )
                {
                    vTmp.push_back( p );
                }
            }
        }


        if ( 0 == pdwStartIndex )
        {
            if ( *pdwSize < vTmp.size() )
            {
                return LibJingleEx::LIBJINGLE_ERR_NOT_ENOUGH_MEM;
            }

            unsigned long i;
            vDmsMember::iterator it;

            for ( it = vTmp.begin(), i = 0; it != vTmp.end(); it++, i++ )
            {
                LibJingleEx::PTDmsMember pDmsMember = *it;
                strcpy( ptMemberState[i].szJid, pDmsMember->szJid );

                // �����jid����
                if ( DMS_MEMBER_TYPE_JID == pDmsMember->eType )
                {    
                    pAppObj->GetDmsMemberState( aszRes, dwResCnt, &ptMemberState[i] );
                }
            }

            *pdwSize = i;
            return LibJingleEx::LIBJINGLE_ERR_OK;
        }
        else
        {
            unsigned long i;
            vDmsMember::iterator it;

            for ( i = 0, it = vTmp.begin() + *pdwStartIndex; (i < *pdwSize) && (i + *pdwStartIndex < vTmp.size()); i++, it++ )
            {
                LibJingleEx::PTDmsMember pDmsMember = *it;
                strcpy( ptMemberState[i].szJid, pDmsMember->szJid );

                // �����jid����
                if ( DMS_MEMBER_TYPE_JID == pDmsMember->eType )
                {
                    pAppObj->GetDmsMemberState( aszRes, dwResCnt, &ptMemberState[i] );
                }
            }

            *pdwSize = i;
            *pdwStartIndex = *pdwStartIndex + i;

            if ( *pdwStartIndex >= vTmp.size() )
            {
                return LibJingleEx::LIBJINGLE_ERR_OK;
            }
            else
            {
                return LibJingleEx::LIBJINGLE_ERR_PARAM_BUFF_FULL;
            }
        }
    }



    LIBJINGLE_API  ELibjingleError  DmsQueryMember (  HANDLE hApp, unsigned long dwMemberId, PTDmsMember ptDmsMember )
    {

        if ( 0 == hApp ||  0 == dwMemberId || 0 == ptDmsMember  )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        cFuncLock cLock( &pAppObj->m_lock );

        // ���л�ȡgroup�����û�л�ȡmember���
        if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
        {
            return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
        }

        PTDmsMember p = pAppObj->GetDmsMember( dwMemberId );
        if ( 0 == p )
        {
            return LIBJINGLE_ERR_DMS_NO_SUCH_FRIEND;
        }

        memcpy( ptDmsMember, p, sizeof(TDmsMember) );

        return LibJingleEx::LIBJINGLE_ERR_OK;

    }

#else  // DMS_CONTACTS_SWITCH









    
LIBJINGLE_API  ELibjingleError  DmsAddGroup( HANDLE hApp, PTDmsGroup    ptDmsGroup )
{
    char buf[MAX_BUFF_SIZE];

    if ( 0 == hApp ||  0 == ptDmsGroup )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();
    cFuncLock cLock( &pAppObj->m_lock );

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }

    // ����и�����
    if ( 0 != ptDmsGroup->dwParentId )
    {
        return LIBJINGLE_ERR_PARENT_GROUP_NOT_SUPPORTED;
    }    

    // ��roster�����������
    BOOL  bAdded = FALSE;
    LibJingleEx::PTGroup  pRosterGroup = pAppObj->AddGroup( ptDmsGroup->szGroupName, &bAdded );
    assert( pRosterGroup );
    ptDmsGroup->dwGroupId = pRosterGroup->dwGroupId;

    // ���ȷʵ�����һ����
    if ( bAdded )
    {
        pAppObj->m_bRosterChanged = TRUE;
        pAppObj->SaveRoster2File();
        
        // ֪ͨ�ϲ�Ӧ����������
        if ( g_pfnOnMessage )
        {
            g_pfnOnMessage( LibJingleEx::ON_NOTIFY_ADD_DMS_GROUP, pAppObj, pAppObj->m_pUserData, 
                            (unsigned char *)ptDmsGroup, sizeof(LibJingleEx::TDmsGroup) );
        }
        // END( ����֪ͨ )
        return LibJingleEx::LIBJINGLE_ERR_OK;
    }
    else
    {
        return LIBJINGLE_ERR_ROSTER_GROUP_EXISTS;
    }
}

LIBJINGLE_API  ELibjingleError  DmsDelGroup( HANDLE hApp, unsigned long dwGroupId, BOOL bMemberReserverd /*= FALSE*/ )
{
    if ( 0 == hApp ||  0 == dwGroupId )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // ���л�ȡgroup�����û�л�ȡmember���
    if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
    {
        return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
    }

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }



    LibJingleEx::PTGroup pRosterGroup = pAppObj->FindGroup( dwGroupId );
    // ���û���ҵ���roster group
    if ( 0 == pRosterGroup )
    {
        return LIBJINGLE_ERR_NO_SUCH_ROSTER_GROUP;
    }

    // ���������ϵ�˵�δ����
    if ( bMemberReserverd )
    {
        // �����δ���飬����Ķ�
        if ( pRosterGroup->dwGroupId == SPECIAL_GROUP_SN )
        {
            return LIBJINGLE_ERR_OK;
        }

        PTMember pChild = pRosterGroup->pChild;
        while( pChild )
        {
            // �ƶ���δ����
            MoveFriend( hApp,  &pChild->tJid, pRosterGroup->szName, "" );
            pChild = pChild->pNext;
        }

        // ���������û�г�Ա
        if ( 0 == pRosterGroup->pChild )
        {
            pAppObj->RemoveGroup( pRosterGroup );
            pAppObj->m_bRosterChanged = TRUE;
            pAppObj->SaveRoster2File();

            // ֪ͨ��ɾ��
            if ( g_pfnOnMessage )
            {
                g_pfnOnMessage( LibJingleEx::ON_NOTIFY_DEL_DMS_GROUP, pAppObj, pAppObj->m_pUserData, 
                                (unsigned char *)&dwGroupId, sizeof(dwGroupId) );
            }
            // END(֪ͨ��ɾ��)
        }
        else
        {
            pRosterGroup->bToBeDeleted = TRUE;
        }
    }
    // �����ȫɾ��
    else
    {
        PTMember pChild = pRosterGroup->pChild;
        while( pChild )
        {
            DelFriend( hApp,  &pChild->tJid, pRosterGroup->szName, FALSE );
            pChild = pChild->pNext;
        }

        // ������ǡ�δ���顱
        if ( SPECIAL_GROUP_SN != pRosterGroup->dwGroupId )
        {
            // ���������û�г�Ա
            if ( 0 == pRosterGroup->pChild )
            {
                pAppObj->RemoveGroup( pRosterGroup );
                pAppObj->m_bRosterChanged = TRUE;
                pAppObj->SaveRoster2File();

                // ֪ͨ��ɾ��
                if ( g_pfnOnMessage )
                {
                    g_pfnOnMessage( LibJingleEx::ON_NOTIFY_DEL_DMS_GROUP, pAppObj, pAppObj->m_pUserData, 
                        (unsigned char *)&dwGroupId, sizeof(dwGroupId) );
                }
                // END(֪ͨ��ɾ��)
            }
            else
            {
                pRosterGroup->bToBeDeleted = TRUE;
            }
        }
    }

    return LibJingleEx::LIBJINGLE_ERR_OK;
}


LIBJINGLE_API  ELibjingleError  DmsModifyGroup( HANDLE hApp, PTDmsGroup    ptDmsGroup )
{
    if ( 0 == hApp ||  0 == ptDmsGroup )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // ���л�ȡgroup�����û�л�ȡmember���
    //if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
    //{
    //    return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
    //}

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }

    if ( 0 != ptDmsGroup->dwParentId )
    {
        return LIBJINGLE_ERR_PARENT_GROUP_NOT_SUPPORTED;
    }

    // ����ǡ�δ���顱
    if ( SPECIAL_GROUP_SN  == ptDmsGroup->dwGroupId )
    {
        return LIBJINGLE_ERR_CANT_MODIFY_SPECIAL_GROUP;
    }

    LibJingleEx::PTGroup pRosterGroup = pAppObj->FindGroup( ptDmsGroup->dwGroupId );
    // ���û���ҵ���roster group
    if ( 0 == pRosterGroup )
    {
        return LIBJINGLE_ERR_NO_SUCH_ROSTER_GROUP;
    }

    if ( pRosterGroup->bToBeDeleted )
    {
        return LIBJINGLE_ERR_ROSTER_GROUP_TO_BE_DELETED;
    }

    // û�иı�
    if ( 0 == strcmp( pRosterGroup->szName, ptDmsGroup->szGroupName ) )
    {
        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    LibJingleEx::PTGroup pMovedToGroup = pAppObj->FindGroupByName(ptDmsGroup->szGroupName);
    // ����и����ˣ��������޸�
    if ( 0 != pMovedToGroup )
    {
        return LIBJINGLE_ERR_ROSTER_GROUP_NAME_CONFLICT;
    }


    LibJingleEx::PTMember  pChild = pRosterGroup->pChild;
    while( pChild )
    {
        MoveFriend( hApp, &pChild->tJid, pRosterGroup->szName, ptDmsGroup->szGroupName );
        pChild = pChild->pNext;
    }

    if ( pRosterGroup->pChild )
    {
        pRosterGroup->bToBeDeleted = TRUE;
    }
    else
    {
        if ( g_pfnOnMessage )
        {
            g_pfnOnMessage( ON_NOTIFY_MODIFY_DMS_GROUP, pAppObj, pAppObj->m_pUserData, 
                            (unsigned char *)ptDmsGroup,  sizeof(TDmsGroup) );
        }
        strncpy( pRosterGroup->szName,  ptDmsGroup->szGroupName, LibJingleEx::MAX_GROUP_NAME_LEN );

        pAppObj->m_bRosterChanged = TRUE;
        pAppObj->SaveRoster2File();
    }
    

    // END(֪ͨ)
    return LibJingleEx::LIBJINGLE_ERR_OK;
}

// ��ʱ��֧��
LIBJINGLE_API  ELibjingleError  DmsMoveGroup  ( HANDLE hApp, unsigned long dwGroupId, 
                                               unsigned long dwNewParentGroupId )
{
    return LIBJINGLE_ERR_ERROR;
}


LIBJINGLE_API  ELibjingleError  DmsQuerySubGroups( HANDLE hApp, unsigned long dwParentGroupId, 
                                                  PTDmsGroup    ptDmsGroups,
                                                  unsigned long * pdwGroupsSize )
{
    if ( 0 == hApp ||  0 == ptDmsGroups || 0 == pdwGroupsSize )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }

    if ( 0 != dwParentGroupId )
    {
        *pdwGroupsSize = 0;
        return LibJingleEx::LIBJINGLE_ERR_OK;
    }


    LibJingleEx::TGroup * pGroup = pAppObj->m_pFirstGroup;
    unsigned long i = 0;
    while( pGroup )
    {
        if ( !pGroup->bToBeDeleted )
        {
            // ������㹻���� 
            if ( i < *pdwGroupsSize )
            {
                ptDmsGroups[i].dwParentId = 0;
                ptDmsGroups[i].dwGroupId  = pGroup->dwGroupId;
                if ( SPECIAL_GROUP_SN == pGroup->dwGroupId )
                {
                    strncpy( ptDmsGroups[i].szGroupName, SPECIAL_GROUP_NAME, LibJingleEx::MAX_DMS_GROUP_NAME_LEN );
                }
                else
                {
                    strncpy( ptDmsGroups[i].szGroupName, pGroup->szName, LibJingleEx::MAX_DMS_GROUP_NAME_LEN );
                }
                i++;
            }
            else
            {
                return LibJingleEx::LIBJINGLE_ERR_DMS_NOT_ENOUGH_BUFF;
            }
        }

        pGroup = pGroup->pNext;
    }

    *pdwGroupsSize = i;

    return LibJingleEx::LIBJINGLE_ERR_OK;
}


LIBJINGLE_API  ELibjingleError  DmsQueryGroupDepth( HANDLE hApp, unsigned long dwGroupId, 
                                                   unsigned long * pdwGroupsDepth )
{
    // ���Ϊ0
    *pdwGroupsDepth = 0;

    return LibJingleEx::LIBJINGLE_ERR_OK;
}



LIBJINGLE_API  ELibjingleError  DmsAddMember( HANDLE hApp, PTDmsMember     ptDmsMemberSrc )
{
    if ( 0 == hApp ||  0 == ptDmsMemberSrc || 0 == ptDmsMemberSrc->dwGroupId )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // ��û�л�ȡgroup�����û�л�ȡmember���
    if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
    {
        return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
    }

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }



    LibJingleEx::PTGroup pRosterGroup = pAppObj->FindGroup( ptDmsMemberSrc->dwGroupId );
    // ���û���ҵ���roster group
    if ( 0 == pRosterGroup )
    {
        return LIBJINGLE_ERR_NO_SUCH_ROSTER_GROUP;
    }


    // �����ϵ�˸���
    unsigned long dwTotalCount = pAppObj->GetFriendsCount();
    if ( dwTotalCount >= LibJingleEx::MAX_DMS_TOTAL_MEMBERS_COUNT )
    {
        return LIBJINGLE_ERR_DMS_REACH_MAX_TOTOL_MEMBERS_COUNT;
    }



    // �Ƿ���ͬjid��
    BOOL  bFound = FALSE;
    LibJingleEx::TJid  tJid;
    LibJingleEx::StrJid2TJid( ptDmsMemberSrc->szJid, tJid );

    LibJingleEx::PTMember pChild = pRosterGroup->pChild;
    while( pChild )
    {
        if ( IfBareJidEqual( pChild->tJid, tJid ) )
        {
            if ( !IfUnscribedContact( pChild ) )
            {
                bFound = TRUE;
            }
            else
            {
                Subscribe( hApp, &pChild->tJid );
                return LIBJINGLE_ERR_OK;
            }
            break;
        }

        pChild = pChild->pNext;
    }

    // �������
    if ( bFound )
    {
        return LIBJINGLE_ERR_ROSTER_CONTACT_EXISTS;
    }

    AddFriend( hApp, &tJid, pRosterGroup->szName, ptDmsMemberSrc->szName, ptDmsMemberSrc->szReserved[RESERVERD_BITRATE_INDEX], ptDmsMemberSrc->eType );

    return LIBJINGLE_ERR_OK;
}



LIBJINGLE_API  ELibjingleError  DmsDelMember( HANDLE hApp, unsigned long   dwMemberId )
{
    if ( 0 == hApp ||  0 == dwMemberId )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // ��û�л�ȡgroup�����û�л�ȡmember���
    if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
    {
        return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
    }

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }


    LibJingleEx::PTGroup  pGroup = 0;
    LibJingleEx::PTMember pMember = pAppObj->FindMember( dwMemberId, pGroup );

    // û���ҵ�
    if ( 0 == pMember )
    {
        return LIBJINGLE_ERR_ROSTER_NO_CONTACT;
    }

    // �����jid������û�ж������˵�״̬���Ҷ��ĵ�����Ҳû��
    if (  IfUnscribedContact(pMember) )
    {
        return LIBJINGLE_ERR_ROSTER_UNSCRIBED_CONTACT;
    }

    DelFriend( hApp, &pMember->tJid, pGroup->szName, FALSE );

    return LibJingleEx::LIBJINGLE_ERR_OK;
}

// ���� ptDmsMember->dwGroupId �� ptDmsMember->jid
LIBJINGLE_API  ELibjingleError  DmsModifyMember( HANDLE hApp, PTDmsMember  ptDmsMember )
{
    if ( 0 == hApp ||  0 == ptDmsMember || 0 == ptDmsMember->dwMemberId )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // ��û�л�ȡgroup�����û�л�ȡmember���
    if ( !pAppObj->m_bGetDmsGroupResult || !pAppObj->m_bGetDmsMemberResult )
    {
        return LIBJINGLE_ERR_DMS_PRIVATE_DATA_NOT_READY;
    }

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }


    LibJingleEx::PTGroup  pGroup = 0;
    LibJingleEx::PTMember pMember = pAppObj->FindMember( ptDmsMember->dwMemberId, pGroup );

    // û���ҵ�
    if ( 0 == pMember )
    {
        return LIBJINGLE_ERR_ROSTER_NO_CONTACT;
    }

    // �����jid������û�ж������˵�״̬���Ҷ��ĵ�����Ҳû��
    if (  IfUnscribedContact(pMember) )
    {
        return LIBJINGLE_ERR_ROSTER_UNSCRIBED_CONTACT;
    }

    SetFriendNick( hApp, &pMember->tJid, ptDmsMember->szName, ptDmsMember->szReserved[RESERVERD_BITRATE_INDEX] );

    return LIBJINGLE_ERR_OK;
}

LIBJINGLE_API  ELibjingleError  DmsMoveMember  ( HANDLE hApp, unsigned long dwMemberId, 
                                                unsigned long dwNewGroupId )
{
    if ( 0 == hApp ||  0 == dwMemberId || 0 == dwNewGroupId )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }


    LibJingleEx::PTGroup  pGroup = 0;
    LibJingleEx::PTMember pMember = pAppObj->FindMember( dwMemberId, pGroup );
    if ( 0 == pMember )
    {
        return LIBJINGLE_ERR_ROSTER_NO_CONTACT;
    }

    // �����jid������û�ж������˵�״̬���Ҷ��ĵ�����Ҳû��
    if (  IfUnscribedContact(pMember) )
    {
        return LIBJINGLE_ERR_ROSTER_UNSCRIBED_CONTACT;
    }

    LibJingleEx::PTGroup pNewGroup = pAppObj->FindGroup( dwNewGroupId );
    if ( 0 == pNewGroup )
    {
        return LIBJINGLE_ERR_NO_SUCH_ROSTER_GROUP;
    }

    MoveFriend( hApp, &pMember->tJid, pGroup->szName, pNewGroup->szName );

    return LIBJINGLE_ERR_OK;
}

LIBJINGLE_API  ELibjingleError  DmsQueryMembers (    HANDLE hApp, unsigned long dwParentGroupId, 
                                                     PTDmsMember     ptDmsMembers,
                                                     unsigned long * pdwMembersSize,
                                                     unsigned long * pdwStartIndex /*= 0*/,
                                                     unsigned long * pdwEndIndex   /*= 0*/ )
{
    if ( 0 == hApp ||  0 == dwParentGroupId || 0 == ptDmsMembers || 0 == pdwMembersSize )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }



    LibJingleEx::PTGroup pRosterGroup = pAppObj->FindGroup( dwParentGroupId );
    if ( 0 == pRosterGroup )
    {
        return LIBJINGLE_ERR_NO_SUCH_ROSTER_GROUP;
    }

    if ( pRosterGroup->bToBeDeleted )
    {
        return LIBJINGLE_ERR_ROSTER_GROUP_TO_BE_DELETED;
    }


    // ����ǰ����ٸ���Ա
    unsigned long dwStartIndex = 0;
    if ( 0 != pdwStartIndex )
    {
        dwStartIndex = *pdwStartIndex;
    }

    // ͬһ���µ�ǰ��Ա������ 
    unsigned long dwCurMemberIndex = 0;

    LibJingleEx::PTMember  pChild = pRosterGroup->pChild;
    while( pChild )
    {
        // �����û�ж��ĵ���ϵ�ˣ�����Ե�
        if ( IfUnscribedContact(pChild) )
        {
            pChild = pChild->pNext;
            continue;
        }

        if ( dwCurMemberIndex >= dwStartIndex )
        {
            unsigned long k = dwCurMemberIndex - dwStartIndex;

            // ������㹻����
            if ( k < *pdwMembersSize )
            {
                // memcpy( &ptDmsMembers[k], p, sizeof(TDmsMember) );
                ptDmsMembers[k].dwGroupId  = dwParentGroupId;
                ptDmsMembers[k].dwMemberId = pChild->dwId;
                ptDmsMembers[k].eType      = (EDmsMemberType)pChild->nType;
                LibJingleEx::TJid2StrJid( pChild->tJid, ptDmsMembers[k].szJid, sizeof(ptDmsMembers[k].szJid) );
                strncpy( ptDmsMembers[k].szName, pChild->szName, sizeof(ptDmsMembers[k].szName) );
                strncpy( ptDmsMembers[k].szReserved[RESERVERD_BITRATE_INDEX], pChild->szBitRate, MAX_DMS_MEMBER_RESERVERD_BUF_LEN );
            }
            // ���û���㹻����
            else
            {
                if ( 0 != pdwEndIndex )
                {
                    *pdwEndIndex = dwCurMemberIndex;
                }

                return LibJingleEx::LIBJINGLE_ERR_DMS_NOT_ENOUGH_BUFF;
            }
        }

        dwCurMemberIndex++;
        pChild = pChild->pNext;
    }

    if ( dwCurMemberIndex >= dwStartIndex )
    {
        *pdwMembersSize = dwCurMemberIndex - dwStartIndex;
    }
    else
    {
        *pdwMembersSize = 0;
    }

    return LibJingleEx::LIBJINGLE_ERR_OK;
}


LIBJINGLE_API ELibjingleError DmsGetPresenceByGroup(    HANDLE hApp, TPresence * ptPresence,
                                                        unsigned long * pdwSize, unsigned long dwGroupId, 
                                                        unsigned long * pdwStartIndex /* = 0 */ )
{
    if ( 0 == hApp ||  0 == ptPresence || 0 == pdwSize || 0 == dwGroupId )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }


    LibJingleEx::PTGroup pRosterGroup = pAppObj->FindGroup( dwGroupId );
    if ( 0 == pRosterGroup )
    {
        return LIBJINGLE_ERR_NO_SUCH_ROSTER_GROUP;
    }

    if ( pRosterGroup->bToBeDeleted )
    {
        return LIBJINGLE_ERR_ROSTER_GROUP_TO_BE_DELETED;
    }


    // ����ǰ����ٸ���Ա
    unsigned long dwStartIndex = 0;
    if ( 0 != pdwStartIndex )
    {
        dwStartIndex = *pdwStartIndex;
    }

    // ͬһ���µ�ǰ��Ա������ 
    unsigned long dwCurMemberIndex = 0;

    LibJingleEx::PTMember  pChild = pRosterGroup->pChild;
    while( pChild )
    {
        // �����û�ж��ĵ���ϵ�ˣ�����Ե�
        if ( IfUnscribedContact(pChild) )
        {
            pChild = pChild->pNext;
            continue;
        }

        if ( dwCurMemberIndex >= dwStartIndex )
        {
            unsigned long k = dwCurMemberIndex - dwStartIndex;

            // ������㹻����
            if ( k < *pdwSize )
            {
                // �����jid����
                if ( DMS_MEMBER_TYPE_JID == pChild->nType )
                {
                    memcpy( &ptPresence[k].tJid, &pChild->tJid, sizeof(LibJingleEx::TJid) );
                    ptPresence[k].ePresence = LibJingleEx::GetFriendPresence( hApp, &ptPresence[k].tJid );
                }
                else
                {
                    ptPresence[k].ePresence = PRESENCE_OFFLINE;
                }
            }
            // ���û���㹻����
            else
            {
                if ( pdwStartIndex )
                {
                    *pdwStartIndex = dwCurMemberIndex;
                }
                return LibJingleEx::LIBJINGLE_ERR_PARAM_BUFF_FULL;
            }
        }

        dwCurMemberIndex++;
        pChild = pChild->pNext;
    }


    if ( dwCurMemberIndex >= dwStartIndex )
    {
        *pdwSize = dwCurMemberIndex - dwStartIndex;
    }
    else
    {
        *pdwSize = 0;
    }

    return LibJingleEx::LIBJINGLE_ERR_OK;
}


LIBJINGLE_API ELibjingleError DmsGetPresenceByGroup(    HANDLE hApp, 
                                                        const char aszRes[][MAX_RESOURCE_LEN], unsigned long dwResCnt,
                                                        PTDmsMemberState  ptMemberState,       unsigned long * pdwSize, 
                                                        unsigned long   dwGroupId, 
                                                        unsigned long * pdwStartIndex /* = 0 */ )
{
    if ( 0 == hApp ||  0 == aszRes || 0 == dwResCnt || 0 == ptMemberState || 0 == pdwSize || 0 == dwGroupId
        || dwResCnt > MAX_DMS_MEMBER_STATE_CNT )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    cFuncLock cLock( &pAppObj->m_lock );

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }

    LibJingleEx::PTGroup pRosterGroup = pAppObj->FindGroup( dwGroupId );
    if ( 0 == pRosterGroup )
    {
        return LIBJINGLE_ERR_NO_SUCH_ROSTER_GROUP;
    }

    if ( pRosterGroup->bToBeDeleted )
    {
        return LIBJINGLE_ERR_ROSTER_GROUP_TO_BE_DELETED;
    }

    // ����ǰ����ٸ���Ա
    unsigned long dwStartIndex = 0;
    if ( 0 != pdwStartIndex )
    {
        dwStartIndex = *pdwStartIndex;
    }

    // ͬһ���µ�ǰ��Ա������ 
    unsigned long dwCurMemberIndex = 0;

    LibJingleEx::PTMember  pChild = pRosterGroup->pChild;
    while( pChild )
    {
        // �����û�ж��ĵ���ϵ�ˣ�����Ե�
        if ( IfUnscribedContact(pChild) )
        {
            pChild = pChild->pNext;
            continue;
        }

        if ( dwCurMemberIndex >= dwStartIndex )
        {
            unsigned long k = dwCurMemberIndex - dwStartIndex;

            // ������㹻����
            if ( k < *pdwSize )
            {
                LibJingleEx::TJid2StrJid( pChild->tJid, ptMemberState[k].szJid, MAX_JID_LEN );
                // �����jid����
                if ( DMS_MEMBER_TYPE_JID == pChild->nType )
                {    
                    pAppObj->GetDmsMemberState( aszRes, dwResCnt, &ptMemberState[k] );
                }
            }
            // ���û���㹻����
            else
            {
                if ( pdwStartIndex )
                {
                    *pdwStartIndex = dwCurMemberIndex;
                }
                return LibJingleEx::LIBJINGLE_ERR_PARAM_BUFF_FULL;
            }
        }

        dwCurMemberIndex++;
        pChild = pChild->pNext;
    }


    if ( dwCurMemberIndex >= dwStartIndex )
    {
        *pdwSize = dwCurMemberIndex - dwStartIndex;
    }
    else
    {
        *pdwSize = 0;
    }

    return LibJingleEx::LIBJINGLE_ERR_OK;
}



LIBJINGLE_API  ELibjingleError  DmsQueryMember (  HANDLE hApp, unsigned long dwMemberId, PTDmsMember ptDmsMember )
{

    if ( 0 == hApp ||  0 == dwMemberId || 0 == ptDmsMember  )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LIBJINGLE_ERR_DISCONNECTED;
    }

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    cFuncLock cLock( &pAppObj->m_lock );

    // �����û�л�ȡroster����
    if ( !pAppObj->m_bGetRosterData )
    {
        return LIBJINGLE_ERR_ROSTER_NOT_READY;
    }

    LibJingleEx::PTGroup  pGroup = 0;
    LibJingleEx::PTMember pMember = pAppObj->FindMember( dwMemberId, pGroup );

    if ( 0 == pMember )
    {
        return LIBJINGLE_ERR_ROSTER_NO_CONTACT;
    }

    // �����û�ж��ĵ���ϵ��
    if ( IfUnscribedContact(pMember) )
    {
        return LIBJINGLE_ERR_ROSTER_UNSCRIBED_CONTACT;
    }

    ptDmsMember->dwGroupId  = pGroup->dwGroupId;
    ptDmsMember->dwMemberId = pMember->dwId;
    ptDmsMember->eType      = (EDmsMemberType)pMember->nType;
    LibJingleEx::TJid2StrJid( pMember->tJid, ptDmsMember->szJid, MAX_JID_LEN );
    strncpy( ptDmsMember->szName, pMember->szName, MAX_DMS_MEMBER_NAME_LEN );
    strncpy( ptDmsMember->szReserved[RESERVERD_BITRATE_INDEX], pMember->szBitRate, MAX_DMS_MEMBER_RESERVERD_BUF_LEN );

    return LibJingleEx::LIBJINGLE_ERR_OK;
}





#endif











































    // ˵����aPres����Ԫ�ظ��������aszRes��ͬ����dwResCnt
    LIBJINGLE_API ELibjingleError  GetMemberCapabilities( HANDLE hApp, char aszRes[][MAX_RESOURCE_LEN], 
        unsigned long dwResCnt, const PTJid ptBareJid, 
        PTDmsMemberState ptMemberState )
    {
        if ( 0 == hApp || 0 == aszRes || 0 == dwResCnt || 0 == ptBareJid || 0 == ptMemberState )
        {
            return LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock( &pAppObj->m_external_lock );
        cFuncLock cLock_( &pAppObj->m_lock );


        // ��ȡBareJid
        LibJingleEx::TJid tJid;
        memcpy( &tJid, ptBareJid, sizeof(tJid) );
        tJid.sResource[0] = '\0';


        vMemberGroups & v = pAppObj->m_MemberGroups;
        vMemberGroups::iterator it_find = find_if( v.begin(), v.end(), FindMemberGroupObj(&tJid) );
        if ( it_find == v.end() )
        {
            return LibJingleEx::LIBJINGLE_ERR_NO_SUCH_FRIEND;
        }

        PTMemberGroup p = *it_find;
        vMemberStatus & s = p->m_Status;
        vMemberStatus::iterator it;

        ptMemberState->eMaxPres = PRESENCE_NONE;

        unsigned long i;
        for ( i = 0; i < dwResCnt; i++ )
        {
            ptMemberState->ePres[i]  = PRESENCE_NONE;
            ptMemberState->bAudio[i] = FALSE;
            ptMemberState->bVideo[i] = FALSE;
            ptMemberState->bCall[i]  = FALSE;
        }

        ptMemberState->bMaxAudio = FALSE;
        ptMemberState->bMaxVideo = FALSE;

        LibJingleEx::TJid2StrJid( tJid, ptMemberState->szJid, sizeof(ptMemberState->szJid) );


        unsigned long dwMaxWeight = GetPresenceWeight( ptMemberState->eMaxPres );

        for ( it = s.begin(); it != s.end(); it++ )
        {
            PTMemberStatus pStatus = *it;
            unsigned long dwWeight = GetPresenceWeight( pStatus->m_ePresence );

            // �ҵ����״̬
            if ( dwWeight > dwMaxWeight )
            {
                dwMaxWeight = dwWeight;
            }

            if ( pStatus->m_bCall )
            {
                if ( pStatus->m_bAudio )
                {
                    ptMemberState->bMaxAudio = TRUE;
                }

                if ( pStatus->m_bVideo )
                {
                    ptMemberState->bMaxVideo = TRUE;
                }
            }

            for ( i = 0; i < dwResCnt; i++ )
            {
                // ����ҵ���Ӧ����Դ
                if ( 0 == strcmp(aszRes[i], pStatus->m_szResource) )
                {
                    ptMemberState->ePres[i] = pStatus->m_ePresence;

                    ptMemberState->bAudio[i] = pStatus->m_bAudio;
                    ptMemberState->bVideo[i] = pStatus->m_bVideo;
                    ptMemberState->bCall[i]  = pStatus->m_bCall;
                }
            }
        }

        ptMemberState->eMaxPres = GetPresenceFromWeight( dwMaxWeight );


        return LibJingleEx::LIBJINGLE_ERR_OK;
    }


    // ����
    LIBJINGLE_API  ELibjingleError  CreatePubsubNode( HANDLE hApp, const TPubsubParam * ptParam, 
        unsigned long dwContextId )
    {
        if ( 0 == hApp || 0 == ptParam || '\0' == ptParam->szNodeName[0] )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        if ( 0 == client->PubsubJid().length() )
        {
            return LIBJINGLE_ERR_NO_PUBSUB_SERVICE;
        }

        char buf[MAX_BUFF_SIZE];
        char szRandId[MAX_RAND_ID_LENGTH];
        GenerateRandomId( szRandId, MAX_RAND_ID_LENGTH );

        buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);
        iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
        iq->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->PubsubJid().c_str(), buf, MAX_BUFF_SIZE ) );
        iq->SetAttr( buzz::QN_ID,   szRandId );

        buzz::XmlElement * pubsub = new buzz::XmlElement( buzz::QN_PUBSUB, true );
        iq->AddElement( pubsub );

        buzz::XmlElement * create = new buzz::XmlElement( buzz::QN_CREATE, true );
        pubsub->AddElement( create );
        create->SetAttr( buzz::QN_NODE, AnsiToUtf8( ptParam->szNodeName, buf, MAX_BUFF_SIZE ) );

        buzz::XmlElement * configure = new buzz::XmlElement( buzz::QN_CONFIGURE, true );
        pubsub->AddElement( configure );

        buzz::XmlElement * x = new buzz::XmlElement( buzz::QN_X, true );
        configure->AddElement( x );
        x->SetAttr( buzz::QN_TYPE, "submit" );

        buzz::XmlElement * field = new buzz::XmlElement( buzz::QN_FIELD, true );
        x->AddElement( field );
        field->SetAttr( buzz::QN_VAR, "FORM_TYPE" );
        field->SetAttr( buzz::QN_TYPE, "hidden" );

        buzz::XmlElement * value = new buzz::XmlElement( buzz::QN_VALUE, true );
        field->AddElement( value );
        value->SetBodyText("http://jabber.org/protocol/pubsub#node_config");


        field = new buzz::XmlElement( buzz::QN_FIELD, true );
        x->AddElement( field );
        field->SetAttr( buzz::QN_VAR, "pubsub#publish_model" );

        value = new buzz::XmlElement( buzz::QN_VALUE, true );
        field->AddElement( value );
        value->SetBodyText("open");

        PTIqId  ptIqId = new TIqId;
        ptIqId->dwType = IQ_TYPE_PUBSUB;
        strcpy( ptIqId->szIqId, szRandId );
        ptIqId->dwContextId = dwContextId;
        ptIqId->dwSubType   = PUBSUB_SUBTYPE_CREATE_NODE;

        // ptIqId���յ�result��error��Ӧ������
        pAppObj->m_pXmppThread->SendStanza( ptIqId, iq );

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    // ����
    LIBJINGLE_API  ELibjingleError  SubscribePubsubNode( HANDLE hApp, const char * szPubsubNodeName,
        unsigned long dwContextId )
    {
        if ( 0 == hApp || 0 == szPubsubNodeName || '\0' == szPubsubNodeName[0] )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����


        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        if ( 0 == client->PubsubJid().length() )
        {
            return LIBJINGLE_ERR_NO_PUBSUB_SERVICE;
        }

        char buf[MAX_BUFF_SIZE];
        char szRandId[MAX_RAND_ID_LENGTH];
        GenerateRandomId( szRandId, MAX_RAND_ID_LENGTH );

        buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);
        iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
        iq->SetAttr( buzz::QN_TO,   AnsiToUtf8(client->PubsubJid().c_str(), buf, MAX_BUFF_SIZE) );
        iq->SetAttr( buzz::QN_ID,   szRandId );

        buzz::XmlElement * pubsub = new buzz::XmlElement( buzz::QN_PUBSUB, true );
        iq->AddElement( pubsub );

        buzz::XmlElement * subscribe = new buzz::XmlElement( buzz::QN_SUBSCRIBE, true );
        pubsub->AddElement( subscribe );
        subscribe->SetAttr( buzz::QN_NODE, AnsiToUtf8(szPubsubNodeName, buf, MAX_BUFF_SIZE) );
        subscribe->SetAttr( buzz::QN_JID,  client->jid().BareJid().Str() );

        PTIqId  ptIqId = new TIqId;
        ptIqId->dwType = IQ_TYPE_PUBSUB;
        strcpy( ptIqId->szIqId, szRandId );
        ptIqId->dwContextId = dwContextId;
        ptIqId->dwSubType   = PUBSUB_SUBTYPE_SUBSCRIBE_NODE;

        // ptIqId���յ�result��error��Ӧ������
        pAppObj->m_pXmppThread->SendStanza( ptIqId, iq );

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }

    // ɾ��
    LIBJINGLE_API  ELibjingleError  DeletePubsubNode( HANDLE hApp, const char * szPubsubNodeName,
        unsigned long dwContextId   )
    {
        if ( 0 == hApp || 0 == szPubsubNodeName || '\0' == szPubsubNodeName[0] )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

        if ( 0 == client->PubsubJid().length() )
        {
            return LIBJINGLE_ERR_NO_PUBSUB_SERVICE;
        }

        char buf[MAX_BUFF_SIZE];
        char szRandId[MAX_RAND_ID_LENGTH];
        GenerateRandomId( szRandId, MAX_RAND_ID_LENGTH );

        buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);
        iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
        iq->SetAttr( buzz::QN_TO,   AnsiToUtf8(client->PubsubJid().c_str(), buf, MAX_BUFF_SIZE) );
        iq->SetAttr( buzz::QN_ID,   szRandId );

        buzz::XmlElement * pubsub = new buzz::XmlElement( buzz::QN_PUBSUB_ADMIN, true );
        iq->AddElement( pubsub );

        buzz::XmlElement * del = new buzz::XmlElement( buzz::QN_DELETE, true );
        pubsub->AddElement( del );
        del->SetAttr( buzz::QN_NODE, AnsiToUtf8(szPubsubNodeName, buf, MAX_BUFF_SIZE) );

        PTIqId  ptIqId = new TIqId;
        ptIqId->dwType = IQ_TYPE_PUBSUB;
        strcpy( ptIqId->szIqId, szRandId );
        ptIqId->dwContextId = dwContextId;
        ptIqId->dwSubType   = PUBSUB_SUBTYPE_DELETE_NODE;

        // ptIqId���յ�result��error��Ӧ������
        pAppObj->m_pXmppThread->SendStanza( ptIqId, iq );

        return LibJingleEx::LIBJINGLE_ERR_OK;
    }



    LIBJINGLE_API  ELibjingleError  SendPing( HANDLE hApp )
    {
        if ( 0 == hApp )
        {
            return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
        }

        // ת��
        CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

        // �����
        CGlobalData * pGlobal = CGlobalData::GetInstance();
        if ( !pGlobal->IfExistApp(pAppObj) )
        {
            return LIBJINGLE_ERR_INVALID_HANDLE;
        }
        // END �����

        // �̰߳�ȫ
        cFuncLock cLock_( &pAppObj->m_external_lock );

        // û�е�¼
        if ( 0 == pAppObj->m_pXmppThread )
        {
            return LIBJINGLE_ERR_NOT_LOGIN;
        }

        if ( STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
        {
            return LIBJINGLE_ERR_DISCONNECTED;
        }

        cFuncLock cLock( &pAppObj->m_lock );

        pAppObj->m_pXmppThread->Ping();


        //char szRandId[32];
        //GenerateRandomId( szRandId, 32, PREFIX_PING );

        //char buf[MAX_BUFF_SIZE];
        //SNPRINTF( buf, MAX_BUFF_SIZE,  "<iq type='get' id='%s'>"
        //    "<ping xmlns='urn:xmpp:ping'/>"
        //    "</iq>",  szRandId );

        //TIqId  tIqId;
        //tIqId.dwType = IQ_TYPE_PING;
        //strcpy( tIqId.szIqId, szRandId );

        //// �ȱ�����ping task��
        //pAppObj->m_pXmppThread->pump()->AddIqId( &tIqId );    

        //// �ٷ���ping
        //pAppObj->m_pXmppThread->SendRaw( buf );

        //// 10����������յ�pingӦ��
        //pAppObj->m_pXmppThread->PostDelayed( PING_TIMEOUT, pAppObj->m_pXmppThread,  MSG_PING_TIMEOUT, new PingTimeoutMsg( &tIqId ) );


        return LibJingleEx::LIBJINGLE_ERR_OK;
    }









}// end of namespace LibjingleEx




void  SaveDmsGroup2Server( CLibJinglexApp * pAppObj )
{
    vDmsGroup & v = pAppObj->m_vDmsGroups;
    vDmsGroup::iterator it;

    char buf[MAX_BUFF_SIZE];

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

    // iq����type����
    iq->SetAttr(buzz::QN_TYPE,buzz::STR_SET);

    // iq����id����
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->SetAttr(buzz::QN_ID, randid);

    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
    iq->AddElement( query );

    buzz::XmlElement * group = new buzz::XmlElement(buzz::QN_DMS_GROUP, true);
    query->AddElement( group );

    for ( it = v.begin(); it != v.end(); it++ )
    {
        LibJingleEx::PTDmsGroup p = *it;

        buzz::XmlElement * item = new buzz::XmlElement(buzz::QN_DMS_ITEM, true);
        group->AddElement(item);

        sprintf( buf, "%u", p->dwGroupId );
        item->SetAttr( buzz::QN_DMS_ID,        buf );

        sprintf( buf, "%u", p->dwParentId );
        item->SetAttr( buzz::QN_DMS_PARENT_ID, buf );

        item->SetAttr( buzz::QN_DMS_NAME,      AnsiToUtf8( p->szGroupName, buf, MAX_BUFF_SIZE ) );
    }

    // client->SendStanza( iq );
    //delete iq;
    pAppObj->m_pXmppThread->SendStanza( iq );
}


void  SaveDmsMember2Server( CLibJinglexApp * pAppObj, unsigned long dwBlockBits )
{
    char buf[MAX_BUFF_SIZE];
    unsigned long dwBitIndex;
    for ( dwBitIndex = 0; dwBitIndex < 31 && dwBitIndex < DMS_MEMBERS_BLOCK_CNT; dwBitIndex++ )
    {
        // ���û�����ø�bitλ
        if ( !IfHasBit( dwBlockBits, dwBitIndex ) )
        {
            continue;
        }


        vDmsMember & v = pAppObj->m_vDmsMembers[dwBitIndex];
        vDmsMember::iterator it;


        buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

        // iq����type����
        iq->SetAttr(buzz::QN_TYPE,buzz::STR_SET);

        // iq����id����
        char randid[MAX_RAND_ID_LENGTH];
        GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
        iq->SetAttr(buzz::QN_ID, randid);

        buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
        iq->AddElement( query );


        // buzz::XmlElement * member = new buzz::XmlElement(buzz::QN_DMS_MEMBER, true);
        char  szBlockName[256];
        GetDmsMembersBlockName( szBlockName, 256, dwBitIndex );
        buzz::XmlElement * member = new buzz::XmlElement( buzz::QName( true, szBlockName, "member" ), true );
        query->AddElement( member );


        for ( it = v.begin(); it != v.end(); it++ )
        {
            LibJingleEx::PTDmsMember p = *it;

            buzz::XmlElement * item = new buzz::XmlElement(buzz::QN_DMS_ITEM, true);
            member->AddElement(item);

            item->SetAttr( buzz::QN_DMS_JID, AnsiToUtf8( p->szJid, buf, MAX_BUFF_SIZE )  );

            sprintf( buf, "%u", p->dwMemberId );
            item->SetAttr( buzz::QN_DMS_ID,        buf );

            sprintf( buf, "%u", p->dwGroupId );
            item->SetAttr( buzz::QN_DMS_PARENT_ID, buf );

            item->SetAttr( buzz::QN_DMS_NAME,      AnsiToUtf8( p->szName, buf, MAX_BUFF_SIZE ) );

            sprintf( buf, "%u", (unsigned long)p->eType );
            item->SetAttr( buzz::QN_DMS_TYPE,      buf );

            item->SetAttr( buzz::QN_DMS_RESERVERD1, AnsiToUtf8(p->szReserved[0], buf, MAX_BUFF_SIZE ) );
            item->SetAttr( buzz::QN_DMS_RESERVERD2, AnsiToUtf8(p->szReserved[1], buf, MAX_BUFF_SIZE ) );
            item->SetAttr( buzz::QN_DMS_RESERVERD3, AnsiToUtf8(p->szReserved[2], buf, MAX_BUFF_SIZE ) );
            item->SetAttr( buzz::QN_DMS_RESERVERD4, AnsiToUtf8(p->szReserved[3], buf, MAX_BUFF_SIZE ) );
        }

        pAppObj->m_pXmppThread->SendStanza( iq );
    }
}









enum EmJError
{
    JSTR_ERROR_OK                    = 0,            // ��ȷ
    JSTR_ERROR_INVALID_ARGUMENT,                     // ��Ч����
    JSTR_ERROR_NOT_ENOUGH,                           // û���㹻������
    JSTR_ERROR_NO_MEMORY,                            // �����ڴ�ʧ��
    JSTR_ERROR_FAIL_TRANSFER,                        // ת��ʧ��(��ʽ����)
    JSTR_ERROR_TOO_LARGE_NUMBER,                     // ̫�������
    JSTR_ERROR_BASE64_LENGTH_INVALID                 // base64�ִ�������Ч
};

static const char s_base64_alphabet[]= 
{ 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P', 
'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f', 
'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v', 
'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/', '=' }; 

static BYTE _GetBase64Index( char chBase64 )
{
    if ( chBase64 >= 'A' && chBase64 <= 'Z' )
    {
        return chBase64 - 'A';
    }
    else if ( chBase64 >= 'a' && chBase64 <= 'z' )
    {
        return chBase64 - 'a' + 26;
    }
    else if ( chBase64 >= '0' && chBase64 <= '9' )
    {
        return chBase64 - '0' + 52;
    }
    else if ( chBase64 == '+' )
    {
        return 62;
    }
    else if ( chBase64 == '/' )
    {
        return 63;
    }
    else if ( chBase64 == '=' )
    {
        return 64;
    }
    else
    {
        return (BYTE)-1;
    }
}

// �õ�ĳ���ֽڵ�ǰ��λ
#define GET_FRONT_BITS( b, n )     ((BYTE)((b)>>(8-(n))))
// �õ�ĳ���ֽڵĺ�λ
#define GET_BACK_BITS( b, n )      (((BYTE)((b)<<(8-(n))))>>(8-(n)))

/*****************************************************************************/
// ����: ���ֽ���ת����base64��ʽ               
/*****************************************************************************/
LIBJINGLE_API  int LibJingleEx::EncodeBase64( char * szBase64, unsigned long & dwSize, 
                                             const            unsigned char * pbySrc, 
                                             unsigned long dwSrcLength )
{
    if ( 0 == szBase64 || 0 == dwSize || 0 == pbySrc || 0 == dwSrcLength )
    {
        return JSTR_ERROR_INVALID_ARGUMENT;
    }

    DWORD dwMultiple  = dwSrcLength / 3;
    DWORD dwResidue   = dwSrcLength % 3;

    DWORD dwMinSize = 4 * dwMultiple;

    if ( 0 != dwResidue )
    {
        dwMinSize += 4;
    }

    // ����һ�������ַ�'\0'
    if ( dwSize < dwMinSize + 1 )
    {
        return JSTR_ERROR_NOT_ENOUGH;
    }

    DWORD i;
    for ( i = 0; i < dwMultiple; i++ )
    {
        const BYTE * p = pbySrc + 3 * i;

        BYTE b1 = GET_FRONT_BITS( p[0], 6 ) ;
        BYTE b2 = ( GET_BACK_BITS( p[0], 2 )<<4) + GET_FRONT_BITS(p[1], 4);
        BYTE b3 = ( GET_BACK_BITS(p[1],4)<<2 ) + GET_FRONT_BITS(p[2], 2);
        BYTE b4 = GET_BACK_BITS( p[2], 6 );

        assert( b1 <= 63 && b2 <= 63 && b3 <= 63 && b4 <= 63 );

        char * q = szBase64 + 4 * i;

        q[0] = s_base64_alphabet[b1];
        q[1] = s_base64_alphabet[b2];
        q[2] = s_base64_alphabet[b3];
        q[3] = s_base64_alphabet[b4];
    }

    const BYTE * p = pbySrc + 3 * i;
    char * q                = szBase64 + 4 * i;

    if ( 1 == dwResidue )
    {
        BYTE b1 = GET_FRONT_BITS( p[0], 6 ) ;
        BYTE b2 = ( GET_BACK_BITS( p[0], 2 )<<4);

        q[0] = s_base64_alphabet[b1];
        q[1] = s_base64_alphabet[b2];
        q[2] = s_base64_alphabet[64];
        q[3] = s_base64_alphabet[64];

        i++;
    }
    else if ( 2 == dwResidue )
    {
        BYTE b1 = GET_FRONT_BITS( p[0], 6 ) ;
        BYTE b2 = ( GET_BACK_BITS( p[0], 2 )<<4) | GET_FRONT_BITS(p[1], 4);
        BYTE b3 = GET_BACK_BITS(p[1],4)<<2;

        q[0] = s_base64_alphabet[b1];
        q[1] = s_base64_alphabet[b2];
        q[2] = s_base64_alphabet[b3];
        q[3] = s_base64_alphabet[64];

        i++;
    }

    dwSize = 4 * i;
    *( szBase64 + 4 * i ) = '\0';

    return JSTR_ERROR_OK;
}

/*****************************************************************************/
// ����: ��base64��ʽת�����ֽ���           
// ˵��: pdwSize���������������pbyDest�Ĵ�С
/*****************************************************************************/
LIBJINGLE_API  int LibJingleEx::DecodeBase64( unsigned char * pbyDest, unsigned long * pdwSize, 
                                             const char * szBase64 )
{
    if ( 0 == pbyDest || 0 == pdwSize || 0 == szBase64 )
    {
        return JSTR_ERROR_INVALID_ARGUMENT;
    }

    DWORD dwBase64Size = strlen( szBase64 );

    if ( 0 != dwBase64Size % 4 )
    {
        return JSTR_ERROR_BASE64_LENGTH_INVALID;
    }

    DWORD dwMultiple =  dwBase64Size / 4;

    DWORD i;

    DWORD dwSize = 0;

    for ( i = 0; i < dwMultiple; i++ )
    {
        BYTE * q = pbyDest + 3 * i;
        const char * p = szBase64 + 4 * i;

        BYTE b1 = _GetBase64Index( p[0] );
        BYTE b2 = _GetBase64Index( p[1] );
        BYTE b3 = _GetBase64Index( p[2] );
        BYTE b4 = _GetBase64Index( p[3] );

        if (   b1 == (BYTE)(-1) || b2 == (BYTE)(-1)
            || b3 == (BYTE)(-1) || b4 == (BYTE)(-1) )
        {
            return JSTR_ERROR_INVALID_ARGUMENT;
        }

        if ( 64 == b1 || 64 == b2 )
        {
            return JSTR_ERROR_INVALID_ARGUMENT;
        }

        if ( i < dwMultiple - 1 )
        {
            if ( 64 == b3 || 64 == b4 )
            {
                return JSTR_ERROR_INVALID_ARGUMENT;
            }

            dwSize += 3;
            if ( *pdwSize < dwSize )
            {
                return JSTR_ERROR_NOT_ENOUGH;
            }

            q[0] = (GET_BACK_BITS(b1,6)<<2) | GET_FRONT_BITS(b2,4);
            q[1] = (GET_BACK_BITS(b2,4)<<4) | GET_FRONT_BITS(b3,6);
            q[2] = (GET_BACK_BITS(b3,2)<<6) | b4;
        }
        else
        {
            if ( 64 == b3 )
            {
                if ( 64 != b4 )
                {
                    return JSTR_ERROR_INVALID_ARGUMENT;
                }

                dwSize++;
                if ( *pdwSize < dwSize )
                {
                    return JSTR_ERROR_NOT_ENOUGH;
                }

                q[0] = (GET_BACK_BITS(b1,6)<<2) | GET_FRONT_BITS(b2,4);
            }
            else if ( 64 == b4 )
            {
                dwSize += 2;
                if ( *pdwSize < dwSize )
                {
                    return JSTR_ERROR_NOT_ENOUGH;
                }

                q[0] = (GET_BACK_BITS(b1,6)<<2) | GET_FRONT_BITS(b2,4);
                q[1] = (GET_BACK_BITS(b2,4)<<4) | GET_FRONT_BITS(b3,6);
            }
            else
            {
                dwSize += 3;
                if ( *pdwSize < dwSize )
                {
                    return JSTR_ERROR_NOT_ENOUGH;
                }

                q[0] = (GET_BACK_BITS(b1,6)<<2) | GET_FRONT_BITS(b2,4);
                q[1] = (GET_BACK_BITS(b2,4)<<4) | GET_FRONT_BITS(b3,6);
                q[2] = (GET_BACK_BITS(b3,2)<<6) | b4;

                *pdwSize += 3;
            }
        }
    }

    *pdwSize = dwSize;
    return JSTR_ERROR_OK;
}


EFileShareSubType GetFileShareSubType( const char * szSubType )
{
    if ( 0 == strcmp("start", szSubType) )
    {
        return FILE_SHARE_SUB_TYPE_START;
    }
    else if ( 0 == strcmp("end", szSubType) )
    {
        return FILE_SHARE_SUB_TYPE_END;
    }
    else if ( 0 == strcmp("cancel", szSubType) )
    {
        return FILE_SHARE_SUB_TYPE_CANCEL;
    }
    else if ( 0 == strcmp("fail", szSubType) )
    {
        return FILE_SHARE_SUB_TYPE_FAIL;
    }
    else if ( 0 == strcmp("transfer", szSubType) )
    {
        return FILE_SHARE_SUB_TYPE_TRANSFER;
    }
    else
    {
        assert( 0 );
        return (EFileShareSubType)-1;
    }
}

void  LibjingleLogError( const char * szFileName, unsigned long dwLine, const char * szFormat, ... )
{
    if ( 0 == g_pOnLogMessage )
    {
        return;
    }

    char buf[MAX_BUFF_SIZE] = {0};
    char * p = buf;
    SNPRINTF( buf, MAX_BUFF_SIZE, "file: %s, line: %u, error: ", szFileName, dwLine );

    unsigned long dwLen = strlen( buf );
    p = buf + dwLen;

    va_list  ap;
    va_start( ap, szFormat );
    vsnprintf( p, MAX_BUFF_SIZE - dwLen, szFormat, ap );
    va_end( ap );

    g_pOnLogMessage( buf );
}


unsigned long  GetCurTick()
{
#ifdef WIN32
    return GetTickCount();
#else
    struct tms t;
    memset( &t, 0, sizeof(t) );

    unsigned long ticks = (unsigned long) times( &t );
    unsigned long HZ    = sysconf( _SC_CLK_TCK );
    return ticks * ( 1000 / HZ  );
#endif
}



void  SavePersistentRoom( CLibJinglexApp * pAppObj )
{
    // ���û�иı��û��ȷ����һ�ε����й̶�������
    if ( !(pAppObj->m_bPersistentRoomsChanged && pAppObj->m_bPrivatePersistentRoomsCntConfirmed) )
    {
        MYLOG("==== [ no need to save the private chatroom data ] \n");
        return;
    }

    buzz::XmppClient * client = pAppObj->m_pXmppThread->client();
    if ( 0 == client )
    {
        return;
    }


    vPersistentRoom & v = pAppObj->m_vPersistentRooms;
    vPersistentRoom::iterator it;

    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

    // iq����type����
    iq->SetAttr(buzz::QN_TYPE,buzz::STR_SET);

    // iq����id����
    char buf[MAX_BUFF_SIZE];
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->SetAttr(buzz::QN_ID, randid);

    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
    iq->AddElement( query );

    buzz::XmlElement * room = new buzz::XmlElement(buzz::QN_PERSISTENT_ROOM, true);
    query->AddElement( room );

    // ���item
    for ( it = v.begin(); it != v.end(); it++ )
    {
        PTPersistentRoom p = *it;

        // ��������QN_DMS_ITEM
        buzz::XmlElement * item = new buzz::XmlElement(buzz::QN_DMS_ITEM, true);
        room->AddElement(item);

        item->SetAttr( buzz::QN_ROOM_ID,           AnsiToUtf8( p->m_szRoomId, buf, MAX_BUFF_SIZE )  );
        item->SetAttr( buzz::QN_ROOM_MUC_SERVICE,  AnsiToUtf8( p->m_szGroupChatServiceName, buf, MAX_BUFF_SIZE )  );
    }

    client->SendStanza( iq );
    delete iq;

    pAppObj->m_bPersistentRoomsChanged = FALSE;
    
}

unsigned long  GetUnavailablePersistentRoomCnt( vPersistentRoom & v )
{
    unsigned long dwCnt = 0;
    vPersistentRoom::iterator it;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        PTPersistentRoom  pPersistent = *it;

        if ( pPersistent->m_bNotFound )
        {
            dwCnt++;
        }
    }

    return dwCnt;
}

/******************************************************/
// ���ܣ��ж��ַ��Ƿ�հ��ַ�(SPACE,TAB,\r,\n)       
// ����ֵ��0   ����                                     
//         ��0 ����                                    
/******************************************************/
static BOOL _IsBlankChar( char ch )
{
    if ( ' ' == ch || '\t' == ch || '\r' == ch || '\n' == ch )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/****************************************************************************/
// ���ܣ�  ��ȥ�ַ���ǰ��Ŀհ��ַ�
// ����ֵ��0����Ч�Ĳ���
//         ��0���ü�����ַ���
/****************************************************************************/
char * strtrim( char * szStr )
{
    if ( NULL == szStr )
    {
        return NULL;
    }

    unsigned long dwLen = strlen( szStr );

    if ( 0 == dwLen )
    {
        return szStr;
    }

    unsigned long i, j;
    for ( i = 0; i < dwLen; i++ )
    {
        if ( !_IsBlankChar(szStr[i]) )
        {
            break;
        }
    }

    for ( j = dwLen - 1; j >= i; j-- )
    {
        if ( !_IsBlankChar(szStr[j]) )
        {
            break;
        }
    }

    unsigned long dwNewLen = j - i + 1;
    if ( dwNewLen == dwLen )
    {
        return szStr;
    }

    if ( 0 == dwNewLen )
    {
        szStr[0] = '\0';
    }
    else
    {
        memmove( szStr, szStr + i, dwNewLen );
        szStr[dwNewLen] = '\0';
    }

    return szStr;
}




static JLockType             s_lock;
static BOOL                  s_bInited        = FALSE;
static buzz::XmlElement *    s_rootElement    = 0;

static void XMLCALL
startElement(void *data, const char *el, const char **attr)
{
    buzz::XmlElement * child = new buzz::XmlElement( buzz::QName(true, "", el) );

    assert( s_rootElement );
    s_rootElement->AddElement( child );

    int i;
    for (i = 0; attr[i]; i += 2)
    {
        child->SetAttr( buzz::QName(true, "", attr[i] ), attr[i+1] );
    }

    s_rootElement = child;
}

static void XMLCALL
endElement(void *data, const char *el)
{
    assert( s_rootElement );
    s_rootElement = s_rootElement->Parent();
}

static void XMLCALL
getBodyText(void *userData,
            const XML_Char *s,
            int len)
{
    assert( s_rootElement );

    char buf[MAX_BUFF_SIZE] = { 0 };
    memcpy( buf, s, len );

    strtrim( buf );

    // CAUSION: ֻ��ӷǿհ��ַ���ΪBodyText
    if ( '\0' != buf[0] )
    {
        s_rootElement->SetBodyText( buf );
    }
}

BOOL     ParseXmlBuf( buzz::XmlElement * root, const char * szXmlBuf, unsigned long dwSize )
{
    if ( !s_bInited )
    {
        JInitLock( &s_lock );
        s_bInited = TRUE;
    }

    assert( root && szXmlBuf && dwSize > 0 );

    class cFuncLock  cLock( &s_lock );

    XML_Parser p = XML_ParserCreate( 0 );
    if ( 0 == p )
    {
        fprintf( stderr, "Couldn't allocate memory for parser\n" );
        return FALSE;
    }

    s_rootElement = root;

    XML_SetElementHandler( p, startElement, endElement );
    XML_SetCharacterDataHandler( p, getBodyText );

    if (XML_Parse(p, szXmlBuf, dwSize, TRUE) == XML_STATUS_ERROR) {
        fprintf(stderr, "Parse error at line %lu:\n%s\n",
            XML_GetCurrentLineNumber(p),
            XML_ErrorString(XML_GetErrorCode(p)));
        XML_ParserFree(p);
        return FALSE;
    }

    XML_ParserFree(p);
    return TRUE;
}

void  PrintXmlElement( const buzz::XmlElement * root, int nDepth, std::string & s )
{
    assert( root );

    int i;
    for ( i = 0; i < nDepth; i++ )
    {
        s += "  ";
    }

    char buf[1024];

    // ������ı�
    if ( root->IsText() )
    {    
        SNPRINTF( buf, 1024, "[%s]\n", root->AsText()->Text().c_str() );
        s += buf;
    }
    else
    {
        if ( root->Name().Namespace().length() > 0 )
        {
            // ��ʱֻ��ӡ�����ռ�(����ӡ����)
            SNPRINTF( buf, 1024, "<%s ns='%s'", root->Name().LocalPart().c_str(), root->Name().Namespace().c_str() );
            s += buf;
        }
        else
        {
            // ��ʱֻ��ӡ�����ռ�(����ӡ����)
            SNPRINTF( buf, 1024, "<%s", root->Name().LocalPart().c_str() );
            s += buf;
        }

        const buzz::XmlAttr * attr = root->FirstAttr();
        while( attr )
        {
            SNPRINTF( buf, 1024, " %s=\"%s\"", attr->Name().LocalPart().c_str(), attr->Value().c_str() );
            s += buf;
            attr = attr->NextAttr();
        }

        SNPRINTF( buf, 1024, ">\n" );
        s += buf;


        const buzz::XmlElement * child = root->FirstElement();
        if ( child )
        {
            PrintXmlElement( child, nDepth + 1, s );
        }
        else if ( root->BodyText().length() > 0 )
        {
            for ( i = 0; i < nDepth; i++ )
            {
                s += "  ";
            }

            s +=  root->BodyText();
            s += "\n";
        }

        for ( i = 0; i < nDepth; i++ )
        {
            SNPRINTF( buf, 1024, "  " );
            s += buf;
        }
        SNPRINTF( buf, 1024, "</%s>\n", root->Name().LocalPart().c_str() );
        s += buf;

        const buzz::XmlElement * next = root->NextElement();
        if ( next )
        {
            PrintXmlElement( next, nDepth, s );
        }
    }
}

void     PrintXmlElement( const buzz::XmlElement * root )
{
    assert( root );

    std::string s;

    PrintXmlElement( root, 0, s );

    MYLOG ( "%s", s.c_str() );
}


LIBJINGLE_API  void  LibJingleEx::SetTempPath( const char * szTempPath )
{
    CGlobalData * pGlobal = CGlobalData::GetInstance();

    strncpy( pGlobal->m_szTmpPath, szTempPath, MAX_TMP_PATH_LEN );
}


LIBJINGLE_API  BOOL LibJingleEx::test(HANDLE hApp)
{
    //buzz::XmlElement * root = ParseXmlFile( "E:\\KDVP_daily\\b0-thirdparty\\libjingle-0.4.0\\talk\\MyTalk\\test.xml" );
    //if ( root )
    //{
    //    PrintXmlElement(root);

    //    if ( 0 != WriteXmlFile( root, "E:\\KDVP_daily\\b0-thirdparty\\libjingle-0.4.0\\talk\\MyTalk\\test1.xml" ) )
    //    {
    //        int a = 100;
    //    }

    //    delete root;
    //}

    //talk_base::Pathname   folder;
    //folder.SetFolder("d:\\test1\\test1");
    //talk_base::Filesystem::CreateFolder( folder );

    char buf[1024];
    MyGetVersion( buf, 1024, TERMINAL_VERSION );
    printf( "version = %s \n", buf );

    return TRUE;
}

LIBJINGLE_API  const char * LibJingleEx::GetCompileTime( )
{
    CGlobalData * pGlobal = CGlobalData::GetInstance();

    return  pGlobal->m_szCompileTime;
}

LIBJINGLE_API  const char * LibJingleEx::GetVersion( )
{
    return   _LIBJINGLE_VERSION_;
}


// ��ȡ�洢dms��Ա��˽�����ݿ������(�����ռ�)
char *   GetDmsMembersBlockName( char * szBuf, unsigned long dwSize, unsigned long dwIndex )
{
    if ( 0 == dwIndex )
    {
        strncpy( szBuf, buzz::NS_KEDACOM_DMS_MEMBER.c_str(), dwSize );
    }
    else
    {
        SNPRINTF( szBuf, dwSize, "%s%d", buzz::NS_KEDACOM_DMS_MEMBER.c_str(), dwIndex );
    }
    return szBuf;
}



// ���������������ռ��ȡindex
unsigned long GetDmsMembersBlockIndex( const char * szNamespace )
{
    unsigned long dwLen = strlen( szNamespace );
    assert( dwLen >= buzz::NS_KEDACOM_DMS_MEMBER.length() );

    if ( dwLen == buzz::NS_KEDACOM_DMS_MEMBER.length() )
    {
        return 0;
    }
    else
    {
        unsigned long dwIndex = (unsigned long)-1;
        int nRet = sscanf( szNamespace + buzz::NS_KEDACOM_DMS_MEMBER.length(), "%u", &dwIndex );
        assert( 1 == nRet );
        return dwIndex;
    }
}

unsigned long  GetPresenceWeight( LibJingleEx::EPresence e )
{
    unsigned long dwRet = 0;

    // ����dnd��online��Ȩ��
    if ( LibJingleEx::PRESENCE_DND == e )
    {
        dwRet = 5;
    }
    else if ( LibJingleEx::PRESENCE_ONLINE == e )
    {
        dwRet = 4;
    }
    else
    {
        dwRet = (unsigned long)e;
    }

    return dwRet;
}

LibJingleEx::EPresence  GetPresenceFromWeight( unsigned long w )
{
    if ( 5 == w )
    {
        return LibJingleEx::PRESENCE_DND;
    }
    else if ( 4 == w )
    {
        return LibJingleEx::PRESENCE_ONLINE;
    }
    else
    {
        return (LibJingleEx::EPresence)w;
    }
}


bool
IsXmlSpace(int ch) {
    return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t';
}

bool
ListContainsToken(const std::string & list, const std::string & token) {
    size_t i = list.find(token);
    if (i == std::string::npos || token.empty())
        return false;
    bool boundary_before = (i == 0 || IsXmlSpace(list[i - 1]));
    bool boundary_after = (i == list.length() - token.length() || IsXmlSpace(list[i + token.length()]));
    return boundary_before && boundary_after;
}


bool IsIgnoredResource( const std::string & resource )
{
    size_t pos = resource.find( IGNORE_PRESENCE_PREFIX );
    if ( pos == std::string::npos )
    {
        return false;
    }
    else
    {
        return true;
    }
}

FindRoomRosterItem::FindRoomRosterItem( const LibJingleEx::PTJid  ptBareJid )
{
    memcpy( &m_tJid, ptBareJid, sizeof(LibJingleEx::TJid) );
    m_tJid.sResource[0] = '\0';
}


bool FindRoomRosterItem::operator ()(LibJingleEx::PTRoomRosterItem pItem)
{
    if ( IfJidEqual( pItem->m_tJid, m_tJid ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


FindBareJidObj::FindBareJidObj( const LibJingleEx::PTJid  ptBareJid )
{
    memcpy( &m_tJid, ptBareJid, sizeof(LibJingleEx::TJid) );
    m_tJid.sResource[0] = '\0';
}

bool FindBareJidObj::operator ()(LibJingleEx::PTJid  ptJid)
{
    if( IfBareJidEqual( *ptJid, m_tJid ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


FindFullJidObj::FindFullJidObj( const LibJingleEx::PTJid  ptJid )
{
    memcpy( &m_tJid, ptJid, sizeof(LibJingleEx::TJid) );
}

FindFullJidObj::FindFullJidObj( const std::string & jid )
{
    LibJingleEx::StrJid2TJid( jid.c_str(), m_tJid );
}

bool FindFullJidObj::operator ()(LibJingleEx::PTJid  ptJid)
{
    if( IfJidEqual( *ptJid, m_tJid ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}


FindDeclineItemObj::FindDeclineItemObj(PTDeclineItem pItem)
{
    memcpy( &m_tItem, pItem, sizeof(TDeclineItem) );
}

bool FindDeclineItemObj::operator() ( PTDeclineItem p )
{
    if (    (0 == strcmp(m_tItem.szRoomId, p->szRoomId)) && (0 == strcmp(m_tItem.szService, p->szService)) 
        && IfBareJidEqual(m_tItem.tDeclineJid, p->tDeclineJid) )
    {
        return true;
    }
    else
    {
        return false;
    }
}



LibJingleEx::EAffiliation StrAffi2Int( const char * strAffi )
{
    if ( 0 == strcmp( strAffi, "owner" ) )
    {
        return LibJingleEx::AFFILIATION_OWNER;
    }
    else if ( 0 == strcmp( strAffi, "none" ) )
    {
        return LibJingleEx::AFFILIATION_NONE;
    }
    else if ( 0 == strcmp( strAffi, "member" ) )
    {
        return LibJingleEx::AFFILIATION_MEMBER;
    }
    else if ( 0 == strcmp( strAffi, "admin" ) )
    {
        return LibJingleEx::AFFILIATION_ADMIN;
    }
    else
    {
        return LibJingleEx::AFFILIATION_UNKNOWN;
    }
}

LibJingleEx::EPresence StrShow2Int( const char * show )
{
    if ( 0 == strcmp( show, "away" ) )
    {
        return LibJingleEx::PRESENCE_AWAY;
    }
    else if ( 0 == strcmp( show, "chat" ) )
    {
        return LibJingleEx::PRESENCE_CHAT;
    }
    else if ( 0 == strcmp( show, "dnd" ) )
    {
        return LibJingleEx::PRESENCE_DND;
    }
    else if ( 0 == strcmp( show, "xa" ) )
    {
        return LibJingleEx::PRESENCE_XA;
    }
    else if ( 0 == strcmp( show, "offline" ) )
    {
        return LibJingleEx::PRESENCE_OFFLINE;
    }
    else
    {
        return LibJingleEx::PRESENCE_NONE;
    }
}


void GetRoomRoster( void * c, PTRoom pRoom )
{
    buzz::XmppClient * client = (buzz::XmppClient *)c;

    std::string sTmp = pRoom->m_szRoomId;
    sTmp += "@";
    sTmp += pRoom->m_szGroupChatServiceName;                    

    buzz::XmlElement * iq_admins = new buzz::XmlElement( buzz::QN_IQ );

    // iq����type����
    iq_admins->SetAttr(buzz::QN_TYPE,buzz::STR_GET);

    // iq����id����
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq_admins->SetAttr(buzz::QN_ID, randid);

    iq_admins->SetAttr( buzz::QN_TO, sTmp );

    // ���query��Ԫ��
    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_MUC_ADMIN_QUERY, true);
    iq_admins->AddElement( query );

    // ���item��Ԫ��
    buzz::XmlElement * item = new buzz::XmlElement(buzz::QN_ITEM, true);
    query->AddElement( item );

    // item����affiliation����
    item->SetAttr( buzz::QN_MUC_AFFILIATION, "admin" );      



    // ���֧��member�Ļ�
#if CHATROOM_SUPPORT_MEMBER_AFFILIATION

    buzz::XmlElement * iq_members = new buzz::XmlElement( *iq_admins );
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq_members->SetAttr(buzz::QN_ID, randid);

    item = iq_members->FirstElement()->FirstElement();
    item->SetAttr( buzz::QN_MUC_AFFILIATION, "member" );

#endif




    buzz::XmlElement * iq_owners = new buzz::XmlElement( *iq_admins );
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq_owners->SetAttr(buzz::QN_ID, randid);

    // �������һ������roster items����¼��
    strcpy( pRoom->m_szLastRosterQueryId, randid );

    item = iq_owners->FirstElement()->FirstElement();
    item->SetAttr( buzz::QN_MUC_AFFILIATION, "owner" );


    // �����Ա��������
    if ( client )
    {
#if CHATROOM_SUPPORT_MEMBER_AFFILIATION
        client->SendStanza( iq_members );
#endif
        client->SendStanza( iq_admins );
        client->SendStanza( iq_owners );
    }

#if CHATROOM_SUPPORT_MEMBER_AFFILIATION
    delete iq_members;
#endif
    delete iq_admins;
    delete iq_owners;

    ClearVector( pRoom->m_vTmpRoster );
}


void ClearVector( vRoomMember & v )
{
    vRoomMember::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( vRoomRoster & v )
{
    vRoomRoster::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( std::vector<LibJingleEx::PTJid> & v )
{
    std::vector<LibJingleEx::PTJid>::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( std::vector< TIqId * > & v )
{
    std::vector< TIqId * >::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( std::vector< cPicSession * > & v )
{
    std::vector< cPicSession * >::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( std::vector<PTDeclineItem> & v )
{
    std::vector< PTDeclineItem >::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( vQueryIdsList & v )
{
    vQueryIdsList::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete[] *it;
    }
    v.clear();
}

void ClearVector( vPersistentRoom & v )
{
    vPersistentRoom::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( vChatRoomIds & v )
{
    vChatRoomIds::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( vCachedGrouchatMsgs & v )
{
    vCachedGrouchatMsgs::iterator  it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( std::vector<PTUnscribedContact> & v )
{
    std::vector<PTUnscribedContact>::iterator  it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void ClearVector( std::vector<PTRoomChatMsgItem> & v )
{
    std::vector<PTRoomChatMsgItem>::iterator  it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

void  ClearVector( std::vector<TSendRoomPictureParam *> & v )
{
    std::vector<TSendRoomPictureParam *>::iterator  it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete *it;
    }
    v.clear();
}

FindUnscribedContactObj::FindUnscribedContactObj( const LibJingleEx::TJid * ptJid )
{
    memcpy( &tJid,  ptJid,  sizeof(LibJingleEx::TJid) );
}

bool FindUnscribedContactObj::operator() ( PTUnscribedContact p)
{
    if ( IfJidEqual( p->tJid, tJid) )
    {
        return true;
    }
    else
    {
        return false;
    }
}



// ����ֵ��TRUE�� ������һ�������� FALSE�� û������
BOOL GetAbilityString( std::string & ability, CLibJinglexApp * app )
{
    if ( app->m_bFileShareAvailable || app->m_bAudio || app->m_bVideo || app->m_bCall )
    {
        if ( app->m_bFileShareAvailable )
        {
            if ( ability.length() > 0 )
            {
                ability += " ";
            }
            ability += "share-v1";
        }

        if ( app->m_bAudio )
        {
            if ( ability.length() > 0 )
            {
                ability += " ";
            }
            ability += "audio";
        }

        if ( app->m_bVideo )
        {
            if ( ability.length() > 0 )
            {
                ability += " ";
            }
            ability += "video";
        }

        if ( app->m_bCall )
        {
            if ( ability.length() > 0 )
            {
                ability += " ";
            }

            ability += "call";
        }

        return TRUE;
    }
    else
    {
        return TRUE;
    }
}

const char * GetPresShowString( LibJingleEx::EPresence e )
{
    const char * show = 0;

    switch( e )
    {
    case LibJingleEx::PRESENCE_XA:
        show = "xa";
        break;

    case LibJingleEx::PRESENCE_AWAY:
        show = "away";
        break;

    case LibJingleEx::PRESENCE_DND:
        show = "dnd";
        break;

    case LibJingleEx::PRESENCE_CHAT:
        show = "chat";
        break;

    default:
        break;
    }

    return show;
}

// �����·������ȡ�ļ���
char * GetFileName( char * szDest, unsigned long dwSize, const char * szFilePath )
{
    const char * pSlash = 0;
#ifdef WIN32
    pSlash = strrchr( szFilePath, '\\' );
#else
    pSlash = strrchr( szFilePath, '/' );
#endif

    if ( 0 == pSlash )
    {
        SNPRINTF( szDest, dwSize, szFilePath );
    }
    else
    {
        SNPRINTF( szDest, dwSize, pSlash + 1 );
    }

    return szDest;
}


FindRoomPicSessionObj::FindRoomPicSessionObj( const char * szNick, const char * szSid )
{
    STRNCPY( m_szNick, szNick, sizeof(m_szNick) );
    STRNCPY( m_szSid,  szSid,  sizeof(m_szSid) );
}

bool FindRoomPicSessionObj::operator () ( cPicSession * p )
{
    if ( 0 == strcmp( p->m_szNick, m_szNick ) && 0 == strcmp( p->m_szSid, m_szSid ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void GetMemberJidFromChatRoomResource( PTRoom pRoom, const char * szResource, LibJingleEx::PTJid  ptMemberJid )
{
    vRoomMember & v = pRoom->m_vMembers;
    vRoomMember::iterator it_m = find_if( v.begin(), v.end(), FindRoomMemberByNickObj(szResource) );

    PTRoomMember pMember = 0;

    // û���ҵ�������
    if ( it_m == v.end() )
    {
        StrJid2TJid( szResource, *ptMemberJid );
    }
    else
    {
        PTRoomMember ptMember = *it_m;
        memcpy( ptMemberJid, &ptMember->m_tJid, sizeof(LibJingleEx::TJid) );
    }
}


void DeleteChatroomMember( PTRoom pRoom,  LibJingleEx::PTJid  ptMemberJid, void * pClient )
{
    char buf[MAX_BUFF_SIZE];
    char szId[MAX_RAND_ID_LENGTH];

    buzz::XmppClient * client = ( buzz::XmppClient * )pClient;

    // �����߳���stanza
    std::string sTemplate = "<iq id='%s' to='%s' type='set'>"
        "<query xmlns='http://jabber.org/protocol/muc#admin'>"
        "<item affiliation='none' jid='%s' >"
        "</item>"
        "</query>"
        "</iq>";

    std::string  room_id;
    room_id += pRoom->m_szRoomId;
    room_id += "@";
    room_id += pRoom->m_szGroupChatServiceName;

    std::string member_jid;
    member_jid  = ptMemberJid->sNode;
    member_jid += "@";
    member_jid += ptMemberJid->sDomain;


    SNPRINTF( buf, MAX_BUFF_SIZE, sTemplate.c_str(), GenerateRandomId( szId,MAX_RAND_ID_LENGTH), room_id.c_str(), member_jid.c_str() );
    client->SendRaw( buf );


    // ����㲥ɾ���˳�Ա
    buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );

    GenerateRandomId( szId, MAX_RAND_ID_LENGTH );
    message->SetAttr( buzz::QN_ID, szId );
    message->SetAttr( buzz::QN_TO, room_id );
    message->SetAttr( buzz::QN_TYPE, "groupchat" );

    buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
    message->AddElement( operation );
    operation->SetAttr( buzz::QN_TYPE, "chatroom delete member" );

    buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ITEM );
    operation->AddElement( item );

    item->SetAttr( buzz::QN_JID, member_jid );

    client->SendStanza( message );
    // END( ����㲥ɾ���˳�Ա )

    delete message;
}






buzz::XmlElement *  ConstructChatBody( const LibJingleEx::TDmsChatContent * ptMsg, CLibJinglexApp * pAppObj, unsigned long * pdwSendTime )
{
    std::string str;
    char buf[MAX_BUFF_SIZE];
    unsigned long buf_size = MAX_BUFF_SIZE;
    int ret = 0;

    buzz::XmlElement * body = new buzz::XmlElement(buzz::QN_XBODY, true);
    body->SetAttr( buzz::QN_VER, CHAT_MESSAGE_VERSION );


    // FONT

    // �����ǰ�汾��1.1���ǵ�1.0�汾��font
    if ( 0 == strcmp( CHAT_MESSAGE_VERSION, "1.1" ) )
    {
        buzz::XmlElement * font = new buzz::XmlElement(buzz::QN_DMS_FONT);
        body->AddElement( font );

        unsigned char abyFontBuf[LibJingleEx::MAX_DMS_FONT_BUF_SIZE];
        memset( abyFontBuf, 0, sizeof(abyFontBuf) );

        unsigned long dwMin = MINIMIZATION( sizeof(abyFontBuf), sizeof(LibJingleEx::TDmsFont) );
        memcpy( abyFontBuf, &ptMsg->m_tFont, dwMin );

        buf_size = MAX_BUFF_SIZE;
        ret = LibJingleEx::EncodeBase64( buf, buf_size, abyFontBuf, sizeof(abyFontBuf) );
        assert( 0 == ret );
        font->SetBodyText( buf );
    }


    buzz::XmlElement * font_1 = new buzz::XmlElement(buzz::QN_DMS_FONT_1);
    body->AddElement( font_1 );

    // font-height
    buzz::XmlElement * font_height = new buzz::XmlElement( buzz::QN_DMS_FONT_HEIGHT);
    font_1->AddElement( font_height );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%ld", ptMsg->m_tFont.lfHeight );
    font_height->SetBodyText( buf );

    // font-width
    buzz::XmlElement * font_width = new buzz::XmlElement( buzz::QN_DMS_FONT_WIDTH);
    font_1->AddElement( font_width );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%ld", ptMsg->m_tFont.lfWidth );
    font_width->SetBodyText( buf );

    // font-escape
    buzz::XmlElement * font_escape = new buzz::XmlElement( buzz::QN_DMS_FONT_ESCAPE);
    font_1->AddElement( font_escape );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%ld", ptMsg->m_tFont.lfEscapement );
    font_escape->SetBodyText( buf );

    // font-orientation
    buzz::XmlElement * font_orientation = new buzz::XmlElement( buzz::QN_DMS_FONT_ORIENTATION);
    font_1->AddElement( font_orientation );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%ld", ptMsg->m_tFont.lfOrientation );
    font_orientation->SetBodyText( buf );

    // font-weight
    buzz::XmlElement * font_weight = new buzz::XmlElement( buzz::QN_DMS_FONT_WEIGHT);
    font_1->AddElement( font_weight );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%ld", ptMsg->m_tFont.lfWeight );
    font_weight->SetBodyText( buf );

    // font-italic
    buzz::XmlElement * font_italic = new buzz::XmlElement( buzz::QN_DMS_FONT_ITALIC);
    font_1->AddElement( font_italic );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%s", ptMsg->m_tFont.lfItalic ? "true" : "false" );
    font_italic->SetBodyText( buf );

    // font-underline
    buzz::XmlElement * font_underline = new buzz::XmlElement( buzz::QN_DMS_FONT_UNDERLINE);
    font_1->AddElement( font_underline );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%s", ptMsg->m_tFont.lfUnderline ? "true" : "false" );
    font_underline->SetBodyText( buf );

    // font-strikeout
    buzz::XmlElement * font_strikeout = new buzz::XmlElement( buzz::QN_DMS_FONT_STRIKE_OUT);
    font_1->AddElement( font_strikeout );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%s", ptMsg->m_tFont.lfStrikeOut ? "true" : "false" );
    font_strikeout->SetBodyText( buf );


    // font-charset
    buzz::XmlElement * font_charset = new buzz::XmlElement( buzz::QN_DMS_FONT_CHARSET);
    font_1->AddElement( font_charset );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%u", ptMsg->m_tFont.lfCharSet );
    font_charset->SetBodyText( buf );

    // font-out-precision
    buzz::XmlElement * font_out_precision = new buzz::XmlElement( buzz::QN_DMS_FONT_OUT_PRECISION );
    font_1->AddElement( font_out_precision );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%u", ptMsg->m_tFont.lfOutPrecision );
    font_out_precision->SetBodyText( buf );

    // font-clip_precisino
    buzz::XmlElement * font_clip_precision = new buzz::XmlElement( buzz::QN_DMS_FONT_CLIP_PRECISION);
    font_1->AddElement( font_clip_precision );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%u", ptMsg->m_tFont.lfClipPrecision );
    font_clip_precision->SetBodyText( buf );

    // font-quality
    buzz::XmlElement * font_quality = new buzz::XmlElement( buzz::QN_DMS_FONT_QUALITY);
    font_1->AddElement( font_quality );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%u", ptMsg->m_tFont.lfQuality );
    font_quality->SetBodyText( buf );

    // font-pitch-and-family
    buzz::XmlElement * font_pitch = new buzz::XmlElement( buzz::QN_DMS_FONT_PITCH_AND_FAMILY);
    font_1->AddElement( font_pitch );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%u", ptMsg->m_tFont.lfPitchAndFamily );
    font_pitch->SetBodyText( buf );

    // FONT-FACE
    buzz::XmlElement * font_face = new buzz::XmlElement( buzz::QN_DMS_FONT_FACE_NAME );
    font_1->AddElement( font_face );
    font_face->SetBodyText( AnsiToUtf8( ptMsg->m_tFont.lfFaceName, buf, MAX_BUFF_SIZE ) );

    // END FONT


    // color
    buzz::XmlElement * color = new buzz::XmlElement(buzz::QN_DMS_COLOR);
    body->AddElement( color );

    SNPRINTF( buf, MAX_BUFF_SIZE, "%u", ptMsg->m_dwColor );
    color->SetBodyText( buf );

    
    // time
    time_t curTime;
    time( &curTime );
    // ���Ϸ������ͱ���ʱ��Ĳ�ֵ
    DWORD adjuctedTime = (DWORD)curTime + pAppObj->m_dwTimeDiff;

    if ( 0 == strcmp( CHAT_MESSAGE_VERSION, "1.1" ) )
    {
        buzz::XmlElement * dms_time = new buzz::XmlElement(buzz::QN_DMS_TIME);
        body->AddElement( dms_time );

        unsigned char abyTimeBuf[LibJingleEx::MAX_DMS_TIME_BUF_SIZE];
        memset( abyTimeBuf, 0, sizeof(abyTimeBuf) );

        unsigned long dwMin = MINIMIZATION( LibJingleEx::MAX_DMS_TIME_BUF_SIZE, sizeof(unsigned long) );
        // memcpy( abyTimeBuf, &ptMsg->m_dwTime, dwMin );
        memcpy( abyTimeBuf, &adjuctedTime, dwMin );

        buf_size = MAX_BUFF_SIZE;
        ret = LibJingleEx::EncodeBase64( buf, buf_size, abyTimeBuf, sizeof(abyTimeBuf) );
        assert( 0 == ret );
        dms_time->SetBodyText( buf );
    }


    buzz::XmlElement * dms_time_1 = new buzz::XmlElement(buzz::QN_DMS_TIME_1);
    body->AddElement( dms_time_1 );

    char szTime[32] = { 0 };
    // time_t  tmpTime = ptMsg->m_dwTime;
    time_t tmpTime = (time_t)adjuctedTime;

    strftime( szTime, 32, "%Y-%m-%d %H:%M:%S", gmtime( &tmpTime ) );
    dms_time_1->SetBodyText( szTime );

    if ( pdwSendTime )
    {
        *pdwSendTime = adjuctedTime;
    }

    // END time

    // HEADER
    buzz::XmlElement * header = new buzz::XmlElement( buzz::QN_DMS_HEADER);
    body->AddElement( header );

    header->SetBodyText( AnsiToUtf8( ptMsg->m_szMsgHeader, buf, MAX_BUFF_SIZE ) );

    // content
    buzz::XmlElement * content = new buzz::XmlElement( buzz::QN_DMS_CONTENT);
    body->AddElement( content );

#ifdef  SUPPORT_UTF8
    content->SetBodyText( ptMsg->m_szContent );
#else
    content->SetBodyText( AnsiToUtf8( ptMsg->m_szContent, buf, MAX_BUFF_SIZE ) );
#endif

    // chat type
    buzz::XmlElement * chat_type = new buzz::XmlElement(buzz::QN_DMS_CHAT_TYPE);
    body->AddElement( chat_type );

    SNPRINTF( buf, MAX_BUFF_SIZE, "%u", ptMsg->m_dwChatType );
    chat_type->SetBodyText( buf );

    // pic count
    buzz::XmlElement * pic_count = new buzz::XmlElement(buzz::QN_DMS_PIC_COUNT);
    body->AddElement( pic_count );

    SNPRINTF( buf, MAX_BUFF_SIZE, "%u", ptMsg->m_dwPicCount );
    pic_count->SetBodyText( buf );

    // pics
    unsigned long i;
    for ( i = 0; i < LibJingleEx::MAX_DMS_SEND_PIC_COUNT; i++ )
    {
        buzz::XmlElement * pic = new buzz::XmlElement(buzz::QN_DMS_PIC);
        body->AddElement( pic );

        SNPRINTF( buf, MAX_BUFF_SIZE, "%u", i );
        pic->SetAttr( buzz::QN_ID, buf );


        buzz::XmlElement * pic_index = new buzz::XmlElement(buzz::QN_DMS_PIC_INDEX);
        pic->AddElement(pic_index);

        SNPRINTF( buf, MAX_BUFF_SIZE, "%u", ptMsg->m_atPicFileInfoTable[i].m_dwIndex );
        pic_index->SetBodyText( buf );


        buzz::XmlElement * pic_path = new buzz::XmlElement(buzz::QN_DMS_PIC_PATH);
        pic->AddElement(pic_path);

        pic_path->SetBodyText( AnsiToUtf8( ptMsg->m_atPicFileInfoTable[i].m_achFileName, buf, MAX_BUFF_SIZE ) );
    }

    return body;
}

void ParseChatMessage( const buzz::XmlElement * body, LibJingleEx::TDmsChatContent & tMsg )
{
    const buzz::XmlElement * dms_child = 0;
    dms_child = body->FirstElement();
    const std::string & version = body->Attr( buzz::QN_VER );

    unsigned long pic_i = 0;

    while( dms_child )
    {
        const std::string & dms_body = dms_child->BodyText();
        bool bHandled = false;

        if ( version == "1.0" )
        {
            if ( dms_child->Name() == buzz::QN_DMS_FONT )
            {
                unsigned char abyFontBuf[LibJingleEx::MAX_DMS_FONT_BUF_SIZE];
                memset( abyFontBuf, 0, sizeof(abyFontBuf) );
                unsigned long dwSize = sizeof(abyFontBuf);

                LibJingleEx::DecodeBase64( abyFontBuf, &dwSize, dms_body.c_str() );

                unsigned long dwMin = MINIMIZATION( sizeof(abyFontBuf), sizeof(LibJingleEx::TDmsFont) );
                memcpy( &tMsg.m_tFont, abyFontBuf, dwMin );

                bHandled = true;
            }
            else if ( dms_child->Name() == buzz::QN_DMS_TIME )
            {
                unsigned char abyTimeBuf[LibJingleEx::MAX_DMS_TIME_BUF_SIZE];
                memset( abyTimeBuf, 0, sizeof(abyTimeBuf) );
                unsigned long dwSize = sizeof(abyTimeBuf);


                LibJingleEx::DecodeBase64( abyTimeBuf, &dwSize, dms_body.c_str() );

                unsigned long dwMin = MINIMIZATION( sizeof(abyTimeBuf), sizeof(unsigned long) );
                memcpy( &tMsg.m_dwTime, abyTimeBuf, dwMin );

                bHandled = true;
            }
        }
        else if ( version == "1.1" )
        {
            if ( dms_child->Name() == buzz::QN_DMS_FONT_1 )
            {
                const buzz::XmlElement * grand_child = 0;
                grand_child = dms_child->FirstElement();

                while( grand_child )
                {
                    const std::string & grand_child_body = grand_child->BodyText();

                    if ( grand_child->Name() == buzz::QN_DMS_FONT_HEIGHT )
                    {
                        sscanf( grand_child_body.c_str(), "%ld", &tMsg.m_tFont.lfHeight );
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_WIDTH )
                    {
                        sscanf( grand_child_body.c_str(), "%ld", &tMsg.m_tFont.lfWidth );
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_ESCAPE )
                    {
                        sscanf( grand_child_body.c_str(), "%ld", &tMsg.m_tFont.lfEscapement );
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_ORIENTATION )
                    {
                        sscanf( grand_child_body.c_str(), "%ld", &tMsg.m_tFont.lfOrientation );
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_WEIGHT )
                    {
                        sscanf( grand_child_body.c_str(), "%ld", &tMsg.m_tFont.lfWeight );
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_ITALIC )
                    {
                        tMsg.m_tFont.lfItalic = ( grand_child_body == "true" ? TRUE : FALSE );
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_UNDERLINE )
                    {
                        tMsg.m_tFont.lfUnderline = ( grand_child_body == "true" ? TRUE : FALSE );
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_STRIKE_OUT )
                    {
                        tMsg.m_tFont.lfStrikeOut = ( grand_child_body == "true" ? TRUE : FALSE );
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_CHARSET )
                    {
                        unsigned long dwTmp = 0;
                        sscanf( grand_child_body.c_str(), "%u", &dwTmp );
                        tMsg.m_tFont.lfCharSet = (unsigned char)dwTmp;
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_OUT_PRECISION )
                    {
                        unsigned long dwTmp = 0;
                        sscanf( grand_child_body.c_str(), "%u", &dwTmp );
                        tMsg.m_tFont.lfOutPrecision = (unsigned char)dwTmp;
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_CLIP_PRECISION )
                    {
                        unsigned long dwTmp = 0;
                        sscanf( grand_child_body.c_str(), "%u", &dwTmp );
                        tMsg.m_tFont.lfClipPrecision = (unsigned char)dwTmp;
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_QUALITY )
                    {
                        unsigned long dwTmp = 0;
                        sscanf( grand_child_body.c_str(), "%u", &dwTmp );
                        tMsg.m_tFont.lfQuality = (unsigned char)dwTmp;
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_PITCH_AND_FAMILY )
                    {
                        unsigned long dwTmp = 0;
                        sscanf( grand_child_body.c_str(), "%u", &dwTmp );
                        tMsg.m_tFont.lfPitchAndFamily = (unsigned char)dwTmp;
                    }
                    else if ( grand_child->Name() == buzz::QN_DMS_FONT_FACE_NAME )
                    {
                        Utf8ToAnsi( grand_child_body.c_str(), tMsg.m_tFont.lfFaceName, sizeof(tMsg.m_tFont.lfFaceName) );
                    }

                    grand_child = grand_child->NextElement();
                }

                bHandled = true;
            }
            else if ( dms_child->Name() == buzz::QN_DMS_TIME_1 )
            {
                struct tm  tTime;
                memset( &tTime, 0, sizeof(tTime) );

                sscanf( dms_body.c_str(), "%d-%d-%d %d:%d:%d", &tTime.tm_year, &tTime.tm_mon, &tTime.tm_mday, 
                    &tTime.tm_hour, &tTime.tm_min, &tTime.tm_sec );
                tTime.tm_year -= 1900;
                tTime.tm_mon--;

                time_t tmpTime = mktime( &tTime );
                if ( -1 == tmpTime )
                {
                    tmpTime = 0;
                }
                else
                {
                    tmpTime -= CGlobalData::GetInstance()->m_nBias;
                }

                tMsg.m_dwTime = (unsigned long)tmpTime;

                bHandled = true;
            }
        }

        if ( !bHandled )
        {
            if ( dms_child->Name() == buzz::QN_DMS_COLOR )
            {
                sscanf( dms_body.c_str(), "%u", &tMsg.m_dwColor );
            }
            else if ( dms_child->Name() == buzz::QN_DMS_HEADER )
            {
                Utf8ToAnsi( dms_body.c_str(), tMsg.m_szMsgHeader, sizeof(tMsg.m_szMsgHeader) );
            }
            else if ( dms_child->Name() == buzz::QN_DMS_CONTENT )
            {
#ifdef SUPPORT_UTF8
                strncpy(    tMsg.m_szContent, dms_body.c_str(), sizeof(tMsg.m_szContent) );
#else
                Utf8ToAnsi( dms_body.c_str(), tMsg.m_szContent, sizeof(tMsg.m_szContent) );
#endif
            }
            else if ( dms_child->Name() == buzz::QN_DMS_CHAT_TYPE )
            {
                sscanf( dms_body.c_str(), "%u", &tMsg.m_dwChatType );
            }
            else if ( dms_child->Name() == buzz::QN_DMS_PIC_COUNT )
            {
                sscanf( dms_body.c_str(), "%u", &tMsg.m_dwPicCount );
            }
            else if ( dms_child->Name() == buzz::QN_DMS_PIC )
            {
                LibJingleEx::TDmsChatPicInfo & tPicInfo = tMsg.m_atPicFileInfoTable[pic_i];

                const buzz::XmlElement * pic_index = dms_child->FirstElement();
                assert( pic_index );

                sscanf( pic_index->BodyText().c_str(), "%u", &tPicInfo.m_dwIndex );


                const buzz::XmlElement * pic_path = pic_index->NextElement();
                assert( pic_path );

                Utf8ToAnsi( pic_path->BodyText().c_str(), tPicInfo.m_achFileName, sizeof(tPicInfo.m_achFileName) );

                pic_i++;
            }
        }

        dms_child = dms_child->NextElement();
    }
}


buzz::XmlElement * BuildCostomMsgBody( const LibJingleEx::TCustomMsg * ptCustomeMsg )
{
    char buf[MAX_BUFF_SIZE];

    buzz::XmlElement * body = new buzz::XmlElement( buzz::QN_COSTOM_BODY, true );

    // type
    buzz::XmlElement * custom_type = new buzz::XmlElement( buzz::QN_COSTOM_TYPE );
    body->AddElement( custom_type );
    SNPRINTF( buf, MAX_BUFF_SIZE, "%d", ptCustomeMsg->nMsgType );
    custom_type->SetBodyText( buf );

    // param0-param3
    unsigned long i;
    for ( i = 0; i < LibJingleEx::MAX_CUSTOM_PARAMS_CNT; i++ )
    {
        SNPRINTF( buf, MAX_BUFF_SIZE, "param%d", i );
        buzz::XmlElement * custom_param = new buzz::XmlElement( buzz::QName(buzz::NS_CUSTOM_BODY, buf) );
        body->AddElement( custom_param );
        SNPRINTF( buf, MAX_BUFF_SIZE, "%d", ptCustomeMsg->nParams[i] );
        custom_param->SetBodyText( buf );
    }

    // param-buffer
    buzz::XmlElement * custom_buf = new buzz::XmlElement( buzz::QN_COSTOM_BUF );
    body->AddElement( custom_buf );
    custom_buf->SetBodyText( AnsiToUtf8( ptCustomeMsg->achBuf,  buf, MAX_BUFF_SIZE) );

    return body;
}

void ParseCustomMsg( const buzz::XmlElement * body, LibJingleEx::TCustomMsg & tCustomMsg )
{
    const buzz::XmlElement * child = 0;
    child = body->FirstElement();

    while( child )
    {
        if ( child->Name() == buzz::QN_COSTOM_TYPE )
        {
            sscanf( child->BodyText().c_str(), "%d", &tCustomMsg.nMsgType );
        }
        else if ( 0 == strncmp( child->Name().LocalPart().c_str(), "param", 5 ) )
        {
            int  nIndex = 0;
            sscanf( child->Name().LocalPart().c_str() + 5, "%d", &nIndex );

            if ( nIndex < LibJingleEx::MAX_CUSTOM_PARAMS_CNT )
            {
                sscanf( child->BodyText().c_str(), "%d", &tCustomMsg.nParams[nIndex] );
            }
        }
        else if ( child->Name() == buzz::QN_COSTOM_BUF )
        {
            Utf8ToAnsi( child->BodyText().c_str(), tCustomMsg.achBuf,  LibJingleEx::MAX_CUSTOM_PARAM_BUF_LEN );
        }
        child = child->NextElement();
    }
}

LIBJINGLE_API  LibJingleEx::ELibjingleError  LibJingleEx::TryEnterAllPersistentChatrooms( LibJingleEx::HANDLE hApp )
{
    if ( 0 == hApp )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp *  pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    // û�е�¼
    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LibJingleEx::LIBJINGLE_ERR_NOT_LOGIN;
    }

    buzz::XmppClient * client = pAppObj->m_pXmppThread->client();
    if ( 0 == client->ConferenceJid().length() )
    {
        return LibJingleEx::LIBJINGLE_ERR_NOT_CHATROOM_SERVICE;
    }

    if ( pAppObj->m_bTryEnterAllPersistentRooms )
    {
        return LibJingleEx::LIBJINGLE_ERR_BUSY;
    }

    const char * szFormat = "<iq type='get' id='%s' to='%s'>"
                              "<query xmlns='http://jabber.org/protocol/disco#items'/>"
                            "</iq>";

    char buf[MAX_BUFF_SIZE];
    char szId[MAX_RAND_ID_LENGTH];
    GenerateRandomId( szId, MAX_RAND_ID_LENGTH );

    SNPRINTF( buf, MAX_BUFF_SIZE, szFormat, szId, client->ConferenceJid().c_str() );
    
    pAppObj->m_pXmppThread->SendRaw( buf );

    pAppObj->m_bTryEnterAllPersistentRooms = TRUE;
    
    return LibJingleEx::LIBJINGLE_ERR_OK;
}


LIBJINGLE_API  LibJingleEx::ELibjingleError  LibJingleEx::SendCustomMsg( LibJingleEx::HANDLE hApp, 
                                                            const LibJingleEx::PTJid pJid, 
                                                            const LibJingleEx::TCustomMsg * ptCustomMsg )
{
    if ( 0 == hApp || 0 == pJid || 0 == ptCustomMsg )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    cFuncLock cLock( &pAppObj->m_lock );

    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LibJingleEx::LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( LibJingleEx::STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LibJingleEx::LIBJINGLE_ERR_DISCONNECTED;
    }



    // ����stanza
    std::string str;
    char buf[MAX_BUFF_SIZE];

    buzz::XmlElement * msg  = new buzz::XmlElement(buzz::QN_MESSAGE);
    // ������Ա���Ϊ������Ϣ
    if ( ptCustomMsg->bOffline )
    {
        msg->SetAttr( buzz::QN_TYPE, "custom" );
    }
    else
    {
        msg->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );
    }

    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    msg->SetAttr( buzz::QN_ID,   randid );

    str  = pJid->sNode;
    str += '@';
    str += pJid->sDomain;
    msg->SetAttr( buzz::QN_TO,  AnsiToUtf8( str.c_str(), buf, MAX_BUFF_SIZE ) );



    msg->AddElement( BuildCostomMsgBody( ptCustomMsg ) );  

    pAppObj->m_pXmppThread->SendStanza( msg );

    return LibJingleEx::LIBJINGLE_ERR_OK;
}


LIBJINGLE_API  LibJingleEx::ELibjingleError  LibJingleEx::SetCustomCfg( 
            LibJingleEx::HANDLE hApp, const LibJingleEx::TCustomConfigItem * achCfgItems, unsigned long dwCnt )
{
    if ( 0 == hApp || 0 == achCfgItems || 0 == dwCnt )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    cFuncLock cLock( &pAppObj->m_lock );

    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LibJingleEx::LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( LibJingleEx::STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LibJingleEx::LIBJINGLE_ERR_DISCONNECTED;
    }

    // �����û�л�ȡ˽������
    if ( !pAppObj->m_bConfigAvailable )
    {
        return LibJingleEx::LIBJINGLE_ERR_CONFIG_NOT_READY;
    }


    for ( unsigned long i = 0; i < dwCnt; i++ )
    {
        pAppObj->m_cConfig.SetConfig( achCfgItems[i].szKey,  achCfgItems[i].szValue );
    }




    char randid[MAX_RAND_ID_LENGTH];
    char buf[MAX_BUFF_SIZE];

    // ����
    // iq
    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);
    iq->SetAttr(buzz::QN_TYPE,buzz::STR_SET);
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->SetAttr(buzz::QN_ID, randid);

    // private query
    buzz::XmlElement * query = new buzz::XmlElement(buzz::QN_PRIVATE_QUERY, true);
    iq->AddElement( query );

    // config
    buzz::XmlElement * config = new buzz::XmlElement(buzz::QN_CUSTOM_CONFIG, true );
    query->AddElement( config );


    const cKeyValue * pItem = pAppObj->m_cConfig.GetFirstItem();
    while( pItem )
    {
        // item
        if ( pItem->sValue.length() > 0 )
        {
            buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ITEM, true );
            config->AddElement( item );

            item->SetAttr( buzz::QN_NAME, AnsiToUtf8( pItem->sKey.c_str(), buf, MAX_BUFF_SIZE ) );
            item->SetBodyText( AnsiToUtf8( pItem->sValue.c_str(), buf, MAX_BUFF_SIZE ) );
        }

        pItem = pAppObj->m_cConfig.GetNext();
    }

    pAppObj->m_pXmppThread->SendStanza( iq );



    buzz::XmppClient * client = pAppObj->m_pXmppThread->pump()->client();

    // ֪ͨ�����ն�
    // message
    buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
    message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
    message->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );

    // operation
    buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
    message->AddElement( operation );
    operation->SetAttr( buzz::QN_TYPE, "custom config" );


    for ( unsigned long i = 0; i < dwCnt; i++ )
    {
        // item
        buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_ITEM, true );
        operation->AddElement( item );

        item->SetAttr( buzz::QN_NAME, AnsiToUtf8( achCfgItems[i].szKey, buf, MAX_BUFF_SIZE ) );
        item->SetBodyText( AnsiToUtf8( achCfgItems[i].szValue, buf, MAX_BUFF_SIZE ) );
    }

    pAppObj->m_pXmppThread->SendStanza( message );

    
    return LibJingleEx::LIBJINGLE_ERR_OK;
}


LIBJINGLE_API  LibJingleEx::ELibjingleError  LibJingleEx::GetCustomCfg( 
                                        LibJingleEx::HANDLE hApp, LibJingleEx::TCustomConfigItem * ptItem )
{
    if ( 0 == hApp || 0 == ptItem )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_ARGUMENT;
    }

    // ת��
    CLibJinglexApp * pAppObj = (CLibJinglexApp *)hApp;

    // �����
    CGlobalData * pGlobal = CGlobalData::GetInstance();
    if ( !pGlobal->IfExistApp(pAppObj) )
    {
        return LibJingleEx::LIBJINGLE_ERR_INVALID_HANDLE;
    }
    // END �����

    // �̰߳�ȫ
    cFuncLock cLock_( &pAppObj->m_external_lock );

    cFuncLock cLock( &pAppObj->m_lock );

    if ( 0 == pAppObj->m_pXmppThread )
    {
        return LibJingleEx::LIBJINGLE_ERR_NOT_LOGIN;
    }

    if ( LibJingleEx::STATE_SUBCODE_NONE != pAppObj->m_pXmppThread->GetErrcode() )
    {
        return LibJingleEx::LIBJINGLE_ERR_DISCONNECTED;
    }

    // �����û�л�ȡ˽������
    if ( !pAppObj->m_bConfigAvailable )
    {
        return LibJingleEx::LIBJINGLE_ERR_CONFIG_NOT_READY;
    }

    // ������������������ݣ�����Ҫm_bPrivatePersistentRoomsCntConfirmed ready
    if ( 0 == _STRICMP( ptItem->szKey, LibJingleEx::SCREENED_CHATROOMS_KEY ) )
    {
        if ( !pAppObj->m_bPrivatePersistentRoomsCntConfirmed )
        {
            return LibJingleEx::LIBJINGLE_ERR_CONFIG_NOT_READY;
        }
    }

    strncpy( ptItem->szValue, pAppObj->m_cConfig[ptItem->szKey], sizeof(ptItem->szValue) - 1 );
    ptItem->szValue[LibJingleEx::MAX_CUSTOM_CONFIG_VALUE_LEN - 1] = '\0';

    return LibJingleEx::LIBJINGLE_ERR_OK;
}






unsigned char *  GetSs5ProxyUserPwdMethodQueryString( unsigned char * buf, unsigned long & dwSize, const char * szUser, const char * szPwd )
{
    assert( buf );

    LibJingleEx::CMsgData    messageData;

    unsigned char  byTmp;

    byTmp = '\x01';
    messageData.Append( (unsigned char *)&byTmp, sizeof(byTmp) );

    unsigned long dwTmp;
    dwTmp = strlen( szUser );

    // �û������� 
    if ( dwTmp > 255 )
    {
        return 0;
    }

    byTmp = (unsigned char)dwTmp;
    messageData.Append( (unsigned char *)&byTmp, sizeof(byTmp) );
    messageData.Append( (unsigned char *)szUser, dwTmp );


    dwTmp = strlen( szPwd );

    // ������� 
    if ( dwTmp > 255 )
    {
        return 0;
    }

    byTmp = (unsigned char)dwTmp;
    messageData.Append( (unsigned char *)&byTmp, sizeof(byTmp) );
    messageData.Append( (unsigned char *)szPwd, dwTmp );

    if ( dwSize < messageData.GetMsgLength() )
    {
        return 0;
    }

    memcpy( buf, messageData.GetMsgData(), messageData.GetMsgLength() );
    dwSize = messageData.GetMsgLength();

    return buf;
}

unsigned char *   GetSs5ProxyConnectQueryString( unsigned char * buf, unsigned long & dwSize, unsigned long dwDestIp, unsigned short int wDestPort )
{
    assert( buf );

    LibJingleEx::CMsgData    messageData;

    unsigned char  byTmp;

    byTmp = SS5_PROXY_VERSION;
    messageData.Append( (unsigned char *)&byTmp, sizeof(byTmp) );

    byTmp = SS5_PROXY_CONNECT_CMD;
    messageData.Append( (unsigned char *)&byTmp, sizeof(byTmp) );

    byTmp = SS5_PROXY_RSV;
    messageData.Append( (unsigned char *)&byTmp, sizeof(byTmp) );

    byTmp = SS5_PROXY_IPV4_TYPE;
    messageData.Append( (unsigned char *)&byTmp, sizeof(byTmp) );

    // ip
    dwDestIp = htonl( dwDestIp );
    messageData.Append( (unsigned char *)&dwDestIp, sizeof(dwDestIp) );

    wDestPort = htons( wDestPort );
    messageData.Append( (unsigned char *)&wDestPort, sizeof(wDestPort) );


    if ( dwSize < messageData.GetMsgLength() )
    {
        return 0;
    }

    memcpy( buf, messageData.GetMsgData(), messageData.GetMsgLength() );
    dwSize = messageData.GetMsgLength();

    return buf;
}


const char *  GetCharStateString(LibJingleEx::ECharStatus e)
{
    switch ( e )
    {
    case LibJingleEx::CHAR_STATUS_ACTIVE:
        return "active";
        break;

    case LibJingleEx::CHAR_STATUS_COMPOSING:
        return "composing";
        break;

    case LibJingleEx::CHAR_STATUS_PAUSED:
        return "paused";
        break;

    default:
        return "unknown";
        break;
    }

    return "unknown";
}

const char *     GetBoolString( BOOL b )
{
    if ( b )
    {
        return "true";
    }
    else
    {
        return "false";
    }
}

LibJingleEx::EPresence   GetMaxPresence( vMemberStatus & s, BOOL * pbMaxAudio /*= 0*/, BOOL * pbMaxVideo /*= 0*/ )
{
    // �ҵ�����״ֵ̬
    LibJingleEx::EPresence ePres_max = LibJingleEx::PRESENCE_NONE;
    unsigned dwMaxWeight = GetPresenceWeight(ePres_max);

    BOOL   bMaxAudio = FALSE;              // ������GK����������Ƶ��������������ΪTRUE
    BOOL   bMaxVideo = FALSE;              // ������GK����������Ƶ��������������ΪTRUE

    vMemberStatus::iterator it;
    for ( it = s.begin(); it != s.end(); it++ )
    {
        PTMemberStatus pStatus = *it;
        unsigned long dwWeight = GetPresenceWeight(pStatus->m_ePresence);

        if ( dwWeight > dwMaxWeight )
        {
            dwMaxWeight = dwWeight;
        }

        if ( pStatus->m_bCall )
        {
            if ( pStatus->m_bAudio )
            {
                bMaxAudio = TRUE;
            }

            if ( pStatus->m_bVideo )
            {
                bMaxVideo = TRUE;
            }
        }

    }

    ePres_max = GetPresenceFromWeight( dwMaxWeight );
    // END( �ҵ�����״ֵ̬ )


    if ( pbMaxAudio )
    {
        *pbMaxAudio = bMaxAudio;
    }

    if ( pbMaxVideo )
    {
        *pbMaxVideo = bMaxVideo;
    }

    return ePres_max;
}


/****************************************************************************
 * ��������  Str2Lower                                                      *
 * ��  �ܣ�  ת��ΪСд�ַ���                                               *
 * ����ֵ��  0          ʧ��                                                *
 *           ��0        ת������ַ���                                      *
 ****************************************************************************/
char * _Str2Lower( char * szStr )
{
    // �жϲ���
    if ( 0 == szStr )
    {
        return 0;
    }

    // ��ȡ�ַ�������
    unsigned long dwLen = strlen( szStr );

    unsigned long i;
    for ( i = 0; i < dwLen; i++ )
    {
        if ( szStr[i] >= 'A' && szStr[i] <= 'Z' )
        {
            szStr[i] = szStr[i] - 'A' + 'a';
        }
    }

    return szStr;
}

char  _TOLOWER( char ch )
{
    if ( ch >= 'A' && ch <= 'Z')
    {
        return 'a' + (ch - 'A');
    }
    else
    {
        return ch;
    }
}


int  _STRICMP( const char * s1, const char * s2 )
{
    while( _TOLOWER(*s1) == _TOLOWER(*s2) && '\0' != *s1 )
    {
        s1++;
        s2++;
    }

    if ( _TOLOWER(*s1) < _TOLOWER(*s2) )
    {
        return -1;
    }
    else if ( _TOLOWER(*s1) > _TOLOWER(*s2) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}



void  ClearList( std::list<cKeyValue *> & lst )
{
    std::list<cKeyValue *>::iterator it;
    for ( it = lst.begin();  it != lst.end(); it++ )
    {
        delete *it;
    }
    lst.clear();
}

const char * CCustomConfig::operator [] ( const char * szKey ) const
{
    std::list<cKeyValue *>::const_iterator it = std::find_if( m_lstConfigItems.begin(), m_lstConfigItems.end(), 
        FindConfigObj(szKey) );

    if ( m_lstConfigItems.end() == it )
    {
        return "";
    }
    else
    {
        cKeyValue * p = *it;
        return  p->sValue.c_str();
    }
}

void  CCustomConfig::SetConfig(const char * szKey, const char * szValue)
{
    LibJingleEx::TCustomConfigItem  tmp;
    strncpy( tmp.szKey,   szKey,   sizeof(tmp.szKey) - 1   );
    strncpy( tmp.szValue, szValue, sizeof(tmp.szValue) - 1 );
    strtrim( tmp.szKey );
    _Str2Lower( tmp.szKey );

    std::list<cKeyValue *>::iterator it = std::find_if( m_lstConfigItems.begin(), m_lstConfigItems.end(), 
        FindConfigObj(tmp.szKey) );

    if ( m_lstConfigItems.end() == it )
    {
        cKeyValue * p = new cKeyValue;
        p->sKey   = tmp.szKey;
        p->sValue = tmp.szValue;
        m_lstConfigItems.push_back( p );
    }
    else
    {
        cKeyValue * p = *it;
        p->sValue = tmp.szValue;
    }
}

const cKeyValue * CCustomConfig::GetFirstItem()
{
    m_it = m_lstConfigItems.begin();
    if ( m_it != m_lstConfigItems.end() )
    {
        return *m_it;
    }
    else
    {
        return 0;
    }
}

const cKeyValue * CCustomConfig::GetNext()
{
    m_it++;
    if ( m_it != m_lstConfigItems.end() )
    {
        return *m_it;
    }
    else
    {
        return 0;
    }
}






char *  MyGetVersion( char * buf, unsigned long dwSize, unsigned long dwVersion )
{
    int a, b, c, d;

    a = dwVersion & 0xFF000000;
    a >>= 24;

    b = dwVersion & 0x00FF0000;
    b >>= 16;

    c = dwVersion & 0x0000FF00;
    c >>= 8;

    d = dwVersion & 0x000000FF;

    SNPRINTF( buf, dwSize, "%d.%d.%d.%d", a, b, c, d  );

    return buf;
}

unsigned long  ConvertVersion(const char * szVersion)
{
    int a = 0, b = 0, c = 0, d = 0;
    sscanf( szVersion, "%d.%d.%d.%d", &a, &b, &c, &d );
    return GENERATE_NUMERIC_VERSION(a, b, c, d);
}








// ������������Ԫ�������������ַ�

static void XMLCALL
startElement_(void *data, const char *el, const char **attr)
{
    buzz::XmlElement * child = new buzz::XmlElement( buzz::QName(true, "", el) );

    char  buf[MAX_BUFF_SIZE];

    int i;
    for (i = 0; attr[i]; i += 2)
    {
        // child->SetAttr( buzz::QName(true, "", attr[i] ), Utf8ToAnsi( attr[i+1], buf, MAX_BUFF_SIZE ) );
        child->SetAttr( buzz::QName(true, "", attr[i] ), attr[i+1] );
    }

    buzz::XmlElement ** ppCurrent = (buzz::XmlElement **)data;
    if ( *ppCurrent )
    {
        (*ppCurrent)->AddElement( child );
    }

    *ppCurrent = child;
}

static void XMLCALL
endElement_(void *data, const char *el)
{
    buzz::XmlElement ** ppCurrent = (buzz::XmlElement **)data;
    assert( *ppCurrent );

    if ( (*ppCurrent)->Parent() )
        *ppCurrent = (*ppCurrent)->Parent();
}

static void XMLCALL
getBodyText_(void *userData,
            const XML_Char *s,
            int len)
{
    buzz::XmlElement ** ppCurrent = (buzz::XmlElement **)userData;
    assert( *ppCurrent );

    char buf[MAX_BUFF_SIZE] = { 0 };
    char * pBuf = 0;

    if ( len >= MAX_BUFF_SIZE )
    {
        pBuf = new char[len+1];
    }
    else
    {
        pBuf = buf;
    }

    memcpy( pBuf, s, len );
    pBuf[len] = '\0';

    strtrim( pBuf );

    // CAUSION: ֻ��ӷǿհ��ַ���ΪBodyText
    if ( '\0' != pBuf[0] )
    {
        std::string sTmp = (*ppCurrent)->BodyText();
        
        //char  szAnsi[MAX_BUFF_SIZE];
        //Utf8ToAnsi( pBuf, szAnsi, MAX_BUFF_SIZE );
        //sTmp += szAnsi;
        sTmp += pBuf;

        (*ppCurrent)->SetBodyText( sTmp );
    }

    if ( pBuf != buf )
    {
        delete[] pBuf;
    }
}


buzz::XmlElement * ParseXmlFile( const char * szFilePath )
{
    if ( 0 == szFilePath )
    {
        return 0;
    }

    XML_Parser parser = XML_ParserCreate(NULL);

    if ( !parser ) 
    {
        return 0;
    }

    buzz::XmlElement * current_node_ = 0;

    XML_SetElementHandler       ( parser, startElement_, endElement_ );
    XML_SetCharacterDataHandler ( parser, getBodyText_ );
    XML_SetUserData             ( parser, &current_node_ );

    
    // ��xml�ļ�
    talk_base::FileStream  file_stream;
    if ( !file_stream.Open( szFilePath, "rb" ) )
    {
        XML_ParserFree(parser);
        return 0;
    }

    const int MAX_DATA_SIZE    = 8192;
    char          data_buf[MAX_DATA_SIZE];

    size_t         dwRead      = 0;
    int            nError      = 0;


    // ����xml�ļ�
    talk_base::StreamResult eRet = file_stream.Read( data_buf, MAX_DATA_SIZE, &dwRead, &nError );
    while( talk_base::SR_EOS != eRet )
    {
        if ( XML_Parse( parser, data_buf, dwRead, 0 ) == XML_STATUS_ERROR )
        {
            fprintf(stderr, "Parse error at line %lu:\n%s\n", XML_GetCurrentLineNumber(parser), XML_ErrorString(XML_GetErrorCode(parser)));
            XML_ParserFree(parser);
            file_stream.Close();

            // ����root xml element
            buzz::XmlElement * tmp_node_ = 0;
            while( current_node_ )
            {
                tmp_node_ = current_node_;
                current_node_ = current_node_->Parent();
            }
            if ( tmp_node_ )
            {
                delete tmp_node_;
            }
            return 0;
        }

        dwRead = 0;
        eRet = file_stream.Read( data_buf, MAX_DATA_SIZE, &dwRead, &nError );
    }


    // ��������
    if ( XML_Parse( parser, "", 0, 1 ) == XML_STATUS_ERROR )
    {
        fprintf(stderr, "Parse error at line %lu:\n%s\n", XML_GetCurrentLineNumber(parser), XML_ErrorString(XML_GetErrorCode(parser)));
        XML_ParserFree(parser);
        file_stream.Close();

        // ����root xml element
        buzz::XmlElement * tmp_node_ = 0;
        while( current_node_ )
        {
            tmp_node_ = current_node_;
            current_node_ = current_node_->Parent();
        }
        if ( tmp_node_ )
        {
            delete tmp_node_;
        }
        return 0;
    }


    XML_ParserFree(parser);
    file_stream.Close();

    buzz::XmlElement * tmp_node_ = 0;
    while( current_node_ )
    {
        tmp_node_ = current_node_;
        current_node_ = current_node_->Parent();
    }

    return tmp_node_;
}




void StringReplace( std::string & strBase, const std::string & strSrc, const std::string & strDes )  
{  
    std::string::size_type pos = 0;  
    std::string::size_type srcLen = strSrc.size();  
    std::string::size_type desLen = strDes.size();  

    pos=strBase.find(strSrc, pos);   
    while ((pos != std::string::npos))  
    {  
        strBase.replace(pos, srcLen, strDes);  
        pos=strBase.find(strSrc, (pos+desLen));  
    }  
}

static std::string XmlSpecialChatReplace( const std::string & strText )
{
    std::string sRet = strText;

    StringReplace( sRet, "&",  "&amp;" );
    StringReplace( sRet, ">",  "&gt;" );
    StringReplace( sRet, "<",  "&lt;" );
    StringReplace( sRet, "\"", "&quot;" );
    StringReplace( sRet, "'",  "&apos;" );

    return sRet;
}

#define FILE_STREAM_WRITE_STD_STRING( f, s, dwWriten, nError ) \
    do \
    { \
        dwWriten = 0; \
        nError   = 0; \
        f.Write( s.c_str(), s.length(), &dwWriten, &nError ); \
    }while( 0 )

static void  WriteXmlElement( const buzz::XmlElement * root, talk_base::FileStream & file_stream )
{
    assert( root );

    size_t  dwWriten = 0;
    int     nError   = 0;
    char    buf[MAX_BUFF_SIZE];

    std::string s;

    s  = "<";
    s += root->Name().LocalPart().c_str();
    FILE_STREAM_WRITE_STD_STRING( file_stream, s, dwWriten, nError );

    // д����
    const buzz::XmlAttr * attr = root->FirstAttr();
    while( attr )
    {
        s  = " ";
        s += attr->Name().LocalPart().c_str();
        s += "=\"";
        FILE_STREAM_WRITE_STD_STRING( file_stream, s, dwWriten, nError );

        // s = XmlSpecialChatReplace( AnsiToUtf8( attr->Value().c_str(), buf, MAX_BUFF_SIZE ) );
        s = XmlSpecialChatReplace( attr->Value().c_str() );
        FILE_STREAM_WRITE_STD_STRING( file_stream, s, dwWriten, nError );

        s = "\"";
        FILE_STREAM_WRITE_STD_STRING( file_stream, s, dwWriten, nError );

        attr = attr->NextAttr();
    }

    if ( 0 == root->FirstElement() && 0 == root->BodyText().length()  )
    {
        s  = "/>";
        FILE_STREAM_WRITE_STD_STRING( file_stream, s, dwWriten, nError );
    }
    else
    {
        s  = ">";
        FILE_STREAM_WRITE_STD_STRING( file_stream, s, dwWriten, nError );

        // û���ӽڵ��BodyText��������д��<element/>��ʽ
        const buzz::XmlElement * child = root->FirstElement();
        while ( child )
        {
            WriteXmlElement( child, file_stream );
            child = child->NextElement();
        }

        if ( root->BodyText().length() > 0 )
        {
            s = root->BodyText();
            // s = XmlSpecialChatReplace( AnsiToUtf8( s.c_str(), buf, MAX_BUFF_SIZE ) );
            s = XmlSpecialChatReplace( s.c_str() );
            FILE_STREAM_WRITE_STD_STRING( file_stream, s, dwWriten, nError );
        }

        s  = "</";
        s += root->Name().LocalPart().c_str();
        s += ">";
        FILE_STREAM_WRITE_STD_STRING( file_stream, s, dwWriten, nError );
    }
}



int WriteXmlFile( buzz::XmlElement * rootElement, const char * szFilePath  )
{
    if ( 0 == rootElement || 0 == szFilePath )
    {
        return -1;
    }

    // ��xml�ļ�
    talk_base::FileStream  file_stream;
    if ( !file_stream.Open( szFilePath, "wb" ) )
    {
        return -1;
    }

    WriteXmlElement( rootElement, file_stream );

    file_stream.Close();
    return 0;
}


const buzz::XmlElement * GetFirstNamedElementByLocalPart( const buzz::XmlElement * root, const char * szLocalPart )
{
    assert( root && szLocalPart );

    const buzz::XmlElement * child = root->FirstElement();
    while( child )
    {
        if ( child->Name().LocalPart() == szLocalPart )
        {
            return child;
        }

        child = child->NextElement();
    }

    return 0;
}







#if !DMS_CONTACTS_SWITCH

int  _DmsDelMember( CLibJinglexApp *  pAppObj,  unsigned long dwMemberId )
{
    assert( dwMemberId > 0 );

    LibJingleEx::PTDmsMember ptDmsMember = pAppObj->GetDmsMember( dwMemberId );
    if ( 0 == ptDmsMember )
    {
        return -1;
    }

    // ��Dms˽��������ɾ��
    unsigned long dwBlockIndex = pAppObj->DeleteDmsMember( dwMemberId );

    // ���͸�����������
    unsigned long dwBlockBits = 0;
    SetBit( dwBlockBits, dwBlockIndex );
    SaveDmsMember2Server( pAppObj, dwBlockBits );


    buzz::XmppClient * client = pAppObj->m_pXmppThread->pump()->client();
    char buf[MAX_BUFF_SIZE];


    // ͬ��dms��������ͬresource���Լ�
    buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
    message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
    message->SetAttr( buzz::QN_TYPE, "dms" );

    buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
    message->AddElement( operation );
    operation->SetAttr( buzz::QN_TYPE, "delete member" );

    buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
    operation->AddElement( item );

    char szId[256];
    sprintf( szId,         "%u", dwMemberId );

    item->SetAttr( buzz::QN_DMS_ID,          szId  );

    pAppObj->m_pXmppThread->SendStanza( message );

    return 0;
}



int  _DmsAddMember( CLibJinglexApp *  pAppObj,  LibJingleEx::PTDmsMember  ptDmsMemberSrc )
{
    assert( pAppObj && ptDmsMemberSrc && SPECIAL_GROUP_SN == ptDmsMemberSrc->dwGroupId );

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    LibJingleEx::TDmsMember   tmpDmsMember;
    memcpy( &tmpDmsMember,  ptDmsMemberSrc,  sizeof(LibJingleEx::TDmsMember) );
    _Str2Lower( tmpDmsMember.szJid );
    LibJingleEx::PTDmsMember     ptDmsMember  =  &tmpDmsMember;


    vDmsGroup & vGroups = pAppObj->m_vDmsGroups;
    vDmsGroup::iterator it_group;

    // ��ѯ������������
    it_group = find_if( vGroups.begin(), vGroups.end(), FindDmsGroupObj( ptDmsMember->dwGroupId ) );
    // û�и���
    if ( it_group == vGroups.end() )
    {
        return LibJingleEx::LIBJINGLE_ERR_DMS_NO_SUCH_PARENT_GROUP;
    }

    unsigned long  i;
    // ��Ա����
    unsigned long  dwTotalsCnt = 0;
    // ͬһ���ڳ�Ա����
    unsigned long  dwSiblingsCnt = 0;

    // �Ƿ��Ѿ��������Ա�ˣ�����У�����������subscribe�ˣ�
    BOOL  bExistSuchMember = FALSE;

    for ( i = 0; i < DMS_MEMBERS_BLOCK_CNT; i++ )
    {
        vDmsMember & vMembers = pAppObj->m_vDmsMembers[i];
        vDmsMember::iterator it_member;

        dwTotalsCnt += vMembers.size();    

        for ( it_member = vMembers.begin(); it_member != vMembers.end(); it_member++ )
        {
            LibJingleEx::PTDmsMember p = *it_member;

            // �����ͬһ���ڵĳ�Ա
            if ( p->dwGroupId == ptDmsMember->dwGroupId )
            {
                dwSiblingsCnt++;

                // ���ͬһ�����Ѿ��иó�Ա��
                if (  0 == strcmp( p->szJid, ptDmsMember->szJid )  )
                {
                    bExistSuchMember = TRUE;
                    return LibJingleEx::LIBJINGLE_ERR_DMS_FRIEND_ALREADY_IN_THE_GROUP;
                }
            }
            // ����ͬһ���ڳ�Ա������jid��ͬ
            else if (  0 == strcmp( p->szJid, ptDmsMember->szJid )  )
            {
                bExistSuchMember = TRUE;
            }
        }
    }

    // ������еĳ�Ա�Ƿ�ﵽ�����Ŀ
    //if ( dwTotalsCnt >= MAX_DMS_TOTAL_MEMBERS_COUNT )
    //{
    //    return LibJingleEx::LIBJINGLE_ERR_DMS_REACH_MAX_TOTOL_MEMBERS_COUNT;
    //}
    // END ������еĳ�Ա�Ƿ�ﵽ�����Ŀ


    // ������ǡ�δ���顱���ͼ�����ڵĳ�Ա����(δ����ĳ�Ա���������)
    //if (  SPECIAL_GROUP_SN != ptDmsMember->dwGroupId && dwSiblingsCnt >= MAX_DMS_MEMBERS_IN_A_GROUP )
    //{
    //    return LIBJINGLE_ERR_DMS_REACH_MAX_SUB_MEMBER_COUNT;
    //}
    // END ��鸸����ӵ�еĳ�Ա�Ƿ�ﵽ���ֵ


    // ����Ƿ�����Լ�(�ܾ�����Լ�Ϊ����)
    if ( LibJingleEx::DMS_MEMBER_TYPE_JID == ptDmsMember->eType )
    {
        LibJingleEx::TJid    tSelfJid;
        LibJingleEx::StrJid2TJid( client->jid().BareJid().Str().c_str(), tSelfJid );

        LibJingleEx::TJid    tMemberJid;
        LibJingleEx::StrJid2TJid( ptDmsMember->szJid,  tMemberJid );

        if ( IfJidEqual( tSelfJid, tMemberJid) )
        {
            return LibJingleEx::LIBJINGLE_ERR_DMS_CANNOT_ADD_SELF;
        }
    }
    // END ����Ƿ�����Լ�(�ܾ�����Լ�Ϊ����)


    unsigned long dwBlockBits = 0;

    pAppObj->m_dwDmsMaxMemberId++;

    LibJingleEx::PTDmsMember ptNewMember = new LibJingleEx::TDmsMember;
    memcpy( ptNewMember, ptDmsMember, sizeof(LibJingleEx::TDmsMember) );
    ptNewMember->dwMemberId = pAppObj->m_dwDmsMaxMemberId;
    // vMembers.push_back( ptNewMember );
    // �ĸ��������˸��³�Ա
    unsigned long dwBlockIndex = pAppObj->AddDmsMember2Block( ptNewMember, dwBlockBits );
    ptDmsMember->dwMemberId = ptNewMember->dwMemberId;
    ptDmsMemberSrc->dwMemberId = ptDmsMember->dwMemberId;


    // ���͸�����������
    SaveDmsMember2Server( pAppObj, dwBlockBits );


    // �����jid���ͣ�������Ҫ��ע����
    //if ( DMS_MEMBER_TYPE_JID == ptNewMember->eType )
    //{
    //    // �����û��������ѣ���ע��
    //    if ( !bExistSuchMember )
    //    {
    //        TJid tJid;
    //        StrJid2TJid( ptDmsMember->szJid, tJid );

    //        Subscribe( hApp, &tJid );
    //    }
    //}


    char buf[MAX_BUFF_SIZE];
    // ͬ��dms��������ͬresource���Լ�
    buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
    message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
    message->SetAttr( buzz::QN_TYPE, "dms" );

    buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
    message->AddElement( operation );
    operation->SetAttr( buzz::QN_TYPE, "add member" );

    buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
    operation->AddElement( item );

    char szId[256];
    char szParentId[256];
    char szType[256];

    sprintf( szId,         "%u", ptDmsMember->dwMemberId );
    sprintf( szParentId,   "%u", ptDmsMember->dwGroupId );
    sprintf( szType,       "%u", ptDmsMember->eType );

    item->SetAttr( buzz::QN_DMS_ID,          szId  );
    item->SetAttr( buzz::QN_DMS_PARENT_ID,   szParentId  );
    item->SetAttr( buzz::QN_DMS_NAME,        AnsiToUtf8( ptDmsMember->szName, buf, MAX_BUFF_SIZE ) );
    item->SetAttr( buzz::QN_DMS_JID,         AnsiToUtf8( ptDmsMember->szJid, buf, MAX_BUFF_SIZE ) );
    item->SetAttr( buzz::QN_DMS_TYPE,        szType );

    item->SetAttr( buzz::QN_DMS_RESERVERD1,  AnsiToUtf8( ptDmsMember->szReserved[0], buf, MAX_BUFF_SIZE ) );
    item->SetAttr( buzz::QN_DMS_RESERVERD2,  AnsiToUtf8( ptDmsMember->szReserved[1], buf, MAX_BUFF_SIZE ) );
    item->SetAttr( buzz::QN_DMS_RESERVERD3,  AnsiToUtf8( ptDmsMember->szReserved[2], buf, MAX_BUFF_SIZE ) );
    item->SetAttr( buzz::QN_DMS_RESERVERD4,  AnsiToUtf8( ptDmsMember->szReserved[3], buf, MAX_BUFF_SIZE ) );

    pAppObj->m_pXmppThread->SendStanza( message );
    // ����

    return LibJingleEx::LIBJINGLE_ERR_OK;
}


int  _DmsModifyMember( CLibJinglexApp *  pAppObj, LibJingleEx::PTDmsMember  ptDmsMember )
{
    assert( pAppObj && ptDmsMember );

    buzz::XmppClient * client  = pAppObj->m_pXmppThread->client();

    LibJingleEx::PTDmsMember pFind = pAppObj->GetDmsMember( ptDmsMember->dwMemberId );
    if ( 0 == pFind )
    {
        return LibJingleEx::LIBJINGLE_ERR_DMS_NO_SUCH_FRIEND;
    }

    LibJingleEx::TDmsMember  tTmp;
    memcpy( &tTmp, ptDmsMember, sizeof(LibJingleEx::TDmsMember) );
    strcpy( tTmp.szJid, pFind->szJid );

    // ����ͬ��jid�ĳ�Ա���޸�
    unsigned long dwBlockbits = pAppObj->ModifyDmsMemberByJid( &tTmp );

    // ���͸�����������
    SaveDmsMember2Server( pAppObj, dwBlockbits );

    char buf[MAX_BUFF_SIZE];
    // ͬ��dms��������ͬresource���Լ�
    buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
    message->SetAttr( buzz::QN_TO,   AnsiToUtf8( client->jid().BareJid().Str().c_str(), buf, MAX_BUFF_SIZE ) );
    message->SetAttr( buzz::QN_TYPE, "dms" );

    buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
    message->AddElement( operation );
    operation->SetAttr( buzz::QN_TYPE, "modify member" );

    buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM );
    operation->AddElement( item );

    char szId[256];
    sprintf( szId,         "%u", ptDmsMember->dwMemberId );

    item->SetAttr( buzz::QN_DMS_ID,          szId  );
    item->SetAttr( buzz::QN_DMS_NAME,        AnsiToUtf8( ptDmsMember->szName, buf, MAX_BUFF_SIZE )  );

    item->SetAttr( buzz::QN_DMS_RESERVERD1,  AnsiToUtf8( ptDmsMember->szReserved[0], buf, MAX_BUFF_SIZE ) );
    item->SetAttr( buzz::QN_DMS_RESERVERD2,  AnsiToUtf8( ptDmsMember->szReserved[1], buf, MAX_BUFF_SIZE ) );
    item->SetAttr( buzz::QN_DMS_RESERVERD3,  AnsiToUtf8( ptDmsMember->szReserved[2], buf, MAX_BUFF_SIZE ) );
    item->SetAttr( buzz::QN_DMS_RESERVERD4,  AnsiToUtf8( ptDmsMember->szReserved[3], buf, MAX_BUFF_SIZE ) );


    pAppObj->m_pXmppThread->SendStanza( message );
    // ����

    return 0;
}


#endif





// ��ͬjid����ϵ�˵�Ui���Ըı�
BOOL   IfMemberUiChanged( const LibJingleEx::TMember * pOld , const LibJingleEx::TMember * pNew )
{
    if ( 0 != strcmp( pOld->szName, pNew->szName) )
    {
        return TRUE;
    }
    else if ( 0 != strcmp(pOld->szBitRate, pNew->szBitRate) )
    {
        return TRUE;
    }
    else if ( pOld->nType != pNew->nType )
    {
        return TRUE;
    }

    return FALSE;
}


BOOL   IfUnscribedContact( const LibJingleEx::TMember * pMember )
{
    if (    ( LibJingleEx::DMS_MEMBER_TYPE_JID == pMember->nType ) 
         && !( pMember->nSubscription & SUB_STATE_SUBSCRIBED ) 
         && (0 == pMember->nAsk) )
    {
        return TRUE;
    }

    return FALSE;
}











LIBJINGLE_API void LibJingleEx::SetTrueSocialName( const char * szJid, const char * szTureSocialName, BOOL bUtf8 /*= FALSE*/ )
{
    if ( 0 == szJid || 0 == szTureSocialName )
    {
        return;
    }

    char  szJid_[LibJingleEx::MAX_JID_LEN] = { 0 };
    char  szTsName_[MAX_TURE_SOCIAL_NAME_LEN] = { 0 };

    if ( !bUtf8 )
    {
        strncpy( szJid_,    szJid,            LibJingleEx::MAX_JID_LEN - 1 );
        strncpy( szTsName_, szTureSocialName, MAX_TURE_SOCIAL_NAME_LEN - 1 );
    }
    else
    {
        Utf8ToAnsi( szJid,            szJid_,     LibJingleEx::MAX_JID_LEN );
        Utf8ToAnsi( szTureSocialName, szTsName_,  MAX_TURE_SOCIAL_NAME_LEN );            
    }

    CGlobalData * pGlobal = CGlobalData::GetInstance();

    cFuncLock  cLock( &pGlobal->m_arrLocks[LOCK_INDEX_APP_VECTOR] );

    std::vector< CLibJinglexApp * >::iterator it;
    for ( it = pGlobal->m_vApps.begin(); it != pGlobal->m_vApps.end(); it++ )
    {
        CLibJinglexApp * p = *it;
        
        if ( p->m_pXmppThread )
        {
            p->m_pXmppThread->SetTrueSocialName( szJid_, szTsName_ );
        }
    }
}


FILE * XmppOpenFile   ( const char * szFile, const char * szMode )
{
    FILE * fp = 0;

#ifdef WIN32
    WCHAR   wszFile[256] = { 0 };
    WCHAR   wszMode[256] = { 0 };

    MultiByteToWideChar( CP_ACP,  0, szFile, strlen(szFile),  wszFile, 256 - 1 );
    MultiByteToWideChar( CP_ACP,  0, szMode, strlen(szMode),  wszMode, 256 - 1 );

    fp = _wfopen( wszFile, wszMode );
#else
    fp = fopen( szFile, szMode );
#endif

    return fp;
}