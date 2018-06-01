/******************************************************************/
/* 说明：  代码从libjingle-0.4.0中提取出来                        */
/******************************************************************/



#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#define SECURITY_WIN32
#include <security.h>
#include <schannel.h>
#else
#include "sys/socket.h"
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/x509v3.h"
#endif

#include <assert.h>
#include <vector>
#include "ssl_adapter.h"





#define DEBUG_FLAG

#ifdef DEBUG_FLAG

#ifdef WIN32
#define  PERROR(f,...)                    fprintf( stderr, f, __VA_ARGS__ )
#define  MYTRACE(f,...)                   printf( f, __VA_ARGS__ )
#define  MYTRACE_W(f,...)                 wprintf( f, __VA_ARGS__ )
#else
#define  PERROR(f...)                     fprintf( stderr, f )
#define  MYTRACE(f...)                    printf( f )
#endif

#else

#ifdef WIN32
#define  PERROR(f,...)             
#define  MYTRACE(f,...)             
#define  MYTRACE_W(f,...)                 
#else
#define  PERROR(f...)                     
#define  MYTRACE(f...)                     
#endif

#endif // DEBUG_FLAG

#define  MAX_READ_BUF_SIZE                8192

#ifdef WIN32
#define  SLEEP_TIME                       10      // 毫秒
#else
#define  SLEEP_TIME                       10000   // 微秒
#endif




#ifndef WIN32
// This is the Equifax Secure global eBusiness CA-1 certificate
// in C form.

// It was generated with the following command line:
// > openssl x509 -in Equifax_Secure_Global_eBusiness_CA-1.cer -noout -C

// The certificate was retrieved from:
// http://www.geotrust.com/resources/root_certificates/certificates/Equifax_Secure_Global_eBusiness_CA-1.cer

/* subject:/C=US/O=Equifax Secure Inc./CN=Equifax Secure Global eBusiness CA-1 */
/* issuer :/C=US/O=Equifax Secure Inc./CN=Equifax Secure Global eBusiness CA-1 */
static unsigned char EquifaxSecureGlobalEBusinessCA1_certificate[660]={
    0x30,0x82,0x02,0x90,0x30,0x82,0x01,0xF9,0xA0,0x03,0x02,0x01,0x02,0x02,0x01,0x01,
    0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x04,0x05,0x00,0x30,
    0x5A,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x1C,
    0x30,0x1A,0x06,0x03,0x55,0x04,0x0A,0x13,0x13,0x45,0x71,0x75,0x69,0x66,0x61,0x78,
    0x20,0x53,0x65,0x63,0x75,0x72,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x2D,0x30,0x2B,
    0x06,0x03,0x55,0x04,0x03,0x13,0x24,0x45,0x71,0x75,0x69,0x66,0x61,0x78,0x20,0x53,
    0x65,0x63,0x75,0x72,0x65,0x20,0x47,0x6C,0x6F,0x62,0x61,0x6C,0x20,0x65,0x42,0x75,
    0x73,0x69,0x6E,0x65,0x73,0x73,0x20,0x43,0x41,0x2D,0x31,0x30,0x1E,0x17,0x0D,0x39,
    0x39,0x30,0x36,0x32,0x31,0x30,0x34,0x30,0x30,0x30,0x30,0x5A,0x17,0x0D,0x32,0x30,
    0x30,0x36,0x32,0x31,0x30,0x34,0x30,0x30,0x30,0x30,0x5A,0x30,0x5A,0x31,0x0B,0x30,
    0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x1C,0x30,0x1A,0x06,0x03,
    0x55,0x04,0x0A,0x13,0x13,0x45,0x71,0x75,0x69,0x66,0x61,0x78,0x20,0x53,0x65,0x63,
    0x75,0x72,0x65,0x20,0x49,0x6E,0x63,0x2E,0x31,0x2D,0x30,0x2B,0x06,0x03,0x55,0x04,
    0x03,0x13,0x24,0x45,0x71,0x75,0x69,0x66,0x61,0x78,0x20,0x53,0x65,0x63,0x75,0x72,
    0x65,0x20,0x47,0x6C,0x6F,0x62,0x61,0x6C,0x20,0x65,0x42,0x75,0x73,0x69,0x6E,0x65,
    0x73,0x73,0x20,0x43,0x41,0x2D,0x31,0x30,0x81,0x9F,0x30,0x0D,0x06,0x09,0x2A,0x86,
    0x48,0x86,0xF7,0x0D,0x01,0x01,0x01,0x05,0x00,0x03,0x81,0x8D,0x00,0x30,0x81,0x89,
    0x02,0x81,0x81,0x00,0xBA,0xE7,0x17,0x90,0x02,0x65,0xB1,0x34,0x55,0x3C,0x49,0xC2,
    0x51,0xD5,0xDF,0xA7,0xD1,0x37,0x8F,0xD1,0xE7,0x81,0x73,0x41,0x52,0x60,0x9B,0x9D,
    0xA1,0x17,0x26,0x78,0xAD,0xC7,0xB1,0xE8,0x26,0x94,0x32,0xB5,0xDE,0x33,0x8D,0x3A,
    0x2F,0xDB,0xF2,0x9A,0x7A,0x5A,0x73,0x98,0xA3,0x5C,0xE9,0xFB,0x8A,0x73,0x1B,0x5C,
    0xE7,0xC3,0xBF,0x80,0x6C,0xCD,0xA9,0xF4,0xD6,0x2B,0xC0,0xF7,0xF9,0x99,0xAA,0x63,
    0xA2,0xB1,0x47,0x02,0x0F,0xD4,0xE4,0x51,0x3A,0x12,0x3C,0x6C,0x8A,0x5A,0x54,0x84,
    0x70,0xDB,0xC1,0xC5,0x90,0xCF,0x72,0x45,0xCB,0xA8,0x59,0xC0,0xCD,0x33,0x9D,0x3F,
    0xA3,0x96,0xEB,0x85,0x33,0x21,0x1C,0x3E,0x1E,0x3E,0x60,0x6E,0x76,0x9C,0x67,0x85,
    0xC5,0xC8,0xC3,0x61,0x02,0x03,0x01,0x00,0x01,0xA3,0x66,0x30,0x64,0x30,0x11,0x06,
    0x09,0x60,0x86,0x48,0x01,0x86,0xF8,0x42,0x01,0x01,0x04,0x04,0x03,0x02,0x00,0x07,
    0x30,0x0F,0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x05,0x30,0x03,0x01,0x01,
    0xFF,0x30,0x1F,0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0xBE,0xA8,
    0xA0,0x74,0x72,0x50,0x6B,0x44,0xB7,0xC9,0x23,0xD8,0xFB,0xA8,0xFF,0xB3,0x57,0x6B,
    0x68,0x6C,0x30,0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,0xBE,0xA8,0xA0,
    0x74,0x72,0x50,0x6B,0x44,0xB7,0xC9,0x23,0xD8,0xFB,0xA8,0xFF,0xB3,0x57,0x6B,0x68,
    0x6C,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x04,0x05,0x00,
    0x03,0x81,0x81,0x00,0x30,0xE2,0x01,0x51,0xAA,0xC7,0xEA,0x5F,0xDA,0xB9,0xD0,0x65,
    0x0F,0x30,0xD6,0x3E,0xDA,0x0D,0x14,0x49,0x6E,0x91,0x93,0x27,0x14,0x31,0xEF,0xC4,
    0xF7,0x2D,0x45,0xF8,0xEC,0xC7,0xBF,0xA2,0x41,0x0D,0x23,0xB4,0x92,0xF9,0x19,0x00,
    0x67,0xBD,0x01,0xAF,0xCD,0xE0,0x71,0xFC,0x5A,0xCF,0x64,0xC4,0xE0,0x96,0x98,0xD0,
    0xA3,0x40,0xE2,0x01,0x8A,0xEF,0x27,0x07,0xF1,0x65,0x01,0x8A,0x44,0x2D,0x06,0x65,
    0x75,0x52,0xC0,0x86,0x10,0x20,0x21,0x5F,0x6C,0x6B,0x0F,0x6C,0xAE,0x09,0x1C,0xAF,
    0xF2,0xA2,0x18,0x34,0xC4,0x75,0xA4,0x73,0x1C,0xF1,0x8D,0xDC,0xEF,0xAD,0xF9,0xB3,
    0x76,0xB4,0x92,0xBF,0xDC,0x95,0x10,0x1E,0xBE,0xCB,0xC8,0x3B,0x5A,0x84,0x60,0x19,
    0x56,0x94,0xA9,0x55,
};
#endif



#ifndef WIN32

//////////////////////////////////////////////////////////////////////
// SocketBIO
//////////////////////////////////////////////////////////////////////

static int socket_write(BIO* h, const char* buf, int num);
static int socket_read(BIO* h, char* buf, int size);
static int socket_puts(BIO* h, const char* str);
static long socket_ctrl(BIO* h, int cmd, long arg1, void* arg2);
static int socket_new(BIO* h);
static int socket_free(BIO* data);

static BIO_METHOD methods_socket = 
{
    BIO_TYPE_BIO,
    "socket",
    socket_write,
    socket_read,
    socket_puts,
    0,
    socket_ctrl,
    socket_new,
    socket_free,
    NULL,
};

BIO_METHOD* BIO_s_socket2() { return(&methods_socket); }

BIO* BIO_new_socket( void * socket, lmn_base::CSslAdapter::socket_recv_cb recv_cb, lmn_base::CSslAdapter::socket_send_cb send_sb ) 
{
    BIO* ret = BIO_new(BIO_s_socket2());
    if (ret == NULL) 
    {
        return NULL;
    }

    void ** pParam = new void *[3];
    if ( NULL == pParam  )
    {
        return NULL;
    }

    pParam[0] = socket;
    pParam[1] = (void *)recv_cb;
    pParam[2] = (void *)send_sb;

    ret->ptr = (void *)pParam;

    return ret;
}

static int socket_new(BIO* b) 
{
    b->shutdown = 0;
    b->init = 1;
    b->num = 0; // 1 means socket closed
    b->ptr = 0;
    return 1;
}

static int socket_free(BIO* b) 
{
    if (b == NULL)
        return 0;

    void ** pParam = (void **)b->ptr;
    if ( pParam )
    {
        delete[] pParam;
        b->ptr = 0;
    }

    return 1;
}

static int socket_read(BIO* b, char* out, int outl) 
{
    if (!out)
        return -1;

    void ** pParam = (void **)b->ptr;
    if ( 0 == pParam )
    {
        return -1;
    }

    void * socket = pParam[0];
    lmn_base::CSslAdapter::socket_recv_cb recv_cb = (lmn_base::CSslAdapter::socket_recv_cb)pParam[1];
    // lmn_base::CSslAdapter::socket_send_cb send_cb = (lmn_base::CSslAdapter::socket_send_cb)pParam[2];

    BIO_clear_retry_flags(b);
    int result = 0;

    int nError = 0;
    int bBlocked = 0;
    if ( recv_cb )
    {
        result = recv_cb( socket, out, outl, &bBlocked );
        if ( result <= 0 && bBlocked )
        {
            result = -1;
            nError = EWOULDBLOCK;
        }
    }
    else
    {
        result = ::recv( (int)socket, out, outl, 0 );
        nError = errno;
    }

    // printf("really received length %d, error = %d \n", result, nError );


    if (result > 0) 
    {
        return result;
    } 
    else if (result == 0) 
    {
        b->num = 1;
    } 
    else if ( nError == EWOULDBLOCK ) 
    {
        BIO_set_retry_read(b);
    }
    return -1;
}

static int socket_write(BIO* b, const char* in, int inl) 
{
    if (!in)
        return -1;

    void ** pParam = (void **)b->ptr;
    if ( 0 == pParam )
    {
        return -1;
    }

    void * socket = pParam[0];
    // lmn_base::CSslAdapter::socket_recv_cb recv_cb = (lmn_base::CSslAdapter::socket_recv_cb)pParam[1];
    lmn_base::CSslAdapter::socket_send_cb send_cb = (lmn_base::CSslAdapter::socket_send_cb)pParam[2];

    
    BIO_clear_retry_flags(b);
    int result = 0;
    int nError = 0;
    int bBlocked = 0;

    // printf("required send length %d\n", inl );

    if ( send_cb )
    {
        result = send_cb( socket, in, inl, &bBlocked );
        if ( result <= 0 && bBlocked )
        {
            result = -1;
            nError = EWOULDBLOCK;
        }
    }
    else
    {
        result = ::send( (int)socket, in, inl, 0 );
        nError = errno;
    }

    // printf("really send length %d\n", result );
    
    if (result > 0) 
    {
        return result;
    } 
    else if (nError == EWOULDBLOCK) 
    {
        BIO_set_retry_write(b);
    }
    return -1;
}

static int socket_puts(BIO* b, const char* str) 
{
    return socket_write(b, str, strlen(str));
}

static long socket_ctrl(BIO* b, int cmd, long num, void* ptr) 
{
    switch (cmd) 
    {
    case BIO_CTRL_RESET:
        return 0;
    case BIO_CTRL_EOF:
        return b->num;
    case BIO_CTRL_WPENDING:
    case BIO_CTRL_PENDING:
        return 0;
    case BIO_CTRL_FLUSH:
        return 1;
    default:
        return 0;
    }
}

#endif





typedef struct tagConstantLabel
{
    int            value; 
    const char *   label;
}ConstantLabel, *PConstantLabel;

#define LABEL(x)       { x, #x }
#define LASTLABEL      { 0, 0  }


#ifdef WIN32
static const char * FindLabel( int value, const ConstantLabel entries[] ) 
{
    for ( int i=0; entries[i].label; ++i ) 
    {
        if ( value == entries[i].value) 
        {
            return entries[i].label;
        }
    }
    return 0;
}
#endif










namespace lmn_base
{





#ifdef WIN32

    /**************************   schannel ( for win32 ) **********************************/

    const ULONG SSL_FLAGS_DEFAULT =   ISC_REQ_ALLOCATE_MEMORY
                                    | ISC_REQ_CONFIDENTIALITY
                                    | ISC_REQ_EXTENDED_ERROR
                                    | ISC_REQ_INTEGRITY
                                    | ISC_REQ_REPLAY_DETECT
                                    | ISC_REQ_SEQUENCE_DETECT
                                    | ISC_REQ_STREAM;
                                  //| ISC_REQ_USE_SUPPLIED_CREDS;

    const ConstantLabel SECURITY_ERRORS[] = 
    {
        LABEL(SEC_I_COMPLETE_AND_CONTINUE),
        LABEL(SEC_I_COMPLETE_NEEDED),
        LABEL(SEC_I_CONTEXT_EXPIRED),
        LABEL(SEC_I_CONTINUE_NEEDED),
        LABEL(SEC_I_INCOMPLETE_CREDENTIALS),
        LABEL(SEC_I_RENEGOTIATE),
        LABEL(SEC_E_CERT_EXPIRED),
        LABEL(SEC_E_INCOMPLETE_MESSAGE),
        LABEL(SEC_E_INSUFFICIENT_MEMORY),
        LABEL(SEC_E_INTERNAL_ERROR),
        LABEL(SEC_E_INVALID_HANDLE),
        LABEL(SEC_E_INVALID_TOKEN),
        LABEL(SEC_E_LOGON_DENIED),
        LABEL(SEC_E_NO_AUTHENTICATING_AUTHORITY),
        LABEL(SEC_E_NO_CREDENTIALS),
        LABEL(SEC_E_NOT_OWNER),
        LABEL(SEC_E_OK),
        LABEL(SEC_E_SECPKG_NOT_FOUND),
        LABEL(SEC_E_TARGET_UNKNOWN),
        LABEL(SEC_E_UNKNOWN_CREDENTIALS),
        LABEL(SEC_E_UNSUPPORTED_FUNCTION),
        LABEL(SEC_E_UNTRUSTED_ROOT),
        LABEL(SEC_E_WRONG_PRINCIPAL),
        LASTLABEL
    };

    typedef std::vector<char> SChannelBuffer;

    // win32 下用到的类型
    struct SslImpl
    {
        CredHandle                 cred;
        CtxtHandle                 ctx;
        bool                       cred_init, ctx_init;
        SChannelBuffer             inbuf, outbuf, readable;
        SecPkgContext_StreamSizes  sizes;

        SecBufferDesc              out_buf_desc;
        SecBufferDesc              in_buf_desc;

        SecBuffer                  buffers_out[4];
        SecBuffer                  buffers_in[4];

        SslImpl() : cred_init(false), ctx_init(false) 
        {
        }

        void  InitInbufDesc(int nCnt)
        {
            memset( buffers_in, 0, sizeof(buffers_in) );

            in_buf_desc.ulVersion = SECBUFFER_VERSION;
            in_buf_desc.cBuffers  = nCnt;
            in_buf_desc.pBuffers  = buffers_in;
        }

        void  InitOutbufDesc(int nCnt)
        {
            memset( buffers_out, 0, sizeof(buffers_out) );

            out_buf_desc.ulVersion = SECBUFFER_VERSION;
            out_buf_desc.cBuffers  = nCnt;
            out_buf_desc.pBuffers  = buffers_out;
        }
    };

    class  CClearSecBuf
    {
    public:
        CClearSecBuf( _SecBufferDesc * pDesc ) : m_pDesc( pDesc ){};
        ~CClearSecBuf()
        {
            if ( m_pDesc )
            {
                unsigned long i;
                for ( i = 0; i < m_pDesc->cBuffers; i++ )
                {
                    if ( m_pDesc->pBuffers[i].pvBuffer )
                    {
                        SECURITY_STATUS status;
                        status = ::FreeContextBuffer( m_pDesc->pBuffers[i].pvBuffer );
                        assert( SEC_E_OK == status);
                    }
                }
                m_pDesc->cBuffers = 0;
                m_pDesc->pBuffers = 0;
            }
        }
        
    private:
        _SecBufferDesc * m_pDesc;
    };


    // schannel class
    // all functions return 0 if successful
    class  CSChannelAdapter : public CSslAdapter
    {
    public:

        CSChannelAdapter() : CSslAdapter()
        {

        }

        ~CSChannelAdapter()
        {
            StopSsl();
        }


        // start ssl
        int  StartSsl( void * socket, const char * szHostName = 0, bool bIgnoreBadCert = true, const SockCb * pCbParam = 0 )
        {
            // 正忙
            if ( SSL_CONNECTING == m_state || SSL_CONNECTED == m_state )
            {
                return -1;
            }

            StopSsl();

            m_socket = socket;
            if ( pCbParam )
            {
                m_pfnRecv  = pCbParam->pfnRecv;
                m_pfnSend  = pCbParam->pfnSend;
                m_pfnClose = pCbParam->pfnClose;
            }
            m_state  = SSL_CONNECTING;

            if ( szHostName )
            {
                m_szHost = new char[strlen(szHostName)+1];
                strcpy( m_szHost, szHostName );
            }

            m_bIgnordBadCert = bIgnoreBadCert;


            // 如果BeginSSL()失败
            if ( 0 != BeginSSL() )
            {
                m_state = SSL_ERROR;
                return -1;
            }

            return 0;
        }


        // stop ssl
        int  StopSsl()
        {
#if 0
            if ( !m_impl.readable.empty() ) 
            {
                MYTRACE( "SChannelAdapter::Close with readable data \n" );
                // Note: this isn't strictly an error, but we're using it temporarily to
                // track bugs.
            }

            if ( m_state == SSL_CONNECTED ) 
            {
                DWORD token = SCHANNEL_SHUTDOWN;

                m_impl.InitInbufDesc( 1 );
                m_impl.in_buf_desc.pBuffers[0].BufferType = SECBUFFER_TOKEN;
                m_impl.in_buf_desc.pBuffers[0].cbBuffer = sizeof(token);
                m_impl.in_buf_desc.pBuffers[0].pvBuffer = &token;
                ApplyControlToken( &m_impl.ctx, &m_impl.in_buf_desc );
                // TODO: In theory, to do a nice shutdown, we need to begin shutdown
                // negotiation with more calls to InitializeSecurityContext.  Since the
                // socket api doesn't support nice shutdown at this point, we don't bother.
            }
#endif

            Cleanup();

            return 0;
        }

        void Cleanup() 
        {
            //if ( -1 != m_socket )
            //{
            //    ::closesocket( m_socket );
            //    m_socket = -1;
            //}

            if ( m_pfnClose )
            {
                m_pfnClose( m_socket );
                m_socket = (void *)(-1);
            }
            else if ( -1 != (int)m_socket )
            {
                ::closesocket( (int)m_socket );
                m_socket = (void *)-1;
            }

            if ( m_szHost )
            {
                delete[]  m_szHost;
                m_szHost = 0;
            }

            if (m_impl.ctx_init)
            {
                DeleteSecurityContext(&m_impl.ctx);
                m_impl.ctx_init = false;
            }

            if (m_impl.cred_init)
            {
                FreeCredentialsHandle(&m_impl.cred);
                m_impl.cred_init = false;
            }

            m_impl.readable.clear();
            m_impl.inbuf.clear();
            m_impl.outbuf.clear();

            m_pfnRecv  = 0;
            m_pfnSend  = 0;
            m_pfnClose = 0;

            m_state = SSL_NONE;
        }


        // begin ssl
        int  BeginSSL()
        {
            SECURITY_STATUS ret;

            SCHANNEL_CRED   sc_cred = { 0 };
            sc_cred.dwVersion       = SCHANNEL_CRED_VERSION;
            sc_cred.dwFlags         = SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_AUTO_CRED_VALIDATION;

            ret = AcquireCredentialsHandle( NULL, UNISP_NAME, SECPKG_CRED_OUTBOUND, NULL,
                                            &sc_cred, NULL, NULL, &m_impl.cred, NULL );
            if ( ret != SEC_E_OK )
            {
                PERROR("AcquireCredentialsHandle error!(%d, %s) \n", ret, ::FindLabel(ret, SECURITY_ERRORS) );
                return ret;
            }

            m_impl.cred_init = true;


#if 0
            SecPkgCred_CipherStrengths cipher_strengths = { 0 };
            ret = QueryCredentialsAttributes( &m_impl.cred, SECPKG_ATTR_CIPHER_STRENGTHS, &cipher_strengths );
            if ( SUCCEEDED(ret) )
            {
                MYTRACE( "SChannel cipher strength: %lu - %lu \n", cipher_strengths.dwMinimumCipherStrength, 
                                                                   cipher_strengths.dwMaximumCipherStrength );
            }

            SecPkgCred_SupportedAlgs supported_algs = { 0 };
            ret = QueryCredentialsAttributes(&m_impl.cred, SECPKG_ATTR_SUPPORTED_ALGS, &supported_algs);
            if ( SUCCEEDED(ret) )
            {
                MYTRACE ( "SChannel supported algorithms: \n" );
                for ( DWORD i=0; i<supported_algs.cSupportedAlgs; ++i )
                {
                    ALG_ID alg_id          = supported_algs.palgSupportedAlgs[i];
                    PCCRYPT_OID_INFO oinfo = CryptFindOIDInfo(CRYPT_OID_INFO_ALGID_KEY, &alg_id, 0 );
                    LPCWSTR alg_name = (NULL != oinfo) ? oinfo->pwszName : L"Unknown";
                    MYTRACE_W(L"  %s (%u) \n", alg_name, alg_id );
                }

                if ( supported_algs.palgSupportedAlgs )
                {
                    ::FreeContextBuffer( supported_algs.palgSupportedAlgs );
                }
            }
#endif


            ULONG flags = SSL_FLAGS_DEFAULT, ret_flags = 0;
            if ( m_bIgnordBadCert )
            {
                flags |= ISC_REQ_MANUAL_CRED_VALIDATION;
            }

            m_impl.InitOutbufDesc( 2 );

            // 回收内存
            CClearSecBuf   destroy_out_buf ( &m_impl.out_buf_desc );

            ret = InitializeSecurityContextA( &m_impl.cred, NULL, m_szHost, flags, 0, 0, NULL, 0, &m_impl.ctx, &m_impl.out_buf_desc,
                                              &ret_flags,   NULL );
            if ( SUCCEEDED(ret) )
            {
                m_impl.ctx_init = true;
            }

            return ProcessContext(ret, NULL, &m_impl.out_buf_desc );
        }

        // process
        int ProcessContext( long int status, _SecBufferDesc* sbd_in, _SecBufferDesc* sbd_out )
        {
            if ( status == SEC_E_INCOMPLETE_MESSAGE )
            {
                // Wait for more input from server.
                return Flush();
            }

            if ( FAILED(status) )
            {
                // We can't continue.  Common errors:
                // SEC_E_CERT_EXPIRED - Typically, this means the computer clock is wrong.
                return status;
            }

            // Note: we check both input and output buffers for SECBUFFER_EXTRA.
            // Experience shows it appearing in the input, but the documentation claims
            // it should appear in the output.
            size_t extra = 0;
            if ( sbd_in ) 
            {
                for (size_t i=0; i<sbd_in->cBuffers; ++i) 
                {
                    SecBuffer& buffer = sbd_in->pBuffers[i];
                    if (buffer.BufferType == SECBUFFER_EXTRA) 
                    {
                        extra += buffer.cbBuffer;
                    }
                }
            }

            if ( sbd_out ) 
            {
                for (size_t i=0; i<sbd_out->cBuffers; ++i) 
                {
                    SecBuffer& buffer = sbd_out->pBuffers[i];
                    if (buffer.BufferType == SECBUFFER_EXTRA) 
                    {
                        extra += buffer.cbBuffer;
                    } else if (buffer.BufferType == SECBUFFER_TOKEN) 
                    {
                        m_impl.outbuf.insert( m_impl.outbuf.end(),
                            reinterpret_cast<char*>(buffer.pvBuffer),
                            reinterpret_cast<char*>(buffer.pvBuffer) + buffer.cbBuffer);
                    }
                }
            }

            if (extra) 
            {
                assert( extra <= m_impl.inbuf.size() );
                size_t consumed = m_impl.inbuf.size() - extra;
                memmove(&m_impl.inbuf[0], &m_impl.inbuf[consumed], extra);
                m_impl.inbuf.resize(extra);
            } 
            else 
            {
                m_impl.inbuf.clear();
            }

            if (SEC_I_CONTINUE_NEEDED == status) 
            {
                // Send data to server and wait for response.
                // Note: ContinueSSL will result in a Flush, anyway.
                return m_impl.inbuf.empty() ? Flush() : ContinueSSL();
            }

            if (SEC_E_OK == status) 
            {
                status = QueryContextAttributes(&m_impl.ctx, SECPKG_ATTR_STREAM_SIZES, &m_impl.sizes );
                if ( FAILED(status)) 
                {
                    PERROR("QueryContextAttributes error!(%d, %s) \n", status, ::FindLabel(status, SECURITY_ERRORS) );
                    return status;
                }

                m_state = SSL_CONNECTED;

                if (int err = DecryptData()) 
                {
                    return err;
                } 
                else if (int err = Flush()) 
                {
                    return err;
                } 
                else 
                {
                    // If we decrypted any data, queue up a notification here
                    // PostEvent();
                    // Signal our connectedness
                    // AsyncSocketAdapter::OnConnectEvent(this);
                }
                return 0;
            }

            if (SEC_I_INCOMPLETE_CREDENTIALS == status) 
            {
                // We don't support client authentication in schannel.
                return status;
            }

            // We don't expect any other codes
            assert(false);
            return status;
        }


        int Flush( ) 
        {
            int ret = 0;
            size_t pos = 0;
            SChannelBuffer& outbuf = m_impl.outbuf;

            while (pos < outbuf.size()) 
            {
                int nError   = 0;
                int sent     = 0;
                int bBlocked = 0;

                if ( m_pfnSend )
                {
                    sent = m_pfnSend( m_socket, &outbuf[pos], outbuf.size() - pos, &bBlocked );
                    if ( 0 <= sent )
                    {
                        if ( bBlocked )
                        {
                            nError = WSAEWOULDBLOCK;
                        }
                        else
                        {
                            nError = -1;
                        }
                    }
                }
                else
                {
                    sent = ::send( (int)m_socket, &outbuf[pos], outbuf.size() - pos, 0 );
                    nError = WSAGetLastError();
                }

                if ( sent > 0 ) 
                {
                    pos += sent;
                } 
                else
                {
                    if ( WSAEWOULDBLOCK == nError )
                    {
                        Sleep( SLEEP_TIME );
                        continue;
                    }
                    else
                    {
                        ret = nError;
                        break;
                    }
                } 


            }

            if (int remainder = outbuf.size() - pos) 
            {
                memmove(&outbuf[0], &outbuf[pos], remainder);
                outbuf.resize(remainder);
            } 
            else
            {
                outbuf.clear();
            }
            return ret;
        }

        int ContinueSSL() 
        {
            SECURITY_STATUS ret;

            m_impl.InitOutbufDesc( 2 );
            m_impl.InitInbufDesc( 2 );

            // 回收内存
            CClearSecBuf   destroy_out_buf ( &m_impl.out_buf_desc );

            m_impl.in_buf_desc.pBuffers[0].BufferType = SECBUFFER_TOKEN;
            m_impl.in_buf_desc.pBuffers[0].cbBuffer   = static_cast<unsigned long>(m_impl.inbuf.size());
            m_impl.in_buf_desc.pBuffers[0].pvBuffer   = &m_impl.inbuf[0];
            

            ULONG flags = SSL_FLAGS_DEFAULT, ret_flags = 0;
            if ( m_bIgnordBadCert )
            {
                flags |= ISC_REQ_MANUAL_CRED_VALIDATION;
            }

            ret = InitializeSecurityContextA( &m_impl.cred, &m_impl.ctx, m_szHost, flags, 0, 0, &m_impl.in_buf_desc, 0,
                                              NULL, &m_impl.out_buf_desc, &ret_flags, NULL);
            return ProcessContext( ret, &m_impl.in_buf_desc, &m_impl.out_buf_desc );
        }

        int DecryptData() 
        {
            SChannelBuffer& inbuf    = m_impl.inbuf;
            SChannelBuffer& readable = m_impl.readable;

            while (!inbuf.empty()) 
            {
                m_impl.InitInbufDesc( 4 );

                m_impl.in_buf_desc.pBuffers[0].BufferType = SECBUFFER_DATA;
                m_impl.in_buf_desc.pBuffers[0].cbBuffer = static_cast<unsigned long>(inbuf.size());
                m_impl.in_buf_desc.pBuffers[0].pvBuffer = &inbuf[0];

                SECURITY_STATUS status = DecryptMessage(&m_impl.ctx, &m_impl.in_buf_desc, 0, 0);

                // Note: We are explicitly treating SEC_E_OK, SEC_I_CONTEXT_EXPIRED, and
                // any other successful results as continue.
                if (SUCCEEDED(status) && status != SEC_I_CONTEXT_EXPIRED) 
                {
                    size_t data_len = 0, extra_len = 0;
                    for ( size_t i=0; i<m_impl.in_buf_desc.cBuffers; ++i) 
                    {
                        if (m_impl.in_buf_desc.pBuffers[i].BufferType == SECBUFFER_DATA) 
                        {
                            data_len += m_impl.in_buf_desc.pBuffers[i].cbBuffer;
                            readable.insert(readable.end(),
                                reinterpret_cast<char*>(m_impl.in_buf_desc.pBuffers[i].pvBuffer),
                                reinterpret_cast<char*>(m_impl.in_buf_desc.pBuffers[i].pvBuffer) + m_impl.in_buf_desc.pBuffers[i].cbBuffer);
                        } 
                        else if (m_impl.in_buf_desc.pBuffers[i].BufferType == SECBUFFER_EXTRA) 
                        {
                            extra_len += m_impl.in_buf_desc.pBuffers[i].cbBuffer;
                        }
                    }
                    // There is a bug on Win2K where SEC_I_CONTEXT_EXPIRED is misclassified.
                    if ((data_len == 0) && (inbuf[0] == 0x15)) 
                    {
                        status = SEC_I_CONTEXT_EXPIRED;
                    }
                    if (extra_len) 
                    {
                        size_t consumed = inbuf.size() - extra_len;
                        memmove(&inbuf[0], &inbuf[consumed], extra_len);
                        inbuf.resize(extra_len);
                    } 
                    else 
                    {
                        inbuf.clear();
                    }
                    // TODO: Handle SEC_I_CONTEXT_EXPIRED to do clean shutdown
                    if (status != SEC_E_OK) 
                    {
                        PERROR("DecryptMessage returned continuation code: %d(%s) \n", status, ::FindLabel(status, SECURITY_ERRORS) );
                        // return status;
                    }
                    continue;
                }

                if (status == SEC_E_INCOMPLETE_MESSAGE) 
                {
                    break;
                } 
                else 
                {
                    return status;
                }
            }
            return 0;
        }

        int Read()
        {
            char buffer[MAX_READ_BUF_SIZE];
            SChannelBuffer& inbuf = m_impl.inbuf;

            while (true)
            {
                int ret      = 0;
                int nError   = -1;
                int bBlocked = 0;

                if ( m_pfnRecv )
                {
                    ret = m_pfnRecv( m_socket, buffer, sizeof(buffer), &bBlocked );
                    if ( ret <= 0 && bBlocked )
                    {
                        nError = WSAEWOULDBLOCK;
                    }
                }
                else
                {
                    ret = ::recv( (int)m_socket, buffer, sizeof(buffer), 0 );
                    nError = WSAGetLastError();
                }

                if (ret > 0) 
                {
                    inbuf.insert( inbuf.end(), buffer, buffer + ret );
                } 
                else
                {
                    if ( WSAEWOULDBLOCK == nError )
                    {
                        return 0;
                    }
                    else
                    {
                        return nError;
                    }
                }
            }
        }

        SslState OnReadEvent( ) 
        {
            if ( SSL_ERROR == m_state || SSL_NONE == m_state ) 
            {
                return m_state;
            }

            if ( int err = Read() ) 
            {
                PERROR( "Read error(%d)! \n", err );
                m_state = SSL_ERROR;
                return m_state;
            }

            if ( m_impl.inbuf.empty() )
                return m_state;

            if ( m_state == SSL_CONNECTED )
            {
                if (int err = DecryptData()) 
                {
                    PERROR( "DecryptData error(%d, %s)! \n", err, ::FindLabel(err, SECURITY_ERRORS) );
                    m_state = SSL_ERROR;
                } 
            } 
            else if ( m_state == SSL_CONNECTING ) 
            {
                if (int err = ContinueSSL()) 
                {
                    PERROR("ContinueSSL error(%d, %s) \n", err, ::FindLabel(err, SECURITY_ERRORS));
                    m_state = SSL_ERROR;
                }
            }

            return m_state;
        }


        int Recv( void* pv, size_t cb)
        {
            switch ( m_state ) 
            {
            case SSL_NONE:
                return -1;
                break;

            case SSL_CONNECTING:
                return -1;
                break;

            case SSL_CONNECTED:
                break;

            case SSL_ERROR:
            default:
                return -1;
                break;
            }

            OnReadEvent();

            SChannelBuffer& readable = m_impl.readable;
            if ( readable.empty() ) 
            {
                return 0;
            }

            size_t read = min(cb, readable.size());
            memcpy(pv, &readable[0], read);
            if (size_t remaining = readable.size() - read) 
            {
                memmove(&readable[0], &readable[read], remaining);
                readable.resize(remaining);
            } 
            else 
            {
                readable.clear();
            }

            return static_cast<int>(read);
        }

        int Send(const void* pv, size_t cb)
        {
            switch ( m_state ) 
            {
            case SSL_NONE:
                return -1;
                break;

            case SSL_CONNECTING:
                return -1;
                break;

            case SSL_CONNECTED:
                break;

            case SSL_ERROR:
            default:
                return -1;
                break;
            }

            size_t written = 0;
            SChannelBuffer& outbuf = m_impl.outbuf;

            while (written < cb) 
            {
                const size_t encrypt_len = std::min<size_t>( cb - written, m_impl.sizes.cbMaximumMessage );

                m_impl.InitOutbufDesc( 4 );
                m_impl.out_buf_desc.pBuffers[0].BufferType = SECBUFFER_STREAM_HEADER;
                m_impl.out_buf_desc.pBuffers[0].cbBuffer = m_impl.sizes.cbHeader;
                m_impl.out_buf_desc.pBuffers[1].BufferType = SECBUFFER_DATA;
                m_impl.out_buf_desc.pBuffers[1].cbBuffer = static_cast<unsigned long>(encrypt_len);
                m_impl.out_buf_desc.pBuffers[2].BufferType = SECBUFFER_STREAM_TRAILER;
                m_impl.out_buf_desc.pBuffers[2].cbBuffer = m_impl.sizes.cbTrailer;

                size_t packet_len =   m_impl.out_buf_desc.pBuffers[0].cbBuffer
                                    + m_impl.out_buf_desc.pBuffers[1].cbBuffer
                                    + m_impl.out_buf_desc.pBuffers[2].cbBuffer;

                SChannelBuffer message;
                message.resize(packet_len);
                m_impl.out_buf_desc.pBuffers[0].pvBuffer = &message[0];
                m_impl.out_buf_desc.pBuffers[1].pvBuffer = &message[m_impl.out_buf_desc.pBuffers[0].cbBuffer];
                m_impl.out_buf_desc.pBuffers[2].pvBuffer = &message[m_impl.out_buf_desc.pBuffers[0].cbBuffer + m_impl.out_buf_desc.pBuffers[1].cbBuffer];

                memcpy(m_impl.out_buf_desc.pBuffers[1].pvBuffer, static_cast<const char*>(pv) + written, encrypt_len );

                SECURITY_STATUS res = EncryptMessage(&m_impl.ctx, 0, &m_impl.out_buf_desc, 0);

                if (FAILED(res)) 
                {
                    PERROR("EncryptMessage failed(%d, %s)\n", res, FindLabel(res, SECURITY_ERRORS));
                    return -1;
                }

                // We assume that the header and data segments do not change length,
                // or else encrypting the concatenated packet in-place is wrong.
                assert(m_impl.out_buf_desc.pBuffers[0].cbBuffer == m_impl.sizes.cbHeader);
                assert(m_impl.out_buf_desc.pBuffers[1].cbBuffer == static_cast<unsigned long>(encrypt_len));

                // However, the length of the trailer may change due to padding.
                assert(m_impl.out_buf_desc.pBuffers[2].cbBuffer <= m_impl.sizes.cbTrailer);

                packet_len =      m_impl.out_buf_desc.pBuffers[0].cbBuffer
                                + m_impl.out_buf_desc.pBuffers[1].cbBuffer
                                + m_impl.out_buf_desc.pBuffers[2].cbBuffer;

                written += encrypt_len;
                outbuf.insert( outbuf.end(), &message[0], &message[packet_len-1]+1 );
            }

            if (int err = Flush()) 
            {
                m_state = SSL_ERROR;
                return -1;
            }
            return static_cast<int>(written);
        }



    private:
        SslImpl      m_impl;
    }; // end of class schannel

#else

    typedef struct ssl_ctx_st SSL_CTX;

    /**************************   openssl channel ( for non-win32 ) ***********************/

    // openssl class
    // all functions return 0 if successful
    class  COpenSslAdapter : public CSslAdapter
    {
    public:

        COpenSslAdapter() : CSslAdapter()
        {
            ssl_read_needs_write_ = false;
            ssl_write_needs_read_ = false;
            ssl_                  = NULL;
            ssl_ctx_              = NULL;
        }

        ~COpenSslAdapter()
        {
            StopSsl();
        }


        // start ssl
        int  StartSsl( void * socket, const char * szHostName = 0, bool bIgnoreBadCert = true, const SockCb * pCbParam = 0 )
        {
            // 正忙
            if ( SSL_CONNECTING == m_state || SSL_CONNECTED == m_state )
            {
                return -1;
            }

            Cleanup();

            m_socket = socket;
            m_state  = SSL_CONNECTING;
            if ( szHostName )
            {
                m_szHost = new char[strlen(szHostName)+1];
                strcpy( m_szHost, szHostName );
            }

            m_bIgnordBadCert = bIgnoreBadCert;

            if ( pCbParam )
            {
                m_pfnRecv  = pCbParam->pfnRecv;
                m_pfnSend  = pCbParam->pfnSend;
                m_pfnClose = pCbParam->pfnClose;
            }


            // 如果BeginSSL()失败
            if ( 0 != BeginSSL() )
            {
                m_state = SSL_ERROR;
                return -1;
            }

            return 0;
        }

        int  StopSsl()
        {
            Cleanup();
            return 0;
        }

        
        int BeginSSL()
        {
            int err  = 0;
            BIO* bio = NULL;

            // First set up the context
            if (!ssl_ctx_)
                ssl_ctx_ = SetupSSLContext();

            if (!ssl_ctx_) 
            {
                err = -1;
                goto ssl_error;
            }

            bio = BIO_new_socket( m_socket, m_pfnRecv, m_pfnSend );
            if ( !bio ) 
            {
                err = -1;
                goto ssl_error;
            }

            ssl_ = SSL_new(ssl_ctx_);
            if (!ssl_) 
            {
                err = -1;
                goto ssl_error;
            }

            SSL_set_app_data(ssl_, this);

            SSL_set_bio(ssl_, bio, bio);
            SSL_set_mode(ssl_, SSL_MODE_ENABLE_PARTIAL_WRITE |
                SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

            // the SSL object owns the bio now
            bio = NULL;

            // Do the connect
            err = ContinueSSL();
            if (err != 0)
                goto ssl_error;

            return err;

        ssl_error:
            if (bio)
                BIO_free(bio); 
            return err;
        }

        SSL_CTX* SetupSSLContext() 
        {
            SSL_CTX* ctx = SSL_CTX_new( TLSv1_client_method() );
            if (ctx == NULL) 
                return NULL;

            // Add the root cert to the SSL context
#if OPENSSL_VERSION_NUMBER >= 0x0090800fL
            const unsigned char* cert_buffer
#else
            unsigned char* cert_buffer
#endif
                = EquifaxSecureGlobalEBusinessCA1_certificate;
            size_t cert_buffer_len = sizeof(EquifaxSecureGlobalEBusinessCA1_certificate);
            X509* cert = d2i_X509(NULL, &cert_buffer, cert_buffer_len);
            if (cert == NULL) 
            {
                SSL_CTX_free(ctx);
                return NULL;
            }
            if (!X509_STORE_add_cert(SSL_CTX_get_cert_store(ctx), cert)) 
            {
                X509_free(cert);
                SSL_CTX_free(ctx);
                return NULL;
            }

#ifdef _DEBUG
            SSL_CTX_set_info_callback(ctx, SSLInfoCallback);
#endif

            SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, SSLVerifyCallback);
            SSL_CTX_set_verify_depth(ctx, 4);
            SSL_CTX_set_cipher_list(ctx, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");

            X509_free(cert);
            return ctx;
        }

#ifdef _DEBUG
        static void SSLInfoCallback(const SSL* s, int where, int ret)
        {
            const char* str = "undefined";
            int w = where & ~SSL_ST_MASK;
            if (w & SSL_ST_CONNECT) 
            {
                str = "SSL_connect";
            } 
            else if (w & SSL_ST_ACCEPT) 
            {
                str = "SSL_accept";
            }
            if (where & SSL_CB_LOOP) 
            {
                MYTRACE( "%s:%s \n", str, SSL_state_string_long(s) );
            } 
            else if (where & SSL_CB_ALERT) 
            {
                str = (where & SSL_CB_READ) ? "read" : "write";
                MYTRACE( "SSL3 alert %s:%s:%s \n", str, SSL_alert_type_string_long(ret), SSL_alert_desc_string_long(ret) );
            } 
            else if (where & SSL_CB_EXIT) 
            {
                if (ret == 0) 
                {
                    MYTRACE ( "%s::failed in %s \n", str, SSL_state_string_long(s) );
                } 
                else if (ret < 0) 
                {
                    MYTRACE( "%s:error in %s \n", str, SSL_state_string_long(s) );
                }
            }
        }
#endif

        int ContinueSSL() 
        {
            int code = SSL_connect(ssl_);
            switch (SSL_get_error(ssl_, code)) 
            {

            case SSL_ERROR_NONE:
                // MYTRACE( " -- success \n" );
                if (!SSLPostConnectionCheck(ssl_, m_szHost)) 
                {
                    PERROR ("TLS post connection check failed");
                    //// make sure we close the socket
                    //Cleanup();
                    //// The connect failed so return -1 to shut down the socket
                    return -1;
                }

                m_state = SSL_CONNECTED;
                break;

            case SSL_ERROR_WANT_READ:
                // MYTRACE ( " -- error want read \n" );
                break;

            case SSL_ERROR_WANT_WRITE:
                // MYTRACE ( " -- error want write \n" );
                break;

            case SSL_ERROR_ZERO_RETURN:
            default:
                // MYTRACE ( " -- error %d \n", code );
                return (code != 0) ? code : -1;
            }
            return 0;
        }

        void Cleanup() 
        {
            ssl_read_needs_write_ = false;
            ssl_write_needs_read_ = false;

            if (ssl_) 
            {
                SSL_free(ssl_);
                ssl_ = NULL;
            }

            if (ssl_ctx_) 
            {
                SSL_CTX_free(ssl_ctx_);
                ssl_ctx_ = NULL;
            }

            if ( m_szHost )
            {
                delete[]  m_szHost;
                m_szHost = 0;
            }

            if ( m_pfnClose )
            {
                m_pfnClose( m_socket );
                m_socket = (void *)-1;
            }
            else if ( (void *)-1 != m_socket )
            {
                ::close( (int)m_socket );
                m_socket = (void *)-1;
            }

            m_pfnSend  = 0;
            m_pfnRecv  = 0;
            m_pfnClose = 0;
            m_state = SSL_NONE;
        }

        static int SSLVerifyCallback(int ok, X509_STORE_CTX* store) 
        {
#if _DEBUG
            if (!ok) 
            {
                char data[256];
                X509* cert = X509_STORE_CTX_get_current_cert(store);
                int depth = X509_STORE_CTX_get_error_depth(store);
                int err = X509_STORE_CTX_get_error(store);

                MYTRACE( "Error with certificate at depth: %d \n", depth );
                X509_NAME_oneline(X509_get_issuer_name(cert), data, sizeof(data));
                MYTRACE("  issuer  = %s \n", data);
                X509_NAME_oneline(X509_get_subject_name(cert), data, sizeof(data));
                MYTRACE("  subject = %s \n", data);
                MYTRACE("  err     = %d:%s \n", err, X509_verify_cert_error_string(err) );
            }
#endif

            // Get our stream pointer from the store
            SSL* ssl = reinterpret_cast<SSL*>( X509_STORE_CTX_get_ex_data(store,SSL_get_ex_data_X509_STORE_CTX_idx()) );

            COpenSslAdapter* stream = reinterpret_cast<COpenSslAdapter*>(SSL_get_app_data(ssl));

            if (!ok && stream->m_bIgnordBadCert ) 
            {
                MYTRACE ( "Ignoring cert error while verifying cert chain \n" );
                ok = 1;
            }
            return ok;
        }

        bool SSLPostConnectionCheck( SSL* ssl, const char* host ) 
        {
            if (!host)
                return false;

            // Checking the return from SSL_get_peer_certificate here is not strictly
            // necessary.  With our setup, it is not possible for it to return
            // NULL.  However, it is good form to check the return.
            X509* certificate = SSL_get_peer_certificate(ssl);
            if (!certificate)
                return false;

#ifdef _DEBUG
            MYTRACE("Certificate from server: \n");
            BIO* mem = BIO_new(BIO_s_mem());
            X509_print_ex(mem, certificate, XN_FLAG_SEP_CPLUS_SPC, X509_FLAG_NO_HEADER);
            BIO_write(mem, "\0", 1);
            char* buffer;
            BIO_get_mem_data(mem, &buffer);
            MYTRACE( buffer );
            MYTRACE( "\n" );
            BIO_free(mem);

            char* cipher_description =
                SSL_CIPHER_description(SSL_get_current_cipher(ssl), NULL, 128);
            MYTRACE( "Cipher: %s \n", cipher_description );
            OPENSSL_free(cipher_description);
#endif

            bool ok = false;
            int extension_count = X509_get_ext_count(certificate);
            for (int i = 0; i < extension_count; ++i) 
            {
                X509_EXTENSION* extension = X509_get_ext(certificate, i);
                int extension_nid = OBJ_obj2nid(X509_EXTENSION_get_object(extension));

                if (extension_nid == NID_subject_alt_name) {
                    X509V3_EXT_METHOD* meth = (X509V3_EXT_METHOD*)X509V3_EXT_get(extension);
                    if (!meth)
                        break;

                    void* ext_str = NULL;

#if OPENSSL_VERSION_NUMBER >= 0x0090800fL
                const unsigned char **ext_value_data = (const_cast<const unsigned char **>
                                                        (&extension->value->data));
#else
                unsigned char **ext_value_data = &extension->value->data;
#endif

                    if (meth->it) 
                    {
                        ext_str = ASN1_item_d2i(NULL, ext_value_data, extension->value->length,
                            ASN1_ITEM_ptr(meth->it));
                    } 
                    else 
                    {
                        ext_str = meth->d2i(NULL, ext_value_data, extension->value->length);
                    }

                    STACK_OF(CONF_VALUE)* value = meth->i2v(meth, ext_str, NULL);
                    for (int j = 0; j < sk_CONF_VALUE_num(value); ++j) 
                    {
                        CONF_VALUE* nval = sk_CONF_VALUE_value(value, j);
                        if (!strcmp(nval->name, "DNS") && !strcmp(nval->value, host)) 
                        {
                            ok = true;
                            break;
                        }
                    }
                }
                if (ok)
                    break;
            }

            char data[256];
            X509_name_st* subject;
            if (!ok && (subject = X509_get_subject_name(certificate))
                    && (X509_NAME_get_text_by_NID(subject, NID_commonName,
                data, sizeof(data)) > 0)) 
            {
                    data[sizeof(data)-1] = 0;
                    if (strcasecmp(data, host) == 0)
                        ok = true;
            }

            X509_free(certificate);

            if (!ok && m_bIgnordBadCert) 
            {
                MYTRACE("TLS certificate check FAILED.  Allowing connection anyway.\n");
                ok = true;
            }

            if (ok) 
                ok = (SSL_get_verify_result(ssl) == X509_V_OK);

            if (!ok && m_bIgnordBadCert) 
            {
                MYTRACE( "Other TLS post connection checks failed.\n" );
                ok = true;
            }
            return ok;
        }

        SslState OnReadEvent( ) 
        {
            if ( m_state == SSL_NONE ) 
            {
                return m_state;
            }

            if ( m_state == SSL_CONNECTING ) 
            {
                if (int err = ContinueSSL()) 
                {
                    PERROR( "ContinueSSL error(%d) \n", err);
                    m_state = SSL_ERROR;
                }
                return m_state;
            }

            return m_state;
        }

        int Recv( void* pv, size_t cb )
        {
            switch ( m_state ) 
            {
            case SSL_NONE:
              return -1;

            case SSL_CONNECTING:
              return -1;

            case SSL_CONNECTED:
              break;

            case SSL_ERROR:
            default:
              return -1;
            }

            // Don't trust OpenSSL with zero byte reads
            if ( cb == 0 )
                return 0;

            ssl_read_needs_write_ = false;


#if 0   // not try to full the pv buffer

            int code = SSL_read(ssl_, pv, cb);
            switch (SSL_get_error(ssl_, code)) 
            {
            case SSL_ERROR_NONE:
                // MYTRACE ( " -- SSL_read success \n" );
                return code;
            case SSL_ERROR_WANT_READ:
                // MYTRACE ( " -- SSL_read error want read \n" );
                return 0;
                break;
            case SSL_ERROR_WANT_WRITE:
                // MYTRACE (  " -- SSL_read error want write \n" );
                ssl_read_needs_write_ = true;
                break;
            case SSL_ERROR_ZERO_RETURN:
                // MYTRACE ( " -- SSL_read remote side closed \n" );
                break;
            default:
                // MYTRACE( "SSL_read error = %d \n", code );
                break;
            }
            m_state = SSL_ERROR;
            return -1;

#else  // try to full the pv buff

            size_t left   = cb;
            char * pRecv  = (char *)pv;

            int code = SSL_read( ssl_, pRecv, left );
            while ( 1 )
            {
                switch (SSL_get_error(ssl_, code)) 
                {
                case SSL_ERROR_NONE:

                    left -= (size_t)code;
                    // pv is full
                    if ( left == 0 )
                    {
                        return cb;
                    }
                    // not full
                    else
                    {
                        pRecv += code;
                        code = SSL_read(ssl_, pRecv, left);
                    }
                    break;
                    
                case SSL_ERROR_WANT_READ:
                    return cb - left;
                    break;

                case SSL_ERROR_WANT_WRITE:
                    ssl_read_needs_write_ = true;
                    m_state = SSL_ERROR;
                    return -1;
                    break;

                case SSL_ERROR_ZERO_RETURN:
                    m_state = SSL_ERROR;
                    // return -1;
                    return cb - left;
                    break;

                default:
                    m_state = SSL_ERROR;
                    return -1;
                    break;
                }
            }
            m_state = SSL_ERROR;
            return -1;
#endif

        }

        int Send( const void* pv, size_t cb )
        {
            switch ( m_state ) 
            {
            case SSL_NONE:
                return -1;

            case SSL_CONNECTING:
                return -1;

            case SSL_CONNECTED:
                break;

            case SSL_ERROR:
            default:
              return -1;
            }

            // OpenSSL will return an error if we try to write zero bytes
            if (cb == 0)
                return 0;


            ssl_write_needs_read_ = false;

#if 0
            int code = SSL_write(ssl_, pv, cb);
            switch (SSL_get_error(ssl_, code)) 
            {
            case SSL_ERROR_NONE:
                // MYTRACE (" -- success \n");
                return code;
            case SSL_ERROR_WANT_READ:
                // MYTRACE ( " -- error want read \n" );
                ssl_write_needs_read_ = true;
                break;
            case SSL_ERROR_WANT_WRITE:
                // MYTRACE ( " -- error want write, code = %d \n" );
                return -1;
                break;
            case SSL_ERROR_ZERO_RETURN:
                // MYTRACE ( " -- remote side closed \n" );
                break;
            default:
                // MYTRACE ( "SSL_write error = %d \n", code );
                break;
            }
            m_state = SSL_ERROR;
            return -1;
#else

            size_t dwTotal = 0;
            int code = SSL_write(ssl_, pv, cb);

            while ( dwTotal < cb )
            {
                if ( code > 0 )
                {
                    dwTotal += code;
                }
                else
                {
                    switch (SSL_get_error(ssl_, code)) 
                    {
                    case SSL_ERROR_NONE:
                        break;

                    case SSL_ERROR_WANT_READ:
                        ssl_write_needs_read_ = true;
                        m_state = SSL_ERROR;
                        return -1;
                        break;

                    case SSL_ERROR_WANT_WRITE:
                        usleep( SLEEP_TIME );
                        break;

                    case SSL_ERROR_ZERO_RETURN:
                        m_state = SSL_ERROR;
                        return -1;
                        break;

                    default:
                        m_state = SSL_ERROR;
                        return -1;        
                        break;
                    }
                }   

                code = SSL_write(ssl_, (const char *)pv+dwTotal, cb-dwTotal);
            }
            return (int)dwTotal;
#endif
        }

        private:
            SSL_CTX *   ssl_ctx_;
            SSL*        ssl_;
            bool        ssl_read_needs_write_;
            bool        ssl_write_needs_read_;

    }; // end of openssl class


    // initialize openssl
    class  CStatic4Openssl
    {
    public:
        CStatic4Openssl()
        {
            SSL_library_init();
            OpenSSL_add_all_algorithms();
            SSL_load_error_strings();
        }
    };

    static  CStatic4Openssl  s_static_for_openssl;


#endif



























    CSslAdapter::CSslAdapter()
    {
        m_socket         = (void *)-1;
        m_state          = SSL_NONE;
        m_szHost         = 0;
        m_bIgnordBadCert = false;;

        m_pfnRecv        = 0;
        m_pfnSend        = 0;
        m_pfnClose       = 0;
    }

    CSslAdapter::~CSslAdapter()
    {
        if ( m_szHost )
        {
            delete[]  m_szHost;
            m_szHost = 0;
        }
    }

    CSslAdapter::SslState  CSslAdapter::GetState() const
    {
        return m_state;
    }

    CSslAdapter * CSslAdapter::Create( )
    {
#ifdef WIN32
        return  new CSChannelAdapter();
#else
        return  new COpenSslAdapter();
#endif
    }

    void CSslAdapter::Destroy( CSslAdapter * pAdapter )
    {
        if ( pAdapter )
        {
            delete pAdapter;
        }
    }




} // end of namespace: lmn_base


