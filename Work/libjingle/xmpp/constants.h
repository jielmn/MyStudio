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

#ifndef _CRICKET_XMPP_XMPPLIB_BUZZ_CONSTANTS_H_
#define _CRICKET_XMPP_XMPPLIB_BUZZ_CONSTANTS_H_

#include <string>
#include "talk/xmllite/qname.h"
#include "talk/xmpp/jid.h"


#define NS_CLIENT Constants::ns_client()
#define NS_SERVER Constants::ns_server()
#define NS_STREAM Constants::ns_stream()
#define NS_XSTREAM Constants::ns_xstream()
#define NS_TLS Constants::ns_tls()
#define NS_SASL Constants::ns_sasl()
#define NS_BIND Constants::ns_bind()
#define NS_DIALBACK Constants::ns_dialback()
#define NS_SESSION Constants::ns_session()
#define NS_STANZA Constants::ns_stanza()
#define NS_PRIVACY Constants::ns_privacy()
#define NS_ROSTER Constants::ns_roster()
#define NS_VCARD Constants::ns_vcard()
#define NS_AVATAR_HASH Constants::ns_avatar_hash()
#define NS_VCARD_UPDATE Constants::ns_vcard_update()
#define STR_CLIENT Constants::str_client()
#define STR_SERVER Constants::str_server()
#define STR_STREAM Constants::str_stream()


namespace buzz {

extern const Jid JID_EMPTY;

class Constants {
 public:
  static const std::string & ns_client();
  static const std::string & ns_server();
  static const std::string & ns_stream();
  static const std::string & ns_xstream();
  static const std::string & ns_tls();
  static const std::string & ns_sasl();
  static const std::string & ns_bind();
  static const std::string & ns_dialback();
  static const std::string & ns_session();
  static const std::string & ns_stanza();
  static const std::string & ns_privacy();
  static const std::string & ns_roster();
  static const std::string & ns_vcard();
  static const std::string & ns_avatar_hash();
  static const std::string & ns_vcard_update();

  static const std::string & str_client();
  static const std::string & str_server();
  static const std::string & str_stream();
};

extern const std::string STR_GET;
extern const std::string STR_SET;
extern const std::string STR_RESULT;
extern const std::string STR_ERROR;


extern const std::string STR_FROM;
extern const std::string STR_TO;
extern const std::string STR_BOTH;
extern const std::string STR_REMOVE;

extern const std::string STR_MESSAGE;
extern const std::string STR_BODY;
extern const std::string STR_PRESENCE;
extern const std::string STR_STATUS;
extern const std::string STR_SHOW;
extern const std::string STR_PRIOIRTY;
extern const std::string STR_IQ;

extern const std::string STR_TYPE;
extern const std::string STR_NAME;
extern const std::string STR_ID;
extern const std::string STR_JID;
extern const std::string STR_SUBSCRIPTION;
extern const std::string STR_ASK;
extern const std::string STR_X;
extern const std::string STR_GOOGLE_COM;
extern const std::string STR_GMAIL_COM;
extern const std::string STR_GOOGLEMAIL_COM;
extern const std::string STR_DEFAULT_DOMAIN;
extern const std::string STR_TALK_GOOGLE_COM;
extern const std::string STR_TALKX_L_GOOGLE_COM;

#ifdef FEATURE_ENABLE_VOICEMAIL
extern const std::string STR_VOICEMAIL;
extern const std::string STR_OUTGOINGVOICEMAIL;
#endif

extern const std::string STR_UNAVAILABLE;

extern const QName QN_STREAM_STREAM;
extern const QName QN_STREAM_FEATURES;
extern const QName QN_STREAM_ERROR;

extern const QName QN_XSTREAM_BAD_FORMAT;
extern const QName QN_XSTREAM_BAD_NAMESPACE_PREFIX;
extern const QName QN_XSTREAM_CONFLICT;
extern const QName QN_XSTREAM_CONNECTION_TIMEOUT;
extern const QName QN_XSTREAM_HOST_GONE;
extern const QName QN_XSTREAM_HOST_UNKNOWN;
extern const QName QN_XSTREAM_IMPROPER_ADDRESSIING;
extern const QName QN_XSTREAM_INTERNAL_SERVER_ERROR;
extern const QName QN_XSTREAM_INVALID_FROM;
extern const QName QN_XSTREAM_INVALID_ID;
extern const QName QN_XSTREAM_INVALID_NAMESPACE;
extern const QName QN_XSTREAM_INVALID_XML;
extern const QName QN_XSTREAM_NOT_AUTHORIZED;
extern const QName QN_XSTREAM_POLICY_VIOLATION;
extern const QName QN_XSTREAM_REMOTE_CONNECTION_FAILED;
extern const QName QN_XSTREAM_RESOURCE_CONSTRAINT;
extern const QName QN_XSTREAM_RESTRICTED_XML;
extern const QName QN_XSTREAM_SEE_OTHER_HOST;
extern const QName QN_XSTREAM_SYSTEM_SHUTDOWN;
extern const QName QN_XSTREAM_UNDEFINED_CONDITION;
extern const QName QN_XSTREAM_UNSUPPORTED_ENCODING;
extern const QName QN_XSTREAM_UNSUPPORTED_STANZA_TYPE;
extern const QName QN_XSTREAM_UNSUPPORTED_VERSION;
extern const QName QN_XSTREAM_XML_NOT_WELL_FORMED;
extern const QName QN_XSTREAM_TEXT;

extern const QName QN_TLS_STARTTLS;
extern const QName QN_TLS_REQUIRED;
extern const QName QN_TLS_PROCEED;
extern const QName QN_TLS_FAILURE;

extern const std::string  NS_COMPRESSION;
extern const QName QN_COMPRESSION;

extern const std::string  NS_P_COMPRESSION;
extern const QName QN_START_COMPRESS;
extern const QName QN_COMPRESSION_METHOD;
extern const QName QN_COMPRESSED;


extern const QName QN_SASL_MECHANISMS;
extern const QName QN_SASL_MECHANISM;
extern const QName QN_SASL_AUTH;
extern const QName QN_SASL_CHALLENGE;
extern const QName QN_SASL_RESPONSE;
extern const QName QN_SASL_ABORT;
extern const QName QN_SASL_SUCCESS;
extern const QName QN_SASL_FAILURE;
extern const QName QN_SASL_ABORTED;
extern const QName QN_SASL_INCORRECT_ENCODING;
extern const QName QN_SASL_INVALID_AUTHZID;
extern const QName QN_SASL_INVALID_MECHANISM;
extern const QName QN_SASL_MECHANISM_TOO_WEAK;
extern const QName QN_SASL_NOT_AUTHORIZED;
extern const QName QN_SASL_TEMPORARY_AUTH_FAILURE;

extern const std::string NS_GOOGLE_AUTH;
extern const QName QN_MISSING_USERNAME;

extern const QName QN_DIALBACK_RESULT;
extern const QName QN_DIALBACK_VERIFY;

extern const QName QN_STANZA_BAD_REQUEST;
extern const QName QN_STANZA_CONFLICT;
extern const QName QN_STANZA_FEATURE_NOT_IMPLEMENTED;
extern const QName QN_STANZA_FORBIDDEN;
extern const QName QN_STANZA_GONE;
extern const QName QN_STANZA_INTERNAL_SERVER_ERROR;
extern const QName QN_STANZA_ITEM_NOT_FOUND;
extern const QName QN_STANZA_JID_MALFORMED;
extern const QName QN_STANZA_NOT_ACCEPTABLE;
extern const QName QN_STANZA_NOT_ALLOWED;
extern const QName QN_STANZA_PAYMENT_REQUIRED;
extern const QName QN_STANZA_RECIPIENT_UNAVAILABLE;
extern const QName QN_STANZA_REDIRECT;
extern const QName QN_STANZA_REGISTRATION_REQUIRED;
extern const QName QN_STANZA_REMOTE_SERVER_NOT_FOUND;
extern const QName QN_STANZA_REMOTE_SERVER_TIMEOUT;
extern const QName QN_STANZA_RESOURCE_CONSTRAINT;
extern const QName QN_STANZA_SERVICE_UNAVAILABLE;
extern const QName QN_STANZA_SUBSCRIPTION_REQUIRED;
extern const QName QN_STANZA_UNDEFINED_CONDITION;
extern const QName QN_STANZA_UNEXPECTED_REQUEST;
extern const QName QN_STANZA_TEXT;

extern const QName QN_BIND_BIND;
extern const QName QN_BIND_RESOURCE;
extern const QName QN_BIND_JID;

extern const QName QN_MESSAGE;
extern const QName QN_BODY;
extern const QName QN_SUBJECT;
extern const QName QN_THREAD;
extern const QName QN_PRESENCE;
extern const QName QN_SHOW;
extern const QName QN_STATUS;
extern const QName QN_LANG;
extern const QName QN_PRIORITY;
extern const QName QN_IQ;
extern const QName QN_ERROR;

extern const QName QN_SERVER_MESSAGE;
extern const QName QN_SERVER_BODY;
extern const QName QN_SERVER_SUBJECT;
extern const QName QN_SERVER_THREAD;
extern const QName QN_SERVER_PRESENCE;
extern const QName QN_SERVER_SHOW;
extern const QName QN_SERVER_STATUS;
extern const QName QN_SERVER_LANG;
extern const QName QN_SERVER_PRIORITY;
extern const QName QN_SERVER_IQ;
extern const QName QN_SERVER_ERROR;

extern const QName QN_SESSION_SESSION;

extern const QName QN_PRIVACY_QUERY;
extern const QName QN_PRIVACY_ACTIVE;
extern const QName QN_PRIVACY_DEFAULT;
extern const QName QN_PRIVACY_LIST;
extern const QName QN_PRIVACY_ITEM;
extern const QName QN_PRIVACY_IQ;
extern const QName QN_PRIVACY_MESSAGE;
extern const QName QN_PRIVACY_PRESENCE_IN;
extern const QName QN_PRIVACY_PRESENCE_OUT;

extern const QName QN_ROSTER_QUERY;
extern const QName QN_ROSTER_ITEM;
extern const QName QN_ROSTER_GROUP;

extern const QName QN_VCARD;
extern const QName QN_VCARD_FN;
extern const QName QN_VCARD_PHOTO;
extern const QName QN_VCARD_PHOTO_TYPE;
extern const QName QN_VCARD_PHOTO_BINVAL;
extern const QName QN_VCARD_AVATAR_HASH;
extern const QName QN_VCARD_AVATAR_HASH_MODIFIED;
extern const QName QN_VCARD_CATEGORIES;
extern const QName QN_VCARD_CATEGORIES_KEYWORD;


extern const QName QN_XML_LANG;

extern const QName QN_ENCODING;
extern const QName QN_VERSION;
extern const QName QN_TO;
extern const QName QN_FROM;
extern const QName QN_TYPE;
extern const QName QN_ID;
extern const QName QN_CODE;
extern const QName QN_NAME;
extern const QName QN_VALUE;
extern const QName QN_ACTION;
extern const QName QN_ORDER;
extern const QName QN_MECHANISM;
extern const QName QN_ASK;
extern const QName QN_JID;
extern const QName QN_SUBSCRIPTION;
extern const QName QN_TITLE1;
extern const QName QN_TITLE2;
extern const QName QN_LABEL;

extern const QName QN_XMLNS_CLIENT;
extern const QName QN_XMLNS_SERVER;
extern const QName QN_XMLNS_STREAM;

// Presence
extern const std::string STR_SHOW_AWAY;
extern const std::string STR_SHOW_CHAT;
extern const std::string STR_SHOW_DND;
extern const std::string STR_SHOW_XA;
extern const std::string STR_SHOW_OFFLINE;

// Subscription
extern const std::string STR_SUBSCRIBE;
extern const std::string STR_SUBSCRIBED;
extern const std::string STR_UNSUBSCRIBE;
extern const std::string STR_UNSUBSCRIBED;


// JEP 0030
extern const QName QN_NODE;
extern const QName QN_CATEGORY;
extern const QName QN_VAR;
extern const std::string NS_DISCO_INFO;
extern const std::string NS_DISCO_ITEMS;

extern const QName QN_DISCO_INFO_QUERY;
extern const QName QN_DISCO_IDENTITY;
extern const QName QN_DISCO_FEATURE;

extern const QName QN_DISCO_ITEMS_QUERY;
extern const QName QN_DISCO_ITEM;


// JEP 0115
extern const std::string NS_CAPS;
extern const QName QN_CAPS_C;
extern const QName QN_VER;
extern const QName QN_EXT;


// Avatar - JEP 0153
extern const std::string kNSVCard;
extern const QName kQnVCardX;
extern const QName kQnVCardPhoto;

// JEP 0172 User Nickname
extern const std::string kNSNickname;
extern const QName kQnNickname;


// JEP 0085 chat state
extern const std::string NS_CHATSTATE;
extern const QName QN_CS_ACTIVE;
extern const QName QN_CS_COMPOSING;
extern const QName QN_CS_PAUSED;
extern const QName QN_CS_INACTIVE;
extern const QName QN_CS_GONE;

// JEP 0091 Delayed Delivery
extern const std::string kNSDelay;
extern const QName kQnDelayX;
extern const QName kQnStamp;

// Google time stamping (higher resolution)
extern const std::string kNSTimestamp;
extern const QName kQnTime;
extern const QName kQnMilliseconds;


extern const std::string NS_JINGLE_INFO;
extern const QName QN_JINGLE_INFO_QUERY;
extern const QName QN_JINGLE_INFO_STUN;
extern const QName QN_JINGLE_INFO_RELAY;
extern const QName QN_JINGLE_INFO_SERVER;
extern const QName QN_JINGLE_INFO_TOKEN;
extern const QName QN_JINGLE_INFO_HOST;
extern const QName QN_JINGLE_INFO_TCP;
extern const QName QN_JINGLE_INFO_UDP;
extern const QName QN_JINGLE_INFO_TCPSSL;



// JEP 0045
extern const std::string NS_MUC;
extern const QName QN_MUC_X;
extern const QName QN_MUC_ITEM;
extern const QName QN_MUC_AFFILIATION;
extern const QName QN_MUC_ROLE;
extern const std::string STR_AFFILIATION_NONE;
extern const std::string STR_ROLE_PARTICIPANT;
extern const std::string NS_MUC_USER;
extern const QName QN_MUC_USER_CONTINUE;
extern const QName QN_MUC_USER_X;
extern const QName QN_MUC_USER_ITEM;
extern const QName QN_MUC_USER_STATUS;
extern const QName QN_MUC_USER_CODE;
extern const QName QN_MUC_USER_AFFILIATION;
extern const std::string NS_MUC_UNIQUE;
extern const QName QN_MUC_UNIQUE;
extern const std::string NS_MUC_OWNER;
extern const QName QN_MUC_QUERY_OWNER;
extern const QName QN_HISTORY;
extern const QName QN_MAX_STANZAS;
extern const QName QN_SINCE;

extern const QName QN_REASON;

extern const QName QN_MUC_ADMIN_QUERY;

extern const QName QN_MUC_SUBJECT;



// file share
extern const std::string NS_KEDACOM;
extern const QName QN_FILE_SHARE;
extern const QName QN_FILE_SHARE_SUBTYPE;
extern const QName QN_FILE_SHARE_SESSION_ID;
extern const QName QN_FILE_SHARE_NAME;
extern const QName QN_FILE_SHARE_MODE;
extern const QName QN_FILE_SHARE_ERROR;

// private
extern const std::string NS_IQ_PRIVATE;
extern const QName QN_PRIVATE_QUERY;

extern const std::string NS_KEDACOM_DMS_GROUP;
extern const std::string NS_KEDACOM_DMS_MEMBER;
extern const QName QN_DMS_GROUP;
extern const QName QN_DMS_MEMBER;
extern const QName QN_DMS_ITEM;


extern const std::string NS_CUSTOM_CONFIG;
extern const QName       QN_CUSTOM_CONFIG;


extern const QName QN_DMS_ID;
extern const QName QN_DMS_PARENT_ID;
extern const QName QN_DMS_NAME;
extern const QName QN_DMS_JID;
extern const QName QN_DMS_TYPE;
extern const QName QN_DMS_RESERVERD1;
extern const QName QN_DMS_RESERVERD2;
extern const QName QN_DMS_RESERVERD3;
extern const QName QN_DMS_RESERVERD4;

extern const std::string NS_KEDACOM_ROOM;
extern const QName QN_PERSISTENT_ROOM;
extern const QName QN_ITEM;
extern const QName QN_ROOM_ID;
extern const QName QN_ROOM_MUC_SERVICE;
extern const QName QN_FIRST;

extern const std::string NS_KEDACOM_ROOM_HISTORY;
extern const QName QN_CHATROOM_HISTORY;
extern const QName QN_CHATROOM_SERVICE;

extern const QName QN_GROUPCHAT;

// Regiser
extern const std::string NS_REGISTER;
extern const QName QN_REGISTER_QUERY;
extern const QName QN_USER_NAME;
extern const QName QN_PASSWORD;

extern const QName QN_OPERATION;

// PUBSUB
extern const std::string  NS_PUBSUB;
extern const std::string  NS_PUBSUB_ADMIN;
extern const std::string  NS_XDATA;
extern const QName QN_PUBSUB;
extern const QName QN_PUBSUB_ADMIN;
extern const QName QN_CREATE;
extern const QName QN_DELETE;
extern const QName QN_SUBSCRIBE;
extern const QName QN_CONFIGURE;
extern const QName QN_X;
extern const QName QN_FIELD;

extern const QName QN_DATA;



// keda WEIBO
extern const QName QN_ADDRESSES;


// delay message
extern const QName QN_DELAY;
extern const QName QN_DELAY_X;



extern const QName QN_XBODY_BASE64;
extern const QName QN_SYS_NOTIFY;
extern const std::string  NS_XBODY;
extern const QName QN_XBODY;

extern const std::string  NS_CUSTOM_BODY;
extern const QName QN_COSTOM_BODY;
extern const QName QN_COSTOM_TYPE;
extern const QName QN_COSTOM_BUF;

extern const std::string  NS_CHARSTATES;
extern const QName QN_CHAR_ACTIVE;
extern const QName QN_CHAR_COMPOSING;
extern const QName QN_CHAR_PAUSED;

extern const QName QN_RECEIPT_REQUEST;
extern const QName QN_RECEIPT_RECEIVED;



extern const QName QN_DMS_FONT;
extern const QName QN_DMS_COLOR;
extern const QName QN_DMS_TIME;
extern const QName QN_DMS_HEADER;
extern const QName QN_DMS_CONTENT;
extern const QName QN_DMS_CHAT_TYPE;
extern const QName QN_DMS_PIC_COUNT;
extern const QName QN_DMS_PIC;
extern const QName QN_DMS_PIC_INDEX;
extern const QName QN_DMS_PIC_PATH;

extern const QName QN_DMS_FONT_1;
extern const QName QN_DMS_FONT_HEIGHT;
extern const QName QN_DMS_FONT_WIDTH;
extern const QName QN_DMS_FONT_ESCAPE;
extern const QName QN_DMS_FONT_ORIENTATION;
extern const QName QN_DMS_FONT_WEIGHT;
extern const QName QN_DMS_FONT_ITALIC;
extern const QName QN_DMS_FONT_UNDERLINE;
extern const QName QN_DMS_FONT_STRIKE_OUT;
extern const QName QN_DMS_FONT_CHARSET;
extern const QName QN_DMS_FONT_OUT_PRECISION;
extern const QName QN_DMS_FONT_CLIP_PRECISION;
extern const QName QN_DMS_FONT_QUALITY;
extern const QName QN_DMS_FONT_PITCH_AND_FAMILY;
extern const QName QN_DMS_FONT_FACE_NAME;

extern const QName QN_DMS_TIME_1;

extern const std::string  NS_IBB;
extern const QName QN_IBB_OPEN;
extern const QName QN_IBB_CLOSE;
extern const QName QN_IBB_DATA;
extern const QName QN_BLOCK_SIZE;
extern const QName QN_SID;
extern const QName QN_SEQ;


extern const std::string  NS_TIME;
extern const QName QN_TIME_QUERY;
extern const QName QN_TIME;


extern const std::string  NS_KEDA_ROSTER;
extern const QName QN_KEDA_ROSTER_BITRATE;
extern const QName QN_KEDA_ROSTER_TYPE;
}

#endif // _CRICKET_XMPP_XMPPLIB_BUZZ_CONSTANTS_H_
