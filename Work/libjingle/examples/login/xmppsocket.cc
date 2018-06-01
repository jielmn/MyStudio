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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include "talk/base/basicdefs.h"
#include "talk/base/logging.h"
#include "talk/base/thread.h"
#ifdef FEATURE_ENABLE_SSL
#include "talk/base/ssladapter.h"
#endif
#include "xmppsocket.h"

#include "libjingle/libjingle_inner.h"

XmppSocket::XmppSocket( const buzz::XmppClientSettings & cxs ) {
 
  settings_ = cxs;

  talk_base::Thread* pth = talk_base::Thread::Current();
  talk_base::AsyncSocket* socket =
    pth->socketserver()->CreateAsyncSocket(SOCK_STREAM);
#ifdef FEATURE_ENABLE_SSL
  if (settings_.use_tls()) {
    socket = talk_base::SSLAdapter::Create(socket);
  }
#endif
  cricket_socket_ = socket;
  cricket_socket_->SignalReadEvent.connect(this, &XmppSocket::OnReadEvent);
  cricket_socket_->SignalWriteEvent.connect(this, &XmppSocket::OnWriteEvent);
  cricket_socket_->SignalConnectEvent.connect(this,
                                              &XmppSocket::OnConnectEvent);
  cricket_socket_->SignalCloseEvent.connect( this, &XmppSocket::OnCloseEvent );
  state_ = buzz::AsyncSocket::STATE_CLOSED;
}

XmppSocket::~XmppSocket() {
  Close();
  delete cricket_socket_;
}

void XmppSocket::OnReadEvent(talk_base::AsyncSocket * socket) {
    // 没有用代理
    if ( talk_base::PROXY_NONE == settings_.proxy() )
    {
        SignalRead(); 
    }
    // ss5 代理
    else
    {
        char bytes[4096];
        size_t bytes_read = 0;
        size_t i = 0;
        unsigned char send_data[MAX_BUFF_SIZE];
        unsigned long send_data_size;

        for (i = 0; ;i++)
        {
            if ( !Read(bytes, sizeof(bytes), &bytes_read) )
            {
                // TODO: deal with error information

#ifndef WIN32
                if ( 0 == i && 0 == bytes_read )
                {
                    int nError = GetError();
                    if ( !( (nError == EWOULDBLOCK) || (nError == EAGAIN) || (nError == EINPROGRESS) ) )
                    {
                        MYLOG("====[ OnSocketClosed ]  %s, %u, error = %d  \r\n", __FILE__, __LINE__, nError );
                        SignalClosed();
                    }
                }    
#endif

                return;
            }

            if (bytes_read == 0)
            {
                return;
            }

            
            if ( state_ == STATE_SS5_CONNECTED )
            {
                if ( 2 != bytes_read && (unsigned char)bytes[0] != SS5_PROXY_VERSION )
                {
                    SignalClosed();
                }

                // 如果是不用验证
                if ( SS5_PROXY_AUTH_METHOD_NO_AUTH == (unsigned char)bytes[1] )
                {
                    state_ = STATE_SS5_AUTH_NEGOTIATION_SUCCESS;

                    send_data_size = MAX_BUFF_SIZE;
                    GetSs5ProxyConnectQueryString( send_data, send_data_size, settings_.server().ip(), settings_.server().port() );
                    Write( (char *)send_data, send_data_size );
                }
                // 如果是用户名/密码验证
                else if ( SS5_PROXY_AUTH_METHOD_USER_PWD == (unsigned char)bytes[1] )
                {
                    state_ = STATE_SS5_AUTH_USER_PWD_METHOD_NEGOTIATED;

                    char szPwd[256];
                    settings_.proxy_pass().CopyTo( szPwd,  true );

                    send_data_size = MAX_BUFF_SIZE;
                    GetSs5ProxyUserPwdMethodQueryString( send_data, send_data_size, settings_.proxy_user().c_str(), szPwd );
                    Write( (char *)send_data, send_data_size );

                }
                // 不支持的验证方法
                else
                {
                    SignalClosed();
                }
            }
            else if ( state_ == STATE_SS5_AUTH_USER_PWD_METHOD_NEGOTIATED )
            {
                if ( 2 != bytes_read && (unsigned char)bytes[0] != '\x01' )
                {
                    SignalClosed();
                }

                // 验证失败
                if ( bytes[1] != '\x00' )
                {
                    SignalClosed();
                }
                else
                {
                    state_ = STATE_SS5_AUTH_NEGOTIATION_SUCCESS;

                    send_data_size = MAX_BUFF_SIZE;
                    GetSs5ProxyConnectQueryString( send_data, send_data_size, settings_.server().ip(), settings_.server().port() );
                    Write( (char *)send_data, send_data_size );
                }
            }
            else if ( state_ == STATE_SS5_AUTH_NEGOTIATION_SUCCESS )
            {
                if ( 10 != bytes_read && (unsigned char)bytes[0] != SS5_PROXY_VERSION && '\x00' != (unsigned char)bytes[1] )
                {
                    SignalClosed();
                }
                // proxy 连接成功
                else
                {
                    // 透明掉proxy
                    settings_.set_proxy( talk_base::PROXY_NONE );

#if defined(FEATURE_ENABLE_SSL)
                    if (state_ == buzz::AsyncSocket::STATE_TLS_CONNECTING) {
                        state_ = buzz::AsyncSocket::STATE_TLS_OPEN;
                        SignalSSLConnected();
                        OnWriteEvent(cricket_socket_);
                        return;
                    }
#endif  // !defined(FEATURE_ENABLE_SSL)
                    state_ = buzz::AsyncSocket::STATE_OPEN;
                    SignalConnected();
                }
            }

        }
    }
}

void XmppSocket::OnWriteEvent(talk_base::AsyncSocket * socket) {
  // Write bytes if there are any
  while (buffer_.Length() != 0) {
    int written = cricket_socket_->Send(buffer_.Data(), buffer_.Length());
    if (written > 0) {
      buffer_.Shift(written);
      continue;
    }
    if (!cricket_socket_->IsBlocking())
      LOG(LS_ERROR) << "Send error: " << cricket_socket_->GetError();
    return;
  }
}

void XmppSocket::OnConnectEvent(talk_base::AsyncSocket * socket) {

    // 没有用代理
    if ( talk_base::PROXY_NONE == settings_.proxy() )
    {
#if defined(FEATURE_ENABLE_SSL)
        if (state_ == buzz::AsyncSocket::STATE_TLS_CONNECTING) {
            state_ = buzz::AsyncSocket::STATE_TLS_OPEN;
            SignalSSLConnected();
            OnWriteEvent(cricket_socket_);
            return;
        }
#endif  // !defined(FEATURE_ENABLE_SSL)
        state_ = buzz::AsyncSocket::STATE_OPEN;
        SignalConnected();
    }
    // 使用了socks5代理
    else
    {
        state_ = STATE_SS5_CONNECTED;


        LibJingleEx::CMsgData   messageData;

        unsigned char ss5_proxy_version = SS5_PROXY_VERSION;
        messageData.Append( (unsigned char *)&ss5_proxy_version,  sizeof(ss5_proxy_version)  );

        unsigned char auth_method_cnt = 2;
        messageData.Append( (unsigned char *)&auth_method_cnt,  sizeof(auth_method_cnt)  );

        unsigned char auth_method = SS5_PROXY_AUTH_METHOD_NO_AUTH;
        messageData.Append( (unsigned char *)&auth_method,  sizeof(auth_method)  );
        auth_method = SS5_PROXY_AUTH_METHOD_USER_PWD;
        messageData.Append( (unsigned char *)&auth_method,  sizeof(auth_method)  );

        Write( (char *)messageData.GetMsgData(), messageData.GetMsgLength() );
    }
}

void XmppSocket::OnCloseEvent(talk_base::AsyncSocket * socket, int err)
{
    SignalClosed();
}

buzz::AsyncSocket::State XmppSocket::state() {
  return state_;
}

buzz::AsyncSocket::Error XmppSocket::error() {
  return buzz::AsyncSocket::ERROR_NONE;
}

int XmppSocket::GetError() {

#ifdef WIN32
    return 0;
#else
    return cricket_socket_->GetError();
#endif

}

bool XmppSocket::Connect(const talk_base::SocketAddress& addr) {
  if (cricket_socket_->Connect(addr) < 0) {
    return cricket_socket_->IsBlocking();
  }
  return true;
}

bool XmppSocket::Read(char * data, size_t len, size_t* len_read) {
  int read = cricket_socket_->Recv(data, len);
  if (read > 0) {
    *len_read = (size_t)read;
    return true;
  }
  return false;
}

bool XmppSocket::Write(const char * data, size_t len) {
  buffer_.WriteBytes(data, len);
  OnWriteEvent(cricket_socket_);
  return true;
}

bool XmppSocket::Close() {
#if defined(FEATURE_ENABLE_SSL)
  if (state_ != buzz::AsyncSocket::STATE_OPEN && state_ != buzz::AsyncSocket::STATE_TLS_OPEN )
#else
  if (state_ != buzz::AsyncSocket::STATE_OPEN )
#endif
     return false;
  if (cricket_socket_->Close() == 0) {
    state_ = buzz::AsyncSocket::STATE_CLOSED;
    SignalClosed();
    return true;
  }
  return false;
}

bool XmppSocket::StartTls(const std::string & domainname) {
#if defined(FEATURE_ENABLE_SSL)
  if (!settings_.use_tls())
    return false;
  talk_base::SSLAdapter* ssl_adapter =
    static_cast<talk_base::SSLAdapter *>(cricket_socket_);
  ssl_adapter->set_ignore_bad_cert(true);
  if (ssl_adapter->StartSSL(domainname.c_str(), false) != 0)
    return false;
  state_ = buzz::AsyncSocket::STATE_TLS_CONNECTING;
  return true;
#else  // !defined(FEATURE_ENABLE_SSL)
  return false;
#endif  // !defined(FEATURE_ENABLE_SSL)
}

void XmppSocket::StartCompression( )
{
#ifdef FEATURE_ENABLE_SSL
    if ( !settings_.use_tls() )
    {
        cricket_socket_->SetOption( talk_base::Socket::OPT_COMPRESSION, 1 );
    }
    else
    {
        talk_base::SSLAdapter* ssl_adapter =
            static_cast<talk_base::SSLAdapter *>(cricket_socket_);
        ssl_adapter->StartCompression();
    }
#else
    cricket_socket_->SetOption( talk_base::Socket::OPT_COMPRESSION, 1 );
#endif
}
