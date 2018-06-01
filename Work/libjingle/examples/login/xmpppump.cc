/*
 * libjingle
 * Copyright 2004--2005, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products 
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "talk/examples/login/xmpppump.h"
#include "talk/examples/login/xmppauth.h"

#include "libjingle/libjingle.h"
#include "libjingle/libjingle_inner.h"

#include "talk/base/tarstream.h"
#include "talk/base/pathutils.h"
#include "talk/base/helpers.h"
#include "talk/base/httpcommon.h"

#include "talk/examples/login/xmppthread.h"

XmppPump::XmppPump( void * hApp, XmppPumpNotify * notify ) {
    m_hApp = hApp;

  state_ = buzz::XmppEngine::STATE_NONE;
  notify_ = notify;
  client_ = new buzz::XmppClient(this);  // NOTE: deleted by TaskRunner
  client_->m_hApp = hApp;

  presence_push_ = 0;
  presence_out_  = 0;
  roster_task_   = 0;
  msg_task_      = 0;
  disco_task_    = 0;
  chatroom_task_ = 0;
  session_manager_task_ = 0;
  fileshare_task_ = 0;
  private_task_   = 0;
  vcard_task_     = 0;
  pubsub_task_    = 0;
  ping_task_      = 0;
  privacy_task_   = 0;
  time_task_      = 0;

  JInitLock( &m_tFileShareClientLock );
}


XmppPump::~XmppPump()
{
    Clear();
    // assert( m_vFileShareClients.size() == 0 );
    JDeinitLock( &m_tFileShareClientLock );
}

void XmppPump::Clear()
{
    
}

void XmppPump::AddFileShareClient( FileShareClient * p )
{
    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    m_vFileShareClients.push_back( p );
}

void XmppPump::RemoveFileShareClient( FileShareClient * pFileShareClient )
{
    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    vFileShareClient::iterator it;
    vFileShareClient & v = m_vFileShareClients;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        FileShareClient * p = *it;
        if ( p == pFileShareClient )
        {
            v.erase( it );
            delete p;
            break;
        }
    }
}

BOOL XmppPump::IfExistFileShareClient( FileShareClient * pSession )
{
    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    vFileShareClient::iterator it;
    vFileShareClient & v = m_vFileShareClients;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        FileShareClient * p = *it;
        if ( p == pSession )
        {
            return TRUE;
        }
    }

    return FALSE;
}

void XmppPump::TerminateAllFileShareSessions()
{
    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    vFileShareClient::iterator it;
    vFileShareClient & v = m_vFileShareClients;
    BOOL bLocal = TRUE;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        FileShareClient * p = *it;

        if ( p->m_pSession )
        {
            p->m_bPost2Pump = FALSE; 
            p->m_pSession->Cancel();
        }
        // 通过服务器传输
        else
        {
            CGlobalData * pGlobal = CGlobalData::GetInstance();
            pGlobal->m_dwFileSessionCnt--;

            // 告知上层取消
            if ( g_tEvents.pOnFileCancel )
            {
                g_tEvents.pOnFileCancel( m_hApp, p, bLocal );
            }
            else if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData  msgData;

                LibJingleEx::HSession   hSession = p;
                msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );
                msgData.Append( (unsigned char *)&bLocal,    sizeof(bLocal) );

                g_pfnOnMessage( LibJingleEx::ON_FILE_CANCEL,  app, app->m_pUserData,
                    msgData.GetMsgData(),   msgData.GetMsgLength() );
            }
        }

        delete p;
    }
    v.clear();
}

// 仅用于通过xmpp服务器传送文件的情况下使用
FileShareClient * XmppPump::FindFileShareClientBySessionId( const char * szPeerJid, const char * szSessionId )
{
    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    vFileShareClient::iterator it;
    vFileShareClient & v = m_vFileShareClients;

    LibJingleEx::TJid tJId;
    LibJingleEx::StrJid2TJid( szPeerJid, tJId );

    for ( it = v.begin(); it != v.end(); it++ )
    {
        FileShareClient * p = *it;
        // 0 == p->m_pSession 表示p2p传送文件失败
        if (    0 == p->m_pSession && 0 == strcmp( p->m_szSessionId, szSessionId ) 
             && IfJidEqual(tJId, p->m_PeerJid) )
        {
            return p;
        }
    }
    return 0;
}

void XmppPump::AcceptFiles( cricket::FileShareSession * pSession, const char * szSavePath /*= 0*/ )
{
    if ( 0 == pSession )
    {
        return;
    }

    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    vFileShareClient::iterator it;
    vFileShareClient & v = m_vFileShareClients;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        FileShareClient * p = *it;

        if ( p->m_pSession == pSession )
        {
            if ( !p->m_pSession->is_sender() )
            {
                if ( szSavePath && strlen( szSavePath ) > 0 )
                {
                    char * pSlash = 0;
#ifdef WIN32
                    pSlash = strrchr( (char *)szSavePath, '\\' );
#else
                    pSlash = strrchr( (char *)szSavePath, '/' );
#endif
                    if ( pSlash )                    
                    {
                        pSession->m_sSaveAsName = pSlash + 1;
                        strncpy( p->m_szSaveAsName, pSlash + 1, sizeof(p->m_szSaveAsName) );

                        char szTemp[256] = {0};
                        strncpy( szTemp, szSavePath, (pSlash - szSavePath) );

                        p->m_pSession->SetLocalFolder( szTemp );
                        strncpy( p->m_szLocalDir, szTemp, LibJingleEx::MAX_PATH_SIZE );
                    }
                    else
                    {
                        pSession->m_sSaveAsName = szSavePath;
                        strncpy( p->m_szSaveAsName, szSavePath, sizeof(p->m_szSaveAsName) );
                    }
                }
                p->m_pSession->Accept();
            }
            break;
        }
    }
}

void XmppPump::DeclineFiles( cricket::FileShareSession * pSession )
{
    if ( 0 == pSession )
    {
        return;
    }

    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    vFileShareClient::iterator it;
    vFileShareClient & v = m_vFileShareClients;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        FileShareClient * p = *it;

        if ( p->m_pSession == pSession )
        {
            if ( !p->m_pSession->is_sender() )
            {
                p->m_pSession->Decline();
            }
            break;
        }
    }
}

void XmppPump::OnFriendOffline( const LibJingleEx::TJid * ptJid )
{
    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    vFileShareClient::iterator it;
    vFileShareClient & v = m_vFileShareClients;

    BOOL bLocal = FALSE;

    for ( it = v.begin(); it != v.end(); )
    {
        FileShareClient * p = *it;

        // 如果对端的jid相同
        if ( IfJidEqual( p->m_PeerJid, *ptJid ) )
        {
            if ( p->m_pSession )
            {
                p->m_bPeerOffline = TRUE;
                p->m_pSession->Cancel();
                it++;
            }
            // 通过服务器传输
            else
            {
                CGlobalData * pGlobal = CGlobalData::GetInstance();
                pGlobal->m_dwFileSessionCnt--;

                // 告知上层取消
                if ( g_tEvents.pOnFileCancel )
                {
                    g_tEvents.pOnFileCancel( m_hApp, p, bLocal );
                }
                else if ( g_pfnOnMessage )
                {
                    LibJingleEx::CMsgData  msgData;

                    LibJingleEx::HSession   hSession = p;
                    msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );
                    msgData.Append( (unsigned char *)&bLocal,    sizeof(bLocal) );

                    g_pfnOnMessage( LibJingleEx::ON_FILE_CANCEL,  app, app->m_pUserData,
                        msgData.GetMsgData(),   msgData.GetMsgLength() );
                }

                it = v.erase( it );
                delete p;
            }
        }
        else
        {
            it++;
        }
    }
}

void XmppPump::CancelFiles( FileShareClient * pFileShareClient, BOOL bLocal, BOOL bSendIq, int nError /*= 0*/ )
{
    if ( 0 == pFileShareClient )
    {
        LIBJINGLE_LOG_ERROR( "0 == pFileShareClient" );
        return;
    }

    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    vFileShareClient::iterator it;
    vFileShareClient & v = m_vFileShareClients;

    for ( it = v.begin(); it != v.end(); it++ )
    {
        FileShareClient * p = *it;

        if ( p == pFileShareClient )
        {
            if ( p->m_pSession )
            {
                p->m_pSession->Cancel();
            }
            // 通过服务器传输
            else
            {
                if ( bLocal && bSendIq )
                {
                    // 发送Cancel Stanze
                    CancelTransfer( p );
                }


                CGlobalData * pGlobal = CGlobalData::GetInstance();
                pGlobal->m_dwFileSessionCnt--;


                // 如果nError为0，告知上层取消
                if ( 0 == nError )
                {
                    if ( g_tEvents.pOnFileCancel )
                    {
                        g_tEvents.pOnFileCancel( m_hApp, p, bLocal );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        LibJingleEx::HSession   hSession = p;
                        msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );
                        msgData.Append( (unsigned char *)&bLocal,    sizeof(bLocal) );

                        g_pfnOnMessage( LibJingleEx::ON_FILE_CANCEL,  app, app->m_pUserData,
                            msgData.GetMsgData(),   msgData.GetMsgLength() );
                    }
                }
                // 如果nError为0，告知上层失败
                else
                {
                    int  nTmpErr = 0;
                    if ( talk_base::HE_DISCONNECTED == nError )
                    {
                        nTmpErr = LibJingleEx::FILE_SHARE_ERR_DISCONNECTED;
                    }
                    else if ( talk_base::HE_STREAM == nError )
                    {
                        nTmpErr = LibJingleEx::FILE_SHARE_ERR_NO_SPACE;
                    }

                    if ( g_tEvents.pOnFileFailure )
                    {
                        g_tEvents.pOnFileFailure( m_hApp, this, nTmpErr );
                    }
                    else if ( g_pfnOnMessage )
                    {
                        LibJingleEx::CMsgData  msgData;

                        LibJingleEx::HSession   hSession = this;
                        msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );
                        msgData.Append( (unsigned char *)&nTmpErr,   sizeof(nTmpErr) );

                        g_pfnOnMessage( LibJingleEx::ON_FILE_FAILURE,  app, app->m_pUserData,
                                                   msgData.GetMsgData(),   msgData.GetMsgLength() );
                    }
                }

                
                

                v.erase( it );
                delete p;
            }
            break;
        }
    }
}

void XmppPump::DoLogin(const buzz::XmppClientSettings & xcs,
                       buzz::AsyncSocket* socket,
                       buzz::PreXmppAuth* auth) {
  OnStateChange(buzz::XmppEngine::STATE_START, 0, 0);
  if (!AllChildrenDone()) {
    client_->SignalStateChange.connect(this, &XmppPump::OnStateChange);
    client_->Connect(xcs, "", socket, auth);
    client_->Start();
  }
}


void XmppPump::AfterLogin()
{
    char buf[MAX_BUFF_SIZE];
    CLibJinglexApp * pApp = (CLibJinglexApp *)m_hApp;

    // 如果开启了文件传输能力 
    if ( pApp->m_bFileShareAvailable )
    {
        std::string client_unique = client_->jid().Str();
        cricket::InitRandom(client_unique.c_str(), client_unique.size());

        JLock( &pApp->m_lock );
        port_allocator_.reset(new cricket::HttpPortAllocator(&network_manager_, "pcp", 
            pApp->m_szStunAddr, pApp->m_wStunPort, pApp->m_szRelayAddr ));
        JUnlock( &pApp->m_lock );

        session_manager_.reset(new cricket::SessionManager(port_allocator_.get(), NULL));

        if ( session_manager_task_ )
        {
            delete session_manager_task_;
            session_manager_task_ = 0;
        }
        session_manager_task_ = new cricket::SessionManagerTask( client_, session_manager_.get() );
        session_manager_task_->EnableOutgoingMessages();
        session_manager_task_->Start();


        if ( fileshare_task_ )
        {
            delete fileshare_task_;
            fileshare_task_ = 0;
        }
        fileshare_task_ = new buzz::FileShareTask( client_ );
        fileshare_task_->Start();


        file_share_session_client_.reset(new cricket::FileShareSessionClient( session_manager_.get(), client_->jid(), "pcp") );
        file_share_session_client_->SignalFileShareSessionCreate.connect( this, &XmppPump::OnFileShareSessionCreate);
        session_manager_->AddClient(NS_GOOGLE_SHARE, file_share_session_client_.get());
    }



    if ( disco_task_ )
    {
        delete disco_task_;
        disco_task_ = 0;
    }
    disco_task_ = new buzz::MyDiscoTask( client_ );
    disco_task_->GetInfo();
    disco_task_->GetItems();
    disco_task_->Start();


#if ROSTER_SWITCH

    // 获得roster版本号
    pApp->GetRosterData( pApp->m_szRosterVer, ROSTER_VERSION_MAX_LENGTH );

    if ( roster_task_ )
    {
        delete roster_task_;
        roster_task_ = 0;
    }

    roster_task_ = new buzz::MyRosterTask( client_ );
    roster_task_->GetRoster( pApp->m_szRosterVer );
    roster_task_->Start();
#endif


    // added by shijie
    if ( presence_push_ )
    {
        delete presence_push_;
        presence_push_ = 0;
    }

    presence_push_ = new buzz::PresencePushTask(client_);
    // Hook up to the notification signal sent when the stanza is received.
    presence_push_->SignalStatusUpdate.connect(this, &XmppPump::OnStatusUpdate);

    // Start listening for presence stanzas.
    presence_push_->Start();
    // end of adding

    if ( msg_task_ )
    {
        delete msg_task_;
        msg_task_ = 0;
    }

    msg_task_ = new buzz::MyMsgTask( client_ );
    msg_task_->Start();

    if ( chatroom_task_ )
    {
        delete chatroom_task_;
        chatroom_task_ = 0;
    }

    chatroom_task_= new buzz::MyChatRoomTask( client_ );
    chatroom_task_->Start();

    if ( private_task_ )
    {
        delete private_task_;
        private_task_ = 0;
    }
    private_task_ = new buzz::MyPrivateTask( client_ );
    private_task_->GetDmsGroupInfo();
    private_task_->GetDmsMemberInfo();
    private_task_->GetPersistentRoomsInfo();
    private_task_->GetGroupchatHistory();
    private_task_->GetCustomConfigInfo();
    private_task_->Start();

    if ( vcard_task_ )
    {
        delete vcard_task_;
        vcard_task_ = 0;
    }

    vcard_task_ = new buzz::MyVcardTask( client_ );
    vcard_task_->Start();


    if ( pubsub_task_ )
    {
        delete pubsub_task_;
        pubsub_task_ = 0;
    }

    pubsub_task_ = new buzz::MyPubsubTask( client_ );
    pubsub_task_->Start();


    if ( ping_task_ )
    {
        delete ping_task_;
        ping_task_ = 0;
    }

    ping_task_ = new buzz::PingTask( client_ );
    ping_task_->Start();


    if ( privacy_task_ )
    {
        delete privacy_task_;
        privacy_task_ = 0;
    }

    privacy_task_ = new buzz::MyPrivacyTask( client_ );
    privacy_task_->Start();


    if ( time_task_ )
    {
        delete time_task_;
        time_task_ = 0;
    }

    time_task_ = new buzz::MyTimeTask( client_ );
    time_task_->QueryServerTime();
    time_task_->Start();


    // 延时1秒后检查是否发送过status
    pApp->m_pXmppThread->PostDelayed( SURE_SEND_STATUS_DELAY, pApp->m_pXmppThread, MSG_SURE_SEND_STATUS );


    pApp->m_pXmppThread->PostDelayed( INNER_LOOP_TIMEOUT, pApp->m_pXmppThread, MSG_INNER_LOOP );


    pApp->m_dwLoginTick = GetCurTick();
}

void XmppPump::OnFileShareSessionCreate(cricket::FileShareSession *sess) {

    FileShareClient * fs_ = 0;//= new FileShareClient( m_hApp, sess );

    CGlobalData * pGlobal = CGlobalData::GetInstance();
    pGlobal->m_dwFileSessionCnt++;

    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    // 接收者
    if ( 0 == sess->m_dwShareId )
    {
        fs_             = new FileShareClient;
        fs_->m_hApp     = m_hApp;
        fs_->m_pSession = sess;
        m_vFileShareClients.push_back( fs_ );

        if ( 0 == sess->local_folder().length() )
        {
            CLibJinglexApp * pApp = (CLibJinglexApp *)m_hApp;
            cFuncLock cLock( &pApp->m_lock );
            sess->SetLocalFolder( pApp->m_szSaveFileDir );
        }
    }
    else
    {
        vFileShareClient::iterator it;
        vFileShareClient & v = m_vFileShareClients;
        for ( it = v.begin(); it != v.end(); it++ )
        {
            FileShareClient * p = *it;
            if ( p->m_dwNo == sess->m_dwShareId )
            {
                break;
            }
        }
        fs_ = *it;
    }

    assert( fs_ );

    sess->SignalState.connect(fs_, &FileShareClient::OnSessionState);
    sess->SignalNextFile.connect(fs_, &FileShareClient::OnUpdateProgress);
    sess->SignalUpdateProgress.connect(fs_, &FileShareClient::OnUpdateProgress);
    sess->SignalResampleImage.connect(fs_, &FileShareClient::OnResampleImage);
}


void XmppPump::DoDisconnect() {
    // assert( m_vFileShareClients.size() == 0 );

  if (!AllChildrenDone())
    client_->Disconnect();

  Clear();
  
  OnStateChange(buzz::XmppEngine::STATE_CLOSED, 0, 0);
}

void XmppPump::OnStateChange(buzz::XmppEngine::State state, int nError, int nSubCode) {
  if (state_ == state)
    return;
  state_ = state;

  if ( state_ == buzz::XmppEngine::STATE_CLOSED )
  {
     TerminateAllFileShareSessions();
  }

  if (notify_ != NULL)
    notify_->OnStateChange(state, nError, nSubCode);
}

void XmppPump::WakeTasks() {
  talk_base::Thread::Current()->Post(this);
}

int64 XmppPump::CurrentTime() {
  return (int64)talk_base::Time();
}

void XmppPump::TransferData( const char * szBase64, FileShareClient * ptFileShare )
{
    buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );

    char szId[MAX_RAND_ID_LENGTH];
    char szJid[256];
    LibJingleEx::TJid2StrJid( ptFileShare->m_PeerJid, szJid, 256 );


    iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
    iq->SetAttr( buzz::QN_TO,   szJid );
    iq->SetAttr( buzz::QN_ID,   GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

    buzz::XmlElement * fileshare = new buzz::XmlElement( buzz::QN_FILE_SHARE, true );
    iq->AddElement(fileshare);

    fileshare->SetAttr( buzz::QN_FILE_SHARE_SUBTYPE, "transfer" );
    fileshare->SetAttr( buzz::QN_FILE_SHARE_SESSION_ID, ptFileShare->m_szSessionId );
    fileshare->SetBodyText( szBase64 );

    client_->SendStanza( iq );
    delete iq;
}

bool XmppPump::OnFileShareComplete( const char * szPeerJid, const char * szSessionId, const char * szPath )
{
    FileShareClient * pFileShareClient = FindFileShareClientBySessionId( szPeerJid,  szSessionId );
    assert( pFileShareClient && 0 == pFileShareClient->m_pSession );

    assert( pFileShareClient->m_Manifest.GetSize() > 0 );

    const LibJingleEx::FileShareManifest::Item & item = pFileShareClient->m_Manifest.GetItem( 0 );

    talk_base::Pathname path_old( szPath );
    talk_base::Pathname path_new;
    if ( 0 == strlen( pFileShareClient->m_szSaveAsName ) )
    {
        path_new.SetPathname( item.m_szName );
    }
    else
    {
        path_new.SetPathname( pFileShareClient->m_szSaveAsName );
    }
    path_new.SetFolder( pFileShareClient->m_szLocalDir );
    
    if ( !talk_base::CreateUniqueFile(path_new, false) )
    {
        return false;
    }

    if ( !talk_base::Filesystem::MoveFile( path_old, path_new ) )
    {
        return false;
    }

    CGlobalData * pGlobal = CGlobalData::GetInstance();
    pGlobal->m_dwFileSessionCnt--;

    CLibJinglexApp * pAppObj    = (CLibJinglexApp *)m_hApp;
    cFuncLock cLock( &pAppObj->m_lock );

    if ( g_tEvents.pOnFileComplete )
    {
        g_tEvents.pOnFileComplete( m_hApp, pFileShareClient, path_new.pathname().c_str() );
    }
    else if ( g_pfnOnMessage )
    {
        LibJingleEx::CMsgData  msgData;

        LibJingleEx::HSession   hSession = pFileShareClient;
        msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );

        msgData.Append( (unsigned char *)path_new.pathname().c_str(), path_new.pathname().length() + 1 );

        g_pfnOnMessage( LibJingleEx::ON_FILE_COMPLETE,  pAppObj, pAppObj->m_pUserData,
            msgData.GetMsgData(),   msgData.GetMsgLength() );
    }

    return true;
}

void  XmppPump::EndTransfer( FileShareClient * ptFileShare )
{
    buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );

    char szId[MAX_RAND_ID_LENGTH];
    char szJid[256];
    LibJingleEx::TJid2StrJid( ptFileShare->m_PeerJid, szJid, 256 );


    iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
    iq->SetAttr( buzz::QN_TO,   szJid );
    iq->SetAttr( buzz::QN_ID,   GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

    buzz::XmlElement * fileshare = new buzz::XmlElement( buzz::QN_FILE_SHARE, true );
    iq->AddElement(fileshare);

    fileshare->SetAttr( buzz::QN_FILE_SHARE_SUBTYPE, "end" );
    fileshare->SetAttr( buzz::QN_FILE_SHARE_SESSION_ID, ptFileShare->m_szSessionId );

    client_->SendStanza( iq );
    delete iq;
}

void   XmppPump::CancelTransfer( FileShareClient * ptFileShare )
{
    buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );

    char szId[MAX_RAND_ID_LENGTH];
    char szJid[256];
    LibJingleEx::TJid2StrJid( ptFileShare->m_PeerJid, szJid, 256 );


    iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
    iq->SetAttr( buzz::QN_TO,   szJid );
    iq->SetAttr( buzz::QN_ID,   GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

    buzz::XmlElement * fileshare = new buzz::XmlElement( buzz::QN_FILE_SHARE, true );
    iq->AddElement(fileshare);

    fileshare->SetAttr( buzz::QN_FILE_SHARE_SUBTYPE, "cancel" );
    fileshare->SetAttr( buzz::QN_FILE_SHARE_SESSION_ID, ptFileShare->m_szSessionId );

    client_->SendStanza( iq );
    delete iq;
}

FileShareClient *   XmppPump::FindFileShareObject( const char * szJid,  const char * szSessionId )
{
    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    vFileShareClient::iterator it;
    vFileShareClient & v = m_vFileShareClients;

    LibJingleEx::TJid tJid;
    LibJingleEx::StrJid2TJid( szJid, tJid );

    for ( it = v.begin(); it != v.end(); it++ )
    {
        FileShareClient * p = *it;
        if ( 0 == strcmp( p->m_szSessionId, szSessionId ) && IfJidEqual( tJid, p->m_PeerJid ) )
        {
            return p;
        }
    }
    return 0;
}

// 收到类型为"result"的file share stanze
void XmppPump::OnFileShareResult( PTFileShareMsg p )
{
    CLibJinglexApp * pAppObj    = (CLibJinglexApp *)m_hApp;
    cFuncLock cLock( &pAppObj->m_lock );


    FileShareClient * pFileShareClient = FindFileShareClientBySessionId( p->szPeerJid, p->szSessionId );

    // 没有找到，可能被用户之前Cancel掉了
    if ( 0 == pFileShareClient )
    {
        return;
    }

    char buf[MAX_BYTE_PER_TRANSFER];
    char data_buf[MAX_BYTE_PER_TRANSFER * 2];

    size_t        dwRead;
    int           nError;

    switch ( p->eSubType )
    {
    case FILE_SHARE_SUB_TYPE_START:
        {
            assert( pFileShareClient->m_Manifest.GetSize() > 0 );
            const LibJingleEx::FileShareManifest::Item & item = pFileShareClient->m_Manifest.GetItem(0);
            if ( LibJingleEx::FileShareManifest::T_FOLDER == item.m_eTyp )
            {
                talk_base::TarStream * tar = new talk_base::TarStream;
                std::string  sPath = pFileShareClient->m_szLocalDir;
#ifdef WIN32
                sPath += '\\';
#else
                sPath += '/';
#endif
                sPath += item.m_szName;

                // 如果打开文件夹读
                if ( !tar->Open( sPath, true ) )
                {
                    // assert( false );
                    MYLOG( "====[failed to open fold(%s)] \r\n", sPath.c_str() );
                    CancelFiles( pFileShareClient, TRUE, TRUE );
                    return;
                }
                pFileShareClient->m_stream = tar;

                tar->SignalNextEntry.connect( pFileShareClient, &FileShareClient::OnNextEntry);
            }
            else
            {
                talk_base::FileStream * file = new talk_base::FileStream;
                std::string  sPath = pFileShareClient->m_szLocalDir;
#ifdef WIN32
                sPath += '\\';
#else
                sPath += '/';
#endif
                sPath += item.m_szName;

                // 如果打开文件读失败
                if ( !file->Open( sPath, "rb" ) )
                {
                    // assert( false );
                    MYLOG( "====[failed to open file(%s)] \r\n", sPath.c_str() );
                    CancelFiles( pFileShareClient, TRUE, TRUE );
                    return;
                }
                pFileShareClient->m_stream = file;

                pFileShareClient->OnNextEntry( item.m_szName, 0  );
            }

            pFileShareClient->m_counter = new StreamCounter(pFileShareClient->m_stream);
            pFileShareClient->m_counter->SignalUpdateByteCount.connect( pFileShareClient, &FileShareClient::OnUpdateBytes);


            //talk_base::StreamResult eRet = 
            //    pFileShareClient->m_stream->Read( buf, MAX_BYTE_PER_TRANSFER, &dwRead, &nError);
            talk_base::StreamResult eRet = pFileShareClient->m_counter->Read( buf, MAX_BYTE_PER_TRANSFER, 
                            &dwRead, &nError );
            // 如果读文件还没有结束
            if ( talk_base::SR_EOS != eRet )
            {
                unsigned long dwSize = MAX_BYTE_PER_TRANSFER * 2;
                if ( 0 != LibJingleEx::EncodeBase64( data_buf, dwSize, (unsigned char *)buf, dwRead ) )
                {
                    // assert( false );
                    MYLOG( "====[failed to LibJingleEx::EncodeBase64()] \r\n" );
                    CancelFiles( pFileShareClient, TRUE, TRUE );
                    return;
                }

                // 传输数据
                TransferData( data_buf, pFileShareClient );
            }
            // 读文件已经结束
            else
            {
                pFileShareClient->m_stream->Close();
                pFileShareClient->m_stream = 0;
                pFileShareClient->m_bEof = TRUE;

                // 发送文件传输结束的Stanza
                EndTransfer( pFileShareClient );
            }
        }
        break;
    // 收到接收者END回复后，才认为自己文件传输结束
    case FILE_SHARE_SUB_TYPE_END:
        {
            CGlobalData * pGlobal = CGlobalData::GetInstance();
            pGlobal->m_dwFileSessionCnt--;

            if ( g_tEvents.pOnFileComplete )
            {
                g_tEvents.pOnFileComplete( m_hApp, pFileShareClient, "" );
            }
            else if ( g_pfnOnMessage )
            {
                LibJingleEx::CMsgData  msgData;

                LibJingleEx::HSession   hSession = pFileShareClient;
                msgData.Append( (unsigned char *)&hSession,  sizeof(hSession) );

                msgData.Append( (unsigned char *)"", 1 );

                g_pfnOnMessage( LibJingleEx::ON_FILE_COMPLETE,  pAppObj, pAppObj->m_pUserData,
                    msgData.GetMsgData(),   msgData.GetMsgLength() );
            }

            RemoveFileShareClient( pFileShareClient );
        }
        break;
    case FILE_SHARE_SUB_TYPE_CANCEL:
        {

        }
        break;
    case FILE_SHARE_SUB_TYPE_FAIL:
        {
            CancelFiles( pFileShareClient, FALSE, FALSE, p->nError );
        }
        break;
    case FILE_SHARE_SUB_TYPE_TRANSFER:
        {
            unsigned long dwTotalSize = pFileShareClient->m_Manifest.GetTotalSize();
            pFileShareClient->OnUpdateProgress( dwTotalSize, pFileShareClient->m_szTransferName );

            if ( pFileShareClient->m_bEof )
            {
                assert( 0 );
                EndTransfer( pFileShareClient );
            }
            else
            {
                assert( pFileShareClient->m_stream );

                //talk_base::StreamResult eRet = 
                //    pFileShareClient->m_stream->Read( buf, MAX_BYTE_PER_TRANSFER, &dwRead, &nError);
                talk_base::StreamResult eRet = pFileShareClient->m_counter->Read( buf, MAX_BYTE_PER_TRANSFER, 
                                                &dwRead, &nError );
                if ( talk_base::SR_EOS != eRet )
                {
                    unsigned long dwSize = MAX_BYTE_PER_TRANSFER * 2;
                    if ( 0 != LibJingleEx::EncodeBase64( data_buf, dwSize, (unsigned char *)buf, dwRead ) )
                    {
                        // 暂时不考虑读取失败
                        // assert( false );
                        MYLOG( "====[failed to LibJingleEx::EncodeBase64()] \r\n" );
                        CancelFiles( pFileShareClient, TRUE, TRUE );
                        return;
                    }

                    TransferData( data_buf, pFileShareClient );
                }
                else
                {
                    pFileShareClient->m_stream->Close();
                    pFileShareClient->m_stream = 0;
                    pFileShareClient->m_bEof = TRUE;

                    EndTransfer( pFileShareClient );
                }
            }
        }
        break;
    default:
        assert(0);
        break;
    }
}

void XmppPump::OnMessage(talk_base::Message *pmsg) {

    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock  cLock( &app->m_lock );

    if ( 0 == pmsg->message_id )
    {
        RunTasks();
    }
    else if ( MSG_STOP_FILE_SHARE == pmsg->message_id )
    {
        FileShareClient * p = (FileShareClient *)pmsg->pdata;
        RemoveFileShareClient( p );
    }
    // p2p文件传输失败，再通过服务中转文件数据
    else if ( MSG_P2P_FAIL == pmsg->message_id )
    {
        FileShareClient * p = (FileShareClient *)pmsg->pdata;
        // 关闭FileShare Session
        delete p->m_pSession;
        p->m_pSession = 0;

        // 如果是主动文件传输者，向接收者发送stanza
        if ( p->m_IsSender )
        {
            BeginFileShare( p );
        }
    }
}

// 发送自定义的file share stanza
void  XmppPump::BeginFileShare( FileShareClient * ptFileShare )
{
    buzz::XmlElement * iq = new buzz::XmlElement( buzz::QN_IQ );
    
    char buf[MAX_BUFF_SIZE];
    char szId[MAX_RAND_ID_LENGTH];
    char szJid[256];
    LibJingleEx::TJid2StrJid( ptFileShare->m_PeerJid, szJid, 256 );


    iq->SetAttr( buzz::QN_TYPE, buzz::STR_SET );
    iq->SetAttr( buzz::QN_TO,   szJid );
    iq->SetAttr( buzz::QN_ID,   GenerateRandomId(szId,MAX_RAND_ID_LENGTH) );

    buzz::XmlElement * fileshare = new buzz::XmlElement( buzz::QN_FILE_SHARE, true );
    iq->AddElement(fileshare);

    fileshare->SetAttr( buzz::QN_FILE_SHARE_SUBTYPE, "start" );
    fileshare->SetAttr( buzz::QN_FILE_SHARE_SESSION_ID, ptFileShare->m_szSessionId );

    assert( ptFileShare->m_Manifest.GetSize() == 1 );

    const LibJingleEx::FileShareManifest::Item & item = ptFileShare->m_Manifest.GetItem(0);

    fileshare->SetAttr( buzz::QN_FILE_SHARE_NAME, AnsiToUtf8(item.m_szName, buf, MAX_BUFF_SIZE) );
    if ( LibJingleEx::FileShareManifest::T_FOLDER == item.m_eTyp )
    {
        fileshare->SetAttr( buzz::QN_FILE_SHARE_MODE, "folder" );
    }
    else
    {
        fileshare->SetAttr( buzz::QN_FILE_SHARE_MODE, "file" );
    }

    client_->SendStanza( iq );
    delete iq;
}

buzz::XmppReturnStatus XmppPump::SendStanza(const buzz::XmlElement *stanza) {
  if (!AllChildrenDone())
    return client_->SendStanza(stanza);
  return buzz::XMPP_RETURN_BADSTATE;
}


void XmppPump::OnStatusUpdate(const buzz::Status& status)
{
    if (notify_ != NULL)
        notify_->OnStatusUpdate(status);
}

void XmppPump::GetVCard( const LibJingleEx::TJid * ptJid )
{    
    // 发送stanza
    buzz::XmlElement * iq = new buzz::XmlElement(buzz::QN_IQ);

    // iq设置type属性
    iq->AddAttr( buzz::QN_TYPE, buzz::STR_GET );

    // iq设置id属性
    char buf[MAX_BUFF_SIZE];
    char randid[MAX_RAND_ID_LENGTH];
    GenerateRandomId( randid, MAX_RAND_ID_LENGTH );
    iq->AddAttr( buzz::QN_ID, randid );

    // 如果ptJid不为0，表示获取别人的Jid
    if ( 0 != ptJid )
    {
        char szJid[256];
        TJid2StrJid( *ptJid, szJid, 256 );

        iq->SetAttr( buzz::QN_TO, AnsiToUtf8(szJid, buf, MAX_BUFF_SIZE) );
    }

    buzz::XmlElement * vcard = new buzz::XmlElement( buzz::QN_VCARD, true );
    iq->AddElement( vcard );

    if ( client_ )
    {
        client_->SendStanza( iq );
    }
    delete iq;


    // 保存id到vcard task
    char * pchId = new char[MAX_RAND_ID_LENGTH];
    strcpy( pchId, randid );
    vcard_task_->m_vIds.push_back( pchId );

}

bool   XmppPump::AddIqId( const TIqId * ptIqId )
{
    if ( ptIqId->dwType == IQ_TYPE_PUBSUB )
    {
        if ( pubsub_task_ )
        {
            return pubsub_task_->AddIqId( ptIqId );
        }
    }
    else if ( IQ_TYPE_PRIVATE == ptIqId->dwType )
    {
        if ( private_task_ )
        {
            return private_task_->AddIqId( ptIqId );
        }
    }
    else if ( IQ_TYPE_PING == ptIqId->dwType )
    {
        if ( ping_task_ )
        {
            return ping_task_->AddIqId( ptIqId );
        }
    }
    else if ( IQ_TYPE_P2P_MESSAGE == ptIqId->dwType )
    {
        if ( msg_task_ )
        {
            return msg_task_->AddIqId( ptIqId );
        }
    }
    else if (  IQ_TYPE_GROUPCHAT_MESSAGE == ptIqId->dwType )
    {
        PTRoom pRoom = (PTRoom)ptIqId->pRoom;

        CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;

        vRoom::iterator it;
        for ( it = app->m_Rooms.begin(); it != app->m_Rooms.end(); it++ )
        {
            PTRoom  p = *it;
            if ( p == pRoom )
            {
                break;
            }
        }

        if ( it != app->m_Rooms.end() )
        {
            return pRoom->AddIqId( ptIqId );
        }
        else
        {
            return false;
        }
    }

    return false;
}


bool  XmppPump::DelIqId( const TIqId * ptIqId )
{
    if ( ptIqId->dwType == IQ_TYPE_PUBSUB )
    {
        if ( pubsub_task_ )
        {
            return pubsub_task_->DelIqId( ptIqId );
        }
    }
    else if ( IQ_TYPE_PRIVATE == ptIqId->dwType )
    {
        if ( private_task_ )
        {
            return private_task_->DelIqId( ptIqId );
        }
    }
    else if ( IQ_TYPE_PING == ptIqId->dwType )
    {
        if ( ping_task_ )
        {
            return ping_task_->DelIqId( ptIqId );
        }
    }

    return false;
}


bool XmppPump::IfPingIqIdExist( const TIqId * ptIqId, bool bDelete /*= false*/ )
{
    assert( ping_task_ );
    bool bRet = ping_task_->IfExistIqId( ptIqId );

    // 如果存在，并且需要删除它
    if ( bRet && bDelete )
    {
        ping_task_->DelIqId( ptIqId );
    }

    return bRet;
}

void XmppPump::OnXmppClientDestroyed()
{
    CLibJinglexApp * pAppObj    = (CLibJinglexApp *)m_hApp;
    cFuncLock cLock( &pAppObj->m_lock );
    client_  = 0;
}


void XmppPump::QueryPersistentRooms()
{
    assert( private_task_ );
    private_task_->GetPersistentRoomsInfo();
}

void XmppPump::QueryServerTime()
{
    assert( time_task_ );
    time_task_->QueryServerTime();
}

void XmppPump::CheckMsgResp()
{
    msg_task_->CheckMsgResp();
}