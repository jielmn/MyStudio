#include <stdio.h>

#include "MyConfigFile.h"
#include "LmnCommon.h"
#include "fileshare.h"
#include <string>
#include <assert.h>

#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#if VLD_ON
#include "vld.h"
#endif

#endif

#ifdef WIN32
#define CONFIG_NAME "config.txt"
#else
#define CONFIG_NAME "config1.txt"
#endif

//static  BOOL  s_bSender  = FALSE;
//static  char  s_SessionId[64];
//static  char  s_MyId[64];
//static  char  s_PeerId[64];
//static  HFileStream  s_hFile = 0;

static  TCreateFileShareSesionParam  s_tCreateFileShareParam;
static  HFileShareSession            s_hFileShareSession;

static  char s_MyIps[4][32];
static  int  s_nMyIpsCnt = 0;

static  unsigned long  s_dwStartTime = 0;
static  unsigned long  s_dwEndTime   = 0;

static  int   s_nFlag = 1;

#define NEGOTIATION_MSG                  '\x01'
#define IP_ADDR_MSG                      '\x02'

static void OnFssEvent_( int nEvent, HFileShareSession hSession, DWORD dwParam, void * pArgument )
{
    switch (nEvent)
    {
    case FSS_ON_FAILED_TO_ESTABLISH_CONNECTION:
        {
            printf("FSS_ON_FAILED_TO_ESTABLISH_CONNECTION \n");
        }
        break;

    case FSS_ON_START:
        {
            printf("FSS_ON_START \n");
            s_dwStartTime = LmnGetTickCount();
        }
        break;

    case FSS_ON_PROGRESS:
        {
            printf("FSS_ON_PROGRESS, transfer bytes = %u \n", dwParam );;
        }
        break;

    case FSS_ON_COMPLETE:
        {
            printf("FSS_ON_COMPLETE \n");
            CloseFileStream( s_tCreateFileShareParam.hFileStream );
            s_dwEndTime = LmnGetTickCount();

            float fTime = ( s_dwEndTime - s_dwStartTime ) / 1000.0f;
            printf("elapsed time = %0.2f \n", fTime );
        }
        break;

    case FSS_ON_ERROR:
        {
            printf("FSS_ON_ERROR, error = %u \n", dwParam );
            CloseFileStream( s_tCreateFileShareParam.hFileStream );
        }
        break;

    default:
        assert( 0 );
        break;
    }
}

static void OnSocketEvent_( FdType fd, int liEvent, int error, void * context )
{
    char buf[4096];
    int len = 0;
    int i, ret;

    switch ( liEvent)
    {
    case liEvConnect:
        {
            printf("connected! \n" );

            printf("send negotiation message \n");
            buf[0] = NEGOTIATION_MSG;
            buf[1] = (char)s_tCreateFileShareParam.bSender;
            len = 2;
            liTcpSend( fd, buf, &len );
        }
        break;

    case liEvAccept:
        {
            printf("accepted! \n" );
            liAccept( fd, OnSocketEvent_, (void *)100 );
            liClose( fd );
        }
        break;

    case liEvClose:
        {
            printf("closed! \n" );
            liClose( fd );


            // ≤‚ ‘”√
            if ( s_hFileShareSession )
                fssDestroySession( s_hFileShareSession );

            fssDeinit();
            s_nFlag = 0;


        }        
    	break;

    case liEvRead:
        {
            len = sizeof(buf);
            liTcpRecv( fd, buf, &len );

            if ( 100 == (int)context )
            {
                if ( NEGOTIATION_MSG == buf[0] )
                {
                    assert( len - 1 == 1 );

                    if ( (BOOL)buf[1] == s_tCreateFileShareParam.bSender )
                    {
                        printf("negotiation failed! \n");
                        liClose( fd );
                        return;
                    }

                    s_hFileShareSession = fssCreateSession( &s_tCreateFileShareParam );
                    if ( 0 == s_hFileShareSession )
                    {
                        printf("failed to create file share session! ");
                        liClose( fd );
                        return;
                    }

                    WORD wPort = 0;
                    ret = fssListen( s_hFileShareSession, &wPort );
                    if ( 0 != ret )
                    {   
                        printf("failed to fssListen()! ");
                        liClose( fd );
                        return;
                    }

                    printf( "file share listen port = %u \n", (DWORD)wPort );


                    TSocketInfo  tData;
                    tData.dwCount = htonl(s_nMyIpsCnt);
                    for ( i = 0; i < s_nMyIpsCnt; i++ )
                    {
                        tData.dwIpAddrs[i] = htonl( inet_addr(s_MyIps[i]) );
                    }
                    tData.wPort = htons(wPort);

                    buf[0] = IP_ADDR_MSG;
                    memcpy( buf + 1, &tData, sizeof(tData) );


                    printf("send ip address message \n");

                    len = 1 + sizeof(tData);
                    liTcpSend( fd, buf, &len );
                }
            }
            else if ( 2 == (int)context )
            {
                if ( IP_ADDR_MSG == buf[0] )
                {
                    assert( len - 1 == sizeof(TSocketInfo) );

                    TSocketInfo  tData;
                    memcpy( &tData, buf + 1, sizeof(tData) );
                    tData.dwCount = ntohl( tData.dwCount );
                    tData.wPort   = ntohs( tData.wPort );
                    for ( i = 0; i < (int)tData.dwCount; i++ )
                    {
                        tData.dwIpAddrs[i] = ntohl( tData.dwIpAddrs[i] );
                        printf("Server ip[%d] = 0x%08X \n", i, tData.dwIpAddrs[i] );
                    }
                    printf("Server listen port = %u \n", (DWORD)tData.wPort );

                    s_hFileShareSession = fssCreateSession( &s_tCreateFileShareParam );
                    if ( 0 == s_hFileShareSession )
                    {
                        printf("failed to fssCreateSession()! \n");
                        liClose( fd );
                        return;
                    }
                    
                    ret = fssConnect( s_hFileShareSession, &tData );
                    if ( 0 != ret )
                    {
                        printf("failed to fssConnect()! \n");
                        liClose( fd );
                        return;
                    }
                    
                }
            }

        }
        break;

    default:
        break;
    }
}

int main()
{
    MyConfigFile   config;

    try
    {
        config.LoadFile( CONFIG_NAME );
    }
    catch ( MyConfigFile::file_not_found_exception  e )
    {
        fprintf( stderr, "failed to load %s file! \n", e.filename.c_str() );
        getchar();
        return -1;
    }

    BOOL bServer = FALSE;
    WORD wPort = 0;
    DWORD dwTmp;

    if ( config["server"] == "1" )
    {
        bServer = TRUE;
    }

    if ( config["port"].length() > 0 )
    {
        dwTmp = 0;
        sscanf( config["port"].c_str(), "%u", &dwTmp );
        wPort = (WORD)dwTmp;
    }

    strncpy( s_tCreateFileShareParam.szUserId,      config["myid"].c_str(),      sizeof(s_tCreateFileShareParam.szUserId) );
    strncpy( s_tCreateFileShareParam.szPeerId,      config["peerid"].c_str(),    sizeof(s_tCreateFileShareParam.szPeerId) );
    strncpy( s_tCreateFileShareParam.szSessionId,   config["sessionid"].c_str(), sizeof(s_tCreateFileShareParam.szSessionId) );

    if ( config["sender"] == "1" )
    {
        s_tCreateFileShareParam.bSender = TRUE;
    }
    else
    {
        s_tCreateFileShareParam.bSender = FALSE;
    }

    printf("bServer = %s \n", bServer ? "true" : "false" );
    printf("bSender = %s \n", s_tCreateFileShareParam.bSender ? "true" : "false" );
    printf("s_SessionId = %s \n", s_tCreateFileShareParam.szSessionId );
    printf("s_MyId = %s \n", s_tCreateFileShareParam.szUserId );
    printf("s_PeerId = %s \n", s_tCreateFileShareParam.szPeerId );
    printf("file path = %s \n", config["file path"].c_str() );
    printf("my ips = %s \n", config["myips"].c_str() );


    if (    '\0' == s_tCreateFileShareParam.szSessionId[0] || '\0' == s_tCreateFileShareParam.szUserId[0] 
         || '\0' == s_tCreateFileShareParam.szPeerId[0] || 0 == config["file path"].length()
         ||    0 == config["myips"].length()  )
    {
        fprintf( stderr, "configure not correct!\n" );
        getchar();
        return -1;
    }


     char seps[]   = " ";
     char buf[1024];
     strncpy( buf,config["myips"].c_str(), sizeof(buf) );

     char *token = strtok( buf, seps );
     s_nMyIpsCnt = 0;

     while ( token && s_nMyIpsCnt < 4 )
     {
        strncpy( s_MyIps[s_nMyIpsCnt], token, sizeof(s_MyIps[s_nMyIpsCnt]) );
        printf("ip addr[%d] = %s \n", s_nMyIpsCnt, s_MyIps[s_nMyIpsCnt] );

        token = strtok( NULL, seps );
        s_nMyIpsCnt++;
     }


    if ( s_tCreateFileShareParam.bSender )
    {
        ELmnFileType eFileType = LmnGetFileType( config["file path"].c_str() );
        if ( E_FILE_TYPE_NONE == eFileType )
        {
            fprintf( stderr, "file not exist! \n");
            getchar();
            return -1;
        }

        s_tCreateFileShareParam.hFileStream = OpenFileStream( config["file path"].c_str(), TRUE );
        if ( 0 == s_tCreateFileShareParam.hFileStream )
        {
            fprintf( stderr, "can't open file/folder[%s] \n", config["file path"].c_str() );
            getchar();
            return -1;
        }
    }
    else
    {
        if ( config["file type"] == "file" )
        {
            s_tCreateFileShareParam.hFileStream = OpenFileStream( config["file path"].c_str(), FALSE, E_FILE_TYPE_FILE );
            if ( 0 == s_tCreateFileShareParam.hFileStream )
            {
                fprintf(stderr, "can't open file/folder[%s] for write \n", config["file path"].c_str() );
                getchar();
                return -1;
            }
        }
        else if ( config["file type"] == "folder" )
        {
            s_tCreateFileShareParam.hFileStream = OpenFileStream( config["file path"].c_str(), FALSE, E_FILE_TYPE_FOLDER );
            if ( 0 == s_tCreateFileShareParam.hFileStream )
            {
                fprintf(stderr, "can't open file/folder[%s] for write \n", config["file path"].c_str() );
                getchar();
                return -1;
            }
        }
        else
        {
            fprintf(stderr, "file type error! (%s) \n", config["file type"].c_str() );
            getchar();
            return -1;
        }
    }

    s_tCreateFileShareParam.pfnOnEvent = OnFssEvent_;



    seliInit( 64 );
    fssInit();

    int nContext = 1;
    if ( !bServer )
    {
        nContext = 2;
    }


    FdType fd = liOpen(  LI_PROTOTYPE_TCP, 0, wPort, OnSocketEvent_, (void *)nContext, 0  );
    if ( -1 == fd )
    {
        fprintf( stderr, "failed to create socket! \n");
        getchar();
        return -1;
    }

    if ( bServer )
    {
        if ( 0 != liListen( fd, 10 ) )
        {
            fprintf( stderr, "failed to listen! \n" );
            getchar();
            return -1;
        }

        printf("listen: port = %u \n", (DWORD)wPort );
    }
    else
    {
        if ( 0 == config["remote addr"].length() )
        {
            fprintf( stderr, "not set remote address! \n");
            getchar();
            return -1;
        }

        if ( 0 == config["remote port"].length() )
        {
            fprintf( stderr, "not set remote port! \n");
            getchar();
            return -1;
        }

        DWORD dwRemoteIp = 0;
        WORD  wRemotePort = 0;

        dwRemoteIp = inet_addr( config["remote addr"].c_str() );

        dwTmp = 0;
        sscanf( config["remote port"].c_str(), "%u", &dwTmp );
        wRemotePort = (WORD)dwTmp;

        printf("remote ip = %08x, port = %u \n", dwRemoteIp, (DWORD)wRemotePort );
        if ( 0 != liConnect( fd, dwRemoteIp, wRemotePort, 2 ) )
        {
            fprintf( stderr,"failed to liconnect()! \n");
            getchar();
            return -1;
        }
    }

    while( s_nFlag )
    {
        DWORD dwTime = 200;
        seliSelect( &dwTime );

        if ( dwTime > 0 )
        {
            LmnSleep( dwTime );
        }
    }

    
    seliEnd();
    getchar();
    return 0;
}