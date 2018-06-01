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

#include <time.h>

#include "talk/xmpp/xmppclientsettings.h"
#include "talk/examples/login/xmppthread.h"
#include "talk/examples/login/xmppauth.h"

#include <assert.h>
#include "libjingle/libjingle_inner.h"

namespace {

struct LoginData: public talk_base::MessageData {
    LoginData(const buzz::XmppClientSettings& s) : xcs(s) {}
  virtual ~LoginData() {}

  buzz::XmppClientSettings xcs;
};


struct ShareData: public talk_base::MessageData {
    ShareData( FileShareClient * pFileShareClient,
               const LibJingleEx::TJid              *    ptJid, 
               const LibJingleEx::FileShareManifest *    ptManifest,
               const char * szLocalDir ) : m_pFileShareClient(pFileShareClient)
    {
        memcpy( &m_tJid,      ptJid,      sizeof(LibJingleEx::TJid) );
        memcpy( &m_tManifest, ptManifest, sizeof(LibJingleEx::FileShareManifest) );

        if ( 0 == szLocalDir )
        {
            getcwd( m_szLocalDir, sizeof(m_szLocalDir) );
        }
        else
        {
            strncpy( m_szLocalDir, szLocalDir, LibJingleEx::MAX_PATH_SIZE );
        }
    }
    ~ShareData() { }

    LibJingleEx::TJid               m_tJid;
    LibJingleEx::FileShareManifest  m_tManifest;
    FileShareClient *               m_pFileShareClient;
    char                            m_szLocalDir[LibJingleEx::MAX_PATH_SIZE];
};

struct SessionData: public talk_base::MessageData {
    SessionData( cricket::FileShareSession * pSession, const char * szSavePath = 0 ) : m_pSession(pSession) 
    {
        memset( m_szSavePath, 0, sizeof( m_szSavePath ) );

        if ( 0 != szSavePath )
        {
            strncpy( m_szSavePath, szSavePath, LibJingleEx::MAX_PATH_SIZE - 1 );
            m_szSavePath[LibJingleEx::MAX_PATH_SIZE - 1] = '\0';
        }
    }
    ~SessionData() {}

    cricket::FileShareSession * m_pSession;
    char                        m_szSavePath[LibJingleEx::MAX_PATH_SIZE];
};

struct SessionHandle: public talk_base::MessageData {
    SessionHandle( FileShareClient * p ) : m_pFileShareClient(p) {}
    ~SessionHandle() {}

    FileShareClient * m_pFileShareClient;
};

struct GetVCardParam : public talk_base::MessageData {
    GetVCardParam( const LibJingleEx::TJid * p )
    {
        if ( p )
        {
            memcpy( &m_tJid, p, sizeof(m_tJid ) );
            m_bSelf = FALSE;
        }
        else
        {
            m_bSelf = TRUE;
        }
    }
    ~GetVCardParam() {}

    LibJingleEx::TJid m_tJid;
    BOOL              m_bSelf;
};

struct StanzaParam : public talk_base::MessageData
{
    StanzaParam( const buzz::XmlElement * stanza )
    {
        m_stanza = stanza;
    }
    ~StanzaParam()
    {
        if ( m_stanza )
        {
            delete m_stanza;
            m_stanza = 0;
        }
    }

    const buzz::XmlElement * m_stanza;
};

struct RawParam : public talk_base::MessageData
{
    RawParam( const char * raw )
    {
        if ( raw )
        {
            m_raw = new char[strlen(raw)+1];
            strcpy( m_raw, raw );
        }
        else
        {
            m_raw = 0;
        }
    }

    ~RawParam()
    {
        if ( m_raw )
        {
            delete m_raw;
            m_raw = 0;
        }
    }

    char * m_raw;
};


struct StanzaParamEx : public talk_base::MessageData
{
    StanzaParamEx( const buzz::XmlElement * stanza, const TIqId * ptIqId )
    {
        m_stanza = stanza;
        m_ptIqId = ptIqId;           // 在得到iq result后销毁内存
    }
    ~StanzaParamEx()
    {
        if ( m_stanza )
        {
            delete m_stanza;
            m_stanza = 0;
        }

        if ( m_ptIqId )
        {
            delete m_ptIqId;
            m_ptIqId = 0;
        }
    }

    const buzz::XmlElement * m_stanza;
    const TIqId *            m_ptIqId;
};


} // namespace

XmppThread::XmppThread( void * hApp ) {
  pump_ = new XmppPump( hApp, this );
  m_hApp = hApp;
  m_eSubCode = LibJingleEx::STATE_SUBCODE_NONE;
}

XmppThread::~XmppThread() {
  delete pump_;
}

void XmppThread::ProcessMessages(int cms) {
  talk_base::Thread::ProcessMessages(cms);
}

void XmppThread::Login(const buzz::XmppClientSettings& xcs) {
  if ( xcs.delay_login() )
  {
    unsigned long dwSpan      = MAX_RELOGIN_DELAY_TIME - MIN_RELOGIN_DELAY_TIME + 1;
    unsigned long dwDelayTime = MIN_RELOGIN_DELAY_TIME + rand() % dwSpan;

    PostDelayed( dwDelayTime * 1000, this, MSG_LOGIN, new LoginData(xcs) );
  }
  else
  {
    Post(this, MSG_LOGIN, new LoginData(xcs));
  }
}

void XmppThread::Disconnect() {
  Post(this, MSG_DISCONNECT);
}

void XmppThread::UpperReady()
{
  Post(this, MSG_UPPER_READY);
}

void XmppThread::ShareFiles( FileShareClient * pFileShareClient, const LibJingleEx::PTJid  ptJid,  
                             const LibJingleEx::FileShareManifest * pManifest, const char * szLocalDir )
{
    Post(this, MSG_SHARE_FILES, new ShareData( pFileShareClient, ptJid, pManifest, szLocalDir ) );
}

void XmppThread::AcceptFiles( void * hSession, const char * szSavePath /* = 0 */ )
{
    Post(this, MSG_ACCEPT_FILES, new SessionData( (cricket::FileShareSession *)hSession, szSavePath ) );
}

void XmppThread::DeclineFiles( void * hSession )
{
    Post(this, MSG_DECLINE_FILES, new SessionData( (cricket::FileShareSession *)hSession ) );
}

void XmppThread::CancelFiles( void * pFileShareClient )
{
    Post(this, MSG_CANCEL_FILES, new SessionHandle( (FileShareClient *)pFileShareClient ) );
}

void XmppThread::GetVCard( const LibJingleEx::TJid * ptJid )
{
    Post( this, MSG_GET_VCARD, new GetVCardParam(ptJid) );
}

void XmppThread::SendStanza( const buzz::XmlElement * stanza )
{
    Post( this, MSG_SEND_STANZA, new StanzaParam( stanza ) );
    
}

void XmppThread::SendRaw( const char * raw )
{
    Post( this, MSG_SEND_RAW, new RawParam( raw ) );
}

void XmppThread::SendStanza( const TIqId * ptIqId, const buzz::XmlElement * stanza )
{
    Post( this, MSG_SEND_STANZA_EX, new StanzaParamEx( stanza, ptIqId ) );
}

// 不接受房间邀请时，销毁内存(不在主线程销毁)
void XmppThread::DeleteRoom( PTRoom pRoom )
{
    Post( this, MSG_DELETE_ROOM_MEMORY, new DeleteRoomMemoryMsg( pRoom ) );
}

// 获取讨论组成员(非在线)
void XmppThread::GetChatRoomMembers( PTRoom pRoom )
{
    Post( this, MSG_GET_ROOM_MEMBERS, new CommonChatroomMsg( pRoom ) );
}

void XmppThread::Ping()
{
    Post( this, MSG_PING, 0 );
}

void XmppThread::SetTrueSocialName( const char * szJid, const char * szTureSocialName )
{
    Post( this, MSG_SET_TRUE_SOCIAL_NAME, new SetTrueSocialNameMsg( szJid, szTureSocialName ) );
}

void XmppThread::SendRoomPicture( PTRoom hRoom, const char * szPicFilePathName, unsigned long dwPicSize )
{
    Post( this, MSG_SEND_ROOM_PICTURE, new SendRoomPictureMsg( hRoom, szPicFilePathName, dwPicSize ) );
}

void XmppThread::OnStateChange(buzz::XmppEngine::State state, int nError, int nSubCode) {

    LibJingleEx::ELibjingleState e = (LibJingleEx::ELibjingleState)state;

    LibJingleEx::ELibjingleStateSubCode eSubCode = LibJingleEx::STATE_SUBCODE_NONE;

    if ( buzz::XmppEngine::STATE_CLOSED == state )
    {
        if ( buzz::XmppEngine::ERROR_UNAUTHORIZED == nError )
        {
            eSubCode = LibJingleEx::STATE_SUBCODE_UNAUTHORIED;
        }
        else if (  buzz::XmppEngine::ERROR_STREAM == nError &&  buzz::XmppEngine::ERROR_SUBCODE_CONFLICT == nSubCode )
        {
            eSubCode = LibJingleEx::STATE_SUBCODE_CONFLICT;
        }        
        else if ( 0 != nError )
        {
            eSubCode = LibJingleEx::STATE_SUBCODE_DISCONECTED;
        }

        m_eSubCode = eSubCode;
    }

    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock cLock( &app->m_lock );

    if ( g_tEvents.pOnLoginResult )
    {
        g_tEvents.pOnLoginResult( app, e, eSubCode, app->m_pUserData );
    }
    else if ( g_pfnOnMessage )
    {
        LibJingleEx::CMsgData cMsg;
        cMsg.Append( (unsigned char *)&e,        sizeof(e) );
        cMsg.Append( (unsigned char *)&eSubCode, sizeof(eSubCode) );

        g_pfnOnMessage( LibJingleEx::ON_STATE, app, app->m_pUserData, cMsg.GetMsgData(), cMsg.GetMsgLength() );
    }

    if ( buzz::XmppEngine::STATE_OPEN == state )
    {
        Post(this, MSG_AFTER_LOGIN);
    }
    // 在Libjingle::Logout()时回收线程资源
    //else if ( buzz::XmppEngine::STATE_CLOSED == state )
    //{
    //    CGlobalData * pGlobal = CGlobalData::GetInstance();
    //    pGlobal->EraseApp( (CLibJinglexApp*)m_hApp );
    //}

}

void XmppThread::OnMessage(talk_base::Message* pmsg) {

    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock   cLock( &app->m_lock );

    buzz::XmppClient * client = pump_->client();

    if ( 0 == client )
    {
        Post( this, MSG_TERMINATE_THREAD );
        return;
    }

    char   buf[MAX_BUFF_SIZE];
    char   szId[MAX_RAND_ID_LENGTH];


  if (pmsg->message_id == MSG_LOGIN) {
    assert(pmsg->pdata);

    unsigned int dwCurTime = (unsigned int)time(0);
    srand( dwCurTime );

    LoginData* data = reinterpret_cast<LoginData*>(pmsg->pdata);
    pump_->DoLogin(data->xcs, new XmppSocket( data->xcs ), new XmppAuth());
    delete data;
  } else if (pmsg->message_id == MSG_DISCONNECT) {

      MYLOG("=====[ MSG_DISCONNECT] \r\n");
 

      if ( client )
      {
          // 通知临时被关注的联系人自己的状态
          std::vector<LibJingleEx::PTJid>::iterator   it_c;
          for ( it_c = app->m_vQueryStatusContact.begin(); it_c != app->m_vQueryStatusContact.end(); it_c++ )
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

              item->SetAttr( buzz::QN_SHOW, "unavailable" );
              client->SendStanza( message );
              delete message;
          }
      }
      

      // 记录roster数据
      app->SaveRoster2File();



      pump_->TerminateAllFileShareSessions();

      // 如果不是被动断开，做一些处理
      if ( LibJingleEx::STATE_SUBCODE_NONE == m_eSubCode )
      {
          // 查看聊天室已读消息是否改变( xmppclient还没有销毁 )
          if ( app->m_bRoomReadMsgsChanged  )
          {
              TIqId  tIqId;
              tIqId.dwType  = IQ_TYPE_PRIVATE;

              // 保存已读的聊天室历史
              app->SavePrivateData( tIqId.szIqId, MAX_RAND_ID_LENGTH );
              // END 保存已读的聊天室历史


              // 等到回应后，再退出
              pump_->AddIqId( &tIqId );

              // 但是加个1秒定时器
              PostDelayed( LOGOUT_TIMEOUT, this, MSG_LOGOUT_TIMEOUT );


              app->m_bRoomReadMsgsChanged = FALSE;
          }
          else
          {
              Post(this, MSG_DISCONNECT2 );
              // 关闭线程泵
              // Post(this, MSG_TERMINATE_THREAD );
              PostDelayed( DELAYED_TERMINATE_TIME, this, MSG_TERMINATE_THREAD );
          }
      }
      else
      {
          // 关闭线程泵
          // Post(this, MSG_TERMINATE_THREAD );
          PostDelayed( DELAYED_TERMINATE_TIME, this, MSG_TERMINATE_THREAD );
      }


    //pump_->DoDisconnect();
  } else if (pmsg->message_id == MSG_DISCONNECT2 ) {
      pump_->DoDisconnect();
  } 
  else if ( MSG_LOGOUT_TIMEOUT == pmsg->message_id )
  {
      Post(this, MSG_DISCONNECT2 );
      // 关闭线程泵
      // Post(this, MSG_TERMINATE_THREAD );
      PostDelayed( DELAYED_TERMINATE_TIME, this, MSG_TERMINATE_THREAD );
  }
  else if ( pmsg->message_id == MSG_AFTER_LOGIN )
  {
      pump_->AfterLogin();

      PostDelayed( NOTIFY_UNSCRIBED_STATUS_INTERVAL, this, MSG_NOTIFY_UNSCRIBED_STATUS, 0 );
  }
  else if ( MSG_SHARE_FILES == pmsg->message_id )
  {
    ShareData * data = reinterpret_cast<ShareData *>(pmsg->pdata);

    pump_->AddFileShareClient( data->m_pFileShareClient );

    cricket::FileShareSession * pSession = pump_->file_share_session_client_->CreateFileShareSession( 
                                             data->m_pFileShareClient->m_dwNo, data->m_szLocalDir );
    
    data->m_pFileShareClient->m_pSession = pSession;
    data->m_pFileShareClient->m_hApp     = m_hApp;

    buzz::Jid  jid( data->m_tJid.sNode, data->m_tJid.sDomain, data->m_tJid.sResource );
    cricket::FileShareManifest manifest;
    FileShareManifestConvert( manifest, data->m_tManifest );

    pSession->Share( jid, &manifest );
    delete data;
  }
  else if ( MSG_ACCEPT_FILES == pmsg->message_id )
  {
    SessionData * data = reinterpret_cast<SessionData *>(pmsg->pdata);

    pump_->AcceptFiles( data->m_pSession, data->m_szSavePath );

    delete data;
  }
  else if ( MSG_DECLINE_FILES == pmsg->message_id )
  {
      SessionData * data = reinterpret_cast<SessionData *>(pmsg->pdata);

      pump_->DeclineFiles( data->m_pSession );

      delete data;
  }
  else if ( MSG_CANCEL_FILES == pmsg->message_id )
  {
      SessionHandle * data = reinterpret_cast<SessionHandle *>(pmsg->pdata);

      pump_->CancelFiles( data->m_pFileShareClient, TRUE, TRUE );

      delete data;
  }
  else if ( MSG_GET_VCARD == pmsg->message_id )
  {
     GetVCardParam * data = reinterpret_cast<GetVCardParam *>(pmsg->pdata);
    
     if ( data->m_bSelf )
     {
        pump_->GetVCard( 0 );
     }
     else
     {
        pump_->GetVCard( &data->m_tJid );
     }

     delete data;
  }
  else if ( MSG_SEND_STANZA == pmsg->message_id )
  {
      StanzaParam * data = reinterpret_cast<StanzaParam *>(pmsg->pdata);

      if ( data->m_stanza )
      {
          if ( client )
          {
              client->SendStanza( data->m_stanza );
          }
      }

      delete data;
  }
  else if ( MSG_SEND_RAW == pmsg->message_id )
  {
      RawParam * data = reinterpret_cast<RawParam *>(pmsg->pdata);

      if ( data->m_raw )
      {
          if ( client )
          {
              client->SendRaw( data->m_raw );
          }
      }

      delete data;
  }
  else if ( MSG_SEND_STANZA_EX == pmsg->message_id )
  {
      StanzaParamEx * data = reinterpret_cast<StanzaParamEx *>(pmsg->pdata);

      assert( data->m_stanza && data->m_ptIqId );
      if ( client )
      {
          client->SendStanza( data->m_stanza );
      }

      pump_->AddIqId( data->m_ptIqId );

      delete data;
  }
  else if ( MSG_DELETE_ROOM_MEMORY == pmsg->message_id )
  {
      DeleteRoomMemoryMsg * data = reinterpret_cast<DeleteRoomMemoryMsg *>(pmsg->pdata);

      vRoom::iterator it_room;
      for ( it_room = app->m_Rooms.begin(); it_room != app->m_Rooms.end(); it_room++ )
      {
          PTRoom pRoom = *it_room;
          if ( pRoom == data->m_pRoom )
          {
              app->m_Rooms.erase( it_room );
              delete pRoom;
              break;
          }
      }

      delete data;
  }
  else if ( MSG_GET_ROOM_MEMBERS == pmsg->message_id )
  {
      CommonChatroomMsg * data = reinterpret_cast<CommonChatroomMsg *>(pmsg->pdata);

      vRoom::iterator it_room;
      for ( it_room = app->m_Rooms.begin(); it_room != app->m_Rooms.end(); it_room++ )
      {
          PTRoom pRoom = *it_room;
          if ( pRoom == data->m_pRoom )
          {
#if CHATROOM_SUPPORT_MEMBER_AFFILIATION
              if (    LibJingleEx::AFFILIATION_MEMBER == pRoom->m_eAffiliation 
                   || LibJingleEx::AFFILIATION_ADMIN  == pRoom->m_eAffiliation 
                   || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation  )
              {
                  GetRoomRoster( client, pRoom );
                  pRoom->m_bGetMembers = TRUE;
              }
#else
              if ( LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation  )
              {
                  GetRoomRoster( client, pRoom );
                  pRoom->m_bGetMembers = TRUE;
              }
#endif
          }
      }

      delete data;
  }
  else if ( MSG_SUBJECT_DELAY == pmsg->message_id )
  {
      SubjectDelayMsg * pDelayMsg = reinterpret_cast<SubjectDelayMsg *>(pmsg->pdata);

      vRoom::iterator it_room = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
                                         FindRoomByFullId( pDelayMsg->szRoomId, pDelayMsg->szGroupChatServiceName ) );
      // 如果有这个房间
      if ( it_room != app->m_Rooms.end() )
      {
          PTRoom pRoom = *it_room;    

          const  char * szFormat = 0;
          
          if ( ROOM_TYPE_NORMAL == pRoom->m_nType )
          {
              // 如果delay message id没变，则认为是最后的delay id
              if ( 0 == strcmp( pRoom->m_szLastSubjectDelayMsgId, pDelayMsg->szSubjectMsgId ) )
              {
                  pRoom->m_bGetSubject = TRUE;

                  // 如果是管理员，则处理退出房间请求
                  if ( LibJingleEx::AFFILIATION_OWNER ==  pRoom->m_eAffiliation && pRoom->m_vQueryQuitMembers.size() > 0 )
                  {
                      szFormat = "<iq  id='%s' to='%s@%s' type='set'>"
                                      "<query xmlns='http://jabber.org/protocol/muc#admin'>"
                                           "<item jid='%s@%s' affiliation='none' />"
                                      "</query>"
                                  "</iq>";     

                      std::vector<LibJingleEx::PTJid>::iterator it_x;
                      for ( it_x = pRoom->m_vQueryQuitMembers.begin(); it_x != pRoom->m_vQueryQuitMembers.end(); it_x++ )
                      {
                          LibJingleEx::PTJid  ptJid = *it_x;

                          GenerateRandomId( szId, MAX_RAND_ID_LENGTH );
                          SNPRINTF( buf, MAX_BUFF_SIZE, szFormat, szId, pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, 
                                                        ptJid->sNode, ptJid->sDomain );
                          // 请求服务器删除成员
                          if ( client )
                          {
                              client->SendRaw( buf );
                          }
                      }

                      // 清空subject
                      GenerateRandomId( szId, MAX_RAND_ID_LENGTH );
                      szFormat = "<message to='%s@%s' type='groupchat' id='%s'><subject/></message>";
                      SNPRINTF( buf, MAX_BUFF_SIZE, szFormat, pRoom->m_szRoomId, pRoom->m_szGroupChatServiceName, szId );
                      if ( client )
                      {
                          client->SendRaw( buf );
                      }

                      ClearVector( pRoom->m_vQueryQuitMembers );
                  }

#if CHATROOM_SUPPORT_MEMBER_AFFILIATION
                  // 如果还没有去获取成员而且有权限, 记得去获取成员
                  if (  pRoom->m_bGetSubject  &&  !pRoom->m_bGetMembers 
                      && ( LibJingleEx::AFFILIATION_MEMBER == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
                  {
                      GetRoomRoster( client, pRoom );
                      pRoom->m_bGetMembers = TRUE;
                  }
                  // END(如果有权限，则开始请求roster items)
#else
                  // 如果还没有去获取成员而且有权限, 记得去获取成员
                  if (  pRoom->m_bGetSubject  &&  !pRoom->m_bGetMembers 
                      && ( LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
                  {
                      GetRoomRoster( client, pRoom );
                      pRoom->m_bGetMembers = TRUE;
                  }
                  // END(如果有权限，则开始请求roster items)
#endif
                  

              }
          }
      }

      
      delete pDelayMsg;
  }
  else if ( MSG_GET_CHATROOM_MEMBERS_DELAY == pmsg->message_id )
  {
      SubjectDelayMsg * pDelayMsg = reinterpret_cast<SubjectDelayMsg *>(pmsg->pdata);

      vRoom::iterator it_room = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
          FindRoomByFullId( pDelayMsg->szRoomId, pDelayMsg->szGroupChatServiceName ) );
      // 如果有这个房间
      if ( it_room != app->m_Rooms.end() )
      {
          PTRoom pRoom = *it_room;    
          if ( ROOM_TYPE_NORMAL == pRoom->m_nType )
          {
#if CHATROOM_SUPPORT_MEMBER_AFFILIATION
              // 如果还没有去获取成员而且有权限, 记得去获取成员
              if (  !pRoom->m_bGetMembers 
                  && ( LibJingleEx::AFFILIATION_MEMBER == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
              {
                  GetRoomRoster( client, pRoom );
                  pRoom->m_bGetMembers = TRUE;
              }
              // END(如果有权限，则开始请求roster items)
#else
              // 如果还没有去获取成员而且有权限, 记得去获取成员
              if (  !pRoom->m_bGetMembers 
                  && ( LibJingleEx::AFFILIATION_ADMIN == pRoom->m_eAffiliation || LibJingleEx::AFFILIATION_OWNER == pRoom->m_eAffiliation ) )
              {
                  GetRoomRoster( client, pRoom );
                  pRoom->m_bGetMembers = TRUE;
              }
              // END(如果有权限，则开始请求roster items)
#endif
          }
      }

      delete pDelayMsg;
  }
  else if ( MSG_CHATROOM_MSGS_DELAY_CHECK == pmsg->message_id )
  {
      assert( 0 );
      //ChatroomMsgsCheckMsg * pDelayMsg = reinterpret_cast<ChatroomMsgsCheckMsg *>(pmsg->pdata);
      //vRoom::iterator it_room = find_if( app->m_Rooms.begin(), app->m_Rooms.end(), 
      //                                   FindRoomByFullId( pDelayMsg->szRoomId, pDelayMsg->szGroupChatServiceName ) );
      //// 如果有这个房间
      //if ( it_room != app->m_Rooms.end() )
      //{
      //    PTRoom pRoom = *it_room;     
      //    app->NotifyBufferedChatRoomMsgs( pRoom );
      //}
      //delete pDelayMsg;
  }
  else if ( MSG_PING_TIMEOUT == pmsg->message_id )
  {
      PingTimeoutMsg * pPintTimeoutMsg = reinterpret_cast<PingTimeoutMsg *>(pmsg->pdata);
    
      // 如果30秒后，这个ping还没有得到result回应(应为请求的iq还在)
      if ( 0 == client || 0 == pump_->ping_task_ || pump_->IfPingIqIdExist( &pPintTimeoutMsg->m_tIqId, true ) )
      {
          app->m_dwPingTimeoutCnt++;

          if ( app->m_dwPingTimeoutCnt >= MAX_PING_TIME_OUT_COUNT )
          {
              MYLOG("==== [have not received ping result %u times, quit] \r\n", MAX_PING_TIME_OUT_COUNT );

              app->m_pXmppThread->Post(app->m_pXmppThread, MSG_DISCONNECT2 );
              // 关闭线程泵
              // app->m_pXmppThread->Post(app->m_pXmppThread, talk_base::Thread::MSG_TERMINATE_THREAD );
              app->m_pXmppThread->PostDelayed( DELAYED_TERMINATE_TIME, app->m_pXmppThread, talk_base::Thread::MSG_TERMINATE_THREAD );
          }
      }

      delete pPintTimeoutMsg;
  }
  else if ( MSG_INNER_LOOP == pmsg->message_id )
  {
      // 定时保存聊天室已读消息
      if ( app->m_bRoomReadMsgsChanged )
      {
          TIqId  tIqId;
          tIqId.dwType  = IQ_TYPE_PRIVATE;

          // 保存已读的聊天室历史
          app->SavePrivateData( tIqId.szIqId, MAX_RAND_ID_LENGTH );
          // END 保存已读的聊天室历史

          app->m_bRoomReadMsgsChanged = FALSE;
      }

      // 如果还没有收到固定讨论组应答，再发送请求
      if ( !app->m_bPersistentRoomsResponsed )
      {
         pump_->QueryPersistentRooms();
      }

      pump_->QueryServerTime();

      // 检查是否有消息没有收到回应
      pump_->CheckMsgResp();

      app->SendRoomPictureNextStep( 0 );

      this->PostDelayed( INNER_LOOP_TIMEOUT, this, MSG_INNER_LOOP );
  }
  else if ( MSG_UPPER_READY == pmsg->message_id )
  {
      app->m_bUpperAppReady = TRUE;
      app->HandleDelayInvite2ChatRoomMsgs();

      // 进入企业状态推送聊天室
      app->EnterEnterpriseStatusRoom();
  }
  else if ( MSG_NOTIFY_UNSCRIBED_STATUS == pmsg->message_id )
  {
      std::vector<PTUnscribedContact> & v = app->m_vUnscribedContacts;
      std::vector<PTUnscribedContact>::iterator it;

      for ( it = v.begin(); it != v.end(); )
      {
          PTUnscribedContact  p = *it;

          // 3次ping都没有响应
          if ( p->dwFailCnt >= MAX_UNSCRIBED_CONTACT_PING_FAIL_COUNT )
          {
              LibJingleEx::TJid2StrJid( p->tJid, buf, MAX_BUFF_SIZE );
              buzz::Jid      jid_( buf );
              buzz::Status   status;
              status.set_jid( jid_ );
              status.set_show( (buzz::Status::Show)LibJingleEx::PRESENCE_NONE );
           
              // 通知上层下线
              OnStatusUpdate( status );

              delete p;
              it = v.erase( it );
              continue;
          }
          else
          {
              it++;
          }
      }

      // 向所有的unscribed 联系人发送ping
      for ( it = v.begin(); it != v.end(); it++ )
      {
          PTUnscribedContact  p = *it;

          GenerateRandomId( szId, MAX_RAND_ID_LENGTH, PREFIX_PING );
          SNPRINTF( buf, MAX_BUFF_SIZE,  "<iq type='get' id='%s' to='%s@%s/%s'>"
                                          "<ping xmlns='urn:xmpp:ping'/>"
                                         "</iq>",  szId, p->tJid.sNode, p->tJid.sDomain, p->tJid.sResource );
          client->SendRaw( buf );

          p->dwFailCnt++;
      }

      

      PostDelayed( NOTIFY_UNSCRIBED_STATUS_INTERVAL, this, MSG_NOTIFY_UNSCRIBED_STATUS, 0 );
  }
  else if ( MSG_PING == pmsg->message_id )
  {
      if ( client )
      {
          char szRandId[32];
          GenerateRandomId( szRandId, 32, PREFIX_PING );

          char buf[MAX_BUFF_SIZE];
          SNPRINTF( buf, MAX_BUFF_SIZE,  "<iq type='get' id='%s'>"
                                           "<ping xmlns='urn:xmpp:ping'/>"
                                         "</iq>",  szRandId );

          TIqId  tIqId;
          tIqId.dwType = IQ_TYPE_PING;
          strcpy( tIqId.szIqId, szRandId );

          // 先保存在ping task中
          pump_->AddIqId( &tIqId );    

          // 再发送ping
          client->SendRaw( buf );

          // 10秒后检查有无收到ping应带
          PostDelayed( PING_TIMEOUT, this,  MSG_PING_TIMEOUT, new PingTimeoutMsg( &tIqId ) );          
      }
  }
  else if ( MSG_SURE_SEND_STATUS == pmsg->message_id )
  {
      // 如果超时上层应用还没有发送status
      if ( !app->m_bSendStatus )
      {
          buzz::XmlElement * presence = new buzz::XmlElement(buzz::QN_PRESENCE);
          const char * szPresence = GetPresShowString( app->m_ePres );
          if ( szPresence )
          {
              buzz::XmlElement * show   = new buzz::XmlElement(buzz::QN_SHOW );
              presence->AddElement( show );
              show->SetBodyText( szPresence );
          }

          buzz::XmlElement * priority = new buzz::XmlElement( buzz::QN_PRIORITY );
          presence->AddElement( priority );
          priority->SetBodyText("1");

          std::string   ability;
          if ( GetAbilityString( ability, app ) )
          {
              presence->AddElement( new buzz::XmlElement( buzz::QN_CAPS_C, true) );
              presence->AddAttr( buzz::QN_NODE, "http://www.google.com/xmpp/client/caps", 1);
              MyGetVersion( buf, MAX_BUFF_SIZE, TERMINAL_VERSION );
              presence->AddAttr( buzz::QN_VER,  buf, 1);
              presence->AddAttr( buzz::QN_EXT, ability, 1);
          }

          client->SendStanza( presence );
          delete presence;

          app->m_bSendStatus = TRUE;
      }
  }
  else if ( MSG_RE_ENTER_CHATROOM == pmsg->message_id )
  {
      ChatRoomMsg * pChatRoomMsg = reinterpret_cast<ChatRoomMsg *>(pmsg->pdata);

      // 进入房间
      buzz::XmlElement * presence = new buzz::XmlElement( buzz::QN_PRESENCE );

      std::string sTmp;
      sTmp  = pChatRoomMsg->m_szRoomId;
      sTmp += '@';
      sTmp += pChatRoomMsg->m_szGroupChatServiceName;
      sTmp += '/';
      sTmp += client->jid().Str();
      presence->SetAttr( buzz::QN_TO, AnsiToUtf8( sTmp.c_str(), buf, MAX_BUFF_SIZE ) );

      const char * szPresence = GetPresShowString( app->m_ePres );
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
      if ( GetAbilityString( ability, app ) )
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
      history->SetAttr( buzz::QN_MAX_STANZAS, "0" );
      client->SendStanza( presence );

      delete presence;


      PTRoom pRoom = new TRoom(app);
      strcpy( pRoom->m_szRoomId,               pChatRoomMsg->m_szRoomId );
      strcpy( pRoom->m_szGroupChatServiceName, pChatRoomMsg->m_szGroupChatServiceName );
      strcpy( pRoom->m_szName,                 pChatRoomMsg->m_szName );
      pRoom->m_bPersistent = pChatRoomMsg->m_bPersistent;
      pRoom->m_nType       = pChatRoomMsg->m_nType;
      pRoom->m_eState      = ROOM_STATE_ON_PRESENCE;

      app->m_Rooms.push_back( pRoom );


      delete  pChatRoomMsg;
  }
  else if ( MSG_SET_TRUE_SOCIAL_NAME == pmsg->message_id )
  {
      SetTrueSocialNameMsg * pData = reinterpret_cast<SetTrueSocialNameMsg *>(pmsg->pdata);

      LibJingleEx::TJid  tJid;
      LibJingleEx::StrJid2TJid( pData->m_szJid, tJid );

      vMemberGroups::iterator it_find = find_if( app->m_MemberGroups.begin(), app->m_MemberGroups.end(), FindMemberGroupObj(&tJid) );
      if ( it_find != app->m_MemberGroups.end() )
      {
          PTMemberGroup  pMg= *it_find;
          strncpy( pMg->m_szTrueSocialName, pData->m_szTrueSocialName,  MAX_TURE_SOCIAL_NAME_LEN );
      }

      delete pData;
  }
  else if ( MSG_SEND_ROOM_PICTURE == pmsg->message_id )
  {
      SendRoomPictureMsg * pData = reinterpret_cast<SendRoomPictureMsg *>(pmsg->pdata);

      // 添加到vector
      TSendRoomPictureParam * pParam = new TSendRoomPictureParam;
      pParam->m_pRoom  = pData->m_pRoom;
      strcpy( pParam->m_szPictureFilePath, pData->m_szPicturePath );
      pParam->m_dwPictureSize = pData->m_dwPictureSize;

      app->m_vSendRoomPictureParams.push_back( pParam );
      // end (添加到vector)
      

      app->SendRoomPictureNextStep( 0 );


      delete pData;
  }
  else
  {
    assert(false);
  }
}

void XmppThread::OnStatusUpdate(const buzz::Status& status )
{
    // 是否微博的状态推送，忽略掉微博的状态
    // 计算总状态和能力时也忽略掉
    bool    bWeibo = IsIgnoredResource( status.jid().resource() );
    if ( bWeibo )
    {
        return;
    }


    CLibJinglexApp * app = (CLibJinglexApp *)m_hApp;
    cFuncLock cLock( &app->m_lock );

    buzz::XmppClient * c = client();

    char buf[MAX_BUFF_SIZE];

    LibJingleEx::TPresence tPrensence;	
    memset( &tPrensence, 0, sizeof(LibJingleEx::TPresence) );

    const buzz::Jid & jid = status.jid();

    strncpy( tPrensence.tJid.sNode,     jid.node().c_str(),     LibJingleEx::MAX_NODE_LEN - 1 );
    strncpy( tPrensence.tJid.sDomain,   jid.domain().c_str(),   LibJingleEx::MAX_DOMAIN_LEN - 1 );
    strncpy( tPrensence.tJid.sResource, jid.resource().c_str(), LibJingleEx::MAX_RESOURCE_LEN - 1 );

    tPrensence.ePresence = (LibJingleEx::EPresence)status.show();

    strncpy( tPrensence.sDescription,   status.status().c_str(), LibJingleEx::MAX_PRESENCE_DESCRIPTION - 1 );

    BOOL  bDelay = status.is_delay();

    LibJingleEx::TJid tJid;
    memcpy( &tJid, &tPrensence.tJid, sizeof(LibJingleEx::TJid) );
    tJid.sResource[0] = '\0';

    vMemberGroups & v = app->m_MemberGroups;
    vMemberGroups::iterator it_find = find_if( v.begin(), v.end(), FindMemberGroupObj( &tJid ) ) ;

    PTMemberGroup p = 0;
    // 如果没有就添加
    if ( it_find == v.end() )
    {
        p = new TMemberGroup;
        memcpy( &p->m_tJid, &tJid, sizeof(LibJingleEx::TJid) );

        v.push_back( p );
    }
    else
    {
        p = *it_find;
    }

    vMemberStatus & s = p->m_Status;
    vMemberStatus::iterator it;
    BOOL bFind = FALSE;

    PTMemberStatus  pMemberStatus = 0;
    BOOL  bChanged  = FALSE;                       // 该状态推送和member当前的状态比较后，是否有改变

    for ( it = s.begin(); it != s.end(); it++ )
    {
        PTMemberStatus pStatus = *it;

        // 找到resource一样的
        if ( 0 == strcmp(pStatus->m_szResource, tPrensence.tJid.sResource) )
        {
            // 如果状态记录为来自聊天室
            if ( pStatus->m_bFromStatusRoom )
            {
                // 但是这个状态不来自聊天室，则不上报状态
                if ( !status.from_status_chatroom() && !bDelay )
                {
                    return;
                }
            }
            // 如果当前状态不是来自聊天室(来自自己关注的人)
            else
            {
                if ( status.from_status_chatroom() )
                {
                    pStatus->m_bFromStatusRoom = TRUE;
                }
            }

            // 下线
            if ( tPrensence.ePresence == LibJingleEx::PRESENCE_NONE )
            {
                if ( !pStatus->m_bFromStatusRoom || status.from_status_chatroom() )
                {
                    pump_->OnFriendOffline( &tPrensence.tJid );

                    delete pStatus;
                    s.erase( it );

                    // 因为不可能保存PRESENCE_NONE的状态，所以认为状态有改变
                    bChanged = TRUE;
                }
            }
            else
            {
                // 如果没带能力集
                if ( status.ignore_capability() )
                {
                    // 如果状态不一样，则认为状态有改变
                    if ( pStatus->m_ePresence != tPrensence.ePresence )
                    {
                        bChanged = TRUE;          
                    }
                }
                else
                {
                    // 如果状态不一样，则认为状态有改变
                    if ( pStatus->m_ePresence != tPrensence.ePresence )
                    {
                        bChanged = TRUE;          
                    }
                    else if ( pStatus->m_bFileShareAvailable != GET_BOOL(status.fileshare_capability())  )
                    {
                        bChanged = TRUE;
                    }
                    else if ( pStatus->m_bAudio != GET_BOOL(status.audio_capability())  )
                    {
                        bChanged = TRUE;
                    }
                    else if ( pStatus->m_bVideo != GET_BOOL(status.video_capability())  )
                    {
                        bChanged = TRUE;
                    }
                    else if ( pStatus->m_bCall != GET_BOOL(status.call_capability())  )
                    {
                        bChanged = TRUE;
                    }
                }


                pStatus->m_ePresence = tPrensence.ePresence;

                if ( !status.ignore_capability() )
                {
                    // 记录文件传输能力
                    pStatus->m_bFileShareAvailable = ( status.fileshare_capability() ? TRUE : FALSE );

                    pStatus->m_bAudio = ( status.audio_capability() ? TRUE : FALSE );
                    pStatus->m_bVideo = ( status.video_capability() ? TRUE : FALSE );
                    pStatus->m_bCall  = ( status.call_capability() ?  TRUE : FALSE );

                }

                pStatus->m_dwLastTick = GetCurTick();
                pStatus->m_dwVersion  = ConvertVersion( status.version().c_str() );

                pMemberStatus = pStatus;
            }
            bFind = TRUE;
            break;
        }
    }

    // 是否是某个实例第一次状态改变
    BOOL  bAdded = FALSE;

    // 是否在自己之前登录
    BOOL  bLoginBeforeMe = FALSE;

    // 没有找到
    if ( !bFind )
    {
        // 添加
        if ( tPrensence.ePresence != LibJingleEx::PRESENCE_NONE )
        {
            pMemberStatus = new TMemberStatus;
            strcpy( pMemberStatus->m_szResource, tPrensence.tJid.sResource );
            pMemberStatus->m_ePresence = tPrensence.ePresence;

            if ( !status.ignore_capability() )
            {
                // 记录文件传输能力
                pMemberStatus->m_bFileShareAvailable = ( status.fileshare_capability() ? TRUE : FALSE );

                pMemberStatus->m_bAudio = ( status.audio_capability() ? TRUE : FALSE );
                pMemberStatus->m_bVideo = ( status.video_capability() ? TRUE : FALSE );
                pMemberStatus->m_bCall  = ( status.call_capability() ?  TRUE : FALSE );

            }

            pMemberStatus->m_bFromStatusRoom = ( status.from_status_chatroom() ? TRUE : FALSE );
            pMemberStatus->m_dwLastTick = GetCurTick();
            pMemberStatus->m_dwVersion  = ConvertVersion( status.version().c_str() );

            s.push_back( pMemberStatus );

            bAdded   = TRUE;
            bChanged = TRUE;

            bLoginBeforeMe = ( pMemberStatus->m_dwLastTick - app->m_dwLoginTick > LOGIN_AFTER_TIME_DIFF ? FALSE : TRUE );
        }
        // 既没有找到又是Presence_None(有可能是因为聊天室和好友两方面推送过来导致的)
        else
        {
            // return;
        }
    }


    if ( pMemberStatus )
    {
        assert( tPrensence.ePresence != LibJingleEx::PRESENCE_NONE );

        tPrensence.bFileShare = pMemberStatus->m_bFileShareAvailable;
        tPrensence.bAudio     = pMemberStatus->m_bAudio;
        tPrensence.bVideo     = pMemberStatus->m_bVideo;
        tPrensence.bCall      = pMemberStatus->m_bCall;
    }
    



    // 找到最大的状态值
    LibJingleEx::EPresence ePres_max = LibJingleEx::PRESENCE_NONE;
    unsigned dwMaxWeight = GetPresenceWeight(ePres_max);

    BOOL   bMaxAudio = FALSE;              // 当连上GK，并且有音频能力，才能设置为TRUE
    BOOL   bMaxVideo = FALSE;              // 当连上GK，并且有视频能力，才能设置为TRUE

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
    // END( 找到最大的状态值 )



    // 是否同一账号
    BOOL bSelf = FALSE;

    // 如果是同一账号
    if ( c->jid().BareJid().Str() == jid.BareJid().Str() )
    {
        bSelf = TRUE;

        // full jid相同, 不用告知上层
        if ( c->jid().resource() == jid.resource() )
        {
            return;
        }
    }

    if ( bSelf && bAdded )
    {
        std::vector<LibJingleEx::PTJid>::iterator it_c;
        for ( it_c = app->m_vQueryStatusContact.begin(); it_c != app->m_vQueryStatusContact.end(); it_c++ )
        {
            LibJingleEx::PTJid ptJid = *it_c;

            buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
            message->SetAttr( buzz::QN_TO,   c->jid().BareJid().Str() );
            message->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );

            buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
            message->AddElement( operation );
            operation->SetAttr( buzz::QN_TYPE, "get status" );

            buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM, true );
            operation->AddElement( item );

            LibJingleEx::TJid2StrJid( *ptJid, buf, MAX_BUFF_SIZE );
            item->SetAttr( buzz::QN_ID, buf );

            c->SendStanza( message );
            delete message;
        }

#if SYNC_CHATROOM_FLAG
        // 邀请进入临时讨论组
        vRoom::iterator  it_room;
        for ( it_room = app->m_Rooms.begin();  it_room != app->m_Rooms.end();  it_room++ )
        {   
            PTRoom   pRoom = *it_room;
            if ( pRoom->m_nType == ROOM_TYPE_NORMAL && !pRoom->m_bPersistent && ROOM_STATE_AVAILABLE == pRoom->m_eState )
            {
                // 同步其他终端进入聊天室
                buzz::XmlElement * message = new buzz::XmlElement( buzz::QN_MESSAGE );
                message->SetAttr( buzz::QN_TO,   c->jid().BareJid().Str() );
                message->SetAttr( buzz::QN_TYPE, INSTANT_MESSAGE_TYPE );

                buzz::XmlElement * operation = new buzz::XmlElement( buzz::QN_OPERATION, true );
                message->AddElement( operation );
                operation->SetAttr( buzz::QN_TYPE, OPERATION_TYPE_SYNC_CHATROOM );

                buzz::XmlElement * item = new buzz::XmlElement( buzz::QN_DMS_ITEM, true );
                operation->AddElement( item );

                item->SetAttr( buzz::QN_ROOM_ID,          pRoom->m_szRoomId  );
                item->SetAttr( buzz::QN_CHATROOM_SERVICE, pRoom->m_szGroupChatServiceName );
                item->SetAttr( buzz::QN_TYPE,             "temporary" );
                item->SetAttr( buzz::QN_NAME,             AnsiToUtf8(pRoom->m_szName, buf, MAX_BUFF_SIZE ) );

                c->SendStanza( message );
                delete message;
                // END
            }
        }
        // END( 邀请进入临时讨论组 )
#endif

    }

    if ( g_tEvents.pOnRosterPresenceChange )
    {
        g_tEvents.pOnRosterPresenceChange( app, &tPrensence );
    }
    else if ( g_pfnOnMessage )
    {
        BOOL bFromStatusChatRoom = ( status.from_status_chatroom() ? TRUE : FALSE  );

        LibJingleEx::CMsgData  msgData;

        msgData.Append( (unsigned char *)&tPrensence,              sizeof(tPrensence) );
        msgData.Append( (unsigned char *)&bSelf,                   sizeof(bSelf) );
        msgData.Append( (unsigned char *)&ePres_max,               sizeof(ePres_max) );
        msgData.Append( (unsigned char *)&bAdded,                  sizeof(bAdded) );
        msgData.Append( (unsigned char *)&bMaxAudio,               sizeof(bMaxAudio) );
        msgData.Append( (unsigned char *)&bMaxVideo,               sizeof(bMaxVideo) );
        msgData.Append( (unsigned char *)&bFromStatusChatRoom,     sizeof(bFromStatusChatRoom) );
        msgData.Append( (unsigned char *)&bChanged,                sizeof(bChanged) );
        msgData.Append( (unsigned char *)&bDelay,                  sizeof(bDelay) );
        msgData.Append( (unsigned char *)&bLoginBeforeMe,          sizeof(bLoginBeforeMe) );

        g_pfnOnMessage( LibJingleEx::ON_FRIEND_PRESENCE, app, app->m_pUserData, 
                        msgData.GetMsgData(), msgData.GetMsgLength() );


        // 如果是同一账号在其他终端类型登录，而且还没有安全性通知
        if ( bSelf && bAdded && !app->m_bNotifySecurity )
        {
            app->m_bNotifySecurity = TRUE;
            g_pfnOnMessage( LibJingleEx::ON_NOTIFY_SECURITY, app, app->m_pUserData, 0, 0 );
        }
    }
}

LibJingleEx::ELibjingleStateSubCode  XmppThread::GetErrcode()
{
    return m_eSubCode;
}

