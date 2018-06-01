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

#ifndef _XMPPTHREAD_H_
#define _XMPPTHREAD_H_

#include "talk/xmpp/xmppclientsettings.h"
#include "talk/base/thread.h"
#include "talk/examples/login/xmpppump.h"
#include "talk/examples/login/xmppsocket.h"
#include <iostream>

class XmppThread:
    public talk_base::Thread, public XmppPumpNotify, public talk_base::MessageHandler {
public:
  XmppThread( void * hApp);
  ~XmppThread();

  buzz::XmppClient* client() { return pump_->client(); }
  XmppPump*   pump()         { return pump_; }

  void ProcessMessages(int cms);

  void Login(const buzz::XmppClientSettings & xcs);
  void Disconnect();
  void UpperReady();

  void ShareFiles( FileShareClient * pFileShareClient, const LibJingleEx::PTJid  ptJid,  
                   const LibJingleEx::FileShareManifest * pManifest, const char * szLocalDir );

  void AcceptFiles( void * hSession, const char * szSavePath = 0 );

  void DeclineFiles( void * hSession );

  void CancelFiles( void * pFileShareClient );

  void GetVCard( const LibJingleEx::TJid * ptJid );

  void SendStanza( const buzz::XmlElement * stanza );

  void SendRaw( const char * raw );

  // 不接受房间邀请时，销毁内存(不在主线程销毁)
  void DeleteRoom( PTRoom pRoom );

  // 获取讨论组的成员(非在线)
  void GetChatRoomMembers( PTRoom pRoom );

  void SendStanza( const TIqId *  ptIqId, const buzz::XmlElement * stanza );

  void Ping();

  void SetTrueSocialName( const char * szJid, const char * szTureSocialName );

  void SendRoomPicture( PTRoom hRoom, const char * szPicFilePathName, unsigned long dwPicSize );

  // added by shijie
  void * m_hApp;
  // end of adding

  LibJingleEx::ELibjingleStateSubCode  GetErrcode(); 

private:
  XmppPump* pump_;

  void OnStateChange(buzz::XmppEngine::State state, int nError, int nSubCode);

public:
  void OnStatusUpdate(const buzz::Status& status);

private:
  void OnMessage(talk_base::Message* pmsg);

  LibJingleEx::ELibjingleStateSubCode  m_eSubCode;
};

#endif // _XMPPTHREAD_H_
