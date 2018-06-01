/*
 * Jingle call example
 * Copyright 2004--2005, Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "talk/base/stringencode.h"
#include "talk/examples/login/presencepushtask.h"
#include "talk/xmpp/constants.h"
#include <sstream>

#include "assert.h"
#include "talk/examples/login/xmppthread.h"
#include "libjingle/libjingle_inner.h"


namespace buzz {

// string helper functions -----------------------------------------------------

bool
PresencePushTask::HandleStanza(const XmlElement * stanza) {
  if (stanza->Name() != QN_PRESENCE)
    return false;
  //if (stanza->HasAttr(QN_TYPE) && stanza->Attr(QN_TYPE) != STR_UNAVAILABLE)
  //  return false;
  //QueueStanza(stanza);

  // 如果是MUC，交给其他task处理
  const XmlElement * child = stanza->FirstElement();
  while( child )
  {
      if ( child->Name() == QN_MUC_USER_X || child->Name() == QN_MUC_X )
      {
          return false;
      }
      child = child->NextElement();
  }

  // 不是MUC
  XmppClient * c = GetClient();
  CLibJinglexApp * app = (CLibJinglexApp *)c->m_hApp;
  cFuncLock cLock( &app->m_lock );

  if ( stanza->HasAttr(QN_TYPE) )
  {
      if ( stanza->Attr(QN_TYPE) == STR_UNAVAILABLE )
      {
          QueueStanza(stanza);
      }
      else if ( stanza->Attr(QN_TYPE) == "subscribe" )
      {
          LibJingleEx::TJid tJid;
          memset( &tJid, 0, sizeof(tJid) );
          LibJingleEx::StrJid2TJid( stanza->Attr(QN_FROM).c_str(), tJid  );

          if ( g_tEvents.pOnSubscribe )
          {
              g_tEvents.pOnSubscribe( app, &tJid );
          }
          else if ( g_pfnOnMessage )
          {
              g_pfnOnMessage( LibJingleEx::ON_QUERY_SUBSCRIBED, app, app->m_pUserData, 
                             (unsigned char *)&tJid, sizeof(tJid) );
          }
      }
  }
  else
  {
      QueueStanza(stanza);
  }

  return true;
}

static bool IsUtf8FirstByte(int c) {
  return (((c)&0x80)==0) || // is single byte
    ((unsigned char)((c)-0xc0)<0x3e); // or is lead byte
}

int
PresencePushTask::ProcessStart() {
  const XmlElement * stanza = NextStanza();
  if (stanza == NULL)
    return STATE_BLOCKED;
  Status s;
  char buf[MAX_BUFF_SIZE];

  s.set_jid(Jid(stanza->Attr(QN_FROM)));

  if (stanza->Attr(QN_TYPE) == STR_UNAVAILABLE) {
    s.set_available(false);
    SignalStatusUpdate(s);
  }
  else {
    s.set_available(true);
    const XmlElement * status = stanza->FirstNamed(QN_STATUS);
    if (status != NULL) {
      s.set_status( Utf8ToAnsi(status->BodyText().c_str(), buf, MAX_BUFF_SIZE) );

      // Truncate status messages longer than 300 bytes
      if (s.status().length() > 300) {
        size_t len = 300;

        // Be careful not to split legal utf-8 chars in half
        while (!IsUtf8FirstByte(s.status()[len]) && len > 0) {
          len -= 1;
        }
        std::string truncated(s.status(), 0, len);
        s.set_status(truncated);
      }
    }

    const XmlElement * priority = stanza->FirstNamed(QN_PRIORITY);
    if (priority != NULL) {
      int pri;
      if (talk_base::FromString(priority->BodyText(), &pri)) {
        s.set_priority(pri);
      }
    }

    const XmlElement * show = stanza->FirstNamed(QN_SHOW);
    if (show == NULL || show->FirstChild() == NULL) {
      s.set_show(Status::SHOW_ONLINE);
    }
    else {
      if (show->BodyText() == "away") {
        s.set_show(Status::SHOW_AWAY);
      }
      else if (show->BodyText() == "xa") {
        s.set_show(Status::SHOW_XA);
      }
      else if (show->BodyText() == "dnd") {
        s.set_show(Status::SHOW_DND);
      }
      else if (show->BodyText() == "chat") {
        s.set_show(Status::SHOW_CHAT);
      }
      else {
        s.set_show(Status::SHOW_ONLINE);
      }
    }

    const XmlElement * caps = stanza->FirstNamed(QN_CAPS_C);
    if (caps != NULL) {
      std::string node = caps->Attr(QN_NODE);
      std::string ver = caps->Attr(QN_VER);
      std::string exts = caps->Attr(QN_EXT);

      s.set_know_capabilities(true);

      if (node == GOOGLE_CLIENT_NODE) {
        s.set_is_google_client(true);
        s.set_version(ver);
        if (ListContainsToken(exts, "voice-v1")) {
          s.set_phone_capability(true);
        }
	    if (ListContainsToken(exts, "share-v1")) {
	      s.set_fileshare_capability(true);
	    }
        if (ListContainsToken(exts, "audio")) {
            s.set_audio_capability(true);
        }
        if (ListContainsToken(exts, "video")) {
            s.set_video_capability(true);
        }
        if (ListContainsToken(exts, "call")) {
            s.set_call_capability(true);
        }
      }
    }

    const XmlElement* delay = stanza->FirstNamed(kQnDelayX);
    if (delay != NULL) {
      // Ideally we would parse this according to the Psuedo ISO-8601 rules
      // that are laid out in JEP-0082:
      // http://www.jabber.org/jeps/jep-0082.html
      const std::string & stamp = delay->Attr(kQnStamp);
      s.set_sent_time(stamp);
      s.set_delay(true);
    }

    SignalStatusUpdate(s);
  }

  return STATE_START;
}


}


