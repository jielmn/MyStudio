// TelSvrLib.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include "MyMisc.h"
#include "TelSvrLib.h"


#define DEBUG_SWITCH     0


#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#endif


#ifndef WIN32

typedef int                             SOCKET;
typedef pthread_t                       HANDLE;
typedef unsigned char                   BYTE;
typedef void * (*LPTHREAD_START_ROUTINE)(void *);
typedef void *                          LPVOID;
typedef int                             BOOL;

#define INVALID_SOCKET                  (-1)
#define SOCKET_ERROR                    (-1)
#define WINAPI
#define TRUE                             1
#define FALSE                            0

#endif


#ifdef WIN32
#define THREAD_ERROR                    -1
#else
#define THREAD_ERROR                    (void *)-1
#endif

static HANDLE s_hStartThrd = 0;
static HANDLE s_hClientThrd = 0;
static SOCKET s_tSvrSocket = INVALID_SOCKET;
static SOCKET s_tClientSock = INVALID_SOCKET;
static int    s_nPrintFlag = 0;
static JLockType s_lock;

#define IAC      ((BYTE)0xFF)
#define WILL     ((BYTE)0xFB)
#define WONT     ((BYTE)0xFC)
#define DO       ((BYTE)0xFD)
#define DONT     ((BYTE)0xFE)
#define SB       ((BYTE)0xFA)
#define SE       ((BYTE)0xF0)

#define BUFF_SIZE        8182

#define TELNET_HEAD   "Jesse telnet server\r\n"


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

static const unsigned int  COLUMNS_NUMBER = 80;

static int _IsBlankChar( char ch )
{
	if ( ' ' == ch || '\t' == ch || '\r' == ch || '\n' == ch )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static char * strtrim( char * str )
{
	if ( NULL == str )
	{
		return NULL;
	}

	int nLen = strlen( str );

	int i, j;
	for ( i = 0; i < nLen; i++ )
	{
		if ( !_IsBlankChar(str[i]) )
		{
			break;
		}
	}

	for ( j = nLen - 1; j >= i; j-- )
	{
		if ( !_IsBlankChar(str[j]) )
		{
			break;
		}
	}

	int nNewLen = j - i + 1;
	if ( nNewLen == nLen )
	{
		return str;
	}

	if ( 0 == nNewLen )
	{
		str[0] = '\0';
	}
	else
	{
		char * tmp = ( char * )malloc( sizeof(char) * (nNewLen + 1) );
		strncpy( tmp, str + i, nNewLen );
		tmp[nNewLen] = '\0';
		strcpy( str, tmp );
		free( tmp );
	}

	return str;
}

static char * str2lower( char * str )
{
    int n = strlen( str );
    int i = 0;
    for ( i = 0; i < n; i++ )
    {
        if ( str[i] >= 'A' && str[i] <= 'Z' )
        {
            str[i] = 'a' + str[i] - 'A';
        }
    }

    return str;
}

static int _Ch2Num( char ch, int bHexadecimal )
{
	if ( bHexadecimal )
	{
		if ( ch >= '0' && ch <= '9' )
		{
			return ch - '0';
		}
		else if ( ch >= 'a' && ch <= 'f' )
		{
			return ch - 'a' + 10;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		if ( ch >= '0' && ch <= '9' )
		{
			return ch - '0';
		}
		else
		{
			return -1;
		}
	}
}

/////////////////////////////////////////////////////////
// nDigit: 数字
// nPos  : 位置。如个位，十位，百位等。序号从0开始
// nMode : 进制( 10, 16 )
/////////////////////////////////////////////////////////
static unsigned int _ParseNum( unsigned int nDigit, unsigned int nPos, unsigned int nMode )
{
	unsigned int i = 0;
	for ( i = 0; i < nPos; i++ )
	{
		nDigit *= nMode;
	}
	return nDigit;
}

////////////////转换为数字/////////////////////////
// 解析32bit内的整数(注意：不负责超出范围)
// 解析格式 
// 格式1.12345678, -100
// 格式2.0xabFcd, -0x100
// return value: -2, invalid parameters
//               0, failed
//               non-zero, success
///////////////////////////////////////////////////
static int  Str2Num( const char * szNum, int * pNum )
{
	if ( NULL == szNum || NULL == pNum )
	{
		return -2;
	}

	int n = strlen( szNum );
	char * buff = (char *)malloc( sizeof(char) * (n + 1) );

	strcpy( buff, szNum );
	strtrim( buff );

	// to lower case
	str2lower( buff );

	size_t nLen = strlen( buff );

	if ( 0 == nLen )
	{
		free( buff );
		return 0;
	}

	char * p = NULL;
	int bPositive = 1;
	int bDigital  = 1;

	if ( '-' == buff[0] )
	{
		bPositive = 0;
		p = buff + 1;
		nLen--;
	}
	else
	{
		bPositive = 1;
		p = buff;
	}

	if ( 0 == nLen )
	{
		free( buff );
		return 0;
	}
	
	// maybe hexadecimal
	if ( nLen > 2 && '0' == p[0] && 'x' == p[1] )
	{
		bDigital = 0;
		p += 2;
		nLen -= 2;
	}
	else
	{
		bDigital = 1;
	}

	//check every character
	unsigned int i;
	int nCnt = 0;
	int nMode = bDigital ? 10 : 16;

	for ( i = 0; i < nLen; i++ )
	{
		int nNum = _Ch2Num( p[i], !bDigital );
		if ( -1 == nNum )
		{
			free( buff );
			return 0;
		}
		
		nCnt += _ParseNum( nNum, nLen - i - 1, nMode );
	}

	free( buff );

	if ( bPositive )
	{
		*pNum = nCnt;
		return 1;
	}
	else
	{
		*pNum = -nCnt;
		return 1;
	}
}

class History
{
public:
    History( unsigned int nMaxSize = 10 );
    ~History();
    
    void Pushback( const char * cmd );
    const char * operator []( unsigned int nIndex );
    void Clear();
    unsigned int GetSize();
    const char * GetPrevCmd();
    const char * GetNextCmd();
    void ResetCurIndex();

private:
    std::vector<char *> m_cmds;
    unsigned int        m_nMaxSize;
    unsigned int        m_nCurIndex;
};

History::History( unsigned int nMaxSize /* = 10 */ )
{
    if ( 0 == nMaxSize )
    {
        nMaxSize = 10;
    }

    m_nMaxSize  = nMaxSize;
    m_nCurIndex = 0;
}

History::~History()
{
    Clear();
}

void History::Clear()
{
    std::vector<char *>::iterator it;
    for ( it = m_cmds.begin(); it != m_cmds.end(); it++ )
    {
        delete[] *it;
    }
    m_cmds.clear();
}

void History::Pushback( const char * cmd )
{
    assert( 0 != cmd );

    unsigned int nSize = m_cmds.size();
    unsigned int nLen  = strlen( cmd );

    char * p = 0;
    p = new char[nLen + 1];
    strcpy( p, cmd );

    if ( 0 == nSize )
    {
        m_cmds.push_back( p );
    }
    else
    {
        assert( nSize <= m_nMaxSize );

        if ( nSize == m_nMaxSize )
        {
            delete[] m_cmds.at(0);
            m_cmds.erase( m_cmds.begin() );
        }

        m_cmds.push_back( p );
    }
}

const char * History::operator []( unsigned int nIndex )
{
    if ( nIndex >= m_cmds.size() )
    {
        return 0;
    }
    else
    {
        return m_cmds.at( nIndex );
    }
}

unsigned int History::GetSize()
{
    return m_cmds.size();
}

const char * History::GetPrevCmd()
{
    if ( m_nCurIndex > 0 )
    {
        m_nCurIndex--;

        return m_cmds.at( m_nCurIndex );
    }
    else
    {
        return 0;
    }
}

const char * History::GetNextCmd()
{
    if ( m_nCurIndex + 1 < m_cmds.size() )
    {
        m_nCurIndex++;

        return m_cmds.at( m_nCurIndex );
    }
    else
    {
        return 0;
    }
}

void History::ResetCurIndex()
{
    m_nCurIndex = m_cmds.size();
}

static History s_history;


class SingleCommand
{
public:
    SingleCommand( const char * szCmd, const char * szHelpMsg,
                   LPREG_CMD_FUNC pfunc, va_list ap );

    ~SingleCommand();

    void Clear();
    void Execute(int nCnt, void * args[]);
    void PrintHelp();

    void GetArgTypes( std::vector<int> & vTypes );

    const char * GetCommand();
    const char * GetHelp();

private:
    char *           m_szCommand;
    char *           m_szHelpMsg;
    LPREG_CMD_FUNC   m_pfunc;
    std::vector<int> m_ArgTypes;
};

SingleCommand::SingleCommand( const char * szCmd, const char * szHelpMsg,
                              LPREG_CMD_FUNC pfunc, va_list ap )
{
    unsigned int nLen = 0;

    assert( 0 != szCmd );
    nLen = strlen( szCmd );
    m_szCommand = new char[nLen + 1];
    strcpy( m_szCommand, szCmd );

    if ( 0 != szHelpMsg )
    {
        nLen = strlen( szHelpMsg );
        m_szHelpMsg = new char[nLen + 1];
        strcpy( m_szHelpMsg, szHelpMsg );
    }
    else
    {
        m_szHelpMsg = 0;
    }

    assert( 0 != pfunc );
    m_pfunc = pfunc;

    int t = va_arg( ap, int );
    while( 0 != t )
    {
        if ( JTEL_ARG_TYPE_INT != t && JTEL_ARG_TYPE_TEXT != t )
        {
            t = JTEL_ARG_TYPE_INT;
        }

        m_ArgTypes.push_back( t );
        t = va_arg( ap, int );
    }
}

SingleCommand::~SingleCommand()
{
    Clear();
}

void SingleCommand::Clear()
{
    delete[] m_szCommand;

    if ( 0 != m_szHelpMsg )
    {
        delete[] m_szHelpMsg;
    }
    
    m_ArgTypes.clear();
}

// args 类型确定
// 
void SingleCommand::Execute(int nCnt, void * args[])
{
    assert( m_pfunc );

    assert( nCnt >= (int)m_ArgTypes.size() );

    s_nPrintFlag = 1;
    m_pfunc( nCnt, args );
    s_nPrintFlag = 0;
}

void SingleCommand::PrintHelp()
{
    if ( 0 != m_szHelpMsg )
    {
        char buff[BUFF_SIZE];
        sprintf( buff, "%s\r\n", m_szHelpMsg );
        send( s_tClientSock, buff, strlen(buff), 0 );
    }
}

const char * SingleCommand::GetCommand()
{
    return m_szCommand;
}

const char * SingleCommand::GetHelp()
{
    return m_szHelpMsg;
}

void SingleCommand::GetArgTypes( std::vector<int> & vTypes )
{
    std::copy( m_ArgTypes.begin(), m_ArgTypes.end(), std::back_inserter(vTypes) );
}

class FindCmdObj
{
public:
    FindCmdObj( const char * szCmd );
    FindCmdObj( const FindCmdObj & obj );
    ~FindCmdObj();

    bool operator()( SingleCommand * cmd);

private:
    char * m_szCmd;
};

FindCmdObj::FindCmdObj( const char * szCmd )
{
    unsigned int nLen = strlen( szCmd );
    m_szCmd = new char[nLen + 1];
    strcpy( m_szCmd, szCmd );
}

FindCmdObj::FindCmdObj( const FindCmdObj & obj )
{
    unsigned int nLen = strlen( obj.m_szCmd );
    m_szCmd = new char[nLen + 1];
    strcpy( m_szCmd, obj.m_szCmd );
}

FindCmdObj::~FindCmdObj()
{
    delete[] m_szCmd;
}

bool FindCmdObj::operator()( SingleCommand * cmd)
{
    if ( 0 == strcmp( m_szCmd, cmd->GetCommand()) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

class Commands
{
public:
    Commands();
    ~Commands();
    void Clear();

    BOOL Add( SingleCommand * cmd );
    void Sort();
    void ExecuteRawCommand( const char * szRawCmd );
    void PrintAllHelpMsg();
    char * FindAlikeCmd( char * szAlikeCmd, unsigned int nSize, 
                         const char * szRawCmd, unsigned int nPos );

private:
    char * GetHelpItem( char * buff, unsigned int nSize,
                        const char * pCmd, const char * pHelp);
    SingleCommand * FindCommand( const char * szCommand );
    void ParseRawCommand( const char * szCommand, std::vector<char *> & vTokens );
    char * CompleteCommand( char * szCompleteCmd, unsigned int nSize, 
                            const char * szCmd, std::vector<SingleCommand *> & vCmds );

    std::vector<SingleCommand *> m_cmds;
};

Commands::Commands()
{

}

Commands::~Commands()
{
    Clear();
};

void Commands::Clear()
{
    m_cmds.clear();
}

SingleCommand * Commands::FindCommand( const char * szCommand )
{
    std::vector<SingleCommand *>::iterator it;

    it = std::find_if( m_cmds.begin(), m_cmds.end(), FindCmdObj(szCommand) );

    if ( it == m_cmds.end() )
    {
        return 0;
    }
    else
    {
        return *it;
    }
}

BOOL Commands::Add( SingleCommand * cmd )
{
    SingleCommand * pCmd = FindCommand( cmd->GetCommand() );

    if ( 0 != pCmd )
    {
        return FALSE;
    }

    m_cmds.push_back( cmd );

    return TRUE;
}

bool CompareCmd( SingleCommand * p1, SingleCommand * p2 )
{
    int nRet = strcmp( p1->GetCommand(), p2->GetCommand() );

    return nRet > 0 ? false : true;
}

void Commands::Sort()
{
    std::sort( m_cmds.begin(), m_cmds.end(), CompareCmd );
}

BOOL IsWhiteChar( char ch )
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

void Commands::ParseRawCommand( const char * szCommand, 
                                std::vector<char *> & vTokens )
{
    unsigned int nLen = strlen( szCommand );

    unsigned int   i = 0;
    BOOL bTokenStart = FALSE;
    BOOL bInQuote    = FALSE;

    unsigned int nTokenLen   = 0;

    char * pToken = new char[nLen + 1];

    while( i < nLen )
    {
        char ch = szCommand[i];

        if ( bTokenStart )    
        {
            if ( bInQuote )
            {
                if ( '"' == ch )
                {
                    if ( i + 1 < nLen )
                    {
                        if ( IsWhiteChar(szCommand[i+1]) )
                        {
                            bInQuote    = FALSE;
                            bTokenStart = FALSE;
                            pToken[nTokenLen] = '\0';

                            //存储起来
                            char * tmp = new char[nTokenLen + 1];
                            strcpy(tmp, pToken);
                            vTokens.push_back(tmp);
                            nTokenLen = 0;

                            i += 2;
                        }
                        else
                        {
                            if ( '"' == szCommand[i+1] )
                            {
                                pToken[nTokenLen] = '\0';

                                //存储起来
                                char * tmp = new char[nTokenLen + 1];
                                strcpy(tmp, pToken);
                                vTokens.push_back(tmp);
                                nTokenLen = 0;

                                bInQuote    = TRUE;
                                bTokenStart = TRUE;

                                i += 2;
                            }
                            else
                            {
                                pToken[nTokenLen] = szCommand[i+1];
                                nTokenLen++;

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
                    if ( i + 1 < nLen )
                    {
                        pToken[nTokenLen] = szCommand[i+1];
                        nTokenLen++;
                        
                        i += 2;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    pToken[nTokenLen] = ch;
                    nTokenLen++;

                    i++;
                }
            }
            else
            {
                if ( IsWhiteChar(ch) )
                {
                    bTokenStart = FALSE;
                    pToken[nTokenLen] = '\0';

                    //存储起来
                    char * tmp = new char[nTokenLen + 1];
                    strcpy(tmp, pToken);
                    vTokens.push_back(tmp);
                    nTokenLen = 0;

                    i++;
                }
                else
                {
                    pToken[nTokenLen] = ch;
                    nTokenLen++;

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
                    nTokenLen = 0;

                    i++;
                }
                else
                {
                    bTokenStart = TRUE;

                    pToken[0] = ch;
                    nTokenLen = 1;

                    i++;
                }
            }
        }
    }

    if ( bTokenStart )
    {
        pToken[nTokenLen] = '\0';

        char * tmp = new char[nTokenLen + 1];
        strcpy(tmp, pToken);
        vTokens.push_back(tmp);
        nTokenLen = 0;
    }

    delete[] pToken;
}

static void ClearStringVector( std::vector<char*> & v )
{
    std::vector<char *>::iterator it;
    for ( it = v.begin(); it != v.end(); it++ )
    {
        delete[] *it;
    }
    v.clear();
}

void Commands::ExecuteRawCommand( const char * szRawCmd )
{
    assert( 0 != szRawCmd );


    std::vector<char *> vTokens;
    
    ParseRawCommand( szRawCmd, vTokens );
    if ( 0 == vTokens.size() )
    {
        return;
    }

    char * pCmd = vTokens.at( 0 );
    strtrim( pCmd );

    //空命令
    if ( '\0' == pCmd[0] )
    {
        ClearStringVector(vTokens);
        return;
    }

    char buff[BUFF_SIZE];

    SingleCommand * pCmdObj = FindCommand( pCmd );
    //如果没有找到命令
    if ( 0 == pCmdObj )
    {
#ifdef WIN32
        _snprintf( buff, BUFF_SIZE-1, COMMAND_NOT_FOUND_MSG, pCmd );
#else
        snprintf( buff, BUFF_SIZE-1, COMMAND_NOT_FOUND_MSG, pCmd );
#endif
        buff[BUFF_SIZE-1] = '\0' ;
        send( s_tClientSock, buff, strlen(buff), 0 );

        ClearStringVector(vTokens);
        return;
    }

    BOOL bFindHelp = FALSE;
    unsigned int i = 0;
    for ( i = 1; i < vTokens.size(); i++ )
    {
        char * pToken = vTokens.at( i );
        if ( 0 == strcmp(pToken, "--help") )
        {
            bFindHelp = TRUE;
            break;
        }
    }

    //打印帮助
    if ( bFindHelp )
    {
        pCmdObj->PrintHelp();

        ClearStringVector(vTokens);
        return;
    }

    
    std::vector<int> vArgTypes;
    pCmdObj->GetArgTypes( vArgTypes );

    //参数不够
    if ( vTokens.size() < vArgTypes.size() + 1 )
    {
        send( s_tClientSock, ARGS_NOT_ENOUGH_MSG, strlen(ARGS_NOT_ENOUGH_MSG), 0 );

        ClearStringVector(vTokens);
        return;
    }

    std::vector<int>::iterator it;
    void ** ppArgs = 0;

    if ( vArgTypes.size() > 0 )
    {
        ppArgs = new void*[vArgTypes.size()];
    }

    for ( it = vArgTypes.begin(), i = 0; it != vArgTypes.end(); it++, i++ )
    {
        int nType = *it;
        char * pToken = vTokens.at( i + 1 );

        if ( JTEL_ARG_TYPE_INT == nType )
        {
            int nNumber = 0;

            int nRet = Str2Num( pToken, &nNumber );
            //无效的整数
            if ( nRet <= 0 )
            {
                sprintf( buff, ARG_TYPE_WRONG, i + 1 );
                send( s_tClientSock, buff, strlen(buff), 0 );
                
                ClearStringVector(vTokens);
                return;
            }
            else
            {
                //保存
                ppArgs[i] = (void *)nNumber;
            }
        }
        else
        {
            ppArgs[i] = (void *)pToken;
        }
    }
    
    pCmdObj->Execute( vArgTypes.size(), ppArgs );
    
    if ( 0 != ppArgs )
    {
        delete[] ppArgs;
        ppArgs = 0;
    }

    ClearStringVector(vTokens);
}

// 0-18: print command
// 19-20: ' ', ' '
// 21-80: help
char * Commands::GetHelpItem( char * buff, unsigned int nSize,
                              const char * pCmd, const char * pHelp)
{
    const unsigned int nCmdLimit = 18;
    const unsigned int nHelpLimit = COLUMNS_NUMBER - nCmdLimit - 2;

    assert( pCmd );
    unsigned int nCmdLen = strlen( pCmd );
    assert( nCmdLen > 0 );


    unsigned int nCmdLines  = ( nCmdLen - 1 ) / nCmdLimit + 1;

    unsigned int nHelpLen   = 0;
    unsigned int nHelpLines = 1;
    if ( pHelp )
    {
        nHelpLen = strlen( pHelp );
        if ( nHelpLen > 0 )
        {
            nHelpLines = ( nHelpLen - 1 ) / nHelpLimit + 1;
        }
    }

    unsigned int nMaxLines = ( nCmdLines <= nHelpLines ? nHelpLines : nCmdLines );

    unsigned int i;
    for ( i = 0; i < nMaxLines; i++ )
    {
        char * p = buff + COLUMNS_NUMBER * i;

        if ( nCmdLen >= nCmdLimit )
        {
            memcpy( p, pCmd, nCmdLimit );
            
            pCmd += nCmdLimit;
            nCmdLen -= nCmdLimit;
        }
        else
        {
            if ( nCmdLen > 0 )
            {
                memcpy( p, pCmd, nCmdLen );
                memset( p + nCmdLen, ' ', nCmdLimit - nCmdLen );

                pCmd += nCmdLen;
                nCmdLen = 0;
            }
            else
            {
                memset( p, ' ', nCmdLimit);
            }
        }

        p[nCmdLimit] = ' ';
        p[nCmdLimit+1] = ' ';

        char * q = p + nCmdLimit + 2;

        if ( pHelp )
        {
            if ( nHelpLen >= nHelpLimit )
            {
                memcpy( q, pHelp, nHelpLimit );

                pHelp += nHelpLimit;
                nHelpLen -= nHelpLimit;
            }
            else
            {
                if ( nHelpLen > 0 )
                {
                    memcpy( q, pHelp, nHelpLen );
                    memset( q + nHelpLen, ' ', nHelpLimit - nHelpLen );

                    pHelp += nHelpLen;
                    nHelpLen = 0;
                }
                else
                {
                    memset( q, ' ', nHelpLimit);
                } 
            }
        }
        else
        {
            memset( q, ' ',  nHelpLimit );
        }
    }

    memcpy( buff + COLUMNS_NUMBER * nMaxLines, "\r\n\0", 3 );

    return buff;
}

void Commands::PrintAllHelpMsg()
{
    char buff[8092];
    char * p = buff;
    p[0] = '\0';

    std::vector<SingleCommand *>::iterator it;
    for ( it = m_cmds.begin(); it != m_cmds.end(); it++ )
    {
        SingleCommand * pCmdObj = *it;

        const char * pCmd  = pCmdObj->GetCommand();
        const char * pHelp = pCmdObj->GetHelp();

        char tmp[8092];
        GetHelpItem( tmp, 8092, pCmd, pHelp );
        strcat( buff, tmp );
    }

    send( s_tClientSock, buff, strlen(buff), 0 );

}

char * Commands::FindAlikeCmd( char * szAlikeCmd, unsigned int nSize, 
                               const char * szRawCmd, unsigned int nPos )
{
    assert( szAlikeCmd && nSize > 0 && szRawCmd );

    unsigned int nRawLen = strlen( szRawCmd );
    assert( nPos <= nRawLen );

    if ( ' ' != szRawCmd[nPos] && '\0' != szRawCmd[nPos] )
    {
        return 0;
    }

    char * tmp = new char[nPos + 1];
    memcpy( tmp, szRawCmd, nPos );
    tmp[nPos] = '\0';

    std::vector<char *> vTokens;
    ParseRawCommand( tmp, vTokens );
    delete[] tmp;

    //全是空白字符
    if ( 0 == vTokens.size() )
    {
        return 0;
    }

    //前面至少有2个token
    if ( vTokens.size() > 1 )
    {
        ClearStringVector(vTokens);
        return 0;
    }

    BOOL bInQuote = FALSE;
    char * pCmd = vTokens.at(0);
    strtrim( pCmd );

    unsigned int nCmdLen = strlen( pCmd );
    assert ( nPos >= nCmdLen );
    
    if ( nPos > nCmdLen )
    {
        if ( ' ' == szRawCmd[nPos-nCmdLen-1] )
        {
            bInQuote = FALSE;
        }
        else if ( '"' == szRawCmd[nPos-nCmdLen-1] )
        {
            bInQuote = TRUE;
        }
        else
        {
            assert( 0 );
        }
    }

    std::vector<SingleCommand *> vCmds;
    std::copy( m_cmds.begin(), m_cmds.end(), std::back_inserter(vCmds) );

    tmp = new char[nSize + 1];
    char * pRet = CompleteCommand( tmp, nSize + 1, pCmd, vCmds );
    if ( 0 == pRet )
    {
        delete[] tmp;
        ClearStringVector(vTokens);
        return 0;
    }
    
    if ( strlen(pRet) == nCmdLen )
    {
        delete[] tmp;
        ClearStringVector(vTokens);
        return 0;
    }

    unsigned int nLen = 0;
    memcpy( szAlikeCmd, szRawCmd, nPos );
    nLen = nPos;

    strcpy( szAlikeCmd + nLen, tmp + nCmdLen );
    nLen += strlen( tmp + nCmdLen );

    if ( bInQuote )
    {
        memcpy( szAlikeCmd + nLen, "\"", 1);
        nLen++;
    }

    strcpy( szAlikeCmd + nLen, szRawCmd + nPos );
    nLen += strlen( szRawCmd + nPos );

    delete[] tmp;
    ClearStringVector(vTokens);
    return szAlikeCmd;
}

char * Commands::CompleteCommand( char * szCompleteCmd, unsigned int nSize, 
                                  const char * szCmd,
                                  std::vector<SingleCommand *> & vCmds )
{
    unsigned int nCmdLen = strlen( szCmd );

    std::vector<SingleCommand *> vNew;

    std::vector<SingleCommand *>::iterator it;
    const char * pPrev = 0;

    for ( it = vCmds.begin(); it != vCmds.end(); it++ )
    {
        SingleCommand * pCmdObj = *it;
        
        const char * pItemCmd     = pCmdObj->GetCommand();
        unsigned int nItemCmdLen  = strlen( pItemCmd );
        
        int nRet = strcmp( szCmd, pItemCmd );

        if ( nRet > 0 )
        {

        }
        else if ( 0 == nRet )
        {
            strcpy( szCompleteCmd, szCmd );
            return szCompleteCmd;
        }
        else
        {
            if ( nItemCmdLen <= nCmdLen )
            {
                break;
            }
            else
            {
                if ( 0 != memcmp( szCmd, pItemCmd, nCmdLen ) )
                {
                    break;
                }

                if ( 0 == pPrev )
                {
                    pPrev = pItemCmd;

                    vNew.push_back( pCmdObj );
                }
                else
                {
                    if ( pPrev[nCmdLen] != pItemCmd[nCmdLen] )
                    {
                        strcpy( szCompleteCmd, szCmd );
                        return szCompleteCmd;
                    }
                    else
                    {
                        vNew.push_back( pCmdObj );
                        pPrev = pItemCmd;
                    }
                }
            }
        }
    }

    if ( vNew.size() > 0 )
    {
        if ( vNew.size() > 1 )
        {
            SingleCommand * pObj = vNew.at( 0 );

            char * tmp = new char[nCmdLen + 2];
            memcpy( tmp, pObj->GetCommand(), nCmdLen + 1 );
            tmp[nCmdLen + 1] = '\0';

            char * pRet = CompleteCommand( szCompleteCmd, nSize, tmp, vNew );
            delete[] tmp;

            return pRet;
        }
        else
        {
            SingleCommand * pObj = vNew.at( 0 );
            strcpy( szCompleteCmd, pObj->GetCommand() );
            return szCompleteCmd;
        }
    }
    else
    {
        return 0;
    }
}

static Commands s_cmds;

#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif

int CloseSocket( SOCKET s )
{
#ifdef WIN32
    //shutdown( s, SD_BOTH );
    return closesocket( s );
#else
    shutdown( s, SHUT_RDWR );
    return close( s );
#endif
}

void WaitForThread( HANDLE t )
{
#ifdef WIN32
    WaitForSingleObject( t, INFINITE );
#else
    pthread_join( t, 0 );
#endif
}

void _CreateThread( HANDLE * pHandle, void * pfun, void * pParam )
{
#ifdef WIN32
    assert( pHandle );
    *pHandle = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)pfun, pParam, 0, 0 );
#else
    pthread_create( pHandle, 0, (LPTHREAD_START_ROUTINE)pfun, pParam );
#endif

}

static BYTE * WriteByte2Buff( BYTE * buff, unsigned int nSize, unsigned int nCount, ...)
{
    assert( buff );
    assert( nCount <= nSize );

    va_list vl;

    va_start( vl, nCount );     /* Initialize variable arguments. */

    unsigned int i;
    for ( i = 0; i < nCount; i++ )
    {
        buff[i] = (BYTE)va_arg( vl, int );
    }

    va_end( vl );

    return buff;
}

static unsigned int FilterIac( BYTE * pchCmd, unsigned int nSize, BYTE * pchData, 
                  unsigned int nLen )
{
    unsigned int i = 0;
    unsigned int j = 0;

    while( i < nLen )
    {
        if ( IAC == pchData[i] )
        {
            i++;
            if ( SB == pchData[i] )
            {
                i++;
                while( !(pchData[i-1] != IAC && pchData[i] == SE) && (i < nLen) )
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
            if ( j < nSize )
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

static int SendPrompt( SOCKET socket )
{
    return send( socket, PROMPT, strlen(PROMPT), 0 );
}

static int SendLeftCmd( SOCKET socket )
{
    return send( socket, CURSOR_LEFT, 1, 0 );
}

static int SendRightCmd( SOCKET socket )
{
    return send( socket, RIGHT_SEND, 3, 0 );
}

static char s_cmd[BUFF_SIZE];
static unsigned int s_nPos = 0;
static unsigned int s_nCmdLen = 0;

#ifdef WIN32 
static DWORD WINAPI ClientThread( LPVOID lpParam )
#else
static void * ClientThread( void * lpParam )
#endif
{
    SOCKET socket = (SOCKET)lpParam;

    char buff[BUFF_SIZE];
    WriteByte2Buff( (BYTE *)buff, BUFF_SIZE, 
        18, IAC, DO,  0x01,  IAC, DO,   0x03, IAC, DO, 0x21, IAC, DO, 0x22,
            IAC, WILL, 0x01, IAC, WILL, 0x03 );

    int nRet = send( socket, buff, 18, 0 );
    if ( SOCKET_ERROR == nRet )
    {
        return THREAD_ERROR;
    }

    nRet = send( socket, TELNET_HEAD, strlen(TELNET_HEAD), 0 );
    if ( SOCKET_ERROR == nRet )
    {
        return THREAD_ERROR;
    }

    if ( SOCKET_ERROR == SendPrompt( socket ) )
    {
        return THREAD_ERROR;
    }

    char * cmd = s_cmd;
    unsigned int & nPos = s_nPos;
    unsigned int & nCmdLen = s_nCmdLen;

    while ( TRUE )
    {
        nRet = recv( socket, buff, BUFF_SIZE, 0 );
        if ( SOCKET_ERROR == nRet )
        {
            break;
        }

        if ( 0 == nRet )
        {
            break;
        }

        char tmp[BUFF_SIZE];
        unsigned int nDataLen = 0;

#if DEBUG_SWITCH
        DebugStream( tmp, BUFF_SIZE, buff, nRet );
        printf("# receive stream[%d]: \r\n%s", nRet, tmp);
#endif

        nDataLen = FilterIac( (BYTE *)tmp, BUFF_SIZE, (BYTE *)buff, nRet );

        if ( nDataLen > 0 )
        {
            CJFuncLock cLock( s_lock );

            if ( 2 < nDataLen 
                && 0 == memcmp( "\r", tmp + nDataLen - 2, 1 ) )
            {
                //do nothing
            }
            //删除操作
            else if ( 
                  ( DELETE_SEND1_LEN <= nDataLen 
                   && 0 == memcmp( DELETE_SEND1, tmp, DELETE_SEND1_LEN ) )
                ||
                  ( DELETE_SEND2_LEN <= nDataLen 
                  && 0 == memcmp( DELETE_SEND2, tmp, DELETE_SEND2_LEN ) )
                ||
                  ( BACKSPACE_SEND1_LEN <= nDataLen 
                  && 0 == memcmp( BACKSPACE_SEND1, tmp, BACKSPACE_SEND1_LEN ) )
                ||
                  ( BACKSPACE_SEND2_LEN <= nDataLen 
                  && 0 == memcmp( BACKSPACE_SEND2, tmp, BACKSPACE_SEND2_LEN ) )
                )
            {
                if ( nPos > 0 )
                {
                    unsigned int nReservedLen = nCmdLen - nPos;

                    buff[0] = '\b';

                    unsigned int nLen = 1;
                    memcpy( buff + nLen, cmd + nPos, nReservedLen );

                    memcpy( cmd + nPos - 1, buff + nLen, nReservedLen );
                    nCmdLen--;
                    nPos--;

                    nLen += nReservedLen;

                    memcpy( buff + nLen, "\033[K", 3 );
                    nLen += 3;

                    unsigned int i;
                    for ( i = 0; i < nReservedLen; i++ )
                    {
                        buff[nLen + i] = '\b';
                    }

                    nLen += nReservedLen;

                    send( socket, buff, nLen, 0 );
                }
            }
            else if ( 3 <= nDataLen 
                && 0 == memcmp( UP_SEND, tmp, 3 ) )
            {
                const char * pPrevCmd = s_history.GetPrevCmd();
                if ( 0 != pPrevCmd )
                {
                    unsigned int nPrevLen = strlen( pPrevCmd );
                    unsigned int nCurLen  = nPos;
                    cmd[nCmdLen] = '\0';
                    
                    unsigned int i;
                    for ( i = 0; i < nCurLen; i++ )
                    {
                        buff[i] = '\b';
                    }

                    memcpy( buff + nCurLen, pPrevCmd, nPrevLen );
                    unsigned int nLen = nCurLen + nPrevLen;

                    if ( nPrevLen < nCmdLen )
                    {
                        memcpy( buff + nLen, CLEAR_SEND, strlen(CLEAR_SEND) );
                        nLen += strlen( CLEAR_SEND );
                    }
                    
                    memcpy( cmd, pPrevCmd, nPrevLen );
                    nCmdLen = nPrevLen;
                    nPos    = nCmdLen;

                    send( socket, buff, nLen, 0 );
                }
            }
            else if ( 3 <= nDataLen 
                && 0 == memcmp( DOWN_SEND, tmp, 3 ) )
            {
                const char * pNextCmd = s_history.GetNextCmd();
                if ( 0 != pNextCmd )
                {
                    unsigned int nNextLen = strlen( pNextCmd );
                    unsigned int nCurLen  = nPos;
                    cmd[nCmdLen] = '\0';
                    
                    unsigned int i;
                    for ( i = 0; i < nCurLen; i++ )
                    {
                        buff[i] = '\b';
                    }

                    memcpy( buff + nCurLen, pNextCmd, nNextLen );
                    unsigned int nLen = nCurLen + nNextLen;

                    if ( nNextLen < nCmdLen )
                    {
                        memcpy( buff + nLen, CLEAR_SEND, strlen(CLEAR_SEND) );
                        nLen += strlen( CLEAR_SEND );
                    }
                    
                    memcpy( cmd, pNextCmd, nNextLen );
                    nCmdLen = nNextLen;
                    nPos    = nCmdLen;

                    send( socket, buff, nLen, 0 );
                }
            }
            else if ( 3 <= nDataLen 
                && 0 == memcmp( LEFT_SEND, tmp, 3 ) )
            {
                if ( nPos > 0 )
                {
                    nPos--;

                    SendLeftCmd( socket );
                }
            }
            else if ( 3 <= nDataLen 
                && 0 == memcmp( RIGHT_SEND, tmp, 3 ) )
            {
                if ( nPos < nCmdLen )
                {
                    nPos++;

                    SendRightCmd( socket );
                }
            }
            else if ( 1 <= nDataLen 
                && 0 == memcmp( "\t", tmp, 1 ) )
            {
                char szAlikeCmd[BUFF_SIZE];
                cmd[nCmdLen] = '\0';
                char * pRet = s_cmds.FindAlikeCmd( szAlikeCmd, BUFF_SIZE, cmd, nPos );
                if ( 0 != pRet )
                {
                    unsigned int nLatterLen = strlen( cmd + nPos );

                    strcpy( buff, szAlikeCmd + nPos );
                    unsigned int nLen = strlen( szAlikeCmd + nPos );

                    unsigned int i;
                    for ( i = 0; i < nLatterLen; i++ )
                    {
                        buff[nLen + i] = '\b';
                    }

                    nLen += nLatterLen;

                    nPos += strlen( szAlikeCmd ) - strlen( cmd );
                    strcpy( cmd, szAlikeCmd );
                    nCmdLen = strlen( cmd );

                    send( socket, buff, nLen, 0 );
                }
            }
            //命令结束
            else if ( 2 == nDataLen
                && 0 == memcmp( "\r", tmp, 1) )
            {   
                cmd[nCmdLen] = '\0';

                strcpy( buff, cmd );
                strtrim( buff );

                if ( '\0' != buff[0] )
                {
                    s_history.Pushback( cmd );
                }
                s_history.ResetCurIndex();

                send( socket, "\r\n", 2, 0 );
#if DEBUG_SWITCH
                strcpy( buff, "input command is: \r\n");
                DebugStream( tmp, BUFF_SIZE, cmd, nCmdLen );
                strcat( buff, tmp );
                send( socket, buff, strlen(buff), 0 );
#else
                s_cmds.ExecuteRawCommand( cmd );
#endif

                nCmdLen = 0;
                nPos    = 0;

                if ( SOCKET_ERROR == SendPrompt( socket ) )
                {
                    return THREAD_ERROR;
                }
            }
            else
            {
                assert( nPos <= nCmdLen );

                if ( nPos == nCmdLen )
                {
                    memcpy( cmd + nPos, tmp, nDataLen );
                    nPos += nDataLen;
                    nCmdLen += nDataLen;
                    
                    send( socket, tmp, nDataLen, 0 );
                }
                else
                {
                    unsigned int nReservedLen = nCmdLen - nPos;
                    
                    memcpy( buff, cmd + nPos, nReservedLen );
                    memcpy( cmd + nPos, tmp, nDataLen );
                    memcpy( cmd + nPos + nDataLen, buff, nReservedLen );

                    nCmdLen += nDataLen;
                    
                    unsigned int nLen = nCmdLen - nPos;
                    memcpy( buff, cmd + nPos, nLen );

                    unsigned int i;
                    for ( i = 0; i < nReservedLen; i++ )
                    {
                        memcpy( buff + nLen + strlen(CURSOR_LEFT) * i, 
                            CURSOR_LEFT, strlen(CURSOR_LEFT) );
                    }

                    nPos += nDataLen;

                    send( socket, buff, nLen + strlen(CURSOR_LEFT) * nReservedLen, 0 );
                }
            }
        }
    }

    return 0;
}

#ifdef WIN32 
static DWORD WINAPI StartThread( LPVOID lpParam )
#else
static void * StartThread( void * lpParam )
#endif
{
    SOCKET tSvrSocket = (SOCKET)lpParam;

    // 我们需要一些变量来保存客户端的套接字，因此我们在此声明之。
    SOCKET      tClientSock;
    sockaddr_in tFromAddr;

#ifdef WIN32
    int nFromlen;
#else
    socklen_t nFromlen;
#endif

    nFromlen = sizeof(tFromAddr);

    while( TRUE ) // 无限循环
    {
        // accept()将会接收即将到来的客户端连接。
        tClientSock = accept( tSvrSocket, 
            (struct sockaddr*)&tFromAddr, &nFromlen );

        if ( INVALID_SOCKET == tClientSock )
        {
            break;
        }

        if ( INVALID_SOCKET != s_tClientSock )
        {
            CloseSocket( s_tClientSock );
            s_tClientSock = INVALID_SOCKET;
        }

        if ( 0 != s_hClientThrd )
        {
            printf( "wait for thread! \n" );
            WaitForThread( s_hClientThrd );
            printf( "ok! \n" );
#ifdef WIN32
            CloseHandle( s_hClientThrd );
#endif
            s_hClientThrd = 0;
        }

        s_tClientSock = tClientSock;
        _CreateThread( &s_hClientThrd, (LPVOID)ClientThread, (LPVOID)tClientSock );

    }

    return 0;
}

static int help(int nCnt, void * args[])
{
    if ( INVALID_SOCKET == s_tClientSock )
    {
        return 1;
    }

    s_cmds.PrintAllHelpMsg();

    return 0;
}

static int bye(int nCnt, void * args[])
{
    if ( INVALID_SOCKET != s_tClientSock )
    {
        CloseSocket( s_tClientSock );
        s_tClientSock = INVALID_SOCKET;
    }

    return 0;
}

int JTelSvrStart( unsigned short swPort )
{

#ifdef WIN32
    WSADATA tWsaData;
    int nRet = WSAStartup( 0x202, &tWsaData );
    if( 0 != nRet )
    {
        return JTEL_ERROR_FAILED_TO_WSASTARTUP;
    }
#endif

    //如果已经启动
    if ( 0 != s_hStartThrd )
    {
        return JTEL_ERROR_ALREADY_STARTED;
    }

    JTelSvrRegCommand( "help", "Show all help messages.Type --help after command for help of this command.", help, 0 );
    JTelSvrRegCommand( "bye",  "Quit telnet processing.", bye,  0 );

    s_cmds.Sort();

    sockaddr_in tSockArr;

    // 现在我们来为sockaddr_in结构赋值。
    tSockArr.sin_family      = AF_INET;               // 地址族
    tSockArr.sin_addr.s_addr = INADDR_ANY;            // 网际IP地址
    tSockArr.sin_port        = htons( swPort );       // 使用的端口

    // 由socket函数创建我们的SOCKET。
    s_tSvrSocket = socket( AF_INET,SOCK_STREAM, 0 );

    // 如果socket()函数失败，我们就退出。
    if( INVALID_SOCKET == s_tSvrSocket )
    {
        return JTEL_ERROR_FAILED_TO_SOCKET;
    }

    // bind将我们刚创建的套接字和sockaddr_in结构联系起来。
    // 它主要使用本地地址及一个特定的端口来连接套接字。
    // 如果它返回非零值，就表示出现错误。
    if( 0 != bind( s_tSvrSocket, (sockaddr*)&tSockArr, sizeof(tSockArr)) )
    {
        return JTEL_ERROR_FAILED_TO_BIND;
    }

    // listen命令套接字监听来自客户端的连接。
    // 第二个参数是最大连接数。
    if( 0 != listen( s_tSvrSocket,10 ) )
    {
        return JTEL_ERROR_FAILED_TO_LISTEN;
    }

    JInitLock( &s_lock );

    _CreateThread( &s_hStartThrd, (LPVOID)StartThread, (LPVOID)s_tSvrSocket );

 
    return 0;
}



/**************************************************************/
// 功能: 关闭telnet服务
/**************************************************************/
int JTelSvrStop ( )
{
    // 如果没有启动
    if ( 0 == s_hStartThrd )
    {
        return JTEL_ERROR_NOT_STARTED;
    }

    assert( INVALID_SOCKET != s_tSvrSocket );

    // 关闭接收客户端连接的socket 
    CloseSocket( s_tSvrSocket );
    s_tSvrSocket = INVALID_SOCKET;

    // 等待ACCEPT线程结束
    WaitForThread( s_hStartThrd );
#ifdef WIN32
    // 回收句柄资源
    CloseHandle( s_hStartThrd );
#endif
    s_hStartThrd = 0;

    // 如果有客户端接入
    if ( INVALID_SOCKET != s_tClientSock )
    {
        // 关闭客户端socket
        CloseSocket( s_tClientSock );
        s_tClientSock = INVALID_SOCKET;
    }

    if ( 0 != s_hClientThrd )
    {
        WaitForThread( s_hClientThrd );
#ifdef WIN32
        CloseHandle( s_hClientThrd );
#endif
        s_hClientThrd = 0;
    }

    JDeinitLock( &s_lock );

    s_cmds.Clear();
    
    return 0;
}

int JTelSvrRegCommand( const char * szCmd, const char * szHelpMsg, 
                       LPREG_CMD_FUNC pfunc, ... )
{
    // 如果已经启动
    if ( 0 != s_hStartThrd )
    {
        return JTEL_ERROR_ALREADY_STARTED;
    }
    
    va_list ap;
    va_start( ap, pfunc );

    SingleCommand * pCmdObj = new SingleCommand( szCmd, szHelpMsg, pfunc, ap );
    if ( !s_cmds.Add( pCmdObj ) )
    {
        delete pCmdObj;
    }

    va_end( ap );
    return 0;
}

int JTelSvrPrint( const char * szFormat, ... )
{
    if ( INVALID_SOCKET == s_tClientSock )
    {
        return 1;
    }

    va_list ap;
    char buff[BUFF_SIZE];

    va_start( ap, szFormat );
    vsprintf( buff, szFormat, ap );
    va_end( ap );

    strcat( buff, "\r\n" );

    send( s_tClientSock, buff, strlen(buff), 0 );

    if ( 0 == s_nPrintFlag )
    {
        if ( s_nCmdLen > 0 )
        {
            SendPrompt( s_tClientSock );

            JLock( &s_lock );

            //s_nPos = s_nCmdLen;
            s_cmd[s_nCmdLen] = '\0';
            send( s_tClientSock, s_cmd, strlen(s_cmd), 0 );
                       
            assert( s_nPos <= s_nCmdLen );

            int n = s_nCmdLen - s_nPos;
            if ( n > 0 )
            {
                char tmp[BUFF_SIZE];
                int i = 0;
                for ( i = 0; i < n; i++ )
                {
                    tmp[i] = '\b';
                }
                send( s_tClientSock, tmp, n, 0 );
            }
            

            JUnlock( &s_lock );
        }
    }
    return 0;
}

