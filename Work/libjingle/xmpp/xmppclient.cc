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

#include "xmppclient.h"
#include "xmpptask.h"
#include "talk/xmpp/constants.h"
#include "talk/base/sigslot.h"
#include "talk/xmpp/saslplainmechanism.h"
#include "talk/xmpp/prexmppauth.h"
#include "talk/base/scoped_ptr.h"
#include "talk/xmpp/plainsaslhandler.h"

#include "talk/examples/login/xmpppump.h"

#include "libjingle/libjingle_inner.h"
#include <assert.h>

namespace buzz {

talk_base::Task* XmppClient::GetParent(int code) {
  if (code == XMPP_CLIENT_TASK_CODE)
    return this;
  else
    return talk_base::Task::GetParent(code);
}

class XmppClient::Private :
    public sigslot::has_slots<>,
    public XmppSessionHandler,
    public XmppOutputHandler {
public:

  Private(XmppClient * client) :
    client_(client),
    socket_(NULL),
    engine_(NULL),
    proxy_type_(talk_base::PROXY_NONE),
    pre_engine_error_(XmppEngine::ERROR_NONE),
    pre_engine_subcode_(0),
    signal_closed_(false),
    allow_plain_(false) {}

  // the owner
  XmppClient * const client_;

  // the two main objects
  scoped_ptr<AsyncSocket> socket_;
  scoped_ptr<XmppEngine> engine_;
  scoped_ptr<PreXmppAuth> pre_auth_;
  talk_base::CryptString pass_;
  std::string auth_cookie_;
  talk_base::SocketAddress server_;
  talk_base::SocketAddress proxy_server_;
  talk_base::ProxyType proxy_type_;
  XmppEngine::Error pre_engine_error_;
  int pre_engine_subcode_;
  CaptchaChallenge captcha_challenge_;
  bool signal_closed_;
  bool allow_plain_;
  void OnEngineDestroyed();

  // implementations of interfaces
  void OnStateChange(int state, int nErrorCode, int nSubCode);
  void WriteOutput(const char * bytes, size_t len);
  void StartTls(const std::string & domainname);
  void CloseConnection();
  void StartCompression();

  // slots for socket signals
  void OnSocketConnected();
  void OnSocketRead();
  void OnSocketClosed();
};

XmppReturnStatus
XmppClient::Connect(const XmppClientSettings & settings, const std::string & lang, AsyncSocket * socket, PreXmppAuth * pre_auth) {
  if (socket == NULL)
    return XMPP_RETURN_BADARGUMENT;
  if (d_->socket_.get() != NULL)
    return XMPP_RETURN_BADSTATE;

  d_->socket_.reset(socket);

  d_->socket_->SignalConnected.connect(d_.get(), &Private::OnSocketConnected);
  d_->socket_->SignalRead.connect(d_.get(), &Private::OnSocketRead);
  d_->socket_->SignalClosed.connect(d_.get(), &Private::OnSocketClosed);

  d_->engine_.reset(XmppEngine::Create());
  d_->engine_->SetSessionHandler(d_.get());
  d_->engine_->SetOutputHandler(d_.get());
  if (!settings.resource().empty()) {
    d_->engine_->SetRequestedResource(settings.resource());
  }
  d_->engine_->SetUseTls(settings.use_tls());
  d_->engine_->SetUseCompression(settings.use_compression());

  d_->engine_->m_SignalDestroyed.connect( d_.get(), &Private::OnEngineDestroyed );

  //
  // The talk.google.com server expects you to use "gmail.com" in the
  // stream, and expects the domain certificate to be "gmail.com" as well.
  // For all other servers, we leave the strings empty, which causes
  // the jid's domain to be used.  "foo@example.com" -> stream to="example.com"
  // tls certificate for "example.com"
  //
  // This is only true when using Gaia auth, so let's say if there's no preauth,
  // we should use the actual server name
  if ((settings.server().IPAsString() == buzz::STR_TALK_GOOGLE_COM ||
      settings.server().IPAsString() == buzz::STR_TALKX_L_GOOGLE_COM) && 
      pre_auth != NULL) {
    d_->engine_->SetTlsServer(buzz::STR_GMAIL_COM, buzz::STR_GMAIL_COM);
  }

  // Set language
  d_->engine_->SetLanguage(lang);

  d_->engine_->SetUser(buzz::Jid(settings.user(), settings.host(), STR_EMPTY));

  d_->pass_ = settings.pass();
  d_->auth_cookie_ = settings.auth_cookie();
  d_->server_ = settings.server();
  d_->proxy_server_ = settings.proxy_server();
  d_->proxy_type_ = settings.proxy();
  d_->allow_plain_ = settings.allow_plain();
  d_->pre_auth_.reset(pre_auth);

  return XMPP_RETURN_OK;
}

XmppEngine::State
XmppClient::GetState() {
  if (d_->engine_.get() == NULL)
    return XmppEngine::STATE_NONE;
  return d_->engine_->GetState();
}

XmppEngine::Error
XmppClient::GetError(int *subcode) {
  if (subcode) {
    *subcode = 0;
  }
  if (d_->engine_.get() == NULL)
    return XmppEngine::ERROR_NONE;
  if (d_->pre_engine_error_ != XmppEngine::ERROR_NONE) {
    if (subcode) {
      *subcode = d_->pre_engine_subcode_;
    }
    return d_->pre_engine_error_;
  }
  return d_->engine_->GetError(subcode);
}

const XmlElement *
XmppClient::GetStreamError() {
  if (d_->engine_.get() == NULL) {
    return NULL;
  }
  return d_->engine_->GetStreamError();
}

CaptchaChallenge XmppClient::GetCaptchaChallenge() {
  if (d_->engine_.get() == NULL)
    return CaptchaChallenge();
  return d_->captcha_challenge_;
}

std::string
XmppClient::GetAuthCookie() {
  if (d_->engine_.get() == NULL)
    return "";
  return d_->auth_cookie_;
}

static void
ForgetPassword(std::string & to_erase) {
  size_t len = to_erase.size();
  for (size_t i = 0; i < len; i++) {
    // get rid of characters
    to_erase[i] = 'x';
  }
  // get rid of length
  to_erase.erase();
}

int
XmppClient::ProcessStart() {
  if (d_->pre_auth_.get()) {
    d_->pre_auth_->SignalAuthDone.connect(this, &XmppClient::OnAuthDone);
    d_->pre_auth_->StartPreXmppAuth(
        d_->engine_->GetUser(), d_->server_, d_->pass_, d_->auth_cookie_);
    d_->pass_.Clear(); // done with this;
    return STATE_PRE_XMPP_LOGIN;
  }
  else {
    d_->engine_->SetSaslHandler(new PlainSaslHandler(
              d_->engine_->GetUser(), d_->pass_, d_->allow_plain_));
    d_->pass_.Clear(); // done with this;
    return STATE_START_XMPP_LOGIN;
  }
}

void
XmppClient::OnAuthDone() {
  Wake();
}

int
XmppClient::ProcessCookieLogin() {
  // Don't know how this could happen, but crash reports show it as NULL
  if (!d_->pre_auth_.get()) {
    d_->pre_engine_error_ = XmppEngine::ERROR_AUTH;
    EnsureClosed(XmppEngine::ERROR_AUTH,0);
    return STATE_ERROR;
  }

  // Wait until pre authentication is done is done
  if (!d_->pre_auth_->IsAuthDone())
    return STATE_BLOCKED;

  if (!d_->pre_auth_->IsAuthorized()) {
    // maybe split out a case when gaia is down?
    if (d_->pre_auth_->HadError()) {
      d_->pre_engine_error_ = XmppEngine::ERROR_AUTH;
      d_->pre_engine_subcode_ = d_->pre_auth_->GetError();
    }
    else {
      d_->pre_engine_error_ = XmppEngine::ERROR_UNAUTHORIZED;
      d_->pre_engine_subcode_ = 0;
      d_->captcha_challenge_ = d_->pre_auth_->GetCaptchaChallenge();
    }
    d_->pre_auth_.reset(NULL); // done with this
    EnsureClosed(XmppEngine::ERROR_UNAUTHORIZED, 0);
    return STATE_ERROR;
  }

  // Save auth cookie as a result
  d_->auth_cookie_ = d_->pre_auth_->GetAuthCookie();

  // transfer ownership of pre_auth_ to engine
  d_->engine_->SetSaslHandler(d_->pre_auth_.release());
  return STATE_START_XMPP_LOGIN;
}

int
XmppClient::ProcessStartXmppLogin() {

    if ( 0 == d_->socket_.get() )
    {
        EnsureClosed( 0, 0 );
        return STATE_ERROR;
    }

    if ( talk_base::PROXY_NONE == d_->proxy_type_ )
    {
        // Done with pre-connect tasks - connect!
        if (!d_->socket_->Connect(d_->server_)) {
            EnsureClosed(0, 0);
            return STATE_ERROR;
        }
    }
    else
    {        // Done with pre-connect tasks - connect!
        if (!d_->socket_->Connect(d_->proxy_server_)) {
            EnsureClosed(0, 0);
            return STATE_ERROR;
        }
    }
  
  
  return STATE_RESPONSE;
}

int
XmppClient::ProcessResponse() {
  // Hang around while we are connected.
  if (!delivering_signal_ && (d_->engine_.get() == NULL ||
    d_->engine_->GetState() == XmppEngine::STATE_CLOSED))
    return STATE_DONE;
  return STATE_BLOCKED;
}

XmppReturnStatus
XmppClient::Disconnect() {
  if (d_->socket_.get() == NULL)
    return XMPP_RETURN_BADSTATE;
  d_->engine_->Disconnect();
  return XMPP_RETURN_OK;
}

XmppClient::XmppClient(Task * parent) 
    : Task(parent), 
      delivering_signal_(false),
      valid_(false) {
  d_.reset(new Private(this));
  valid_ = true;

  m_hApp = 0;

  JInitLock( &m_tLock );

  XmppPump * pump = (XmppPump *)parent;
  SignalDestroyed.connect( pump, &XmppPump::OnXmppClientDestroyed );
}

XmppClient::~XmppClient() {
  valid_ = false;

  JDeinitLock( &m_tLock );

  SignalDestroyed();
}

const Jid &
XmppClient::jid() {
  return d_->engine_->FullJid();
}


std::string
XmppClient::NextId() {
  return d_->engine_->NextId();
}

XmppReturnStatus
XmppClient::SendStanza(const XmlElement * stanza) {
    cFuncLock   cLock( &m_tLock );

    if ( d_->engine_.get() )
    {
        return d_->engine_->SendStanza(stanza);
    }
    else
    {
        return XMPP_RETURN_UNEXPECTED;
    }
}

XmppReturnStatus
XmppClient::SendStanzaError(const XmlElement * old_stanza, XmppStanzaError xse, const std::string & message) {
    cFuncLock   cLock( &m_tLock );

    if ( d_->engine_.get() )
    {
        return d_->engine_->SendStanzaError(old_stanza, xse, message);
    }
    else
    {
        return XMPP_RETURN_UNEXPECTED;
    }
}

XmppReturnStatus
XmppClient::SendRaw(const std::string & text) {
    cFuncLock   cLock( &m_tLock );

    if ( d_->engine_.get() )
    {
        return d_->engine_->SendRaw(text);
    }
    else
    {
        return XMPP_RETURN_UNEXPECTED;
    }
}

XmppEngine*
XmppClient::engine() {
  return d_->engine_.get();
}

void
XmppClient::Private::OnSocketConnected() {
  engine_->Connect();
}

void
XmppClient::Private::OnSocketRead() {
  char bytes[4096];
  size_t bytes_read = 0;
  size_t i = 0;
  for (i = 0; ;i++) {
    if (!socket_->Read(bytes, sizeof(bytes), &bytes_read)) {
      // TODO: deal with error information

#ifndef WIN32
    if ( 0 == i && 0 == bytes_read )
    {
        int nError = socket_->GetError();
        if ( !( (nError == EWOULDBLOCK) || (nError == EAGAIN) || (nError == EINPROGRESS) ) )
        {
            MYLOG("====[ OnSocketClosed ]  %s, %u, error = %d  \r\n", __FILE__, __LINE__, nError );
            OnSocketClosed();
        }
    }    
#endif

      return;
    }

    if (bytes_read == 0)
      return;

//#ifdef _DEBUG
    client_->SignalLogInput(bytes, bytes_read);
//#endif

    engine_->HandleInput(bytes, bytes_read);
  }
}

void
XmppClient::Private::OnSocketClosed() {
    if ( engine_.get() )
    {
        int code = socket_->GetError();  
        engine_->ConnectionClosed(code);
    }
}


void
XmppClient::Private::OnEngineDestroyed()
{
    engine_.release();
}

void
XmppClient::Private::OnStateChange(int state, int nErrorCode, int nSubCode) {
  if (state == XmppEngine::STATE_CLOSED) {
    client_->EnsureClosed( nErrorCode, nSubCode );
  }
  else {
    client_->SignalStateChange((XmppEngine::State)state, nErrorCode, nSubCode);
  }
  client_->Wake();
}

void
XmppClient::Private::WriteOutput(const char * bytes, size_t len) {

//#ifdef _DEBUG
  client_->SignalLogOutput(bytes, len);
//#endif

  socket_->Write(bytes, len);
  // TODO: deal with error information
}

void
XmppClient::Private::StartTls(const std::string & domain) {
#if defined(FEATURE_ENABLE_SSL)
  socket_->StartTls(domain);
#endif
}

void
XmppClient::Private::StartCompression()
{
    socket_->StartCompression();
}

void
XmppClient::Private::CloseConnection() {
  socket_->Close();
}

void
XmppClient::AddXmppTask(XmppTask * task, XmppEngine::HandlerLevel level) {
  d_->engine_->AddStanzaHandler(task, level);
}

void
XmppClient::RemoveXmppTask(XmppTask * task) {
  d_->engine_->RemoveStanzaHandler(task);
}

void
XmppClient::EnsureClosed( int nError, int nSubCode) {
  if (!d_->signal_closed_) {
    d_->signal_closed_ = true;
    delivering_signal_ = true;
    SignalStateChange(XmppEngine::STATE_CLOSED, nError, nSubCode);
    delivering_signal_ = false;
  }
}


}
