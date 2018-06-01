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

#ifndef _XMPPPUMP_H_
#define _XMPPPUMP_H_

#include "talk/base/messagequeue.h"
#include "talk/base/taskrunner.h"
#include "talk/base/thread.h"
#include "talk/base/time_.h"
#include "talk/xmpp/xmppclient.h"
#include "talk/xmpp/xmppengine.h"
#include "talk/xmpp/xmpptask.h"
#include "talk/examples/login/presencepushtask.h"
#include "talk/examples/login/presenceouttask.h"
#include "talk/p2p/base/sessionmanager.h"
#include "talk/p2p/client/sessionmanagertask.h"

// Simple xmpp pump

class XmppPumpNotify {
public:
  virtual void OnStateChange(buzz::XmppEngine::State state, int nError, int nSubCode) = 0;
  virtual void OnStatusUpdate(const buzz::Status& status) = 0;
};

class XmppPump : public talk_base::MessageHandler, public talk_base::TaskRunner {
public:
    XmppPump( void * hApp, XmppPumpNotify * notify );
    ~XmppPump();

  void Clear();

  buzz::XmppClient *client() { return client_; }

  void DoLogin(const buzz::XmppClientSettings & xcs,
               buzz::AsyncSocket* socket,
               buzz::PreXmppAuth* auth);

  void AfterLogin();

  void DoDisconnect();

  void OnStateChange(buzz::XmppEngine::State state, int nError, int nSubCode);

  void WakeTasks();

  int64 CurrentTime();

  void OnMessage(talk_base::Message *pmsg);

  void OnStatusUpdate(const buzz::Status& status);

  buzz::XmppReturnStatus SendStanza(const buzz::XmlElement *stanza);

  void AddFileShareClient( FileShareClient * p );

  void RemoveFileShareClient( FileShareClient * pFileShareClient );

  BOOL IfExistFileShareClient( FileShareClient * pSession );

  FileShareClient * FindFileShareClientBySessionId( const char * szPeerJid, const char * szSessionId );

  void AcceptFiles( cricket::FileShareSession * pSession, const char * szSavePath = 0 );

  void DeclineFiles( cricket::FileShareSession * pSession );

  void CancelFiles( FileShareClient * pFileShareClient, BOOL bLocal, BOOL bSendIq, int nError = 0 );

  void * m_hApp;

  bool OnFileShareComplete( const char * szPeerJid, const char * szSessionId, const char * szPath );

  void TerminateAllFileShareSessions();

  void OnFileShareResult( PTFileShareMsg p );

  FileShareClient *   FindFileShareObject( const char * szJid,  const char * szSessionId );

  void GetVCard( const LibJingleEx::TJid * ptJid );


  void OnXmppClientDestroyed();

  // 当好友掉线的时候，检查文件传输Session
  void OnFriendOffline( const LibJingleEx::TJid * ptJid );

  void QueryPersistentRooms();

  void QueryServerTime();

  void CheckMsgResp();

private:
  buzz::XmppClient *client_;

  buzz::PresencePushTask* presence_push_;
  buzz::PresenceOutTask*  presence_out_;
  buzz::MyRosterTask *    roster_task_;
  buzz::MyMsgTask    *    msg_task_;
  buzz::MyDiscoTask  *    disco_task_;
  buzz::MyChatRoomTask *  chatroom_task_;
  buzz::FileShareTask *   fileshare_task_;
  buzz::MyPrivateTask *   private_task_;
  buzz::MyVcardTask *     vcard_task_;
  buzz::MyPubsubTask *    pubsub_task_;
  buzz::MyPrivacyTask *   privacy_task_;
  buzz::MyTimeTask *      time_task_;

public:
  buzz::PingTask *        ping_task_;

private:
  cricket::SessionManagerTask * session_manager_task_;

public:
  talk_base::NetworkManager                              network_manager_;
  talk_base::scoped_ptr<cricket::HttpPortAllocator>      port_allocator_;
  talk_base::scoped_ptr<cricket::SessionManager>         session_manager_;
  talk_base::scoped_ptr<cricket::FileShareSessionClient> file_share_session_client_;

private:
  buzz::XmppEngine::State state_;
  XmppPumpNotify *notify_;


  vFileShareClient   m_vFileShareClients;
  JLockType          m_tFileShareClientLock;

  void OnFileShareSessionCreate(cricket::FileShareSession *sess);


private:
    void   BeginFileShare( FileShareClient * ptFileShare);
    void   TransferData( const char * szBase64, FileShareClient * p );
    void   EndTransfer( FileShareClient * p );
    void   CancelTransfer( FileShareClient * p );

public:
    bool   AddIqId( const TIqId * ptIqId );
    bool   DelIqId( const TIqId * ptIqId );
    bool   IfPingIqIdExist( const TIqId * ptIqId, bool bDelete = false );
};

#endif // _XMPPPUMP_H_
