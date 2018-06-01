#include "StdAfx.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#ifndef WIN32
#include <unistd.h>
#include <iconv.h>
#include <sys/times.h>
#include <semaphore.h>
#endif

#include <new>

#include "Inner.h"
#include "LmnCommon.h"





class History
{
public:
    History( DWORD dwMaxSize = 10 );
    ~History();

    void Pushback( const char * cmd );
    const char * operator []( DWORD dwIndex );
    void Clear();
    DWORD GetSize();
    const char * GetPrevCmd();
    const char * GetNextCmd();
    void ResetCurIndex();

private:
    PArray              m_cmds;
    DWORD               m_dwMaxSize;
    DWORD               m_dwCurIndex;
};

History::History( DWORD dwMaxSize /* = 10 */ )
{
    if ( 0 == dwMaxSize )
    {
        dwMaxSize = 10;
    }

    m_dwMaxSize  = dwMaxSize;
    m_dwCurIndex = 0;
    m_cmds       = InitArray( m_dwMaxSize );
}

History::~History()
{
    Clear();
    if ( m_cmds )
    {
        DeinitArray( m_cmds );
    }
}

void History::Clear()
{
    DWORD dwSize = GetArraySize( m_cmds );

    DWORD i;
    for ( i = 0; i < dwSize; i++ )
    {
        g_pfnFreeMem( GetFromArray(m_cmds, i)  );
    }

    ClearArray( m_cmds );
}

void History::Pushback( const char * cmd )
{
    assert( 0 != cmd );

    DWORD dwSize = GetArraySize( m_cmds );
    DWORD dwLen  = strlen( cmd );

    char * p = 0;
    p = (char *)g_pfnAllocMem(dwLen + 1);
    strcpy( p, cmd );

    if ( 0 == dwSize )
    {
        Append2Array( m_cmds,  p );
    }
    else
    {
        assert( dwSize <= m_dwMaxSize );

        if ( dwSize == m_dwMaxSize )
        {
            g_pfnFreeMem( GetFromArray(m_cmds, 0) );
            EraseArray( m_cmds, 0 );
        }

        Append2Array( m_cmds,  p );
    }
}

const char * History::operator []( DWORD dwIndex )
{
    DWORD dwSize = GetArraySize( m_cmds );

    if ( dwIndex >= dwSize )
    {
        return 0;
    }
    else
    {
        return (const char *)GetFromArray(m_cmds, dwIndex);
    }
}

DWORD History::GetSize()
{
    return GetArraySize( m_cmds );
}

const char * History::GetPrevCmd()
{
    if ( m_dwCurIndex > 0 )
    {
        m_dwCurIndex--;

        return (const char *)GetFromArray(m_cmds, m_dwCurIndex);
    }
    else
    {
        return 0;
    }
}

const char * History::GetNextCmd()
{
    DWORD dwSize = GetArraySize( m_cmds );

    if ( m_dwCurIndex + 1 < dwSize )
    {
        m_dwCurIndex++;

        return (const char *)GetFromArray(m_cmds, m_dwCurIndex);
    }
    else
    {
        return 0;
    }
}

void History::ResetCurIndex()
{
    m_dwCurIndex = GetArraySize( m_cmds );
}





#define  MAX_SELIX_INIT_SOCKETS_COUNT       1024

typedef  struct  tagTelnetd_
{
	LPOnTelnetCmd		pfnOnCommand;
	DWORD				dwMaxCoUsersCnt;
	WORD                wPort;
	char                reserverd0[2];
	TRegTelnetdCmd *    ptCommands;
	DWORD               dwCommandsCnt;
    SelixNode           pSeliNode;

    LmnThrdType         TelnetdThrd;
    BOOL                bTelnetdLoopFlag;
    LmnLockType         lock_QueryCloseClients;
    PList               lstQueryCloseClients;

    PList               lstQueryPrintf;

    LmnThrdHandle       hListenThrd;
}TTelnetd_, *PTTelnetd_;

#define  SOCKET_TYPE_LISTEN                 0
#define  SOCKET_TYPE_CLIENT                 1

typedef struct  tagSocketParam_
{
    PTTelnetd_          pTelnetParam;
    int                 nSocketType;
    FdType              fd;
    LmnThrdHandle       hClientThrd;
    char                szCommand[NORMAL_BUF_SIZE];
    DWORD               dwCommandPos;
    DWORD               dwCommandLen;
    History *           history;
}TSocketParam_, *PTSocketParam_;


static   BOOL          s_bTelnetdInited   = FALSE;

//static   LmnThrdType   s_TelnetdThrd      = 0;
//static   BOOL          s_bTelnetdLoopFlag = FALSE;
//static   LmnLockType   s_lock;
//static   PList         s_lstQueryCloseClients = 0;

static   PTTelnetd_    s_pTelnet          = 0;
static   LmnLockType   s_lock_telnet;




typedef struct tagPrintfParam_
{
    DWORD               dwClientId;
    char *              pMsg;
}TPrintfParam_, *PTPrintfParam_;



#define  CLIENT_MSG_ID_INIT            0
#define  CLIENT_MSG_ID_DATA            1
#define  CLIENT_MSG_ID_DESTROY_CLIENT_THRD    2

#define  SYSTEM_COMMANDS_COUNT         2

static  TRegTelnetdCmd  s_SystemCommand[SYSTEM_COMMANDS_COUNT];




#define IAC      ((BYTE)0xFF)
#define WILL     ((BYTE)0xFB)
#define WONT     ((BYTE)0xFC)
#define DO       ((BYTE)0xFD)
#define DONT     ((BYTE)0xFE)
#define SB       ((BYTE)0xFA)
#define SE       ((BYTE)0xF0)


#define TELNET_HEAD   "telnet server\r\n"


#define DELETE_SEND1         "\177"
#define DELETE_SEND1_LEN     1

#define DELETE_SEND2         "\033[3~"
#define DELETE_SEND2_LEN     4

#define BACKSPACE_SEND1      "\010"
#define BACKSPACE_SEND1_LEN  1

#define BACKSPACE_SEND2      "\177"
#define BACKSPACE_SEND2_LEN  1

#define UP_SEND             "\033[A"
#define DOWN_SEND           "\033[B"
#define LEFT_SEND           "\033[D"
#define RIGHT_SEND          "\033[C"

#define CLEAR_SEND          "\033[K"

#ifdef WIN32
#define CMD_END_SEND        "\r\n"
#else
#define CMD_END_SEND        "\r\0"
#endif

#define CURSOR_LEFT         "\010"

#define PROMPT              "$:"
#define ARGS_NOT_ENOUGH_MSG "Not enough arguments!\r\n"
#define ARG_TYPE_WRONG      "The type of %dth argument is wrong!\r\n"
#define COMMAND_NOT_FOUND_MSG "%s: command not found\r\n"


static void ClearPrintfList( PList lst )
{
    PListNode pNode = GetListHead(lst);
    while( pNode )
    {
        PTPrintfParam_  pParam = (PTPrintfParam_)pNode->pData;
        assert( pParam && pParam->pMsg );
        g_pfnFreeMem( pParam->pMsg );
        g_pfnFreeMem( pParam );
        pNode = GetNextListNode( pNode );
    }
    ClearList( lst );
}

static void ClearPrintfListByClientId( PList lst, DWORD dwClientId )
{
    PListNode pNode = GetListHead(lst);
    while( pNode )
    {
        PTPrintfParam_  pParam = (PTPrintfParam_)pNode->pData;
        assert( pParam && pParam->pMsg );

        if ( pParam->dwClientId == dwClientId )
        {
            g_pfnFreeMem( pParam->pMsg );
            g_pfnFreeMem( pParam );

            pNode = EraseList( lst, pNode );
        }
        else
        {
            pNode = GetNextListNode( pNode );
        }
    }
}


static BYTE * WriteByte2Buff( BYTE * buff, DWORD dwSize, DWORD dwCount, ...)
{
    assert( buff );
    assert( dwCount <= dwSize );

    va_list vl;

    va_start( vl, dwCount );     /* Initialize variable arguments. */

    unsigned int i;
    for ( i = 0; i < dwCount; i++ )
    {
        buff[i] = (BYTE)va_arg( vl, int );
    }

    va_end( vl );

    return buff;
}

static int SendPrompt( SelixNode pSeli, FdType fd )
{
    int len = strlen(PROMPT);
    return lixTcpCompleteSend( pSeli, fd,  PROMPT, &len, 0 );
}

static DWORD FilterIac( BYTE * pchCmd, DWORD dwSize, BYTE * pchData, DWORD dwLen )
{
    DWORD i = 0;
    DWORD j = 0;

    while( i < dwLen )
    {
        if ( IAC == pchData[i] )
        {
            i++;
            if ( SB == pchData[i] )
            {
                i++;
                while( !(pchData[i-1] != IAC && pchData[i] == SE) && (i < dwLen) )
                {
                    i++;
                }
            }
            else
            {
                i += 2;
            }
        }
        else
        {
            if ( j < dwSize )
            {
                pchCmd[j] = pchData[i];

                j++;
                i++;
            }
            else
            {
                break;
            }
        }
    }

    return j;
}

static int SendLeftCmd( SelixNode pSeli, FdType fd )
{
    int len = 1;
    return lixTcpCompleteSend( pSeli, fd,  CURSOR_LEFT, &len, 0 );
}

static int SendRightCmd( SelixNode pSeli, FdType fd )
{
    int len = 3;
    return lixTcpCompleteSend( pSeli, fd,  RIGHT_SEND, &len, 0 );
}

static BOOL IsWhiteChar( char ch )
{
    if ( ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void ParseRawCommand( const char * szCommand, 
                             PArray  tokens )
{
    DWORD dwLen = strlen( szCommand );

    DWORD          i = 0;
    BOOL bTokenStart = FALSE;
    BOOL bInQuote    = FALSE;
    DWORD dwTokenLen = 0;

    char * pToken = (char *)g_pfnAllocMem ( (dwLen + 1) * sizeof(char) );

    while( i < dwLen )
    {
        char ch = szCommand[i];

        if ( bTokenStart )    
        {
            if ( bInQuote )
            {
                if ( '"' == ch )
                {
                    if ( i + 1 < dwLen )
                    {
                        if ( IsWhiteChar(szCommand[i+1]) )
                        {
                            bInQuote    = FALSE;
                            bTokenStart = FALSE;
                            pToken[dwTokenLen] = '\0';

                            //存储起来
                            char * tmp = new char[dwTokenLen + 1];
                            strcpy(tmp, pToken);

                            Append2Array( tokens, (void *)tmp );
                            dwTokenLen = 0;

                            i += 2;
                        }
                        else
                        {
                            if ( '"' == szCommand[i+1] )
                            {
                                pToken[dwTokenLen] = '\0';

                                //存储起来
                                char * tmp = (char *)g_pfnAllocMem( (dwTokenLen + 1) * sizeof(char) );
                                strcpy(tmp, pToken);

                                Append2Array( tokens, (void *)tmp );
                                dwTokenLen = 0;

                                bInQuote    = TRUE;
                                bTokenStart = TRUE;

                                i += 2;
                            }
                            else
                            {
                                pToken[dwTokenLen] = szCommand[i+1];
                                dwTokenLen++;

                                bInQuote = FALSE;

                                i += 2;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                else if ( '\\' == ch )
                {
                    if ( i + 1 < dwLen )
                    {
                        pToken[dwTokenLen] = szCommand[i+1];
                        dwTokenLen++;

                        i += 2;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    pToken[dwTokenLen] = ch;
                    dwTokenLen++;

                    i++;
                }
            }
            else
            {
                if ( IsWhiteChar(ch) )
                {
                    bTokenStart = FALSE;
                    pToken[dwTokenLen] = '\0';

                    //存储起来
                    char * tmp = (char*)g_pfnAllocMem( (dwTokenLen + 1) * sizeof(char) );
                    strcpy(tmp, pToken);

                    Append2Array( tokens, (void *)tmp );
                    dwTokenLen = 0;

                    i++;
                }
                else
                {
                    pToken[dwTokenLen] = ch;
                    dwTokenLen++;

                    i++;
                }
            }
        }
        else
        {
            if ( IsWhiteChar(ch) )
            {
                i++;
            }
            else
            {
                if ( '"' == ch )
                {
                    bTokenStart = TRUE;
                    bInQuote = TRUE;
                    dwTokenLen = 0;

                    i++;
                }
                else
                {
                    bTokenStart = TRUE;

                    pToken[0] = ch;
                    dwTokenLen = 1;

                    i++;
                }
            }
        }
    }

    if ( bTokenStart )
    {
        pToken[dwTokenLen] = '\0';

        char * tmp = (char *)g_pfnAllocMem( (dwTokenLen + 1) * sizeof(char) );
        strcpy(tmp, pToken);

        Append2Array( tokens, (void *)tmp );
        dwTokenLen = 0;
    }

    g_pfnFreeMem( pToken );
}

static void ClearStringVector( PArray  tokens )
{
    DWORD  dwSize = GetArraySize  ( tokens );
    DWORD  i;

    for ( i = 0; i < dwSize; i++ )
    {
        g_pfnFreeMem( GetFromArray( tokens, i) );
    }
    ClearArray( tokens );
}

static char * CompleteCommand( char * szCompleteCmd, DWORD dwSize, 
                               const char * szCmd,
                               PArray cmds )
{
    DWORD dwCmdLen = strlen( szCmd );

    PArray new_cmds = InitArray( 10 );

    DWORD i;
    DWORD dwCmdCnt = GetArraySize( cmds );
    const char * pPrev = 0;

    for ( i = 0; i < dwCmdCnt; i++ )
    {
        const char * pItemCmd      = (const char *)GetFromArray( cmds, i );
        DWORD        dwItemCmdLen  = strlen( pItemCmd );

        int nRet = strcmp( szCmd, pItemCmd );

        if ( nRet > 0 )
        {

        }
        else if ( 0 == nRet )
        {
            strcpy( szCompleteCmd, szCmd );
            DeinitArray( new_cmds );
            return szCompleteCmd;
        }
        else
        {
            if ( dwItemCmdLen <= dwCmdLen )
            {
                break;
            }
            else
            {
                if ( 0 != memcmp( szCmd, pItemCmd, dwCmdLen ) )
                {
                    break;
                }

                if ( 0 == pPrev )
                {
                    pPrev = pItemCmd;
                    Append2Array( new_cmds, (void *)pItemCmd );
                }
                else
                {
                    if ( pPrev[dwCmdLen] != pItemCmd[dwCmdLen] )
                    {
                        strcpy( szCompleteCmd, szCmd );
                        DeinitArray( new_cmds );
                        return szCompleteCmd;
                    }
                    else
                    {
                        Append2Array( new_cmds, (void *)pItemCmd );
                        pPrev = pItemCmd;
                    }
                }
            }
        }
    }

    if ( GetArraySize(new_cmds) > 0 )
    {
        if ( GetArraySize(new_cmds) > 1 )
        {
            const char * pItemCmd = (const char *)GetFromArray(new_cmds, 0);

            char * tmp = (char *)g_pfnAllocMem ( (dwCmdLen + 2) * sizeof(char) );
            memcpy( tmp, pItemCmd, dwCmdLen + 1 );
            tmp[dwCmdLen + 1] = '\0';

            char * pRet = CompleteCommand( szCompleteCmd, dwSize, tmp, new_cmds );
            DeinitArray( new_cmds );
            g_pfnFreeMem( tmp );
            return pRet;
        }
        else
        {
            const char * pItemCmd = (const char *)GetFromArray(new_cmds, 0);
            strcpy( szCompleteCmd, pItemCmd );
            DeinitArray( new_cmds );
            return szCompleteCmd;
        }
    }
    else
    {
        DeinitArray( new_cmds );
        return 0;
    }
}


static char * FindAlikeCmd( char * szAlikeCmd,     DWORD dwSize, 
                            const char * szRawCmd, DWORD dwCmdPos, PTTelnetd_  pTelnet )
{
    assert( szAlikeCmd && dwSize > 0 && szRawCmd );

    DWORD dwRawLen = strlen( szRawCmd );
    assert( dwCmdPos <= dwRawLen );

    if ( ' ' != szRawCmd[dwCmdPos] && '\0' != szRawCmd[dwCmdPos] )
    {
        return 0;
    }

    char * tmp = (char *)g_pfnAllocMem( (dwCmdPos + 1) * sizeof(char) );
    memcpy( tmp, szRawCmd, dwCmdPos );
    tmp[dwCmdPos] = '\0';

    PArray  tokens = InitArray( 10 );
    ParseRawCommand( tmp, tokens );
    g_pfnFreeMem( tmp );

    //全是空白字符
    if ( 0 == GetArraySize( tokens ) )
    {
        DeinitArray( tokens );
        return 0;
    }

    //前面至少有2个token
    if ( GetArraySize( tokens ) > 1 )
    {
        ClearStringVector(tokens);
        DeinitArray( tokens );
        return 0;
    }

    BOOL bInQuote = FALSE;
    char * pCmd = (char *)GetFromArray( tokens, 0 );
    StrTrim( pCmd );

    DWORD dwCmdLen = strlen( pCmd );
    assert ( dwCmdPos >= dwCmdLen );

    if ( dwCmdPos > dwCmdLen )
    {
        if ( ' ' == szRawCmd[dwCmdPos-dwCmdLen-1] )
        {
            bInQuote = FALSE;
        }
        else if ( '"' == szRawCmd[dwCmdPos-dwCmdLen-1] )
        {
            bInQuote = TRUE;
        }
        else
        {
            // assert( 0 );
        }
    }

    PArray cmds = InitArray( pTelnet->dwCommandsCnt );
    DWORD i;
    for ( i = 0; i < pTelnet->dwCommandsCnt; i++ )
    {
        Append2Array( cmds, (void*)pTelnet->ptCommands[i].szCmdName );
    }

    tmp = (char *)g_pfnAllocMem( (dwSize + 1) * sizeof(char) );
    char * pRet = CompleteCommand( tmp, dwSize + 1, pCmd, cmds );
    DeinitArray( cmds );

    if ( 0 == pRet )
    {
        g_pfnFreeMem(tmp);
        ClearStringVector(tokens);
        DeinitArray( tokens );
        return 0;
    }

    if ( strlen(pRet) == dwCmdLen )
    {
        g_pfnFreeMem(tmp);
        ClearStringVector(tokens);
        DeinitArray( tokens );
        return 0;
    }

    DWORD dwLen = 0;
    memcpy( szAlikeCmd, szRawCmd, dwCmdPos );
    dwLen = dwCmdPos;

    strcpy( szAlikeCmd + dwLen, tmp + dwCmdLen );
    dwLen += strlen( tmp + dwCmdLen );

    if ( bInQuote )
    {
        memcpy( szAlikeCmd + dwLen, "\"", 1);
        dwLen++;
    }

    strcpy( szAlikeCmd + dwLen, szRawCmd + dwCmdPos );
    dwLen += strlen( szRawCmd + dwCmdPos );

    g_pfnFreeMem(tmp);
    ClearStringVector(tokens);
    DeinitArray( tokens );
    return szAlikeCmd;
}


static void  OnHelp( PTSocketParam_  pSocketParam, PTTelnetd_    pTelnet )
{
    char buf[NORMAL_BUF_SIZE];

    DWORD i;
    int   len = 0;

    for ( i = 0; i < pTelnet->dwCommandsCnt; i++ )
    {
        SNPRINTF( buf, NORMAL_BUF_SIZE-1, "%s\t\t%s\r\n", pTelnet->ptCommands[i].szCmdName, pTelnet->ptCommands[i].szHelp );
        buf[NORMAL_BUF_SIZE-1] = '\0';

        len = strlen( buf );
        lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, buf, &len, 0 );
    }
}



static  BOOL  OnClientMessage( LmnThrdHandle h, const LmxMsgData * pMsgData, void * pContext )
{
    PTSocketParam_  pSocketParam = (PTSocketParam_)pContext;
    PTTelnetd_      pTelnet      = pSocketParam->pTelnetParam;

    switch ( pMsgData->dwMsgId )
    {
    case CLIENT_MSG_ID_INIT:
        {
            char buff[NORMAL_BUF_SIZE];
            WriteByte2Buff( (BYTE *)buff, NORMAL_BUF_SIZE, 
                            18, IAC, DO,  0x01,  IAC, DO,   0x03, IAC, DO, 0x21, IAC, DO, 0x22,
                            IAC, WILL, 0x01, IAC, WILL, 0x03 );

            int nRet = 0;
            int nLen = 18;
            nRet = lixTcpCompleteSend( pTelnet->pSeliNode,  pSocketParam->fd,  buff, &nLen, 0 );

            nLen = strlen(TELNET_HEAD);
            nRet = lixTcpCompleteSend( pTelnet->pSeliNode,  pSocketParam->fd,  TELNET_HEAD, &nLen, 0 );

            nRet = SendPrompt( pTelnet->pSeliNode,  pSocketParam->fd );
        }
        break;

    case CLIENT_MSG_ID_DATA:
        {
            char  tmp[NORMAL_BUF_SIZE];
            char  buff[NORMAL_BUF_SIZE];
            DWORD dwDataLen = 0;

            dwDataLen = FilterIac( (BYTE *)tmp, NORMAL_BUF_SIZE, (BYTE *)pMsgData->pMsg, pMsgData->dwMsgLen );

            if ( 0 == dwDataLen )
            {
                break;
            }

            DWORD & dwCmdPos  = pSocketParam->dwCommandPos;
            DWORD & dwCmdLen  = pSocketParam->dwCommandLen;
            char *  pCmd      = pSocketParam->szCommand;
            History * history = pSocketParam->history;

            if ( 2 < dwDataLen 
                && 0 == memcmp( "\r", tmp + dwDataLen - 2, 1 ) )
            {
                //do nothing
            }
            //删除操作
            else if ( 
                ( DELETE_SEND1_LEN <= dwDataLen 
                && 0 == memcmp( DELETE_SEND1, tmp, DELETE_SEND1_LEN ) )
                ||
                ( DELETE_SEND2_LEN <= dwDataLen 
                && 0 == memcmp( DELETE_SEND2, tmp, DELETE_SEND2_LEN ) )
                ||
                ( BACKSPACE_SEND1_LEN <= dwDataLen 
                && 0 == memcmp( BACKSPACE_SEND1, tmp, BACKSPACE_SEND1_LEN ) )
                ||
                ( BACKSPACE_SEND2_LEN <= dwDataLen 
                && 0 == memcmp( BACKSPACE_SEND2, tmp, BACKSPACE_SEND2_LEN ) )
                )
            {
                if ( dwCmdPos > 0 )
                {
                    DWORD dwReservedLen = dwCmdLen - dwCmdPos;

                    buff[0] = '\b';

                    DWORD dwLen = 1;
                    memcpy( buff + dwLen, pCmd + dwCmdPos, dwReservedLen );

                    memcpy( pCmd + dwCmdPos - 1, buff + dwLen, dwReservedLen );
                    dwCmdLen--;
                    dwCmdPos--;

                    dwLen += dwReservedLen;

                    memcpy( buff + dwLen, "\033[K", 3 );
                    dwLen += 3;

                    DWORD i;
                    for ( i = 0; i < dwReservedLen; i++ )
                    {
                        buff[dwLen + i] = '\b';
                    }

                    dwLen += dwReservedLen;

                    lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, buff, (int *)&dwLen, 0 );
                }
            }
            else if ( 3 <= dwDataLen 
                && 0 == memcmp( UP_SEND, tmp, 3 ) )
            {
                const char * pPrevCmd = history->GetPrevCmd();
                if ( 0 != pPrevCmd )
                {
                    DWORD dwPrevLen = strlen( pPrevCmd );
                    DWORD dwCurLen  = dwCmdPos;
                    pCmd[dwCmdLen] = '\0';

                    DWORD i;
                    for ( i = 0; i < dwCurLen; i++ )
                    {
                        buff[i] = '\b';
                    }

                    memcpy( buff + dwCurLen, pPrevCmd, dwPrevLen );
                    DWORD dwLen = dwCurLen + dwPrevLen;

                    if ( dwPrevLen < dwCmdLen )
                    {
                        memcpy( buff + dwLen, CLEAR_SEND, strlen(CLEAR_SEND) );
                        dwLen += strlen( CLEAR_SEND );
                    }

                    memcpy( pCmd, pPrevCmd, dwPrevLen );
                    dwCmdLen = dwPrevLen;
                    dwCmdPos    = dwCmdLen;

                    lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, buff, (int *)&dwLen, 0 );
                }
            }
            else if ( 3 <= dwDataLen 
                && 0 == memcmp( DOWN_SEND, tmp, 3 ) )
            {
                const char * pNextCmd = history->GetNextCmd();
                if ( 0 != pNextCmd )
                {
                    DWORD dwNextLen = strlen( pNextCmd );
                    DWORD dwCurLen  = dwCmdPos;
                    pCmd[dwCmdLen] = '\0';

                    DWORD i;
                    for ( i = 0; i < dwCurLen; i++ )
                    {
                        buff[i] = '\b';
                    }

                    memcpy( buff + dwCurLen, pNextCmd, dwNextLen );
                    DWORD dwLen = dwCurLen + dwNextLen;

                    if ( dwNextLen < dwCmdLen )
                    {
                        memcpy( buff + dwLen, CLEAR_SEND, strlen(CLEAR_SEND) );
                        dwLen += strlen( CLEAR_SEND );
                    }

                    memcpy( pCmd, pNextCmd, dwNextLen );
                    dwCmdLen = dwNextLen;
                    dwCmdPos = dwCmdLen;

                    lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, buff, (int *)&dwLen, 0 );
                }
            }
            else if ( 3 <= dwDataLen 
                && 0 == memcmp( LEFT_SEND, tmp, 3 ) )
            {
                if ( dwCmdPos > 0 )
                {
                    dwCmdPos--;

                    SendLeftCmd( pTelnet->pSeliNode, pSocketParam->fd );
                }
            }
            else if ( 3 <= dwDataLen 
                && 0 == memcmp( RIGHT_SEND, tmp, 3 ) )
            {
                if ( dwCmdPos < dwCmdLen )
                {
                    dwCmdPos++;

                    SendRightCmd( pTelnet->pSeliNode, pSocketParam->fd );
                }
            }
            else if ( 1 <= dwDataLen 
                && 0 == memcmp( "\t", tmp, 1 ) )
            {
                char szAlikeCmd[NORMAL_BUF_SIZE];
                pCmd[dwCmdLen] = '\0';

                char * pRet = FindAlikeCmd( szAlikeCmd, NORMAL_BUF_SIZE, pCmd, dwCmdPos, pTelnet );
                if ( 0 != pRet )
                {
                    DWORD dwLatterLen = strlen( pCmd + dwCmdPos );

                    strcpy( buff, szAlikeCmd + dwCmdPos );
                    DWORD dwLen = strlen( szAlikeCmd + dwCmdPos );

                    DWORD i;
                    for ( i = 0; i < dwLatterLen; i++ )
                    {
                        buff[dwLen + i] = '\b';
                    }

                    dwLen += dwLatterLen;

                    dwCmdPos += strlen( szAlikeCmd ) - strlen( pCmd );
                    strcpy( pCmd, szAlikeCmd );
                    dwCmdLen = strlen( pCmd );

                    lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, buff, (int *)&dwLen, 0 );
                }
            }
            //命令结束
            else if ( 2 == dwDataLen
                && 0 == memcmp( "\r", tmp, 1) )
            {   
                pCmd[dwCmdLen] = '\0';

                strcpy( buff, pCmd );
                StrTrim( buff );

                if ( '\0' != buff[0] )
                {
                    history->Pushback( pCmd );
                }
                history->ResetCurIndex();

                int tmpLen = 2;
                lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, "\r\n", &tmpLen, 0 );


                PArray  tokens = InitArray( 10 );
                ParseRawCommand( pCmd, tokens );
                
                DWORD dwTokenSize = GetArraySize(tokens);
                if ( dwTokenSize > 0 )
                {
                    const char * pArg0  = (const char *)GetFromArray( tokens, 0 );
                    const char * szHelp = 0;

                    DWORD i;
                    for ( i = 0; i < pTelnet->dwCommandsCnt; i++ )
                    {
                        if ( 0 == strcmp( pArg0, pTelnet->ptCommands[i].szCmdName ) )
                        {
                            szHelp = pTelnet->ptCommands[i].szHelp;
                            break;
                        }
                    }

                    // 没有该命令
                    if ( i >= pTelnet->dwCommandsCnt )
                    {
#ifdef WIN32
                        _snprintf( buff, NORMAL_BUF_SIZE-1, COMMAND_NOT_FOUND_MSG, pArg0 );
#else
                        snprintf ( buff, NORMAL_BUF_SIZE-1, COMMAND_NOT_FOUND_MSG, pArg0 );
#endif
                        buff[NORMAL_BUF_SIZE-1] = '\0' ;
                        tmpLen = strlen( buff );
                        lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, buff, &tmpLen, 0 );
                    }
                    else
                    {
                        BOOL  bShowHelpInformation = FALSE;

                        if ( dwTokenSize > 1 )
                        {
                            const char * pArg1 = (const char *)GetFromArray( tokens, 1 );

                            // 显示描述
                            if ( ( 0 == strcmp(pArg1, "--help") ) || ( 0 == strcmp( pArg1, "/?" ) ) )
                            {
                                bShowHelpInformation = TRUE;
                            }
                        }


                        // 如果是显示命令描述
                        if ( bShowHelpInformation )
                        {
                            assert( szHelp );

                            tmpLen = strlen(szHelp);
                            lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, szHelp, &tmpLen, 0 );

                            tmpLen = 2;
                            lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, "\r\n", &tmpLen, 0 );
                        }
                        // 如果是执行命令
                        else
                        {
                            // 如果是help系统命令
                            if ( 0 == strcmp( pArg0, s_SystemCommand[0].szCmdName ) )
                            {
                                OnHelp( pSocketParam, pTelnet );
                            }
                            // 如果是bye系统命令
                            else if ( 0 == strcmp( pArg0, s_SystemCommand[1].szCmdName ) )
                            {
                                LmnLock( &pTelnet->lock_QueryCloseClients );
                                Insert2ListTail( pTelnet->lstQueryCloseClients, (void *)pSocketParam->fd );
                                LmnUnlock( &pTelnet->lock_QueryCloseClients );
                            }
                            // 用户自定义命令
                            else
                            {
                                const char ** args = (const char **)g_pfnAllocMem( sizeof(char*) * dwTokenSize );
                                for ( i = 0; i < dwTokenSize; i++ )
                                {
                                    args[i] = (char *)GetFromArray(tokens, i);
                                }

                                if ( pTelnet->pfnOnCommand )
                                {
                                    pTelnet->pfnOnCommand( args[0], args, dwTokenSize, (DWORD)pSocketParam->fd );
                                }
                                g_pfnFreeMem( args );
                            }
                        }
                    }
                }

                ClearStringVector( tokens );
                DeinitArray( tokens );


                dwCmdLen = 0;
                dwCmdPos = 0;

                SendPrompt( pTelnet->pSeliNode, pSocketParam->fd );
            }
            else
            {
                assert( dwCmdPos <= dwCmdLen );

                if ( dwCmdPos == dwCmdLen )
                {
                    memcpy( pCmd + dwCmdPos, tmp, dwDataLen );
                    dwCmdPos += dwDataLen;
                    dwCmdLen += dwDataLen;

                    lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, tmp, (int*)&dwDataLen, 0 );
                }
                else
                {
                    DWORD dwReservedLen = dwCmdLen - dwCmdPos;

                    memcpy( buff, pCmd + dwCmdPos, dwReservedLen );
                    memcpy( pCmd + dwCmdPos, tmp, dwDataLen );
                    memcpy( pCmd + dwCmdPos + dwDataLen, buff, dwReservedLen );

                    dwCmdLen += dwDataLen;

                    DWORD dwLen = dwCmdLen - dwCmdPos;
                    memcpy( buff, pCmd + dwCmdPos, dwCmdLen );

                    DWORD i;
                    for ( i = 0; i < dwReservedLen; i++ )
                    {
                        memcpy( buff + dwLen + strlen(CURSOR_LEFT) * i, 
                            CURSOR_LEFT, strlen(CURSOR_LEFT) );
                    }

                    dwCmdPos += dwDataLen;

                    int tmpLen = dwLen + strlen(CURSOR_LEFT) * dwReservedLen;
                    lixTcpCompleteSend( pTelnet->pSeliNode, pSocketParam->fd, buff, &tmpLen, 0 );
                }
            }
        }
        break;

    case CLIENT_MSG_ID_DESTROY_CLIENT_THRD:
        {
            assert( sizeof(PTSocketParam_) == pMsgData->dwMsgLen );

            PTSocketParam_  pClientParam = 0;
            memcpy( &pClientParam, pMsgData->pMsg, sizeof(pClientParam) );

            LmxDestroyThread( pClientParam->hClientThrd );
            if ( pClientParam->history )
            {
                pClientParam->history->~History();
                g_pfnFreeMem( pClientParam->history );
            }
            g_pfnFreeMem( pClientParam );
        }
        break;

    default:
        break;
    }
    return TRUE;
}


// listen fd's context is PTTelnetd_
static void OnSocketEvent( SelixNode pSeli, FdType fd, int liEvent, int error, void * context )
{
    PTSocketParam_  pSocketParam = (PTSocketParam_)context;
    PTTelnetd_      pTelnet      = pSocketParam->pTelnetParam;

    switch( liEvent )
    {
    case liEvAccept:
        {
            // fprintf( stderr, "accetp! \n" );

            PTSocketParam_   pClientParam = (PTSocketParam_)g_pfnAllocMem( sizeof(TSocketParam_) );
            memset( pClientParam, 0, sizeof(TSocketParam_) );
            pClientParam->pTelnetParam = pTelnet;
            pClientParam->nSocketType  = SOCKET_TYPE_CLIENT;
            pClientParam->fd           = -1;
            pClientParam->history      = (History *)g_pfnAllocMem( sizeof(History) );
            new(pClientParam->history) History();

            FdType fd_client = lixAccept( pSeli, fd, OnSocketEvent, pClientParam );
            if ( -1 != fd_client )
            {
                

                pClientParam->fd = fd_client;
                pClientParam->hClientThrd  = LmxCreateThread( OnClientMessage, TRUE, pClientParam );

                LmxMsgData  tMsg;
                memset( &tMsg, 0, sizeof(LmxMsgData) );
                tMsg.dwMsgId  = CLIENT_MSG_ID_INIT;
                LmxPostMessage( pClientParam->hClientThrd, &tMsg );
            }
            else
            {
                pClientParam->history->~History();
                g_pfnFreeMem( pClientParam->history );
                g_pfnFreeMem( pClientParam );
            }
        }
        break;

    case liEvRead:
        {
            assert ( fd >=1 && fd <= MAX_TELNET_CO_USERS_COUNT );
            assert ( pSocketParam->hClientThrd );

            char buf[NORMAL_BUF_SIZE];
            int  len = NORMAL_BUF_SIZE;
            int  ret = 0;

            ret = lixTcpRecv( pSeli, fd, buf, &len );
            while ( 0 == ret && len > 0 )
            {
                LmxMsgData  tMsg;
                memset( &tMsg, 0, sizeof(LmxMsgData) );

                tMsg.dwMsgId  = CLIENT_MSG_ID_DATA;
                tMsg.pMsg     = (void *)buf;
                tMsg.dwMsgLen = len;

                LmxPostMessage( pSocketParam->hClientThrd, &tMsg );

                len = NORMAL_BUF_SIZE;
                ret = lixTcpRecv( pSeli, fd, buf, &len );
            }
        }
        break;

    case liEvClose:
        {
            // fprintf( stderr, "close! \n" );

            lixClose( pSeli, fd );

            // fprintf( stderr, "close end! \n" );
        }
        break;

    case liEvDestroy:
        {
            if ( pSocketParam->hClientThrd )
            {
                // Insert2ListTail( pTelnet->lstQueryDestroyThrdx, (void *)(pSocketParam) );

                LmxMsgData  tMsg;
                memset( &tMsg, 0, sizeof(LmxMsgData) );

                tMsg.dwMsgId  = CLIENT_MSG_ID_DESTROY_CLIENT_THRD;
                tMsg.pMsg     = (void *)&pSocketParam;
                tMsg.dwMsgLen = sizeof(pSocketParam);

                LmxPostMessage( pTelnet->hListenThrd, &tMsg );
            }
            else
            {
                if ( pSocketParam->history )
                {
                    pSocketParam->history->~History();
                    g_pfnFreeMem( pSocketParam->history );
                }
                g_pfnFreeMem( pSocketParam );
            }

            ClearPrintfListByClientId( pTelnet->lstQueryPrintf, (DWORD)fd );
        }
        break;

    default:
        break;
    }
}

static void OnQueryPrintf( PTPrintfParam_  pParam, PTTelnetd_   pTelnetdParam )
{
    int  nRealLen = strlen( pParam->pMsg );
    int  ret = 0;

    if ( 0 == pParam->dwClientId )
    {
        DWORD i;
        for ( i = 0; i < MAX_TELNET_CO_USERS_COUNT; i++ )
        {
            int tmpSendLen  = nRealLen;
            lixTcpCompleteSend( pTelnetdParam->pSeliNode, (int)(i+1), pParam->pMsg, &tmpSendLen, 0 );
        }
    }
    else
    {
        int tmpSendLen  = nRealLen;
        ret = lixTcpCompleteSend( s_pTelnet->pSeliNode, (int)pParam->dwClientId, pParam->pMsg, &tmpSendLen, 0 );
        if ( 0 != ret )
        {
            return;
        }
    }
}


static void * TelnetdThrd( void * pArg )
{
    PTSocketParam_  pSocketParam  = (PTSocketParam_)pArg;
    PTTelnetd_      pTelnetdParam = pSocketParam->pTelnetParam;

    while ( pTelnetdParam->bTelnetdLoopFlag )
    {
        DWORD  dwTimeOut = 20;

        selixSelect( pTelnetdParam->pSeliNode, &dwTimeOut );
        if ( dwTimeOut > 0 )
        {
            LmnSleep( dwTimeOut );
        }


        LmnLock( &pTelnetdParam->lock_QueryCloseClients );
        PListNode pNode = GetListHead( pTelnetdParam->lstQueryCloseClients );
        while( pNode )
        {
            // 关闭客户端socket
            FdType fd = (FdType)pNode->pData;
            lixClose( pTelnetdParam->pSeliNode, fd );
            pNode = GetNextListNode( pNode );
        }
        ClearList( pTelnetdParam->lstQueryCloseClients );
        LmnUnlock( &pTelnetdParam->lock_QueryCloseClients );


        pNode = GetListHead( pTelnetdParam->lstQueryPrintf );
        while( pNode )
        {
            PTPrintfParam_  pParam = (PTPrintfParam_)pNode->pData;
            // printf消息
            OnQueryPrintf( pParam, pTelnetdParam );
            //
            pNode = GetNextListNode( pNode );
        }
        ClearPrintfList( pTelnetdParam->lstQueryPrintf );


        
        //pNode = GetListHead( pTelnetdParam->lstQueryDestroyThrdx );
        //while( pNode )
        //{
        //    PTSocketParam_  pSocketParam = (PTSocketParam_)pNode->pData;

        //    LmxDestroyThread( pSocketParam->hClientThrd );
        //    if ( pSocketParam->history )
        //    {
        //        pSocketParam->history->~History();
        //        g_pfnFreeMem( pSocketParam->history );
        //    }
        //    g_pfnFreeMem( pSocketParam );

        //    pNode = GetNextListNode( pNode );
        //}
        //ClearList( pTelnetdParam->lstQueryDestroyThrdx );

        // ClearSocketParamList( pTelnetdParam->lstQueryDestroyThrdx );


    }

	return 0;
}



static  void   InitSystemCommands()
{
    strncpy( s_SystemCommand[0].szCmdName, "help",  MAX_TELNETD_COMMAND_NAME_LEN );
    strncpy( s_SystemCommand[0].szHelp,    "show all commands and help informatino", MAX_TELNETD_COMMAND_NAME_LEN );

    strncpy( s_SystemCommand[1].szCmdName, "bye",  MAX_TELNETD_COMMAND_NAME_LEN );
    strncpy( s_SystemCommand[1].szHelp,    "quit telnet server", MAX_TELNETD_COMMAND_NAME_LEN );
}




COMMONLMN_API  int  LmnStartTelnetd( WORD wPort, const TRegTelnetdCmd * atCmds, DWORD dwCmdCnt, 
									 LPOnTelnetCmd pfnOnCommand, DWORD dwMaxCoUsersCnt )
{
	if ( 0 == wPort || 0 == pfnOnCommand )
	{
		return LMN_ERR_INVALID_ARGUMENT;
	}

	if ( s_bTelnetdInited )
	{
		return LMN_ERR_ALREADY_STARTED;
	}

	// 最多可同时telnet MAX_TELNET_CO_USERS_COUNT个用户
	if ( 0 == dwMaxCoUsersCnt || dwMaxCoUsersCnt > MAX_TELNET_CO_USERS_COUNT )
	{
		dwMaxCoUsersCnt = MAX_TELNET_CO_USERS_COUNT;
	}

	int ret = 0;

	if ( !isSelixInited() )
	{
		ret = selixInit( MAX_SELIX_INIT_SOCKETS_COUNT );
		if ( 0 != ret )
		{
			return ret;
		}
	}
	
	DWORD  dwTmp = dwMaxCoUsersCnt + 1;

	SelixNode  pSeliNode = selixCreateNode( &dwTmp );
	if ( 0 == pSeliNode )
	{
		return LMN_ERR_UNKNOWN;
	}

	// 如果分配的socket个数不够
	if ( dwTmp < dwMaxCoUsersCnt + 1 )
	{
		selixDestroyNode( pSeliNode );
		return LMN_ERR_NO_RESOURCE;
	}

	PTTelnetd_  pTelnetdParam = (PTTelnetd_)g_pfnAllocMem( sizeof(TTelnetd_) );
	if ( 0 == pTelnetdParam )
	{
		selixDestroyNode( pSeliNode );
		return LMN_ERR_NO_MEMORY;
	}
    memset( pTelnetdParam, 0, sizeof(TTelnetd_) );


	pTelnetdParam->dwCommandsCnt = 0;
	pTelnetdParam->ptCommands    = 0;

    // 增加2个命令 help, bye
    InitSystemCommands();
    dwCmdCnt += SYSTEM_COMMANDS_COUNT;

	if ( dwCmdCnt > 0 )
	{
		pTelnetdParam->ptCommands = (TRegTelnetdCmd *)g_pfnAllocMem( sizeof(TRegTelnetdCmd) * dwCmdCnt );
		if ( 0 == pTelnetdParam->ptCommands )
		{
			selixDestroyNode( pSeliNode );
			g_pfnFreeMem( pTelnetdParam );
			return LMN_ERR_NO_MEMORY;
		}
        memset( pTelnetdParam->ptCommands, 0, sizeof(TRegTelnetdCmd) * dwCmdCnt );

        // 排序
        int  i, j, k;
        for ( i = 0; i < (int)dwCmdCnt; i++ )
        {
            const TRegTelnetdCmd * pSrcCmd = 0;
            if ( i < SYSTEM_COMMANDS_COUNT )
            {
                pSrcCmd = s_SystemCommand + i;
            }
            else
            {
                pSrcCmd = atCmds + (i - SYSTEM_COMMANDS_COUNT);
            }

            for ( j = 0; j < (int)pTelnetdParam->dwCommandsCnt; j++ )
            {
                TRegTelnetdCmd * pDestCmd = pTelnetdParam->ptCommands + j;
                int  nCmpRet = strcmp( pSrcCmd->szCmdName, pDestCmd->szCmdName );

                // 相同的command
                if ( 0 == nCmpRet )
                {
                    break;
                }
                // 小于pDestCmd
                else if ( nCmpRet < 0 )
                {
                    for ( k = pTelnetdParam->dwCommandsCnt - 1; k >= j; k--  )
                    {
                        memcpy( &pTelnetdParam->ptCommands[k+1], &pTelnetdParam->ptCommands[k], sizeof(TRegTelnetdCmd) );
                    }
                    memcpy( &pTelnetdParam->ptCommands[j], pSrcCmd, sizeof(TRegTelnetdCmd) );
                    pTelnetdParam->dwCommandsCnt++;
                    break;
                }
            }

            if ( j == (int)pTelnetdParam->dwCommandsCnt )
            {
                memcpy( &pTelnetdParam->ptCommands[j], pSrcCmd, sizeof(TRegTelnetdCmd) );
                pTelnetdParam->dwCommandsCnt++;
            }
        }
	}
	
	pTelnetdParam->dwMaxCoUsersCnt = dwMaxCoUsersCnt;
	pTelnetdParam->pfnOnCommand    = pfnOnCommand;
	pTelnetdParam->wPort           = wPort;


    TSocketParam_ *  pSocketParam = (TSocketParam_ *)g_pfnAllocMem( sizeof(TSocketParam_) );
    if ( 0 == pSocketParam )
    {
        if ( pTelnetdParam->ptCommands )
        {
            g_pfnFreeMem( pTelnetdParam->ptCommands );
        }
        g_pfnFreeMem( pTelnetdParam );
        selixDestroyNode( pSeliNode );
        return LMN_ERR_NO_MEMORY;
    }
    memset( pSocketParam, 0, sizeof(TSocketParam_) );

    pSocketParam->pTelnetParam = pTelnetdParam;
    pSocketParam->nSocketType  = SOCKET_TYPE_LISTEN;
    pSocketParam->hClientThrd  = 0;

    LmnThrdHandle hThrd = LmxCreateThread( OnClientMessage, TRUE, pSocketParam );
    pTelnetdParam->hListenThrd = hThrd;


    FdType fd = lixOpen( pSeliNode, LI_PROTOTYPE_TCP, 0, wPort, OnSocketEvent, (void *)pSocketParam, 0  );
    if ( -1 == fd )
    {
        g_pfnFreeMem( pSocketParam );
        if ( pTelnetdParam->ptCommands )
        {
            g_pfnFreeMem( pTelnetdParam->ptCommands );
        }
        g_pfnFreeMem( pTelnetdParam );
        selixDestroyNode( pSeliNode );
        return LMN_ERR_UNKNOWN;
    }
    pSocketParam->fd = fd;

    ret = lixListen( pSeliNode, fd, 10 );
    if ( 0 != ret )
    {
        g_pfnFreeMem( pSocketParam );
        if ( pTelnetdParam->ptCommands )
        {
            g_pfnFreeMem( pTelnetdParam->ptCommands );
        }
        g_pfnFreeMem( pTelnetdParam );
        selixDestroyNode( pSeliNode );
        return ret;
    }
    pTelnetdParam->pSeliNode = pSeliNode;

    // 开启线程的循环flag
    pTelnetdParam->bTelnetdLoopFlag = TRUE;

	pTelnetdParam->TelnetdThrd = LmnCreateThread( TelnetdThrd, (void *)pSocketParam, 0 );
	if ( 0 == pTelnetdParam->TelnetdThrd )
	{
        g_pfnFreeMem( pSocketParam );
		if ( pTelnetdParam->ptCommands )
		{
			g_pfnFreeMem( pTelnetdParam->ptCommands );
		}
		g_pfnFreeMem( pTelnetdParam );
		selixDestroyNode( pSeliNode );
		return LMN_ERR_UNKNOWN;
	}

    s_pTelnet        = pTelnetdParam;

    LmnInitLock( &pTelnetdParam->lock_QueryCloseClients );
    pTelnetdParam->lstQueryCloseClients = InitList();

    LmnInitLock( &s_lock_telnet );
    pTelnetdParam->lstQueryPrintf = InitList();

	s_bTelnetdInited = TRUE;

	return LMN_ERR_OK;
}



COMMONLMN_API  int  LmnStopTelnetd( )
{
    if ( !s_bTelnetdInited )
    {
        return LMN_ERR_NOT_STARTED;
    }

    LmnLock( &s_lock_telnet );

    s_pTelnet->bTelnetdLoopFlag = FALSE;
    LmnWait4Thrd( s_pTelnet->TelnetdThrd );
    s_pTelnet->TelnetdThrd = 0;

    selixDestroyNode( s_pTelnet->pSeliNode );
    if ( s_pTelnet->ptCommands )
    {
        g_pfnFreeMem( s_pTelnet->ptCommands );
    }

    LmxDestroyThread( s_pTelnet->hListenThrd );
    s_pTelnet->hListenThrd = 0;
    

    assert( s_pTelnet->lstQueryCloseClients );
    DeinitList( s_pTelnet->lstQueryCloseClients );
    s_pTelnet->lstQueryCloseClients = 0;
    LmnDeinitLock( &s_pTelnet->lock_QueryCloseClients );


    ClearPrintfList( s_pTelnet->lstQueryPrintf );
    DeinitList(s_pTelnet->lstQueryPrintf);
    s_pTelnet->lstQueryPrintf = 0;


    g_pfnFreeMem( s_pTelnet );
    s_pTelnet = 0;


    s_bTelnetdInited = FALSE;

    LmnUnlock( &s_lock_telnet );
    LmnDeinitLock( &s_lock_telnet );

    return LMN_ERR_OK;
}

COMMONLMN_API  int  LmnTelPrintf( DWORD dwClientId, const char * szFormat, ... )
{
    if ( !s_bTelnetdInited )
    {
        return LMN_ERR_NOT_STARTED;
    }

    if ( !s_pTelnet->bTelnetdLoopFlag )
    {
        return LMN_ERR_UNKNOWN;
    }

    assert( s_pTelnet );

    char buf0[NORMAL_BUF_SIZE];
    char buf[NORMAL_BUF_SIZE];

    va_list  ap;
    va_start( ap, szFormat );
    vsnprintf( buf0, NORMAL_BUF_SIZE - 1, szFormat, ap );
    va_end( ap );

    buf0[NORMAL_BUF_SIZE-1] = '\0';

    StrReplace( buf, NORMAL_BUF_SIZE, buf0, "\n", "\r\n" );

    int len = strlen(buf);
    int ret = 0;
    
    if ( 0 == dwClientId )
    {
        DWORD i;
        for ( i = 0; i < MAX_TELNET_CO_USERS_COUNT; i++ )
        {
            int tmpSendLen  = len;
            lixTcpCompleteSend( s_pTelnet->pSeliNode, (int)(i+1), buf, &tmpSendLen, 0 );
        }
    }
    else
    {
        int tmpSendLen  = len;
        ret = lixTcpCompleteSend( s_pTelnet->pSeliNode, (int)dwClientId, buf, &tmpSendLen, 0 );
        if ( 0 != ret )
        {
            return ret;
        }
    }

    return LMN_ERR_OK;
}



COMMONLMN_API  int  LmnTelPrintf0( DWORD dwClientId, const char * szMsg )
{
    if ( !s_bTelnetdInited )
    {
        return LMN_ERR_NOT_STARTED;
    }

    if ( 0 == szMsg )
    {
        return LMN_ERR_INVALID_ARGUMENT;
    }

    if ( 0 == s_pTelnet || !s_pTelnet->bTelnetdLoopFlag )
    {
        return LMN_ERR_UNKNOWN;
    }

    LmnCommon::CFuncLock  lock( &s_lock_telnet );


    PTPrintfParam_  pPrintfParam = (PTPrintfParam_)g_pfnAllocMem( sizeof(TPrintfParam_) );
    int             nMsgLen      = strlen(szMsg);
    int             nAllocLen    = nMsgLen * 2 + 1;
    pPrintfParam->pMsg           = (char *)g_pfnAllocMem( nAllocLen * sizeof(char) );
    StrReplace( pPrintfParam->pMsg, nAllocLen, szMsg, "\n", "\r\n" );
    pPrintfParam->dwClientId     = dwClientId;

    Insert2ListTail( s_pTelnet->lstQueryPrintf, (void *)pPrintfParam );

    return LMN_ERR_OK;
}









