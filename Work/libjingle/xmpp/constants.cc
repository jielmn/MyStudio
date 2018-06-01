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

#include <string>
#include "talk/base/basicdefs.h"
#include "talk/xmllite/xmlconstants.h"
#include "talk/xmllite/xmlelement.h"
#include "talk/xmllite/qname.h"
#include "talk/xmpp/jid.h"
#include "talk/xmpp/constants.h"
namespace buzz {

const Jid JID_EMPTY(STR_EMPTY);

const std::string & Constants::ns_client() {
  static const std::string ns_client_("jabber:client");
  return ns_client_;
}

const std::string & Constants::ns_server() {
  static const std::string ns_server_("jabber:server");
  return ns_server_;
}

const std::string & Constants::ns_stream() {
  static const std::string ns_stream_("http://etherx.jabber.org/streams");
  return ns_stream_;
}

const std::string & Constants::ns_xstream() {
  static const std::string ns_xstream_("urn:ietf:params:xml:ns:xmpp-streams");
  return ns_xstream_;
}

const std::string & Constants::ns_tls() {
  static const std::string ns_tls_("urn:ietf:params:xml:ns:xmpp-tls");
  return ns_tls_;
}

const std::string & Constants::ns_sasl() {
  static const std::string ns_sasl_("urn:ietf:params:xml:ns:xmpp-sasl");
  return ns_sasl_;
}

const std::string & Constants::ns_bind() {
  static const std::string ns_bind_("urn:ietf:params:xml:ns:xmpp-bind");
  return ns_bind_;
}

const std::string & Constants::ns_dialback() {
  static const std::string ns_dialback_("jabber:server:dialback");
  return ns_dialback_;
}

const std::string & Constants::ns_session() {
  static const std::string ns_session_("urn:ietf:params:xml:ns:xmpp-session");
  return ns_session_;
}

const std::string & Constants::ns_stanza() {
  static const std::string ns_stanza_("urn:ietf:params:xml:ns:xmpp-stanzas");
  return ns_stanza_;
}

const std::string & Constants::ns_privacy() {
  static const std::string ns_privacy_("jabber:iq:privacy");
  return ns_privacy_;
}

const std::string & Constants::ns_roster() {
  static const std::string ns_roster_("jabber:iq:roster");
  return ns_roster_;
}

const std::string & Constants::ns_vcard() {
  static const std::string ns_vcard_("vcard-temp");
  return ns_vcard_;
}

const std::string & Constants::ns_avatar_hash() {
  static const std::string ns_avatar_hash_("google:avatar");
  return ns_avatar_hash_;
}

const std::string & Constants::ns_vcard_update() {
  static const std::string ns_vcard_update_("vcard-temp:x:update");
  return ns_vcard_update_;
}

const std::string & Constants::str_client() {
  static const std::string str_client_("client");
  return str_client_;
}

const std::string & Constants::str_server() {
  static const std::string str_server_("server");
  return str_server_;
}

const std::string & Constants::str_stream() {
  static const std::string str_stream_("stream");
  return str_stream_;
}

const std::string STR_GET("get");
const std::string STR_SET("set");
const std::string STR_RESULT("result");
const std::string STR_ERROR("error");


const std::string STR_FROM("from");
const std::string STR_TO("to");
const std::string STR_BOTH("both");
const std::string STR_REMOVE("remove");

const std::string STR_UNAVAILABLE("unavailable");

const std::string STR_GOOGLE_COM("google.com");
const std::string STR_GMAIL_COM("gmail.com");
const std::string STR_GOOGLEMAIL_COM("googlemail.com");
const std::string STR_DEFAULT_DOMAIN("default.talk.google.com");
const std::string STR_TALK_GOOGLE_COM("talk.google.com");
const std::string STR_TALKX_L_GOOGLE_COM("talkx.l.google.com");

const std::string STR_X("x");

#ifdef FEATURE_ENABLE_VOICEMAIL
const std::string STR_VOICEMAIL("voicemail");
const std::string STR_OUTGOINGVOICEMAIL("outgoingvoicemail");
#endif

const QName QN_STREAM_STREAM(true, NS_STREAM, STR_STREAM);
const QName QN_STREAM_FEATURES(true, NS_STREAM, "features");
const QName QN_STREAM_ERROR(true, NS_STREAM, "error");

const QName QN_XSTREAM_BAD_FORMAT(true, NS_XSTREAM, "bad-format");
const QName QN_XSTREAM_BAD_NAMESPACE_PREFIX(true, NS_XSTREAM, "bad-namespace-prefix");
const QName QN_XSTREAM_CONFLICT(true, NS_XSTREAM, "conflict");
const QName QN_XSTREAM_CONNECTION_TIMEOUT(true, NS_XSTREAM, "connection-timeout");
const QName QN_XSTREAM_HOST_GONE(true, NS_XSTREAM, "host-gone");
const QName QN_XSTREAM_HOST_UNKNOWN(true, NS_XSTREAM, "host-unknown");
const QName QN_XSTREAM_IMPROPER_ADDRESSIING(true, NS_XSTREAM, "improper-addressing");
const QName QN_XSTREAM_INTERNAL_SERVER_ERROR(true, NS_XSTREAM, "internal-server-error");
const QName QN_XSTREAM_INVALID_FROM(true, NS_XSTREAM, "invalid-from");
const QName QN_XSTREAM_INVALID_ID(true, NS_XSTREAM, "invalid-id");
const QName QN_XSTREAM_INVALID_NAMESPACE(true, NS_XSTREAM, "invalid-namespace");
const QName QN_XSTREAM_INVALID_XML(true, NS_XSTREAM, "invalid-xml");
const QName QN_XSTREAM_NOT_AUTHORIZED(true, NS_XSTREAM, "not-authorized");
const QName QN_XSTREAM_POLICY_VIOLATION(true, NS_XSTREAM, "policy-violation");
const QName QN_XSTREAM_REMOTE_CONNECTION_FAILED(true, NS_XSTREAM, "remote-connection-failed");
const QName QN_XSTREAM_RESOURCE_CONSTRAINT(true, NS_XSTREAM, "resource-constraint");
const QName QN_XSTREAM_RESTRICTED_XML(true, NS_XSTREAM, "restricted-xml");
const QName QN_XSTREAM_SEE_OTHER_HOST(true, NS_XSTREAM, "see-other-host");
const QName QN_XSTREAM_SYSTEM_SHUTDOWN(true, NS_XSTREAM, "system-shutdown");
const QName QN_XSTREAM_UNDEFINED_CONDITION(true, NS_XSTREAM, "undefined-condition");
const QName QN_XSTREAM_UNSUPPORTED_ENCODING(true, NS_XSTREAM, "unsupported-encoding");
const QName QN_XSTREAM_UNSUPPORTED_STANZA_TYPE(true, NS_XSTREAM, "unsupported-stanza-type");
const QName QN_XSTREAM_UNSUPPORTED_VERSION(true, NS_XSTREAM, "unsupported-version");
const QName QN_XSTREAM_XML_NOT_WELL_FORMED(true, NS_XSTREAM, "xml-not-well-formed");
const QName QN_XSTREAM_TEXT(true, NS_XSTREAM, "text");

const QName QN_TLS_STARTTLS(true, NS_TLS, "starttls");
const QName QN_TLS_REQUIRED(true, NS_TLS, "required");
const QName QN_TLS_PROCEED(true, NS_TLS, "proceed");
const QName QN_TLS_FAILURE(true, NS_TLS, "failure");

const std::string  NS_COMPRESSION("http://jabber.org/features/compress");
const QName QN_COMPRESSION(true, NS_COMPRESSION, "compression");

const std::string  NS_P_COMPRESSION("http://jabber.org/protocol/compress");
const QName QN_START_COMPRESS(true, NS_P_COMPRESSION, "compress");
const QName QN_COMPRESSION_METHOD(true, NS_P_COMPRESSION, "method");
const QName QN_COMPRESSED(true, NS_P_COMPRESSION, "compressed");

const QName QN_SASL_MECHANISMS(true, NS_SASL, "mechanisms");
const QName QN_SASL_MECHANISM(true, NS_SASL, "mechanism");
const QName QN_SASL_AUTH(true, NS_SASL, "auth");
const QName QN_SASL_CHALLENGE(true, NS_SASL, "challenge");
const QName QN_SASL_RESPONSE(true, NS_SASL, "response");
const QName QN_SASL_ABORT(true, NS_SASL, "abort");
const QName QN_SASL_SUCCESS(true, NS_SASL, "success");
const QName QN_SASL_FAILURE(true, NS_SASL, "failure");
const QName QN_SASL_ABORTED(true, NS_SASL, "aborted");
const QName QN_SASL_INCORRECT_ENCODING(true, NS_SASL, "incorrect-encoding");
const QName QN_SASL_INVALID_AUTHZID(true, NS_SASL, "invalid-authzid");
const QName QN_SASL_INVALID_MECHANISM(true, NS_SASL, "invalid-mechanism");
const QName QN_SASL_MECHANISM_TOO_WEAK(true, NS_SASL, "mechanism-too-weak");
const QName QN_SASL_NOT_AUTHORIZED(true, NS_SASL, "not-authorized");
const QName QN_SASL_TEMPORARY_AUTH_FAILURE(true, NS_SASL, "temporary-auth-failure");

const std::string NS_GOOGLE_AUTH("google:auth");
const QName QN_MISSING_USERNAME(true, NS_GOOGLE_AUTH, "missing-username");

const QName QN_DIALBACK_RESULT(true, NS_DIALBACK, "result");
const QName QN_DIALBACK_VERIFY(true, NS_DIALBACK, "verify");

const QName QN_STANZA_BAD_REQUEST(true, NS_STANZA, "bad-request");
const QName QN_STANZA_CONFLICT(true, NS_STANZA, "conflict");
const QName QN_STANZA_FEATURE_NOT_IMPLEMENTED(true, NS_STANZA, "feature-not-implemented");
const QName QN_STANZA_FORBIDDEN(true, NS_STANZA, "forbidden");
const QName QN_STANZA_GONE(true, NS_STANZA, "gone");
const QName QN_STANZA_INTERNAL_SERVER_ERROR(true, NS_STANZA, "internal-server-error");
const QName QN_STANZA_ITEM_NOT_FOUND(true, NS_STANZA, "item-not-found");
const QName QN_STANZA_JID_MALFORMED(true, NS_STANZA, "jid-malformed");
const QName QN_STANZA_NOT_ACCEPTABLE(true, NS_STANZA, "not-acceptable");
const QName QN_STANZA_NOT_ALLOWED(true, NS_STANZA, "not-allowed");
const QName QN_STANZA_PAYMENT_REQUIRED(true, NS_STANZA, "payment-required");
const QName QN_STANZA_RECIPIENT_UNAVAILABLE(true, NS_STANZA, "recipient-unavailable");
const QName QN_STANZA_REDIRECT(true, NS_STANZA, "redirect");
const QName QN_STANZA_REGISTRATION_REQUIRED(true, NS_STANZA, "registration-required");
const QName QN_STANZA_REMOTE_SERVER_NOT_FOUND(true, NS_STANZA, "remote-server-not-found");
const QName QN_STANZA_REMOTE_SERVER_TIMEOUT(true, NS_STANZA, "remote-server-timeout");
const QName QN_STANZA_RESOURCE_CONSTRAINT(true, NS_STANZA, "resource-constraint");
const QName QN_STANZA_SERVICE_UNAVAILABLE(true, NS_STANZA, "service-unavailable");
const QName QN_STANZA_SUBSCRIPTION_REQUIRED(true, NS_STANZA, "subscription-required");
const QName QN_STANZA_UNDEFINED_CONDITION(true, NS_STANZA, "undefined-condition");
const QName QN_STANZA_UNEXPECTED_REQUEST(true, NS_STANZA, "unexpected-request");
const QName QN_STANZA_TEXT(true, NS_STANZA, "text");

const QName QN_BIND_BIND(true, NS_BIND, "bind");
const QName QN_BIND_RESOURCE(true, NS_BIND, "resource");
const QName QN_BIND_JID(true, NS_BIND, "jid");

const QName QN_MESSAGE(true, NS_CLIENT, "message");
const QName QN_BODY(true, NS_CLIENT, "body");
const QName QN_SUBJECT(true, NS_CLIENT, "subject");
const QName QN_THREAD(true, NS_CLIENT, "thread");
const QName QN_PRESENCE(true, NS_CLIENT, "presence");
const QName QN_SHOW(true, NS_CLIENT, "show");
const QName QN_STATUS(true, NS_CLIENT, "status");
const QName QN_LANG(true, NS_CLIENT, "lang");
const QName QN_PRIORITY(true, NS_CLIENT, "priority");
const QName QN_IQ(true, NS_CLIENT, "iq");
const QName QN_ERROR(true, NS_CLIENT, "error");

const QName QN_SERVER_MESSAGE(true, NS_SERVER, "message");
const QName QN_SERVER_BODY(true, NS_SERVER, "body");
const QName QN_SERVER_SUBJECT(true, NS_SERVER, "subject");
const QName QN_SERVER_THREAD(true, NS_SERVER, "thread");
const QName QN_SERVER_PRESENCE(true, NS_SERVER, "presence");
const QName QN_SERVER_SHOW(true, NS_SERVER, "show");
const QName QN_SERVER_STATUS(true, NS_SERVER, "status");
const QName QN_SERVER_LANG(true, NS_SERVER, "lang");
const QName QN_SERVER_PRIORITY(true, NS_SERVER, "priority");
const QName QN_SERVER_IQ(true, NS_SERVER, "iq");
const QName QN_SERVER_ERROR(true, NS_SERVER, "error");

const QName QN_SESSION_SESSION(true, NS_SESSION, "session");

const QName QN_PRIVACY_QUERY(true, NS_PRIVACY, "query");
const QName QN_PRIVACY_ACTIVE(true, NS_PRIVACY, "active");
const QName QN_PRIVACY_DEFAULT(true, NS_PRIVACY, "default");
const QName QN_PRIVACY_LIST(true, NS_PRIVACY, "list");
const QName QN_PRIVACY_ITEM(true, NS_PRIVACY, "item");
const QName QN_PRIVACY_IQ(true, NS_PRIVACY, "iq");
const QName QN_PRIVACY_MESSAGE(true, NS_PRIVACY, "message");
const QName QN_PRIVACY_PRESENCE_IN(true, NS_PRIVACY, "presence-in");
const QName QN_PRIVACY_PRESENCE_OUT(true, NS_PRIVACY, "presence-out");

const QName QN_ROSTER_QUERY(true, NS_ROSTER, "query");
const QName QN_ROSTER_ITEM(true, NS_ROSTER, "item");
const QName QN_ROSTER_GROUP(true, NS_ROSTER, "group");

const QName QN_VCARD(true, NS_VCARD, "vCard");
const QName QN_VCARD_FN(true, NS_VCARD, "FN");
const QName QN_VCARD_PHOTO(true, NS_VCARD, "PHOTO");
const QName QN_VCARD_PHOTO_TYPE(true, NS_VCARD, "TYPE");
const QName QN_VCARD_PHOTO_BINVAL(true, NS_VCARD, "BINVAL");
const QName QN_VCARD_AVATAR_HASH(true, NS_AVATAR_HASH, "hash");
const QName QN_VCARD_AVATAR_HASH_MODIFIED(true, NS_AVATAR_HASH, "modified");
const QName QN_VCARD_CATEGORIES(true, NS_VCARD, "CATEGORIES");
const QName QN_VCARD_CATEGORIES_KEYWORD(true, NS_VCARD, "KEYWORD");

const buzz::QName QN_NAME(true, STR_EMPTY, "name");
const QName QN_XML_LANG(true, NS_XML, "lang");

const std::string STR_TYPE("type");
const std::string STR_ID("id");
const std::string STR_NAME("name");
const std::string STR_JID("jid");
const std::string STR_SUBSCRIPTION("subscription");
const std::string STR_ASK("ask");

const QName QN_ENCODING(true, STR_EMPTY, STR_ENCODING);
const QName QN_VERSION(true, STR_EMPTY, STR_VERSION);
const QName QN_TO(true, STR_EMPTY, "to");
const QName QN_FROM(true, STR_EMPTY, "from");
const QName QN_TYPE(true, STR_EMPTY, "type");
const QName QN_ID(true, STR_EMPTY, "id");
const QName QN_CODE(true, STR_EMPTY, "code");

const QName QN_VALUE(true, STR_EMPTY, "value");
const QName QN_ACTION(true, STR_EMPTY, "action");
const QName QN_ORDER(true, STR_EMPTY, "order");
const QName QN_MECHANISM(true, STR_EMPTY, "mechanism");
const QName QN_ASK(true, STR_EMPTY, "ask");
const QName QN_JID(true, STR_EMPTY, "jid");
const QName QN_SUBSCRIPTION(true, STR_EMPTY, "subscription");
const QName QN_TITLE1(true, STR_EMPTY, "title1");
const QName QN_TITLE2(true, STR_EMPTY, "title2");
const QName QN_SOURCE(true, STR_EMPTY, "source");
const QName QN_LABEL(true, STR_EMPTY, "label");

const QName QN_XMLNS_CLIENT(true, NS_XMLNS, STR_CLIENT);
const QName QN_XMLNS_SERVER(true, NS_XMLNS, STR_SERVER);
const QName QN_XMLNS_STREAM(true, NS_XMLNS, STR_STREAM);



// Presence
const std::string STR_SHOW_AWAY("away");
const std::string STR_SHOW_CHAT("chat");
const std::string STR_SHOW_DND("dnd");
const std::string STR_SHOW_XA("xa");
const std::string STR_SHOW_OFFLINE("offline");

// Subscription
const std::string STR_SUBSCRIBE("subscribe");
const std::string STR_SUBSCRIBED("subscribed");
const std::string STR_UNSUBSCRIBE("unsubscribe");
const std::string STR_UNSUBSCRIBED("unsubscribed");


// JEP 0030
const QName QN_NODE(true, STR_EMPTY, "node");
const QName QN_CATEGORY(true, STR_EMPTY, "category");
const QName QN_VAR(true, STR_EMPTY, "var");
const std::string NS_DISCO_INFO("http://jabber.org/protocol/disco#info");
const std::string NS_DISCO_ITEMS("http://jabber.org/protocol/disco#items");
const QName QN_DISCO_INFO_QUERY(true, NS_DISCO_INFO, "query");
const QName QN_DISCO_IDENTITY(true, NS_DISCO_INFO, "identity");
const QName QN_DISCO_FEATURE(true, NS_DISCO_INFO, "feature");

const QName QN_DISCO_ITEMS_QUERY(true, NS_DISCO_ITEMS, "query");
const QName QN_DISCO_ITEM(true, NS_DISCO_ITEMS, "item");


// JEP 0115
const std::string NS_CAPS("http://jabber.org/protocol/caps");
const QName QN_CAPS_C(true, NS_CAPS, "c");
const QName QN_VER(true, STR_EMPTY, "ver");
const QName QN_EXT(true, STR_EMPTY, "ext");

// JEP 0153
const std::string kNSVCard("vcard-temp:x:update");
const QName kQnVCardX(true, kNSVCard, "x");
const QName kQnVCardPhoto(true, kNSVCard, "photo");

// JEP 0172 User Nickname
const std::string kNSNickname("http://jabber.org/protocol/nick");
const QName kQnNickname(true, kNSNickname, "nick");


// JEP 0085 chat state
const std::string NS_CHATSTATE("http://jabber.org/protocol/chatstates");
const QName QN_CS_ACTIVE(true, NS_CHATSTATE, "active");
const QName QN_CS_COMPOSING(true, NS_CHATSTATE, "composing");
const QName QN_CS_PAUSED(true, NS_CHATSTATE, "paused");
const QName QN_CS_INACTIVE(true, NS_CHATSTATE, "inactive");
const QName QN_CS_GONE(true, NS_CHATSTATE, "gone");

// JEP 0091 Delayed Delivery
const std::string kNSDelay("jabber:x:delay");
const QName kQnDelayX(true, kNSDelay, "x");
const QName kQnStamp(true, STR_EMPTY, "stamp");

// Google time stamping (higher resolution)
const std::string kNSTimestamp("google:timestamp");
const QName kQnTime(true, kNSTimestamp, "time");
const QName kQnMilliseconds(true, STR_EMPTY, "ms");



// Jingle Info
const std::string NS_JINGLE_INFO("google:jingleinfo");
const QName QN_JINGLE_INFO_QUERY(true, NS_JINGLE_INFO, "query");
const QName QN_JINGLE_INFO_STUN(true, NS_JINGLE_INFO, "stun");
const QName QN_JINGLE_INFO_RELAY(true, NS_JINGLE_INFO, "relay");
const QName QN_JINGLE_INFO_SERVER(true, NS_JINGLE_INFO, "server");
const QName QN_JINGLE_INFO_TOKEN(true, NS_JINGLE_INFO, "token");
const QName QN_JINGLE_INFO_HOST(true, STR_EMPTY, "host");
const QName QN_JINGLE_INFO_TCP(true, STR_EMPTY, "tcp");
const QName QN_JINGLE_INFO_UDP(true, STR_EMPTY, "udp");
const QName QN_JINGLE_INFO_TCPSSL(true, STR_EMPTY, "tcpssl");






// JEP 0045
const std::string NS_MUC("http://jabber.org/protocol/muc");
const QName QN_MUC_X(true, NS_MUC, "x" );

const QName QN_MUC_AFFILIATION( true, "", "affiliation" );

const std::string NS_MUC_USER("http://jabber.org/protocol/muc#user");
const QName QN_MUC_USER_CONTINUE(true, NS_MUC_USER, "continue");
const QName QN_MUC_USER_X(true, NS_MUC_USER, "x");
const QName QN_MUC_USER_ITEM(true, NS_MUC_USER, "item");
const QName QN_MUC_USER_STATUS(true, NS_MUC_USER, "status");
const QName QN_MUC_USER_CODE(true, NS_MUC_USER, "code");
const QName QN_MUC_USER_AFFILIATION(true, NS_MUC_USER, "affiliation");

const std::string NS_MUC_UNIQUE("http://jabber.org/protocol/muc#unique");
const QName QN_MUC_UNIQUE(true,NS_MUC_UNIQUE,"unique" );

const std::string NS_MUC_OWNER("http://jabber.org/protocol/muc#owner");
const QName QN_MUC_QUERY_OWNER( true, NS_MUC_OWNER, "query");

const QName QN_HISTORY( true, "", "history" );
const QName QN_MAX_STANZAS( true, "", "maxstanzas" );
const QName QN_SINCE( true, "", "since" );

const QName QN_REASON( true, "", "reason" );

const QName QN_MUC_ADMIN_QUERY( true, "http://jabber.org/protocol/muc#admin", "query" );
const QName QN_MUC_SUBJECT( true, "", "subject" );






const std::string NS_KEDACOM("http:://kedacom.com");
const QName QN_FILE_SHARE( true, NS_KEDACOM, "fileshare");
const QName QN_FILE_SHARE_SUBTYPE(true, "", "subtype");
const QName QN_FILE_SHARE_SESSION_ID(true, "", "session_id");
const QName QN_FILE_SHARE_NAME(true, "", "name");
const QName QN_FILE_SHARE_MODE(true, "", "mode");
const QName QN_FILE_SHARE_ERROR(true, "", "error");



const std::string NS_IQ_PRIVATE("jabber:iq:private");
const QName QN_PRIVATE_QUERY( true, NS_IQ_PRIVATE, "query" );

const std::string NS_KEDACOM_DMS_GROUP("http://kedacom.com/dms/group");
const std::string NS_KEDACOM_DMS_MEMBER("http://kedacom.com/dms/member");
const QName QN_DMS_GROUP( true,  NS_KEDACOM_DMS_GROUP, "group" );
const QName QN_DMS_MEMBER( true, NS_KEDACOM_DMS_MEMBER, "member" );
const QName QN_DMS_ITEM( true, "", "item");


const std::string NS_CUSTOM_CONFIG("kedacom:custom_config");
const QName QN_CUSTOM_CONFIG(true, NS_CUSTOM_CONFIG, "custom_config");

const QName QN_DMS_ID( true, "", "id");
const QName QN_DMS_PARENT_ID( true, "", "parentid");
const QName QN_DMS_NAME( true, "", "name" );
const QName QN_DMS_JID( true, "", "jid" );
const QName QN_DMS_TYPE( true, "", "type" );
const QName QN_DMS_RESERVERD1( true, "", "reserverd1" );
const QName QN_DMS_RESERVERD2( true, "", "reserverd2" );
const QName QN_DMS_RESERVERD3( true, "", "reserverd3" );
const QName QN_DMS_RESERVERD4( true, "", "reserverd4" );

const std::string NS_KEDACOM_ROOM("http://kedacom.com/chatroom");
const QName QN_PERSISTENT_ROOM( true, NS_KEDACOM_ROOM, "room" );
const QName QN_ITEM( true, "", "item" );
const QName QN_ROOM_ID( true, "", "id" );
const QName QN_ROOM_MUC_SERVICE( true, "", "mucid" );

const QName QN_FIRST( true, "", "first" );


const std::string NS_KEDACOM_ROOM_HISTORY("http://kedacom.com/chatroom_history");
const QName QN_CHATROOM_HISTORY( true, NS_KEDACOM_ROOM_HISTORY, "history" );
const QName QN_CHATROOM_SERVICE( true, "", "service" );

const QName QN_GROUPCHAT( true, "", "groupchat");


const std::string NS_REGISTER("jabber:iq:register");
const QName QN_REGISTER_QUERY( true, NS_REGISTER, "query" );
const QName QN_USER_NAME( true, "", "username" );
const QName QN_PASSWORD( true, "", "password" );

const QName QN_OPERATION( true, "", "operation" );


// PUBSUB
const std::string  NS_PUBSUB("http://jabber.org/protocol/pubsub");
const std::string  NS_PUBSUB_ADMIN("http://jabber.org/protocol/pubsub#owner");
const std::string  NS_XDATA("jabber:x:data");
const QName QN_PUBSUB( true, NS_PUBSUB, "pubsub" );
const QName QN_PUBSUB_ADMIN( true, NS_PUBSUB_ADMIN, "pubsub" );
const QName QN_CREATE( true, "", "create" );
const QName QN_DELETE( true, "", "delete" );
const QName QN_SUBSCRIBE( true, "", "subscribe" );
const QName QN_CONFIGURE( true, "", "configure" );
const QName QN_X( true, NS_XDATA, "x" );
const QName QN_FIELD( true, "", "field" );

const QName QN_DATA( true, "", "data" );




const QName QN_ADDRESSES( true, "http://jabber.org/protocol/address", "addresses");


const QName QN_DELAY( true, "urn:xmpp:delay", "delay" );
const QName QN_DELAY_X( true, "jabber:x:delay", "x" );


const QName QN_XBODY_BASE64( true, "moooo:xchat:base64", "xbody" );
const QName QN_SYS_NOTIFY( true, "moooo:sysnotify", "sysnotify" );
const std::string  NS_XBODY("moooo:xchat");
const QName QN_XBODY( true, NS_XBODY, "xbody" );

const std::string  NS_CUSTOM_BODY("moooo:custom");
const QName QN_COSTOM_BODY( true, NS_CUSTOM_BODY, "custombody" );
const QName QN_COSTOM_TYPE( true, NS_CUSTOM_BODY, "type" );
const QName QN_COSTOM_BUF( true, NS_CUSTOM_BODY, "buffer" );

const std::string  NS_CHARSTATES("http://jabber.org/protocol/chatstates");
const QName QN_CHAR_ACTIVE( true, NS_CHARSTATES, "active" );
const QName QN_CHAR_COMPOSING( true, NS_CHARSTATES, "composing" );
const QName QN_CHAR_PAUSED( true, NS_CHARSTATES, "paused" );

const QName QN_RECEIPT_REQUEST ( true, "urn:xmpp:receipts", "request" );
const QName QN_RECEIPT_RECEIVED( true, "urn:xmpp:receipts", "received" );


const QName QN_DMS_FONT( true, NS_XBODY, "font" );
const QName QN_DMS_COLOR( true, NS_XBODY, "color" );
const QName QN_DMS_TIME( true, NS_XBODY, "time" );
const QName QN_DMS_HEADER( true, NS_XBODY, "header" );
const QName QN_DMS_CONTENT( true, NS_XBODY, "content" );
const QName QN_DMS_CHAT_TYPE( true, NS_XBODY, "chat_type" );
const QName QN_DMS_PIC_COUNT( true, NS_XBODY, "pic_count" );
const QName QN_DMS_PIC( true, NS_XBODY, "pic" );
const QName QN_DMS_PIC_INDEX( true, NS_XBODY, "pic_index" );
const QName QN_DMS_PIC_PATH( true, NS_XBODY, "pic_path" );

const QName QN_DMS_FONT_1( true, NS_XBODY, "font_1" );

const QName QN_DMS_FONT_HEIGHT( true, NS_XBODY, "height" );
const QName QN_DMS_FONT_WIDTH( true, NS_XBODY, "width" );
const QName QN_DMS_FONT_ESCAPE( true, NS_XBODY, "escape" );
const QName QN_DMS_FONT_ORIENTATION( true, NS_XBODY, "orientation" );
const QName QN_DMS_FONT_WEIGHT( true, NS_XBODY, "weight" );

const QName QN_DMS_FONT_ITALIC( true, NS_XBODY, "italic" );
const QName QN_DMS_FONT_UNDERLINE( true, NS_XBODY, "underline" );
const QName QN_DMS_FONT_STRIKE_OUT( true, NS_XBODY, "strike_out" );

const QName QN_DMS_FONT_CHARSET( true, NS_XBODY, "charset" );
const QName QN_DMS_FONT_OUT_PRECISION( true, NS_XBODY, "out_precision" );
const QName QN_DMS_FONT_CLIP_PRECISION( true, NS_XBODY, "clip_precision" );
const QName QN_DMS_FONT_QUALITY( true, NS_XBODY, "quality" );
const QName QN_DMS_FONT_PITCH_AND_FAMILY( true, NS_XBODY, "pitch_and_family" );

const QName QN_DMS_FONT_FACE_NAME( true, NS_XBODY, "face_name" );

const QName QN_DMS_TIME_1( true, NS_XBODY, "time_1" );



const std::string  NS_IBB("http://jabber.org/protocol/ibb");
const QName QN_IBB_OPEN( true,  NS_IBB, "open" );
const QName QN_IBB_CLOSE( true, NS_IBB, "close" );
const QName QN_IBB_DATA( true, NS_IBB, "data" );
const QName QN_BLOCK_SIZE( true,  "", "block-size" );
const QName QN_SID ( true,  "", "sid" );
const QName QN_SEQ ( true,  "", "seq" );


const std::string  NS_TIME( "jabber:iq:time" );
const QName QN_TIME_QUERY( true, NS_TIME, "query" );
const QName QN_TIME( true, NS_TIME, "time" );

const std::string  NS_KEDA_ROSTER( "keda:roster" );
const QName QN_KEDA_ROSTER_BITRATE( true, NS_KEDA_ROSTER, "bitrate" );
const QName QN_KEDA_ROSTER_TYPE( true, NS_KEDA_ROSTER, "type" );

}
