#ifndef  _LMN_SSL_ADAPTER_
#define  _LMN_SSL_ADAPTER_


namespace  lmn_base
{
    class  CSslAdapter
    {
    public:
        enum SslState
        {
            SSL_NONE, 
            SSL_CONNECTING, 
            SSL_CONNECTED, 
            SSL_ERROR
        };

        typedef int (*socket_recv_cb) ( void * socket, void * data,       size_t length, int * pbBlocked );
        typedef int (*socket_send_cb) ( void * socket, const void * data, size_t length, int * pbBlocked );
        typedef int (*close_socket_cb)( void * socket );

        typedef struct  tagSockCb
        {
            socket_recv_cb    pfnRecv;
            socket_send_cb    pfnSend;
            close_socket_cb   pfnClose;
            tagSockCb()
            {
                memset( this, 0, sizeof(*this) );
            }
        }SockCb, *PSockCb;


        CSslAdapter();
        virtual ~CSslAdapter();

        // returns 0 if successful. must be called when the socket is connected
        virtual int  StartSsl( void * socket, const char * szHostName = 0, 
                               bool bIgnoreBadCert = true, const SockCb * pCbParam = 0 ) = 0;
        virtual int  StopSsl() = 0;

        // return received bytes count
        virtual int Recv( void* pv, size_t cb ) = 0;
        virtual int Send( const void* pv, size_t cb ) = 0;

        // update ssl state and store decrypted data
        // when returned value == SSL_CONNECTED, applicaton can call Recv()
        virtual SslState OnReadEvent( ) = 0;

        // Create the default SSL adapter for this platform
        static CSslAdapter * Create( );
        static void          Destroy( CSslAdapter * pAdapter );

        SslState  GetState() const;

    protected:
        void *       m_socket;
        SslState     m_state;
        char *       m_szHost;
        bool         m_bIgnordBadCert;

        socket_recv_cb   m_pfnRecv;
        socket_send_cb   m_pfnSend;
        close_socket_cb  m_pfnClose;
    };
}















#endif

